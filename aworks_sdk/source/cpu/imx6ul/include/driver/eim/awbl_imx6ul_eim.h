/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL EIM head file
 *
 * define IMX6UL EIM data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-16  deo, first implementation
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_EIM_H
#define __AWBL_IMX6UL_EIM_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "apollo.h"
#include "awbl_eim_bus.h"
#include "imx6ul_eim_regs.h"

/** \brief The Driver name define */
#define AWBL_IMX6UL_EIM_NAME   "awbl_imx6ul_eim"

/** \brief CS ID define, The EIM supports up to four EIM CS selects */
#define AWBL_IMX6UL_EIM_CS0         0
#define AWBL_IMX6UL_EIM_CS1         1
#define AWBL_IMX6UL_EIM_CS2         2
#define AWBL_IMX6UL_EIM_CS3         3


/*******************************************************************************
    register config define
*******************************************************************************/
/**
 * \brief SJA1000配置
 * @{
 */
#define SJA1000_CONFIG  {SJA1000_GENERAL_CONFIG},\
                        {SJA1000_READ_TIMING},\
                        {SJA1000_WRITE_TIMING}

#define SJA1000_GENERAL_CONFIG \
    1, \
    0, \
    0, \
    1, \
    0, \
    0, \
    0, \
    0, \
    0, \
    0, \
    0, \
    0, \
    1, \
    0, \
    0, \
    0, \
    0, \
    0, \
    0, \
    2, \
    0, \
    0, \
    0, \
    0

#define SJA1000_READ_TIMING \
    0, \
    2, \
    0, \
    0, \
    0, \
    0, \
    0, \
    5, \
    0, \
    1, \
    1, \
    0, \
    0, \
    0

#define SJA1000_WRITE_TIMING \
    0, \
    2, \
    0, \
    2, \
    0, \
    1, \
    0, \
    0, \
    4, \
    1, \
    0, \
    0

/** @} */



/*******************************************************************************/


/** \brief IMX6UL EIM device info */
struct awbl_imx6ul_eim_devinfo {

	/** \brief  The eim controller information */

    /** \brief 继承自 AWBus EIM 控制器设备信息 */
    struct awbl_eim_master_devinfo eim_master_devinfo;

    /** \brief The eim regbase */
    uint32_t     regbase;

    /** \brief The eim memory addrbase */
    uint32_t     transmission_base;

    /** \brief CS的大小 */
    uint32_t     cs_size;

    /** \brief CS的配置 */
    struct awbl_eim_config cfg[4];

    /** \brief The platform */
    void (*pfn_plfm_init)(void);
};

struct imx6ul_eim_regs;

/** \brief IMX6UL EIM device */
struct awbl_imx6ul_eim_dev {
    /** \brief 继承自 AWBus EIM 主机控制器 */
    struct awbl_eim_master       super;

    struct imx6ul_eim_reg *cs_regbase[4];
    uint32_t cs_transmission_base[4];
};

void awbl_imx6ul_eim_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif /* __AWBL_IMX28_EIM_H */
