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
    GPTIMER 2 ������Ϣ
*******************************************************************************/

#ifdef AW_DEV_IMX6UL_GPTIMER_2

#include "aw_int.h"
#include "driver/timer/awbl_imx6ul_gptimer.h"

/**
 * \file
 * \brief AWBus-lite iMX6UL GPTIMER �����ļ�
 *
 * iMX6UL GPTIMER ΪiMX6ULϵ��MCU��ͨ�ö�ʱ�����ṩ���¹���:
 *  - �������жϣ�����ʱ����
 *  - ���벶��
 *  - ����Ƚ�
 *
 * ��Ӧ�Ĺ������ü� awbl_imx6ul_gptimer_param_t ���ͣ�����������ʾ�����ã�
 * ͨ���� #__GPT_FOR_TIMER��__GPT_FOR_INPUT��__GPT_FOR_OUTPUT�����й���ѡ��
 *
 * - ���ó�ͨ�ö�ʱ����ʱ��ֻ��Ҫ�� mode ����Ϊ IMX6UL_GPT_MODE_TIMER��
 *   feature ��֮��Ĳ�������Ҫ���ã���0���ɣ�
 *
 * - ���ó����벶���ʱ����Ҫ�� mode ����Ϊ IMX6UL_GPT_MODE_INPUT��
 *   Ȼ����Ҫ�����������ԣ��������ش������½��ش�������˫���ش�����ͨ��ѡ������ͨ��1��2����
 *   �Լ���Ӧͨ��������ѡ�񣻣����Ժ�� awbl_imx6ul_gptimer.h��
 *
 * - ���ó�����Ƚϵ�ʱ����Ҫ�� mode ����Ϊ IMX6UL_GPT_MODE_OUTPUT��
 *   Ȼ����Ҫ����������ԣ��������ת�������λ����������㣻ͨ��ѡ������ͨ��1��2��3����
 *   ��Ӧͨ��������ѡ���Լ�����Ƚ�ֵ�������Ժ�� awbl_imx6ul_gptimer.h��
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
 * \brief ���� GPT ʹ�õ�ͨ��
 *
 * ��ʱ��ģʽʱ��ͨ��������Ч��
 * ���벶��ģʽʱ��ͨ��������Ϊ 1 ���� 2��
 * ����Ƚ�ģʽʱ��ͨ��������Ϊ 1��2 ���� 3��
 */
#define __GPT2_CHAN  1

/** \brief GPT1 ������Ϣ */
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

        /* ���ù������� */
        aw_gpio_pin_cfg(__gpt2_gpios[__GPT2_CHAN - 1][0],
                        __gpt2_gpios[__GPT2_CHAN - 1][1]);
    }
#elif defined __GPT2_FOR_OUTPUT
    int gpt2_gpios[] = {__gpt2_gpios[__GPT2_CHAN + 1][0]};

    if (aw_gpio_pin_request("gpt2_gpios",
                             gpt2_gpios,
                             AW_NELEMENTS(gpt2_gpios)) == AW_OK) {

        /* ���ù������� */
        aw_gpio_pin_cfg(__gpt2_gpios[__GPT2_CHAN + 1][0],
                        __gpt2_gpios[__GPT2_CHAN + 1][1]);
    }
#endif
}

static const aw_clk_id_t gpt2_clks[2] = {IMX6UL_CLK_GPT2_BUS,IMX6UL_CLK_GPT2_SERIAL};

#if defined  __GPT2_FOR_TIMER

/* GPTIMER_2 �豸��Ϣ */
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

/* GPTIMER_2 �豸��Ϣ */
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

/* GPTIMER_2 �豸��Ϣ */
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

/* GPTIMER 2 �豸ʵ���ڴ澲̬���� */
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
