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

#ifndef __AWBL_HWCONF_IMX6UL_TS_H
#define __AWBL_HWCONF_IMX6UL_TS_H

#ifdef AW_DEV_IMX6UL_TS

#include "driver/input/touchscreen/awbl_imx6ul_ts.h"


aw_local void __imx6ul_ts_plfm_init (void)
{
    aw_clk_enable(IMX6UL_CLK_TSC_DIG);

    int tcs_gpios[] = {

                        GPIO1_1,
                        GPIO1_2,
                        GPIO1_3,
                        GPIO1_4,
    };

    if (aw_gpio_pin_request("tcs_gpios",
                            tcs_gpios,
                            AW_NELEMENTS(tcs_gpios)) == AW_OK) {

        /* 触摸屏只用两个ADC通道作为ADC采样就可以了  */
        aw_gpio_pin_cfg(GPIO1_1, IMX6UL_PAD_CTL(0)); /* PKE_0_Pull_Keeper_Disabled */
        aw_gpio_pin_cfg(GPIO1_3, IMX6UL_PAD_CTL(0)); /* PKE_0_Pull_Keeper_Disabled */

    }
}


aw_local awbl_imx6ul_ts_devinfo_t __g_imx6ul_ts_devinfo = {
        IMX6UL_TS_BASE_ADDR,     /* 寄存器基地址 */
        4,                          /* x_plus_ch  : ADC(4) */
        3,                          /* x_minus_ch : ADC(3) */
        2,                          /* y_plus_ch  : ADC(2) */
        1,                          /* y_minus_ch : ADC(1) */
        {
            "480x272",                    /**< \brief 触摸关联的显示屏名字       */
            0x00,                         /**< \brief 触摸关联的显示屏产商ID */
            0x00,                         /**< \brief 触摸关联的显示屏产品ID */
            5,                            /**< \brief 支持最多触摸点的个数.    */
            35,                           /**< \brief 采样个数, 最小值为1  */
            8,                            /**< \brief 滤波个数, 不能大于采样个数, 最小值为1 */
            60,                           /**< \brief 消抖时长      */
            120,                          /**< \brief 采样门限阀值,用于消抖,根据关联不同的显示屏可做适配调整. */
            5,                            /**< \brief 校准门限阀值,用于验证校准系数,根据关联不同的显示屏可做适配调整. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief 五点校准算法   */
        },
       __imx6ul_ts_plfm_init     /* 平台初始化 */
};

/* 设备实例内存静态分配 */
awbl_imx6ul_ts_dev_t __g_imx6ul_ts;

#define AWBL_HWCONF_IMX6UL_TS           \
    {                                   \
        AWBL_IMX6UL_TOUCHSCREEN_NAME,   \
        0,                              \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &(__g_imx6ul_ts.adev),          \
        &(__g_imx6ul_ts_devinfo)        \
    },

#else
#define AWBL_HWCONF_IMX6UL_TS

#endif  /* AW_DEV_IMX6UL_TS */

#endif  /* __AWBL_HWCONF_IMX6UL_TS_H */

/* end of file */
