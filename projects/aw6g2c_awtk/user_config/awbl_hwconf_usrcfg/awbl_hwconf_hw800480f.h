/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_HW800480F_H
#define __AWBL_HWCONF_HW800480F_H

#ifdef AW_DEV_HW800480F
/*******************************************************************************
  HW800480 配置信息
*******************************************************************************/
#include "driver/display/awbl_mxs_tft_panel.h"


#define __AP_TFT7_0_V1_00  0
#define __AP_TFT7_0_V1_01  1

#define __TFT_VERSION   __AP_TFT7_0_V1_00

#define __HW800480_X_RES        800
#define __HW800480_Y_RES        480
#define __HW800480_BUS_BPP      16
#define __HW800480_WORD_LBPP    16
#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define __HW800480_DCLK_F       30000000
#endif

#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define __HW800480_DCLK_F       24000000
#endif

#define DOTCLK_H_ACTIVE         __HW800480_X_RES
#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define DOTCLK_H_PULSE_WIDTH    48
#define DOTCLK_HF_PORCH         200
#define DOTCLK_HB_PORCH         40
#endif
#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define DOTCLK_H_PULSE_WIDTH    1
#define DOTCLK_HF_PORCH         210
#define DOTCLK_HB_PORCH         46
#endif

#define DOTCLK_V_ACTIVE         __HW800480_Y_RES
#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define DOTCLK_V_PULSE_WIDTH    3
#define DOTCLK_VF_PORCH         13
#define DOTCLK_VB_PORCH         29
#endif
#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define DOTCLK_V_PULSE_WIDTH    1
#define DOTCLK_VF_PORCH         22
#define DOTCLK_VB_PORCH         23
#endif

typedef struct __hw800480f_gpio_cfg {
    int         gpio;
    uint32_t    func;

} __hw800480f_gpio_cfg_t;


aw_local aw_const  __hw800480f_gpio_cfg_t __g_hw800480f_gpios_list[] = {

    {GPIO3_0,  GPIO3_0_LCDIF_CLK},         /* LCD clk */
    {GPIO3_1,  GPIO3_1_LCDIF_ENABLE},      /* LCD enable */
    {GPIO3_2,  GPIO3_2_LCDIF_HSYNC},       /* LCD hsync */
    {GPIO3_3,  GPIO3_3_LCDIF_VSYNC},       /* LCD vsync */
    //aw_gpio_pin_cfg(GPIO3_4, GPIO3_4_LCDIF_RESET);       /* lcd reset change by CYX at 2019.1.7*/
#if (__HW800480_BUS_BPP == 16)
    //B
    {GPIO3_5,  GPIO3_5_LCDIF_DATA00},      /* LCD data0 */
    {GPIO3_6,  GPIO3_6_LCDIF_DATA01},      /* LCD data1 */
    {GPIO3_7,  GPIO3_7_LCDIF_DATA02},      /* LCD data2 */
    {GPIO3_8,  GPIO3_8_LCDIF_DATA03},      /* LCD data3 */
    {GPIO3_9,  GPIO3_9_LCDIF_DATA04},      /* LCD data4 */
    //G
    {GPIO3_10,  GPIO3_10_LCDIF_DATA05},      /* LCD data5 */
    {GPIO3_11, GPIO3_11_LCDIF_DATA06},     /* LCD data6 */
    {GPIO3_12, GPIO3_12_LCDIF_DATA07},     /* LCD data7 */
    {GPIO3_13, GPIO3_13_LCDIF_DATA08},     /* LCD data8 */
    {GPIO3_14, GPIO3_14_LCDIF_DATA09},     /* LCD data9 */
    {GPIO3_15, GPIO3_15_LCDIF_DATA10},     /* LCD data10 */
    //R
    {GPIO3_16, GPIO3_16_LCDIF_DATA11},     /* LCD data11 */
    {GPIO3_17, GPIO3_17_LCDIF_DATA12},     /* LCD data12 */
    {GPIO3_18, GPIO3_18_LCDIF_DATA13},     /* LCD data13 */
    {GPIO3_19, GPIO3_19_LCDIF_DATA14},     /* LCD data14 */
    {GPIO3_20, GPIO3_20_LCDIF_DATA15},     /* LCD data15 */
#endif

#if (__HW800480_BUS_BPP == 24)
    //B
    {GPIO3_5,  GPIO3_5_LCDIF_DATA00},      /* LCD data0 */
    {GPIO3_6,  GPIO3_6_LCDIF_DATA01},      /* LCD data1 */
    {GPIO3_7,  GPIO3_7_LCDIF_DATA02},      /* LCD data2 */
    {GPIO3_8,  GPIO3_8_LCDIF_DATA03},      /* LCD data3 */
    {GPIO3_9,  GPIO3_9_LCDIF_DATA04},      /* LCD data4 */
    {GPIO3_10,  GPIO3_10_LCDIF_DATA05},      /* LCD data5 */
    {GPIO3_11, GPIO3_11_LCDIF_DATA06},     /* LCD data6 */
    {GPIO3_12, GPIO3_12_LCDIF_DATA07},     /* LCD data7 */
    //G
    {GPIO3_13, GPIO3_13_LCDIF_DATA08},     /* LCD data8 */
    {GPIO3_14, GPIO3_14_LCDIF_DATA09},     /* LCD data9 */
    {GPIO3_15, GPIO3_15_LCDIF_DATA10},     /* LCD data10 */
    {GPIO3_16, GPIO3_16_LCDIF_DATA11},     /* LCD data11 */
    {GPIO3_17, GPIO3_17_LCDIF_DATA12},     /* LCD data12 */
    {GPIO3_18, GPIO3_18_LCDIF_DATA13},     /* LCD data13 */
    {GPIO3_19, GPIO3_19_LCDIF_DATA14},     /* LCD data14 */
    {GPIO3_20, GPIO3_20_LCDIF_DATA15},     /* LCD data15 */
    //R
    {GPIO3_21, GPIO3_21_LCDIF_DATA16},/* LCD data16 */
    {GPIO3_22, GPIO3_22_LCDIF_DATA17},/* LCD data17 */
    {GPIO3_23, GPIO3_23_LCDIF_DATA18},/* LCD data18 */
    {GPIO3_24, GPIO3_24_LCDIF_DATA19},/* LCD data19 */
    {GPIO3_25, GPIO3_25_LCDIF_DATA20},/* LCD data20 */
    {GPIO3_26, GPIO3_26_LCDIF_DATA21},/* LCD data21 */
    {GPIO3_27, GPIO3_27_LCDIF_DATA22},/* LCD data22 */
    {GPIO3_28, GPIO3_28_LCDIF_DATA23},/* LCD data23 */
#endif
};


aw_local void __hw800480_plfm_init (void)
{
    int i = 0;
    int lcd_gpios[AW_NELEMENTS(__g_hw800480f_gpios_list)];

    for (i = 0; i < AW_NELEMENTS(__g_hw800480f_gpios_list); i++) {
        lcd_gpios[i] = __g_hw800480f_gpios_list[i].gpio;
    }

    if (aw_gpio_pin_request("lcd_gpios",
                             lcd_gpios,
                             AW_NELEMENTS(lcd_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(__g_hw800480f_gpios_list); i++) {
            /* 配置功能引脚 */
            aw_gpio_pin_cfg(__g_hw800480f_gpios_list[i].gpio,
                            __g_hw800480f_gpios_list[i].func);
        }
    }
}

/* 设备信息 */
aw_local aw_const awbl_mxs_tft_panel_devinfo_t  __g_hw800480f_param = {
    /* panel_info */
    {
        __HW800480_X_RES,               /* x_res */
        __HW800480_Y_RES,               /* y_res */
        __HW800480_BUS_BPP,             /* rgb硬件总线数 */
        __HW800480_WORD_LBPP,           /* 一个像素的长度，位为单位,如rgb565数据，该值为16 */
        __HW800480_DCLK_F,              /* dclk_f */
        AWBL_MXS_LCDIF_PANEL_DOTCLK,    /* type */
        99,                             /* default back light intensity */
    },
    /* timing */
    {
        DOTCLK_H_PULSE_WIDTH,           /* h_pulse_width */
        DOTCLK_H_ACTIVE,                /* h_active */
        DOTCLK_HB_PORCH,                /* h_hb_porch */
        DOTCLK_HF_PORCH,                /* h_hf_porch */
        DOTCLK_V_PULSE_WIDTH,           /* v_pulse_width */
        DOTCLK_V_ACTIVE,                /* v_active */
        DOTCLK_VB_PORCH,                /* h_vb_porch */
        DOTCLK_VF_PORCH,                /* h_vf_porch */
        0,                              /* enable_present */
    },
    /* plfm_init */
    __hw800480_plfm_init
};

/* 设备实例内存静态分配 */
aw_local awbl_mxs_tft_panel_t __g_hw800480f;

#define AWBL_HWCONF_HW800480F       \
    {                               \
		AWBL_MXS_TFT_PANEL_NAME,    \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_hw800480f.dev),       \
        &(__g_hw800480f_param)      \
    },

#else
#define AWBL_HWCONF_HW800480F

#endif  /* AWBL_HWCONF_HW800480F */

#endif  /* __AWBL_HWCONF_HW800480F_H */

/* end of file */
