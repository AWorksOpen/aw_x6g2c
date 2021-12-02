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
 * \brief Freescale MXS LCD ����������
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

#define AWBL_FB_HORIZONTAL_SCREEN        0    /* ���� */
#define AWBL_FB_VERTICAL_SCREEN          1    /* ���� */


/** \brief ��ȡ����豸�����ֺ͵�Ԫ�� */
typedef aw_err_t (* awbl_mxs_lcdif_obtain_panel_t) (const char **pp_name, int *p_unit);

/** \brief LCDIF�豸��Ϣ */
typedef struct awbl_mxs_lcdif_devinfo {

    const char *default_panel_name;      /** \brief Ĭ������� */
    int         default_panel_unit;      /** \brief Ĭ����嵥Ԫ�� */

    uint32_t    default_pwm_num;         /** \brief Ĭ�ϱ�����Ƶ�PWM�� */

    /** \brief ����ʹ���ĸ���� (ΪNULL��ʧ��ʱ��ʹ��Ĭ�����) */
    aw_err_t  (*pfunc_decide_panel) (const char **pp_name, int *p_unit);

    uint32_t    regbase;                    /** \brief LCDIF�Ĵ�������ַ */
    int         inum;                       /** \brief LCD�жϺ� */
    const aw_clk_id_t  *dev_clks;           /**< \brief the clocks which the timer need */
    int                 dev_clk_count;      /**< \brief count of device clocks */
    aw_clk_id_t         ref_pixel_clk_id;   /**< \brief count of device clocks */

} awbl_mxs_lcdif_devinfo_t;


struct awbl_mxs_panel;

/** \brief LCDIF�豸 */
typedef struct awbl_mxs_lcdif {

    awbl_dev_t dev;                         /** \brief �̳���AWBus */
    awbl_mxs_lcdif_devinfo_t *p_devinfo;    /** \brief LCDIF �豸��Ϣ */
    struct awbl_mxs_panel    *p_panel;      /** \brief ʹ�õ���� */

    AW_SEMB_DECL(lcd_sem_sync);             /** \brief ����֡ͬ�����ź��� */

} awbl_mxs_lcdif_t;

typedef struct awbl_mxs_dotclk_timing {

    uint16_t    h_pulse_width;  /** \brief �������� */
    uint16_t    h_active;       /** \brief һ�е����ظ��� */
    uint16_t    h_bporch;       /** \brief ˮƽǰ�� */
    uint16_t    h_fporch;       /** \brief ˮƽ���� */
    uint16_t    v_pulse_width;  /** \brief ��ֱ������ */
    uint16_t    v_active;       /** \brief һ֡��������� */
    uint16_t    v_bporch;       /** \brief ��ֱǰ�� */
    uint16_t    v_fporch;       /** \brief ��ֱ���� */
    int         enable_present;

} awbl_mxs_dotclk_timing_t;

/** \brief дLCDIF�Ĵ��� */
#define AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, value, reg_off) \
    AW_REG_WRITE32((p_lcdif)->p_devinfo->regbase + (reg_off), value)

/** \brief ��LCDIF�Ĵ��� */
#define AWBL_MXS_LCDIF_REG_READ32(p_lcdif, reg_off) \
    AW_REG_READ32((p_lcdif)->p_devinfo->regbase + (reg_off))

/** \brief panel information */
typedef struct awbl_mxs_panel_info {

    uint16_t x_res;          /** \brief ��Ļ��� */
    uint16_t y_res;          /** \brief ��Ļ�߶� */
    uint16_t bus_bpp;        /** \brief rgbӲ�������� */
    uint16_t word_lbpp;      /** \brief һ�����صĳ��ȣ�λΪ��λ,��rgb565���ݣ���ֵΪ16 */
    uint32_t dclk_f;         /** \brief ʱ��Ƶ�� */
    int32_t  lcd_type;       /** \brief ʱ��Դ���� */
    int32_t  bl_percent;     /** \brief Ĭ�ϵı������� */
    int32_t  dis_direction;  /** \brief ��Ļ���� */

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

