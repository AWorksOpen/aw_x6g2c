/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  IMX6UL CAAM 配置信息
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_CAAM_H_
#define AWBL_HWCONF_IMX6UL_CAAM_H_


#include "driver/caam/awbl_imx6ul_caam.h"

#ifdef AW_DEV_IMX6UL_CAAM



/* imx6ul caam 设备实例内存静态分配 */
aw_local awbl_imx6ul_caam_dev_t __g_imx6ul_caam_dev;

#define AWBL_HWCONF_IMX6UL_CAAM     \
    {                               \
        AWBL_IMX6UL_CAAM_NAME,      \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_caam_dev.dev,   \
        NULL                        \
    },
#else
#define AWBL_HWCONF_IMX6UL_CAAM
#endif

#endif /* AWBL_HWCONF_IMX6UL_UART6_H_ */
