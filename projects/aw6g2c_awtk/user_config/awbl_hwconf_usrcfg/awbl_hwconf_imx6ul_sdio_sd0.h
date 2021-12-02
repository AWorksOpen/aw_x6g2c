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

#ifndef AWBL_HWCONF_IMX6UL_SDIO_SSP0_H_
#define AWBL_HWCONF_IMX6UL_SDIO_SSP0_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/sdio/awbl_imx6ul_sdio.h"
#include "awbl_sdiobus.h"
#include "driver/sdiocard/awbl_sdcard.h"
#include "aw_delay.h"
#include "imx6ul_gpio_pin.h"

#ifdef AW_DEV_IMX6UL_USDHC1

/* 设备实例内存静态分配 */
 awbl_imx6ul_sdio_dev_t __g_imx6ul_sdio_ssp0_dev;


aw_local void __imx6ul_sdio_ssp0_plfm_init (void);



/* 检测卡插拔状态 */
aw_local aw_bool_t __imx6ul_sdio_ssp0_detect (awbl_imx6ul_sdio_dev_t *p_dev)
{
    return (aw_gpio_get(GPIO1_19) ? AW_FALSE : AW_TRUE);
}


/* 设备信息 */
aw_local aw_const awbl_imx6ul_sdio_dev_info_t __g_imx6ul_sdio_ssp0_devinfo = {
    {
        0,
        AWBL_SDIO_SD_4B_M,
        AWBL_SDIO_CON_DEV_DYNAMIC,
        AW_TRUE,
        AW_FALSE,
        (AWBL_SD_OCR_VDD_27_28 | AWBL_SD_OCR_VDD_28_29 | AWBL_SD_OCR_VDD_29_30 |
         AWBL_SD_OCR_VDD_30_31 | AWBL_SD_OCR_VDD_31_32 | AWBL_SD_OCR_VDD_32_33 |
         AWBL_SD_OCR_VDD_33_34 | AWBL_SD_OCR_VDD_34_35 | AWBL_SD_OCR_VDD_35_36),
    },

    IMX6UL_USDHC1_BASE_ADDR,            /* 寄存器基地址 */
    IMX6UL_CLK_USDHC1,                    /* 输入时钟ID  */
    1,                                  /* 传输任务优先级 */
    __imx6ul_sdio_ssp0_plfm_init,       /* 平台相关初始化 */
    INUM_USDHC1_IRQ,                    /* 中断号 */
    __imx6ul_sdio_ssp0_detect
};



/* 平台相关初始化 */
aw_local void __imx6ul_sdio_ssp0_plfm_init (void)
{

    int usdhc1_gpios[] = {
        GPIO2_16,    /* USDHC1_CMD   */
        GPIO2_17,    /* USDHC1_CLK   */
        GPIO2_18,    /* USDHC1_DATA0 */
        GPIO2_19,    /* USDHC1_DATA1 */
        GPIO2_20,    /* USDHC1_DATA2 */
        GPIO2_21,    /* USDHC1_DATA3 */
        GPIO1_19,
        GPIO1_18
    };

    /* 配置功能引脚 */
    if (aw_gpio_pin_request("usdhc1_gpios",
                            usdhc1_gpios,
                            AW_NELEMENTS(usdhc1_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO2_16, GPIO2_16_USDHC1_CMD);   /**< \brief SD1_CMD  */
        aw_gpio_pin_cfg(GPIO2_17, GPIO2_17_USDHC1_CLK);   /**< \brief SD1_CLK  */
        aw_gpio_pin_cfg(GPIO2_18, GPIO2_18_USDHC1_DATA0); /**< \brief SD1_DAT0 */
        aw_gpio_pin_cfg(GPIO2_19, GPIO2_19_USDHC1_DATA1); /**< \brief SD1_DAT1 */
        aw_gpio_pin_cfg(GPIO2_20, GPIO2_20_USDHC1_DATA2); /**< \brief SD1_DAT2 */
        aw_gpio_pin_cfg(GPIO2_21, GPIO2_21_USDHC1_DATA3); /**< \brief SD1_DAT3 */
        aw_gpio_pin_cfg(GPIO1_19, GPIO1_19_GPIO | AW_GPIO_INPUT|AW_GPIO_PULL_UP);
        aw_gpio_pin_cfg(GPIO1_18, GPIO1_18_GPIO | AW_GPIO_INPUT|AW_GPIO_FLOAT);
    }

    /* 使能 SSP0 时钟 */
    aw_clk_enable(IMX6UL_CLK_USDHC1);
};



#define AWBL_HWCONF_IMX6UL_SDIO_SSP0                    \
    {                                                   \
        AWBL_IMX6UL_SDIO_NAME,                          \
        0,                                              \
        AWBL_BUSID_PLB,                                 \
        0,                                              \
        &(__g_imx6ul_sdio_ssp0_dev.host.super.super),   \
        &__g_imx6ul_sdio_ssp0_devinfo                   \
    },

#else

#define AWBL_HWCONF_IMX6UL_SDIO_SSP0

#endif /* AWBL_HWCONF_IMX6UL_SDIO_SSP0 */

#endif /* AWBL_HWCONF_IMX6UL_SDIO_SSP1_H_ */

/* end of file */
