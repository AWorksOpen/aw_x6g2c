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
#ifndef __AWBL_HWCONF_GPRS_ME909S_H
#define __AWBL_HWCONF_GPRS_ME909S_H

#ifdef AW_DEV_GPRS_ME909S

#include "driver/gprs/awbl_gprs_me909s.h"
#include "aw_delay.h"
/*******************************************************************************
    ÅäÖÃÐÅÏ¢
*******************************************************************************/
aw_local void __gprs_me909s_power_on (void)
{
    // todo init ?
    aw_local char init = AW_FALSE;
    if (!init) {
        init = AW_TRUE;

        aw_gpio_pin_cfg(GPIO3_4, AW_GPIO_OUTPUT | AW_GPIO_PUSH_PULL);
        aw_gpio_set(GPIO3_4, 0);

        aw_gpio_pin_cfg(GPIO5_6, AW_GPIO_OUTPUT | AW_GPIO_PUSH_PULL);
        aw_gpio_set(GPIO5_6, 1);
    }

    aw_gpio_set(GPIO3_4, 1);

}

aw_local void __gprs_me909s_power_off (void)
{
    aw_gpio_set(GPIO3_4, 0);
}

aw_local void __gprs_me909s_reset (void)
{
    aw_gpio_set(GPIO5_6, 0);
    aw_mdelay(1000);
    aw_gpio_set(GPIO5_6, 1);
}

aw_local void __gprs_me909s_write_en (void)
{

}

static aw_err_t __gprs_me909s_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ll_type = AWBL_GPRS_LL_TYPE_PPP;
    return AW_OK;
}

aw_local char __g_me909s_msg_buffer[600];

aw_local struct awbl_gprs_me909s_dev __g_me909s_dev = {
    .gprs.dev_id         = AWBL_GPRS_ME909S_ID_BASE,

    .gprs.usbh_info.vid        = 0x12d1,
    .gprs.usbh_info.pid        = 0x15c1,
    .gprs.usbh_info.at_com     = 2,
    .gprs.usbh_info.ppp_com    = 0,
    .gprs.usbh_info.dm_com     = -1,
    .gprs.usbh_info.gnss_com   = -1,

    .gprs.at_buf         = __g_me909s_msg_buffer,
    .gprs.at_bufsz       = sizeof(__g_me909s_msg_buffer),

    .gprs.info.devname   = "Huawei_ME909S",
    .gprs.info.p_at_err  = { "ERROR", "+CME", "+CMS"},
    .gprs.info.p_apn     = NULL,

    .gprs.drvopr.pfn_hw_power_on   = __gprs_me909s_power_on,
    .gprs.drvopr.pfn_hw_power_off  = __gprs_me909s_power_off,
    .gprs.drvopr.pfn_hw_reset      = __gprs_me909s_reset,
    .gprs.drvopr.pfn_hw_write_en   = __gprs_me909s_write_en,
    .gprs.drvopr.pfn_ll_type_get   = __gprs_me909s_ll_type_get,
};

#define AWBL_HWCONF_GPRS_ME909S                           \
    {                                                     \
        AWBL_GPRS_ME909S_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_me909s_dev.dev,           \
        NULL                                              \
    },

#else
#define AWBL_HWCONF_GPRS_ME909S

#endif    /* AW_DEV_GPRS_ME909S */

#endif  /* __AWBL_HWCONF_GPRS_ME909S_H */

/* end of file */
