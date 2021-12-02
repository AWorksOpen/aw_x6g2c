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

#ifndef __AWBL_HWCONF_IMX6UL_GPMI_H
#define __AWBL_HWCONF_IMX6UL_GPMI_H

#ifdef AW_DEV_IMX6UL_GPMI

#include "driver/busctlr/awbl_imx6ul_gpmi.h"
#include "awbl_nand_bus.h"
#include "imx6ul_regs_base.h"



#define   __IMX6UL_GPMI_PAD_CTL   IMX6UL_PAD_CTL( PUS_1_47K_Ohm_Pull_Up     | \
                                                  PKE_1_Pull_Keeper_Enabled | \
                                                  PUE_1_Pull                | \
                                                  Open_Drain_Disabled       | \
                                                  SPEED_3_max_200MHz        | \
                                                  DSE_1_R0                  | \
                                                  SRE_0_Fast_Slew_Rate)


aw_local void __g_imx6ul_gpmi_plat_init (void)
{
    int gpmi_gpios[] = {GPIO4_10, GPIO4_13, GPIO4_15, GPIO4_2, GPIO4_3, GPIO4_4,
                        GPIO4_5, GPIO4_6, GPIO4_7, GPIO4_8, GPIO4_9, GPIO4_12, GPIO4_0,
                        GPIO4_1, GPIO4_11};
    if (aw_gpio_pin_request("gpmi_gpios",
                             gpmi_gpios,
                             AW_NELEMENTS(gpmi_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO4_10, GPIO4_10_RAWNAND_ALE | __IMX6UL_GPMI_PAD_CTL);  /* NAND_ALE    */

        /* NAND_CE0_B  */
        aw_gpio_pin_cfg(GPIO4_13, GPIO4_13_RAWNAND_CE0_B | __IMX6UL_GPMI_PAD_CTL);

        //aw_gpio_pin_cfg(GPIO4_14, GPIO4_14_RAWNAND_CE1_B );  /* NAND_CE1_B  */
        //aw_gpio_pin_cfg(GPIO4_17, GPIO4_17_RAWNAND_CE2_B );  /* NAND_CE2_B  */
        //aw_gpio_pin_cfg(GPIO4_18, GPIO4_18_RAWNAND_CE3_B );  /* NAND_CE3_B  */

        aw_gpio_pin_cfg(GPIO4_15, GPIO4_15_RAWNAND_CLE | __IMX6UL_GPMI_PAD_CTL);  /* NAND_CLE */

        /* NAND_DATA_0 ~ NAND_DATA_7 */
        aw_gpio_pin_cfg(GPIO4_2, GPIO4_2_RAWNAND_DATA00 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_3, GPIO4_3_RAWNAND_DATA01 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_4, GPIO4_4_RAWNAND_DATA02 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_5, GPIO4_5_RAWNAND_DATA03 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_6, GPIO4_6_RAWNAND_DATA04 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_7, GPIO4_7_RAWNAND_DATA05 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_8, GPIO4_8_RAWNAND_DATA06 | __IMX6UL_GPMI_PAD_CTL);
        aw_gpio_pin_cfg(GPIO4_9, GPIO4_9_RAWNAND_DATA07 | __IMX6UL_GPMI_PAD_CTL);

        /* NAND_READY_B  */
        aw_gpio_pin_cfg(GPIO4_12, GPIO4_12_RAWNAND_READY_B | __IMX6UL_GPMI_PAD_CTL);

        /* NAND_RE_B */
        aw_gpio_pin_cfg(GPIO4_0, GPIO4_0_RAWNAND_RE_B | __IMX6UL_GPMI_PAD_CTL);

        /* NAND_WE_B */
        aw_gpio_pin_cfg(GPIO4_1, GPIO4_1_RAWNAND_WE_B | __IMX6UL_GPMI_PAD_CTL);

        /* NAND_WP_B   */
        aw_gpio_pin_cfg(GPIO4_11, GPIO4_11_RAWNAND_WP_B | __IMX6UL_GPMI_PAD_CTL);

    }


    /* enable the clock */
    aw_clk_enable(IMX6UL_CLK_GPMI_BCH_APB);
    aw_clk_enable(IMX6UL_CLK_GPMI_BCH);
    aw_clk_enable(IMX6UL_CLK_GPMI_IO);
    aw_clk_enable(IMX6UL_CLK_GPMI_APB);
    aw_clk_enable(IMX6UL_CLK_PER_BCH);
}

aw_local aw_const struct awbl_imx6ul_gpmi_devinfo __g_imx6ul_gpmi_devinfo = {
    IMX6UL_NAND0_BUSID,             /* NAND BUS ID号 */
    IMX6UL_GPMI_BASE_ADDR,          /* GPMI 基地址 */
    IMX6UL_BCH_BASE_ADDR,           /* BCH 基地址   */
    IMX6UL_CLK_GPMI_IO,             /* IO时钟ID        */
    IMX6UL_DMA_CHANNEL_GPMI0,       /* DMA通道号    */

    __g_imx6ul_gpmi_plat_init
};

aw_local struct awbl_imx6ul_gpmi __g_imx6ul_gpmi_dev = {
    .p_cur_chip = (struct awbl_imx6ul_nand_dev *)0x1234aaaa,
};

#define AWBL_HWCONF_IMX6UL_GPMI                                 \
    {                                                           \
        AWBL_IMX6UL_GPMI_NAME,                                  \
        0,                                                      \
        AWBL_BUSID_PLB,                                         \
        0,                                                      \
        &(__g_imx6ul_gpmi_dev.nandbus.awbus.super),    \
        &(__g_imx6ul_gpmi_devinfo)                              \
    },

#else

#define AWBL_HWCONF_IMX6UL_GPMI

#endif  /* AW_DEV_IMX6UL_GPMI */

#endif /* __AWBL_HWCONF_IMX6UL_GPMI_H */

/* end of file */
