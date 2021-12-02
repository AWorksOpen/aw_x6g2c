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
    TIMER 0 配置信息
*******************************************************************************/

//#ifdef AW_DEV_IMX28_TIMER_0
#if 1
#include "aw_int.h"
#include "driver/timer/awbl_imx6ul_timer.h"


/* TIMER_0 设备信息 */
const static aw_clk_id_t epit1_dev_clks[1] = {IMX6UL_CLK_EPIT1};
aw_local aw_const struct awbl_imx6ul_timer_param __g_timer0_param = {
    1,                                  /* alloc_byname */
    0x020D0000,                         /* reg_addr */
    INUM_EPIT1_IRQ,                     /* inum */
    IMX6UL_CLK_EPIT1,
    epit1_dev_clks,
    1,
    NULL                                /* pfunc_plfm_init */
};

/* TIMER 0 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_timer __g_timer0_dev;

#define AWBL_HWCONF_IMX6UL_TIMER0   \
    {                               \
       "imx6ul_timer",              \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_timer0_dev.dev,        \
        &__g_timer0_param           \
    },
#else
#define AWBL_HWCONF_IMX28_TIMER0
#endif  /* AW_DEV_LPC17XX_TIMER_0 */

/* end of file */

