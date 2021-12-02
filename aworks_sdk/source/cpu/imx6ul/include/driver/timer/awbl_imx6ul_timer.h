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
 * \brief IMX6UL ϵ�ж�ʱ������
 *
 * ������ֻʵ���˶�ʱ���������жϹ��ܣ����ṩ������ƥ�䡢������ת�����PWM���ܡ�
 *
 * \par 1.������Ϣ
 *
 *  - ������:  "imx28_timer"(32-bit timer)
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_lpc11xx_timer_param
 *
 * \par 2.�����豸
 *
 *  - ��˼����IMX6ULϵ��MCU
 *
 * \par 3.�豸����/����
 *
 *  - \ref EPIT
 *
 * \par 4.�û��ӿ�
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
 * \addtogroup grp_awbl_drv_imx6ul_timer IMX6UL��ʱ������
 * \copydetails awbl_imx6ul_timer.h
 * @{
 */

/**
 * \defgroup EPIT IMX6UL EPIT(��ǿ�����ж϶�ʱ��)�ļĴ�������
 * @{
 */

#pragma pack(push)
#pragma pack(1)
/** \brief EPIT�Ĵ������� */
typedef struct __imx6_epit_reg
{
    volatile uint32_t CR;           //!< Control register
    volatile uint32_t SR;           //!< Status register
    volatile uint32_t LR;           //!< Load register
    volatile uint32_t CMPR;         //!< Compare register
    volatile uint32_t CNR;          //!< Counter register
} imx6ul_epit_reg;
#pragma pack(pop)

/** @} */           // end of EPIT�Ĵ�������


/**
 * \defgroup EPIT_Driver IMX6UL��ʱ��������صĽṹ
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
 * \brief ��AWBus�豸��Ϣָ���ȡimx6ul��ʱ��������Ϣ
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
