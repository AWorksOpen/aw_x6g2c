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
 * \brief AWBus-lite RTC SD3078 ����
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

/** \brief SD3078 ������ */
#define AWBL_SD3078_NAME   "sd3078"

/**
 * \brief SD3078 �豸��Ϣ
 */
typedef struct awbl_sd3078_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

    /** \brief �����Զ��������� */
    //todo;

} awbl_sd3078_devinfo_t;

/**
 * \brief SD3078 �豸ʵ��
 */
typedef struct awbl_sd3078_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device super;

    /** \brief RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
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


