/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      AWorks.support@zlg.cn
*******************************************************************************/
/**
 * \file  aw_atomic.c
 * \brief 原子操作接口实现
 *
 * \internal
 * \par modification history:
 * - 2017-2-13 mex, first implementation.
 * \endinternal
 */

#include "aw_atomic.h"
#include "rtk.h"


int aw_atomic_add_return(int add_val, atomic_t *p_val)
{
    int result = 0;
    int old    = 0;
    old = arch_interrupt_disable();

    result = p_val->counter;
    result += add_val;
    p_val->counter = result;

    arch_interrupt_enable(old);
    return result;
}


int aw_atomic_sub_return(int sub_val, atomic_t *p_val)
{
    int result = 0;
    int old    = 0;

    old = arch_interrupt_disable();
    result = p_val->counter;
    result -= sub_val;
    p_val->counter = result;

    arch_interrupt_enable(old);
    return result;
}

int aw_atomic_cmpxchg(int old_val, int new_val, atomic_t *p_val)
{
    int ret = 0;
    int old = 0;

    old = arch_interrupt_disable();
    ret = p_val->counter;
    if (ret == old_val) {
        p_val->counter = new_val;
    }
    arch_interrupt_enable(old);
    return ret;
}

/* end of file */
