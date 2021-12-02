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
#ifndef __AWBL_HWCONF_GPRS_U9300C_H
#define __AWBL_HWCONF_GPRS_U9300C_H

#ifdef AW_DEV_GPRS_U9300C

#include "driver/gprs/awbl_gprs_u9300c.h"
#include "aw_serial.h"

/*******************************************************************************
    ÅäÖÃÐÅÏ¢
*******************************************************************************/
#define LONGSUNG_U8300C_PID                  0x9B05
#define LONGSUNG_U8300C_VID                  0x1C9E
#define LONGSUNG_U9300C_PID                  0x9B3C
#define LONGSUNG_U9300C_VID                  0x1C9E

#define U9300C_ITF_MODEM              1
#define U9300C_ITF_AT                 2
#define U9300C_ITF_PIPE               3
#define U9300C_ITF_NDIS               4

aw_local void __gprs_u9300c_power_on (void)
{
    // todo init ?

//    aw_gpio_set(GPIO3_5, 1);
}

aw_local void __gprs_u9300c_power_off (void)
{
//    aw_gpio_set(GPIO3_5, 0);
}

aw_local void __gprs_u9300c_reset (void)
{
    __gprs_u9300c_power_on();
    aw_mdelay(2000);
    __gprs_u9300c_power_off();
}

aw_local void __gprs_u9300c_write_en (void)
{

}

static aw_err_t __gprs_u9300c_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}

aw_local char __g_u9300c_msg_buffer[2048];

aw_local struct awbl_gprs_u9300c_dev __g_u9300c_dev = {
   .gprs.dev_id         = AWBL_GPRS_U9300C_ID_BASE,

   .gprs.usbh_info.vid        = LONGSUNG_U9300C_VID,
   .gprs.usbh_info.pid        = LONGSUNG_U9300C_PID,
   .gprs.usbh_info.at_com     = U9300C_ITF_AT,
   .gprs.usbh_info.ppp_com    = U9300C_ITF_MODEM,
   .gprs.usbh_info.dm_com     = -1,
   .gprs.usbh_info.gnss_com   = -1,

   .gprs.at_buf         = __g_u9300c_msg_buffer,
   .gprs.at_bufsz       = sizeof(__g_u9300c_msg_buffer),

   .gprs.info.devname   = "LONGSUNG_U9300C",
   .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},
   .gprs.info.p_apn     = NULL,
   .gprs.info.power_on_mdelay   = 20000,
   .gprs.info.sf_reset_mdelay   = 30000,

   .gprs.drvopr.pfn_hw_power_on   = __gprs_u9300c_power_on,
   .gprs.drvopr.pfn_hw_power_off  = __gprs_u9300c_power_off,
   .gprs.drvopr.pfn_hw_reset      = __gprs_u9300c_reset,
   .gprs.drvopr.pfn_hw_write_en   = __gprs_u9300c_write_en,
   .gprs.drvopr.pfn_ll_type_get   = __gprs_u9300c_ll_type_get,
};

#define AWBL_HWCONF_GPRS_U9300C                           \
    {                                                     \
        AWBL_GPRS_U9300C_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_u9300c_dev.dev,           \
        NULL                                              \
    },

#else
#define AWBL_HWCONF_GPRS_U9300C

#endif    /* AW_DEV_GPRS_U9300C */

#endif  /* __AWBL_HWCONF_GPRS_U9300C_H */

/* end of file */
