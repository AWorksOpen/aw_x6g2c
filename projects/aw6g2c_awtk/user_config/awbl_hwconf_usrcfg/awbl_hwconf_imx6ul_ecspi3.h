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

#ifndef __AWBL_HWCONF_IMX6UL_ECSPI3_H
#define __AWBL_HWCONF_IMX6UL_ECSPI3_H

#ifdef AW_DEV_IMX6UL_ECSPI_3

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/spi/awbl_imx6ul_ecspi.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"

#define ECSPI_CLK_PAD_CTRL  (DSE_6_R0_6   | SRE_0_Fast_Slew_Rate)

#define ECSPI_PAD_CTRL  (PUS_2_100K_Ohm_Pull_Up | PKE_1_Pull_Keeper_Enabled | PUE_1_Pull|    \
                         SPEED_3_max_200MHz   |                                   \
                         DSE_5_R0_5   | SRE_0_Fast_Slew_Rate)

/* 平台相关初始化 */
aw_local void __imx6ul_ecspi3_plfm_init (void)
{
    int ecspi3_gpios[] = {GPIO1_21, GPIO1_22, GPIO1_23};
    if (aw_gpio_pin_request("ecspi3_gpios",
                             ecspi3_gpios,
                             AW_NELEMENTS(ecspi3_gpios)) == AW_OK) {

        /* 配置功能引脚 */
         aw_gpio_pin_cfg(GPIO1_21, GPIO1_21_ECSPI3_SCLK | IMX6UL_PAD_CTL(ECSPI_CLK_PAD_CTRL));  /* ECSPI_SCK */
         aw_gpio_pin_cfg(GPIO1_22, GPIO1_22_ECSPI3_MOSI | IMX6UL_PAD_CTL(ECSPI_PAD_CTRL));  /* ECSPI_MOSI */
         aw_gpio_pin_cfg(GPIO1_23, GPIO1_23_ECSPI3_MISO | IMX6UL_PAD_CTL(ECSPI_PAD_CTRL));  /* ECSPI_MISO */

    }

    aw_clk_enable(IMX6UL_CLK_ECSPI3);
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_ecspi_devinfo __g_imx6ul_ecspi3_devinfo = {
    {
        IMX6UL_ECSPI3_BUSID,        /* 总线编号 */
    },
    IMX6UL_ECSPI3_BASE_ADDR,        /* ssp0寄存器基地址 */
    INUM_ESPI3_IRQ,                 /* 中断编号 */
    0,
    0,                              /* 无cs延时 */
    0,                              /* 延时时钟选择SPI时钟 */
    0,                              /* 无延时 */
    IMX6UL_CLK_ECSPI3,
    __imx6ul_ecspi3_plfm_init       /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_ecspi_dev __g_imx6ul_ecspi3_dev;

#define AWBL_HWCONF_IMX6UL_ECSPI3                   \
    {                                               \
        AWBL_IMX6UL_ECSPI_NAME,                     \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_ecspi3_dev,  \
        &__g_imx6ul_ecspi3_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX6UL_ECSPI3

#endif /* AW_DEV_IMX6UL_ECSPI_3 */

#endif /* __AWBL_HWCONF_IMX6UL_ECSPI1_H */

/* end of file */
