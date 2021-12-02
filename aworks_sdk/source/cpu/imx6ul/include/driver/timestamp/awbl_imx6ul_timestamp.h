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
 * \brief IMX6UL timestamp ����
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

/** \brief timestamp ������ */
#define AWBL_IMX6UL_TIMESTAMP_NAME    "awbl_imx6ul_timestamp"

/** \brief �豸������Ϣ */
typedef struct awbl_imx6ul_timestamp_devinfo {

    const int          inum;           /**< \brief �жϺ� */
    const aw_clk_id_t  clk_id;         /**< \brief ָ��ʱ�� ID �����ָ�� */

} awbl_imx6ul_timestamp_devinfo_t;


/**
 * \brief timestamp �豸ʵ��
 */
typedef struct awbl_imx6ul_timestamp_dev {

    struct awbl_dev super;    /**< \brief �̳��� AWBus �豸 */
    uint32_t        freq;     /**< \brief timestampƵ�� */

} awbl_imx6ul_timestamp_dev_t;

/**
 * \brief �� IMX6UL timestamp ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_imx6ul_timestamp_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif
