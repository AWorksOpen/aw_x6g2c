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
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_TEMPMON_H
#define __AWBL_HWCONF_IMX6UL_TEMPMON_H

#ifdef AW_DEV_IMX6UL_TEMPMON

#include "driver/tempmon/awbl_imx6ul_tempmon.h"

aw_local void __imx6ul_tempmon_plfm_init (void)
{
    /* enable clock */
    aw_clk_enable(IMX6UL_CLK_PLL3_USB_OTG);
    //imx6ul_dev_clock_enable(IMX6UL_OCOTP, AW_TRUE);
    //imx6ul_dev_clock_enable(IMX6UL_USB1, AW_TRUE);
};

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_tempmon_devinfo __g_imx6ul_tempmon_devinfo = {
    IMX6UL_TEMPMON_BASE_ADDR,       /* base addr */
    INUM_TEMP_SENSOR_IRQ,           /* inum */
    0,                              /* low limit */
    70000,                          /* high limit  */
    100000,                         /* reset limit(SRC WARM default is enable) */
    5,                              /* task prio */
    __imx6ul_tempmon_plfm_init
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_tempmon_dev __g_imx6ul_tempmon_dev;

#define AWBL_HWCONF_IMX6UL_TEMPMON                  \
    {                                               \
        AWBL_IMX6UL_TEMPMON_NAME,                   \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_tempmon_dev, \
        &(__g_imx6ul_tempmon_devinfo)               \
},

#else
#define AWBL_HWCONF_IMX6UL_TEMPMON

#endif  /* AW_DEV_IMX6UL_TEMPMON */

#endif /* __AWBL_HWCONF_IMX6UL_TEMPMON_H */

/* end of file */
