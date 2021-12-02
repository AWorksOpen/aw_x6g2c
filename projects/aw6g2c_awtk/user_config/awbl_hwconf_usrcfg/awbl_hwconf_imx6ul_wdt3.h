/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX6UL_WDT3_H
#define  __AWBL_HWCONF_IMX6UL_WDT3_H

#ifdef  AW_DEV_IMX6UL_WDT3

#include "driver/wdt/awbl_imx6ul_wdt.h"

aw_local void __imx6ul_wdt3_plfm_init (void)
{
    /* enable clock */
    aw_clk_enable(IMX6UL_CLK_WDOG3);
};

/* watchdog feed device infomation */
aw_local aw_const struct awbl_imx6ul_wdt_devinfo __g_imx6ul_wdt3_devinfo = {
    IMX6UL_WDT3_BASE_ADDR,      /* reg base addr */
    1000,                       /* watchdog time (ms)*/
    __imx6ul_wdt3_plfm_init
};

/* allocate wdt device instance RAM */
aw_local struct awbl_imx6ul_wdt_dev __g_imx6ul_wdt3_dev;

#define  AWBL_HWCONF_IMX6UL_WDT3    \
    {                               \
        IMX6UL_WDT_NAME,            \
        3,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_wdt3_dev.dev), \
        &(__g_imx6ul_wdt3_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX6UL_WDT3

#endif  /* AW_DEV_IMX6UL_WDT3 */

#endif  /* __AWBL_HWCONF_IMX6UL_WDT3_H */

/* end of file */
