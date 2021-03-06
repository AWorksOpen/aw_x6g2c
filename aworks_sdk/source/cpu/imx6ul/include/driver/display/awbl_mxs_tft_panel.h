/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-20126 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief  Freescale MXS HW480272F 面板驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-05-28  zen, first implemetation
 * \endinternal
 * \Remarks  create by CYX at 2019.1.9
 */

#ifndef __AWBL_MXS_TFT_PANEL_H
#define __AWBL_MXS_TFT_PANEL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"

#include "driver/display/awbl_mxs_lcdif.h"

#define AWBL_MXS_TFT_PANEL_NAME            "tft_panel"


typedef struct awbl_mxs_tft_panel_devinfo {

    awbl_mxs_panel_info_t       panel_info;
    awbl_mxs_dotclk_timing_t    timing;
    void (*pfunc_plfm_init)(void);  /**< 平台相关初始化：开启时钟、初始化引脚 */

} awbl_mxs_tft_panel_devinfo_t;

typedef struct awbl_mxs_tft_panel {

    awbl_dev_t         dev;
    awbl_mxs_panel_t   panel;
    awbl_pwm_it_dev_t  pwm_it;

} awbl_mxs_tft_panel_t;

/**
 * \brief driver register
 */
void awbl_mxs_tft_panel_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_MXS_HW480272F_H */

/* end of file */

