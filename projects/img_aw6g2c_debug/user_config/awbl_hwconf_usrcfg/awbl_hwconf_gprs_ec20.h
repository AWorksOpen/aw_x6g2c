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
#ifndef __AWBL_HWCONF_GPRS_EC20_H
#define __AWBL_HWCONF_GPRS_EC20_H

#ifdef AW_DEV_GPRS_EC20

#include "driver/gprs/awbl_gprs_ec20.h"

/*******************************************************************************
    ÅäÖÃÐÅÏ¢
*******************************************************************************/
aw_local void __gprs_ec20_power_on (void)
{
    // todo init ?

//    aw_gpio_set(GPIO3_20, 1);
}

aw_local void __gprs_ec20_power_off (void)
{
//    aw_gpio_set(GPIO3_20, 0);
}

aw_local void __gprs_ec20_reset (void)
{
//    __gprs_ec20_power_on();
//    aw_mdelay(2000);
//    __gprs_ec20_power_off();
}

aw_local void __gprs_ec20_write_en (void)
{

}

static aw_err_t __gprs_ec20_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}

aw_local char __g_ec20_msg_buffer[2048];

aw_local struct awbl_gprs_ec20_dev __g_ec20_dev = {
    .gprs.dev_id         = AWBL_GPRS_EC20_ID_BASE,

    .gprs.usbh_info.vid        = 0x2C7C,
    .gprs.usbh_info.pid        = 0x0125,
    .gprs.usbh_info.at_com     = 2,
    .gprs.usbh_info.ppp_com    = 3,
    .gprs.usbh_info.dm_com     = 0,
    .gprs.usbh_info.gnss_com   = 1,

    .gprs.at_buf         = __g_ec20_msg_buffer,
    .gprs.at_bufsz       = sizeof(__g_ec20_msg_buffer),

    .gprs.info.devname   = "Quectel_EC20",
    .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},

    .gprs.drvopr.pfn_hw_power_on   = __gprs_ec20_power_on,
    .gprs.drvopr.pfn_hw_power_off  = __gprs_ec20_power_off,
    .gprs.drvopr.pfn_hw_reset      = __gprs_ec20_reset,
    .gprs.drvopr.pfn_hw_write_en   = __gprs_ec20_write_en,
    .gprs.drvopr.pfn_ll_type_get   = __gprs_ec20_ll_type_get,
};


#define AWBL_HWCONF_GPRS_EC20                           \
    {                                                     \
        AWBL_GPRS_EC20_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_ec20_dev.dev,           \
        NULL                                           \
    },

#else
#define AWBL_HWCONF_GPRS_EC20

#endif    /* AW_DEV_GPRS_EC20 */

#endif  /* __AWBL_HWCONF_GPRS_EC20_H */

/* end of file */
