/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX6UL CAAM 驱动
 *
 * iMX6UL CAAM 为 iMX6UL 系列MCU的片上加密加速器件
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "imx6ul_caam"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息:
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-07-21, jac, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_CAAM_H
#define __AWBL_IMX6UL_CAAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include <apollo.h>
#include "aw_sem.h"

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam IMX6UL CAAM驱动
 * \copydetails awbl_imx6ul_caam.h
 * @{
 */



/**
 * \defgroup grp_awbl_drv_imx6ul_caam_micro_defs IMX6UL CAAM驱动宏定义
 * @{
 */

/** \brief imx6ul CAAM 驱动名 */
#define AWBL_IMX6UL_CAAM_NAME   "awbl_imx6ul_caam"

/** @} grp_awbl_drv_imx6ul_caam_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_caam_structs IMX6UL CAAM驱动相关的结构
 * @{
 */





/** \brief dcaam device channel structure for awbus lite */
typedef struct __tag_awbl_imx6ul_caam_dev
{
    /**< \brief device */
    struct awbl_dev             dev;
    AW_MUTEX_DECL(mutex_dev);
    uint8_t                     caam_priv[1060];
} awbl_imx6ul_caam_dev_t;


/** @} grp_awbl_drv_imx6ul_caam_structs*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam_awbus_interface Awbus接口
 * @{
 */
/** \brief 用户调用此函数注册CAAM驱动 */
void awbl_imx6ul_caam_drv_register (void);

/** @} grp_awbl_drv_imx6ul_caam_awbus_interface */
/** @} grp_awbl_drv_imx6ul_caam*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
