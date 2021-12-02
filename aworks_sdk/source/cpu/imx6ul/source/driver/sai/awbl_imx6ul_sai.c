
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
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief The IMX6UL Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include <string.h>
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_system.h"
#include "aw_clk.h"
#include "driver/sai/awbl_imx6ul_sai.h"
#include "driver/dma/aw_imx6ul_sdma.h"

//#define __SAI1_MQS_ENABLE

/*******************************************************************************
  Global variable
*******************************************************************************/
struct awbl_imx6ul_sai_dev  *__gp_imx6ul_sai_dev[AWBL_IMX6UL_SAI_NUM] = {
    NULL,
    NULL,
    NULL
};

#define CONFIG_TX_DMA_BUF_SIZE      7680*3
#define CONFIG_TX_DMA_NUM_BD        3
#define CONFIG_TX_DMA_BUF_PERIOD    (CONFIG_TX_DMA_BUF_SIZE/CONFIG_TX_DMA_NUM_BD)
#define CONFIG_SAI_TX_WATER_MARK    15

#if CONFIG_TX_DMA_BUF_SIZE%CONFIG_TX_DMA_NUM_BD !=0
#error "CONFIG_TX_DMA_BUF_SIZE%CONFIG_TX_DMA_NUM_BD !=0"
#endif

#if (CONFIG_TX_DMA_BUF_PERIOD)%128 !=0
#error "CONFIG_TX_DMA_BUF_PERIOD%128 !=0"
#endif

/*******************************************************************************
  register define
*******************************************************************************/

struct __imx6ul_sai_regs {

    /* Transmit Control Register */
    uint32_t tcsr;
#define __SAI_TCSR_TE                      (1u << 31)
#define __SAI_TCSR_STOPE                   (1u << 30)
#define __SAI_TCSR_DBGE                    (1u << 29)
#define __SAI_TCSR_BCE                     (1u << 28)
#define __SAI_TCSR_FR                      (1u << 25)
#define __SAI_TCSR_SR                      (1u << 24)
#define __SAI_TCSR_WSF                     (1u << 20)
#define __SAI_TCSR_SEF                     (1u << 19)
#define __SAI_TCSR_FEF                     (1u << 18)
#define __SAI_TCSR_FWF                     (1u << 17)
#define __SAI_TCSR_FRF                     (1u << 16)
#define __SAI_TCSR_WSIE                    (1u << 12)
#define __SAI_TCSR_SEIE                    (1u << 11)
#define __SAI_TCSR_FEIE                    (1u << 10)
#define __SAI_TCSR_FWIE                    (1u << 9)
#define __SAI_TCSR_FRIE                    (1u << 8)
#define __SAI_TCSR_FWDE                    (1u << 1)
#define __SAI_TCSR_FRDE                    (1u << 0)

    /* Transmit Configuration 1 Register */
    uint32_t  tcr1;
#define __SAI_TCR1_TFW(tfw)                    ((tfw) & 0x1F)

    /* Transmit Configuration 2 Register */
    uint32_t  tcr2;
#define __SAI_TCR2_SYNC                        (0x03u << 29)

    /* Transmit Configuration 3 Register */
    uint32_t  tcr3;

    /* Transmit Configuration 4 Register */
    uint32_t  tcr4;

    /* Transmit Configuration 5 Register */
    uint32_t  tcr5;

    uint32_t  reserved0[2];

    /* Transmit Data Register */
    uint32_t  tdr;
    uint32_t  reserved1[7];

    /* Transmit FIFO Register */
    uint32_t  tfr;
    uint32_t  reserved2[7];

    /* Transmit Mask Register */
    uint32_t  tmr;
    uint32_t  reserved3[7];

    /* Transmit Control Register */
    uint32_t  rcsr;

    /* Transmit Configuration 1 Register */
    uint32_t  rcr1;

    /* Transmit Configuration 2 Register */
    uint32_t  rcr2;

    /* Transmit Configuration 3 Register */
    uint32_t  rcr3;

    /* Transmit Configuration 4 Register */
    uint32_t  rcr4;

    /* Transmit Configuration 5 Register */
    uint32_t  rcr5;

    uint32_t  reserved4[2];

    /* Transmit Data Register */
    uint32_t  rdr;
    uint32_t  reserved5[7];

    /* Transmit FIFO Register */
    uint32_t  rfr;
    uint32_t  reserved6[7];

    /* Transmit Mask Register */
    uint32_t  rmr;
    uint32_t  reserved7[7];
};

/*******************************************************************************
  Local defines
*******************************************************************************/

/* declare imx6ul dma infomation */
#define __SAI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_sai_dev *p_this = \
        (struct awbl_imx6ul_sai_dev *)(p_dev)

#define __SAI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx6ul_sai_devinfo *p_devinfo = \
        (struct awbl_imx6ul_sai_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __SAI_REGS_DECL(p_regs, p_devinfo) \
    struct __imx6ul_sai_regs *p_regs = \
        (struct __imx6ul_sai_regs *)(p_devinfo->regbase)

/*******************************************************************************
  locals functions
*******************************************************************************/

aw_local void __sai_reg_write (volatile uint32_t *p_reg,
                               uint32_t           mask,
                               uint32_t           val)
{

    volatile uint32_t tmp;

    tmp = readl(p_reg);

    tmp &= ~mask;

    tmp |= val & mask;

    writel(tmp, p_reg);
}

/******************************************************************************/

aw_local aw_err_t __sai_tx_reset (
        struct awbl_imx6ul_sai_dev *p_this)
{
    volatile uint32_t           tmp;
    struct __imx6ul_sai_regs   *p_regs = p_this->p_regs;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    tmp = readl(&p_regs->tcsr);
    /* Software Reset assert */
    writel((tmp | __SAI_TCSR_SR), &p_regs->tcsr);
    aw_udelay(1000);
    /* Software Reset de-assert */
    writel((tmp & (~__SAI_TCSR_SR)), &p_regs->tcsr);
    /* FIFO Reset */
    writel((tmp | __SAI_TCSR_FR), &p_regs->tcsr);
    aw_udelay(1000);
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return AW_OK;
}

uint32_t        tick1;
void __imx6ul_sai_tx_dma_callback (void *p_arg,unsigned int len)
{
    __SAI_DEV_DECL(p_this, p_arg);

    AW_SEMC_GIVE(p_this->semc_seg_audio_data_played);
}

/* DMA interrupt service routine */
aw_local void __imx6ul_sai_tx_isr (void *p_arg)
{
    __SAI_DEV_DECL(p_this, p_arg);
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);

    aw_int_disable(p_devinfo->inum);
    AW_SEMC_GIVE(p_this->semc_seg_audio_data_played);
}

static int __sai_apend_playing_list(
        struct awbl_imx6ul_sai_dev *p_this,
        sai_buffer_internal_t *p_append_buffer)
{
    sai_buffer_internal_t      *p1,*p2;
    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    // 如果当前播放列表为空，则使用添加的列表为首项
    if (NULL == p_this->p_playing_list ) {
        p_this->p_playing_list = p_append_buffer;
        goto cleanup;
    }
    p1 = p_this->p_playing_list;
    p2 = p1;
    // 查找最后一项
    while (p1 != NULL) {
        p2 = p1;
        p1 = p1->next_playing;
    }
    if (p2 != p_append_buffer) {
        p2->next_playing = p_append_buffer;
    }

cleanup:
    p_append_buffer->next_playing = NULL;
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return 0;
}

static int __sai_tx_fill_dma_buffer(
        struct awbl_imx6ul_sai_dev *p_this,uint8_t *p_buffer,int fill_len)
{
    int                             ret = 0;
    int                             dma_len,len;
    sai_buffer_internal_t          *p_fill_buffer = NULL;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    dma_len = 0;
    p_fill_buffer = p_this->p_fill_list;

    while (NULL !=p_fill_buffer ) {
        len = p_fill_buffer->len - p_fill_buffer->filled_count;
        if (len > fill_len - dma_len ) {
            len = fill_len - dma_len;
        }
        memcpy(p_buffer + dma_len,
                p_fill_buffer->p_buf + p_fill_buffer->filled_count,
                len);
        p_fill_buffer->filled_count += len;
        dma_len += len;
        if (len > 0) {
            // 如果有填充数据，则将缓冲区加入到播放队列中
            __sai_apend_playing_list(p_this,p_fill_buffer);
        }
        // 如果当前缓冲区已经填充完，则从填充队列中移除
        if (p_fill_buffer->filled_count >= p_fill_buffer->len) {
            p_this->p_fill_list = p_fill_buffer->next_fill;
            // 如果没有后续队列，则判断是否需要设置填充未满标志
            if (NULL == p_this->p_fill_list ) {
                if (dma_len < fill_len) {
                    p_fill_buffer->not_fill_enough = 1;
                }
            }

            p_fill_buffer = p_this->p_fill_list;
        }
        if (dma_len >= fill_len) {
            //DMA填充已经完成，退出
            break;
        }
    }
    // 填充失败
    if (0 == dma_len ) {
        ret = -AW_ENOBUFS;
        goto cleanup;
    }

    // 将未填充的数据设置为0
    memset( p_buffer + dma_len,0,fill_len-dma_len);
    ret = 0;
cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

static int fill_audio_data_dma_begin(struct awbl_imx6ul_sai_dev *p_this)
{
    struct dma_slave_config         slave_config = {0};
    int                             ret = 0;
    struct __imx6ul_sai_regs       *p_regs;
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    p_regs = p_this->p_regs;

    //初始化DMA缓冲区
    ret = __sai_tx_fill_dma_buffer(p_this,
            p_this->p_tx_dma_buffer,CONFIG_TX_DMA_BUF_SIZE);
    if (ret) {
        aw_kprintf("%s@%d::err __sai_tx_fill_dma_buffer:%d\n",
                __func__,__LINE__,ret);
        goto cleanup;
    }
    aw_cache_flush(p_this->p_tx_dma_buffer,
            CONFIG_TX_DMA_BUF_SIZE);

    slave_config.direction = DMA_MEM_TO_DEV;
    slave_config.dst_addr = (dma_addr_t)&p_regs->tdr;
    slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES;
    slave_config.dst_maxburst = 32 -CONFIG_SAI_TX_WATER_MARK -2;

    ret = imx6ul_sdma_control(p_this->p_tx_dma_chan,
                DMA_SLAVE_CONFIG,(unsigned long)&slave_config);
    if (ret) {
        aw_kprintf("%s@%d::err imx6ul_sdma_control:%d\n",
                __func__,__LINE__,ret);
        goto cleanup;
    }
    ret = imx6ul_sdma_prep_dma_cyclic(p_this->p_tx_dma_chan,
            (uint32_t)p_this->p_tx_dma_buffer,
            CONFIG_TX_DMA_BUF_SIZE,
            CONFIG_TX_DMA_BUF_PERIOD,
            DMA_MEM_TO_DEV);
    if (ret) {
        aw_kprintf("%s@%d::err imx6ul_sdma_control:%d\n",
                __func__,__LINE__,ret);
        goto cleanup;
    }
    ret = imx6ul_sdma_chan_start_async(
                    p_this->p_tx_dma_chan,__imx6ul_sai_tx_dma_callback,p_this);
    if (ret) {
        aw_kprintf("%s@%d::err imx6ul_sdma_control:%d\n",
                __func__,__LINE__,ret);
        goto cleanup;
    }

    AW_SEMC_RESET( p_this->semc_seg_audio_data_played,0);
    aw_clk_enable(p_devinfo->ref_clk_id);
    /* enable the interrupt */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_FRDE);
    tick1 = aw_sys_tick_get();
    writel(0, &p_regs->tdr);
    writel(0, &p_regs->tdr);

    /* enable the Transmitter */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_TE | __SAI_TCSR_STOPE);

    tick1 = aw_sys_tick_get();
cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

static int __sai_tx_dma_period_play_finish(struct awbl_imx6ul_sai_dev *p_this)
{
    int                     ret = 0;
    sai_buffer_internal_t  *p_playing_buffer;
    int                     played_count = CONFIG_TX_DMA_BUF_PERIOD;
    int                     len;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    while (played_count > 0 && NULL != p_this->p_playing_list) {
        p_playing_buffer = p_this->p_playing_list;
        len = p_playing_buffer->len - p_playing_buffer->played_count;
        if (len > played_count) {
            len = played_count;
        }
        p_playing_buffer->played_count += len;
        played_count -= len;
        if (p_playing_buffer->played_count >= p_playing_buffer->len) {
            //缓冲区已经播放完毕
            p_this->p_playing_list = p_playing_buffer->next_playing;
            if (p_playing_buffer->not_fill_enough) {
                played_count = 0;
            }
            //缓冲区播放完成，调用回调函数
            p_playing_buffer->fn_complete(
                    p_playing_buffer->p_arg1,
                    p_playing_buffer->p_arg2);
            aw_pool_item_return(
                    &p_this->pool_sai_buffer_internal,p_playing_buffer);
        }
    }

    // 如果播放队列为空，则播放完毕
    if (NULL == p_this->p_playing_list) {
        ret = 1;
    }

    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}
static int fill_audio_data_dma_internal(struct awbl_imx6ul_sai_dev *p_this)
{
    int                     ret = 0;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    // 尝试填充数据
    ret = __sai_tx_fill_dma_buffer(
            p_this,
            p_this->p_tx_dma_buffer+p_this->cur_idx*CONFIG_TX_DMA_BUF_PERIOD,
            CONFIG_TX_DMA_BUF_PERIOD);
    if (0 == ret) {
        //有数据填充
    }
    else {
        memset(
                p_this->p_tx_dma_buffer+p_this->cur_idx*CONFIG_TX_DMA_BUF_PERIOD,
                0,
                CONFIG_TX_DMA_BUF_PERIOD);
    }
    aw_cache_flush(p_this->p_tx_dma_buffer+p_this->cur_idx*CONFIG_TX_DMA_BUF_PERIOD,
            CONFIG_TX_DMA_BUF_PERIOD);
    p_this->cur_idx = (p_this->cur_idx+1)%CONFIG_TX_DMA_NUM_BD;

    ret = __sai_tx_dma_period_play_finish(p_this);
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

static int fill_audio_data_dma_stop(struct awbl_imx6ul_sai_dev *p_this)
{
    struct __imx6ul_sai_regs    *p_regs;
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);

    p_regs = p_this->p_regs;
    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    aw_task_delay(1);
    /* disable the interrupt */
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_FRDE | __SAI_TCSR_FWDE );

    /* disable the transmitter and bit clock */
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_TE | __SAI_TCSR_BCE);

    while (readl(&p_regs->tcsr) & __SAI_TCSR_TE);
    while (readl(&p_regs->tcsr) & __SAI_TCSR_BCE);

    /* To restart the Transmitter, should soft-reset */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_SR);
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_SR);

    p_this->busy = AW_FALSE;
    if (NULL == p_this->p_fill_list) {
        AW_SEMB_RESET(p_this->semb_has_valid_audio_data,0);
    }

    AW_MUTEX_UNLOCK(p_this->mutex_dev);

    imx6ul_sdma_chan_stop(p_this->p_tx_dma_chan);
    aw_kprintf("%d\n",aw_sys_tick_get()-tick1);
    aw_clk_disable(p_devinfo->ref_clk_id);
    return 0;
}

static void fill_audio_data_dma(struct awbl_imx6ul_sai_dev *p_this)
{
    int                 ret;

    ret = fill_audio_data_dma_begin(p_this);
    if (0 != ret) {
        goto cleanup;
    }
    while(1) {
        AW_SEMC_TAKE(p_this->semc_seg_audio_data_played,AW_WAIT_FOREVER);
        ret = fill_audio_data_dma_internal(p_this);
        if(0 != ret) {
            break;
        }
    }
    fill_audio_data_dma_stop(p_this);
cleanup:
    return;
}

aw_local aw_err_t __sai_fifo_data_write (struct awbl_imx6ul_sai_dev *p_this)
{
    struct __imx6ul_sai_regs       *p_regs;
    int                             ret = 0;
    sai_buffer_internal_t          *p_fill_buffer = NULL;
    register unsigned int           rd_idx;
    register unsigned int           wr_idx;
    int                             fill_len = 0;
    register uint16_t              *p_buf;
    register int                    filled_count;
    register int                    buf_len;
    register unsigned int           temp;

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    p_regs = p_this->p_regs;

    p_fill_buffer = p_this->p_fill_list;

    while (NULL !=p_fill_buffer ) {
        filled_count = p_fill_buffer->filled_count >> 1;
        buf_len = p_fill_buffer->len >> 1;
        p_buf = (uint16_t *)p_fill_buffer->p_buf;

        while (filled_count < buf_len) {
            temp = AW_REG_READ32(&p_regs->tfr);
            rd_idx = temp & 0x3F;
            wr_idx = (temp >> 16) & 0x3F;

            /* The MSB is not equal, other bits equal, FIFO Full  */
            if (((rd_idx & 0x1F) == (wr_idx & 0x1F)) && (rd_idx != wr_idx)) {
                p_fill_buffer->filled_count = filled_count<<1;
                goto exit_fill;
            }
            /* Write Data */
            AW_REG_WRITE32(&p_regs->tdr, p_buf[filled_count++]);
            fill_len += 2;
        }

        //缓冲区播放完成，调用回调函数
        p_fill_buffer->fn_complete(
                p_fill_buffer->p_arg1,
                p_fill_buffer->p_arg2);
        p_this->p_fill_list = p_this->p_fill_list->next_fill;
        aw_pool_item_return(
                &p_this->pool_sai_buffer_internal,p_fill_buffer);
        p_fill_buffer = p_this->p_fill_list;
    }
exit_fill:
    // 填充失败
    if (0 == fill_len ) {
        ret = -AW_ENOBUFS;
        goto cleanup;
    }
cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

static int fill_audio_data_fifo_begin(struct awbl_imx6ul_sai_dev *p_this)
{
    int                             ret = 0;
    struct __imx6ul_sai_regs       *p_regs;
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    p_regs = p_this->p_regs;

    //初始化填充FIFO
    ret = __sai_fifo_data_write(p_this);
    if (0 != ret) {
        goto cleanup;
    }
    aw_clk_enable(p_devinfo->ref_clk_id);

    AW_SEMC_RESET( p_this->semc_seg_audio_data_played,0);

    /* enable the interrupt */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_FEIE | __SAI_TCSR_FWIE | __SAI_TCSR_FRIE);

    /* enable the Transmitter */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_TE | __SAI_TCSR_STOPE |__SAI_TCSR_DBGE);

    aw_int_enable(p_devinfo->inum);
    tick1 = aw_sys_tick_get();
cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

static int fill_audio_data_fifo_stop(struct awbl_imx6ul_sai_dev *p_this)
{
    struct __imx6ul_sai_regs    *p_regs;
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);

    p_regs = p_this->p_regs;
    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    /* disable the interrupt */
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_FEIE |
                                         __SAI_TCSR_FWIE |
                                         __SAI_TCSR_FRIE);

    /* disable the transmitter and bit clock */
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_TE | __SAI_TCSR_BCE);
    while (readl(&p_regs->tcsr) & __SAI_TCSR_TE);
    while (readl(&p_regs->tcsr) & __SAI_TCSR_BCE);

    /* To restart the Transmitter, should soft-reset */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, __SAI_TCSR_SR);
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, __SAI_TCSR_SR);

    p_this->busy = AW_FALSE;
    if (NULL == p_this->p_fill_list) {
        AW_SEMB_RESET(p_this->semb_has_valid_audio_data,0);
    }
    
    aw_clk_disable(p_devinfo->ref_clk_id);

    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    aw_kprintf("%d\n",aw_sys_tick_get()-tick1);
    return 0;
}

static void fill_audio_data_fifo(struct awbl_imx6ul_sai_dev *p_this)
{
    int                             ret;
    struct __imx6ul_sai_regs       *p_regs;
    __SAI_DEVINFO_DECL(p_devinfo, &p_this->dev);
    uint32_t                        int_status;

    p_regs = p_this->p_regs;
    ret = fill_audio_data_fifo_begin(p_this);
    if (0 != ret) {
        goto cleanup;
    }
    while(1) {
        AW_SEMC_TAKE(p_this->semc_seg_audio_data_played,AW_WAIT_FOREVER);
        int_status = readl(&p_regs->tcsr);
        if (int_status & __SAI_TCSR_FEF) {
            /* mask w1c bits */
            int_status &= ~(__SAI_TCSR_WSF | __SAI_TCSR_SEF | __SAI_TCSR_FEF);
            /* Reset The FIFO */
            int_status |= __SAI_TCSR_FR;
            /* clear the FEF */
            int_status |= __SAI_TCSR_FEF;
            writel(int_status, &p_regs->tcsr);
        }

        if (int_status & (__SAI_TCSR_FRF|__SAI_TCSR_FWF) ) {
            ret = __sai_fifo_data_write(p_this);
            if (0 != ret ) {
                if (int_status & __SAI_TCSR_FWF) {
                    break;
                }
            }
        }
        aw_int_enable(p_devinfo->inum);
    }
    fill_audio_data_fifo_stop(p_this);
cleanup:
    return;
}

static void task_fill_audio_data_fn(void *p_param)
{
    struct awbl_imx6ul_sai_dev         *p_this;

    p_this = (struct awbl_imx6ul_sai_dev *)p_param;

    while (1) {
        AW_SEMB_TAKE(p_this->semb_has_valid_audio_data,
                AW_WAIT_FOREVER);
        
        // now wei have valid audio data
        if (NULL != p_this->p_tx_dma_chan) {
            fill_audio_data_dma(p_this);
        }
        else {
            fill_audio_data_fifo(p_this);
        }
    }
}

/******************************************************************************/


/* config a sai */
aw_err_t  aw_imx6ul_sai_tx_config (int unit_id,
                                awbl_imx6ul_sai_config_t *p_config)
{
    struct awbl_imx6ul_sai_dev *p_this = __gp_imx6ul_sai_dev[unit_id];

    __SAI_DEVINFO_DECL(p_devinfo, p_this);
    __SAI_REGS_DECL(p_regs, p_devinfo);
    uint32_t                        bit_clock;
    uint16_t                        div;
    int                             ret = 0;
    uint32_t                        clk_rate;
    uint32_t                        desired_rate;

    if (p_config->width < 8) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);

    if (p_this->busy) {
        ret = -AW_EBUSY;
        goto cleanup;
    }

    /* Set HW params (Set bclk) */
    desired_rate = 32*2*8*p_config->sample_rate;
    bit_clock = p_config->channels * p_config->width * p_config->sample_rate;
    if (desired_rate % (bit_clock*2) != 0 ) {
        ret = -AW_ENOTSUP;
        goto cleanup;
    }
    // 尝试找到并设置合适的频率
    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
    ret = 0;
    if (clk_rate % desired_rate != 0) {
        ret = 1;
        clk_rate = desired_rate;

        while (clk_rate/bit_clock <=256) {
            aw_clk_rate_set(p_devinfo->ref_clk_id,clk_rate);
            clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
            if (clk_rate % desired_rate == 0) {
                ret = 0;
                break;
            }
            clk_rate += desired_rate;
        }
    }
    if (0 != ret) {
        ret = -AW_ENOTSUP;
        goto cleanup;
    }

    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
    div       = (clk_rate / bit_clock) >> 1;

    if (div) {
        div--;
        div &= 0xFF;
    }

    p_this->busy = AW_TRUE;
    p_this->cur_idx      = 0;

    /* disable the interrupt */
    AW_REG_BIT_CLR_MASK32(&p_regs->tcsr, (__SAI_TCSR_FEIE | __SAI_TCSR_FWIE | __SAI_TCSR_FRIE));

    /* Clear the flag (w1c) */
    AW_REG_BIT_SET_MASK32(&p_regs->tcsr, (__SAI_TCSR_WSF | __SAI_TCSR_SEF | __SAI_TCSR_FEF));

    /* 1. MSEL (p_devinfo->mclk_sel)
     * 2. sample inputs on rising edge
     * 3. Bit clock is generated internally in Master mode
     * 4. DIV = (255 + 1) * 2
     */
    __sai_reg_write(&p_regs->tcr2,
                    (0x03 << 26)                | (1 << 25) | (1 << 24) | 0xFF,
                    (p_devinfo->mclk_sel << 26) | (1 << 25) | (1 << 24) | div);

    /* Transmit FIFO Watermark, Half of the TX FIFO */
    __sai_reg_write(&p_regs->tcr1, 0x1F, CONFIG_SAI_TX_WATER_MARK);

    /* enable transmit data channel */
    __sai_reg_write(&p_regs->tcr3, (1 << 16), (1 << 16));

    /* Set Frame size and Sync Width */
    __sai_reg_write(&p_regs->tcr4,
                    (0x1F << 16) | (0x1F << 8)  |
                    (1 << 4)     | (1 << 3)     | (1 << 1)     | (1 << 0),
                    (1 << 16)    | ((p_config->width - 1) << 8) |
                    (1 << 4)     | (0 << 3)     | (0 << 1)     | (1 << 0));

    /* Set Word N Width and Word 0 Width and First Bit Shifted */
    __sai_reg_write(&p_regs->tcr5,
                    (0x1F << 24) | (0x1F << 16) | (0x1F << 8),
                    ((p_config->width - 1) << 24) |
                    ((p_config->width - 1) << 16) |
                    ((p_config->width - 1) << 8));

    /* Set The MASK, Word 0 and Word1 (left and right not mask)  */
    writel((0xFFFFFFFF - ((1 << 2) - 1)), &p_regs->tmr);

cleanup:
    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    return ret;
}

aw_err_t  aw_imx6ul_sai_tx_append_buffer (int unit_id,
                                        awbl_imx6ul_sai_buffer_t *p_trans)
{
    struct awbl_imx6ul_sai_dev *p_this = __gp_imx6ul_sai_dev[unit_id];
    aw_err_t                    ret = AW_OK;
    sai_buffer_internal_t      *p_buffer,*p;

    // 要求buf按4字节对齐
    if ( (uint32_t)p_trans->p_buf %4 != 0) {
        ret = -AW_EINVAL;
        goto cleanup;
    }

    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);
    if (NULL == p_this->p_fill_list &&
            NULL == p_this->p_playing_list ) {
        // 第一次填充必须填满所有DMA缓冲区
        if (p_trans->len < CONFIG_TX_DMA_BUF_SIZE) {
            aw_kprintf("%s@%d::info %%d bytes needed\n",
                    __func__,__LINE__,CONFIG_TX_DMA_BUF_SIZE);
            ret = -AW_EINVAL;
            goto cleanup;
        }
    }


    p_buffer = aw_pool_item_get(&p_this->pool_sai_buffer_internal);
    if (NULL == p_buffer) {
        ret = -AW_ENOBUFS;
        AW_MUTEX_UNLOCK(p_this->mutex_dev);
        goto cleanup;
    }
    memset(p_buffer,0,sizeof(*p_buffer));
    p_buffer->p_buf = p_trans->p_buf;
    p_buffer->len = p_trans->len;
    p_buffer->fn_complete = p_trans->fn_complete;
    p_buffer->p_arg1 = p_trans->p_arg1;
    p_buffer->p_arg2 = p_trans->p_arg2;
    p = p_this->p_fill_list;
    if (NULL == p) {
        p_this->p_fill_list = p_buffer;
    }
    else {
        while (NULL != p->next_fill) {
            p = p->next_fill;
        }
        p->next_fill = p_buffer;
    }


    AW_MUTEX_UNLOCK(p_this->mutex_dev);
    AW_SEMB_GIVE(p_this->semb_has_valid_audio_data);
cleanup:
    return ret;
}

/******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

int arch_get_debug_state(void);
/******************************************************************************/

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init2 (awbl_dev_t *p_dev)
{
    __SAI_DEV_DECL(p_this, p_dev);
    __SAI_DEVINFO_DECL(p_devinfo, p_dev);

    uint8_t                         unit_id = AWBL_DEV_UNIT_GET(p_dev);
    struct imx_dma_data             data = {0};


    if (unit_id >= AWBL_IMX6UL_SAI_NUM) {
        return;
    }

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }
    p_this->p_regs = (void *)p_devinfo->regbase;
    p_this->busy         = AW_FALSE;
    p_this->cur_idx      = 0;
    p_this->p_tx_dma_chan = NULL;
    p_this->p_tx_dma_buffer = NULL;
    p_this->p_fill_list = NULL;
    p_this->p_playing_list = NULL;

    AW_MUTEX_INIT(p_this->mutex_dev,AW_SEM_Q_FIFO);
    AW_MUTEX_LOCK(p_this->mutex_dev,AW_WAIT_FOREVER);

    aw_pool_init(
            &p_this->pool_sai_buffer_internal,
            p_this->sai_buffer_pool_buffer,
            sizeof(p_this->sai_buffer_pool_buffer),
            sizeof(sai_buffer_internal_t) );
    // First,We reset the sai tx
    __sai_tx_reset(p_this);
    // Second, alloc a dma chan for tx
    if (p_devinfo->use_dma_in_debug ||
            !arch_get_debug_state() ) {
        p_this->p_tx_dma_buffer = aw_mem_align(CONFIG_TX_DMA_BUF_SIZE,128);
        if (NULL == p_this->p_tx_dma_buffer) {
            goto out_tx_dma_init;
        }

        data.peripheral_type = IMX_DMATYPE_SAI;
        data.dma_request = p_devinfo->tx_dma_event;
        data.priority = DMA_PRIO_HIGH;
        p_this->p_tx_dma_chan = imx6ul_sdma_alloc_chan(&data);
        if (NULL == p_this->p_tx_dma_chan ) {
            goto out_tx_dma_init;
        }
    }
out_tx_dma_init:
    if (NULL == p_this->p_tx_dma_chan) {
        if (NULL != p_this->p_tx_dma_buffer) {
            aw_mem_free(p_this->p_tx_dma_buffer);
        }
        aw_kprintf("sai:: we use fifo\n");
    }
    else {
        aw_kprintf("sai:: we use dma transfer\n");
    }
    AW_MUTEX_UNLOCK(p_this->mutex_dev);

    AW_SEMB_INIT(p_this->semb_has_valid_audio_data,
            AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    AW_SEMC_INIT(p_this->semc_seg_audio_data_played,
            0,AW_SEM_Q_FIFO);
    // start the fill audio task
    AW_TASK_INIT(
            p_this->task_fill_audio_data,
            "task_fill_audio_data",
            AW_TASK_SYS_PRIORITY(7),
            1024,
            task_fill_audio_data_fn,
            p_this);
    AW_TASK_STARTUP(p_this->task_fill_audio_data);


    /* connect and enable Transmitter interrupt */
    aw_int_connect(p_devinfo->inum, __imx6ul_sai_tx_isr, (void *)p_dev);
    aw_int_enable(p_devinfo->inum);
    __gp_imx6ul_sai_dev[unit_id] = (struct awbl_imx6ul_sai_dev *)p_dev;

    return;
}

/******************************************************************************/

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sai_drvfuncs = {
    __sai_inst_init1,
    __sai_inst_init2,
    __sai_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_sai_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_SAI_NAME,                   /* p_drvname */
        &__g_sai_drvfuncs,                      /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx6ul_sai_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_sai_drv_registration);
    return;
}

/* end of file */

