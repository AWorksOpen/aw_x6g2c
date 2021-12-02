/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX6UL_GPTIMER2_H
#define  __AWBL_HWCONF_IMX6UL_GPTIMER2_H

/*******************************************************************************
    GPTIMER 2 配置信息
*******************************************************************************/

#ifdef AW_DEV_IMX6UL_GPTIMER_2

#include "aw_int.h"
#include "driver/timer/awbl_imx6ul_gptimer.h"

/**
 * \file
 * \brief AWBus-lite iMX6UL GPTIMER 配置文件
 *
 * iMX6UL GPTIMER 为iMX6UL系列MCU的通用定时器，提供如下功能:
 *  - 周期性中断（即定时器）
 *  - 输入捕获
 *  - 输出比较
 *
 * 对应的功能配置见 awbl_imx6ul_gptimer_param_t 类型，给出了如下示例配置，
 * 通过宏 #__GPT_FOR_TIMER、__GPT_FOR_INPUT及__GPT_FOR_OUTPUT来进行功能选择。
 *
 * - 配置成通用定时器的时候，只需要将 mode 设置为 IMX6UL_GPT_MODE_TIMER，
 *   feature 及之后的参数不需要配置，填0即可；
 *
 * - 配置成输入捕获的时候，需要将 mode 设置为 IMX6UL_GPT_MODE_INPUT，
 *   然后需要填入输入特性，如上升沿触发、下降沿触发或是双边沿触发；通道选择（输入通道1或2）；
 *   以及对应通道的引脚选择；（特性宏见 awbl_imx6ul_gptimer.h）
 *
 * - 配置成输出比较的时候，需要将 mode 设置为 IMX6UL_GPT_MODE_OUTPUT，
 *   然后需要填入输出特性，如输出翻转、输出置位或是输出清零；通道选择（输入通道1、2或3）；
 *   对应通道的引脚选择；以及输出比较值。（特性宏见 awbl_imx6ul_gptimer.h）
 *
 * \internal
 * \par modification history
 * - 1.00 2016-05-20, sky, first implementation.
 * \endinternal
 */


#define __GPT2_FOR_TIMER
//#define __GPT2_FOR_INPUT
//#define __GPT2_FOR_OUTPUT

/**
 * \brief 配置 GPT 使用的通道
 *
 * 定时器模式时，通道配置无效；
 * 输入捕获模式时，通道可配置为 1 或者 2；
 * 输出比较模式时，通道可配置为 1、2 或者 3。
 */
#define __GPT2_CHAN  1

/** \brief GPT1 引脚信息 */
aw_local aw_const uint32_t __gpt2_gpios[5][2] = {
    {GPIO1_11, GPIO1_11_GPT2_CAPTURE1},
    {GPIO1_12, GPIO1_12_GPT2_CAPTURE2},
    {GPIO1_13, GPIO1_13_GPT2_COMPARE1},
    {GPIO1_14, GPIO1_14_GPT2_COMPARE2},
    {GPIO1_15, GPIO1_15_GPT2_COMPARE3}
};

aw_local void __imx6ul_gptimer2_plfm_init (void)
{
#if defined __GPT2_FOR_INPUT
    int gpt2_gpios[] = {__gpt2_gpios[__GPT2_CHAN - 1][0]};

    if (aw_gpio_pin_request("gpt2_gpios",
                             gpt2_gpios,
                             AW_NELEMENTS(gpt2_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(__gpt2_gpios[__GPT2_CHAN - 1][0],
                        __gpt2_gpios[__GPT2_CHAN - 1][1]);
    }
#elif defined __GPT2_FOR_OUTPUT
    int gpt2_gpios[] = {__gpt2_gpios[__GPT2_CHAN + 1][0]};

    if (aw_gpio_pin_request("gpt2_gpios",
                             gpt2_gpios,
                             AW_NELEMENTS(gpt2_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(__gpt2_gpios[__GPT2_CHAN + 1][0],
                        __gpt2_gpios[__GPT2_CHAN + 1][1]);
    }
#endif
}

static const aw_clk_id_t gpt2_clks[2] = {IMX6UL_CLK_GPT2_BUS,IMX6UL_CLK_GPT2_SERIAL};

#if defined  __GPT2_FOR_TIMER

/* GPTIMER_2 设备信息 */
aw_local aw_const awbl_imx6ul_gptimer_param_t __g_gptimer2_param = {
    1,                                  /* alloc_byname */
    IMX6UL_GPT2_BASE_ADDR,              /* reg_addr */
    INUM_GPT2_IRQ,                      /* inum */
    gpt2_clks,
    2,
    IMX6UL_GPT_MODE_TIMER,              /* mode select */
    0,                                  /* feature config */
    0,                                  /* channel select(input 1,2 output 1,2,3) */
    0,                                  /* output compare value */
    __imx6ul_gptimer2_plfm_init         /* pfunc_plfm_init */
};
/* __GPT2_FOR_TIMER */

#elif defined  __GPT2_FOR_INPUT

/* GPTIMER_2 设备信息 */
aw_local aw_const awbl_imx6ul_gptimer_param_t __g_gptimer2_param = {
    1,                                  /* alloc_byname */
    IMX6UL_GPT2_BASE_ADDR,              /* reg_addr */
    INUM_GPT2_IRQ,                      /* inum */
    gpt2_clks,
    2,
    IMX6UL_GPT_MODE_INPUT,              /* mode select */
    IMX6UL_GPT_INPUT_FALL,              /* feature config */
    __GPT2_CHAN,                        /* channel select(input 1,2 output 1,2,3) */
    0,                                  /* output compare value */
    __imx6ul_gptimer2_plfm_init         /* pfunc_plfm_init */
};
/* __GPT2_FOR_INPUT */

#elif defined  __GPT2_FOR_OUTPUT

/* GPTIMER_2 设备信息 */
aw_local aw_const awbl_imx6ul_gptimer_param_t __g_gptimer1_param = {
    1,                                  /* alloc_byname */
    IMX6UL_GPT2_BASE_ADDR,              /* reg_addr */
    INUM_GPT2_IRQ,                      /* inum */
    gpt2_clks,
    2,
    IMX6UL_GPT_MODE_OUTPUT,             /* mode select */
    IMX6UL_GPT_OUTPUT_TOG,              /* feature config */
    __GPT2_CHAN,                        /* channel select(input 1,2 output 1,2,3) */
    0x00FFFFFF,                         /* output compare value */
    __imx6ul_gptimer2_plfm_init         /* pfunc_plfm_init */
};
#endif /* __GPT2_FOR_OUTPUT */

/* GPTIMER 2 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_gptimer __g_gptimer2_dev;

#define AWBL_HWCONF_IMX6UL_GPTIMER2 \
    {                               \
        AWBL_IMX6UL_GPT_DRV_NAME,   \
        IMX6UL_GPT2_BUSID,          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_gptimer2_dev.dev,      \
        &__g_gptimer2_param         \
    },

#else
#define AWBL_HWCONF_IMX6UL_GPTIMER2

#endif  /* AW_DEV_IMX6UL_GPTIMER_2 */

#endif  /* __AWBL_HWCONF_IMX6UL_GPTIMER2_H */

/* end of file */
