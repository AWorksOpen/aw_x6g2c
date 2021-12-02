/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL SDK Version interface
 *
 * \internal
 * \par modification history:
 * - 1.00 16-07-06  sky, first implementation
 * - 1.01 17-07-11  win, modify the interface definition
 * \endinternal
 */

#ifndef __AW_SDK_VERSION_H
#define __AW_SDK_VERSION_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief imx6ul get version number
 * \return sdk version information
 */
const char *aw_sdk_version_get (void);

#ifdef __cplusplus
}
#endif

#endif /* __AW_SDK_VERSION_H */

/* end of file */
