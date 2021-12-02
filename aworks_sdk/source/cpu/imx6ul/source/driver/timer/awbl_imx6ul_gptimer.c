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
 * \par modification history
 * - 1.00 16-05-20  sky, first implementation.
 * \endinternal
 */


/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "aw_clk.h"
#include "driver/timer/awbl_imx6ul_gptimer.h"

#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"

#include <stdlib.h> /* for ldiv_t and ldiv() */

/*******************************************************************************
    local defines
*******************************************************************************/
#define __CTRL_EN           0
#define __CTRL_ENMOD        1
#define __CTRL_DEBUGEN      2
#define __CTRL_WAITEN       3
#define __CTRL_DOZEEN       4
#define __CTRL_STOPEN       5
#define __CTRL_CLKSRC       6
#define __CTRL_CLKSRC_LEN   3
#define __CTRL_CLKSRC_NCLK  0
#define __CTRL_CLKSRC_PCLK  1
#define __CTRL_CLKSRC_HCLK  2
#define __CTRL_CLKSRC_ECLK  3
#define __CTRL_CLKSRC_LCLK  4
#define __CTRL_FRR          9
#define __CTRL_EN_24M       10
#define __CTRL_SWR          15
#define __CTRL_IM1          16
#define __CTRL_IM2          18
#define __CTRL_IM_LEN       2
#define __CTRL_IM_DIS       0
#define __CTRL_IM_RISE      1
#define __CTRL_IM_FALL      2
#define __CTRL_IM_BOTH      3
#define __CTRL_OM1          20
#define __CTRL_OM2          23
#define __CTRL_OM3          26
#define __CTRL_OM_LEN       3
#define __CTRL_OM_DIS       0
#define __CTRL_OM_TOG       1
#define __CTRL_OM_CLR       2
#define __CTRL_OM_SET       3
#define __CTRL_OM_LOW_PULSE 4
#define __CTRL_FO1          29
#define __CTRL_FO2          30
#define __CTRL_FO3          31

#define __PRES_24M          12

#define __STAT_OF1          0
#define __STAT_OF2          1
#define __STAT_OF3          2
#define __STAT_IF1          3
#define __STAT_IF2          4
#define __STAT_ROV          5

#define __INT_OF1           0
#define __INT_OF2           1
#define __INT_OF3           2
#define __INT_IF1           3
#define __INT_IF2           4
#define __INT_ROV           5

/** \brief macro to convert a p_drv to gptimer pointer */
#define __pdrv_to_imx6ul_gptimer(p_drv)    ((struct awbl_imx6ul_gptimer *)p_drv)

/*******************************************************************************
    implementation
*******************************************************************************/

/**
 * \brief disable the imx6ul gptimer
 *
 * This routine is called to disable the gptimer.
 *
 * \return AW_OK or AW_ERROR if timer cannot be disabled
 */
aw_local aw_err_t __imx6ul_gptimer_disable (void *p_drv)
{
    awbl_imx6ul_gptimer_t             *p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);
    const awbl_imx6ul_gptimer_param_t *p_param;
    int                                i;

    p_param = awbldev_get_imx6ul_gptimer_param(p_tmr);

    if (p_tmr->enabled) {
        /* 清零控制寄存器 */
        AW_REG_WRITE32(&p_tmr->reg->cr, 0x0);
        /* 禁能GPT时钟 */
        for (i = 0; i< p_param->dev_clk_count;i++) {
            aw_clk_disable(p_param->dev_clks[i]);
        }
        p_tmr->enabled = 0;
    }

    return AW_OK;
}

/**
 * \brief enable the imx6ul gptimer
 *
 * This routine is called to enable the imx6ul gptimer.
 *
 * \return AW_OK or AW_ERROR if timer cannot be enabled
 */
aw_local aw_err_t __imx6ul_gptimer_enable (void *p_drv, uint32_t max_timer_count)
{
    awbl_imx6ul_gptimer_t       *p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);
    awbl_imx6ul_gptimer_param_t *p_param;
    int                          i;

    p_param = awbldev_get_imx6ul_gptimer_param(p_tmr);
    if (p_tmr->enabled) {
        return -AW_EBUSY;
    }
    p_tmr->enabled = 1;

    /* 使能GPT时钟 */
    for (i = 0; i< p_param->dev_clk_count;i++) {
        aw_clk_enable(p_param->dev_clks[i]);
    }

    /* store the maximum count value */
    p_tmr->count = max_timer_count;

    /* 首先要禁用此定时器 */
    AW_REG_WRITE32(&p_tmr->reg->cr, 0x0);

    /* 禁能中断 */
    AW_REG_WRITE32(&p_tmr->reg->ir, 0x0);

    /* 设置预分频 */
    AW_REG_WRITE32(&p_tmr->reg->pr, 0x0);

    /* 选择ipg_clk为源时钟 */
    AW_REG_BITS_SET32(&p_tmr->reg->cr, __CTRL_CLKSRC, __CTRL_CLKSRC_LEN, __CTRL_CLKSRC_PCLK);

    /* 不输出到引脚 */
    AW_REG_BITS_SET32(&p_tmr->reg->cr, __CTRL_OM1, __CTRL_OM_LEN, __CTRL_OM_DIS);

    /* 停止和等待状态下GPT是否使能 */
    AW_REG_BIT_CLR32(&p_tmr->reg->cr, __CTRL_STOPEN);
    AW_REG_BIT_CLR32(&p_tmr->reg->cr, __CTRL_WAITEN);

    /* 设置重新加载模式 */
    AW_REG_BIT_CLR32(&p_tmr->reg->cr, __CTRL_FRR);

    /* 输出比较通道1的比较值设置到最大，避免重新加载模式下计数归0影响到其他功能  */
    AW_REG_WRITE32(&p_tmr->reg->ocr1, 0xffffffff);

    /* 模式检索 */
    switch (p_param->mode) {

    case IMX6UL_GPT_MODE_TIMER:

        /* 启用中断 */
        AW_REG_BIT_SET32(&p_tmr->reg->ir, __INT_OF1);

        /* 设置计数最大值(即比较值) */
        AW_REG_WRITE32(&p_tmr->reg->ocr1, max_timer_count);

        /* 清除标志(写1清0) */
        AW_REG_BIT_SET32(&p_tmr->reg->sr, __STAT_OF1);

        /* 清空计数值 */
        AW_REG_BIT_SET32(&p_tmr->reg->cr, __CTRL_ENMOD);

        /* 测试定时器精准度 */
        /* 配置输出模式 */
        AW_REG_BITS_SET32(&p_tmr->reg->cr, __CTRL_OM1, __CTRL_OM_LEN, p_param->feature);

        break;

    case IMX6UL_GPT_MODE_INPUT:

        /* 启用中断 */
        AW_REG_BIT_SET32(&p_tmr->reg->ir, __INT_IF1 + p_param->chan - 1);

        /* 清除标志(写1清0) */
        AW_REG_BIT_SET32(&p_tmr->reg->sr, __STAT_IF1 + p_param->chan - 1);

        /* 清空计数值 */
        AW_REG_BIT_SET32(&p_tmr->reg->cr, __CTRL_ENMOD);

        /* 配置输入模式 */
        AW_REG_BITS_SET32(&p_tmr->reg->cr, __CTRL_IM1 + (p_param->chan - 1) * 2,
                                           __CTRL_IM_LEN, p_param->feature);

        break;

    case IMX6UL_GPT_MODE_OUTPUT:

        /* 启用中断 */
        AW_REG_BIT_SET32(&p_tmr->reg->ir, __INT_OF1 + p_param->chan - 1);

        /* 清除标志(写1清0) */
        AW_REG_BIT_SET32(&p_tmr->reg->sr, __STAT_OF1 + p_param->chan - 1);

        /* 清空计数值 */
        AW_REG_BIT_SET32(&p_tmr->reg->cr, __CTRL_ENMOD);

        /* 设置比较值 */
        if (p_param->chan == 1) {
            AW_REG_WRITE32(&p_tmr->reg->ocr1, p_param->output_compare);
        } else if (p_param->chan == 2) {
            AW_REG_WRITE32(&p_tmr->reg->ocr2, p_param->output_compare);
        } else if (p_param->chan == 3) {
            AW_REG_WRITE32(&p_tmr->reg->ocr3, p_param->output_compare);
        }

        /* 配置输出模式 */
        AW_REG_BITS_SET32(&p_tmr->reg->cr, __CTRL_OM1 + (p_param->chan - 1) * 3,
                                           __CTRL_OM_LEN, p_param->feature);

        break;

    default:
        break;

    }

    /* 使能 */
    AW_REG_BIT_SET32(&p_tmr->reg->cr, __CTRL_EN);

    return AW_OK;
}

/**
 * \brief allocate resources for the imx6ul gptimer
 *
 * This is the function called to allocate resources for the
 * imx6ul gptimer.
 *
 * \return AW_OK or AW_ERROR if timer allocation fails
 */
aw_local aw_err_t __imx6ul_gptimer_allocate (struct awbl_dev *p_dev,
                                             uint32_t         flags,
                                             void           **pp_drv,
                                             int              timer_no)
{
    struct awbl_imx6ul_gptimer         *p_tmr;
    int                                 ret = AW_ERROR;

    /* check whether the parameters are valid */
    aw_assert((p_dev != NULL) && (pp_drv != NULL));
    if (timer_no != 0) {
        return ret;
    }
    p_tmr = awbldev_to_imx6ul_gptimer(p_dev);

    /* if the timer is already allocated, return AW_ERROR */
    if (AW_FALSE != p_tmr->tmr.allocated) {
        goto cleanup;
    }
    /* set the allocated flag */
    p_tmr->tmr.allocated = AW_TRUE;

    /* update the driver information */
    *pp_drv = (void *)p_tmr;

    ret = aw_int_enable(awbldev_get_imx6ul_gptimer_param(p_dev)->inum);
cleanup:
    return  ret;
}

/**
 * \brief release the imx6ul gptimer resources
 *
 * This is the function called to release the resources allocated for the
 * imx6ul gptimer
 *
 * \return AW_OK or AW_ERROR if parameter is invalid
 */
aw_local aw_err_t __imx6ul_gptimer_release (void  *p_drv)
{
    struct awbl_imx6ul_gptimer *p_tmr;

    /* assert parameters */
    aw_assert(p_drv != NULL);

    /* retrieve the imx6ul timer pointer */
    p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);

    /* if the timer is not allocated, return AW_ERROR */
    if (AW_FALSE == p_tmr->tmr.allocated) {
        return AW_ERROR;
    }

    /* disable the timer */
    __imx6ul_gptimer_disable(p_drv);

    /* reset the ISR function related values */
    p_tmr->pfunc_isr = NULL;
    p_tmr->p_arg     = NULL;

    /* reset the allocated flag */
    p_tmr->tmr.allocated = AW_FALSE;

    return aw_int_disable(awbldev_get_imx6ul_gptimer_param(&(p_tmr->dev))->inum);
}

/**
 * \brief retrieve the current value of the gptimer counter
 *
 * This function is used to retrieve the current value of the gptimer counter.
 * The current value is returned in 'p_count' parameter.
 *
 * \return AW_OK or -AW_EINVAL if the parameter is invalid.
 */
aw_local aw_err_t __imx6ul_gptimer_count_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx6ul_gptimer     *p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);
    awbl_imx6ul_gptimer_param_t    *p_param;

    p_param = awbldev_get_imx6ul_gptimer_param(p_tmr);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }

    /* note: the running count register is decreased each tick */

    /* 根据工作模式的不同返回不同值 */
    if (p_param->mode == IMX6UL_GPT_MODE_INPUT) {

        /* 根据输入通道返回计数值 */
        if (p_param->chan == 1) {
            *p_count = readl(&p_tmr->reg->icr1);
//            *p_count = p_tmr->reg->cnt;
        } else if (p_param->chan == 2) {
            *p_count = readl(&p_tmr->reg->icr2);
        }

    } else {

        /* up counter */
        *p_count = readl(&p_tmr->reg->cnt);
    }

    return AW_OK;
}

/**
 * \brief retrieve the maximum value of imx6ul gptimer count
 *
 * This is the function called to retrieve the maximum value of the
 * imx6ul gptimer counter. The maximum value is returned
 * in 'p_count' parameter.
 *
 * \return AW_OK or AW_ERROR if the parameter is invalid.
 */
aw_local aw_err_t __imx6ul_gptimer_rollover_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx6ul_gptimer *p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }
    *p_count = p_tmr->rollover_period;

    return AW_OK;
}

/**
 * \brief set a function to be called on the imx6ul gptimer interrupt
 *
 * This function is used to set a function which can be called on every
 * imx6ul gptimer interrupt.
 *
 * \return AW_OK or AW_ERROR if a function cannot be set
 */
aw_local aw_err_t __imx6ul_gptimer_isr_set (void  *p_drv,
                                            void (*pfunc_isr) (void *),
                                            void  *p_arg)
{
    struct awbl_imx6ul_gptimer *p_tmr = __pdrv_to_imx6ul_gptimer(p_drv);

    aw_assert(p_tmr != NULL);

    /* copy the ISR function and the argument */
    p_tmr->pfunc_isr = pfunc_isr;
    p_tmr->p_arg     = p_arg;

    return AW_OK;
}

/**
 * \brief method to retrieve the imx6ul gptimer functionality
 *
 * This function is the driver method used to retrieve the
 * imx6ul gptimer functionality.
 *
 * \return AW_OK or AW_ERROR if functionality is not retrieved.
 */
aw_local aw_err_t __imx6ul_gptimer_func_get (struct awbl_dev    *p_dev,
                                             struct awbl_timer **pp_awbl_tmr,
                                             int                 timer_no)
{
    aw_assert((p_dev != NULL) && (pp_awbl_tmr != NULL));

    if (timer_no != 0) {
        return AW_ERROR;
    }

    /* retrieve the timer data */
    *pp_awbl_tmr = &(awbldev_to_imx6ul_gptimer(p_dev)->tmr);

    return AW_OK;
}

/**
 * \brief ISR for imx6ul gptimer interrupts
 *
 * This routine handles the imx6ul gptimer interrupts. This function
 * is hooked as the ISR in the second level initialization routine.
 */
aw_local void __imx6ul_gptimer_isr (struct awbl_imx6ul_gptimer *p_tmr)
{
    void (*pfunc_isr) (void *);
    uint32_t int_stat;

    /* 查看中断状态(调试用) */
    int_stat = AW_REG_READ32(&p_tmr->reg->sr);

    /* clear IRQ flag */
    AW_REG_WRITE32(&p_tmr->reg->sr, 0x3F);

    /* if an ISR function is available, call it */
    pfunc_isr = p_tmr->pfunc_isr;
    if (pfunc_isr != NULL) {
        (*pfunc_isr)(p_tmr->p_arg);
    }
}

/*******************************************************************************
    gptimer driver functions
*******************************************************************************/
aw_local aw_const struct awbl_timer_functions __g_imx6ul_timer_functions = {
    __imx6ul_gptimer_allocate,     /* pfunc_allocate */
    __imx6ul_gptimer_release,      /* pfunc_release */
    __imx6ul_gptimer_rollover_get, /* pfunc_rollover_get */
    __imx6ul_gptimer_count_get,    /* pfunc_count_get */
    __imx6ul_gptimer_disable,      /* pfunc_disable */
    __imx6ul_gptimer_enable,       /* pfunc_enable */
    __imx6ul_gptimer_isr_set,      /* pfunc_isr_set */
    NULL,                          /* pfunc_enable64 */
    NULL,                          /* pfunc_rollover_get64 */
    NULL                           /* pfunc_count_get64 */
};

/**
 * \brief first level initialization routine of timers on chip
 *
 * This is the function called to perform the first level initialization of
 * the imx6ul gptimer.
 *
 * \note
 * This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
aw_local void __imx6ul_gptimer_inst_init (struct awbl_dev *p_dev)
{
    struct awbl_timer                      *p_awbl_tmr;    /* awbl timer data */
    struct awbl_imx6ul_gptimer             *p_tmr;         /* imx timer pointer */
    const struct awbl_imx6ul_gptimer_param *p_param;       /* timer parameter */
    int                                     i;

    p_param     = awbldev_get_imx6ul_gptimer_param(p_dev);
    p_tmr       = awbldev_to_imx6ul_gptimer(p_dev);
    
    p_awbl_tmr  = &(p_tmr->tmr);

    /* TODO: do timer register mapping here */
    /* awbl_reg_map(p_dev, p_param->reg, &p_tmr->handle); */

    /* Initialize platform related stuff */
    if (p_param->pfunc_plfm_init != NULL) {
        p_param->pfunc_plfm_init();
    }

    /* set imx6ul gptimer register base address */
    p_tmr->reg   = (imx6ul_gptimer_reg_t *)(p_param->reg_addr);
    p_tmr->count = 0;
    p_tmr->enabled = 0;
    
    /* 使能GPT时钟 */
    for (i = 0; i< p_param->dev_clk_count;i++) {
        aw_clk_enable(p_param->dev_clks[i]);
    }

    p_tmr->param.clk_frequency = aw_clk_rate_get(IMX6UL_CLK_PERCLK);

    /* 禁能GPT时钟 */
    for (i = 0; i< p_param->dev_clk_count;i++) {
        aw_clk_disable(p_param->dev_clks[i]);
    }

    p_tmr->param.max_frequency = p_tmr->param.clk_frequency / 2;
    p_tmr->param.min_frequency = 1;
    
    p_tmr->param.features = AW_HWTIMER_CAN_INTERRUPT
                          | AW_HWTIMER_INTERMEDIATE_COUNT
                          | AW_HWTIMER_AUTO_RELOAD
                          | AW_HWTIMER_SIZE(32);

    p_tmr->rollover_period =  0xFFFFFFFF / (p_tmr->param.clk_frequency);
    
    /* get timer parameter */
    p_awbl_tmr->p_param = &p_tmr->param;

    /* set the function pointers */
    p_awbl_tmr->p_func = &__g_imx6ul_timer_functions;

    /* set allocated flag as FALSE */
    p_awbl_tmr->allocated = AW_FALSE;

    /* update the current frequency in ticks */
    p_awbl_tmr->ticks_per_second = 60u;
    
    
    /* 定时器通道属性配置，单通道只需要判断bit0为是否置为即可 */
    if (AW_BIT_ISSET(p_param->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }

}

/**
 * \brief initialization routine of millisecond timer
 *
 * This routine performs the second level initialization of the
 * imx6ul gptimer.
 *
 * This routine is called later during system initialization. OS features
 * such as memory allocation are available at this time.
 */
aw_local void __imx6ul_gptimer_inst_init2 (struct awbl_dev *p_dev)
{
    const struct awbl_imx6ul_gptimer_param *p_param;       /* timer parameter */

    p_param = awbldev_get_imx6ul_gptimer_param(p_dev);

    /* connect the ISR */
    while (AW_OK != aw_int_connect(p_param->inum,
                                   (aw_pfuncvoid_t)__imx6ul_gptimer_isr,
                                   (void *)awbldev_to_imx6ul_gptimer(p_dev))) {
    }
}

/**
 * \brief third level initialization routine of imx6ul gptimer
 *
 * This is the function called to perform the third level initialization of
 * the imx6ul timer.
 */
aw_local void __imx6ul_gptimer_inst_connect (struct awbl_dev *p_dev)
{
    /* nothing to do */
}

/*******************************************************************************
    driver registeration
*******************************************************************************/

/* driver functions */
aw_const struct awbl_drvfuncs __g_imx6ul_gptimer_drvfuncs = {
    __imx6ul_gptimer_inst_init,    /* devInstanceInit */
    __imx6ul_gptimer_inst_init2,   /* devInstanceInit2 */
    __imx6ul_gptimer_inst_connect  /* devConnect */
};

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx6ul_gptimer_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx6ul_gptimer_func_get),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * IMX6ul compatible gptimer devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx6ul_gptimer_drvinfo = {
    AWBL_VER_1,                        /* awb_ver */
    AWBL_BUSID_PLB,                    /* bus_id */
    AWBL_IMX6UL_GPT_DRV_NAME,          /* p_drvname */
    &__g_imx6ul_gptimer_drvfuncs,      /* p_busfuncs */
    &__g_imx6ul_gptimer_methods[0],    /* p_methods */
    NULL                               /* pfunc_drv_probe */
};

/**
 * \brief register imx6ul gptimer driver
 *
 * This routine registers the imx6ul gptimer driver and device
 * recognition data with the AWBus subsystem.
 */
void awbl_imx6ul_gptimer_drv_register (void)
{
    awbl_drv_register(&__g_imx6ul_gptimer_drvinfo);
}

/* end of file */
