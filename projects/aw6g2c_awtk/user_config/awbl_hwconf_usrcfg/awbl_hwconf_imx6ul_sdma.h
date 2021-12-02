/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul SDMA hardware configure
 *
 * \internal
 * \par Modification History
 * - 1.00 16-04-11  lnk, first implementation.
 * \endinternal
 */
#ifndef     __AWBL_HWCONF_IMX6UL_SDMA_H
#define     __AWBL_HWCONF_IMX6UL_SDMA_H

#ifdef AW_DEV_IMX6UL_SDMA
#include "driver/dma/awbl_imx6ul_sdma.h"

extern const unsigned char imx6ul_sdma_fw[];

/* ƽ̨��س�ʼ�� */
aw_local void __imx6ul_sdma_plfm_init (void)
{
    aw_clk_enable(IMX6UL_CLK_SDMA);
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_sdma_devinfo __g_imx6ul_sdma_devinfo = {
    IMX6UL_SDMA_BASE_ADDR,              /* SDMA��ַ */
    INUM_SDMA_IRQ,                      /* SDMA�жϺ�  */
    __imx6ul_sdma_plfm_init             /* ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx6ul_sdma_dev_t __g_imx6ul_sdma_dev = {{0}};

#define AWBL_HWCONF_IMX6UL_SDMA     \
    {                               \
        AWBL_IMX6UL_SDMA_NAME,      \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        (struct awbl_dev *)&__g_imx6ul_sdma_dev,   \
        &__g_imx6ul_sdma_devinfo                   \
    },

#else           // AW_DEV_IMX6UL_SDMA

#define AWBL_HWCONF_IMX6UL_SDMA
#endif          // AW_DEV_IMX6UL_SDMA

#endif          // __AWBL_HWCONF_IMX6UL_SDMA_H
/* end of file */
