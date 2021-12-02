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
#ifndef __AWBL_HWCONF_GPRS_EC200T_CN_H
#define __AWBL_HWCONF_GPRS_EC200T_CN_H

#ifdef AW_DEV_GPRS_EC20

#include "driver/gprs/awbl_gprs_ec20.h"

/*******************************************************************************
    ≈‰÷√–≈œ¢
*******************************************************************************/
aw_local void __gprs_ec200t_power_on (void)
{
    // todo init ?

    aw_gpio_set(GPIO3_20, 1);
}

aw_local void __gprs_ec200t_power_off (void)
{
    aw_gpio_set(GPIO3_20, 0);
}

aw_local void __gprs_ec200t_reset (void)
{
    __gprs_ec200t_power_on();
    aw_mdelay(2000);
    __gprs_ec200t_power_off();
}

aw_local void __gprs_ec200t_write_en (void)
{

}


aw_local char __g_ec200t_msg_buffer[2048];

aw_local struct awbl_gprs_ec20_dev __g_ec200t_dev = {
    .gprs.dev_id         = AWBL_GPRS_EC20_ID_BASE + 1,

    .gprs.usbh_info.vid        = 0x2C7C,
    .gprs.usbh_info.pid        = 0x6026,
    .gprs.usbh_info.at_com     = 3,
    .gprs.usbh_info.ppp_com    = 4,
    .gprs.usbh_info.dm_com     = -1,
    .gprs.usbh_info.gnss_com   = -1,

    .gprs.at_buf         = __g_ec200t_msg_buffer,
    .gprs.at_bufsz       = sizeof(__g_ec200t_msg_buffer),

    .gprs.info.devname   = "Quectel_EC200T",
    .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},
    .gprs.info.p_apn     = NULL,

    .gprs.drvopr.pfn_hw_power_on   = __gprs_ec200t_power_on,
    .gprs.drvopr.pfn_hw_power_off  = __gprs_ec200t_power_off,
    .gprs.drvopr.pfn_hw_reset      = __gprs_ec200t_reset,
    .gprs.drvopr.pfn_hw_write_en   = __gprs_ec200t_write_en,
//    .gprs.drvopr.pfn_ll_type_get   = __gprs_ec200t_ll_type_get,
};

aw_local struct awbl_gprs_ec20_dev __g_ec200t_dev;

#define AWBL_HWCONF_GPRS_EC200T                           \
    {                                                     \
        AWBL_GPRS_EC20_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_ec200t_dev.dev,           \
        NULL                                               \
    },

#else
#define AWBL_HWCONF_GPRS_EC200T

#endif    /* AW_DEV_GPRS_EC20 */

#endif  /* __AWBL_HWCONF_GPRS_EC200T_CN_H */

/* end of file */
