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

#ifndef __AWBL_HWCONF_IMX6UL_EIM_H
#define __AWBL_HWCONF_IMX6UL_EIM_H

#ifdef AW_DEV_IMX6UL_EIM

#include "awbl_eim_bus.h"
#include "driver/eim/awbl_imx6ul_eim.h"
#include "imx6ul_regs_base.h"

aw_local void __g_imx6ul_eim_plat_init (void)
{
    int eim_gpios[] = {
            /* 注意CS引脚有无复用 */
            GPIO4_17,   //CS0_B
            GPIO4_12,   //CS1_B
            GPIO3_0,    //CS2_B
            GPIO3_1,    //CS3_B

            GPIO4_18,   //OE
            GPIO4_19,   //RW
            GPIO4_20,   //LBA_B

            GPIO4_21,   //AD0
            GPIO4_22,   //AD1
            GPIO4_23,   //AD2
            GPIO4_24,   //AD3
            GPIO4_25,   //AD4
            GPIO4_26,   //AD5
            GPIO4_27,   //AD6
            GPIO4_28,   //AD7
    };

    if (aw_gpio_pin_request("eim_gpios",
                             eim_gpios,
                             AW_NELEMENTS(eim_gpios)) == AW_OK) {

        /*  IO配置 */
        aw_gpio_pin_cfg(GPIO4_17, GPIO4_17_EIM_CS0_B);
        aw_gpio_pin_cfg(GPIO4_12, GPIO4_12_EIM_CS1_B);
        aw_gpio_pin_cfg(GPIO3_0,  GPIO3_0_EIM_CS2_B);
        aw_gpio_pin_cfg(GPIO3_1,  GPIO3_1_EIM_CS3_B);

        aw_gpio_pin_cfg(GPIO4_18, GPIO4_18_EIM_OE);
        aw_gpio_pin_cfg(GPIO4_19, GPIO4_19_EIM_RW);
        aw_gpio_pin_cfg(GPIO4_20, GPIO4_20_EIM_LBA_B);

        aw_gpio_pin_cfg(GPIO4_21, GPIO4_21_EIM_AD00);
        aw_gpio_pin_cfg(GPIO4_22, GPIO4_22_EIM_AD01);
        aw_gpio_pin_cfg(GPIO4_23, GPIO4_23_EIM_AD02);
        aw_gpio_pin_cfg(GPIO4_24, GPIO4_24_EIM_AD03);
        aw_gpio_pin_cfg(GPIO4_25, GPIO4_25_EIM_AD04);
        aw_gpio_pin_cfg(GPIO4_26, GPIO4_26_EIM_AD05);
        aw_gpio_pin_cfg(GPIO4_27, GPIO4_27_EIM_AD06);
        aw_gpio_pin_cfg(GPIO4_28, GPIO4_28_EIM_AD07);
    }


    /* enable the clock */
    aw_clk_parent_set(IMX6UL_CLK_EIM_SLOW_SEL, IMX6UL_CLK_AXI);
    aw_clk_rate_set(IMX6UL_CLK_EIM_SLOW_PODF, 33000000);
    aw_clk_enable(IMX6UL_CLK_EIM);
}

aw_local aw_const struct awbl_imx6ul_eim_devinfo __g_imx6ul_eim_devinfo = {
    {
            IMX6UL_EIM0_BUSID,           /* EIM BUS ID号 */
    },
    IMX6UL_EIM_BASE_ADDR,                /* EIM 寄存器基地址 */
    (uint32_t)0x50000000,                /* EIM CS空间基地址 */
    (uint32_t)0x2000000,                 /* EIM 单个CS空间大小 */
    {
            {SJA1000_CONFIG},
            {SJA1000_CONFIG},
            {SJA1000_CONFIG},
            {SJA1000_CONFIG},
    },
    __g_imx6ul_eim_plat_init
};

aw_local struct awbl_imx6ul_eim_dev __g_imx6ul_eim_dev;

#define AWBL_HWCONF_IMX6UL_EIM                                 \
    {                                                           \
        AWBL_IMX6UL_EIM_NAME,                                  \
        0,                                                      \
        AWBL_BUSID_PLB,                                         \
        0,                                                      \
        (struct awbl_dev *)&__g_imx6ul_eim_dev,                  \
        &(__g_imx6ul_eim_devinfo)                              \
    },

#else

#define AWBL_HWCONF_IMX6UL_EIM

#endif  /* AW_DEV_IMX6UL_EIM */

#endif /* __AWBL_HWCONF_IMX6UL_EIM_H */

/* end of file */
