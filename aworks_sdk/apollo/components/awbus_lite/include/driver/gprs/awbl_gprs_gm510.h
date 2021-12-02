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
 * \brief GM510CEFILG 模块驱动头文件
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

/** \brief GM510设备控制结构体 */
struct awbl_gprs_gm510_dev {
    struct awbl_dev                       dev;              /**< \brief 基础设备*/
    struct awbl_gprs_dev                  gprs;             /**< \brief GPRS结构体*/

    /** \brief 使用内部协议栈时的socket数量    */
    #define __INTERNAL_SOCKET_NUM         5
    /** \brief 使用内部协议栈时的socket的值   */
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

