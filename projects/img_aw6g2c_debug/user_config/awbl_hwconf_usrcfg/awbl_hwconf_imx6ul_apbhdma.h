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

#ifndef  __AWBL_HWCONF_IMX6UL_APBHDMA_H
#define  __AWBL_HWCONF_IMX6UL_APBHDMA_H

#ifdef AW_DEV_IMX6UL_APBHDMA

/*******************************************************************************
 DMA控制器配置信息
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/dma/awbl_imx6ul_dma.h"

aw_local void __imx6ul_apbhdma_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_APBHDMA);
}

/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_dma_devinfo __g_imx6ul_dma_devinfo = {
    IMX6UL_APBHDMA_BASE_ADDR,          /* DMA基址 */
    INUM_APBHDMA_IRQ,                  /* 中断号  */
    __imx6ul_apbhdma_plat_init,        /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_dma_dev __g_imx6ul_dma_dev;

#define AWBL_HWCONF_IMX6UL_APBHDMA              \
    {                                           \
        AWBL_IMX6UL_DMA_NAME,                   \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        (struct awbl_dev *)&__g_imx6ul_dma_dev, \
        &__g_imx6ul_dma_devinfo                 \
    },


#else

#define AWBL_HWCONF_IMX6UL_APBHDMA

#endif  /* AW_DEV_IMX6UL_APBHDMA */

#endif /* __AWBL_HWCONF_IMX6UL_APBHDMA_H */

/* end of file */
