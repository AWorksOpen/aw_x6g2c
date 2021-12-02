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
 * \brief GM510CEFILG ģ������ͷ�ļ�
 */

#ifndef __AWBL_GPRS_GM510_H__
#define __AWBL_GPRS_GM510_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gprs.h"

#if AWBL_GPRS_USE_LWIP_2_X
#include "netif/ppp/ppp.h"
#else
#include "ppp/ppp.h"
#endif

#define AWBL_GPRS_GM510_NAME   "gosuncn_gm510"

/** \brief GM510�豸���ƽṹ�� */
struct awbl_gprs_gm510_dev {
    struct awbl_dev                       dev;              /**< \brief �����豸*/
    struct awbl_gprs_dev                  gprs;             /**< \brief GPRS�ṹ��*/

    /** \brief ʹ���ڲ�Э��ջʱ��socket����    */
    #define __INTERNAL_SOCKET_NUM         5
    /** \brief ʹ���ڲ�Э��ջʱ��socket��ֵ   */
    #define __INTERNAL_SOCKET_VAL         {1,2,3,4,5}
    uint8_t                               socket[__INTERNAL_SOCKET_NUM];
    uint8_t                               socket_used[__INTERNAL_SOCKET_NUM];
};

void awbl_gprs_gm510_drv_register (void);


/** @}  grp_aw_if_gprs_gm510 */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_GM510_H__ */

