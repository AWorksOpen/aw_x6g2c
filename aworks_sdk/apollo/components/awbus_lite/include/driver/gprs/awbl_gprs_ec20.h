/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief EC20CEFILG ģ������ͷ�ļ�
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-08  vih, first implementation.
 * \endinternal
 */



#ifndef __AWBL_GPRS_EC20_H__
#define __AWBL_GPRS_EC20_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_gprs_ec20
 * \copydoc awbl_gprs_ec20.h
 * @{
 */

#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "awbl_gprs.h"
#include "aw_sem.h"
#include "awbus_lite.h"

#define AWBL_GPRS_EC20_NAME   "Quectel_EC20"

/** \brief ec20�豸���ƿ� */
struct awbl_gprs_ec20_dev {
    struct awbl_dev                  dev;              /**< \brief device */
    struct awbl_gprs_dev             gprs;

/** \brief ʹ���ڲ�Э��ջʱ��socket����    */
#define __INTERNAL_SOCKET_NUM        5
/** \brief ʹ���ڲ�Э��ջʱ��socket��ֵ   */
#define __INTERNAL_SOCKET_VAL        {1,2,3,4,5}
    uint8_t                          socket[__INTERNAL_SOCKET_NUM];
    uint8_t                          socket_used[__INTERNAL_SOCKET_NUM];
};

void awbl_gprs_ec20_drv_register (void);


/** @}  grp_aw_if_gprs_ec20 */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_EC20_H__ */

/* end of file */
