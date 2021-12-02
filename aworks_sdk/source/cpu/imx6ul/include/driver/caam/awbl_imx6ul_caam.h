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
 * \brief AWBus-lite iMX6UL CAAM ����
 *
 * iMX6UL CAAM Ϊ iMX6UL ϵ��MCU��Ƭ�ϼ��ܼ�������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "imx6ul_caam"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ:
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
 * \addtogroup grp_awbl_drv_imx6ul_caam IMX6UL CAAM����
 * \copydetails awbl_imx6ul_caam.h
 * @{
 */



/**
 * \defgroup grp_awbl_drv_imx6ul_caam_micro_defs IMX6UL CAAM�����궨��
 * @{
 */

/** \brief imx6ul CAAM ������ */
#define AWBL_IMX6UL_CAAM_NAME   "awbl_imx6ul_caam"

/** @} grp_awbl_drv_imx6ul_caam_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_caam_structs IMX6UL CAAM������صĽṹ
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
 * \addtogroup grp_awbl_drv_imx6ul_caam_awbus_interface Awbus�ӿ�
 * @{
 */
/** \brief �û����ô˺���ע��CAAM���� */
void awbl_imx6ul_caam_drv_register (void);

/** @} grp_awbl_drv_imx6ul_caam_awbus_interface */
/** @} grp_awbl_drv_imx6ul_caam*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
