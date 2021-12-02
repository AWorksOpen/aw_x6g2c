/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL timestamp 驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 19-09-03  zaf, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_TIMESTAMP_H
#define __AWBL_IMX6UL_TIMESTAMP_H

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_timestamp.h"
#include "aw_clk.h"

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

/** \brief timestamp 驱动名 */
#define AWBL_IMX6UL_TIMESTAMP_NAME    "awbl_imx6ul_timestamp"

/** \brief 设备配置信息 */
typedef struct awbl_imx6ul_timestamp_devinfo {

    const int          inum;           /**< \brief 中断号 */
    const aw_clk_id_t  clk_id;         /**< \brief 指向时钟 ID 数组的指针 */

} awbl_imx6ul_timestamp_devinfo_t;


/**
 * \brief timestamp 设备实例
 */
typedef struct awbl_imx6ul_timestamp_dev {

    struct awbl_dev super;    /**< \brief 继承自 AWBus 设备 */
    uint32_t        freq;     /**< \brief timestamp频率 */

} awbl_imx6ul_timestamp_dev_t;

/**
 * \brief 将 IMX6UL timestamp 驱动注册到 AWBus 子系统中
 */
void awbl_imx6ul_timestamp_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif
