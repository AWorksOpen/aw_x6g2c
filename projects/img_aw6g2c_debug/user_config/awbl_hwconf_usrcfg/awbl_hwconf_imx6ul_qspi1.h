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

#ifndef __AWBL_HWCONF_IMX6UL_QSPI1_H
#define __AWBL_HWCONF_IMX6UL_QSPI1_H

#ifdef AW_DEV_IMX6UL_QSPI_1

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/spi/awbl_imx6ul_qspi.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"


#define   __IMX6UL_QSPI_PAD_CTL        IMX6UL_PAD_CTL(                 \
                                           PUS_1_47K_Ohm_Pull_Up     | \
                                           PKE_1_Pull_Keeper_Enabled | \
                                           PUE_1_Pull                | \
                                           Open_Drain_Disabled       | \
                                           SPEED_3_max_200MHz        | \
                                           DSE_1_R0                  | \
                                           SRE_0_Fast_Slew_Rate)


/* 平台相关初始化 */
aw_local void __imx6ul_qspi1_plfm_init (void)
{
    int qspi1_gpios[] = {GPIO4_0, GPIO4_1, GPIO4_4, GPIO4_5, GPIO4_6, GPIO4_7};
    /* 使能时钟 */
    aw_clk_enable(IMX6UL_CLK_QSPI);

    if (aw_gpio_pin_request("qspi1_gpios",
                             qspi1_gpios,
                             AW_NELEMENTS(qspi1_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO4_0, GPIO4_0_QSPI_B_SCLK   | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_SCLK */
        aw_gpio_pin_cfg(GPIO4_1, GPIO4_1_QSPI_B_SS0_B  | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_SS0 */
        aw_gpio_pin_cfg(GPIO4_4, GPIO4_4_QSPI_B_DATA00 | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_D0 */
        aw_gpio_pin_cfg(GPIO4_5, GPIO4_5_QSPI_B_DATA01 | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_D1 */
        aw_gpio_pin_cfg(GPIO4_6, GPIO4_6_QSPI_B_DATA02 | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_D2 */
        aw_gpio_pin_cfg(GPIO4_7, GPIO4_7_QSPI_B_DATA03 | __IMX6UL_QSPI_PAD_CTL);  /* QSPIB_D3 */
    }
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_qspi_devinfo __g_imx6ul_qspi1_devinfo = {
    {
        IMX6UL_QSPI1_BUSID,        /* 总线编号 */
    },
    IMX6UL_QSPI_BASE_ADDR,         /* QSPI1寄存器基地址 */
    INUM_QSPI,                     /* 中断编号 */
    66000000,                      /* 输入时钟频率 ，此值决定了SPI允许的的最大和最小速率，不能设为0 */
    {
        0x60000000,                /* QSPI A1 map addr(must be 0x60000000)  */
        0x60000000,                /* QSPI A2 map addr  */
        0x60000000,                /* QSPI B1 map addr  */
        0x60800000,                /* QSPI B2 map addr */
        0x60800000,                /* QSPI end map addr */
    },
    2,                             /* QSPI FLASH B1(0x60000000 ~ 0x60800000) */
    3,                             /* 3 means address width 24bit*/
    AWBL_QSPI_NOR_FAST,
    AWBL_QSPI_OP_READ_FAST,
    AWBL_QSPI_OP_PP,
    AWBL_QSPI_OP_BE_4K,
    0x08,
    8,
    __imx6ul_qspi1_plfm_init       /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_qspi_dev __g_imx6ul_qspi1_dev;

#define AWBL_HWCONF_IMX6UL_QSPI1                    \
    {                                               \
        AWBL_IMX6UL_QSPI_NAME,                      \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_qspi1_dev,   \
        &__g_imx6ul_qspi1_devinfo                   \
    },

#else
#define AWBL_HWCONF_IMX6UL_QSPI1

#endif /* AW_DEV_IMX6UL_QSPI_1 */

#endif /* __AWBL_HWCONF_IMX6UL_QSPI1_H */

/* end of file */
