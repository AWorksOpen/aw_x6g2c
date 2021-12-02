/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief GPRS interface implementation
 *
 * \internal
 * \par modification history:
 * - 1.00 2017.11.29  vih, first implementation
 * - 2.00 2020.11.30  vih, refactor
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "driver/gprs/gb2312_unicode_table.h"
#include "awbl_gprs.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/opt.h"
#include "lwip/netifapi.h"

#include "string.h"

#include "aw_ping.h"
#include "aw_time.h"
#include "aw_gprs.h"

/*****************************************************************************/
#define PFX  "GPRS"
#define __COLOR_RED        "\x1B[31m"
#define __COLOR_GREEN      "\x1B[32m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if (p_this->pfn_log_out) {\
         p_this->pfn_log_out(mask, color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, __COLOR_GREEN, "[%s:%4d] [OK] " fmt "\n", \
                       PFX, __LINE__, ##__VA_ARGS__)\
    } else {\
        __LOG_BASE(AW_GPRS_LOG_ERROR, __COLOR_RED, "[%s:%4d] [ER] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)\
    }

#define __ERRCODE(err)  (abs(err) >= AW_GPRS_EBASE ? \
           g_gprs_errcode_str[__EOFF(abs(err))] : g_gprs_errcode_str[abs(err)])

#define __EXP_EMSG(exp, err, fmt, ...)  \
    {\
        if (__ERRCODE(err) != NULL) {\
            __LOG_BASE(AW_GPRS_LOG_ERROR, __COLOR_RED,\
                    "[%s:%4d] %s", PFX, __LINE__, __ERRCODE(err));\
            if (fmt) \
                __LOG_BASE(AW_GPRS_LOG_ERROR, __COLOR_RED, fmt, ##__VA_ARGS__);\
            __LOG_BASE(1, __COLOR_RED, "\n");\
        }                                   \
        exp;                         \
    }

#define __GOTO_EMSG(where, err, fmt, ...)  \
    __EXP_EMSG(ret = err; goto where, err, fmt, ##__VA_ARGS__)

#define __RETURN_EMSG(err, fmt, ...)  \
    __EXP_EMSG(return err, err, fmt, ##__VA_ARGS__)
/******************************************************************************/
#ifdef __VIH_DEBUG
#define __DEBUG_SECTION(cond) if (cond)
#else
#define __DEBUG_SECTION(cond) if (0)
#endif
/******************************************************************************/
#define __TIMEOUT_SEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                              aw_ms_to_ticks((timeout)*1000); \
                                              aw_sys_tick_get() < tick_tmp; )

#define __TIMEOUT_MSEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                               aw_ms_to_ticks(timeout); \
                                               aw_sys_tick_get() < tick_tmp; )

/******************************************************************************/
aw_local aw_err_t __gprs_ppp_close (struct awbl_gprs_dev *p_this);
aw_local aw_err_t __gprs_ppp_open (struct awbl_gprs_dev *p_this);
aw_local aw_err_t __gprs_status_check (struct awbl_gprs_dev *p_this);
static aw_err_t __reset (struct awbl_gprs_dev   *p_this,
                         aw_bool_t               hw_reset,
                         uint32_t                ms);
/******************************************************************************/
#define __EOFF(err) ((err) - AW_GPRS_EBASE + AW_EREMOTEIO)

char *g_gprs_errcode_str[] = {
    [AW_EINVAL]     = "Parameter invalid",
    [AW_EBUSY]      = "Device busy",
    [AW_ENOENT]     = NULL,
    [AW_ENOMEM]     = "Failed to allocate memory",
    [AW_ENOTEMPTY]  = "Insufficient array space",
    [AW_ENOTSUP]    = "Not support",
    [AW_ENXIO]      = "The operation was rejected",
    [AW_EIO]        = "Device IO operation failed",
    [AW_EFAULT]     = "Not found the device",
    [AW_EEXIST]     = "Repetitive operation",
    [AW_EACCES]     = "The device has not completed initialization",
    [AW_ENOSPC]     = "The necessary driver callback are not set",
    [AW_ENOEXEC]    = "Unknown command",

    [AW_EBADF]      = "Error code received for at reply",
    [AW_E2BIG]      = NULL,

    [AW_EAGAIN]     = NULL,
    [AW_EXDEV]      = "Not found the NETIF",
    [AW_EADDRNOTAVAIL]    = "NETIF address is not valid",
    [AW_ENODEV]     = NULL,
    [AW_EMSGSIZE]   = "SMS data is too long",
    [AW_EISCONN]    = "Socket that has been opened is not closed",

    [AW_ESPIPE]     = "AT command pipeline exception",
    [AW_EROFS ]     = "PPP pipeline exception",
    [AW_EMLINK]     = "ETH pipeline exception",
    [AW_EPIPE ]     = "GNSS pipeline exception",
    [AW_ETIME]      = "Receiving the AT relies has time out",
    [AW_ETIMEDOUT]  = "Timeout to establish a socket connection",
    [AW_EPROTOTYPE] = "Socket protocol type error",
    [AW_ENOBUFS]    = "Insufficient number of socket handles",

    [__EOFF(AW_GPRS_ECPPP)]     = "Failed to create a new PPPoS session",
    [__EOFF(AW_GPRS_EIPPP)]     = "Failed to initiate a PPP connection",
    [__EOFF(AW_GPRS_EEPPP)]     = "Failed to establish a PPP connection",
    [__EOFF(AW_GPRS_EGNSS)]     = "GNSS function is not enabled",
    [__EOFF(AW_GPRS_ECSQ)]      = "The signal strength is below the minimum",
    [__EOFF(AW_GPRS_EAPN)]      = "Failed to obtain APN",
    [__EOFF(AW_GPRS_ECREG)]     = "Network registration status is abnormal",
    [__EOFF(AW_GPRS_ECEREG)]     = "EPS network registration status is abnormal",
    [__EOFF(AW_GPRS_ECGREG)]     = "GPRS network registration status is abnormal",
    [__EOFF(AW_GPRS_ECGDCONT)]  = "Failed to set packet domain protocol",
    [__EOFF(AW_GPRS_ECGATT)]    = "Failed to attach to PS service",
    [__EOFF(AW_GPRS_EATD)]      = "Failed to dial-up",
    [__EOFF(AW_GPRS_EAT)]       = "AT test exception",
    [__EOFF(AW_GPRS_EPIN)]      = "MT is waiting for (U)SIM PIN to be given",
    [__EOFF(AW_GPRS_ESOPT)]     = "Failed to set socket option",
    [__EOFF(AW_GPRS_ESMSF)]     = "Failed to set sms format",
    [__EOFF(AW_GPRS_ESMSI)]     = "The AT interaction to send an sms failed",
    [__EOFF(AW_GPRS_ESMSM)]     = "Insufficient SMS array space",
    [__EOFF(AW_GPRS_ESMSC)]     = "Unsopported SMS encoding format",
    [__EOFF(AW_GPRS_EHTTPSC)]     = "Failed to configure HTTPS command context",
    [__EOFF(AW_GPRS_EHTTPSZERO)]     = "The url data size is zero",
    [__EOFF(AW_GPRS_EHTTPSCONN)]     = "Failed to receive 'CONNECT' reply",
    [__EOFF(AW_GPRS_ESMSRC)]     = "Receiving the SMS format is not support",
    [__EOFF(AW_GPRS_EUCS2UTF)]     = "Failed to convert UCS2 to UTF8 encoding",
    [__EOFF(AW_GPRS_ESMSGF)]       = "Failed to get the first SMS index",
    [__EOFF(AW_GPRS_ESMSD)]        = "Failed to delete a SMS by index",
    [__EOFF(AW_GPRS_ESMSPDU)]      = "SMS PDU data is incomplete",
    [__EOFF(AW_GPRS_EPDUUDHI)]     = "Not support the PDU with UDHI flag.",
    [__EOFF(AW_GPRS_ESMSRPDU)]     = "Failed to receive PDP SMS by AT command",
    [__EOFF(AW_GPRS_EGNSSI)]     = "Failed to initialize GNSS function",
    [__EOFF(AW_GPRS_ECSQR)]      = "Failed to query the signal strength",
    [__EOFF(AW_GPRS_EPINGTM)]    = "Ping time out",
    [__EOFF(AW_GPRS_ESMSSBUF)]   = "The sent SMS buffer is not available using"
                                    " the receive buffer",
    [__EOFF(AW_GPRS_ECFUN0)]     = "Failed to enable minimum functionality",
    [__EOFF(AW_GPRS_ECFUN1)]     = "Failed to enable full functionality",
    [__EOFF(AW_GPRS_ECFUNRST)]   = "Failed to reset module with +CFUN command",
    [__EOFF(AW_GPRS_EUSBH)]      = "Device enumeration failed",

    [__EOFF(AW_GPRS_EBASE)]     = "",
    [AW_EPERM]      = NULL,  /* "Unknown error!" */
};

/*****************************************************************************/
AWBL_METHOD_DEF(awbl_gprs_dev_get, "awbl_gprs_dev_get_name");

/** \brief pointer to first gprs service */
aw_local struct awbl_gprs_dev *__gp_this_dev_head = NULL;

/**
 * \brief allocate gprs service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __gprs_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_gprs_dev  = NULL;
    struct awbl_gprs_dev      *p_this_dev       = NULL;

    /* find handler */
    pfunc_gprs_dev = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_gprs_dev_get));

    /* call method handler to allocate GPRS service */
    if (pfunc_gprs_dev != NULL) {
        pfunc_gprs_dev(p_dev, &p_this_dev);

        /* found a gprs service, insert it to the service list */
        if (p_this_dev != NULL) {
            struct awbl_gprs_dev **pp_serv_cur = &__gp_this_dev_head;
            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }
            *pp_serv_cur = p_this_dev;
            p_this_dev->p_next = NULL;

            awbl_gprs_default_init(p_this_dev);
            awbl_gprs_default_drvopr_set(p_this_dev);
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate gprs services from instance tree
 */
aw_local aw_err_t __gprs_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every gprs device will be find */
    __gp_this_dev_head = NULL;

    awbl_dev_iterate(__gprs_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a device who accept the id
 */
aw_local struct awbl_gprs_dev * __gprs_id_to_dev (int id)
{
    struct awbl_gprs_dev *p_serv_cur = __gp_this_dev_head;

    while ((p_serv_cur != NULL)) {
        if (id == p_serv_cur->dev_id) {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }
    return NULL;
}

void awbl_gprs_init (void)
{
    /* allocate gprs services from instance tree */

    __gprs_serv_alloc();
}
/*****************************************************************************/
static void __4g_log_handler ( aw_gprs_log_t mask,
                               void         *p_color,
                               const char   *func_name,
                               int           line,
                               const char   *fmt, ...)
{
    return;
}
/*****************************************************************************/
/**
 * gprs 事件上报
 */
aw_local aw_err_t __evt_cb_array_notify (struct awbl_gprs_dev *p_this,
                                         aw_gprs_evt_t        *p_evt)
{
    int                  i;
    aw_gprs_evt_cb_t     pfn_evt_cb;
    aw_err_t             allow = 1;

    /* 保存最新的事件 */
    p_this->last_evt = *p_evt;

    switch (p_evt->type) {

        /* 上报 PWR_OFF_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
        case AW_GPRS_EVT_TYPE_PWR_OFF_PRE: {

            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_this->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    p_evt->u.pwr_off_pre.retval = AW_OK;
                    pfn_evt_cb(p_this->dev_id, p_evt);

                    /* 检查 retval 不为 OK，则不允许该操作 */
                    if (p_evt->u.pwr_off_pre.retval != AW_OK) {
                        allow = 0;
                    }
                }
            }
            if (!allow) {
                __RETURN_EMSG(-AW_EBUSY, NULL);
            }
        } break;

        /* 上报 RESET_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
        case AW_GPRS_EVT_TYPE_RESET_PRE: {
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_this->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    p_evt->u.reset_pre.retval = AW_OK;
                    pfn_evt_cb(p_this->dev_id, p_evt);

                    /* 检查 retval 不为 OK，则不允许该操作 */
                    if (p_evt->u.reset_pre.retval != AW_OK) {
                        allow = 0;
                    }
                }
            }
            if (!allow) {
                __RETURN_EMSG(-AW_EBUSY, NULL);
            }
        } break;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        case AW_GPRS_EVT_TYPE_ETH_LINK_UP: {
            p_evt->u.eth_link_up.ip      = p_this->ethernet.ethernetaddr.our_ipaddr;
            p_evt->u.eth_link_up.netmask = p_this->ethernet.ethernetaddr.netmask;
            p_evt->u.eth_link_up.gateway = p_this->ethernet.ethernetaddr.gateway;
            p_evt->u.eth_link_up.dns     = p_this->ethernet.ethernetaddr.dns1;
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_this->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    pfn_evt_cb(p_this->dev_id, p_evt);
                }
            }
        } break;
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */
        /*
         * 只上报事件，无其它处理
         */
        case AW_GPRS_EVT_TYPE_PWR_ON:
        case AW_GPRS_EVT_TYPE_PWR_OFF:
        case AW_GPRS_EVT_TYPE_RESET_COMPLETED:
        case AW_GPRS_EVT_TYPE_ETH_LINK_DOWN:
        case AW_GPRS_EVT_TYPE_PPP_LINK_UP:
        case AW_GPRS_EVT_TYPE_PPP_LINK_DOWN: {
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_this->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    pfn_evt_cb(p_this->dev_id, p_evt);
                }
            }
        } break;
        default:break;
    }

    return AW_OK;
}

/*****************************************************************************/
extern struct netif *netif_list;
aw_local struct netif *__gprs_netif_get (void)
{
    struct netif *netif = NULL;

    for(netif = netif_list; netif != NULL; netif = netif->next) {
        if (memcmp(netif->name, "pp", 2) == 0) {
            break;
        }
    }
    return netif;
}

#if AWBL_GPRS_USE_LWIP_1_4

/** \brief  PPP 链路状态回调函数   */
aw_local void __ppp_link_status_cb (void *ctx, int err_code, void *arg)
{
    struct awbl_gprs_dev *p_this = (struct awbl_gprs_dev *)ctx;
    struct awbl_gprs_dev *p_this = p_this;
    struct netif *netif = NULL;

    if (err_code == PPPERR_NONE) {

        netif = __gprs_netif_get();
        if (arg) {
            char _buf[32];
            char _buf1[32];

            p_this->ppp.p_pppaddr = (struct ppp_addrs *)arg;
#if LWIP_DNS_FILETER
            if (dns_server_filter(netif, &p_this->ppp.p_pppaddr->dns1)) {
                dns_setserver(0, &p_this->ppp.p_pppaddr->dns1);
            }
#else
            dns_setserver(0, &p_this->ppp.p_pppaddr->dns1);
#endif
            dns_setserver(1, &p_this->ppp.p_pppaddr->dns2);

            ipaddr_ntoa_r(&p_this->ppp.p_pppaddr->dns1, _buf, 32);
            ipaddr_ntoa_r(&p_this->ppp.p_pppaddr->dns2, _buf1, 32);
        }

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_RUNNING;

        if (netif) {
            netif_set_default(netif);
        }

    } else if (err_code == PPPERR_USER ||
               err_code == PPPERR_PROTOCOL) {
        AW_SEMB_GIVE(p_this->ppp.link_close);
    }
}
#endif /* AWBL_GPRS_USE_LWIP_1_4 */


#if AWBL_GPRS_USE_LWIP_2_X
/*
 * PPP status callback
 * ===================
 *
 * PPP status callback is called on PPP status change (up, down, …) from lwIP
 * core thread
 */

/* PPP status callback example */
static void __ppp_link_status_cb (ppp_pcb *pcb, int err_code, void *ctx)
{
    struct awbl_gprs_dev *p_this = (struct awbl_gprs_dev *)ctx;
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    (void)pppif;

    switch(err_code) {
        case PPPERR_NONE: {
#if LWIP_DNS
            const ip_addr_t *ns;

            (void)ns;
#endif /* LWIP_DNS */

#if !LWIP_ROUTE_TABLE
            netif_set_default(pppif);
#endif

            p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_RUNNING;
            __LOG_PFX(AW_GPRS_LOG_PPP, "__ppp_link_status_cb: Connected");

#if PPP_IPV4_SUPPORT
            __LOG_PFX(AW_GPRS_LOG_PPP, "   our_ipaddr  = %s",
                      ipaddr_ntoa(&pppif->ip_addr));
            __LOG_PFX(AW_GPRS_LOG_PPP, "   his_ipaddr  = %s",
                      ipaddr_ntoa(&pppif->gw));
            __LOG_PFX(AW_GPRS_LOG_PPP, "   netmask     = %s",
                      ipaddr_ntoa(&pppif->netmask));
#if LWIP_DNS && 0
            /* ipcp_up() 中会设置 dns */
            ns = dns_getserver(0);
            __LOG_PFX(AW_GPRS_LOG_PPP, "   dns1        = %s", ipaddr_ntoa(ns));
            ns = dns_getserver(1);
            __LOG_PFX(AW_GPRS_LOG_PPP, "   dns2        = %s", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
            __LOG_PFX(AW_GPRS_LOG_PPP, "   our6_ipaddr = %s",
                      ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
            break;
        }
        case PPPERR_PARAM: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Invalid parameter");
            break;
        }
        case PPPERR_OPEN: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Unable to open PPP session");
            break;
        }
        case PPPERR_DEVICE: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Invalid I/O device for PPP");
            break;
        }
        case PPPERR_ALLOC: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Unable to allocate resources");
            break;
        }
        case PPPERR_USER: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: User interrupt");
            break;
        }
        case PPPERR_CONNECT: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Connection lost");
            break;
        }
        case PPPERR_AUTHFAIL: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Failed authentication challenge");
            break;
        }
        case PPPERR_PROTOCOL: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Failed to meet protocol");
            break;
        }
        case PPPERR_PEERDEAD: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Connection timeout");
            break;
        }
        case PPPERR_IDLETIMEOUT: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Idle Timeout");
            break;
        }
        case PPPERR_CONNECTTIME: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Max connect time reached");
            break;
        }
        case PPPERR_LOOPBACK: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Loopback detected");
            break;
        }
        default: {
            __LOG_PFX(AW_GPRS_LOG_PPP, "PPP_cb: Unknown error code %d", err_code);
            break;
        }
    }

    /*
     * This should be in the switch case, this is put outside of the switch
     * case for example readability.
     */
    if (err_code == PPPERR_NONE) {
        return;
    }

    /* ppp_close() was previously called, don't reconnect */
    if (err_code == PPPERR_USER) {
        /* ppp_free(); -- can be called here */
        return;
    }

    /*
     * Try to reconnect in 30 seconds, if you need a modem chatscript you have
     * to do a much better signaling here ;-)
     */
    /* ppp_connect(pcb, 30); */

    /* OR ppp_listen(pcb); */
}


/*
 * PPPoS serial output callback
 *
 * ppp_pcb, PPP control block
 * data, buffer to write to serial port
 * len, length of the data buffer
 * ctx, optional user-provided callback context pointer
 *
 * Return value: len if write succeed
 */
static u32_t __pppos_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    struct awbl_gprs_dev *p_this = (struct awbl_gprs_dev *)ctx;

    __DEBUG_SECTION(0) {
        __LOG_PFX(1, "output len:%d", len);
    }
    return sio_write(p_this->ppp.sio_mgn.fd, data, len);
}

static void __ppp_input_thread (void *arg)
{
    struct awbl_gprs_dev    *p_this = (struct awbl_gprs_dev *)arg;
    ppp_pcb                 *p_pcb  = p_this->ppp.p_pcb;
    uint32_t                 len;
    int                      ret;
    uint32_t                 rxbuf_size;
    aw_gprs_evt_t            evt;
    
    len = sizeof(p_this->ppp.rxbuf);
    aw_assert(len > (PPP_MRU + PPP_HDRLEN));
    aw_assert((len & (len - 1)) == 0);

    rxbuf_size = 1024;
    aw_assert((rxbuf_size & (rxbuf_size - 1)) == 0);
    aw_assert(rxbuf_size <= sizeof(p_this->ppp.rxbuf));
    aw_mdelay(1000);    /* Wait for ppp change its phase. */

    while (p_pcb->phase != PPP_PHASE_DEAD) {
#if 0
        aw_assert(p_this->ppp.sio_mgn.fd != NULL);
#else
        if (p_this->ppp.sio_mgn.fd == NULL) {
            __LOG_PFX(AW_GPRS_LOG_ERROR, "p_this->ppp.sio_mgn.fd == NULL");
            break;
        }
#endif

        __DEBUG_SECTION(1) {
            rxbuf_size = 512;
        }
        len = sio_read(p_this->ppp.sio_mgn.fd,
                       p_this->ppp.rxbuf,
                       rxbuf_size);

        if (len > 0) {
            __DEBUG_SECTION(1) {
                __LOG_PFX(1, "enter len:%d", len);
            }
            ret = pppos_input_tcpip(p_pcb, p_this->ppp.rxbuf, len);
            if (ret != ERR_OK) {
                __LOG_PFX(1, "[ERROR] ret:%d", ret);
            }
            __DEBUG_SECTION(0) {
                __LOG_PFX(1, "exit");
            }
        } else {
            __DEBUG_SECTION(0) {
                __LOG_PFX(1, "delay");
            }
            aw_mdelay(1);
        }
    }

    /* sys_thread_delete(PPP_THREAD_PRIO); */

    AW_SEMB_GIVE(p_this->ppp.thread_close);

    __LOG_PFX(AW_GPRS_LOG_PPP,
              "__ppp_input_thread(%d): terminate %c%c%d input threaad\n",
               __LINE__, p_this->ppp.netif.name[0], p_this->ppp.netif.name[1],
               p_this->ppp.netif.num);

    AW_TASK_TERMINATE(p_this->ppp.thread);

    evt.type = AW_GPRS_EVT_TYPE_PPP_LINK_DOWN;
    __evt_cb_array_notify(p_this, &evt);

    /* 加锁处理 ppp 关闭 */
    ret = AW_MUTEX_LOCK(p_this->mutex, aw_ms_to_ticks(10000));
    if (ret == AW_OK) {
        __gprs_ppp_close(p_this);
        AW_MUTEX_UNLOCK(p_this->mutex);
    } else {
        aw_assert(p_this->ppp.phase < AWBL_GPRS_PPP_PHASE_RUNNING);
    }
}
#endif /* AWBL_GPRS_USE_LWIP_2_X */

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

/* GPRS设备打开以太网函数 ---*
 * p_this ：GPRS设备结构体
 * 返回             ：成功返回AW_OK*/
aw_local aw_err_t __gprs_ethernet_open (struct awbl_gprs_dev *p_this)
{
    aw_err_t ret;
    int      csq;

    aw_assert(p_this->ethernet.pfn_dial_up);
    /* 先检测信号强度 */
    __LOG_PFX(AW_GPRS_LOG_SYS,
              "Checking Ethernet device signal strength,PLease Wait...");
    ret = p_this->drvopr.pfn_csq_get(p_this, &csq);
    if (ret != AW_OK) {
        return ret;
    }

    if (csq < p_this->csq_min) {
        __RETURN_EMSG(-AW_GPRS_ECSQ, NULL);
    }
    __LOG_PFX(AW_GPRS_LOG_SYS,"Ethernet device signal strength : %d",csq);
    /* 拨号*/
    ret = p_this->ethernet.pfn_dial_up(p_this);
    aw_assert(p_this->snd_pipe == p_this->at_pipe);
    if (ret == AW_OK) {
        aw_gprs_evt_t evt;
        evt.type = AW_GPRS_EVT_TYPE_ETH_LINK_UP;
        __evt_cb_array_notify(p_this, &evt);

        p_this->ethernet.phase = AWBL_GPRS_ETHERNET_PHASE_DIAL_UP;
    } else {
        return ret;
    }
    return AW_OK;
}

/* GPRS设备关闭以太网函数  */
aw_local aw_err_t __gprs_ethernet_close (struct awbl_gprs_dev *p_this)
{
    aw_err_t ret;
    aw_gprs_evt_t evt;
    (void)ret;
    /* 检查以太网状态*/
    if (p_this->ethernet.phase == AWBL_GPRS_ETHERNET_PHASE_DIAL_UP) {
        evt.type = AW_GPRS_EVT_TYPE_ETH_LINK_DOWN;
        __evt_cb_array_notify(p_this, &evt);

        ret = p_this->ethernet.pfn_dial_down(p_this);
    }
    p_this->ethernet.phase = AWBL_GPRS_ETHERNET_PHASE_INIT;
    return AW_OK;
}

#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

aw_local aw_err_t __gprs_ppp_open (struct awbl_gprs_dev *p_this)
{
    aw_err_t ret;
    int      i;
    int      csq;

    aw_assert(p_this->ppp.pfn_dial_up);

    if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
        return AW_OK;
    }

    /* 先检测信号强度 */
    __LOG_PFX(AW_GPRS_LOG_SYS,
              "Checking device signal strength,PLease Wait...");
    ret = p_this->drvopr.pfn_csq_get(p_this, &csq);
    if (ret != AW_OK) {
        return ret;
    }
    if (csq < p_this->csq_min) {
        __RETURN_EMSG(-AW_GPRS_ECSQ, NULL);
    }
    __LOG_PFX(AW_GPRS_LOG_SYS,"The device signal strength : %d",csq);

    if (p_this->ppp.phase < AWBL_GPRS_PPP_PHASE_INIT) {
#if AWBL_GPRS_USE_LWIP_1_4
        pppInit();
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_INIT;
    }

    if (p_this->ppp.phase < AWBL_GPRS_PPP_PHASE_DIAL_UP) {
        ret = p_this->ppp.pfn_dial_up(p_this);
        aw_assert(p_this->snd_pipe == p_this->at_pipe);
        if (ret == AW_OK) {
            p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_DIAL_UP;
        } else {
            return ret;
        }
    }

    if (p_this->ppp.phase < AWBL_GPRS_PPP_PHASE_CREATE) {

#if AWBL_GPRS_USE_LWIP_1_4
        p_this->ppp.ppp_fd = pppOpen((void *)(uint32_t)p_this->ppp.sio_mgn.fd,
                                        __ppp_link_status_cb,
                                         (void*)p_this);
        if (p_this->ppp.ppp_fd < 0) {
            __RETURN_EMSG(-AW_GPRS_ECPPP, NULL);
        }

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        p_this->ppp.p_pcb = pppos_create(&p_this->ppp.netif,
                                         __pppos_output_cb, 
                                         __ppp_link_status_cb, 
                                         p_this);
        if (p_this->ppp.p_pcb == NULL) {
            __RETURN_EMSG(-AW_GPRS_ECPPP, NULL);
        }
        /* ppp_set_auth(p_this->ppp.p_pcb, PPPAUTHTYPE_PAP, "1234", "1234"); */
        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_CREATE;
#endif /* AWBL_GPRS_USE_LWIP_2_X */
    }

#if AWBL_GPRS_USE_LWIP_2_X
    AW_TASK_INIT(p_this->ppp.thread,
                 PPP_THREAD_NAME,
                 PPP_THREAD_PRIO,
                 PPP_THREAD_STACKSIZE,
                 __ppp_input_thread,
                 p_this);
    AW_TASK_STARTUP(p_this->ppp.thread);

    ret = ppp_connect(p_this->ppp.p_pcb, 0);
    if (ret) {
        __RETURN_EMSG(-AW_GPRS_EIPPP, NULL);
    }

    p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;

#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /* 等待 ppp 连接成功 */
    for (i = 0; i < 600; i++) {
        if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
            aw_gprs_evt_t evt;
            evt.type = AW_GPRS_EVT_TYPE_PPP_LINK_UP;
            __evt_cb_array_notify(p_this, &evt);
            return AW_OK;
        }
        aw_mdelay(100);
    }

    __gprs_ppp_close(p_this);

    __RETURN_EMSG(-AW_GPRS_EEPPP, NULL);
}

aw_local aw_err_t __gprs_ppp_close (struct awbl_gprs_dev *p_this)
{
    aw_err_t ret;

    aw_assert(p_this->ppp.pfn_dial_down);

    if (p_this->ppp.phase <= AWBL_GPRS_PPP_PHASE_INIT) {
        return AW_OK;
    }

    if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;
    }

    if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_CONNECT) {

#if AWBL_GPRS_USE_LWIP_1_4
        pppClose(p_this->ppp.ppp_fd);
        /* 
         * 需要等待 ppp 协议栈调用 __ppp_link_status_cb 回调释放 lock，否则，
         * 未释放锁时一关闭模组会导致 ppp_fd 管道为 NULL，ppp 使用该管道引起崩溃
         */
        ret = AW_SEMB_TAKE(p_this->ppp.link_close, aw_ms_to_ticks(30000));
        if (ret) {
            __LOG_PFX(AW_GPRS_LOG_SYS,
                      "Failed to obtain PPP thread close semaphore");
        }

#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        ppp_close(p_this->ppp.p_pcb, 0);

        /* 等待ppp线程关闭的信号量 */
        ret = AW_SEMB_TAKE(p_this->ppp.thread_close, aw_ms_to_ticks(30000));
        if (ret) {
            __LOG_PFX(AW_GPRS_LOG_SYS,
                      "Failed to obtain PPP thread close semaphore");
        }
#endif /* AWBL_GPRS_USE_LWIP_2_X */

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_CREATE;
    }

    if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_CREATE) {
#if AWBL_GPRS_USE_LWIP_2_X
        ppp_free(p_this->ppp.p_pcb);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_DIAL_UP;
    }

    if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_DIAL_UP) {
        ret = p_this->ppp.pfn_dial_down(p_this);
    }

    p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_INIT;

    return ret;
}
/******************************************************************************/
aw_local aw_err_t __ioctl_req_at_trans (int                   id,
                                       aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;

    if (p_arg->at_trans.p_at_cmd) {
        ret = p_this->drvopr.pfn_at_send(p_this, p_arg->at_trans.p_at_cmd);
        if (ret) {
            return ret;
        }
    }

    ret = AW_OK;
    p_arg->at_trans.recvlen = 0;
    p_arg->at_trans.recv_tm = max(p_arg->at_trans.recv_tm, 100);
    if (p_arg->at_trans.p_recv_buf) {
        aw_assert(p_arg->at_trans.bufsize > 0);

        if (p_arg->at_trans.p_recv_expect) {
            char    *arg[1];
            char     buf[256];

            arg[0] = buf;
            if (sizeof(buf) <= strlen(p_arg->at_trans.p_recv_expect)) {
                /* The expected response is too long. */
                return -AW_ENOMEM;
            }
            memcpy(buf, p_arg->at_trans.p_recv_expect, 256);
            ret = p_this->drvopr.pfn_at_recv(p_this,
                                             (char *)p_arg->at_trans.p_recv_buf,
                                             p_arg->at_trans.bufsize,
                                             arg,
                                             1,
                                             p_arg->at_trans.recv_tm);
        } else {
            ret = p_this->drvopr.pfn_at_recv(p_this,
                                             (char *)p_arg->at_trans.p_recv_buf,
                                             p_arg->at_trans.bufsize,
                                             NULL,
                                             0,
                                             p_arg->at_trans.recv_tm);
        }

        if (ret >= 0) {
            p_arg->at_trans.recvlen = strlen((char *)p_arg->at_trans.p_recv_buf);
            ret = AW_OK;
        }
    }
    return ret;
}

aw_local aw_err_t __ioctl_evt_cb_set (int                   id,
                                      aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    int                   i  = 0 ;

    /* 检查是否已设置过该回调 */
    for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
        if (p_this->pfn_evt_cb_array[i] == p_arg->pfn_evt_handler) {
            __RETURN_EMSG(-AW_EEXIST, NULL);
        }
    }

    /* 查找为 NULL 的项，保存回调指针   */
    ret = -AW_ENOTEMPTY;
    for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
        if (p_this->pfn_evt_cb_array[i] == NULL) {
            p_this->pfn_evt_cb_array[i] = p_arg->pfn_evt_handler;
            ret = AW_OK;
            break;
        }
    }

    if (ret == AW_OK) {
        /* 设置回调后，立即上报当前事件 */
        if (p_this->last_evt.type != AW_GPRS_EVT_TYPE_NONE) {
            p_arg->pfn_evt_handler(id, &p_this->last_evt);
        }
    } else {
        __RETURN_EMSG(ret, NULL);
    }

    return ret;
}


/*
 * 不需要初始化即可设置的选项
 */
static aw_err_t __ioctl_without_stat (struct awbl_gprs_dev *p_this,
                                      aw_gprs_ioctl_req_t   req,
                                      aw_gprs_ioctl_args_t *p_arg)
{
    switch (req) {
        case AW_GPRS_LOG_SET: {
            p_this->pfn_log_out = p_arg->pfn_log_out;
        } break;

        case AW_GPRS_EVT_CB_SET: {
            /**
             * 设置事件回调，支持同时设置多个回调；
             * 掉电不清除事件回调
             */
            return __ioctl_evt_cb_set(p_this->dev_id, p_arg);

        } break;

        case AW_GPRS_EVT_CB_CLR: {
            int i = 0;
            for (; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                if (p_this->pfn_evt_cb_array[i] == p_arg->pfn_evt_handler) {
                    p_this->pfn_evt_cb_array[i] = NULL;
                }
            }
        } break;

        default:{
            return -AW_ENOTSUP;
        }
    }
    return AW_OK;
}

/*
 * 需要初始化完成后才可设置的选项
 */
static aw_err_t __ioctl (struct awbl_gprs_dev *p_this,
                         aw_gprs_ioctl_req_t   req,
                         aw_gprs_ioctl_args_t *p_arg)
{
    aw_gprs_evt_t         evt;
    aw_err_t              ret = -AW_ENOTSUP;

    if (!p_this->init_completed) {
        __RETURN_EMSG(-AW_EACCES, NULL);
    }

    switch (req) {
    case AW_GPRS_REQ_PROTOCOL_SET:
        /* 网络已经连接的情况不允许切换协议栈  */
        if (p_this->socket_num > 0) {
            __RETURN_EMSG(-AW_EISCONN, ". (num:%d)", p_this->socket_num);
        }
        __LOG_PFX(AW_GPRS_LOG_SYS, "Switch %S protocol.",
                  0 == p_arg->proto_opt ? "external" : "internal");

        if (0 == p_arg->proto_opt) {
            p_this->use_external_protocol = AW_TRUE;
        } else {
            p_this->use_external_protocol = AW_FALSE;
        }
        ret = AW_OK;
        break;

    case AW_GPRS_REQ_NET_LINK_UP: {

        /* 检查使用的是模块内部协议栈则失败退出 */
        if (!p_this->use_external_protocol) {
            __LOG_PFX(AW_GPRS_LOG_SYS, "The network link cannot be established "
                     "currently using the internal protocol stack!");
            __RETURN_EMSG(-AW_ENXIO, NULL);
        }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            ret = __gprs_ethernet_open(p_this);
            if (ret) {
                return ret;
            }
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            ret = __gprs_ppp_open(p_this);
            if (ret) {
                return ret;
            }
        }
    } break;

    case AW_GPRS_REQ_RF_REBOOT: {
        if (p_this->drvopr.pfn_radio_reboot) {
            ret = p_this->drvopr.pfn_radio_reboot(p_this);
            if (ret) {
                /* Again */
                ret = p_this->drvopr.pfn_radio_reboot(p_this);
            }
        } else {
            ret = -AW_ENOTSUP;
        }
    } break;

    case AW_GPRS_REQ_NET_LINK_DOWN: {
        /* 检查使用的是模块内部协议栈则失败退出 */
        if (!p_this->use_external_protocol) {
            __RETURN_EMSG(-AW_ENXIO, NULL);
        }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            ret = __gprs_ethernet_close(p_this);
            if (ret) {
                return ret;
            }
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            ret = __gprs_ppp_close(p_this);
            if (ret) {
                return ret;
            }
        }
    } break;

    case AW_GPRS_REQ_APN_SET: {
        p_this->info.p_apn = p_arg->p_apn;
        ret = AW_OK;
        break;
    }
    case AW_GPRS_REQ_APN_GET: {
        if (p_this->info.p_apn != NULL) {
            p_arg->p_apn = p_this->info.p_apn;
            return AW_OK;
        } else {
            ret = p_this->drvopr.pfn_apn_get(p_this);
            if (!ret) {
                p_arg->p_apn = p_this->info.p_apn;
            } else {
                p_arg->p_apn = NULL;
            }
        }
        break;
    }
    case AW_GPRS_REQ_AT_TRANS: {
        int end = strlen(p_arg->at_trans.p_at_cmd) - 1;
        if (p_arg->at_trans.p_at_cmd[end] != '\r') {
            __LOG_PFX(AW_GPRS_LOG_WARN, "No found the end character.");
        }
        ret = __ioctl_req_at_trans(p_this->dev_id, p_arg);
    } break;

    case AW_GPRS_REQ_STS:
        ret = __gprs_status_check(p_this);
        if (ret == AW_OK) {
            p_arg->sts = p_this->status;
        }
        break;

    case AW_GPRS_REQ_IP_GET: {

#if AWBL_GPRS_USE_LWIP_1_4

    #if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            p_arg->ip_addr = p_this->ethernet.ethernetaddr.our_ipaddr;
        }
    #endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            p_arg->ip_addr = p_this->ppp.p_pppaddr->our_ipaddr;
        }

        ret = AW_OK;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X

    #if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            p_arg->ip_addr = p_this->ethernet.ethernetaddr.our_ipaddr;
        }
    #endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            p_arg->ip_addr = p_this->ppp.netif.ip_addr;
        }
        ret = AW_OK;
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    } break;

    case AW_GPRS_REQ_CSQ_MIN_SET: {
        p_this->csq_min = p_arg->csq;
        ret = AW_OK;
    } break;

    case AW_GPRS_REQ_CSQ_GET:

        /* ret < 0 表示信号弱到无法使用或查询不到信号强度 */
        ret = p_this->drvopr.pfn_csq_get(p_this, &p_arg->csq);
        break;

    case AW_GPRS_REQ_RESET: {
        /*
         * 0:软件复位, else:硬件复位;
         * 应断开拨号、socket连接等
         */

        evt.type = AW_GPRS_EVT_TYPE_RESET_PRE;
        ret = __evt_cb_array_notify(p_this, &evt);
        if (ret != AW_OK) {
            __LOG_PFX(1, "The request reset failed because some devices "
                      "were not allowed! (ret:%d)", ret);
            return ret;
        }

        if (p_this->socket_num > 0) {
            __RETURN_EMSG(-AW_EISCONN, ". (num:%d)", p_this->socket_num);
        }

        ret = __reset(p_this, p_arg->reset.flag_hw_rst, p_arg->reset.delay_ms);
        p_this->status = AW_GPRS_EAT;

    } break;

    case AW_GPRS_REQ_CCID_GET: {
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = p_this->drvopr.pfn_ccid_get(p_this,
                                          p_arg->ccid,
                                          sizeof(p_arg->ccid));
    } break;

    case AW_GPRS_REQ_VERSION_GET: {
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = p_this->drvopr.pfn_module_version_get(p_this,
                                                    p_arg->version,
                                                    sizeof(p_arg->version));
    } break;

    case AW_GPRS_REQ_IMEI_GET:{
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = p_this->drvopr.pfn_imei_get(p_this, p_arg->imei,
                                          sizeof(p_arg->imei));
    } break;

    case AW_GPRS_REQ_IMSI_GET:{
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = p_this->drvopr.pfn_imsi_get(p_this, p_arg->imsi,
                                          sizeof(p_arg->imsi));
    } break;
    case AW_GPRS_REQ_SCA_GET:
    case AW_GPRS_REQ_NET_MODE_GET:
    case AW_GPRS_REQ_BS_GET:
    case AW_GPRS_REQ_PING:
    default:
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}

/*****************************************************************************/
static int __start (struct awbl_gprs_dev    *p_this,
                    const char              *p_mode,
                    char                    *p_ip,
                    uint16_t                 port,
                    uint32_t                 timeout)
{
    aw_err_t              ret;
    int                   hd;
    uint8_t               mode;

    if (!p_this->init_completed) {
        __RETURN_EMSG(-AW_EACCES, NULL);
    }
    __LOG_PFX(AW_GPRS_LOG_SOCK, "Create a connection."
            "(proto:%s, mode:%s, ip:%s, port:%d)",
                  (p_this->use_external_protocol? "external":"internal"),
                  p_mode, p_ip, port);

    /* 使用外部协议栈 */
    if (p_this->use_external_protocol) {
        struct sockaddr_in  server;
        struct sockaddr_in  client;
#if AWBL_GPRS_USE_LWIP_1_4
        int                 rcv_timeout = 10;
#endif
#if AWBL_GPRS_USE_LWIP_2_X
        struct timeval      rcv_timeout;
#endif
        int                 link_err    = 0;
        struct in_addr      addr;
        struct netif       *ppp_netif;

        (void)addr;
        (void)client;
        (void)ppp_netif;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            if (p_this->ethernet.phase != AWBL_GPRS_ETHERNET_PHASE_DIAL_UP) {
                ret = __gprs_ethernet_open(p_this);
                if(ret != AW_OK) {
                    return ret;
                }
            }
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            if (p_this->ppp.phase != AWBL_GPRS_PPP_PHASE_RUNNING) {
                ret = __gprs_ppp_open(p_this);
                if (ret) {
                    return ret;
                }
            }
        }

        while (1) {
            if (link_err != 0 && hd >= 0) {
                closesocket(hd);
            }
            if (link_err >= 1) {
                link_err = 0;
                __RETURN_EMSG(ret, NULL);
            }

            if (strcmp("TCP", p_mode) == 0) {
                hd = socket(AF_INET, SOCK_STREAM, 0);
            } else if (strcmp("UDP", p_mode) == 0) {
                hd = socket(AF_INET, SOCK_DGRAM, 0);
            } else {
                __RETURN_EMSG(-AW_EPROTOTYPE, NULL);
            }

            if (hd == -1) {
                link_err++;
                ret = -AW_ENOBUFS;
                continue;
            }

#if AWBL_GPRS_USE_LWIP_2_X
            rcv_timeout.tv_sec  = 0;
            rcv_timeout.tv_usec = 30000;
#endif
            ret = setsockopt(hd, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout,
                             sizeof(rcv_timeout));
            if (ret != 0) {
                link_err++;
                ret = -AW_GPRS_ESOPT;
                continue;
            }

            memset(&server, 0, sizeof(struct sockaddr_in));
            server.sin_family = AF_INET;
            server.sin_port   = htons(port);

            /* 将字符串型的ip地址转换为32位的网络序列号 */
            ret = inet_aton(p_ip, &(server.sin_addr));
            if (ret == 0) {
                struct hostent * host;

                host = gethostbyname(p_ip);
                server.sin_addr = *((struct in_addr *)host->h_addr);
                __LOG_PFX(AW_GPRS_LOG_SOCK,
                          "Parsing host name: %s  IP: %s\n)",
                          p_ip,
                          inet_ntoa(server.sin_addr));
            }

            /* UDP也使用connect的方式 */
            ret = connect(hd, (struct sockaddr *)(&server),
                          sizeof(struct sockaddr));
            if (ret == AW_OK) {
                /* 连接成功，设置状态后退出 */
                p_this->socket_num++;
                return hd;
            } else {
                link_err++;
                ret = -AW_ETIMEDOUT;
                continue;
            }
        }

    /* 使用内部协议栈 */
    } else {
        if (p_this->drvopr.pfn_tcpudp_conn == NULL) {
            __RETURN_EMSG(-AW_ENOTSUP, NULL);
        }
        if (strcmp("TCP", p_mode) == 0) {
            mode = 0;
        } else if (strcmp("UDP", p_mode) == 0) {
            mode = 1;
        } else {
            __RETURN_EMSG(-AW_EPROTOTYPE, NULL);
        }
        hd = p_this->drvopr.pfn_tcpudp_conn(p_this, mode, p_ip, port, timeout);
        if (hd >= 0) {
            ret = AW_OK;
        } else {
            ret = hd;
        }
    }
    return hd;  /* Success */
}

/*****************************************************************************/
/** @brief 把GSM基本字符集转换成ASCII字符集, 不能转换的用' '代替 */
aw_const uint8_t gsm_basic_ascii[256]={\
/*    0     1     2    3     4     5     6     7     8     9     A     B     C      D    E     F   */
/*0*/ 64,  156,   36, 157,  138,  130,  151,  141,  149,  128,   10,  237,  ' ',   13,  143,  134,\
/*1*/' ',   95,  232, 226,  ' ',  234,  252,  ' ',  228,  233,  ' ',   27,  146,  145,  225,  144,\
/*2*/ 32,   33,   34,  35,  ' ',   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,\
/*3*/ 48,   49,   50,  51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,\
/*4*/173,   65,   66,  67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,\
/*5*/ 80,   81,   82,  83,   84,   85,   86,   87,   88,   89,   90,  142,  153,  165,  154,  ' ',\
/*6*/168,   97,   98,  99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,\
/*7*/112,  113,  114, 115,  116,  117,  118,  119,  120,  121,  122,  132,  148,  164,  129,  133 \
};

/** @brief 把GSM扩展字符集转换成ASCII字符集, 不能转换的用' '代替 */
aw_const uint8_t gsm_extern_ascii[256]={\
/*    0     1     2    3     4     5     6     7     8     9     A     B     C      D    E     F   */
/*0*/ 64,  156,   36, 157,  138,  130,  151,  141,  149,  128,   10,  237,  ' ',   13,  143,  134,\
/*1*/' ',   95,  232, 226,   94,  234,  252,  ' ',  228,  233,  ' ',  ' ',  146,  145,  225,  144,\
/*2*/ 32,   33,   34,  35,  ' ',   37,   38,   38,  123,  125,   42,   43,   44,   45,   46,   92,\
/*3*/ 48,   49,   50,  51,   52,   53,   54,   55,   56,   57,   58,   59,   91,  126,   93,   63,\
/*4*/124,   65,   66,  67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,\
/*5*/ 80,   81,   82,  83,   84,   85,   86,   87,   88,   89,   90,  142,  153,  165,  154,  ' ',\
/*6*/168,   97,   98,  99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,\
/*7*/112,  113,  114, 115,  116,  117,  118,  119,  120,  121,  122,  132,  148,  164,  129,  133 \
};


#if 0
/** \brief  查找国标码   */
aw_local uint16_t gb_search(uint16_t gb)
{
    uint16_t i;
    uint16_t max_index = AW_NELEMENTS(gb2312_unicode_table);

    for (i=0; i<max_index; i++) {
        if (gb2312_unicode_table[i][0] == gb) {
            return gb2312_unicode_table[i][1];
        }
    }
    return 0x0020;      /* 表中查找不到返回空格*/
}

/** \brief  把国标码转换为Unicode编码   */
aw_local uint16_t gb2unicode(uint8_t *pgb, uint16_t *puni)
{
    uint16_t gb;
    uint16_t len = 0;

    while (*pgb) {
        if (*pgb < 0x80) {
            *puni++ = gb_search(*pgb);
            pgb++;
            len++;
        } else {
            gb  = (*pgb++ << 8) & 0xFF00;
            gb |= *pgb++;
            *puni++ = gb_search(gb);
            len++;
        }
    }
    return len;
}

/** \brief  查找Unicode编码   */
aw_local uint16_t unicode_search(uint16_t uni)
{
    uint16_t i;
    uint16_t max_index = AW_NELEMENTS(gb2312_unicode_table);

    for (i=0; i<max_index; i++) {
        if (gb2312_unicode_table[i][1] == uni) {
            return gb2312_unicode_table[i][0];
        }
    }
    return 0x0020;      /* 表中查找不到返回空格*/
}

/** \brief  把Unicode编码转换为国标码   */
aw_local void unicode2gb(uint16_t *puni, uint8_t *pgb)
{
    uint16_t gb;

    while (*puni) {
        gb = unicode_search(*puni);
        if (gb < 0x100) {
            *pgb++ = gb & 0xff;
        } else {
            *pgb++ = (gb>>8) & 0xff;
            *pgb++ = gb & 0xff;
        }
        puni++;
    }
}
#endif

/** \brief 把数字1~15转换成字符'1','2','3',....'A','B','C','D','E','F' */
aw_local char ctoh(uint8_t num)
{
    if (num > 15) {
        return 0;
    } else if(num >= 10) {
        return (num+0x37);
    } else {
        return (num+0x30);
    }
}

/** \brief 把8位的数值转换成两个16进制数字符 */
aw_local void ctox(uint8_t num, char *cnum)
{
    cnum[0] = ctoh(num/16);
    cnum[1] = ctoh(num%16);
}

/** \brief 把两个16进制数字符转换成8位的数值 */
aw_local uint8_t xtoc(char *cnum)
{
    uint8_t a=0;

    if ((cnum[0]>='A') && (cnum[0]<='F')) {
        a = (cnum[0]-55)*16;
    } else if ((cnum[0]>='a') && (cnum[0]<='f')) {
        a = (cnum[0]-87)*16;
    } else if ((cnum[0]>='0') && (cnum[0]<='9')) {
        a = (cnum[0]-0x30)*16;
    } else {
        return 0;
    }

    if ((cnum[1]>='A') && (cnum[1]<='F')) {
        a += cnum[1]-55;
    } else if ((cnum[1]>='a') && (cnum[1]<='f')) {
        a += cnum[1]-87;
    } else if ((cnum[1]>='0') && (cnum[1]<='9')) {
        a += cnum[1]-0x30;
    } else {
        return 0;
    }

    return a;
}


/** \brief 把GSM编码转换成ASCII编码   */
aw_local uint16_t gsm2ascii(uint8_t *pgsm, uint8_t *pascii, uint16_t len)
{
    uint16_t i, j;
    uint8_t esc=0;
    uint16_t ret = len;

    for (i=0, j=0; i<len; i++) {
        if (pgsm[i] == 0x1B) {
            esc = 1;
            ret--;
        } else {
            if (esc == 1) {
                esc = 0;
                pascii[j++] = gsm_extern_ascii[pgsm[i]];
            } else {
                pascii[j++] = gsm_basic_ascii[pgsm[i]];
            }
        }
    }
    return ret;
}

#if 0
/** \brief 超长短信7位解码   */
aw_local void sms_7bit_decode_for_super(char *cmsg, uint8_t len, char *msg)
{
    aw_const uint8_t mask[]={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
    uint8_t i, j, k;
    uint8_t  mess[170]={0};
    uint8_t tmp=0;
    uint8_t pdu_len;
    uint8_t data;

    pdu_len = (len-1)-(len-1)/8+1;

    data = xtoc(cmsg);;
    cmsg += 2;
    mess[0] = (data>>1) & 0x7f;
    j=1;
    k=1;
    for (i=1; i<pdu_len; i++) {
        data = xtoc(cmsg);
        cmsg += 2;
        if (i%7 == 0) {
            mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
            mess[k++] = (data>>1) & 0x7f;
            continue;
        }
        if ((i-1)%7 == 0) {
            mess[k++] = data & 0x7f;
            tmp = data;
            j=1;
            continue;
        }
        mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
        tmp = data;
        j++;
    }
    gsm2ascii(mess, (uint8_t *)msg, len);
}
#endif

/** \brief 普通短信7位解码   */
aw_local void sms_7bit_decode_for_normal(char *cmsg, uint8_t len, char *msg)
{
    aw_const uint8_t mask[]={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
    uint8_t i, j, k;
    uint8_t mess[170]={0};
    uint8_t tmp=0;
    uint8_t pdu_len;
    uint8_t data;

    pdu_len = len-len/8;

    j=1;
    k=0;
    for (i=0; i<pdu_len; i++) {
        data = xtoc(cmsg);
        cmsg += 2;
        if ((i+1)%7 == 0) {
            mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
            mess[k++] = (data>>1) & 0x7f;
            continue;
        }
        if (i%7 == 0) {
            mess[k++] = data & 0x7f;
            tmp = data;
            j=1;
            continue;
        }
        mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
        tmp = data;
        j++;
    }
    gsm2ascii(mess, (uint8_t *)msg, len);
}

/*
 * dst：目的地址
 * src：源地址
 * dpos：目的字节的位偏移
 * spos：源字节的位偏移
 * copy_bits：要拷贝的位数
 */
#define __BIT_COPY(dst, dpos, src, spos, copy_bits)     \
do {                                                    \
    if (dpos == 0) {                                    \
        *dst = ((*src) >> spos);                        \
    } else if (spos == 0) {                             \
        *dst |= ((*src) << dpos);                       \
    }                                                   \
} while (0)


/** \brief 将 7bit 的 ascii 码存到字节中,如8个字符可压缩为7个字节 */
static int __7bit_to_byte (uint8_t *p_dst, int dst_size, char *p_src, int src_len)
{
    int dpos = 0;
    int spos = 0;
    int dsize = 0;
    int ssize = 0;
    uint8_t *dtmp;
    char    *stmp;

    int len;

    len = ((src_len * 7) + 7) / 8;

    if (len > dst_size) {
        return -AW_ENOMEM;
    }

    dtmp = p_dst;
    stmp = p_src;
    dsize = 8;
    ssize = 7;

    memset(p_dst , 0, dst_size);

    while (1) {

        if (dsize > ssize) {
            /* dtmp 字节中bit空间大于 stmp 中的，则将 ssize 全存入 */
            __BIT_COPY(dtmp, dpos, stmp, spos, ssize);

            dpos    = dpos + ssize;
            dsize   = dsize - ssize;

            stmp    = stmp + 1;
            spos    = 0;
            ssize   = 7;
        } else if (dsize == ssize) {
            /* 剩余空间刚好相等 */
            __BIT_COPY(dtmp, dpos, stmp, spos, dsize);

            dtmp    = dtmp + 1;
            dpos    = 0;
            dsize   = 8;

            stmp    = stmp + 1;
            spos    = 0;
            ssize   = 7;
        } else if ((dsize < ssize)) {
            /* stmp 中剩余的bit较多 */
            __BIT_COPY(dtmp, dpos, stmp, spos, dsize);

            spos    = spos + dsize;
            ssize   = ssize - dsize;

            dtmp    = dtmp + 1;
            dpos    = 0;
            dsize   = 8;
        }

        /* 判断 src 中的字符是否全存入了 dst，dst 最后一个字符剩余的 bit 全填0 */
        if (stmp == p_src + src_len) {
            return len;
        }
    }

    return len;
}

/**
 * UTF-8转ucs2编码
 */
static uint32_t utf8_to_ucs2 (const char   *utf8,
                              int           utf8_length,
                              char         *ucs2,
                              int           ucs2_length)
{
    if((utf8_length < 1) || (utf8_length > ucs2_length - 1)){
        return 0;
    }
    if((utf8 == NULL) || (ucs2 == NULL)){
        return 0;
    }

    uint32_t utf8_pos = 0;
    uint32_t ucs2_pos = 0;

    while(1){
        if((utf8[utf8_pos] & 0x80) == 0){
            ucs2[ucs2_pos] = 0;
            ucs2[ucs2_pos + 1] = utf8[utf8_pos] & 0x7f;
            utf8_pos += 1;
            ucs2_pos += 2;
        }
        else if((utf8[utf8_pos] & 0xe0) == 0){
            ucs2[ucs2_pos] = (utf8[utf8_pos] & 0x1f) >> 2;
            ucs2[ucs2_pos + 1] = ((utf8[utf8_pos] & 0x03) << 6) |
                    (utf8[utf8_pos + 1] & 0x3f);
            utf8_pos += 2;
            ucs2_pos += 2;
        }
        else if((utf8[utf8_pos] & 0xf0) == 0xe0){
            ucs2[ucs2_pos] = ((utf8[utf8_pos] & 0x0f) << 4) |
                    ((utf8[utf8_pos + 1] & 0x3f) >> 2);
            ucs2[ucs2_pos + 1] = ((utf8[utf8_pos + 1] & 0x03) << 6) |
                    (utf8[utf8_pos + 2] & 0x3f);
            utf8_pos += 3;
            ucs2_pos += 2;
        }
        else{
            return 0;
        }

        if((utf8_pos >= utf8_length) || (ucs2_pos >= ucs2_length - 1)){
            ucs2[ucs2_pos] = '\0';
            break;
        }
    }
    return ucs2_pos;
}

/**
 * UCS2转UTF-8编码
 */
static uint32_t ucs2_to_utf8 (const char   *ucs2,
                              int           ucs2_length,
                              char         *utf8,
                              int           utf8_length)
{
    if((utf8_length < 1) || (utf8_length - 1 < ucs2_length)){
        return 0;
    }
    if((utf8 == NULL) || (ucs2 == NULL)){
        return 0;
    }

    uint32_t utf8_pos = 0;
    uint32_t ucs2_pos = 0;

    while(1){
        if((ucs2[ucs2_pos] == 0x00) && (ucs2[ucs2_pos + 1] <= 0x7f)){
            utf8[utf8_pos] = ucs2[ucs2_pos + 1] & 0x7f;
            ucs2_pos += 2;
            utf8_pos += 1;
        }
        else if((ucs2[ucs2_pos] & 0xf8) == 0){
            utf8[utf8_pos] = 0xc0 | ((ucs2[ucs2_pos] & 0x07) << 2) |
                    ((ucs2[ucs2_pos + 1] & 0xc0) >> 6);
            utf8[utf8_pos + 1] = 0x80 | (ucs2[ucs2_pos + 1] & 0x3f);
            ucs2_pos += 2;
            utf8_pos += 2;
        }
        else{
            utf8[utf8_pos] = 0xe0 | ((ucs2[ucs2_pos] & 0xf0) >> 4);
            utf8[utf8_pos + 1] = 0x80 | ((ucs2[ucs2_pos] & 0x0f) <<2) |
                    ((ucs2[ucs2_pos + 1] & 0xc0) >> 6);
            utf8[utf8_pos + 2] = 0x80 | (ucs2[ucs2_pos + 1] & 0x3f);
            ucs2_pos += 2;
            utf8_pos += 3;
        }
        if((ucs2_pos >= ucs2_length) || ((utf8_pos >= utf8_length - 1))){
            utf8[utf8_pos] = '\0';
            break;
        }
    }
    return utf8_pos;
}


/**
 * \brief 将数据打包成PDU（protocol data unit）包
 *
 * \param[in]  message   存放打包的数据的buf
 * \param[in]  msg_size  buf大小
 * \param[in]  p_sms     短信内容
 * \param[in]  len       短信长度
 * \param[in]  p_num     接收方号码
 * \param[in]  p_sca     短消息服务中心地址
 * \param[in]  code      短信编码格式
 *
 * \retval AW_OK        :  成功
 */
static aw_err_t __pdu_pack  (struct awbl_gprs_dev   *p_this,
                             char     *message,
                             size_t    msg_size,
                             uint8_t  *p_sms,
                             uint8_t   len,
                             char     *p_num,
                             char     *p_sca,
                             uint8_t   code)
{
    aw_err_t ret;
    char num_tmp[50]={0};
    char num[50]={0};

    char plen[3]={0};
    uint16_t headlen;
    int i;
    aw_const char *p_dcs_code;
    aw_const char PHONE_END[]="F";              /* 手机号码填充 */
    aw_const char PDU_TYPE_S[]="1100";          /* PDU Type 表示单条短信 */
    aw_const char PID[]="00";                   /* PID 固定为00 */
    aw_const char CODE_ASC[]="00";              /* DCS 表示7位编码 */
    aw_const char CODE_HEX[]="04";              /* DCS 表示HEX编码(8位编码) */
    aw_const char CODE_UCS2[]="08";             /* DCS 表示 UCS2 编码 */
    aw_const char VP[]="FF";                    /* VP 短信有效时间 */

    if (msg_size < 370) {
        __RETURN_EMSG(-AW_GPRS_ESMSM, NULL);
    }
    (void)ret;

    strncpy(num_tmp, p_num, sizeof(num_tmp)-1);
    /* 目标号码为国际号码  */
    if (num_tmp[0] == '+') {
        ctox((strlen(num_tmp)-1), num);
        num[2] = '9';
        num[3] = '1';
        /* 号码除了'+'后为奇数，需要在号码后面添加F */
        if ((strlen(num_tmp)-1)%2 != 0) {
            strcat(num_tmp, PHONE_END);
        }
        for (i=0; i<strlen(num_tmp)-1; i+=2) {
            num[4+i] = num_tmp[2+i];
            num[5+i] = num_tmp[1+i];
        }
        num[4+strlen(num_tmp)-1] = 0;

    /* 目标号码为国家号码  */
    } else {
        ctox((strlen(num_tmp)), num);
        num[2] = '8';
        num[3] = '1';
        /* 号码为奇数，需要在号码后面添加F  */
        if (strlen(num_tmp)%2 != 0) {
            strcat(num_tmp, PHONE_END);
        }

        for (i=0; i<strlen(num_tmp); i+=2) {
            num[4+i] = num_tmp[1+i];
            num[5+i] = num_tmp[i];
        }
        num[4+strlen(num_tmp)] = 0;
    }
    memset(message, 0, msg_size);
    strcat(message, "00");                          /* SCA length */
    strcat(message, PDU_TYPE_S);                    /* PDU Type */
    strcat(message, num);                           /* DA */
    strcat(message, PID);                           /* PID */

    if (code == AW_GPRS_SMS_CODE_7BIT) {
        p_dcs_code = CODE_ASC;
    } else if (code == AW_GPRS_SMS_CODE_8BIT) {
        p_dcs_code = CODE_HEX;
    } else if (code == AW_GPRS_SMS_CODE_UCS2 || code == AW_GPRS_SMS_CODE_UTF8) {
        p_dcs_code = CODE_UCS2;
    } else {
        aw_assert(AW_FALSE);
    }

    strcat(message, p_dcs_code);                    /* DCS 指定编码方式 */
    strcat(message, VP);                            /* VP 短信有效时间 */

    if (code == AW_GPRS_SMS_CODE_7BIT) {
        int sms_len = 0;
        uint8_t _buf[160];

        /* UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 7bit 编码 */
        sms_len = __7bit_to_byte(_buf, sizeof(_buf), (char *)p_sms, len);

        headlen = strlen(message);
        for (i=0; i<sms_len; i++) {
            ctox((uint8_t)_buf[i], &message[headlen]);
            headlen+=2;
        }
    } else if (code == AW_GPRS_SMS_CODE_8BIT || code == AW_GPRS_SMS_CODE_UCS2) {
        /*  UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 8bit 编码 */
        headlen = strlen(message);
        for (i=0; i<len; i++) {
            ctox((uint8_t)p_sms[i], &message[headlen]);
            headlen+=2;
        }
    } else if (code == AW_GPRS_SMS_CODE_UTF8) {

        /* 把utf8编码的短信内容转换成ucs2*/
        char ucs2_temp[512] = {0};

        len = utf8_to_ucs2((char *)p_sms, len, ucs2_temp, sizeof(ucs2_temp));
        if(len == 0){
            __RETURN_EMSG(-AW_EBADMSG, NULL);
        }

        /*  UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 8bit 编码 */
        headlen = strlen(message);
        for (i = 0; i < len; i++) {
            ctox((uint8_t)ucs2_temp[i], &message[headlen]);
            headlen+=2;
        }
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSC, NULL);
    }

    return AW_OK;
}


static aw_err_t __pdu_unpack (struct awbl_gprs_dev   *p_this,
                              char                   *p_pdu_data,
                              struct aw_gprs_sms_buf *p_to_sms)
{
    aw_err_t  ret;
    uint8_t   i;
    char     *pret;
    uint16_t  len;
    uint16_t  pdu_remain_len;
    uint8_t   char_set;

    (void) ret;

    struct {
        struct {
            char    number[25];
            uint8_t len;
            char    type[2];
        } sca;

        uint8_t pdu_type;
        uint8_t mr;
        struct {
            uint8_t len;
            uint8_t type;
            char    number[13];
        } addr;             /* OA or DA */

        uint8_t pid;
        uint8_t dcs;
        char    scts[32];
        uint8_t vp;
        uint8_t udl;
        uint8_t udh;        /* */
        char    ud[512];
    } pdu_fields;

    pret           = p_pdu_data;
    pdu_remain_len = strlen(p_pdu_data);
    memset(&pdu_fields, 0, sizeof(pdu_fields));

    memset(p_to_sms->p_num_buff, 0, p_to_sms->num_buff_len);
    memset(p_to_sms->p_time_buff, 0, p_to_sms->time_buff_len);
    memset(p_to_sms->p_sms_buff, 0, p_to_sms->sms_buff_len);

    /*  解析 Service Center Address  */
    pdu_fields.sca.len = xtoc(pret);
    pret += 2;
    if (pdu_remain_len >= 2 + pdu_fields.sca.len * 2) {
        /* type :81 为国内， 91 为国外  */
        memcpy(pdu_fields.sca.type, pret, 2);
        pret += 2;
        pdu_remain_len -= 2;

        /* len 包含 type 的长度 */
        pdu_fields.sca.len--;

        aw_assert(sizeof(pdu_fields.sca.number) > pdu_fields.sca.len * 2);
        memcpy(pdu_fields.sca.number, pret, pdu_fields.sca.len * 2);
        pret += pdu_fields.sca.len * 2;
        pdu_remain_len  -= pdu_fields.sca.len * 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }

    /* 解析  pdu type */
    if (pdu_remain_len < 2) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.pdu_type = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 Originator Address */
    if (pdu_remain_len < 4) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.addr.len     = xtoc(pret);               /*  短信号码长度   */
    pret += 2;
    pdu_fields.addr.type    = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 4;

    len = pdu_fields.addr.len;
    if (pdu_remain_len < len+len%2) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }

    for(i=0; i<len; i+=2) {
        pdu_fields.addr.number[i]   = pret[i+1];
        pdu_fields.addr.number[i+1] = pret[i];
    }
    pdu_fields.addr.number[len] = 0;
    pret            += len+len%2;
    pdu_remain_len  -= len+len%2;
    strncpy(p_to_sms->p_num_buff, pdu_fields.addr.number,
            p_to_sms->num_buff_len);

    /* 解析 PID  */
    if (pdu_remain_len < 2) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.pid = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 DCS (DataCoding Scheme) */
    if (pdu_remain_len < 2) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.dcs = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 SCTS(Service Center Time Stamp) */
    if (pdu_remain_len < 14) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[0] = pret[1];          /*  年      */
    pdu_fields.scts[1] = pret[0];
    pdu_fields.scts[2] = '/';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[3] = pret[1];          /*  月     */
    pdu_fields.scts[4] = pret[0];
    pdu_fields.scts[5] = '/';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[6] = pret[1];          /*  日     */
    pdu_fields.scts[7] = pret[0];
    pdu_fields.scts[8] = ',';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[9] = pret[1];          /*  时     */
    pdu_fields.scts[10] = pret[0];
    pdu_fields.scts[11] = ':';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[12] = pret[1];         /*  分     */
    pdu_fields.scts[13] = pret[0];
    pdu_fields.scts[14] = ':';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[15] = pret[1];         /*  秒     */
    pdu_fields.scts[16] = pret[0];
    pdu_fields.scts[17] = '+';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.scts[18] = pret[1];         /*  时区     */
    pdu_fields.scts[19] = pret[0];
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    strncpy(p_to_sms->p_time_buff, pdu_fields.scts, p_to_sms->time_buff_len);

    /* UDL (User Data Length : 1~140) */
    if (pdu_remain_len < 2) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    pdu_fields.udl = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* UD */
    if (pdu_remain_len < pdu_fields.udl) {
        __RETURN_EMSG(-AW_GPRS_ESMSPDU, NULL);
    }
    aw_assert(sizeof(pdu_fields.ud) > pdu_fields.udl * 2);
    memcpy(pdu_fields.ud, pret, pdu_fields.udl * 2);

    /*
     * 处理 User Data 格式
     */

    /* 判断是否有 UDHI 标志（UD 中包含用户数据头信息） :
     * SMS 消息无用户数据头；EMS(增强消息业务)才有该标志，如图片、铃声、长短信等;
     */
    if ((pdu_fields.pdu_type & 0x40) != 0) {
        /* 暂不支持 UDHI 标志 */
        __RETURN_EMSG(-AW_GPRS_EPDUUDHI, NULL);
    }

    if ((pdu_fields.udl * 2) > p_to_sms->sms_buff_len) {
        __RETURN_EMSG(-AW_GPRS_ESMSM, NULL);
    }

    char_set = (pdu_fields.dcs >> 2) & 0x03;    /* 获取使用的字符集 */
    switch (char_set) {
        /* 7Bit */
        case 0: {
            char message[200];
            memset(message, 0, sizeof(message));
            sms_7bit_decode_for_normal(pdu_fields.ud, pdu_fields.udl, message);
            strncpy(p_to_sms->p_sms_buff, message, p_to_sms->sms_buff_len);
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_8BIT;
        } break;

        /* 8Bit */
        case 1: {
            for(i = 0; i < pdu_fields.udl; i++) {
                p_to_sms->p_sms_buff[i] = xtoc(pdu_fields.ud + (2 * i));
            }
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_8BIT;
        } break;

        /* UCS2 */
        case 2: {
            for(i = 0; i < pdu_fields.udl; i++) {
                p_to_sms->p_sms_buff[i] = xtoc(pdu_fields.ud + (2 * i));
            }
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_UCS2;
        } break;

#if 0
        case 2: {
            int j;
            uint16_t unicode_tmp[70]={0};
            char message[200];

            for(i=0, j=0; i < pdu_fields.udl / 2; i++, j++) {
                unicode_tmp[j] = xtoc(pdu_fields.ud + 2 * i);
                unicode_tmp[j] = (unicode_tmp[j] << 8) & 0xFF00;
                unicode_tmp[j] |= xtoc(pdu_fields.ud + 2 * i + 1);
            }
            unicode2gb(unicode_tmp, (uint8_t *)message);
            strncpy(p_to_sms->p_sms_buff, message, p_to_sms->sms_buff_len);
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_GB;
        } break;
#endif
    }

    return AW_OK;
}

static aw_err_t __sms_init (struct awbl_gprs_dev   *p_this,
                            uint8_t                 sms_code)
{
    aw_err_t err = AW_OK;

    aw_assert(p_this->sms.p_num_buff);
    aw_assert(p_this->sms.p_time_buff);
    aw_assert(p_this->sms.p_sms_buff);

    if (sms_code == AW_GPRS_SMS_CODE_ATTEXT) {
        if (!p_this->sms_text_init) {
            aw_assert(p_this->drvopr.pfn_sms_text_init);
            aw_assert(p_this->drvopr.pfn_sms_text_send);
            err = p_this->drvopr.pfn_sms_text_init(p_this);
            if (err) {
                return err;
            }
            p_this->sms_text_init = 1;
        }
        /* Clear another sms initialization flag. */
        p_this->sms_pdu_init = 0;
    } else {
        if (!p_this->sms_pdu_init) {
            aw_assert(p_this->drvopr.pfn_sms_pdu_init);
            aw_assert(p_this->drvopr.pfn_sms_pdu_send);
            err = p_this->drvopr.pfn_sms_pdu_init(p_this);
            if (err) {
                return err;
            }
            p_this->sms_pdu_init = 1;
        }
        p_this->sms_text_init = 0;
    }
    return err;
}

static aw_err_t __sms_send (struct awbl_gprs_dev   *p_this,
                            uint8_t                *p_sms,
                            size_t                  len,
                            char                   *p_num,
                            uint8_t                 sms_code)
{
    aw_err_t err;
    if (!p_this->init_completed) {
        __RETURN_EMSG(-AW_EACCES, NULL);
    }

    err = p_this->drvopr.pfn_creg_chk(p_this);
    if (err != AW_OK) {
        return err;
    }

    err = __sms_init(p_this, sms_code);
    if (err) {
        return err;
    }

    if (sms_code == AW_GPRS_SMS_CODE_ATTEXT) {
        err = p_this->drvopr.pfn_sms_text_send(p_this, p_sms, len, p_num);
    } else {
        char    *pdubuf = p_this->sms.p_sms_buff;
        size_t   pdulen = p_this->sms.sms_buff_len;
        char    *p_sca  = p_this->info.p_center_num;

        if (p_sms == (uint8_t *)pdubuf) {
            __RETURN_EMSG(-AW_GPRS_ESMSSBUF, NULL);
        }
        memset(pdubuf, 0, pdulen);
        err    = __pdu_pack(p_this, pdubuf, pdulen, p_sms, len, p_num, p_sca,
                            sms_code);
        pdulen = strlen(pdubuf);
        if (err) {
            return err;
        }
        err = p_this->drvopr.pfn_sms_pdu_send(p_this, pdubuf, pdulen);
    }

    return err;
}

static aw_err_t __sms_recv (struct awbl_gprs_dev   *p_this,
                            struct aw_gprs_sms_buf *p_sms,
                            uint8_t                 sms_code)
{
    aw_err_t err;
    int      index;

again:
    if (!p_this->init_completed) {
        __RETURN_EMSG(-AW_EACCES, NULL);
    }
    err = __sms_init(p_this, sms_code);
    if (err) {
        return err;
    }

    /* Clear AT data */
    p_this->drvopr.pfn_pipe_recv(p_this->at_pipe, (uint8_t *)p_this->at_buf,
                                 p_this->at_bufsz);

    err = p_this->drvopr.pfn_sms_first_index_get(p_this, &index);
    if (err) {
        return err;
    }

    if (sms_code == AW_GPRS_SMS_CODE_ATTEXT) {
        err = p_this->drvopr.pfn_sms_text_recv(p_this, index, p_sms);
    } else {
        char     utf8[1024] = {0};
        uint8_t pdu[1024];
        size_t   pdulen = sizeof(pdu);

        err = p_this->drvopr.pfn_sms_pdu_recv(p_this, index, pdu, pdulen);
        if (err) {
            return err;
        }
        /* Parse PDU */
        err = __pdu_unpack(p_this, (char *)pdu, &p_this->sms);
        if (err) {
            /* Delete the message that just failed to parse. */
            p_this->drvopr.pfn_sms_del(p_this, index);
            goto again;
        }

        /* Convert UCS2 to UTF8 encoding.*/
        if (p_this->sms.sms_code == AW_GPRS_SMS_CODE_UCS2 &&
                sms_code == AW_GPRS_SMS_CODE_UTF8) {
            err = ucs2_to_utf8(p_this->sms.p_sms_buff,
                               p_this->sms.sms_buff_len,
                               utf8,
                               sizeof(utf8));
            if(err <= 0) {
                __RETURN_EMSG(-AW_GPRS_EUCS2UTF, NULL);
            }
            utf8[err] = 0;
            strcpy(p_this->sms.p_sms_buff, utf8);
        }

        /* Delete the message that just read. */
        err = p_this->drvopr.pfn_sms_del(p_this, index);
        if (err) {
            return err;
        }

        *p_sms = p_this->sms;
    }
    return err;
}



/*****************************************************************************/

/* 返回 >= 0 表示 reply 时间长度， < 0 响应失败 */
static int __ping_check (struct awbl_gprs_dev *p_this)
{
#if AWBL_GPRS_USE_LWIP_2_X
    struct sockaddr_storage  to;
    struct sockaddr_storage  from;
    struct sockaddr_in      *si_to;
    struct sockaddr_in      *si_from;
    uint8_t                  ttl;
#endif
    struct in_addr           dest_addr = {0};
    aw_ping_t                ping;
    int                      ret;
    int                      i;
    struct  netif           *netif = __gprs_netif_get();

    if (!netif) {
        __RETURN_EMSG(-AW_EXDEV, NULL);
    }

#if AWBL_GPRS_USE_LWIP_1_4
    dest_addr.s_addr = p_this->ppp.p_pppaddr->dns1.addr;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
    memset(&to, 0, sizeof(to));
    memset(&from, 0, sizeof(from));

    to.ss_family = AF_INET;
    to.s2_len = sizeof(struct sockaddr_in);

    si_to = (struct sockaddr_in *) &to;

#if 0
    si_to->sin_addr.s_addr =
            (in_addr_t)ip4_addr_get_u32(ip_2_ip4(&p_this->ppp.netif.gw));
#elif 1
    si_to->sin_addr.s_addr =
            (in_addr_t)p_this->ppp.p_pcb->ipcp_gotoptions.dnsaddr[0];
#else
    ret = inet_aton("121.33.243.38", &si_to->sin_addr);
    __RETURN_IF(ret == 0, -AW_EINVAL);
#endif

    from.ss_family = AF_INET;
    from.s2_len = sizeof(struct sockaddr_in);

    si_from = (struct sockaddr_in *) &from;
    si_from->sin_addr.s_addr =
            (in_addr_t)ip4_addr_get_u32(ip_2_ip4(&p_this->ppp.netif.ip_addr));

    dest_addr = si_to->sin_addr;

    if (!IP_IS_V4(&p_this->ppp.netif.ip_addr) ||
              si_to->sin_addr.s_addr == IPADDR_ANY ||
              si_from->sin_addr.s_addr == IPADDR_ANY) {
        __RETURN_EMSG(-AW_EADDRNOTAVAIL, NULL);
    }
#endif /* AWBL_GPRS_USE_LWIP_2_X */

#if LWIP_ROUTE_TABLE
    extern struct netif *netif_route_find (ip_addr_t *p_ipaddr);
    extern void netif_route_add (struct netif *netif, ip_addr_t *p_ipaddr);
    extern int netif_route_change (ip_addr_t *p_old_ipaddr,
                                   ip_addr_t *p_new_ipaddr,
                                   struct netif *new_netif);


    if (netif_route_find((ip_addr_t *)&dest_addr) != NULL) {
        netif_route_change((ip_addr_t *)&dest_addr, (ip_addr_t *)&dest_addr, netif);
    } else {
        netif_route_add(netif, (ip_addr_t *)&dest_addr);
    }
#endif

    for (i = 0; i < 4; i++) {
#if AWBL_GPRS_USE_LWIP_1_4
        ret = aw_ping(&ping, &dest_addr, NULL, 96, 0, 8, 2000);
#else
        ttl = 96;
        ret = aw_ping(&ping, &to, &from, &ttl, 8, 2000);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */
        if (ret >= 0) {
            break;
        }
    }

    __LOG_RES(ret >= 0, AW_GPRS_LOG_NET_PING, "gprs dev ping \"%s\" ",
              inet_ntoa(dest_addr));
    if (ret < 0) {
        __RETURN_EMSG(-AW_GPRS_EPINGTM, NULL);
    }
    return ret;
}

/* tag:sts */

/**
 * 返回 AW_OK 时,表明 p_sts 中的值为效(指明 OK 或具体错误);
 * 返回其他值则 p_sts 值无效,查询失败
 */
aw_local aw_err_t __gprs_status_check (struct awbl_gprs_dev *p_this)
{
    aw_err_t     ret;
    char        *arg[1];
    char         buf[256];
    aw_tick_t    tick;

    /* 旧状态是 ok,则直接检测 ping 过程 */
    if (p_this->status == AW_OK) {
        tick = aw_sys_tick_get();

        /* ok 后间隔 6s*/
        if ((tick - p_this->last_status_tick) <= aw_ms_to_ticks(6000)) {
            return AW_OK;
        } else {
            if (p_this->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
                p_this->status = AW_OK;
                goto ping_check;
            }
        }
    }

    memset(buf, 0, sizeof(buf));
    /* 测试 AT 命令 */
    __TIMEOUT_MSEC(tick, 3000) {
        aw_mdelay(500);
        p_this->drvopr.pfn_at_send(p_this, "AT\r");

        arg[0] = "OK";
        ret = p_this->drvopr.pfn_at_recv(p_this, buf, sizeof(buf), arg, 1,
                                         1000);
        if (ret < AW_OK) {
            p_this->status = AW_GPRS_EAT;
            continue;
        }
        p_this->status = AW_OK;
        break;
    }

    if (p_this->status != AW_OK) {
        return AW_OK;
    }

    p_this->status = p_this->drvopr.pfn_cpin_chk(p_this);
    if (p_this->status != AW_OK) {
        return AW_OK;
    }

    /* 检查 APN */
    p_this->status = p_this->drvopr.pfn_apn_get(p_this);
    aw_assert(p_this->status == AW_OK || p_this->status == -AW_GPRS_EAPN);
    if (p_this->status != AW_OK) {
        return AW_OK;
    }

#if 0
    /* 检查信号强度 */
    __TIMEOUT_MSEC(tick, 10000) {
        int      csq;

        aw_mdelay(1000);
        ret = p_this->drvopr.pfn_csq_get(p_this, &csq);
        if (ret != AW_OK || csq < p_this->csq_min) {
            p_this->status = AW_GPRS_STS_CSQ_ERROR;
        } else {
            p_this->status = AW_OK;
            break;
        }
    }

    if (p_this->status != AW_OK) {
        __LOG_RES(AW_FALSE, 1, "status: AW_GPRS_STS_CSQ_ERROR");
        return AW_OK;
    }
#endif

    /* 检查 ppp、csq、cereg  */
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        p_this->status = __gprs_ppp_open(p_this);
        /* ppp 拨号成功，则所有状态正常，返回 */
        if (p_this->status == AW_OK) {
            goto exit;
        }
    }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /* 检查 eth */
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        p_this->status = __gprs_ethernet_open(p_this);
        /* eth 拨号成功，则所有状态正常，返回 */
        if (p_this->status == AW_OK) {
            goto exit;
        }
    }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    aw_assert(p_this->status == AW_OK || p_this->status < (-AW_GPRS_EBASE));
    return AW_OK;

    /* ping dns1 */
ping_check:
    if (0) {
        int ms;

        ms = __ping_check(p_this);
        if (ms >= 0) {
            p_this->status = AW_OK;
        } else {
            p_this->status = ms;
            return AW_OK;
        }
    }

exit:
    aw_assert(p_this->status == AW_OK || p_this->status < (-AW_GPRS_EBASE));
    p_this->last_status_tick = aw_sys_tick_get();
    return AW_OK;
}
/*****************************************************************************/
static aw_err_t __init_step_power_on (struct awbl_gprs_dev    *p_this)
{
    /* If the reset flag is valid, it exits immediately and the reset interface
     * is called in the off step. */
    if (p_this->sf_reset || p_this->hw_reset) {
        return AW_OK;
    }

    if (p_this->drvopr.pfn_hw_power_on) {
        p_this->drvopr.pfn_hw_power_on();
    }
    if (p_this->drvopr.pfn_hw_write_en) {
        p_this->drvopr.pfn_hw_write_en();
    }
    return AW_OK;
}

static aw_err_t __init_step_power_off (struct awbl_gprs_dev    *p_this)
{
    if (p_this->hw_reset) {
        if (p_this->drvopr.pfn_hw_reset) {
            p_this->drvopr.pfn_hw_reset();
        }
        return AW_OK;
    }

    if (p_this->drvopr.pfn_hw_power_off) {
        p_this->drvopr.pfn_hw_power_off();
    }

    return AW_OK;
}

static aw_err_t __init_step_pipe (struct awbl_gprs_dev    *p_this)
{
    aw_err_t     ret;
    uint8_t      phase = 0;

    aw_assert(p_this->sms.p_sms_buff && p_this->usbh_info.at_com >= 0);

    ret = p_this->drvopr.pfn_init_prev(p_this);
    if (ret) {
        return ret;
    }

    ret = p_this->drvopr.pfn_ll_type_get(p_this);
    aw_assert(ret == AW_OK);
    aw_assert(p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH ||
              p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP);

    /* Open the AT pipeline.  */
    p_this->at_pipe = p_this->drvopr.pfn_pipe_open(p_this->usbh_info.pid,
                                                   p_this->usbh_info.vid,
                                                   p_this->usbh_info.at_com,
                                                   AW_TRUE, 512, 90000);
    if (p_this->at_pipe == NULL) {
        if (p_this->pipe_valid == 0) {
            ret = -AW_GPRS_EUSBH;
        } else {
            ret = -AW_ESPIPE;
        }
        __EXP_EMSG(NULL, ret, NULL);
        goto exit;
    }
    __LOG_PFX(AW_GPRS_LOG_PIPE, "Open the AT pipeline of %s successfully.(%d)",
              p_this->info.devname, p_this->usbh_info.at_com);
    awbl_gprs_snd_pipe_install(p_this, p_this->at_pipe);
    phase = 1;



#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        ret = p_this->drvopr.pfn_eth_mode_init(p_this);
        if (ret) {
            goto exit;
        }
    }
#endif  /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        ret = p_this->drvopr.pfn_ppp_mode_init(p_this);
        if (ret) {
            goto exit;
        }
    }
    return AW_OK;

exit:
    if (phase == 1) {
        p_this->drvopr.pfn_pipe_close(p_this->at_pipe);
    }
    return ret;
}


static aw_err_t __uninit_step_pipe (struct awbl_gprs_dev    *p_this)
{

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        p_this->drvopr.pfn_eth_mode_uninit(p_this);
    }
#endif  /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        p_this->drvopr.pfn_ppp_mode_uninit(p_this);
    }

    if(p_this->gnss_pipe) {
        p_this->drvopr.pfn_pipe_close(p_this->gnss_pipe);
        p_this->gnss_pipe = NULL;
    }

    if (p_this->at_pipe) {
        if (p_this->sf_reset) {
            uint32_t ms = p_this->info.sf_reset_mdelay;
            p_this->drvopr.pfn_reset_by_at(p_this);
            while (ms > 0) {
                if (p_this->pipe_valid == 0) {
                    break;
                }
                aw_mdelay(100);
                ms -= min(ms, 100);
            }
            if (p_this->pipe_valid == 0) {
                __LOG_PFX(AW_GPRS_LOG_SYS,
                          "The device took %d ms to eject successfully.",
                          p_this->info.sf_reset_mdelay - ms);
            } else {
                __LOG_PFX(AW_GPRS_LOG_ERROR,
                          "Failed to wait for device ejected.(ms:%d)",
                          p_this->info.sf_reset_mdelay);
            }
        }
        p_this->drvopr.pfn_pipe_close(p_this->at_pipe);
        p_this->at_pipe = NULL;
    }
    return AW_OK;
}

static aw_err_t __init_step_param (struct awbl_gprs_dev    *p_this)
{
    p_this->ppp.phase         = AWBL_GPRS_PPP_PHASE_UNINIT;

    /* 默认使用外部协议栈 */
    if (PPP_SUPPORT || p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        p_this->use_external_protocol = AW_TRUE;
    } else {
        p_this->use_external_protocol = AW_FALSE;
    }

    aw_assert(p_this->socket_num == 0);

    if (p_this->csq_min == 0) {
        p_this->csq_min  = 10;
    }

    if (p_this->pfn_log_out == NULL) {
        p_this->pfn_log_out =__4g_log_handler;
    }

    /* Initializes the type value of last_evt, whick will not be notified. */
    p_this->last_evt.type = AW_GPRS_EVT_TYPE_NONE;

    p_this->gnss_en        = AW_FALSE;
    p_this->status         = AW_GPRS_EAT;
    p_this->htts_init_step = 0;
    p_this->info.p_apn     = NULL;

    return AW_OK;
}


static aw_err_t __init_step_end (struct awbl_gprs_dev    *p_this)
{
    aw_gprs_evt_t        evt;

    if (p_this->sf_reset || p_this->hw_reset) {
        evt.type = AW_GPRS_EVT_TYPE_RESET_COMPLETED;
        __evt_cb_array_notify(p_this, &evt);
    } else {
        evt.type = AW_GPRS_EVT_TYPE_PWR_ON;
        __evt_cb_array_notify(p_this, &evt);
    }

    p_this->init_completed = AW_TRUE;
    return AW_OK;
}

static aw_err_t __uninit_step_start (struct awbl_gprs_dev    *p_this)
{
    aw_gprs_evt_t        evt;

    if (p_this->sf_reset || p_this->hw_reset) {
        /* reset */
    } else {
        /* power off */

        /* 上报 PWR_OFF 事件 */
        evt.type = AW_GPRS_EVT_TYPE_PWR_OFF;
        __evt_cb_array_notify(p_this, &evt);
    }

    p_this->init_completed = AW_FALSE;
    return AW_OK;
}

static aw_err_t __init_step_at_cfg (struct awbl_gprs_dev    *p_this)
{
    aw_err_t     err;

    __LOG_PFX(AW_GPRS_LOG_SYS, "Delay %d ms for device initialization...",
              p_this->info.power_on_mdelay);
    aw_mdelay(p_this->info.power_on_mdelay);
    err = p_this->drvopr.pfn_init_at_cfg(p_this);
    if (err ) {
        return err;
    }

#if 0
    if (!p_this->info.p_apn) {
        p_this->drvopr.pfn_apn_get(p_this);
    }
#endif
    return AW_OK;
}
/*****************************************************************************/
struct __init_cb {
    aw_err_t (*pfn_init) (struct awbl_gprs_dev    *p_this);
    aw_err_t (*pfn_uninit) (struct awbl_gprs_dev    *p_this);
};

static struct __init_cb __g_init_step[] = {
    {__init_step_power_on,          __init_step_power_off},
#if 0
    {awbl_gprs_default_init,        NULL},
    {awbl_gprs_default_drvopr_set,  NULL},
#endif
    {__init_step_pipe,              __uninit_step_pipe},
    {__init_step_param,             NULL},
    {__init_step_at_cfg,            NULL},
    {__init_step_end,               __uninit_step_start},
    {NULL,                          NULL}
};


static aw_err_t __power_on (struct awbl_gprs_dev    *p_this)
{
    aw_err_t             ret;

    if (p_this->init_completed == AW_TRUE) {
        __RETURN_EMSG(-AW_EEXIST, NULL);
    }

    for (int i = p_this->init_step; __g_init_step[i].pfn_init != NULL; i++) {
        aw_assert(__g_init_step[i].pfn_init != NULL);
        ret = __g_init_step[i].pfn_init(p_this);
        if (ret == AW_OK) {
            p_this->init_step++;
        } else {
            goto exit;
        }
    }

    return AW_OK;

exit:
    if (p_this->init_step == 0) {
        return AW_OK;
    }
    for (int i = p_this->init_step - 1; i >= 0; i--) {
        if (__g_init_step[i].pfn_uninit != NULL) {
            __g_init_step[i].pfn_uninit(p_this);
        }
        p_this->init_step--;
    }
    aw_assert(p_this->init_step == 0);
    return ret;
}

static aw_err_t __power_off (struct awbl_gprs_dev    *p_this)
{
    aw_assert(p_this->flag_check == 0);

    if (p_this->init_step == 0) {
        if (p_this->drvopr.pfn_hw_power_off) {
            p_this->drvopr.pfn_hw_power_off();
        }
        return AW_OK;
    }

    /* power off 时，只执行操作，不关心返回值，防止设备被移除时 at 命令全部失败，导致无法 power off. */
#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        __gprs_ethernet_close(p_this);
    }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        __gprs_ppp_close(p_this);
    }

    for (int i = p_this->init_step - 1; i >= 0; i--) {
        if (__g_init_step[i].pfn_uninit != NULL) {
            __g_init_step[i].pfn_uninit(p_this);
        }
        p_this->init_step--;
    }
    aw_assert(p_this->init_step == 0);
    return AW_OK;
}

static aw_err_t __reset (struct awbl_gprs_dev   *p_this,
                         aw_bool_t               hw_reset,
                         uint32_t                ms)
{
    aw_err_t     err;

    /* 使能复位标志，off 中的  power_off 接口会替换为 reset 接口 */
    if (hw_reset) {
        p_this->hw_reset = 1;
        if (!p_this->drvopr.pfn_hw_reset) {
            __RETURN_EMSG(-AW_ENOTSUP, NULL);
        }
    } else {
        p_this->sf_reset = 1;
        if (!p_this->drvopr.pfn_reset_by_at) {
            __RETURN_EMSG(-AW_ENOTSUP, NULL);
        }
    }

    __power_off(p_this);
    __LOG_PFX(AW_GPRS_LOG_DBG, "Delay %d ms for software reset...", ms);
    aw_mdelay(ms);
    err = __power_on(p_this);
    p_this->sf_reset = 0;
    p_this->hw_reset = 0;
    return err;
}

/*****************************************************************************/

static aw_err_t __https_cb_check (struct awbl_gprs_dev *p_this)
{
    if (!p_this->drvopr.pfn_https_ctx_init ||
            !p_this->drvopr.pfn_https_ctx_act ||
            !p_this->drvopr.pfn_https_ctx_ssl_act ||
            !p_this->drvopr.pfn_https_url_dsize_get ||
            p_this->drvopr.pfn_https_url_read) {
        __RETURN_EMSG(-AW_ENOTSUP, NULL)
    }
    return AW_OK;
}
static aw_err_t __https_ctx_init (struct awbl_gprs_dev *p_this)
{
    return p_this->drvopr.pfn_https_ctx_init(p_this);
}
static aw_err_t __https_ctx_act (struct awbl_gprs_dev *p_this)
{
    return p_this->drvopr.pfn_https_ctx_act(p_this);
}
static aw_err_t __https_ctx_ssl_act (struct awbl_gprs_dev *p_this)
{
    return p_this->drvopr.pfn_https_ctx_ssl_act(p_this);
}

static struct __init_cb __g_htts_init_step[] = {
    {__https_cb_check, NULL},
    {__https_ctx_init, NULL},
    {__https_ctx_act, NULL},
    {__https_ctx_ssl_act, NULL},
    {NULL, NULL},
};

/*****************************************************************************/
typedef enum {
    __REQTYPE_POWER_ON,
    __REQTYPE_POWER_OFF,
    __REQTYPE_IOCTL,
    __REQTYPE_START,
    __REQTYPE_SMS_SEND,
    __REQTYPE_SMS_RECV,
} __reqtype_t;

typedef struct {
    __reqtype_t              reqtype;
    struct awbl_gprs_dev    *p_this;
    aw_gprs_result_t        *p_result;

    union {
        struct {
            aw_gprs_ioctl_req_t      req;
            aw_gprs_ioctl_args_t    *p_arg;
        } ioctl;

        struct {
            const char         *p_mode;
            char               *p_ip;
            uint16_t            port;
            uint32_t            timeout;
            int                *p_hd;
        } start;

        struct {
            uint8_t             *p_sms;
            uint8_t              len;
            char                *p_num;
            uint8_t              sms_code;
        } sms_send;
        struct {
            struct aw_gprs_sms_buf  *p_sms;
            uint8_t                  sms_code;
        } sms_recv;
    } u;
} __reqsched_args_t;

static void __result_init (aw_gprs_result_t *p_result)
{
    aw_assert(p_result);
    p_result->completed = AW_FALSE;
    p_result->errcode   = AW_OK;
    p_result->errdesc   = NULL;
}


static void __reqsched_hdr (uint32_t reqid, void *p_data, size_t size)
{
    __reqsched_args_t       *p_reqargs = p_data;
    struct awbl_gprs_dev    *p_this = p_reqargs->p_this;
    aw_err_t                 err;

    aw_assert(p_reqargs);

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    switch (p_reqargs->reqtype) {
        case __REQTYPE_POWER_ON: {
            err = __power_on(p_this);
            if (!err) {
                __LOG_RES(err == AW_OK, AW_GPRS_LOG_API,
                          "GPRS(%d) device power on.", p_this->dev_id);
            }
        } break;
        case __REQTYPE_POWER_OFF: {
            err = __power_off(p_this);
            __LOG_RES(err == AW_OK, AW_GPRS_LOG_API,
                      "GPRS(%d) device power off.", p_this->dev_id);
        } break;
        case __REQTYPE_IOCTL: {
            err = __ioctl_without_stat(p_this,
                                       p_reqargs->u.ioctl.req,
                                       p_reqargs->u.ioctl.p_arg);
            if (err == AW_OK || err != -AW_ENOTSUP) {
                break;
            }
            err = __ioctl(p_this,
                          p_reqargs->u.ioctl.req,
                          p_reqargs->u.ioctl.p_arg);
        } break;

        case __REQTYPE_START: {
            *p_reqargs->u.start.p_hd = __start(p_this,
                                               p_reqargs->u.start.p_mode,
                                               p_reqargs->u.start.p_ip,
                                               p_reqargs->u.start.port,
                                               p_reqargs->u.start.timeout);
        } break;

        case __REQTYPE_SMS_SEND: {
            err = __sms_send(p_this,
                             p_reqargs->u.sms_send.p_sms,
                             p_reqargs->u.sms_send.len,
                             p_reqargs->u.sms_send.p_num,
                             p_reqargs->u.sms_send.sms_code);
        } break;
        case __REQTYPE_SMS_RECV: {
            err = __sms_recv(p_this,
                             p_reqargs->u.sms_recv.p_sms,
                             p_reqargs->u.sms_recv.sms_code);
        } break;
        default: {
            err = -AW_ENOTSUP;
        } break;
    }
    AW_MUTEX_UNLOCK(p_this->mutex);

    p_reqargs->p_result->errcode   = err;
    p_reqargs->p_result->errdesc   = err ? __ERRCODE(err) : NULL;
    p_reqargs->p_result->completed = 1;

    err = reqsched_rsp_send(reqid, p_data);
    aw_assert(err == RS_OK);
    err = reqsched_rsp_tryget(reqid, REQSCHED_DATA_TO_TOKEN(p_data), p_data,
                              size);
    aw_assert(err == RS_OK);
    aw_assert(p_this->token);
    p_this->token = 0;
}

aw_err_t awbl_gprs_snd_pipe_install (struct awbl_gprs_dev *p_this,
                                     void                 *p_pipe)
{
    aw_assert(p_this && p_pipe);
    p_this->snd_pipe = p_pipe;
    return AW_OK;
}

/*****************************************************************************/

aw_err_t aw_gprs_power_on (int id)
{
    struct awbl_gprs_dev    *p_this = __gprs_id_to_dev(id);
    aw_err_t                 ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    if (p_this->token) {
        /* Wait unti the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    /* Initialize the REQSCHED. */
    if (!p_this->reqid) {
        aw_reqschd_lib_init();
        p_this->reqid = aw_reqsched_create(__reqsched_hdr,
                                           1,
                                           sizeof(__reqsched_args_t));
        aw_assert(p_this->reqid);
    }

    ret = __power_on(p_this);
    if (ret == AW_OK) {
        __LOG_RES(1, AW_GPRS_LOG_API, "GPRS(%d) device power on.", id);
    }
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_power_on_asyn (int id, aw_gprs_result_t *p_result)
{
    struct awbl_gprs_dev    *p_this = __gprs_id_to_dev(id);
    aw_err_t                 ret;
    __reqsched_args_t        reqargs;

    if (p_this == NULL || !p_result) {
        return -AW_EINVAL;
    }

    if (p_this->token) {
        /* Return busy directly. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    /* Initialize the REQSCHED. */
    if (!p_this->reqid) {
        aw_reqschd_lib_init();
        p_this->reqid = aw_reqsched_create(__reqsched_hdr,
                                           1,
                                           sizeof(__reqsched_args_t));
        aw_assert(p_this->reqid);
    }

    __result_init(p_result);

    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.p_this   = p_this;
    reqargs.p_result = p_result;
    reqargs.reqtype  = __REQTYPE_POWER_ON;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);

    AW_MUTEX_UNLOCK(p_this->mutex);

    return ret;
}

aw_err_t aw_gprs_power_off (int   id)
{
    struct awbl_gprs_dev    *p_this = __gprs_id_to_dev(id);
    aw_err_t                 ret;
    aw_gprs_evt_t            evt;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    /* 上报 PWR_OFF_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
    evt.type = AW_GPRS_EVT_TYPE_PWR_OFF_PRE;
    ret = __evt_cb_array_notify(p_this, &evt);
    if (ret != AW_OK) {
        __LOG_PFX(AW_GPRS_LOG_ERROR,
                  "Power off failed because some devices were not allowed!");
        goto exit;
    }

    /* 需要关闭socket和ppp */
    if (p_this->socket_num != 0) {
        __GOTO_EMSG(exit, -AW_EISCONN, ". (num:%d)", p_this->socket_num);
    }

    ret = __power_off(p_this);
    __LOG_RES(ret == AW_OK, AW_GPRS_LOG_API,
              "GPRS(%d) device power off.", id);

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_power_off_asyn (int   id, aw_gprs_result_t *p_result)
{
    struct awbl_gprs_dev    *p_this = __gprs_id_to_dev(id);
    aw_err_t                 ret;
    aw_gprs_evt_t            evt;
    __reqsched_args_t        reqargs;

    if (p_this == NULL || !p_result) {
        return -AW_EINVAL;
    }

    if (p_this->token) {
        /* Return busy directly if asynchronous. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    /* 上报 PWR_OFF_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
    evt.type = AW_GPRS_EVT_TYPE_PWR_OFF_PRE;
    ret = __evt_cb_array_notify(p_this, &evt);
    if (ret != AW_OK) {
        __LOG_PFX(AW_GPRS_LOG_ERROR,
                  "Power off failed because some devices were not allowed!");
        goto exit;
    }

    /* 需要关闭socket和ppp */
    if (p_this->socket_num != 0) {
        __GOTO_EMSG(exit, -AW_EISCONN, ". (num:%d)", p_this->socket_num);
    }

    __result_init(p_result);

    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.p_this   = p_this;
    reqargs.p_result = p_result;
    reqargs.reqtype  = __REQTYPE_POWER_OFF;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

int aw_gprs_start (int                 id,
                   const char         *p_mode,
                   char               *p_ip,
                   uint16_t            port,
                   uint32_t            timeout)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    int                   hd;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    hd = __start(p_this, p_mode, p_ip, port, timeout);
    AW_MUTEX_UNLOCK(p_this->mutex);
    return hd;
}

int  aw_gprs_start_asyn (int                 id,
                         const char         *p_mode,
                         char               *p_ip,
                         uint16_t            port,
                         uint32_t            timeout,
                         int                *p_hd,
                         aw_gprs_result_t   *p_result)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret;
    __reqsched_args_t     reqargs;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Return busy directly if asynchronous. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    __result_init(p_result);
    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.reqtype         = __REQTYPE_IOCTL;
    reqargs.p_this          = p_this;
    reqargs.p_result        = p_result;
    reqargs.u.start.p_hd    = p_hd;
    reqargs.u.start.p_ip    = p_ip;
    reqargs.u.start.p_mode  = p_mode;
    reqargs.u.start.port    = port;
    reqargs.u.start.timeout = timeout;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);

    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_close(int   id, int    hd)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        __GOTO_EMSG(exit, -AW_EACCES, NULL);
    }

    /* 使用外部协议栈 */
    if (p_this->use_external_protocol) {
        closesocket(hd);
        if (p_this->socket_num > 0) {
            p_this->socket_num--;
        }
        if (p_this->socket_num == 0) {

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
            if (p_this->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
                __gprs_ethernet_close(p_this);
            }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

            if (p_this->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
                __gprs_ppp_close(p_this);
            }
        }
        ret = AW_OK;
    /* 使用内部协议栈 */
    } else {
        if (p_this->drvopr.pfn_tcpudp_disconn) {
            ret = p_this->drvopr.pfn_tcpudp_disconn(p_this, hd);
        } else {
            __GOTO_EMSG(exit, -AW_ENOTSUP, NULL);
        }
    }
exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

ssize_t aw_gprs_send (int                 id,
                      int                 hd,
                      void               *p_data,
                      size_t              len,
                      uint32_t            timeout)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    ssize_t ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        __GOTO_EMSG(exit, -AW_EACCES, NULL);
    }

    if (p_this->use_external_protocol) {
        ret = send(hd, p_data, len, 0);
    } else {
        if (p_this->drvopr.pfn_tcpudp_send) {
            ret = p_this->drvopr.pfn_tcpudp_send(p_this, hd, p_data, len,
                                                 timeout);
        } else {
            ret = -AW_ENOTSUP;
            goto exit;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}


ssize_t aw_gprs_recv (int                 id,
                      int                 hd,
                      void               *p_buf,
                      size_t              len)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    ssize_t ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        __GOTO_EMSG(exit, -AW_EACCES, NULL);
    }

    memset(p_buf, 0, len);
    if (p_this->use_external_protocol) {
        ret = recv(hd, p_buf, len, 0);
    } else {
        if (p_this->drvopr.pfn_tcpudp_recv) {
            ret = p_this->drvopr.pfn_tcpudp_recv(p_this, hd, p_buf, len);
        } else {
            ret = -AW_ENOTSUP;
            goto exit;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_send  (int                 id,
                            uint8_t            *p_sms,
                            uint8_t             len,
                            char               *p_num,
                            uint8_t             sms_code)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    ret = __sms_send(p_this, p_sms, len, p_num, sms_code);
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_send_asyn (int                  id,
                                uint8_t             *p_sms,
                                uint8_t              len,
                                char                *p_num,
                                uint8_t              sms_code,
                                aw_gprs_result_t    *p_result)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    __reqsched_args_t     reqargs;

    if (!p_this || !p_result) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Return busy directly if asynchronous. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    __result_init(p_result);
    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.reqtype             = __REQTYPE_SMS_SEND;
    reqargs.p_this              = p_this;
    reqargs.p_result            = p_result;
    reqargs.u.sms_send.p_sms    = p_sms;
    reqargs.u.sms_send.len      = len;
    reqargs.u.sms_send.p_num    = p_num;
    reqargs.u.sms_send.sms_code = sms_code;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);

    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_recv  (int                      id,
                            struct aw_gprs_sms_buf  *p_sms,
                            uint8_t                  sms_code)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    ret = __sms_recv(p_this, p_sms, sms_code);
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_recv_asyn (int                      id,
                                struct aw_gprs_sms_buf  *p_sms,
                                uint8_t                  sms_code,
                                aw_gprs_result_t        *p_result)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    __reqsched_args_t     reqargs;

    if (!p_this || !p_result) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Return busy directly if asynchronous. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    __result_init(p_result);
    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.reqtype             = __REQTYPE_SMS_RECV;
    reqargs.p_this              = p_this;
    reqargs.p_result            = p_result;
    reqargs.u.sms_recv.p_sms    = p_sms;
    reqargs.u.sms_recv.sms_code = sms_code;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_all_sms_del(int id)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t ret;
    int      index;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    while (1) {
        ret = p_this->drvopr.pfn_sms_first_index_get(p_this, &index);
        if (ret) {
            break;
        }
        ret = p_this->drvopr.pfn_sms_del(p_this, index);
        if (ret) {
            break;
        }
    }
    AW_MUTEX_UNLOCK(p_this->mutex);
    return AW_OK;
}


aw_err_t aw_gprs_https_get (int id,
                            char    *p_url,
                            size_t   url_len,
                            void (*p_response_cb) (void *rdata,
                                                   size_t rsize))
{
    aw_err_t              err;
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    struct __init_cb     *cb     = __g_htts_init_step;
    size_t                url_dsize;
    size_t                remain;
    size_t                rlen;
    size_t                len;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }

    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    for (int i = p_this->htts_init_step; cb[i].pfn_init != NULL; i++) {
        aw_assert(cb[i].pfn_init);
        err = cb[i].pfn_init(p_this);
        if (err) {
            goto exit;
        }
        p_this->htts_init_step++;
    }
    err = p_this->drvopr.pfn_https_url_dsize_get(p_this, p_url, url_len,
                                                 &url_dsize);
    if (err) {
        goto exit;
    }
    remain = url_dsize;

    do {
        rlen = min(remain, p_this->at_bufsz);
        len = p_this->drvopr.pfn_https_url_read(p_this,
                                                (uint8_t *)p_this->at_buf,
                                                rlen);
        if (len > 0) {
            remain -= len;
            p_response_cb(p_this->at_buf, len);
        } else if (len < 0) {
            goto exit;
        }
    } while (remain);

    if (remain == 0) {
        p_response_cb(p_this->at_buf, 0);  /* 长度为0 接收完成 */
    }

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return err;
}

aw_err_t aw_gprs_ioctl (int                   id,
                        aw_gprs_ioctl_req_t   req,
                        aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;

    if (!p_this || !p_arg) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    ret = __ioctl_without_stat(p_this, req, p_arg);
    if (ret == AW_OK || ret != -AW_ENOTSUP) {
        goto exit;
    }
    ret = __ioctl(p_this, req, p_arg);

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}


aw_err_t aw_gprs_ioctl_asyn (int                   id,
                             aw_gprs_ioctl_req_t   req,
                             aw_gprs_ioctl_args_t *p_arg,
                             aw_gprs_result_t     *p_result)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    __reqsched_args_t     reqargs;

    if (!p_this || !p_result) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Return busy directly if asynchronous. */
        __RETURN_EMSG(-AW_EBUSY, NULL);
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);

    __result_init(p_result);

    aw_assert(aw_reqsched_req_available(p_this->reqid));
    reqargs.reqtype       = __REQTYPE_IOCTL;
    reqargs.p_this        = p_this;
    reqargs.p_result      = p_result;
    reqargs.u.ioctl.req   = req;
    reqargs.u.ioctl.p_arg = p_arg;
    ret = aw_reqsched_req_send(p_this->reqid, &reqargs, sizeof(reqargs),
                               &p_this->token);
    aw_assert(ret == AW_OK && p_this->token > 0);

    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

int aw_gprs_any_get (uint32_t timeout_sec)
{
    aw_local int            id = -1;
    aw_tick_t               tick_tmp;
    struct awbl_gprs_dev    *p_this;

    if (id >= 0) {
        p_this = __gprs_id_to_dev(id);
        if (p_this->init_step > 0) {
            return id;
        } else {
            id = -1;
        }
    }

    if (timeout_sec < 1) {
        timeout_sec = 1;
    }

    __TIMEOUT_SEC(tick_tmp, timeout_sec) {
        aw_mdelay(50);
        p_this = __gp_this_dev_head;
        while ((p_this != NULL)) {
            if (p_this->init_step > 0) {
                id = p_this->dev_id;
                return id;
            }
            if (p_this->drvopr.pfn_probe) {
                if (p_this->drvopr.pfn_probe(p_this) == AW_OK) {
                    id = p_this->dev_id;
                    return id;
                }
            }
            p_this = p_this->p_next;
        }
    }

    return -1;
}

/*******************************************************************************
 * GNSS
 ******************************************************************************/
aw_err_t aw_gprs_gnss_enable (int dev_id, aw_gprs_gnss_cfg_t *p_gnss_cfg)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;
    aw_gprs_ioctl_args_t  ioctl_args;

    if (!p_this || !p_gnss_cfg) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        ret = -AW_EPERM;
        goto exit;
    }
    if (p_this->gnss_en == AW_TRUE) {
        /* gnss 已被使能 */
        __GOTO_EMSG(exit, -AW_EEXIST, NULL);
    }

    /* 添加回调，失败则退出   */
    if (p_gnss_cfg->pfn_evt_cb) {
        ioctl_args.pfn_evt_handler = p_gnss_cfg->pfn_evt_cb;
        ret = aw_gprs_ioctl(dev_id, AW_GPRS_EVT_CB_SET, &ioctl_args);
        if (ret != AW_OK) {
            goto exit;
        }
    }

    if (p_this->drvopr.pfn_gnss_enable) {
        ret = p_this->drvopr.pfn_gnss_enable(p_this, p_gnss_cfg);
        if (ret) {
            goto exit;
        }
        if (!p_this->gnss_pipe) {
            p_this->gnss_pipe = p_this->drvopr.pfn_pipe_open(
                                                p_this->usbh_info.pid,
                                                p_this->usbh_info.vid,
                                                p_this->usbh_info.gnss_com,
                                                AW_TRUE, 512, 15000);
            if (!p_this->gnss_pipe) {
                ret = -AW_EPIPE;
                __EXP_EMSG(NULL, ret, NULL);
            } else {
                ret = AW_OK;
            }
        }
    } else {
        ret = -AW_ENOTSUP;
    }

    if (ret == AW_OK) {
        p_this->gnss_en = AW_TRUE;
    }

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_gnss_disable (int dev_id)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        __GOTO_EMSG(exit, -AW_EACCES, NULL);
    }
    if (p_this->gnss_en == AW_FALSE) {
        ret = AW_OK;
        goto exit;
    }

    if (p_this->drvopr.pfn_gnss_disable) {
        ret = p_this->drvopr.pfn_gnss_disable(p_this);
        if (ret) {
            goto exit;
        }
        if (p_this->gnss_pipe) {
            p_this->drvopr.pfn_pipe_close(p_this->gnss_pipe);
            p_this->gnss_pipe = NULL;
        }

    } else {
        ret = -AW_ENOTSUP;
    }
    if (ret == AW_OK) {
        p_this->gnss_en = AW_FALSE;
    }
exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}

aw_err_t aw_gprs_gnss_nmeasrc_get (int      dev_id,
                                   uint8_t *p_buf,
                                   size_t   bufsize,
                                   uint32_t timeout)
{
    struct awbl_gprs_dev *p_this = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }
    if (p_this->token) {
        /* Wait until the completion of the asynchronous operation. */
        while (p_this->token) {
            aw_mdelay(50);
        }
    }
    AW_MUTEX_LOCK(p_this->mutex, AW_SEM_WAIT_FOREVER);
    if (!p_this->init_completed) {
        __GOTO_EMSG(exit, -AW_EACCES, NULL);
    }
    if (p_this->gnss_en == AW_FALSE) {
        __GOTO_EMSG(exit, -AW_GPRS_EGNSS, NULL);
    }
    if (p_this->drvopr.pfn_gnss_nmeasrc_get) {
        ret = p_this->drvopr.pfn_gnss_nmeasrc_get(p_this, p_buf, bufsize,
                                                  timeout);
    } else {
        __GOTO_EMSG(exit, -AW_ENOTSUP, NULL);
    }

exit:
    AW_MUTEX_UNLOCK(p_this->mutex);
    return ret;
}




/* end of file*/

