/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief PWM«˝∂Ø∑‰√˘∆˜≈‰÷√–≈œ¢.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-08  hgo, first implementation.
 * \endinternal
 */

#ifndef AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM_H
#define AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM_H

#include "driver/input/touchscreen/awbl_touch_calibrate_nvram.h"

#ifdef AW_DEV_IMX6UL_TOUCH_CALIBRATE_NVRAM


aw_local aw_const struct awbl_touch_calibrate_nvram_devinfo __g_touch_calibrate_nvram_definfo = {
        "/touch_mnt/calib_two.txt",
        "/touch_mnt/calib_five.txt"
};
aw_local struct awbl_dev         awdev;

#define AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM  \
    {                           \
        TOUCH_CALIBRATE_NVRAM_NAME,        \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &awdev,  \
        &__g_touch_calibrate_nvram_definfo \
    },                          \

#else
#define AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM
#endif

#endif /* AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM_H*/
