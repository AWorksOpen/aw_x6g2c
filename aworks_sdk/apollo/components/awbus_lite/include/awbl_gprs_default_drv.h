/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Gprs Ĭ��ģ������ͷ�ļ�
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-12-11  vih, first implementation.
 * \endinternal
 */



#ifndef __AWBL_GPRS_DEFAULT_DRV_H__
#define __AWBL_GPRS_DEFAULT_DRV_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_gprs_default_dev
 * \copydoc awbl_gprs_default_dev.h
 * @{
 */

#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "awbl_gprs.h"
#include "aw_sem.h"
#include "awbus_lite.h"

#define AWBL_GPRS_DEFAULT_NAME   "GPRS_DEFAULT_DEV"

/** \brief �豸���ƿ� */
struct awbl_gprs_default_dev {
    struct awbl_dev                  dev;              /**< \brief device */
    struct awbl_gprs_dev             gprs;
};

void awbl_gprs_default_drv_register (void);


/** @}  grp_aw_if_gprs_default_dev */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_DEFAULT_DRV_H__ */

/* end of file */
