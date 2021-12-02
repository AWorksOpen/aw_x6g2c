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
 * \brief Freescale MXS LCD 控制器驱动
 *
 * \internal
 * \par modification history:
 * - 1.02 16-06-12  sky, add elcdif interface
 * - 1.01 14-08-01  chenshuyi, add operations
 * - 1.00 14-05-29  zen, first implemetation
 * \endinternal
 */

#ifndef __AWBL_MXS_LCDIF_H
#define __AWBL_MXS_LCDIF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "driver/intensity/awbl_pwm_it.h"
#include "driver/display/awbl_mxs_lcdif_regs.h"
#include "aw_fb.h"
#include "aw_clk.h"
#include "aw_sem.h"

#define AWBL_MXS_LCDIF_PANEL_SYSTEM      0
#define AWBL_MXS_LCDIF_PANEL_VSYNC       1
#define AWBL_MXS_LCDIF_PANEL_DOTCLK      2
#define AWBL_MXS_LCDIF_PANEL_DVI         3

#define AWBL_FB_HORIZONTAL_SCREEN        0    /* 横屏 */
#define AWBL_FB_VERTICAL_SCREEN          1    /* 竖屏 */


/** \brief 获取面板设备的名字和单元号 */
typedef aw_err_t (* awbl_mxs_lcdif_obtain_panel_t) (const char **pp_name, int *p_unit);

/** \brief LCDIF设备信息 */
typedef struct awbl_mxs_lcdif_devinfo {

    const char *default_panel_name;      /** \brief 默认面板名 */
    int         default_panel_unit;      /** \brief 默认面板单元号 */

    uint32_t    default_pwm_num;         /** \brief 默认背光控制的PWM号 */

    /** \brief 决定使用哪个面板 (为NULL或失败时，使用默认面板) */
    aw_err_t  (*pfunc_decide_panel) (const char **pp_name, int *p_unit);

    uint32_t    regbase;                    /** \brief LCDIF寄存器基地址 */
    int         inum;                       /** \brief LCD中断号 */
    const aw_clk_id_t  *dev_clks;           /**< \brief the clocks which the timer need */
    int                 dev_clk_count;      /**< \brief count of device clocks */
    aw_clk_id_t         ref_pixel_clk_id;   /**< \brief count of device clocks */

} awbl_mxs_lcdif_devinfo_t;


struct awbl_mxs_panel;

/** \brief LCDIF设备 */
typedef struct awbl_mxs_lcdif {

    awbl_dev_t dev;                         /** \brief 继承自AWBus */
    awbl_mxs_lcdif_devinfo_t *p_devinfo;    /** \brief LCDIF 设备信息 */
    struct awbl_mxs_panel    *p_panel;      /** \brief 使用的面板 */

    AW_SEMB_DECL(lcd_sem_sync);             /** \brief 用于帧同步的信号量 */

} awbl_mxs_lcdif_t;

typedef struct awbl_mxs_dotclk_timing {

    uint16_t    h_pulse_width;  /** \brief 行脉冲宽度 */
    uint16_t    h_active;       /** \brief 一行的像素个数 */
    uint16_t    h_bporch;       /** \brief 水平前沿 */
    uint16_t    h_fporch;       /** \brief 水平后沿 */
    uint16_t    v_pulse_width;  /** \brief 垂直脉冲宽度 */
    uint16_t    v_active;       /** \brief 一帧画面的行数 */
    uint16_t    v_bporch;       /** \brief 垂直前沿 */
    uint16_t    v_fporch;       /** \brief 垂直后沿 */
    int         enable_present;

} awbl_mxs_dotclk_timing_t;

/** \brief 写LCDIF寄存器 */
#define AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, value, reg_off) \
    AW_REG_WRITE32((p_lcdif)->p_devinfo->regbase + (reg_off), value)

/** \brief 读LCDIF寄存器 */
#define AWBL_MXS_LCDIF_REG_READ32(p_lcdif, reg_off) \
    AW_REG_READ32((p_lcdif)->p_devinfo->regbase + (reg_off))

/** \brief panel information */
typedef struct awbl_mxs_panel_info {

    uint16_t x_res;          /** \brief 屏幕宽度 */
    uint16_t y_res;          /** \brief 屏幕高度 */
    uint16_t bus_bpp;        /** \brief rgb硬件总线数 */
    uint16_t word_lbpp;      /** \brief 一个像素的长度，位为单位,如rgb565数据，该值为16 */
    uint32_t dclk_f;         /** \brief 时钟频率 */
    int32_t  lcd_type;       /** \brief 时钟源类型 */
    int32_t  bl_percent;     /** \brief 默认的背光亮度 */
    int32_t  dis_direction;  /** \brief 屏幕方向 */

} awbl_mxs_panel_info_t;


struct awbl_mxs_panel;

/** \brief panel information */
typedef struct awbl_mxs_panel_ops {

    aw_err_t (*init_panel)               (struct awbl_mxs_panel *p_panel, uint32_t phys, size_t memsize);
    void     (*release_panel)            (struct awbl_mxs_panel *p_panel);
    aw_err_t (*blank_panel)              (struct awbl_mxs_panel *p_panel, int blank);
    aw_err_t (*run_panel)                (struct awbl_mxs_panel *p_panel);
    aw_err_t (*stop_panel)               (struct awbl_mxs_panel *p_panel);
    aw_err_t (*pan_triple_vram_addr_set) (struct awbl_mxs_panel *p_panel, uint32_t fb_addr);
    aw_err_t (*pan_common_vram_addr_set) (struct awbl_mxs_panel *p_panel, uint32_t fb_addr);
    aw_err_t (*pan_try_vram_addr_set)    (struct awbl_mxs_panel *p_panel, uint32_t fb_addr);
    aw_err_t (*update_panel)             (struct awbl_mxs_panel *p_panel, void *);
    aw_err_t (*pan_backlight)            (struct awbl_mxs_panel *p_panel, int pwm_num, int percent);

} awbl_mxs_panel_ops_t;

typedef struct awbl_mxs_panel {

    awbl_mxs_lcdif_t            *p_lcdif;
    awbl_mxs_panel_info_t       *p_panel_info;
    const awbl_mxs_panel_ops_t  *p_ops;
    void                        *p_cookie;

} awbl_mxs_panel_t;

aw_err_t awbl_mxs_lcdif_init(awbl_mxs_lcdif_t *p_lcdif);
aw_err_t awbl_mxs_lcdif_run(awbl_mxs_lcdif_t *p_lcdif);
aw_err_t awbl_mxs_lcdif_stop(awbl_mxs_lcdif_t *p_lcdif);
aw_err_t awbl_mxs_lcdif_pan_triple_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_mxs_lcdif_pan_common_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_mxs_lcdif_pan_try_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr);
aw_err_t awbl_mxs_lcdif_pan_blank(awbl_mxs_lcdif_t *p_lcdif, int blank);

aw_err_t awbl_mxs_lcdif_dma_init(awbl_mxs_lcdif_t *p_lcdif, uint32_t phys, size_t memsize);
aw_err_t awbl_mxs_lcdif_dma_release(awbl_mxs_lcdif_t *p_lcdif);

aw_err_t awbl_mxs_lcdif_setup_dotclk_panel (awbl_mxs_lcdif_t   *p_lcdif, aw_fb_var_info_t     *p_vinfo);
aw_err_t awbl_mxs_lcdif_release_dotclk_panel (awbl_mxs_lcdif_t *p_lcdif);

aw_err_t __imx_lcdif_common_buf_isr(void *p_dev);
aw_err_t __imx_lcdif_triple_buf_isr(void *p_dev);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_MXS_LCDIF_H */

/* end of file */

