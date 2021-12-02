/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef  __AWBL_HWCONF_GPIO_WDT_H
#define  __AWBL_HWCONF_GPIO_WDT_H

#ifdef  AW_DEV_GPIO_WDT

#include "driver/wdt/awbl_gpio_wdt.h"


/* GPIO喂狗设备信息 */
aw_local aw_const struct awbl_gpio_wdt_param __g_gpio_wdt_param = {
    GPIO5_0, 1000
};

/* 直流蜂鸣器设备实例内存静态分配 */
aw_local struct awbl_gpio_wdt_dev __g_gpio_wdt_dev;

#define  AWBL_HWCONF_GPIO_WDT   \
    {                           \
        GPIO_WDT_NAME,          \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &(__g_gpio_wdt_dev.dev),\
        &(__g_gpio_wdt_param)   \
    },

#else   /* AW_DEV_GPIO_WDT */
#define AWBL_HWCONF_GPIO_WDT

#endif

#endif  /* __AWBL_HWCONF_GPIO_WDT_H */
