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

#ifndef __AWBL_HWCONF_FB_H
#define __AWBL_HWCONF_FB_H

#ifdef AW_DEV_FB

/*******************************************************************************
  HW480272 ������Ϣ
*******************************************************************************/
#include "driver/display/awbl_mxs_fb.h"
#include "imx6ul_pwmid.h"


const static aw_clk_id_t fb_clks[2] = {IMX6UL_CLK_LCDIF_APB,
        IMX6UL_CLK_LCDIF_PIX};
/* �豸��Ϣ */
aw_local aw_const awbl_mxs_fb_devinfo_t  __g_fb_param = {

    /* lcdif_info */
    {
         //"mxs_hw480272f",          /** \brief Ĭ������� */
         "tft_panel",              /** \brief Ĭ������� */
         0,                        /** \brief Ĭ����嵥Ԫ�� */
         PWM5,                     /** \brief Ĭ�ϱ�����Ƶ�PWM�� */
         NULL,                     /** \brief ����ʹ���ĸ���� (ΪNULL��ʧ��ʱ��ʹ��Ĭ�����) */
         IMX6UL_LCDIF_BASE_ADDR,   /** \brief LCDIF�Ĵ�������ַ */
         INUM_LCDIF_IRQ,           /** \brief LCD�жϺ� */
         fb_clks,
         2,
         IMX6UL_CLK_LCDIF_PIX
    }
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_mxs_fb_t __g_fb;

#define AWBL_HWCONF_FB          \
    {                           \
        AWBL_MXS_FB_NAME,       \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &(__g_fb.dev),          \
        &(__g_fb_param)         \
    },

#else
#define AWBL_HWCONF_FB

#endif  /* AW_DEV_FB */

#endif  /* __AWBL_HWCONF_FB_H */

/* end of file */
