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
*********************************************************************************/
/**
 * \file
 * \brief AWBus-lite RTC SD3078 驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 20-12-20  hyz, first implementation
 * \endinternal
 */

#ifndef USER_CODE_AWBL_SD3078_H_
#define USER_CODE_AWBL_SD3078_H_


#ifdef __cpl.usplus
extern "C" {
#endif  /* __cplusplus  */

/** @} grp_awbl_drv_SD3078 */

#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief SD3078 驱动名 */
#define AWBL_SD3078_NAME   "sd3078"

/**
 * \brief SD3078 设备信息
 */
typedef struct awbl_sd3078_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief 设备从机地址 */
    uint8_t addr;

    /** \brief 更多自定义配置项 */
    //todo;

} awbl_sd3078_devinfo_t;

/**
 * \brief SD3078 设备实例
 */
typedef struct awbl_sd3078_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);

} awbl_sd3078_dev_t;

/**
 * \brief SD3078 driver register
 */
void awbl_sd3078_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_SD3078_H */

/* end of file */


