/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/
#ifndef __AWBL_HWCONF_GPRS_L610_H
#define __AWBL_HWCONF_GPRS_L610_H

#ifdef AW_DEV_GPRS_L610

#include "awbl_gprs_default_drv.h"

/*******************************************************************************
    ÅäÖÃÐÅÏ¢
*******************************************************************************/
aw_local void __gprs_l610_power_on (void)
{
    aw_mdelay(10000);
}

aw_local void __gprs_l610_power_off (void)
{
}

aw_local void __gprs_l610_reset (void)
{
}

aw_local void __gprs_l610_write_en (void)
{
}

static aw_err_t __gprs_l610_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}

aw_local char __g_l610_msg_buffer[2048];


aw_local struct awbl_gprs_default_dev __g_l610_dev = {
    .gprs.dev_id         = AWBL_GPRS_L610_ID_BASE,

    .gprs.usbh_info.vid        = 0x1782,
    .gprs.usbh_info.pid        = 0x4d10,
    .gprs.usbh_info.at_com     = 5,
    .gprs.usbh_info.ppp_com    = 0,
    .gprs.usbh_info.dm_com     = 2,
    .gprs.usbh_info.gnss_com   = 3,

    .gprs.at_buf         = __g_l610_msg_buffer,
    .gprs.at_bufsz       = sizeof(__g_l610_msg_buffer),

    .gprs.info.devname   = "FIBOCOM 'L610'",
    .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},
    .gprs.info.p_apn     = NULL,

    .gprs.drvopr.pfn_hw_power_on   = __gprs_l610_power_on,
    .gprs.drvopr.pfn_hw_power_off  = __gprs_l610_power_off,
    .gprs.drvopr.pfn_hw_reset      = __gprs_l610_reset,
    .gprs.drvopr.pfn_hw_write_en   = __gprs_l610_write_en,
    .gprs.drvopr.pfn_ll_type_get   = __gprs_l610_ll_type_get,
};



#define AWBL_HWCONF_GPRS_L610                             \
    {                                                     \
        AWBL_GPRS_DEFAULT_NAME,                           \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_l610_dev.dev,             \
        NULL                                              \
    },

#else
#define AWBL_HWCONF_GPRS_L610

#endif    /* AW_DEV_GPRS_EC20 */

#endif  /* __AWBL_HWCONF_GPRS_L610_H */

/* end of file */
