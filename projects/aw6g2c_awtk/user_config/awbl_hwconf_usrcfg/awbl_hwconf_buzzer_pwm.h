/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief PWM驱动蜂鸣器配置信息.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-08  hgo, first implementation.
 * \endinternal
 */

#ifndef AWBL_HWCONF_BUZZER_PWM_H_
#define AWBL_HWCONF_BUZZER_PWM_H_

#include "driver/buzzer/awbl_buzzer_pwm.h"
#include "imx6ul_pwmid.h"

#ifdef AW_DEV_PWM_BUZZER

aw_local aw_const awbl_pwm_buzzer_param_t __g_pwm_buzzer_param = {
    PWM6,        /* PWM通道ID */
    366166,      /* PWM波形输出周期(单位: 纳秒) */
    50           /* 初始响度(占空比) 50% ，范围0-100 */
};

aw_local awbl_pwm_buzzer_t __g_pwm_buzzer;

#define AWBL_HWCONF_BUZZER_PWM  \
    {                           \
        PWM_BUZZER_NAME,        \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &(__g_pwm_buzzer.dev),  \
        &(__g_pwm_buzzer_param) \
    },                          \

#else

#define AWBL_HWCONF_BUZZER_PWM

#endif

#endif /* AWBL_HWCONF_BUZZER_PWM_H_ */
