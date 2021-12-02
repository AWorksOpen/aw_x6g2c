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
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Freescale MXS 帧缓冲设备
 *
 * \internal
 * \par modification history:
 * - 1.01 14-10-17  chenshuyi, modified awbl_mxs_emwin_fb_t, add awbl_mxs_panel_t
 * - 1.00 14-05-29  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_MXS_FB_H
#define __AWBL_MXS_FB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_fb.h"

#include "driver/display/awbl_mxs_lcdif.h"
#include "aw_spinlock.h"

#define AWBL_MXS_FB_NAME      "mxs_fb"

/** \brief 设备信息 */
typedef struct awbl_mxs_fb_devinfo {
    awbl_mxs_lcdif_devinfo_t lcdif_info;
} awbl_mxs_fb_devinfo_t;

/** \brief LCDC设备 */
typedef struct awbl_mxs_fb {

    awbl_dev_t          dev;
    awbl_mxs_lcdif_t    lcdif;
    awbl_fb_info_t      fb_info;
    aw_spinlock_isr_t   lock;

} awbl_mxs_fb_t;

/**
 * \brief driver register
 */
void awbl_mxs_fb_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_MXS_FB_H */

/* end of file */

