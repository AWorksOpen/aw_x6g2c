/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Freescale i.MX6UL Pixel Pipeline driver
 *
 * \internal
 * \par Modification History
 * - 1.00  19-09-04  cat, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_cpu_clk.h"
#include "aw_clk.h"

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"

#include "awbl_imx6ul_pxp.h"
#include "imx6ul_pxp_regs.h"

/** \brief 用于PXP等待处理完成的信号量 */
AW_SEMB_DECL(pxp_sem_sync);

void awbl_pxp_set_as_position (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    reg_val = __PXP_OUT_AS_ULC_Y(y1) | __PXP_OUT_AS_ULC_X(x1);
    writel(reg_val, &p_hw_pxp->OUT_AS_ULC);

    reg_val = __PXP_OUT_AS_LRC_Y(y2) | __PXP_OUT_AS_LRC_X(x2);
    writel(reg_val, &p_hw_pxp->OUT_AS_LRC);
}

void awbl_pxp_set_as_buffer_config (const awbl_pxp_as_buf_cfg_t *p_cfg)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    aw_assert(p_cfg);

    reg_val = readl(&p_hw_pxp->AS_CTRL);
    reg_val &= ~__PXP_AS_CTRL_FORMAT_MASK;  // bit<7-4>

    reg_val |=  __PXP_AS_CTRL_FORMAT(p_cfg->pixel_fmt);

    writel(reg_val,            &p_hw_pxp->AS_CTRL);
    writel(p_cfg->buf_addr,    &p_hw_pxp->AS_BUF);
    writel(p_cfg->pitch_bytes, &p_hw_pxp->AS_PITCH);
}

void awbl_pxp_set_as_blend_config (const awbl_pxp_as_blend_cfg_t *p_cfg)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    aw_assert(p_cfg);

    reg_val  = readl(&p_hw_pxp->AS_CTRL);
    reg_val &= ~(__PXP_AS_CTRL_ALPHA_INVERT_MASK    | // bit<20>
                 __PXP_AS_CTRL_ROP_MASK             | // bit<19-16>
                 __PXP_AS_CTRL_ALPHA_MASK           | // bit<15-8>
                 __PXP_AS_CTRL_ALPHA_CTRL_MASK);      // bit<1>

    reg_val |= (__PXP_AS_CTRL_ROP(p_cfg->rop_mode)  |
                __PXP_AS_CTRL_ALPHA(p_cfg->alpha)   |
                __PXP_AS_CTRL_ALPHA_CTRL(p_cfg->alpha_mode));

    if (p_cfg->invert_alpha) {
        reg_val |= __PXP_AS_CTRL_ALPHA_INVERT_MASK;
    }

    writel(reg_val, &p_hw_pxp->AS_CTRL);
}

void awbl_pxp_set_ps_position (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    reg_val = __PXP_OUT_PS_ULC_Y(y1) | __PXP_OUT_PS_ULC_X(x1);
    writel(reg_val, &p_hw_pxp->OUT_PS_ULC);

    reg_val = __PXP_OUT_PS_LRC_Y(y2) | __PXP_OUT_PS_LRC_X(x2);
    writel(reg_val, &p_hw_pxp->OUT_PS_LRC);
}

void awbl_pxp_set_ps_buffer_config (const awbl_pxp_ps_buf_cfg_t *p_cfg)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    aw_assert(p_cfg);

    reg_val  = readl(&p_hw_pxp->PS_CTRL);
    reg_val &= ~(__PXP_PS_CTRL_FORMAT_MASK  | // bit<4-0>
                 __PXP_PS_CTRL_WB_SWAP_MASK); // bit<5>

    reg_val |=  (__PXP_PS_CTRL_FORMAT(p_cfg->pixel_fmt)  |
                 __PXP_PS_CTRL_WB_SWAP(p_cfg->swap_byte));

    writel(reg_val, &p_hw_pxp->PS_CTRL);

    writel(p_cfg->buf_addr,    &p_hw_pxp->PS_BUF);
    writel(p_cfg->buf_addr_u,  &p_hw_pxp->PS_UBUF);
    writel(p_cfg->buf_addr_v,  &p_hw_pxp->PS_VBUF);
    writel(p_cfg->pitch_bytes, &p_hw_pxp->PS_PITCH);
}

void awbl_pxp_set_ps_background_color (uint32_t bg_color)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    writel(bg_color, &p_hw_pxp->PS_BACKGROUND);
}

aw_local void __pxp_calc_scaler_param (uint16_t input_dimension,
                                       uint16_t output_dimension,
                                       uint8_t  *dec,
                                       uint32_t *scale)
{
    uint32_t scale_fact = 0;
    if (output_dimension == 0) {
        output_dimension = 1;
    }

    scale_fact = ((uint32_t)input_dimension << 12U) / output_dimension;
    if (scale_fact >= (16U << 12U)) {

        /* Desired fact is two large, use the largest support value. */
        *dec = 3U;
        *scale = 0x2000U;
    } else {
        if (scale_fact > (8U << 12U)) {
            *dec = 3U;
        } else if (scale_fact > (4U << 12U)) {
            *dec = 2U;
        } else if (scale_fact > (2U << 12U)) {
            *dec = 1U;
        } else {
            *dec = 0U;
        }

        *scale = scale_fact >> (*dec);
        if (0U == *scale) {
            *scale = 1U;
        }
    }
}

void awbl_pxp_set_ps_scaler (uint16_t input_width,
                             uint16_t input_height,
                             uint16_t output_width,
                             uint16_t output_height)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    uint8_t  dec_x   = 0, dec_y   = 0;
    uint32_t scale_x = 0, scale_y = 0;

    uint32_t reg_val = 0;

    __pxp_calc_scaler_param(input_width,  output_width,  &dec_x, &scale_x);
    __pxp_calc_scaler_param(input_height, output_height, &dec_y, &scale_y);

    /* write pre-decimation filter */
    reg_val  = readl(&p_hw_pxp->PS_CTRL);
    reg_val &= ~(__PXP_PS_CTRL_DECX_MASK    | // bit<11-10>
                 __PXP_PS_CTRL_DECY_MASK);    // bit<9-8>

    reg_val |= __PXP_PS_CTRL_DECX(dec_x)    | // bit<11-10>
               __PXP_PS_CTRL_DECY(dec_y);     // bit<9-8>

    writel(reg_val, &p_hw_pxp->PS_CTRL);

    /* write PS Scale Factor Register  */
    reg_val = __PXP_PS_SCALE_XSCALE(scale_x)|
              __PXP_PS_SCALE_YSCALE(scale_y);

    writel(reg_val, &p_hw_pxp->PS_SCALE);
}

void awbl_pxp_set_rotate_config (awbl_pxp_rotate_position_t position,
                                 awbl_pxp_rotate_degree_t   degree,
                                 awbl_pxp_flip_mode_t       flip_mode)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = readl(&p_hw_pxp->CTRL);

    reg_val &= ~(__PXP_CTRL_ROTATE_MASK     | // bit<9-8>
                 __PXP_CTRL_ROT_POS_MASK    | // bit<22>
                 __PXP_CTRL_VFLIP_MASK      | // bit<11>
                 __PXP_CTRL_HFLIP_MASK);      // bit<10>

    reg_val |= __PXP_CTRL_ROTATE(degree)    |                   // bit<9-8>
               __PXP_CTRL_ROT_POS(position) |                   // bit<22>
               ((uint32_t)flip_mode << __PXP_CTRL_HFLIP_SHIFT); // bit<11-10>

    writel(reg_val, &p_hw_pxp->CTRL);
}

void awbl_pxp_set_ps_color_key (uint32_t color_key_l, uint32_t color_key_h)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    writel(color_key_l, &p_hw_pxp->PS_CLRKEYLOW);
    writel(color_key_l, &p_hw_pxp->PS_CLRKEYHIGH);
}

void awbl_pxp_enable_csc1 (uint8_t enable)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = readl(&p_hw_pxp->CSC1_COEF0);

    /**
     * \note 虽然手册上没有描述这个寄存器的set、clr寄存器，
     *       但是地址是留了的，猜想可以通过地址来实现set、clr寄存器的功能。
     *
     *       后面测试的时候可以试一下
     */
    reg_val = enable ?                                      \
              (reg_val & (~__PXP_CSC1_COEF0_BYPASS_MASK)) : \
              (reg_val |   __PXP_CSC1_COEF0_BYPASS_MASK);

    writel(reg_val, &p_hw_pxp->CSC1_COEF0);
}

void awbl_pxp_enable_csc2 (uint8_t enable)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = readl(&p_hw_pxp->CSC2_CTRL);

    reg_val = enable ?                                      \
              (reg_val & (~__PXP_CSC2_CTRL_BYPASS_MASK)) :  \
              (reg_val |   __PXP_CSC2_CTRL_BYPASS_MASK);

    writel(reg_val, &p_hw_pxp->CSC2_CTRL);
}

void awbl_pxp_set_output_buffer_config (const awbl_pxp_output_buf_cfg_t *p_cfg)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;
    uint32_t           reg_val  = 0;

    aw_assert(p_cfg);

    reg_val = readl(&p_hw_pxp->OUT_CTRL);
    reg_val &= ~(__PXP_OUT_CTRL_FORMAT_MASK              | // bit<4-0>
                 __PXP_OUT_CTRL_INTERLACED_OUTPUT_MASK);   // bit<9-8>
    reg_val |=   __PXP_OUT_CTRL_FORMAT(p_cfg->pixel_fmt) |
                 __PXP_OUT_CTRL_INTERLACED_OUTPUT(p_cfg->output_mode);

    writel(reg_val, &p_hw_pxp->OUT_CTRL);
    writel(p_cfg->buf0_addr,   &p_hw_pxp->OUT_BUF);
    writel(p_cfg->buf1_addr,   &p_hw_pxp->OUT_BUF2);
    writel(p_cfg->pitch_bytes, &p_hw_pxp->OUT_PITCH);

    /* write lower right coordinate */
    reg_val = __PXP_OUT_LRC_Y(p_cfg->height - 1) |
              __PXP_OUT_LRC_X(p_cfg->width - 1);
    writel(reg_val, &p_hw_pxp->OUT_LRC);
}

void awbl_pxp_start (void)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    writel(__PXP_CTRL_ENABLE_MASK | __PXP_CTRL_IRQ_ENABLE_MASK , &p_hw_pxp->CTRL_SET);
}

void awbl_pxp_complete_status_sync (void)
{
    AW_SEMB_TAKE(pxp_sem_sync, AW_SEM_WAIT_FOREVER);
}

void awbl_pxp_hard_reset (void)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    writel(__PXP_CTRL_SFTRST_MASK, &p_hw_pxp->CTRL_SET);
    writel((__PXP_CTRL_SFTRST_MASK | __PXP_CTRL_CLKGATE_MASK), &p_hw_pxp->CTRL_CLR);
}

aw_local void __pxp_isr (void *p_arg)
{
    imx6ul_pxp_regs_t *p_hw_pxp = (imx6ul_pxp_regs_t *)IMX6UL_PXP_BASE_ADDR;

    writel(__PXP_STAT_IRQ_MASK, &p_hw_pxp->STAT_CLR);
    AW_SEMB_GIVE(pxp_sem_sync);
}

void awbl_imx6ul_pxp_init (void)
{
    aw_clk_enable(IMX6UL_CLK_PXP);

    awbl_pxp_hard_reset();

    /* 初始化信号量 */
    AW_SEMB_INIT(pxp_sem_sync, 0, AW_SEM_Q_FIFO);

    /* 连接并使能PXP中断 */
    aw_int_connect(INUM_PXP_IRQ, (aw_pfuncvoid_t)__pxp_isr, NULL);
    aw_int_enable(INUM_PXP_IRQ);
}


/* end of file */
