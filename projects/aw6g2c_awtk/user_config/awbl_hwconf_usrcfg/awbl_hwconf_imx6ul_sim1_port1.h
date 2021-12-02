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

#ifndef __AWBL_HWCONF_IMX6UL_SIM1_PORT1_H
#define __AWBL_HWCONF_IMX6UL_SIM1_PORT1_H

#ifdef AW_DEV_IMX6UL_SIM1_PORT1

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/sim/awbl_imx6ul_sim.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"

#define SIM_CLK_PAD_CTRL  (DSE_6_R0_6   | SRE_0_Fast_Slew_Rate)

#define SIM_PAD_CTRL  (PUS_2_100K_Ohm_Pull_Up | PKE_1_Pull_Keeper_Enabled | PUE_1_Pull|    \
                         SPEED_3_max_200MHz   |                                   \
                         DSE_5_R0_5   | SRE_0_Fast_Slew_Rate)

#define __IMX6UL_SIM_RCV_BUF_SIZE   400
#define __IMX6UL_SIM_XMT_BUF_SIZE   300

aw_local uint8_t __imx6ul_sim1_port1_rx_buf[__IMX6UL_SIM_RCV_BUF_SIZE];
aw_local uint8_t __imx6ul_sim1_port1_tx_buf[__IMX6UL_SIM_XMT_BUF_SIZE];

/* 平台相关初始化 */
aw_local void __imx6ul_sim1_port1_plfm_init (void)
{
    int sim1_port1_gpios[] = {GPIO4_19, GPIO4_21, GPIO4_22, GPIO4_23};
    if (aw_gpio_pin_request("sim1_port1_gpios",
                             sim1_port1_gpios,
                             AW_NELEMENTS(sim1_port1_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO4_19, GPIO4_19_SIM1_PORT1_CLK | IMX6UL_PAD_CTL(SIM_CLK_PAD_CTRL));  /* SIM1 PORT1 CLK */
        aw_gpio_pin_cfg(GPIO4_21, GPIO4_21_SIM1_PORT1_RST_B | IMX6UL_PAD_CTL(SIM_PAD_CTRL));    /* SIM1 PORT1 RST */
        aw_gpio_pin_cfg(GPIO4_22, GPIO4_22_SIM1_PORT1_SVEN | IMX6UL_PAD_CTL(SIM_PAD_CTRL));     /* SIM1 PORT1 SVEN */
        aw_gpio_pin_cfg(GPIO4_23, GPIO4_23_SIM1_PORT1_TRXD | IMX6UL_PAD_CTL(SIM_PAD_CTRL));     /* SIM1 PORT1 TXRD */
    }
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_sim_devinfo __g_imx6ul_sim1_port1_devinfo = {
    {
        1,                        /* 设备ID */
    },
    IMX6UL_SIM1_BASE_ADDR,        /* sim1寄存器基地址 */
    INUM_SIM1,                    /* 中断编号 */
    IMX6UL_CLK_SIM1,                /* SIM时钟ID */
    1,
    AW_TRUE,
    __imx6ul_sim1_port1_rx_buf,
    __IMX6UL_SIM_RCV_BUF_SIZE,
    __imx6ul_sim1_port1_tx_buf,
    __IMX6UL_SIM_XMT_BUF_SIZE,
    __imx6ul_sim1_port1_plfm_init       /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_sim_dev __g_imx6ul_sim1_port1_dev;

#define AWBL_HWCONF_IMX6UL_SIM1_PORT1                   \
    {                                                   \
        AWBL_IMX6UL_SIM_NAME,                           \
        0,                                              \
        AWBL_BUSID_PLB,                                 \
        0,                                              \
        (struct awbl_dev *)&__g_imx6ul_sim1_port1_dev,  \
        &__g_imx6ul_sim1_port1_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX6UL_SIM1_PORT1

#endif /* AW_DEV_IMX6UL_SIM1_PORT1 */

#endif /* __AWBL_HWCONF_IMX6UL_SIM1_PORT1_H */

/* end of file */
