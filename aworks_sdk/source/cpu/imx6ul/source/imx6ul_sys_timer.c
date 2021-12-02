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
 * \brief 系统定时器实现
 *
 * \internal
 * \par modification history:
 * - 1.00 14-05-04  orz, first implementation.
 * \endinternal
 */

#include "rtk.h"
#include "aw_exc.h"
#include "aw_int.h"
#include "aw_hwtimer.h"

/* 全局变量 */
unsigned long  g_aw_sys_ticks_per_second;    /* 系统时钟节拍频率 */

void systick_isr (void *p)
{
    ENTER_INT_CONTEXT();
    rtk_tick_down_counter_announce();
    EXIT_INT_CONTEXT();
}

/**
 * \brief Cortex-M3 Systick 初始化
 *
 * \param ticks_per_second  系统时钟节拍频率
 */
void awbl_stack_overflow_check_init(char *devicetype);
void imx6ul_cpu_idle_sys_tick_init();

static aw_hwtimer_handle_t __g_tmr = NULL;

void systick_init (unsigned long ticks_per_second)
{


    __g_tmr = aw_hwtimer_alloc_byname("imx6ul_timer", 0, 0,
                                   systick_isr, NULL);
    while (AW_HWTIMER_NULL == __g_tmr) {
    }
    aw_hwtimer_enable(__g_tmr, ticks_per_second);
    g_aw_sys_ticks_per_second = ticks_per_second;
    imx6ul_cpu_idle_sys_tick_init();
}

void systick_deinit ()
{
    if (NULL != __g_tmr) {
        aw_hwtimer_disable(__g_tmr);
    }

}

/**
 * \brief 获取系统时钟节拍频率
 */
unsigned long aw_sys_clkrate_get (void)
{
    return g_aw_sys_ticks_per_second;
}

uint32_t systick_timer_get_clk_freq()
{
    uint32_t        freq = 0;
    aw_err_t        err = 0;

    err = aw_hwtimer_clk_freq_get(__g_tmr,&freq);
    if (AW_OK != err) {
        freq = 0;
    }

    return freq;
}

void systick_timer_get_rollover_count(uint32_t *p_rollover_count)
{
    aw_hwtimer_rollover_get(__g_tmr,p_rollover_count);
}

void systick_timer_get_count(uint32_t *p_count)
{
    aw_hwtimer_count_get(__g_tmr,p_count);
}


/* end of file */
