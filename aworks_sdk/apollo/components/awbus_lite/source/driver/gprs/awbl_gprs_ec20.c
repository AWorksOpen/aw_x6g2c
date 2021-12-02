/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 移远 EC20CEFILG LTE模块驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-08  vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "stdio.h"
#include "string.h"
#include "awbus_lite.h"
#include "awbl_plb.h"

#include "driver/gprs/awbl_gprs_ec20.h"

#if AWBL_GPRS_USB_USBH_SERIAL
#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"
#include "host/class/cdc/awbl_usbh_usbnet.h"
#else
#include "host/class/awbl_usbh_wireless.h"
#endif /* AWBL_GPRS_USB_USBH_SERIAL */

#include "aw_gpio.h"
#include "aw_sem.h"

#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_assert.h"

#include "lwip/sio.h"
#include "lwip/inet.h"

#include "aw_assert.h"

/******************************************************************************/
#define __EC20_DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_ec20_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                   struct awbl_gprs_ec20_dev, \
                                                   dev)

#define __EC20_DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_ec20_dev *p_this = \
                     (struct awbl_gprs_ec20_dev *)p_gprs->p_gprs_dev

#define __at_cmd_send(p_this, MSG)  \
        if (p_this->at_pipe) {p_this->drvopr.pfn_at_send(p_this, MSG);}
/******************************************************************************/
aw_local int __at_cmd_result_wait (struct awbl_gprs_dev *p_this,
                                   char                 *p_str,
                                   uint32_t              ms_timeout)
{
    char *arg[1];

    arg[0] = p_str;

    if (p_this->at_pipe == NULL) {
        return -1;
    }
    return p_this->drvopr.pfn_at_recv(p_this,
                                      p_this->at_buf,
                                      p_this->at_bufsz,
                                      arg,
                                      1,
                                      ms_timeout);
}
/******************************************************************************/


static int __socket_get (struct awbl_gprs_ec20_dev *p_this)
{
    uint8_t *p_socket       = p_this->socket;
    uint8_t *p_socket_used  = p_this->socket_used;
    uint8_t  i = 0;

    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
        if (p_socket_used[i] == 0) {
            if (p_socket_used[i] < 255) {
                p_socket_used[i]++;
            }
            return p_socket[i];
        }
    }
    return -1;
}

static void __socket_put (struct awbl_gprs_ec20_dev *p_this,
                          uint8_t                    socket)
{
    uint8_t *p_socket       = p_this->socket;
    uint8_t *p_socket_used  = p_this->socket_used;
    uint8_t  i = 0;

    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
        if (p_socket[i] == socket) {
            if (p_socket_used[i] > 0) {
                p_socket_used[i]--;
                return ;
            }
        }
    }
}
/******************************************************************************/
static aw_err_t __init_prev (struct awbl_gprs_dev *p_gprs)
{
    uint8_t     socket[__INTERNAL_SOCKET_NUM] = __INTERNAL_SOCKET_VAL;
    __EC20_DECL_FROM_GPRS(p_this, p_gprs);

    memcpy(p_this->socket, socket, sizeof(socket));
    memset(p_this->socket_used, 0, sizeof(p_this->socket_used));
    return AW_OK;
}

static aw_err_t __ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}


static aw_err_t __tcpudp_conn (struct awbl_gprs_dev   *p_gprs,
                               uint8_t                 mode,
                               char                   *p_ip,
                               uint16_t                port,
                               uint32_t                timeout)
{
    __EC20_DECL_FROM_GPRS(p_this, p_gprs);
    char       *buf     = p_gprs->at_buf;
    int         ret     = -1;
    int         hd      = -1;

    hd = __socket_get(p_this);
    if (hd < 0) {
        return -AW_ENOBUFS;
    }

    ret = p_gprs->drvopr.pfn_apn_get(p_gprs);
    if (ret) {
        return ret;
    }

    sprintf(buf, "AT^IPINIT=\"%s\"\r", p_this->gprs.info.p_apn);
    __at_cmd_send(p_gprs, buf);
    if (__at_cmd_result_wait(p_gprs, "OK", 5000) != AW_OK) {
        __at_cmd_send(p_gprs, "AT^IPINIT?\r");
        if (__at_cmd_result_wait(p_gprs, "OK", 5000) != AW_OK) {
            ret = -AW_ETIMEDOUT;
            goto failed;
        }
    }

    sprintf(buf, "AT^IPOPEN=%d,\"%s\",\"%s\",%d\r",
               hd, (mode == 0 ? "TCP":"UDP"), p_ip, port);
    __at_cmd_send(p_gprs, buf);
    if (__at_cmd_result_wait(p_gprs, "OK", 5000) != AW_OK) {
        ret = -AW_ETIMEDOUT;
        goto failed;
    }

    return hd;

failed:
    __socket_put(p_this, hd);
    return ret;
}

/** \brief 关闭GPRS连接 */
aw_local aw_err_t __tcpudp_disconn (struct awbl_gprs_dev *p_gprs, int hd)
{
    __EC20_DECL_FROM_GPRS(p_this, p_gprs);
    char *buf     = p_gprs->at_buf;

    if (hd < 1 || hd > 5) {
        return -AW_ENODEV;
    }

    sprintf(buf, "AT^IPCLOSE=%d\r", hd);
    __at_cmd_send(p_gprs, buf);
    if (__at_cmd_result_wait(p_gprs, "OK", 3000) != AW_OK) {
        return -AW_ENOMSG;
    }
    __socket_put(p_this, hd);
    return AW_OK;
}

/**
 * \brief 使用at命令发送gprs数据，注意发送的数据里不能有回车符'\r'， 如果有则需要自行进行转义掉
 *
 * \param  p_gprs_dev     gprs设备结构体变量
 * \param  p_dat          要发送的数据
 *
 * \return aw_err_t的错误类型
 */
aw_local aw_err_t __tcpudp_send (struct awbl_gprs_dev *p_gprs,
                                 int                   hd,
                                 void                 *p_dat,
                                 size_t                len,
                                 uint32_t              timeout)
{
    char        *buf = p_gprs->at_buf;

    sprintf(buf, "AT^IPSEND=%d,\"%s\"\r", hd, (char *)p_dat);
    __at_cmd_send(p_gprs, buf);
    if (__at_cmd_result_wait(p_gprs, "OK", timeout) != AW_OK) {
        return -AW_ENOMSG;
    }
    return len;
}
aw_local aw_err_t __tcpudp_recv (struct awbl_gprs_dev *p_gprs,
                                 int                   hd,
                                 void                 *p_buf,
                                 size_t                bsize)
{
    return -AW_ENOTSUP;
}


/******************************************************************************/
static aw_err_t __gprs_drvopr_init ( struct awbl_gprs_ec20_dev  *p_this,
                                     struct awbl_gprs_dev       *p_gprs)
{
    p_gprs->drvopr.pfn_init_prev        = __init_prev;
    if (!p_gprs->drvopr.pfn_ll_type_get) {
        p_gprs->drvopr.pfn_ll_type_get      = __ll_type_get;
    }
    p_gprs->drvopr.pfn_tcpudp_conn      = __tcpudp_conn;
    p_gprs->drvopr.pfn_tcpudp_disconn   = __tcpudp_disconn;
    p_gprs->drvopr.pfn_tcpudp_send      = __tcpudp_send;
    p_gprs->drvopr.pfn_tcpudp_recv      = __tcpudp_recv;

    return AW_OK;
}


/******************************************************************************/

/**
 * \brief method "__gnss_serv_get" handler
 */
aw_local aw_err_t awbl_ec20_dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    aw_err_t err;
    __EC20_DEV_DECL(p_this, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;

    if (!p_dev) {
        return -AW_EINVAL;
    }

    p_gprs = &p_this->gprs;

    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);

    p_gprs->p_gprs_dev  = (void *)p_this;
    err = __gprs_drvopr_init(p_this, p_gprs);
    aw_assert(!err);

    *(struct awbl_gprs_dev **)p_arg = p_gprs;
    AW_INFOF(("%s driver register.\n", p_gprs->info.devname));
    return AW_OK;
}

static void __dev_connect (struct awbl_dev *p_dev)
{
}

/******************************************************************************/
/* 初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    __dev_connect
};

AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_const struct awbl_dev_method __g_gprs_ec20_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, awbl_ec20_dev_get),
    AWBL_METHOD_END
};


/* 驱动注册信息*/
aw_local aw_const awbl_plb_drvinfo_t __g_ec20_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_EC20_NAME,                /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_ec20_methods[0],          /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

/* EC20 驱动注册函数*/
void awbl_gprs_ec20_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_ec20_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}
/******************************************************************************/
/* end of file*/
