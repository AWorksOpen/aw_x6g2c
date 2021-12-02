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
 * \brief IMX6UL SDK Version source
 *
 * \internal
 * \par modification history:
 * - 1.00 16-07-06  sky, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_sdk_version.h"
#include "imx6ul_sdk_version.h"

/*******************************************************************************
  locals
*******************************************************************************/

/* imx6ul sdk version */
const char *p_version = IMX6UL_SDK_VERSION;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief imx6ul get version number
 */
const char *aw_sdk_version_get (void)
{
    return p_version;
}

/* end of file */
