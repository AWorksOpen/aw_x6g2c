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

#ifndef __AWBL_HWCONF_IMX6UL_ECSPI2_H
#define __AWBL_HWCONF_IMX6UL_ECSPI2_H

#ifdef AW_DEV_IMX6UL_ECSPI_2

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/spi/awbl_imx6ul_ecspi.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"

#define ECSPI_CLK_PAD_CTRL  (DSE_6_R0_6   | SRE_0_Fast_Slew_Rate)

#define ECSPI_PAD_CTRL  (PUS_2_100K_Ohm_Pull_Up | PKE_1_Pull_Keeper_Enabled | PUE_1_Pull|    \
                         SPEED_3_max_200MHz   |                                   \
                         DSE_5_R0_5   | SRE_0_Fast_Slew_Rate)

/* 平台相关初始化 */
aw_local void __imx6ul_ecspi2_plfm_init (void)
{
    int ecspi2_gpios[] = {GPIO1_28, GPIO1_30, GPIO1_31};
    if (aw_gpio_pin_request("ecspi2_gpios",
                             ecspi2_gpios,
                             AW_NELEMENTS(ecspi2_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO1_28, GPIO1_28_ECSPI2_SCLK | IMX6UL_PAD_CTL(ECSPI_CLK_PAD_CTRL));  /* ECSPI_SCK */
        aw_gpio_pin_cfg(GPIO1_30, GPIO1_30_ECSPI2_MOSI | IMX6UL_PAD_CTL(ECSPI_PAD_CTRL));  /* ECSPI_MOSI */
        aw_gpio_pin_cfg(GPIO1_31, GPIO1_31_ECSPI2_MISO | IMX6UL_PAD_CTL(ECSPI_PAD_CTRL));  /* ECSPI_MISO */
    }

    aw_clk_enable(IMX6UL_CLK_ECSPI2);
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_ecspi_devinfo __g_imx6ul_ecspi2_devinfo = {
    {
        IMX6UL_ECSPI2_BUSID,        /* 总线编号 */
    },
    IMX6UL_ECSPI2_BASE_ADDR,        /* ssp0寄存器基地址 */
    INUM_ESPI2_IRQ,                 /* 中断编号 */
    0,
    0,                              /* 无cs延时 */
    0,                              /* 延时时钟选择SPI时钟 */
    0,                              /* 无延时 */
    IMX6UL_CLK_ECSPI2,
    __imx6ul_ecspi2_plfm_init       /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_ecspi_dev __g_imx6ul_ecspi2_dev;

#define AWBL_HWCONF_IMX6UL_ECSPI2                   \
    {                                               \
        AWBL_IMX6UL_ECSPI_NAME,                     \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_ecspi2_dev,  \
        &__g_imx6ul_ecspi2_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX6UL_ECSPI2

#endif /* AW_DEV_IMX6UL_ECSPI_2 */

#endif /* __AWBL_HWCONF_IMX6UL_ECSPI2_H */

/* end of file */
