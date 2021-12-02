/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX6UL_SAI1_H
#define  __AWBL_HWCONF_IMX6UL_SAI1_H

#ifdef AW_DEV_IMX6UL_SAI1

/*******************************************************************************
 SAI1控制器配置信息
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/sai/awbl_imx6ul_sai.h"
#include "aw_gpio.h"
#include "imx6ul_regs_base.h"

#define __IMX6UL_SAI1_PAD_CTRL             IMX6UL_PAD_CTL(              \
                                           PUS_1_47K_Ohm_Pull_Up      | \
                                           PKE_1_Pull_Keeper_Enabled  | \
                                           PUE_1_Pull                 | \
                                           Open_Drain_Disabled        | \
                                           SPEED_3_max_200MHz         | \
                                           DSE_1_R0                   | \
                                           SRE_0_Fast_Slew_Rate)
aw_local void __imx6ul_sai1_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_SAI1_IPG);
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_sai_devinfo __g_imx6ul_sai1_devinfo = {
     IMX6UL_SAI1_BASE_ADDR,          /* 寄存器基址 */
     INUM_SAI1,                      /* 中断号          */
     IMX6UL_CLK_SAI1,                  /* 时钟id    */
     AW_BL_IMX6UL_SAI_MCLK_SEL_1,    /* 使用MQS时，该值必须为 1 */
     __imx6ul_sai1_plat_init,        /* 平台相关初始化 */
     SDMA_EVENTS_SAI1_TX,
     0,                              /* 调试器中使用DMA，请谨慎使用,容易使硬件挂掉 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_sai_dev __g_imx6ul_sai1_dev;

#define AWBL_HWCONF_IMX6UL_SAI1                     \
    {                                               \
        AWBL_IMX6UL_SAI_NAME,                       \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_sai1_dev,    \
        &__g_imx6ul_sai1_devinfo                    \
    },


#else

#define AWBL_HWCONF_IMX6UL_SAI1

#endif  /* AW_DEV_IMX6UL_SAI1 */

#endif /* __AWBL_HWCONF_IMX6UL_SAI1_H */

/* end of file */
