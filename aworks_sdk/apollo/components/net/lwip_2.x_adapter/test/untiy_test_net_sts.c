/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#include "aworks.h"
#include <stdlib.h>
#include <string.h>

#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"

#include "lwip/ext/net_sts.h"
#include "lwip/netifapi.h"
#include "net_sts_internal.h"

#include "aw_netif.h"
#include "aw_ping.h"
#include "aw_shell.h"
#include "aw_gprs.h"

#include "unity.h"
#include "unity_fixture.h"

/******************************************************************************/
#define PFX "TEST_NET_STS"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
         __log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __LOG(mask, fmt, ...) __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)


#define __LOG_NET_STS           1
#define __MASK_NET_PING         1
/******************************************************************************/
#define __TEST_EXE(func, arg)  \
    do {\
        aw_err_t ret;\
        __LOG(1, "\n======================================================\n");\
        __LOG(1, "startup TEST: %s\n\n", #func);\
        ret = func(arg);\
        if (ret == AW_OK) {\
            __LOG_BASE(1, "\x1B[38;5;40m", "\n\n[ OK ] TEST %s Success.\n", #func);\
        } else {\
            __LOG_BASE(1, "\x1B[31m", "\n\n[ ERROR ] TEST %s failed!\n", #func);\
            aw_mdelay(1000);\
            aw_assert(ret == AW_OK);\
        }\
        __LOG(1, "******************************************************\n");\
    } while (0)
/******************************************************************************/
/* Check to see if it has timed out. */
#define __IS_TIMEOUT_MS(start_tick, ms) \
    (aw_ticks_to_ms(aw_sys_tick_get() - start_tick) >= ms )

#define __TIMEOUT_MSEC(tick_tmp, timeout) \
    for (tick_tmp = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_tmp; )

#define __TIMEOUT_MSEC_REMAIN(tick_tmp) \
    aw_ticks_to_ms(tick_tmp - aw_sys_tick_get())

/* Caculate the MS interval time of current tick and given tick. */
#define __INTERVAL_MS_GET(tick)  aw_ticks_to_ms(aw_sys_tick_get() - (tick))

#define __NETIF_NAME_FMT(p_netif)  p_netif->name[0], p_netif->name[1], p_netif->num
/******************************************************************************/
/*
 * Defines the macro to determine the event type.
 */

#define __EVT_TYPE_IS_ANY_LINK_OK(p_evt) ((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_INET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_SNET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET)

#define __EVT_TYPE_IS_LINK_OK(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_INET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_SNET))

#define __EVT_TYPE_IS_FORCE_LINK_OK(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET))


#define __EVT_TYPE_IS_ANY_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_DOWN ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR || \
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR))

#define __EVT_TYPE_IS_FORCE_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR || \
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR))

#define __EVT_TYPE_IS_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_DOWN))


/******************************************************************************/
typedef enum {
    WAIT_STAT_INIT = 0,
    WAIT_STAT_WAITING,
    WAIT_STAT_COMPLETED,
} wait_stat_t;
typedef struct {

#define TASK_STACK_SIZE   4096
    AW_TASK_DECL(task, TASK_STACK_SIZE);

    aw_net_sts_ctl_t        *p_ctl;
    aw_net_sts_evt_t         ctl_evt;

    aw_net_sts_dev_t        *p_dev_4g;
    aw_bool_t                flag_4g_net_ok;
    wait_stat_t              wait_4g_power_off;
    aw_net_sts_4g_evt_t      _4g_evt;
    aw_bool_t                flag_4g_evt_received;
    aw_bool_t                flag_4g_evt_completed;

    aw_net_sts_dev_t        *p_dev_eth0;
    aw_net_sts_dev_t        *p_dev_eth1;
    aw_net_sts_dev_t        *p_dev_wifi;
} test_ctx_t;

/******************************************************************************/
test_ctx_t __g_test_ctx;

extern aw_bool_t g_unity_test_skip_flag;

/******************************************************************************/
static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
    char        log_buf[256 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
}

/******************************************************************************/


aw_local void __net_sts_wait_completed (aw_net_sts_dev_t    *p_dev,
                                        uint32_t             ms)
{
    aw_tick_t tick = aw_sys_tick_get();
    aw_assert(p_dev && ms);
    while (__INTERVAL_MS_GET(tick) <= ms) {
        if (!aw_net_sts_dev_is_busy(p_dev)) {
            break;
        }
        aw_mdelay(1000);
    }
    aw_assert(__INTERVAL_MS_GET(tick) <= ms);
    __LOG_PFX(1, "Device %s operation completed. %d ms",
              p_dev->attr.netif_name, __INTERVAL_MS_GET(tick));
}
/******************************************************************************/

aw_local aw_err_t __net_sts_reinit (aw_net_sts_ctl_t    *p_ctl,
                                    aw_net_sts_dev_t    *p_dev,
                                    const char          *inet_ip,
                                    const char          *snet_ip,
                                    int                  chk_ms)
{
    aw_net_sts_dev_init_t init = { 0 };
    aw_err_t              ret;

    aw_assert(p_ctl && p_dev);
    init                 = p_dev->attr;
    init.chk_ms          = chk_ms;
    init.p_chk_inet_addr = inet_ip;
    init.p_chk_snet_addr = snet_ip;
    strcpy(init.netif_name, p_dev->attr.netif_name);
    __net_sts_wait_completed(p_dev, 10000);

    ret = aw_net_sts_dev_reinit(p_ctl, p_dev, &init);
    aw_assert(ret == AW_OK);
    __net_sts_wait_completed(p_dev, 10000);

    if (chk_ms < 10000) {
        aw_assert(p_dev->attr.chk_ms == 10000);
    } else {
        aw_assert(p_dev->attr.chk_ms == chk_ms);
    }
    aw_assert(strcmp(p_dev->attr.netif_name, init.netif_name) == 0);
    aw_assert(p_dev->attr.p_chk_inet_addr != init.p_chk_inet_addr);
    aw_assert(p_dev->attr.p_chk_snet_addr != init.p_chk_snet_addr);
    if (inet_ip) {
        aw_assert(strcmp(p_dev->attr.p_chk_inet_addr, inet_ip) == 0);
    } else {
        aw_assert(strlen(p_dev->attr.p_chk_inet_addr) == 0);
    }
    if (snet_ip) {
        aw_assert(strcmp(p_dev->attr.p_chk_snet_addr, snet_ip) == 0);
    } else {
        aw_assert(strlen(p_dev->attr.p_chk_snet_addr) == 0);
    }

    return ret;
}
/******************************************************************************/
aw_local void __4g_event_notify (aw_net_sts_4g_evt_t *p_evt)
{
    test_ctx_t *p_ctx = &__g_test_ctx;

    aw_assert(p_evt);

#if 0
    aw_assert(p_ctx->flag_4g_evt_received  == AW_FALSE);
    aw_assert(p_ctx->flag_4g_evt_completed == AW_TRUE);
    p_ctx->flag_4g_evt_received = AW_TRUE;
    p_ctx->_4g_evt = *p_evt;
#endif

    switch (p_evt->type) {

        case AW_NET_STS_4G_EVT_TYPE_NET_OK_FIRST: {
            p_ctx->flag_4g_net_ok = AW_TRUE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_POWER_ON: {
            p_ctx->flag_4g_net_ok = AW_FALSE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_NET_OK: {
            p_ctx->flag_4g_net_ok = AW_TRUE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_SOFT_RESET: {
            p_ctx->flag_4g_net_ok = AW_FALSE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_POWER_OFF_PRE: {
            p_ctx->flag_4g_net_ok = AW_FALSE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_SOFT_RESET_PRE: {
            p_ctx->flag_4g_net_ok = AW_FALSE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_POWER_OFF: {
            p_ctx->flag_4g_net_ok = AW_FALSE;

            if (p_ctx->wait_4g_power_off == WAIT_STAT_WAITING) {
                p_ctx->wait_4g_power_off = WAIT_STAT_COMPLETED;
            }
        } break;

        case AW_NET_STS_4G_EVT_TYPE_NET_LINK_UP: {
            p_ctx->flag_4g_net_ok = AW_TRUE;
        } break;

        case AW_NET_STS_4G_EVT_TYPE_NET_ERR: {
            p_ctx->flag_4g_net_ok = AW_FALSE;
        } break;

        default: break;
    }
}

/******************************************************************************/
aw_local void __net_sts_evt_handler (aw_net_sts_evt_t *p_evt)
{
    test_ctx_t *p_ctx = &__g_test_ctx;
    aw_assert(p_evt);

    p_ctx->ctl_evt = *p_evt;

    if (p_evt->type == AW_NET_STS_EVT_TYPE_SUBNET_PREFIX_REPEAT) {
        __LOG_RES(AW_FALSE, 1,
                  "It is detected that %c%c%d and %c%c%d have duplicate IP subnet prefix.",
                  __NETIF_NAME_FMT(p_evt->u.subnet_prefix_repeat.p_netif[0]),
                  __NETIF_NAME_FMT(p_evt->u.subnet_prefix_repeat.p_netif[1]));
        netifapi_netif_set_link_down(p_evt->u.subnet_prefix_repeat.p_netif[1]);
        return;
    }


    if (__EVT_TYPE_IS_LINK_OK(p_evt)) {
        __LOG_PFX(1, "Event:UP netif:%s", p_evt->u.link.p_dev->attr.netif_name);

    } else if (__EVT_TYPE_IS_FORCE_LINK_OK(p_evt)) {
        __LOG_PFX(1, "Event:MAN_UP netif:%s", p_evt->u.force_link.p_dev->attr.netif_name);

    } else if (__EVT_TYPE_IS_LINK_DOWN(p_evt)) {
        __LOG_PFX(1, "Event:DOWN netif:%s", p_evt->u.link.p_dev->attr.netif_name);

    } else if (__EVT_TYPE_IS_FORCE_LINK_DOWN(p_evt)) {
        __LOG_PFX(1, "Event:MAN_DOWN netif:%s", p_evt->u.force_link.p_dev->attr.netif_name);

    } else {
        aw_assert(p_evt);
    }
}


/******************************************************************************/
aw_local void __net_sts_man_netif_set (aw_net_sts_ctl_t         *p_ctl,
                                       aw_net_sts_dev_t         *p_dev,
                                       aw_net_sts_evt_type_t     type)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };


    ret = aw_net_sts_get(p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    if (__EVT_TYPE_IS_FORCE_LINK_OK(&sts_evt)) {
        if (sts_evt.u.force_link.p_dev == p_dev) {
            /* 检查该  p_dev 已设置为强制设备，则退出  */
            return;
        } else {
            /* 检查有其他设备设置为强制设备，则清除它 */
            sts_evt.type += 2;   /* 原值+2为 CLR */
            ret = aw_net_sts_evt_proc(p_ctl, &sts_evt);
            aw_assert(ret == AW_OK);
        }
    }

    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = type;
    sts_evt.u.force_link.p_dev  = p_dev;
    ret = aw_net_sts_evt_proc(p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
}
/******************************************************************************/
/* 初始化4g */
aw_local aw_err_t __4g_init (test_ctx_t *p_ctx)
{
    aw_net_sts_ctl_t        *p_ctl;
    aw_net_sts_dev_t        *p_dev;
    aw_net_sts_dev_init_t    dev_init;

    aw_assert(p_ctx);

    p_ctl = p_ctx->p_ctl;
    __ASSERT(p_ctl, "");

    memset(&dev_init, 0, sizeof(dev_init));
    memcpy(dev_init.netif_name, "pp", 3);
    dev_init.chk_ms          = 0;
    dev_init.pfn_log_out     = __log;
    dev_init.p_chk_inet_addr = "www.baidu.com";
    dev_init.p_chk_snet_addr = "www.souhu.com";
    dev_init.prioty          = 0;
    dev_init.type            = AW_NET_STS_DEV_TYPE_4G;
    dev_init.u._4g.pfn_event_notify = __4g_event_notify;
    p_dev = aw_net_sts_dev_create(&dev_init);
    aw_assert(p_dev);
    p_ctx->p_dev_4g = p_dev;

    p_ctx->flag_4g_evt_received  = AW_FALSE;
    p_ctx->flag_4g_evt_completed = AW_TRUE;

    return AW_OK;
}

aw_local aw_net_sts_dev_t *__net_sts_dev_create (test_ctx_t             *p_ctx,
                                                 char                   *p_name,
                                                 aw_net_sts_dev_type_t   type,
                                                 uint8_t                 prio)
{
    aw_net_sts_ctl_t        *p_ctl;
    aw_net_sts_dev_t        *p_dev;
    aw_net_sts_dev_init_t    dev_init;

    aw_assert(p_ctx && p_name);

    p_ctl = p_ctx->p_ctl;
    aw_assert(p_ctl);

    memset(&dev_init, 0, sizeof(dev_init));
    memcpy(dev_init.netif_name, p_name, strlen(p_name) + 1);
    dev_init.chk_ms          = 0;
    dev_init.pfn_log_out     = __log;
    dev_init.p_chk_inet_addr = "www.baidu.com";
    dev_init.p_chk_snet_addr = "www.souhu.com";
    dev_init.prioty          = prio;
    dev_init.type            = type;
    p_dev = aw_net_sts_dev_create(&dev_init);
    aw_assert(p_dev);

    return p_dev;
}
/******************************************************************************/

aw_local void __ctl_obtain_hdr (aw_net_sts_ctl_t    *p_ctl,
                                void                *p_arg)
{
    test_ctx_t *p_ctx = p_arg;
    if (!p_ctx->p_ctl) {
        p_ctx->p_ctl = p_ctl;
    }
}

aw_local void __net_sts_init (test_ctx_t *p_ctx)
{
    aw_err_t ret;
    aw_assert(p_ctx);

    ret = aw_net_sts_ctl_iterate(__ctl_obtain_hdr, p_ctx);
    if (ret == -AW_NET_STS_ECTL) {
        aw_net_sts_ctl_init_t    init;
        aw_net_sts_ctl_t        *p_ctl;

        memset(&init, 0, sizeof(init));

        init.pfn_log_out        = __log;
        init.pfn_evt_handler    = __net_sts_evt_handler;
        p_ctl = aw_net_sts_ctl_create(&init);
        aw_assert(p_ctl);
        p_ctx->p_ctl = p_ctl;
    }
    aw_assert(p_ctx->p_ctl);

    ret = __4g_init(p_ctx);
    aw_assert(ret == AW_OK);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_4g);
    aw_assert(ret == AW_OK);

    ret = aw_netif_dhcpc_start_by_name("eth0");
    aw_assert(ret == -AW_EPERM || ret == AW_OK);
    p_ctx->p_dev_eth0 = __net_sts_dev_create(p_ctx,
                                             "eth0",
                                             AW_NET_STS_DEV_TYPE_ETH,
                                             2);
    aw_assert(p_ctx->p_dev_eth0);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_eth0);
    aw_assert(ret == AW_OK);

    ret = aw_netif_dhcpc_start_by_name("eth1");
    aw_assert(ret == -AW_EPERM || ret == AW_OK);
    p_ctx->p_dev_eth1 = __net_sts_dev_create(p_ctx,
                                             "eth1",
                                             AW_NET_STS_DEV_TYPE_ETH,
                                             3);
    aw_assert(p_ctx->p_dev_eth1);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_eth1);
    aw_assert(ret == AW_OK);

    p_ctx->p_dev_wifi = __net_sts_dev_create(p_ctx,
                                             "st",
                                             AW_NET_STS_DEV_TYPE_WIFI,
                                             4);
    aw_assert(p_ctx->p_dev_wifi);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

}
/******************************************************************************/
/* Restore the removed device and reset the mandatory network state. */
aw_local aw_err_t __all_module_state_reset (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };
    aw_net_sts_evt_t tmp_evt = { 0 };

    (void)tmp_evt;
    __LOG_PFX(1, "Reset all settings and states for subsequent test.");

    ret = aw_net_sts_dev_is_free(p_ctx->p_dev_4g);
    if (ret == AW_TRUE) {
        aw_mdelay(500);
        ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_4g);
        aw_assert(ret == AW_OK);
        aw_mdelay(500);
    }

    ret = aw_net_sts_dev_is_free(p_ctx->p_dev_eth0);
    if (ret == AW_TRUE) {
        aw_mdelay(500);
        ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_eth0);
        aw_assert(ret == AW_OK);
        aw_mdelay(500);
    }

    ret = aw_net_sts_dev_is_free(p_ctx->p_dev_eth1);
    if (ret == AW_TRUE) {
        aw_mdelay(500);
        ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_eth1);
        aw_assert(ret == AW_OK);
        aw_mdelay(500);
    }

    ret = aw_net_sts_dev_is_free(p_ctx->p_dev_wifi);
    if (ret == AW_TRUE) {
        aw_mdelay(500);
        ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_wifi);
        aw_assert(ret == AW_OK);
        aw_mdelay(500);
    }

    /* Reset the mandatory network state. */
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    if (__EVT_TYPE_IS_FORCE_LINK_OK(&sts_evt)) {
        sts_evt.type               += 2;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
    }

    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(!__EVT_TYPE_IS_FORCE_LINK_OK(&sts_evt));

    return AW_OK;
}
/******************************************************************************/
aw_local aw_err_t __test_force_link (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };

    __LOG_PFX(1, "Checks current state. Result: DOWN");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(__EVT_TYPE_IS_ANY_LINK_DOWN(&sts_evt));

    __LOG_PFX(1, "Sets mandatory 4g.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_4g;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Removes wifi module.");
    aw_mdelay(1000);
    ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Sets net state to mandatory wifi. Result: Failed.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_wifi;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret != AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Appends wifi module.");
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

    aw_mdelay(3000);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == -AW_EEXIST);

    __LOG_PFX(1, "Sets net state to be mandatory wifi. Result: Failed.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_wifi;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret != AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Attempts to clean WIFI INET_CLR. Result: Failed.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_wifi;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret != AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Cleans the 4G_SNET net state. Result: Failed.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_4g;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret != AW_OK);

    __LOG_PFX(1, "Cleans the 4G_INET net state. Result: OK ");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_4g;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Sets net state to be mandatory wifi. Result: OK.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_wifi;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Checks current state. Result: WIFI INET_SET");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_wifi);

    __LOG_PFX(1, "Switch to mandatory 4G state. Result: OK");
    __net_sts_man_netif_set(p_ctx->p_ctl,
                            p_ctx->p_dev_4g,
                            AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Removes 4G module and the mandatory 4G state will be clear. Result: OK");
    ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_4g);
    aw_assert(ret == AW_OK);
    aw_mdelay(500);
    ret = aw_net_sts_dev_is_free(p_ctx->p_dev_4g);
    aw_assert(ret == AW_TRUE);
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_LINK_DOWN);

    __all_module_state_reset(p_ctx);
    return AW_OK;
}

/*
 * 测试内容：测试 net_sts 设备的重初始化参数功能，修改参数有延时、检测地址
 * 测试结果：成功
 */
aw_local aw_err_t __test_reinit (test_ctx_t *p_ctx,
                                 aw_net_sts_dev_t *p_dev,
                                 const char * p_module_name)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };

//    __LOG_PFX(1, "Checks current state. Result: DOWN");
//    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
//    aw_assert(ret == AW_OK);
//    aw_assert(__EVT_TYPE_IS_ANY_LINK_DOWN(&sts_evt));

    __LOG_PFX(1, "Removes %s module. Result: OK", p_module_name);
    ret = aw_net_sts_del(p_ctx->p_ctl, p_dev);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Adds %s module. Result: OK", p_module_name);
    aw_mdelay(500);
    ret = aw_net_sts_add(p_ctx->p_ctl, p_dev);
    aw_assert(ret == AW_OK);

    /**
     * Modify the delay time parameter.
     */
    __LOG_PFX(1, "Reinitialize %s module. Result: OK", p_module_name);
    ret = __net_sts_reinit(p_ctx->p_ctl, p_dev, NULL, NULL, 1000);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Reinitialize %s module. Result: OK", p_module_name);
    ret = __net_sts_reinit(p_ctx->p_ctl, p_dev, NULL, NULL, 111000);
    aw_assert(ret == AW_OK);

    /**
     * Modify the check address parameter.
     */
    __LOG_PFX(1, "Reinitialize %s module. Result: OK", p_module_name);
    ret = __net_sts_reinit(p_ctx->p_ctl,
                           p_dev,
                           "www.baidu.com",
                           NULL,
                           p_dev->attr.chk_ms);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Reinitialize %s module. Result: OK", p_module_name);
    ret = __net_sts_reinit(p_ctx->p_ctl,
                           p_dev,
                           NULL,
                           "www.sohu.com",
                           p_dev->attr.chk_ms + 100);
    aw_assert(ret == AW_OK);

    __all_module_state_reset(p_ctx);
    return AW_OK;
}

aw_local aw_err_t __test_4g_reinit (test_ctx_t *p_ctx)
{
    return __test_reinit(p_ctx, p_ctx->p_dev_4g, "4G");
}

aw_local aw_err_t __test_eth0_reinit (test_ctx_t *p_ctx)
{
    return __test_reinit(p_ctx, p_ctx->p_dev_eth0, "ETH0");
}

/******************************************************************************/
/**
 * 测试内容：测试修改 chk_ms 参数，先修改为24小时，再修改回 30s
 * 测试结果：修改成功，且修改加为30s后，网络应该在60s内恢复
 */
aw_local aw_err_t __test_reinit_chk_ms (test_ctx_t *p_ctx, aw_net_sts_dev_t *p_dev)
{
    aw_err_t                 ret;
    aw_net_sts_dev_init_t    init;
    aw_tick_t                tick;
    aw_tick_t                tick1;
    aw_net_sts_evt_t sts_evt = { 0 };

    __LOG_PFX(1, "Current interval between inspections is %d ms.", p_dev->attr.chk_ms);
    while (1) {
        __LOG_PFX(1, "Wait %s to connect network...", p_dev->attr.netif_name);
        if (g_unity_test_skip_flag) {
            return AW_OK;
        }

        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        if (__EVT_TYPE_IS_LINK_OK(&sts_evt)) {
            if (sts_evt.u.link.p_dev == p_dev) {
                break;
            } else {
                ret = aw_net_sts_del(p_ctx->p_ctl, sts_evt.u.link.p_dev);
                aw_assert(ret == AW_OK);
            }
        }
        aw_mdelay(1000);
    }

    init = p_dev->attr;
    init.chk_ms = 1000*60*60*24;  //24hours
    ret = aw_net_sts_dev_reinit(p_dev->p_ctl, p_dev, &init);
    aw_assert(ret == AW_OK);
    __TIMEOUT_MSEC(tick, 20000) {
        if (p_dev->flag_busy == 0) {
            break;
        } else if (__TIMEOUT_MSEC_REMAIN(tick) < 1000) {
            /* time over */
            aw_assert(0);
        }
        aw_mdelay(10);
        if (g_unity_test_skip_flag) {
            return AW_OK;
        }
    }
    __LOG_PFX(1, "Current interval between inspections is %d ms.", p_dev->attr.chk_ms);


    aw_mdelay(3000);
    ret = aw_net_sts_del(p_ctx->p_ctl, p_dev);
    aw_assert(ret == AW_OK);
    __net_sts_wait_completed(p_dev, 30000);
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(__EVT_TYPE_IS_ANY_LINK_DOWN(&sts_evt));

    ret = aw_net_sts_add(p_ctx->p_ctl, p_dev);
    aw_assert(ret == AW_OK);
    __net_sts_wait_completed(p_dev, 30000);
    aw_mdelay(6000);
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(__EVT_TYPE_IS_LINK_OK(&sts_evt) && sts_evt.u.link.p_dev == p_dev);

    tick1 = aw_sys_tick_get();
    init = p_dev->attr;
    init.chk_ms = 1000*30;
    ret = aw_net_sts_dev_reinit(p_dev->p_ctl, p_dev, &init);
    aw_assert(ret == AW_OK || ret == -AW_EBUSY);
    __TIMEOUT_MSEC(tick, 20000) {
        if (p_dev->flag_busy == 0) {
            break;
        } else if (__TIMEOUT_MSEC_REMAIN(tick) < 1000) {
            /* time over */
            aw_assert(0);
        }
        aw_mdelay(10);
        if (g_unity_test_skip_flag) {
            return AW_OK;
        }
    }
    __LOG_PFX(1, "Current interval between inspections is %d ms.", p_dev->attr.chk_ms);

    __TIMEOUT_MSEC(tick, 60000) {
        __LOG_PFX(1, "Wait %s to connect network...", p_dev->attr.netif_name);
        if (g_unity_test_skip_flag) {
            return AW_OK;
        }

        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        if (__EVT_TYPE_IS_LINK_OK(&sts_evt)) {
            aw_assert(sts_evt.u.link.p_dev == p_dev);
            __LOG_PFX(1, "Time to successfully connect to the Internet is %d ms.",
                      __INTERVAL_MS_GET(tick1));
            break;
        }
        aw_mdelay(1000);
    }

    while (1) {
        aw_mdelay(2000);
        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        if (__EVT_TYPE_IS_LINK_OK(&sts_evt)) {
            __LOG_PFX(1, "Wait for %s to link down or unplug...",
                      sts_evt.u.link.p_dev->attr.netif_name);
            continue;
        }

        if (__EVT_TYPE_IS_ANY_LINK_DOWN(&sts_evt)) {
            break;
        } else {
            aw_assert(0);
        }
    }

    return AW_OK;
}

aw_local aw_err_t __test_wifi_reinit_chk_ms (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    ret = __test_reinit_chk_ms(p_ctx, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

    __all_module_state_reset(p_ctx);
    return AW_OK;
}
aw_local aw_err_t __test_eth_reinit_chk_ms (test_ctx_t *p_ctx)
{
    aw_err_t         ret;

    ret = __test_reinit_chk_ms(p_ctx, p_ctx->p_dev_eth1);
    aw_assert(ret == AW_OK);

    __all_module_state_reset(p_ctx);
    return AW_OK;
}

aw_local aw_err_t __test_4g_ioctl (test_ctx_t *p_ctx)
{
    aw_err_t                 ret;
    aw_net_sts_evt_t         sts_evt = { 0 };
    aw_net_sts_dev_init_t    init = { 0 };
    aw_tick_t                tick;
    aw_net_sts_dev_t        *p_dev = p_ctx->p_dev_4g;

    (void)init;

    __LOG_PFX(1, "Reinitialize 4G module. Result: OK");
    ret = __net_sts_reinit(p_ctx->p_ctl, p_dev, NULL, NULL, 10000);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Sets mandatory 4g.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_4g;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Wait for 4g module to establish ppp link.");
    __TIMEOUT_MSEC(tick, 5*60*1000) {
        aw_tick_t tick_tmp;
        aw_tick_t tick_tmp1;

        __TIMEOUT_MSEC(tick_tmp, 1000) {
            if (p_ctx->flag_4g_net_ok) {
                __TIMEOUT_MSEC(tick_tmp1, 1500) {
                    if (p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET) {
                        __LOG_PFX(1, "The interval from 4G network OK event to "
                                " NET_CTL link up event is %d ms.",
                                1500 - __TIMEOUT_MSEC_REMAIN(tick_tmp1));
                        break;
                    }
                    aw_mdelay(1);
                }
                aw_assert(p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
                __LOG_PFX(1, "4g module successfully established ppp link.");
                break;
            } else {
                /* The net_ctl event is executed earlier than the 4G event. */
                if (__EVT_TYPE_IS_ANY_LINK_OK(&p_ctx->ctl_evt)) {
                    tick_tmp1 = aw_sys_tick_get();
                    while (__INTERVAL_MS_GET(tick_tmp1) <= 30000) {
                        aw_mdelay(1);
                        if (p_ctx->flag_4g_net_ok) {
                            __LOG_PFX(1, "The interval from NET_CTL link up event to"
                                    " 4G network OK event is %d ms.",
                                    __INTERVAL_MS_GET(tick_tmp1));
                            break;
                        }
                    }

                    aw_assert(p_ctx->flag_4g_net_ok);
                    __LOG_PFX(1, "4g module successfully established ppp link.");
                }
            }
            aw_mdelay(1);
        }
        if (p_ctx->flag_4g_net_ok) {
            break;
        }
        __LOG_PFX(1, "Wait for 4g module to establish ppp link.(remain:%dms)",
                  __TIMEOUT_MSEC_REMAIN(tick));
        aw_mdelay(1);
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }

    aw_assert(p_ctx->flag_4g_net_ok);

    __LOG_PFX(1, "Removes wifi module.");
    ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    aw_assert(p_ctx->flag_4g_net_ok);
    aw_assert(p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);

    /* Test gprs ioctl function with AT_TRANS option. */
    {
        int                  devid;
        char                 rec_buf[256];
        aw_gprs_ioctl_args_t arg;

        devid = aw_gprs_any_get(1000);
        aw_assert(devid >= 0);

        memset(rec_buf, 0, 256);
        arg.at_trans.p_at_cmd        = "AT+CSQ\r";
        arg.at_trans.p_recv_expect   = "CSQ:";
        arg.at_trans.p_recv_buf      = rec_buf;
        arg.at_trans.bufsize         = sizeof(rec_buf);
        arg.at_trans.recv_tm         = 1500;
        ret = aw_gprs_ioctl(devid, AW_GPRS_REQ_AT_TRANS, &arg);
        aw_assert(ret == AW_OK);
        aw_assert(arg.at_trans.recvlen != 0);
        __LOG_PFX(1, "Test AT command: %s , result: %s \n\t recvlen:%d",
                  arg.at_trans.p_at_cmd, arg.at_trans.p_recv_buf, arg.at_trans.recvlen);

        /* Send AT command only. */
        arg.at_trans.p_at_cmd        = "AT+CIMI\r";
        arg.at_trans.p_recv_expect   = NULL;
        arg.at_trans.p_recv_buf      = NULL;
        aw_gprs_ioctl(devid, AW_GPRS_REQ_AT_TRANS, &arg);
        aw_assert(ret == AW_OK);
        aw_assert(arg.at_trans.recvlen == 0);
        __LOG_PFX(1, "Test AT command with %s only", arg.at_trans.p_at_cmd);

        /* Send AT response only. */
        arg.at_trans.p_at_cmd        = NULL;
        arg.at_trans.p_recv_expect   = "46";
        arg.at_trans.p_recv_buf      = rec_buf;
        arg.at_trans.bufsize         = sizeof(rec_buf);
        arg.at_trans.recv_tm         = 1500;
        aw_gprs_ioctl(devid, AW_GPRS_REQ_AT_TRANS, &arg);
        aw_assert(ret == AW_OK);
        aw_assert(arg.at_trans.recvlen != 0);
        aw_assert(strstr(arg.at_trans.p_recv_buf, arg.at_trans.p_recv_expect) != 0);
        __LOG_PFX(1, "Test AT response only.(expect:%s)", arg.at_trans.p_recv_expect);
    }

exit:
    __all_module_state_reset(p_ctx);

    aw_mdelay(3000);
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_LINK_INET);
    aw_assert(sts_evt.u.link.p_dev == p_ctx->p_dev_4g);

    return AW_OK;
}

aw_local aw_err_t __test_4g_link_ok (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };
    aw_net_sts_dev_init_t init = { 0 };
    aw_tick_t        tick;

    (void)init;
    __LOG_PFX(1, "Reinitialize 4G module. Result: OK");
    aw_mdelay(500);
    init                 = p_ctx->p_dev_4g->attr;
    init.chk_ms          = 1000;
    init.p_chk_inet_addr = NULL;
    init.p_chk_snet_addr = NULL;
    init.pfn_log_out     = NULL;
    strcpy(init.netif_name, "pp");
    ret = aw_net_sts_dev_reinit(p_ctx->p_ctl, p_ctx->p_dev_4g, &init);
    aw_assert(ret == AW_OK);
    __net_sts_wait_completed(p_ctx->p_dev_4g, 10000);
    aw_assert(p_ctx->p_dev_4g->attr.chk_ms == 10000); /* The minimum is 10s */
    aw_assert(strcmp(p_ctx->p_dev_4g->attr.netif_name, init.netif_name) == 0);

    __LOG_PFX(1, "Waif for 4g modult to unplug SIM and reset.");
    __TIMEOUT_MSEC(tick, 30*60*1000) {
        if (!p_ctx->flag_4g_net_ok) {
            p_ctx->p_dev_4g->p_netif = NULL;
            break;
        }
        aw_mdelay(2000);
        __LOG_PFX(1, "Waif for 4g modult to unplug SIM and reset.(remain:%d S)",
                  __TIMEOUT_MSEC_REMAIN(tick) / 1000);
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }

    __LOG_PFX(1, "Waif for 4g modult to insert SIM.");
    aw_mdelay(5000);

    __LOG_PFX(1, "Sets mandatory 4g.");
    memset(&sts_evt, 0, sizeof(sts_evt));
    sts_evt.type                = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET;
    sts_evt.u.force_link.p_dev  = p_ctx->p_dev_4g;
    ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    __LOG_PFX(1, "Wait for 4g module to establish ppp link.");
    __TIMEOUT_MSEC(tick, 30*60*1000) {
        aw_tick_t tick_tmp;
        aw_tick_t tick_tmp1;

        __TIMEOUT_MSEC(tick_tmp, 1000) {
            if (p_ctx->flag_4g_net_ok) {
                __TIMEOUT_MSEC(tick_tmp1, 1500) {
                    if (p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET) {
                        __LOG_PFX(1, "The interval from 4G network OK event to "
                                " NET_CTL link up event is %d ms.",
                                1500 - __TIMEOUT_MSEC_REMAIN(tick_tmp1));
                        break;
                    }
                    aw_mdelay(1);
                }
                aw_assert(p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
                __LOG_PFX(1, "4g module successfully established ppp link.");
                break;
            } else {
                /* The net_ctl event is executed earlier than the 4G event. */
                if (__EVT_TYPE_IS_ANY_LINK_OK(&p_ctx->ctl_evt)) {

                    tick_tmp1 = aw_sys_tick_get();
                    while (__INTERVAL_MS_GET(tick_tmp1) <= 30000) {
                        aw_mdelay(1);
                        if (p_ctx->flag_4g_net_ok) {
                            __LOG_PFX(1, "The interval from NET_CTL link up event to"
                                    " 4G network OK event is %d ms.",
                                    __INTERVAL_MS_GET(tick_tmp1));
                            break;
                        }
                    }

                    aw_assert(p_ctx->flag_4g_net_ok);
                    __LOG_PFX(1, "4g module successfully established ppp link.");
                }
            }
            aw_mdelay(1);
        }

        if (p_ctx->flag_4g_net_ok) {
            break;
        }

        __LOG_PFX(1, "Wait for 4g module to establish ppp link.(remain:%dms)",
                  __TIMEOUT_MSEC_REMAIN(tick));
        aw_mdelay(1);
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }

    aw_assert(p_ctx->flag_4g_net_ok);

    __LOG_PFX(1, "Removes wifi module.");
    ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_wifi);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Checks net state and it should be 4G INET_SET.");
    ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
    aw_assert(ret == AW_OK);
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);
    aw_assert(sts_evt.u.force_link.p_dev == p_ctx->p_dev_4g);

    aw_assert(p_ctx->flag_4g_net_ok);
    aw_assert(p_ctx->ctl_evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET);

exit:
    __all_module_state_reset(p_ctx);
    return AW_OK;
}


aw_local aw_err_t __test_subnet_check_add (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    aw_net_sts_evt_t sts_evt = { 0 };
    aw_netif_t      *p_netif;

    if (p_ctx->p_ctl == NULL) {
        ret = aw_net_sts_ctl_iterate(__ctl_obtain_hdr, p_ctx);
    }
    aw_assert(p_ctx->p_ctl);

    p_netif = aw_netif_dev_open("ap");
    aw_assert(p_netif);

    /* Delete it once to avoid it already exists. */
    sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL;
    sts_evt.u.netif_subnet_check.p_netif = &p_netif->netif;
    aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);

    for (int i = 0; i < 20; i++) {
        sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD;
        sts_evt.u.netif_subnet_check.p_netif = &p_netif->netif;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);

        aw_mdelay(1000);
        sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL;
        sts_evt.u.netif_subnet_check.p_netif = &p_netif->netif;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);

        if (g_unity_test_skip_flag) {
            return AW_OK;
        }
    }
    return AW_OK;
}

aw_local aw_err_t __test_subnet_check_repeat (test_ctx_t *p_ctx)
{
    aw_err_t         ret;
    aw_tick_t        tick;
    aw_net_sts_evt_t sts_evt = { 0 };
    aw_netif_t      *p_if[2];
    struct {
        char *netif_name[2];
    } test_arg;

    /* Set test parameters.*/
    test_arg.netif_name[0] = "eth0";
    test_arg.netif_name[1] = "eth1";

    /* To enable DHCPC, both ports need to be connected to the LAN port of the
     * same router. */
    {
        ret = aw_netif_dhcpc_start_by_name(test_arg.netif_name[0]);
        aw_assert(ret == -AW_EPERM || ret == AW_OK);
        ret = aw_netif_dhcpc_start_by_name(test_arg.netif_name[1]);
        aw_assert(ret == -AW_EPERM || ret == AW_OK);

        p_if[0] = aw_netif_dev_open(test_arg.netif_name[0]);
        aw_assert(p_if[0]);
        p_if[1] = aw_netif_dev_open(test_arg.netif_name[1]);
        aw_assert(p_if[1]);
    }

    /* Both devices tested were added to network management. */
    {
        __TIMEOUT_MSEC(tick, 30*60*1000) {
            __LOG_PFX(1, "wait for the DHCPS to assign the same subnet address...");

            if (ip_addr_netcmp(&p_if[0]->netif.ip_addr,
                               &p_if[1]->netif.ip_addr,
                               &p_if[0]->netif.netmask.u_addr.ip4) == AW_TRUE) {
                break;
            }
            aw_mdelay(2000);
            if (g_unity_test_skip_flag) {
                goto exit;
            }
        }
        __LOG_PFX(1, "It is detected that %s and %s are the same subnet.",
                  p_if[0]->p_name, p_if[1]->p_name);

        aw_mdelay(6000);
        if (netif_is_link_up(&p_if[0]->netif) && netif_is_link_up(&p_if[1]->netif)) {
            aw_assert(ip_addr_netcmp(&p_if[0]->netif.ip_addr,
                                     &p_if[1]->netif.ip_addr,
                                     &p_if[0]->netif.netmask.u_addr.ip4) == AW_FALSE);
        }
        __LOG_PFX(1, "Link down one of the devices of the same subnet.");

        sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD;
        sts_evt.u.netif_subnet_check.p_netif = &p_if[0]->netif;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == -AW_EEXIST);
    }

    /* Remove one from the network management and then add it to the subnet
     * detection using the AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD event */
    {
        ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_eth0);
        aw_assert(ret == AW_OK);

        sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD;
        sts_evt.u.netif_subnet_check.p_netif = &p_if[0]->netif;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);

        /* Reopen the network card that was previously closed by the callback function. */
        if (!netif_is_link_up(&p_if[0]->netif)) {
            netif_set_link_up(&p_if[0]->netif);
            aw_assert(netif_is_link_up(&p_if[0]->netif));
        } else if (!netif_is_link_up(&p_if[1]->netif)) {
            netif_set_link_up(&p_if[1]->netif);
            aw_assert(netif_is_link_up(&p_if[1]->netif));
        } else {
            aw_assert(0);
        }

        /* Wait for callback function to link down the network card. */
        aw_mdelay(6000);
        if (netif_is_link_up(&p_if[0]->netif) && netif_is_link_up(&p_if[1]->netif)) {
            aw_assert(ip_addr_netcmp(&p_if[0]->netif.ip_addr,
                                     &p_if[1]->netif.ip_addr,
                                     &p_if[0]->netif.netmask.u_addr.ip4) == AW_FALSE);
        }

        sts_evt.type = AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL;
        sts_evt.u.netif_subnet_check.p_netif = &p_if[0]->netif;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
    }


exit:
    {
        aw_netif_dev_close(p_if[0]);
        aw_netif_dev_close(p_if[1]);
    }
    __all_module_state_reset(p_ctx);
    return AW_OK;
}

aw_local aw_err_t __test_dns_query (test_ctx_t *p_ctx)
{
    aw_err_t     ret;
    ip_addr_t    addr;
    aw_tick_t    tick;
    aw_tick_t    tick_itv;
    aw_tick_t    tick_itv1;
    aw_net_sts_evt_t evt;
    aw_net_sts_evt_t sts_evt = { 0 };

    struct {
        char *netif_name;
        char *p_chk_ip;
        char *p_chk_ip1;
    } test_arg;

    /* Set test parameters.*/
    test_arg.netif_name = "eth1";
    test_arg.p_chk_ip   = "www.baidu.com";
    test_arg.p_chk_ip1  = "www.sohu.com";


    /* Configure network interface, need to be able to access the Internet. */
    ret = aw_netif_dhcpc_start_by_name(test_arg.netif_name);
    aw_assert(ret == -AW_EPERM || ret == AW_OK);

    ret = aw_net_sts_del(p_ctx->p_ctl, p_ctx->p_dev_4g);
    aw_assert(ret == AW_OK);

    __LOG_PFX(1, "Wait for domain name query to succeed.");
    __TIMEOUT_MSEC(tick, 1000*60*5) {
        evt.type                            = AW_NET_STS_EVT_TYPE_DNS_QUERY;
        evt.u.dns_query.host_name           = test_arg.p_chk_ip;
        evt.u.dns_query.flag_force_query    = AW_FALSE;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &evt);
        if (ret == AW_OK) {
            addr = evt.u.dns_query.ip_addr;
            ret = AW_OK;
            break;
        } else {
            ret = -AW_ENODATA;
        }
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }
    aw_assert(ret == AW_OK);

    __TIMEOUT_MSEC(tick, 1000*60*30) {
        __LOG_PFX(1, "Wait for %s network to link Internet...", test_arg.netif_name);
        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        if (__EVT_TYPE_IS_ANY_LINK_OK(&sts_evt)) {
            break;
        }
        if (g_unity_test_skip_flag) {
            goto exit;
        }
        aw_mdelay(100);
    }
    aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_LINK_INET);
    aw_assert(sts_evt.u.link.p_dev == p_ctx->p_dev_eth1);

    __LOG_PFX(1, "Domain name query successfully.(host:%s ip:%s)",
              test_arg.p_chk_ip, inet_ntoa((addr.u_addr.ip4)));

    __LOG_PFX(1, "Query the domain name every minute, total test 15 minutes, "
             "each step \n\t of the test results must be successful.");
    tick_itv  = aw_sys_tick_get();
    tick_itv1 = aw_sys_tick_get();
    __TIMEOUT_MSEC(tick, 1000*60*15) {

        if (__INTERVAL_MS_GET(tick_itv1) >= 1000*60) {
            tick_itv1 = aw_sys_tick_get();

            evt.type                            = AW_NET_STS_EVT_TYPE_DNS_QUERY;
            evt.u.dns_query.host_name           = test_arg.p_chk_ip;
            evt.u.dns_query.flag_force_query    = AW_FALSE;
            ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &evt);
            aw_assert(ret == AW_OK);
        }

        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_LINK_INET);
        aw_assert(sts_evt.u.link.p_dev == p_ctx->p_dev_eth1);

        if (__INTERVAL_MS_GET(tick_itv) >= 5000) {
            tick_itv = aw_sys_tick_get();
            __LOG_PFX(1, "Query the domain name, result: OK (remain:%d S)",
                      __TIMEOUT_MSEC_REMAIN(tick) / 1000);
        }

        aw_mdelay(10);
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }

    __LOG_PFX(1, "Check the domain name query for 1 hour must be successful.");
    tick_itv = aw_sys_tick_get();
    __TIMEOUT_MSEC(tick, 1000*60*60) {
        evt.type                            = AW_NET_STS_EVT_TYPE_DNS_QUERY;
        evt.u.dns_query.host_name           = test_arg.p_chk_ip;
        evt.u.dns_query.flag_force_query    = AW_FALSE;
        ret = aw_net_sts_evt_proc(p_ctx->p_ctl, &evt);
        aw_assert(ret == AW_OK);

        ret = aw_net_sts_get(p_ctx->p_ctl, &sts_evt);
        aw_assert(ret == AW_OK);
        aw_assert(sts_evt.type == AW_NET_STS_EVT_TYPE_LINK_INET);
        aw_assert(sts_evt.u.link.p_dev == p_ctx->p_dev_eth1);

        if (__INTERVAL_MS_GET(tick_itv) >= 5000) {
            tick_itv = aw_sys_tick_get();
            __LOG_PFX(1, "Query the domain name, result: OK (remain:%d S)",
                      __TIMEOUT_MSEC_REMAIN(tick) / 1000);
        }
        aw_mdelay(10);
        if (g_unity_test_skip_flag) {
            goto exit;
        }
    }

exit:
    __all_module_state_reset(p_ctx);
    return AW_OK;
}

/*****************************************************************************/
TEST_GROUP(net_sts);

TEST_SETUP(net_sts)
{
    g_unity_test_skip_flag = AW_FALSE;
}

TEST_TEAR_DOWN(net_sts)
{
}

/*****************************************************************************/

TEST(net_sts, __test_force_link)
{
    __TEST_EXE(__test_force_link, &__g_test_ctx);

}

TEST(net_sts, __test_eth0_reinit)
{
    __TEST_EXE(__test_eth0_reinit, &__g_test_ctx);
}
TEST(net_sts, __test_eth_reinit_chk_ms)
{
    __TEST_EXE(__test_eth_reinit_chk_ms, &__g_test_ctx);
}

TEST(net_sts, __test_4g_reinit)
{
    __TEST_EXE(__test_4g_reinit, &__g_test_ctx);
}
TEST(net_sts, __test_4g_ioctl)
{
    __TEST_EXE(__test_4g_ioctl, &__g_test_ctx);
}
TEST(net_sts, __test_4g_link_ok)
{
    __TEST_EXE(__test_4g_link_ok, &__g_test_ctx);
}

TEST(net_sts, __test_wifi_reinit_chk_ms)
{
    __TEST_EXE(__test_wifi_reinit_chk_ms, &__g_test_ctx);
}

TEST(net_sts, __test_dns_query)
{
    __TEST_EXE(__test_dns_query, &__g_test_ctx);
}
TEST(net_sts, __test_subnet_check_repeat)
{
    __TEST_EXE(__test_subnet_check_repeat, &__g_test_ctx);
}
TEST(net_sts, __test_subnet_check_add)
{
    __TEST_EXE(__test_subnet_check_add, &__g_test_ctx);
}
/*****************************************************************************/

TEST_GROUP_RUNNER(net_sts)
{
    extern void unity_test_net_sts_init(void);
    unity_test_net_sts_init();

    __net_sts_init(&__g_test_ctx);

    RUN_TEST_CASE(net_sts, __test_force_link);
    RUN_TEST_CASE(net_sts, __test_eth0_reinit);
    RUN_TEST_CASE(net_sts, __test_eth_reinit_chk_ms);

    RUN_TEST_CASE(net_sts, __test_4g_reinit);
    RUN_TEST_CASE(net_sts, __test_4g_ioctl);
    RUN_TEST_CASE(net_sts, __test_4g_link_ok);

    RUN_TEST_CASE(net_sts, __test_wifi_reinit_chk_ms);

    RUN_TEST_CASE(net_sts, __test_subnet_check_add);
    RUN_TEST_CASE(net_sts, __test_subnet_check_repeat);

    /* 耗时较长 */
    RUN_TEST_CASE(net_sts, __test_dns_query);
}

/* end of file */

