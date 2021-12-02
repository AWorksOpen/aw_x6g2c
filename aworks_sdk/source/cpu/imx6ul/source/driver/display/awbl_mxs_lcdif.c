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
 * - 1.02 19-05-07  cat, add common-buffer and triple-buffer drivers.
 * - 1.02 16-06-12  sky, add elcdif interface
 * - 1.01 14-08-01  chenshuyi, add operations
 * - 1.00 14-05-29  zen, first implemetation
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "aw_delay.h"

#include "driver/timer/awbl_imx6ul_timer.h"

#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "aw_intensity.h"
#include "aw_fb.h"

#include "driver/display/awbl_mxs_lcdif_regs.h"
#include "driver/display/awbl_mxs_lcdif.h"
#include "driver/display/awbl_mxs_fb.h"


/*******************************************************************************
    local defines
*******************************************************************************/

/* write register */
#define __LCDIF_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __LCDIF_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* 每一个像素点数据字节数 */
#define __LCDIF_BPP_8_BIT        8
#define __LCDIF_BPP_16_BIT       16
#define __LCDIF_BPP_24_BIT       24
#define __LCDIF_BPP_18_BIT       18
#define __LCDIF_BPP_32_BIT       32

/*******************************************************************************
    implementation
*******************************************************************************/

aw_err_t awbl_mxs_lcdif_setup_dotclk_panel (awbl_mxs_lcdif_t     *p_lcdif,
                                            aw_fb_var_info_t     *p_vinfo)
{
    uint32_t val;
    uint32_t bus_val  = 0;
    uint32_t word_val = 0;
    uint16_t v_period;
    uint16_t h_period;
    uint16_t v_wait_cnt;
    uint16_t h_wait_cnt;

    h_period = p_vinfo->hsync.h_pulse_width
              + p_vinfo->hsync.h_bporch
              + p_vinfo->hsync.h_active
              + p_vinfo->hsync.h_fporch;
    v_period = p_vinfo->vsync.v_pulse_width
              + p_vinfo->vsync.v_bporch
              + p_vinfo->vsync.v_active
              + p_vinfo->vsync.v_fporch;
    h_wait_cnt = p_vinfo->hsync.h_pulse_width + p_vinfo->hsync.h_bporch;
    v_wait_cnt = p_vinfo->vsync.v_pulse_width + p_vinfo->vsync.v_bporch;


    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_DATA_SHIFT_DIR, HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_SHIFT_NUM_BITS, HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_BYTE_PACKING_FORMAT, HW_LCDIF_CTRL1_CLR);

    /* 屏幕硬件总线宽度 */
    if (__LCDIF_BPP_16_BIT == p_vinfo->bpp.bus_bpp) {
        bus_val = 0x00;
    } else if (__LCDIF_BPP_8_BIT == p_vinfo->bpp.bus_bpp) {
        bus_val = 0x01;
    } else if (__LCDIF_BPP_18_BIT == p_vinfo->bpp.bus_bpp) {
        bus_val = 0x02;
    } else if (__LCDIF_BPP_24_BIT == p_vinfo->bpp.bus_bpp) {
        bus_val = 0x03;
    }

    /* 一个像素的长度，位为单位*/
    if (__LCDIF_BPP_16_BIT == p_vinfo->bpp.word_lbpp) {
        word_val = 0x00;
    } else if (__LCDIF_BPP_8_BIT == p_vinfo->bpp.word_lbpp) {
        word_val = 0x01;
    } else if (__LCDIF_BPP_18_BIT == p_vinfo->bpp.word_lbpp) {
        word_val = 0x02;
    } else if (__LCDIF_BPP_24_BIT == p_vinfo->bpp.word_lbpp) {
        word_val = 0x03;
    }

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_WORD_LENGTH        |
                                        BM_LCDIF_CTRL_INPUT_DATA_SWIZZLE |
                                        BM_LCDIF_CTRL_LCD_DATABUS_WIDTH  |
                                        BM_LCDIF_CTRL_DATA_FORMAT_16_BIT |
                                        BM_LCDIF_CTRL_DATA_FORMAT_18_BIT |
                                        BM_LCDIF_CTRL_DATA_FORMAT_24_BIT,
                                        HW_LCDIF_CTRL_CLR);

    /**
     *  Word Length : Input data is 16 bit per pixel.
     *  Data Select : Data mode, DCn signal is high.
     */

    if (__LCDIF_BPP_18_BIT == p_vinfo->bpp.word_lbpp) {
        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_CTRL_WORD_LENGTH(word_val)       |  /* 0:16bit 3:24 bit */
                                            BM_LCDIF_CTRL_DATA_SELECT                 |  /* data mode */
                                            BF_LCDIF_CTRL_INPUT_DATA_SWIZZLE(0)       |  /* no swap */
                                            BF_LCDIF_CTRL_LCD_DATABUS_WIDTH(bus_val)  |  /* 0:16bit 3:24 bit */
                                            BM_LCDIF_CTRL_DATA_FORMAT_24_BIT,
                                            HW_LCDIF_CTRL_SET);
    } else {
        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_CTRL_WORD_LENGTH(word_val)       |  /* 0:16bit 3:24 bit */
                                            BM_LCDIF_CTRL_DATA_SELECT                 |  /* data mode */
                                            BF_LCDIF_CTRL_INPUT_DATA_SWIZZLE(0)       |  /* no swap */
                                            BF_LCDIF_CTRL_LCD_DATABUS_WIDTH(bus_val),    /* 0:16bit 3:24 bit */
                                            HW_LCDIF_CTRL_SET);
    }

    if (__LCDIF_BPP_18_BIT == p_vinfo->bpp.word_lbpp) {
        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x7) |//7.....................
                 BM_LCDIF_CTRL1_RECOVER_ON_UNDERFLOW,
                 HW_LCDIF_CTRL1_SET);
    } else {
        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_CTRL1_BYTE_PACKING_FORMAT(0xf) |//f.....................
                 BM_LCDIF_CTRL1_RECOVER_ON_UNDERFLOW,
                 HW_LCDIF_CTRL1_SET);
    }



    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_TRANSFER_COUNT);
    val &= ~(BM_LCDIF_TRANSFER_COUNT_V_COUNT |
             BM_LCDIF_TRANSFER_COUNT_H_COUNT);

    val |= BF_LCDIF_TRANSFER_COUNT_H_COUNT(p_vinfo->hsync.h_active) |   //设置屏幕的长宽像数
           BF_LCDIF_TRANSFER_COUNT_V_COUNT(p_vinfo->vsync.v_active);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_TRANSFER_COUNT);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_VSYNC_MODE,
             HW_LCDIF_CTRL_CLR);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_WAIT_FOR_VSYNC_EDGE,
             HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_DVI_MODE,
             HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_DOTCLK_MODE,    //设置为DOTCLK模式
             HW_LCDIF_CTRL_SET);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_BYPASS_COUNT,
             HW_LCDIF_CTRL_SET);




    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL0);
    val &= ~(BM_LCDIF_VDCTRL0_VSYNC_POL  |
             BM_LCDIF_VDCTRL0_HSYNC_POL  |
             BM_LCDIF_VDCTRL0_ENABLE_POL |
             BM_LCDIF_VDCTRL0_DOTCLK_POL);
    val |= BM_LCDIF_VDCTRL0_ENABLE_POL | BM_LCDIF_VDCTRL0_DOTCLK_POL;
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL0);

    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL0);
    val &= ~(BM_LCDIF_VDCTRL0_VSYNC_OEB);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL0);  /* vsync is output */

    /*
     * need enable sig for true RGB i/f.  Or, if not true RGB, leave it
     * zero.
     */
    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL0);
    val |= BM_LCDIF_VDCTRL0_ENABLE_PRESENT;
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL0);

    /*
     * For DOTCLK mode, count VSYNC_PERIOD in terms of complete hz lines
     */
    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL0);
    val &= ~(BM_LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT |
             BM_LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT);
    val |= BM_LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT |
           BM_LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT;
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL0);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH,
                                        HW_LCDIF_VDCTRL0_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, p_vinfo->vsync.v_pulse_width,
                                        HW_LCDIF_VDCTRL0_SET);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_VDCTRL1_VSYNC_PERIOD(v_period),
                                        HW_LCDIF_VDCTRL1);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BF_LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH(p_vinfo->hsync.h_pulse_width) |
                                        BF_LCDIF_VDCTRL2_HSYNC_PERIOD(h_period),
                                        HW_LCDIF_VDCTRL2);

    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL4);
    val &= ~BM_LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT;
    val |= BF_LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT(p_vinfo->hsync.h_active);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL4);

    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL3);
    val &= ~(BM_LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT |
             BM_LCDIF_VDCTRL3_VERTICAL_WAIT_CNT);
    val |= BF_LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT(h_wait_cnt) |
           BF_LCDIF_VDCTRL3_VERTICAL_WAIT_CNT(v_wait_cnt);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL3);

    val = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_VDCTRL4);
    val |= BM_LCDIF_VDCTRL4_SYNC_SIGNALS_ON;
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, val, HW_LCDIF_VDCTRL4);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_release_dotclk_panel(awbl_mxs_lcdif_t *p_lcdif)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_DOTCLK_MODE,
             HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 0, HW_LCDIF_VDCTRL0);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 0, HW_LCDIF_VDCTRL1);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 0, HW_LCDIF_VDCTRL2);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 0, HW_LCDIF_VDCTRL3);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_init(awbl_mxs_lcdif_t *p_lcdif)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_CLKGATE,
                                        HW_LCDIF_CTRL_CLR);
    /* Reset controller */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_SFTRST,
                                        HW_LCDIF_CTRL_SET);
    aw_udelay(10);

    /* Take controller out of reset */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_SFTRST |
                                        BM_LCDIF_CTRL_CLKGATE,
                                        HW_LCDIF_CTRL_CLR);

    /* Setup the bus protocol */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_MODE86,
                                        HW_LCDIF_CTRL1_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_BUSY_ENABLE,
                                        HW_LCDIF_CTRL1_CLR);

    /* Take display out of reset */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_RESET,
                                        HW_LCDIF_CTRL1_SET);

    /* VSYNC is an input by default */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_VDCTRL0_VSYNC_OEB,
                                        HW_LCDIF_VDCTRL0_SET);

    /* Add 2014.08.12 */
    /* 使能VSYNC_EDGE_IRQ */
//    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_VSYNC_EDGE_IRQ_EN,
//                                        HW_LCDIF_CTRL1_SET);

    /*AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_VSYNC_EDGE_IRQ |
                                         (BV_LCDIF_CTRL1_VSYNC_EDGE_IRQ__REQUEST << 12),
                                          HW_LCDIF_CTRL1_SET);
    */

    /* Reset display */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_RESET,
                                        HW_LCDIF_CTRL1_CLR);
    aw_udelay(10);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_RESET,
                                        HW_LCDIF_CTRL1_SET);
    aw_udelay(10);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_run(awbl_mxs_lcdif_t *p_lcdif)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_LCDIF_MASTER,
                                        HW_LCDIF_CTRL_SET);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_RUN,
                                        HW_LCDIF_CTRL_SET);
    /* 开启CUR_FRAME_DONE中断 */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN,
                                        HW_LCDIF_CTRL1_SET);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_stop(awbl_mxs_lcdif_t *p_lcdif)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_RUN,
                                        HW_LCDIF_CTRL_CLR);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_LCDIF_MASTER,
                                        HW_LCDIF_CTRL_CLR);
    /* 关闭CUR_FRAME_DONE中断 */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL1_CUR_FRAME_DONE_IRQ_EN,
                                        HW_LCDIF_CTRL1_CLR);
    aw_udelay(100);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_dma_init(awbl_mxs_lcdif_t *p_lcdif, uint32_t phys, size_t memsize)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_LCDIF_MASTER,
                                        HW_LCDIF_CTRL_SET);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, phys, HW_LCDIF_CUR_BUF);
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, phys, HW_LCDIF_NEXT_BUF);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_mxs_lcdif_dma_release(awbl_mxs_lcdif_t *p_lcdif)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_LCDIF_MASTER,
                                        HW_LCDIF_CTRL_CLR);

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 使用双缓存或单缓存时显存地址设置函数
 */
aw_err_t awbl_mxs_lcdif_pan_common_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr)
{
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, addr, HW_LCDIF_NEXT_BUF);

    /* 先复位信号量，可能中断先于此函数的到来，导致数据不同步 */
    AW_SEMB_RESET(p_lcdif->lcd_sem_sync, 0);
    AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, AW_SEM_WAIT_FOREVER);

    return AW_OK;
}

/**
 * \brief 使用三缓存时的显存地址设置函数
 */
aw_err_t awbl_mxs_lcdif_pan_triple_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr)
{
    /* 等待NEXT缓冲区可写 */
    AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, AW_SEM_WAIT_FOREVER);

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, addr, HW_LCDIF_NEXT_BUF);

    return AW_OK;
}

/**
 * \brief 尝试设置显存地址函数
 */
aw_err_t awbl_mxs_lcdif_pan_try_vram_addr_set(awbl_mxs_lcdif_t *p_lcdif, uint32_t addr) {
    aw_err_t    ret;

    ret = AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, 0);

    if (AW_OK != ret) {
        return -AW_EAGAIN;
    }

    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, addr, HW_LCDIF_NEXT_BUF);

    return ret;
}

aw_err_t __imx_lcdif_common_buf_isr(void *p_dev)
{
    awbl_mxs_lcdif_t *p_lcdif = \
               (awbl_mxs_lcdif_t *)(&(((awbl_mxs_fb_t *)p_dev)->lcdif));

    /* 清除LCDIF_CTRL1_CUR_FRAME_DONE_IRQ 中断标志位  */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 1 << 9, HW_LCDIF_CTRL1_CLR);

    /* 释放信号量  */
    AW_SEMB_GIVE(p_lcdif->lcd_sem_sync);

    return AW_OK;
}

aw_err_t __imx_lcdif_triple_buf_isr(void *p_dev)
{
    volatile uint32_t reg = 0;
    static uint32_t last_cur_buffer = 0;
    awbl_mxs_lcdif_t *p_lcdif = \
               (awbl_mxs_lcdif_t *)(&(((awbl_mxs_fb_t *)p_dev)->lcdif));

    /* 当前cur-buffer的值 */
    reg = AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_CUR_BUF);

    /* 清除LCDIF_CTRL1_CUR_FRAME_DONE_IRQ 中断标志位  */
    AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, 1 << 9, HW_LCDIF_CTRL1_CLR);

    /* 如果进行了缓冲区交换就释放信号量 */
    if (last_cur_buffer != reg) {

        /* 释放同步信号量  */
        AW_SEMB_GIVE(p_lcdif->lcd_sem_sync);
        /* 保存当前cur-buffer的地址 */
        last_cur_buffer = reg;
    }

    return AW_OK;
}

aw_err_t awbl_mxs_lcdif_pan_blank(awbl_mxs_lcdif_t *p_lcdif, int blank)
{

    int ret = 0, count;

    switch (blank) {

    case AW_FB_BLANK_NORMAL:
    case AW_FB_BLANK_VSYNC_SUSPEND:
    case AW_FB_BLANK_HSYNC_SUSPEND:
    case AW_FB_BLANK_POWERDOWN:

        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_BYPASS_COUNT, HW_LCDIF_CTRL_CLR);

        for (count = 10000; count; count--) {

            if (AWBL_MXS_LCDIF_REG_READ32(p_lcdif, HW_LCDIF_STAT) & BM_LCDIF_STAT_TXFIFO_EMPTY)
                break;

            aw_udelay(1);
        }
        break;

    case AW_FB_BLANK_UNBLANK:
        AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_BYPASS_COUNT, HW_LCDIF_CTRL_SET);

        //AWBL_MXS_LCDIF_REG_WRITE32(p_lcdif, BM_LCDIF_CTRL_RUN, HW_LCDIF_CTRL_SET);

        break;

    default:
        ret = -AW_EINVAL;
        break;
    }

    return ret;
}

/* end of file */
