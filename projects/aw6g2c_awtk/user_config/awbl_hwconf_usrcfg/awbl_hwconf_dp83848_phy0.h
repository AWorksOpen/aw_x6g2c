/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_DP83848_PHY0_H
#define __AWBL_HWCONF_DP83848_PHY0_H

#ifdef AW_DEV_DP83848_PHY0

#include "aw_gpio.h"
#include "aw_delay.h"

#include "driver/phy/awbl_dp83848_phy.h"

/* 硬件 PHY 地址 */
#define  DP83848_PHY0_ADDR       0x05

/* PHY复位引脚 */
#define  DP83848_PHY0_RESET_PIN  GPIO5_4

/* 平台相关初始化 */
aw_local void __dp83848_phy0_plfm_init (void)
{
    aw_gpio_pin_cfg(DP83848_PHY0_RESET_PIN, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);

    /**
     * Hard Reset PHY
     */
    aw_gpio_set(DP83848_PHY0_RESET_PIN, 1);
    aw_udelay(1000);
    aw_gpio_set(DP83848_PHY0_RESET_PIN, 0);
    aw_udelay(1000);          /* 大于1us的低电平将会复位PHY芯片（DP83848）*/
    aw_gpio_set(DP83848_PHY0_RESET_PIN, 1);
}

/* 设备信息 */
aw_local aw_const struct awbl_dp83848_phy_devinfo __g_dp83848_phy0_devinfo = {
    "eth0",
    DP83848_PHY0_ADDR,          /* phy addr */
    AWBL_PHY_LINK_AUTONEG,      /* PHY mode */
    __dp83848_phy0_plfm_init,   /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_dp83848_phy_dev __g_dp83848_phy0_dev;

#define AWBL_HWCONF_DP83848_PHY0                        \
    {                                                   \
        AWBL_DP83848_PHY_NAME,                          \
        0,                                              \
        AWBL_BUSID_MII,                                 \
        IMX6UL_MII0_BUSID,                              \
        (struct awbl_dev *)&__g_dp83848_phy0_dev,       \
        &__g_dp83848_phy0_devinfo                       \
    },

#else
#define AWBL_HWCONF_DP83848_PHY0

#endif  /* AW_DEV_DP83848_PHY0 */

#endif  /* __AWBL_HWCONF_DP83848_PHY0_H */

/* end of file */
