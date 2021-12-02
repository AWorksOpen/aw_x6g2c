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
 * \brief ME909S GPRS模块驱动
 *
 * \par 使用示例
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

/** \brief me909s设备控制块 */
struct awbl_gprs_me909s_dev {
    struct awbl_dev                  dev;        /**< \brief device */
    struct awbl_gprs_dev             gprs;

/** \brief 使用内部协议栈时的socket数量    */
#define __INTERNAL_SOCKET_NUM        5            
/** \brief 使用内部协议栈时的socket的值   */  
#define __INTERNAL_SOCKET_VAL        {1,2,3,4,5}    
    uint8_t                          socket[__INTERNAL_SOCKET_NUM];
    uint8_t                          socket_used[__INTERNAL_SOCKET_NUM];
};

struct awbl_gprs_me909s_devinfo {

    /** \brief gprs设备的唯一ID，用于给中间层获取设备 */
    int      id;
    char    *buf;
    size_t   buf_size;

    /** \brief gprs模块的参数 */
    struct awbl_gprs_info       *p_gprs_info;

    /** \brief 短信的数据和信息 */
    struct aw_gprs_sms_buf      *p_sms_info;
};

void awbl_gprs_me909s_drv_register (void);


/** @}  grp_aw_if_gprs_me909s */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_ME909S_H__ */

/* end of file */
