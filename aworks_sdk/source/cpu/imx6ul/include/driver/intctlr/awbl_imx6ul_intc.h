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
 * \brief Freescale i.MX6UL系列CPU通用中断控制器驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-05-06  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_INTC_H
#define __AWBL_IMX6UL_INTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_intctlr.h"
#include "aw_spinlock.h"

#define AWBL_IMX6UL_INTC_NAME           "imx6ul_intc"

#define AWBL_IMX6UL_INTC_MAX_INT_CNT    160

/**
 * \brief NVIC 设备信息
 */
typedef struct awbl_imx6ul_intc_devinfo {

    /** \brief IntCtlr 服务相关信息 */
    struct awbl_intctlr_servinfo intctlr_servinfo;

    /** \brief IntCtlr 寄存器基址 */
    uint32_t regbase;

} awbl_imx6ul_intc_devinfo_t;

/**
 * \brief ISR information
 */
typedef struct awbl_imx6ul_intc_isrinfo {

    /** \brief trigger callback function */
    aw_pfuncvoid_t  pfunc_isr;

    /** \brief argument for\a  pfunc_callback */
    void *p_arg;
} awbl_imx6ul_intc_isrinfo_t;

/**
 * \brief NVIC 设备实例
 */
typedef struct awbl_imx6ul_intc_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief IntCtlr 服务 */
    struct awbl_intctlr_service intctlr_serv;

    /** \brief 设备锁 */
    aw_spinlock_isr_t devlock;

    /** \brief ISR信息*/
    awbl_imx6ul_intc_isrinfo_t isrinfo[AWBL_IMX6UL_INTC_MAX_INT_CNT];

} awbl_imx6ul_intc_dev_t;

/**
 * \brief driver register
 */
void awbl_imx6ul_intc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX6UL_INTC_H */

/* end of file */

