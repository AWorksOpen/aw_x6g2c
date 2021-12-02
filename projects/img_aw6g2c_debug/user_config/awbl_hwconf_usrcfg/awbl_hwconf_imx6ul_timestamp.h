/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_TIMESTAMP_H
#define __AWBL_HWCONF_IMX6UL_TIMESTAMP_H

#ifdef AW_DEV_IMX6UL_TIMESTAMP

#include "apollo.h"
#include "aw_clk.h"
#include "driver/timestamp/awbl_imx6ul_timestamp.h"

/**
 * \brief imx6ul timestamp 设备信息
 */
aw_local struct awbl_imx6ul_timestamp_devinfo __g_imx6ul_timestamp_devinfo = {

    INUM_CPU_PUI,       /**< \brief pmu中断号 */
    IMX6UL_CLK_ARM,     /**< \brief timestamp时钟 */
};

/**
 * \brief imx6ul timestamp 设备实例
 */
aw_local struct awbl_imx6ul_timestamp_dev __g_imx6ul_timestamp;

#define AWBL_HWCONF_IMX6UL_TIMESTAMP     \
    {                                    \
        AWBL_IMX6UL_TIMESTAMP_NAME,      \
        0,                               \
        AWBL_BUSID_PLB,                  \
        0,                               \
        &(__g_imx6ul_timestamp.super),   \
        &(__g_imx6ul_timestamp_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX6UL_TIMESTAMP
#endif /* AW_DEV_IMX6UL_TIMESTAMP */

#endif /* __AWBL_HWCONF_IMX6UL_TIMESTAMP_H */

/* end of line */

