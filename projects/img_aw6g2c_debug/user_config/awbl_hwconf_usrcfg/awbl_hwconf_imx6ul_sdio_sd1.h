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

#ifndef AWBL_HWCONF_IMX6UL_SDIO_SSP1_H_
#define AWBL_HWCONF_IMX6UL_SDIO_SSP1_H_


#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/sdio/awbl_imx6ul_sdio.h"
#include "awbl_sdiobus.h"
#include "driver/sdiocard/awbl_sdcard.h"
#include "aw_delay.h"
#include "imx6ul_gpio_pin.h"

#ifdef AW_DEV_IMX6UL_USDHC2

#define USDHC2_PAD_CTRL    (SRE_0_Fast_Slew_Rate | \
                            DSE_6_R0_6 |           \
                            SPEED_3_max_200MHz|    \
                            PKE_1_Pull_Keeper_Enabled |\
                            PUE_1_Pull |           \
                            PUS_1_47K_Ohm_Pull_Up| \
                            HYS_1_Hysteresis_Enabled)



/* 设备实例内存静态分配 */
awbl_imx6ul_sdio_dev_t __g_imx6ul_sdio_ssp1_dev;

aw_local void __imx6ul_sdio_ssp1_plfm_init (void);


/* 检测卡插拔状态 */
aw_local aw_bool_t __imx6ul_sdio_ssp1_detect (awbl_imx6ul_sdio_dev_t *p_dev)
{
    return AW_TRUE;
}

/* 设备信息 */
aw_local aw_const awbl_imx6ul_sdio_dev_info_t __g_imx6ul_sdio_ssp1_devinfo = {
    {
        IMX6UL_SDIO1_BUSID,
        AWBL_SDIO_SD_4B_M,
        AWBL_SDIO_CON_DEV_STATIC,
        //AWBL_SDIO_CON_DEV_DYNAMIC,
        AW_TRUE,
        AW_FALSE,
        (AWBL_SD_OCR_VDD_27_28 | AWBL_SD_OCR_VDD_28_29 | AWBL_SD_OCR_VDD_29_30 |
         AWBL_SD_OCR_VDD_30_31 | AWBL_SD_OCR_VDD_31_32 | AWBL_SD_OCR_VDD_32_33 |
         AWBL_SD_OCR_VDD_33_34 | AWBL_SD_OCR_VDD_34_35 | AWBL_SD_OCR_VDD_35_36),
    },

    IMX6UL_USDHC2_BASE_ADDR,            /* 寄存器基地址 */
    IMX6UL_CLK_USDHC2,                    /* 输入时钟ID  */
    3,                                  /* 传输任务优先级 */
    __imx6ul_sdio_ssp1_plfm_init,       /* 平台相关初始化 */
    INUM_USDHC2_IRQ,                    /* 中断号 */
    __imx6ul_sdio_ssp1_detect,
};


/* 平台相关初始化 */
aw_local void __imx6ul_sdio_ssp1_plfm_init (void)
{

     int usdhc2_gpios[] = {
         GPIO4_20,    /* USDHC2_CMD   */
         GPIO4_19,    /* USDHC2_CLK   */
         GPIO4_21,    /* USDHC2_DATA0 */
         GPIO4_22,    /* USDHC2_DATA1 */
         GPIO4_23,    /* USDHC2_DATA2 */
         GPIO4_24,    /* USDHC2_DATA3 */
     };

     /* 配置功能引脚 */
     if (aw_gpio_pin_request("usdhc2_gpios",
                              usdhc2_gpios,
                              AW_NELEMENTS(usdhc2_gpios)) == AW_OK) {
         aw_gpio_pin_cfg(GPIO4_20, GPIO4_20_USDHC2_CMD   | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_CMD  */
         aw_gpio_pin_cfg(GPIO4_19, GPIO4_19_USDHC2_CLK   | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_CLK  */
         aw_gpio_pin_cfg(GPIO4_21, GPIO4_21_USDHC2_DATA0 | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_DAT0 */
         aw_gpio_pin_cfg(GPIO4_22, GPIO4_22_USDHC2_DATA1 | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_DAT1 */
         aw_gpio_pin_cfg(GPIO4_23, GPIO4_23_USDHC2_DATA2 | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_DAT2 */
         aw_gpio_pin_cfg(GPIO4_24, GPIO4_24_USDHC2_DATA3 | IMX6UL_PAD_CTL(USDHC2_PAD_CTRL)); /**< \brief SD2_DAT3 */
     }

    aw_clk_parent_set(IMX6UL_CLK_USDHC2_SEL,IMX6UL_CLK_PLL2_PFD2);

    /* 使能 USDHC2 时钟 */
    aw_clk_enable(IMX6UL_CLK_USDHC2);
};



#define AWBL_HWCONF_IMX6UL_SDIO_SSP1                    \
    {                                                   \
        AWBL_IMX6UL_SDIO_NAME,                          \
        0,                                              \
        AWBL_BUSID_PLB,                                 \
        0,                                              \
        &(__g_imx6ul_sdio_ssp1_dev.host.super.super),   \
        &__g_imx6ul_sdio_ssp1_devinfo                   \
    },

#else

#define AWBL_HWCONF_IMX6UL_SDIO_SSP1

#endif /* AWBL_HWCONF_IMX6UL_SDIO_SSP1 */

#endif /* AWBL_HWCONF_IMX6UL_SDIO_SSP1_H_ */

/* end of file */
