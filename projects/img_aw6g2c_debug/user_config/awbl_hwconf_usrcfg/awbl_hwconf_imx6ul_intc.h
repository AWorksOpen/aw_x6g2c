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
  中断控制器配置信息
*******************************************************************************/
#include "driver/intctlr/awbl_imx6ul_intc.h"

/* 设备信息 */
aw_local aw_const awbl_imx6ul_intc_devinfo_t  __g_imx6ul_int_param = {
    {
        0,      /* 支持的起始中断号 */
        159     /* 支持的结束中断号 */
    },
    0x00A00000  /* 寄存器基地址 */
};

/* 设备实例内存静态分配 */
aw_local awbl_imx6ul_intc_dev_t __g_imx6ul_int;

#define AWBL_HWCONF_IMX6UL_INTC     \
    {                               \
        AWBL_IMX6UL_INTC_NAME,      \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_int.super),    \
        &(__g_imx6ul_int_param)     \
    },

/* end of file */

