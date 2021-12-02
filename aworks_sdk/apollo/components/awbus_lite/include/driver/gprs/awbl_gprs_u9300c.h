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
 * \brief U9300C GPRSģ������
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



#ifndef __AWBL_GPRS_U9300C_H__
#define __AWBL_GPRS_U9300C_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_gprs_u9300c
 * \copydoc awbl_gprs_u9300c.h
 * @{
 */

#include "aworks.h"                    
#include "awbl_gprs.h"
#include "aw_sem.h"
#include "awbus_lite.h"

#define AWBL_GPRS_U9300C_NAME   "LongSung_u9300c"

/** \brief U9300C�豸״̬ */
typedef enum u9300c_status {
    /** \brief ģ��������λ */
    AWBL_GPRS_U9300C_RST_ON             = 0x00000001,       
    /** \brief �ڲ�Э��ջ��ppp����  */    
    AWBL_GPRS_U9300C_GPRS_PPP_OK        = 0x00000001<<3,         
    /** \brief gprs��PDP�������  */    
    AWBL_GPRS_U9300C_GPRS_PDP_OK        = 0x00000001<<5,            
    /** \brief SIM������ �����sim���������ź�������ȴ���������ţ���Ƿ�ѣ� */
    AWBL_GPRS_U9300C_SIM_OK             = 0x00000001<<6,            
} awbl_gprs_u9300c_status_e;

struct awbl_gprs_u9300c_devinfo {

    /** \brief gprs�豸��ΨһID�����ڸ��м���ȡ�豸 */
    int      id;
    char    *buf;
    size_t   buf_size;

    /** \brief gprsģ��Ĳ��� */
    struct awbl_gprs_info       *p_gprs_info;

    /** \brief ���ŵ����ݺ���Ϣ */
    struct aw_gprs_sms_buf      *p_sms_info;
};

/** \brief u9300c�豸���ƿ� */
struct awbl_gprs_u9300c_dev {
    struct awbl_dev                  dev;              /**< \brief device */
    struct awbl_gprs_dev             gprs;
    const struct awbl_gprs_u9300c_devinfo *p_devinfo;
    int                              status;           /**< \brief �豸��״̬ */
    void                            *p_pipe;
    struct ip_addr                   ppp_ipaddr;       /**< \brief ��������� IP��ַ */
    AW_SEMB_DECL(                    lock);            /**< \brief �ź��� */

    uint8_t                          net_mode;
    aw_bool_t                           init_flag;
};

void awbl_gprs_u9300c_drv_register (void);


/** @}  grp_aw_if_gprs_u9300c */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_U9300C_H__ */

/* end of file */
