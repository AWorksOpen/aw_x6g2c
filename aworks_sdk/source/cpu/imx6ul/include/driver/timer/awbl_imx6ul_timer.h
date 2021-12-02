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
 * \brief IMX6UL 系列定时器驱动
 *
 * 本驱动只实现了定时器的周期中断功能，不提供计数、匹配、捕获、旋转编码和PWM功能。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:  "imx28_timer"(32-bit timer)
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc11xx_timer_param
 *
 * \par 2.兼容设备
 *
 *  - 飞思卡尔IMX6UL系列MCU
 *
 * \par 3.设备定义/配置
 *
 *  - \ref EPIT
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_hwtimer
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-15  jac, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_TIMER_H
#define __AWBL_IMX6UL_TIMER_H

#include "awbl_timer.h"
#include "aw_clk.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_imx6ul_timer IMX6UL定时器驱动
 * \copydetails awbl_imx6ul_timer.h
 * @{
 */

/**
 * \defgroup EPIT IMX6UL EPIT(增强周期中断定时器)的寄存器定义
 * @{
 */

#pragma pack(push)
#pragma pack(1)
/** \brief EPIT寄存器汇总 */
typedef struct __imx6_epit_reg
{
    volatile uint32_t CR;           //!< Control register
    volatile uint32_t SR;           //!< Status register
    volatile uint32_t LR;           //!< Load register
    volatile uint32_t CMPR;         //!< Compare register
    volatile uint32_t CNR;          //!< Counter register
} imx6ul_epit_reg;
#pragma pack(pop)

/** @} */           // end of EPIT寄存器定义


/**
 * \defgroup EPIT_Driver IMX6UL定时器驱动相关的结构
 * @{
 */
/** \brief timer fixed parameter */
struct awbl_imx6ul_timer_param {
    uint32_t            alloc_byname;   /**< \brief the bit with 1 means the chan the timer can any be alloc by name   */
    uint32_t            reg_addr;       /**< \brief register base address   */
    int                 inum;           /**< \brief interrupt number        */
    aw_clk_id_t         ref_clk_id;     /**< \brief the clock which the counter use */
    const aw_clk_id_t  *dev_clks;       /**< \brief the clocks which the timer need */
    int                 dev_clk_count;  /**< \brief count of device clocks */
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing   */ 
};

 /*!
 * \def awbldev_get_imx6ul_timer_param(p_dev)
 *
 * \brief 从AWBus设备信息指针获取imx6ul定时器参数信息
 */
#define awbldev_get_imx6ul_timer_param(p_dev) \
    ((struct awbl_imx6ul_timer_param *)AWBL_DEVINFO_GET(p_dev))
 
/******************************************************************************/

/** \brief imx6ul Timer type for AWBus */
struct awbl_imx6ul_timer {
    /* always go first */
    awbl_dev_t              dev;                /**< \brief AWBus device data */
    struct awbl_timer       tmr;                /**< \brief AWBL timer instance */
    imx6ul_epit_reg         *reg;               /**< \brief register base */
    
    uint32_t                count;              /**< \brief current max counts */
    uint32_t                rollover_period;    /**< \brief rollover_period */
    struct awbl_timer_param  param;             /**< \brief the timer's param */
    
    void (*pfunc_isr) (void *p_arg);            /**< \brief ISR */
    void  *p_arg;                               /**< \brief ISR argument */
};

/** \brief Convert a awbl_timer pointer to a awbl_imx6ul_timer pointer */
#define awbl_timer_to_imx6ul_timer(p_timer) \
    ((struct awbl_imx6ul_timer *)(p_timer))

/** \brief convert a awbl_dev pointer to awbl_imx6ul_timer pointer */
#define awbldev_to_imx6ul_timer(p_dev) \
          ((struct awbl_imx6ul_timer *)(p_dev))

/** \brief get IMX6UL timer parameter */
#define imx6ul_timer_get_param(p_imxtimer)  ((struct awbl_imx6ul_timer_param *) \
    AWBL_DEVINFO_GET(&((p_imxtimer)->tmr.dev)))

/******************************************************************************/


/** @} */           // end of EPIT_Driver

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* } __AWBL_IMX6UL_TIMER_H */

/* end of file */
