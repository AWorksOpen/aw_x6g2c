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

#ifndef __AWBL_HWCONF_IMX6UL_SIM1_PORT0_H
#define __AWBL_HWCONF_IMX6UL_SIM1_PORT0_H

#ifdef AW_DEV_IMX6UL_SIM1_PORT0

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

aw_local uint8_t __imx6ul_sim1_port0_rx_buf[__IMX6UL_SIM_RCV_BUF_SIZE];
aw_local uint8_t __imx6ul_sim1_port0_tx_buf[__IMX6UL_SIM_XMT_BUF_SIZE];

/* ƽ̨��س�ʼ�� */
aw_local void __imx6ul_sim1_port0_plfm_init (void)
{
    int sim1_port0_gpios[] = {GPIO2_9, GPIO2_10, GPIO2_11, GPIO2_8};
    if (aw_gpio_pin_request("sim1_port0_gpios",
                             sim1_port0_gpios,
                             AW_NELEMENTS(sim1_port0_gpios)) == AW_OK) {

        /* ���ù������� */
        aw_gpio_pin_cfg(GPIO2_9, GPIO2_9_SIM1_PORT0_CLK | IMX6UL_PAD_CTL(SIM_CLK_PAD_CTRL));  /* SIM1 PORT0 CLK */
        aw_gpio_pin_cfg(GPIO2_10, GPIO2_10_SIM1_PORT0_RST_B | IMX6UL_PAD_CTL(SIM_PAD_CTRL));  /* SIM1 PORT0 RST */
        aw_gpio_pin_cfg(GPIO2_11, GPIO2_11_SIM1_PORT0_SVEN | IMX6UL_PAD_CTL(SIM_PAD_CTRL));   /* SIM1 PORT0 SVEN */
        aw_gpio_pin_cfg(GPIO2_8, GPIO2_8_SIM1_PORT0_TRXD | IMX6UL_PAD_CTL(SIM_PAD_CTRL));     /* SIM1 PORT0 TRXD */
    }
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_sim_devinfo __g_imx6ul_sim1_port0_devinfo = {
    {
        0,                        /* �豸ID */
    },
    IMX6UL_SIM1_BASE_ADDR,        /* sim1�Ĵ�������ַ */
    INUM_SIM1,                    /* �жϱ�� */
    IMX6UL_CLK_SIM1,               /* SIMʱ��ID */
    0,
    AW_TRUE,
    __imx6ul_sim1_port0_rx_buf,
    __IMX6UL_SIM_RCV_BUF_SIZE,
    __imx6ul_sim1_port0_tx_buf,
    __IMX6UL_SIM_XMT_BUF_SIZE,
    __imx6ul_sim1_port0_plfm_init       /* ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_sim_dev __g_imx6ul_sim1_port0_dev;

#define AWBL_HWCONF_IMX6UL_SIM1_PORT0                   \
    {                                                   \
        AWBL_IMX6UL_SIM_NAME,                           \
        0,                                              \
        AWBL_BUSID_PLB,                                 \
        0,                                              \
        (struct awbl_dev *)&__g_imx6ul_sim1_port0_dev,  \
        &__g_imx6ul_sim1_port0_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX6UL_SIM1_PORT0

#endif /* AW_DEV_IMX6UL_SIM1_PORT0 */

#endif /* __AWBL_HWCONF_IMX6UL_SIM1_PORT0_H */

/* end of file */
