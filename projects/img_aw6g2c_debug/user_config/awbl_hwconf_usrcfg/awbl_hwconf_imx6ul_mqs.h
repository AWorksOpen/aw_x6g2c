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

#ifndef  __AWBL_HWCONF_IMX6UL_MQS_H
#define  __AWBL_HWCONF_IMX6UL_MQS_H

#ifdef AW_DEV_IMX6UL_MQS

/*******************************************************************************
 MQS控制器配置信息
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/mqs/awbl_imx6ul_mqs.h"
#include "aw_gpio.h"
#include "imx6ul_regs_base.h"

#define __IMX6UL_MQS_PAD_CTRL             IMX6UL_PAD_CTL(              \
                                          PUS_0_100K_Ohm_Pull_Down   | \
                                          PKE_0_Pull_Keeper_Disabled | \
                                          PUE_0_Keeper               | \
                                          Open_Drain_Disabled        | \
                                          SPEED_1_medium_100MHz      | \
                                          DSE_7_R0_7                 | \
                                          SRE_0_Fast_Slew_Rate)

aw_local void __imx6ul_mqs_plat_init (void)
{

    int mqs_gpio[] = {GPIO3_27, GPIO3_28};
    if (aw_gpio_pin_request("mqs_gpio",
                             mqs_gpio,
                             AW_NELEMENTS(mqs_gpio)) == AW_OK) {

        /* MQS Right */
        aw_gpio_pin_cfg(GPIO3_27, GPIO3_27_MQS_RIGHT |
                                 __IMX6UL_MQS_PAD_CTRL);

        /* MQS Left */
        aw_gpio_pin_cfg(GPIO3_28, GPIO3_28_MQS_LEFT |
                                 __IMX6UL_MQS_PAD_CTRL);
    }

//    *(volatile uint32_t *)0x20E0170 = 0x0011;
//    *(volatile uint32_t *)0x20E0174 = 0x0011;
//
//    *(volatile uint32_t *)0x20E03FC = 0x4000007f;
//    *(volatile uint32_t *)0x20E0400 = 0x4000007f;
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_mqs_devinfo __g_imx6ul_mqs_devinfo = {
     0x020E4008,                     /* 寄存器地址 */
     IMX6UL_CLK_SAI1_PODF,           /* 时钟id   */
     __imx6ul_mqs_plat_init,         /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_mqs_dev __g_imx6ul_mqs_dev;

#define AWBL_HWCONF_IMX6UL_MQS                      \
    {                                               \
        AWBL_IMX6UL_MQS_NAME,                       \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_mqs_dev,     \
        &__g_imx6ul_mqs_devinfo                     \
    },


#else

#define AWBL_HWCONF_IMX6UL_MQS

#endif  /* AW_DEV_IMX6UL_MQS */

#endif /* __AWBL_HWCONF_IMX6UL_MQS_H */

/* end of file */
