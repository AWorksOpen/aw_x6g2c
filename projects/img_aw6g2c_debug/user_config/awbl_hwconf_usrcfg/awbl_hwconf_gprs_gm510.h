/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/
#ifndef __AWBL_HWCONF_GPRS_GM510_H
#define __AWBL_HWCONF_GPRS_GM510_H

#ifdef AW_DEV_GPRS_GM510

#include "driver/gprs/awbl_gprs_gm510.h"
#include "aw_delay.h"

static void __gm510_null (void)
{
}


aw_local struct awbl_gprs_gm510_dev __g_gm510_dev = {
    .gprs.dev_id         = 6,

    .gprs.usbh_info.vid        = 0x305a,
    .gprs.usbh_info.pid        = 0x1415,
    .gprs.usbh_info.at_com     = 3,
    .gprs.usbh_info.ppp_com    = 4,
    .gprs.usbh_info.dm_com     = 0,
    .gprs.usbh_info.gnss_com   = 1,

//    .gprs.at_buf         = __g_gm510_msg_buffer,
//    .gprs.at_bufsz       = sizeof(__g_gm510_msg_buffer),

    .gprs.info.devname   = "GM510",
    .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},
    .gprs.info.p_apn     = NULL,

    .gprs.drvopr.pfn_hw_power_on   = __gm510_null,
    .gprs.drvopr.pfn_hw_power_off  = __gm510_null,
    .gprs.drvopr.pfn_hw_reset      = __gm510_null,
    .gprs.drvopr.pfn_hw_write_en   = __gm510_null,
//    .gprs.drvopr.pfn_ll_type_get   = __gm510_ll_type_get,
};


#define AWBL_HWCONF_GPRS_GM510                           \
    {                                                     \
        AWBL_GPRS_GM510_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_gm510_dev.dev,           \
        NULL                                           \
    },

#else
#define AWBL_HWCONF_GPRS_GM510

#endif    /* AW_DEV_GPRS_GM510 */

#endif  /* __AWBL_HWCONF_GPRS_GM510_H */
