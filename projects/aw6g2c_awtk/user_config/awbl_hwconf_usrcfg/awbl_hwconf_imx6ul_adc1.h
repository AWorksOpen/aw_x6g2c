/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_ADC1_H
#define __AWBL_HWCONF_IMX6UL_ADC1_H

#ifdef AW_DEV_IMX6UL_ADC1

#include "driver/adc/awbl_imx6ul_adc.h"
#include "aw_adc.h"

aw_local void __imx6ul_adc1_plfm_init (void)
{
    int adc1_gpios[] = {
//                        GPIO1_0,
//                        GPIO1_1,
//                        GPIO1_2,
//                        GPIO1_3,
//                        GPIO1_4,
//                        GPIO1_5,
//                        GPIO1_6,
//                        GPIO1_7,
                        GPIO1_8,
                        GPIO1_9
    };
    int i = 0;

    if (aw_gpio_pin_request("adc1_gpios",
                            adc1_gpios,
                            AW_NELEMENTS(adc1_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(adc1_gpios); i++) {
            aw_gpio_pin_cfg(adc1_gpios[i], IMX6UL_PAD_CTL(0)); /* PKE_0_Pull_Keeper_Disabled */
        }
    }

};


/* 设备信息 */
static const aw_clk_id_t _adc1_ref_clk = IMX6UL_CLK_ADC1;
aw_local aw_const struct awbl_imx6ul_adc_devinfo __g_imx6ul_adc1_devinfo = {
    {
        IMX6UL_ADC1_MIN_CH,     /* 最小通道号 */
        IMX6UL_ADC1_MAX_CH      /* 最大通道号 */
    },
    IMX6UL_ADC1_BASE_ADDR,      /* 基址 */
    INUM_ADC1_IRQ,              /* 中断号 */
    12,                         /* 采样精度 8 10 12 bits 可选 */
    3300,                       /* 参考电压 */
    &_adc1_ref_clk,
    1,
    __imx6ul_adc1_plfm_init
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_adc_dev __g_imx6ul_adc1_dev;

#define AWBL_HWCONF_IMX6UL_ADC1                     \
    {                                               \
        IMX6UL_ADC_NAME,                            \
        1,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_adc1_dev,    \
        &(__g_imx6ul_adc1_devinfo)                  \
},

#else
#define AWBL_HWCONF_IMX6UL_ADC1

#endif  /* AW_DEV_IMX6UL_ADC1 */


#endif /* __AWBL_HWCONF_IMX6UL_ADC1_H */

/* end of file */
