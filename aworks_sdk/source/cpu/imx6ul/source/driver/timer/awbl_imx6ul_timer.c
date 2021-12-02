/*******************************************************************************
*                                  Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6ul 系列定时器驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-05-08  orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/timer/awbl_imx6ul_timer.h"
#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include <stdlib.h> /* for ldiv_t and ldiv() */

/**
 * \addtogroup grp_imx6ul_bsp IMX6UL BSP支持
 * @{
 */

/**
 * \addtogroup grp_awbus_lite_driver AWbus lite驱动
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx6ul_timer IMX6UL定时器驱动
 * @{
 */

/**
 * \defgroup grp_awbl_drv_imx6ul_timer_functions 定时器驱动关键函数
 * @{
 */

/** \brief macro to convert a p_drv to timer pointer */
#define __pdrv_to_imx6ul_timer(p_drv)    ((struct awbl_imx6ul_timer *)p_drv)

/*******************************************************************************
    implementation
*******************************************************************************/

/**
 * \brief disable the imx6ul timer
 *
 * This routine is called to disable the timer.
 *
 * \return AW_OK or AW_ERROR if timer cannot be disabled
 */
aw_local aw_err_t __imx6ul_timer_disable (void *p_drv)
{
    struct awbl_imx6ul_timer *p_tmr = __pdrv_to_imx6ul_timer(p_drv);
    const struct awbl_imx6ul_timer_param *p_param;
    int     i;

    p_param = awbldev_get_imx6ul_timer_param(p_tmr);

    AW_REG_BITS_SET32(&p_tmr->reg->CR,0,1,0);

    /* 禁能EPIT时钟 */
    for (i = 0; i< p_param->dev_clk_count;i++) {
        aw_clk_disable(p_param->dev_clks[i]);
    }

    return AW_OK;
}

/**
 * \brief enable the imx6ul timer
 *
 * This routine is called to enable the imx6ul timer.
 *
 * \return AW_OK or AW_ERROR if timer cannot be enabled
 */
aw_local aw_err_t __imx6ul_timer_enable (void *p_drv, uint32_t max_timer_count)
{
    struct awbl_imx6ul_timer       *p_tmr = __pdrv_to_imx6ul_timer(p_drv);
    imx6ul_epit_reg                *p_reg = p_tmr->reg;
    int                             i;

    const struct awbl_imx6ul_timer_param *p_param;

    p_param = awbldev_get_imx6ul_timer_param(p_tmr);

    /* 使能EPIT时钟 */
    for (i = 0; i< p_param->dev_clk_count;i++) {
        aw_clk_enable(p_param->dev_clks[i]);
    }
    
    /* store the maximum count value */
    p_tmr->count = max_timer_count;

    /* 首先要禁用此定时器 */
    AW_REG_BITS_SET32(&p_reg->CR,0,1,0);
    /* 设置预分频 */
    AW_REG_BITS_SET32(&p_reg->CR,4,12,0);
    /* 选择ipg_clk为源时钟 */
    AW_REG_BITS_SET32(&p_reg->CR,24,2,1);
    /* 不输出到引脚 */
    AW_REG_BITS_SET32(&p_reg->CR,22,2,0);

    /* 停止和等待状态下EPIT是否使能 */
    AW_REG_BITS_SET32(&p_reg->CR,21,1,0);
    AW_REG_BITS_SET32(&p_reg->CR,19,1,0);
    /* 设置重新加载模式 */
    AW_REG_BITS_SET32(&p_reg->CR,3,1,1);
    /* 启用中断 */
    AW_REG_BITS_SET32(&p_reg->CR,2,1,1);
    AW_REG_BITS_SET32(&p_reg->CR,1,1,1);

    /* 设置最大值 */
    writel(max_timer_count, &p_reg->LR);
    /* 设置比较值 */
    writel(0, &p_reg->CMPR);
    /* 清除标志 */
    writel(1, &p_reg->SR);
    /* 使能 */
    AW_REG_BITS_SET32(&p_reg->CR,0,1,1);
    return AW_OK;
}

/**
 * \brief allocate resources for the imx6ul timer
 *
 * This is the function called to allocate resources for the
 * imx6ul timer.
 *
 * \return AW_OK or AW_ERROR if timer allocation fails
 */
aw_local aw_err_t __imx6ul_timer_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no)
{
    struct awbl_imx6ul_timer *p_tmr;

    /* check whether the parameters are valid */
    aw_assert((p_dev != NULL) && (pp_drv != NULL));
    if (timer_no != 0) {
        return AW_ERROR;    /* invalid parameters return AW_ERROR */
    }

    p_tmr = awbldev_to_imx6ul_timer(p_dev);

    /* if the timer is already allocated, return AW_ERROR */
    if (AW_FALSE != p_tmr->tmr.allocated) {
        return AW_ERROR;
    }
    /* set the allocated flag */
    p_tmr->tmr.allocated = AW_TRUE;

    /* update the driver information */
    *pp_drv = (void *)p_tmr;

    return aw_int_enable(awbldev_get_imx6ul_timer_param(p_dev)->inum);
}

/**
 * \brief release the imx6ul timer resources
 *
 * This is the function called to release the resources allocated for the
 * imx6ul timer
 *
 * \return AW_OK or AW_ERROR if parameter is invalid
 */
aw_local aw_err_t __imx6ul_timer_release (void  *p_drv)
{
    struct awbl_imx6ul_timer *p_tmr;

    /* assert parameters */
    aw_assert(p_drv != NULL);

    /* retrieve the imx6ul timer pointer */
    p_tmr = __pdrv_to_imx6ul_timer(p_drv);

    /* if the timer is not allocated, return AW_ERROR */
    if (AW_FALSE == p_tmr->tmr.allocated) {
        return AW_ERROR;
    }

    /* disable the timer */
    __imx6ul_timer_disable(p_drv);

    /* reset the ISR function related values */
    p_tmr->pfunc_isr = NULL;
    p_tmr->p_arg     = NULL;

    /* reset the allocated flag */
    p_tmr->tmr.allocated = AW_FALSE;

    return aw_int_disable(awbldev_get_imx6ul_timer_param(&(p_tmr->dev))->inum);
}

/**
 * \brief retrieve the current value of the timer counter
 *
 * This function is used to retrieve the current value of the timer counter.
 * The current value is returned in 'p_count' parameter.
 *
 * \return AW_OK or -AW_EINVAL if the parameter is invalid.
 */
aw_local aw_err_t __imx6ul_timer_count_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx6ul_timer *p_tmr = __pdrv_to_imx6ul_timer(p_drv);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }
    /* note: the running count register is decreased each tick */
    *p_count = p_tmr->count - readl(&p_tmr->reg->CNR);

    return AW_OK;
}

/**
 * \brief retrieve the maximum value of imx6ul timer count
 *
 * This is the function called to retrieve the maximum value of the
 * imx6ul timer counter. The maximum value is returned
 * in 'p_count' parameter.
 *
 * \return AW_OK or AW_ERROR if the parameter is invalid.
 */
aw_local aw_err_t __imx6ul_timer_rollover_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx6ul_timer *p_tmr = __pdrv_to_imx6ul_timer(p_drv);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }
    *p_count = p_tmr->count;

    return AW_OK;
}

/**
 * \brief set a function to be called on the imx6ul timer interrupt
 *
 * This function is used to set a function which can be called on every
 * imx6ul timer interrupt.
 *
 * \return AW_OK or AW_ERROR if a function cannot be set
 */
aw_local aw_err_t __imx6ul_timer_isr_set (void  *p_drv,
                                         void (*pfunc_isr) (void *),
                                         void  *p_arg)
{
    struct awbl_imx6ul_timer *p_tmr = __pdrv_to_imx6ul_timer(p_drv);

    aw_assert(p_tmr != NULL);

    /* copy the ISR function and the argument */
    p_tmr->pfunc_isr = pfunc_isr;
    p_tmr->p_arg     = p_arg;

    return AW_OK;
}

/**
 * \brief method to retrieve the imx6ul timer functionality
 *
 * This function is the driver method used to retrieve the
 * imx6ul timer functionality.
 *
 * \return AW_OK or AW_ERROR if functionality is not retrieved.
 */
aw_local aw_err_t __imx6ul_timer_func_get (struct awbl_dev    *p_dev,
                                          struct awbl_timer **pp_awbl_tmr,
                                          int                 timer_no)
{
    aw_assert((p_dev != NULL) && (pp_awbl_tmr != NULL));

    if (timer_no != 0) {
        return AW_ERROR;
    }

    /* retrieve the timer data */
    *pp_awbl_tmr = &(awbldev_to_imx6ul_timer(p_dev)->tmr);

    return AW_OK;
}

/**
 * \brief ISR for imx6ul timer interrupts
 *
 * This routine handles the imx6ul timer interrupts. This function
 * is hooked as the ISR in the second level initialization routine.
 */
aw_local void __imx6ul_timer_isr (struct awbl_imx6ul_timer *p_tmr)
{
    void (*pfunc_isr) (void *);

    /* clear IRQ flag */
    writel(1, &p_tmr->reg->SR);

    /* if an ISR function is available, call it */
    pfunc_isr = p_tmr->pfunc_isr;
    if (pfunc_isr != NULL) {
        (*pfunc_isr)(p_tmr->p_arg);
    }
}

/*******************************************************************************
    timer driver functions
*******************************************************************************/
aw_local aw_const struct awbl_timer_functions __g_imx6ul_timer_functions = {
    __imx6ul_timer_allocate,     /* pfunc_allocate */
    __imx6ul_timer_release,      /* pfunc_release */
    __imx6ul_timer_rollover_get, /* pfunc_rollover_get */
    __imx6ul_timer_count_get,    /* pfunc_count_get */
    __imx6ul_timer_disable,      /* pfunc_disable */
    __imx6ul_timer_enable,       /* pfunc_enable */
    __imx6ul_timer_isr_set,      /* pfunc_isr_set */
    NULL,                       /* pfunc_enable64 */
    NULL,                       /* pfunc_rollover_get64 */
    NULL                        /* pfunc_count_get64 */
};

/**
 * \brief first level initialization routine of timers on chip
 *
 * This is the function called to perform the first level initialization of
 * the imx6ul timer.
 *
 * \note
 * This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
aw_local void __imx6ul_timer_inst_init (struct awbl_dev *p_dev)
{
    struct awbl_timer                    *p_awbl_tmr;    /* awbl timer data */
    struct awbl_imx6ul_timer             *p_tmr;         /* imx timer pointer */
    const struct awbl_imx6ul_timer_param *p_param;       /* timer parameter */
    uint32_t                              freq;

    p_param     = awbldev_get_imx6ul_timer_param(p_dev);
    p_tmr       = awbldev_to_imx6ul_timer(p_dev);
    
    p_awbl_tmr  = &(p_tmr->tmr);

    /* TODO: do timer register mapping here */
    /* awbl_reg_map(p_dev, p_param->reg, &p_tmr->handle); */

    /* Initialize platform related stuff */
    if (p_param->pfunc_plfm_init != NULL) {
        p_param->pfunc_plfm_init();
    }
    
    freq = aw_clk_rate_get(p_param->ref_clk_id);

    /* set lpc timer register base address */
    p_tmr->reg   = (imx6ul_epit_reg *)(p_param->reg_addr);
    p_tmr->count = 0;    
    p_tmr->param.clk_frequency = freq;
    p_tmr->param.max_frequency = freq/ 2;
    p_tmr->param.min_frequency = 1;
    
    p_tmr->param.features = AW_HWTIMER_CAN_INTERRUPT
                          | AW_HWTIMER_INTERMEDIATE_COUNT
                          | AW_HWTIMER_AUTO_RELOAD
                          | AW_HWTIMER_SIZE(32);    

    p_tmr->rollover_period =  0xFFFFFFFF / freq;
    
    /* get timer parameter */
    p_awbl_tmr->p_param = &p_tmr->param;

    /* set the function pointers */
    p_awbl_tmr->p_func = &__g_imx6ul_timer_functions;

    /* set allocated flag as AW_FALSE */
    p_awbl_tmr->allocated = AW_FALSE;

    /* update the current frequency in ticks */
    p_awbl_tmr->ticks_per_second = 60u;
    
    
    /** 定时器通道属性配置，单通道只需要判断bit0为是否置为即可 */
    if (AW_BIT_ISSET(p_param->alloc_byname,0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }

    /* connect the ISR */
    while (AW_OK != aw_int_connect(p_param->inum,
                                   (aw_pfuncvoid_t)__imx6ul_timer_isr,
                                   (void *)awbldev_to_imx6ul_timer(p_dev))) {
    }

}

/**
 * \brief initialization routine of millisecond timer
 *
 * This routine performs the second level initialization of the
 * imx6ul timer.
 *
 * This routine is called later during system initialization. OS features
 * such as memory allocation are available at this time.
 */
aw_local void __imx6ul_timer_inst_init2 (struct awbl_dev *p_dev)
{
//    const struct awbl_imx6ul_timer_param *p_param;       /* timer parameter */
//
//    p_param = awbldev_get_imx6ul_timer_param(p_dev);
//
//    /* connect the ISR */
//    while (AW_OK != aw_int_connect(p_param->inum,
//                                   (aw_pfuncvoid_t)__imx6ul_timer_isr,
//                                   (void *)awbldev_to_imx6ul_timer(p_dev))) {
//    }
}

/**
 * \brief third level initialization routine of imx6ul timer
 *
 * This is the function called to perform the third level initialization of
 * the imx6ul timer.
 */
aw_local void __imx6ul_timer_inst_connect (struct awbl_dev *p_dev)
{
    /* nothing to do */
}

/*******************************************************************************
    driver registeration
*******************************************************************************/

/* driver functions */
aw_const struct awbl_drvfuncs __g_imx6ul_timer_drvfuncs = {
    __imx6ul_timer_inst_init,    /* devInstanceInit */
    __imx6ul_timer_inst_init2,   /* devInstanceInit2 */
    __imx6ul_timer_inst_connect  /* devConnect */
};

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx6ul_timer_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx6ul_timer_func_get),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * IMX6ul compatible timer devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx6ul_timer_drvinfo = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    "imx6ul_timer",                 /* p_drvname */
    &__g_imx6ul_timer_drvfuncs,      /* p_busfuncs */
    &__g_imx6ul_timer_methods[0],    /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/**
 * \brief register imx6ul timer driver
 *
 * This routine registers the imx6ul timer driver and device
 * recognition data with the AWBus subsystem.
 */
void awbl_imx6ul_timer_drv_register (void)
{
    awbl_drv_register(&__g_imx6ul_timer_drvinfo);
}
/** @} grp_awbl_drv_imx6ul_timer_functions*/
/** @} grp_awbl_drv_imx6ul_timer*/
/** @} grp_awbus_lite_driver*/
/** @} grp_imx6ul_bsp*/

/* end of file */
