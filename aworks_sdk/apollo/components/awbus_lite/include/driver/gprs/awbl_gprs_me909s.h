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
 * \brief ME909S GPRSģ������
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 17-12-09  vih, redefine the architecture
 * - 1.00 16-10-15  vih, first implementation.
 * \endinternal
 */



#ifndef __AWBL_GPRS_ME909S_H__
#define __AWBL_GPRS_ME909S_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_gprs_me909s
 * \copydoc awbl_gprs_me909s.h
 * @{
 */

#include "aworks.h"                 
#include "awbl_gprs.h"
#include "aw_sem.h"
#include "awbus_lite.h"

#define AWBL_GPRS_ME909S_NAME   "HuaWei_me909s"

/** \brief me909s�豸���ƿ� */
struct awbl_gprs_me909s_dev {
    struct awbl_dev                  dev;        /**< \brief device */
    struct awbl_gprs_dev             gprs;

/** \brief ʹ���ڲ�Э��ջʱ��socket����    */
#define __INTERNAL_SOCKET_NUM        5            
/** \brief ʹ���ڲ�Э��ջʱ��socket��ֵ   */  
#define __INTERNAL_SOCKET_VAL        {1,2,3,4,5}    
    uint8_t                          socket[__INTERNAL_SOCKET_NUM];
    uint8_t                          socket_used[__INTERNAL_SOCKET_NUM];
};

struct awbl_gprs_me909s_devinfo {

    /** \brief gprs�豸��ΨһID�����ڸ��м���ȡ�豸 */
    int      id;
    char    *buf;
    size_t   buf_size;

    /** \brief gprsģ��Ĳ��� */
    struct awbl_gprs_info       *p_gprs_info;

    /** \brief ���ŵ����ݺ���Ϣ */
    struct aw_gprs_sms_buf      *p_sms_info;
};

void awbl_gprs_me909s_drv_register (void);


/** @}  grp_aw_if_gprs_me909s */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_ME909S_H__ */

/* end of file */
