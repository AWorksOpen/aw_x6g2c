/*******************************************************************************
*                                 Apollo
*                       ---------------------------
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
 * \brief The IMX6UL Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_MQS_H
#define __AWBL_IMX6UL_MQS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"

#define AWBL_IMX6UL_MQS_NAME   "awbl_imx6ul_mqs"

/**
 * \brief imx6ul MQS 设备信息
 */
struct awbl_imx6ul_mqs_devinfo {

    uint32_t        iomux_grp2_regaddr;     /**< \brief IOMUX_GRP2 寄存器地址                         */
    aw_clk_id_t     ref_clk_id;             /**< \brief refence clock id*/

    void (*pfn_plfm_init)(void);            /**< \brief 平台相关初始化 */
};

/**
 * \brief imx6ul MQS 设备实例
 */
struct awbl_imx6ul_mqs_dev {
    struct awbl_dev  dev;
};

/**
 * \brief enbale the MQS
 *
 * \param[in] samples_rate : samples rate
 * \param[in] width        : data width, only support 16 now
 *
 * \return AW_OK
 */
aw_err_t aw_imx6ul_mqs_enable (uint32_t samples_rate,
                               uint8_t  width,
                               uint8_t  channels);

/**
 * \brief Disable the MQS
 * \return AW_OK
 */
aw_err_t  aw_imx6ul_mqs_disable (void);

/**
 * \brief Reset The MQS
 * \return AW_OK
 */
aw_err_t aw_imx6ul_mqs_reset (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


