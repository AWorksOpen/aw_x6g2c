/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief NandBus source file
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-03  tee, first implementation
 * \endinternal
 */

/*******************************************************************************
    include
*******************************************************************************/
#include "apollo.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_clk.h"
#include "driver/busctlr/awbl_imx6ul_gpmi.h"
#include "driver/dma/awbl_imx6ul_dma.h"
//#include "driver/nand/awbl_nandflash.h"
#include "imx6ul_gpmi_regs.h"
#include "string.h"
//#include "awbl_nandflash_ctlr_std_drvs.h"
#include "nand_common.h"
#include "awbus_lite.h"
/*******************************************************************************
    macro configs
*******************************************************************************/
#define CONFIG_GPMI_MAX_DMA_DESC_COUNT      16
#define CONFIG_GPMI_DMA_BUF_SIZE            5120
#define CONGIG_GPMI_CMD_BUF_SIZE            512
#define BUG_HERE                            aw_assert(0)

/*******************************************************************************
    macro operation
*******************************************************************************/
#define __IMX6UL_GPMI_DEVINFO_DECL(p_info, p_awdev) \
        struct awbl_imx6ul_gpmi_devinfo *p_info = \
            (struct awbl_imx6ul_gpmi_devinfo *)AWBL_DEVINFO_GET(p_awdev)

#define __IMX6UL_GPMI_DEV_DECL(p_dev, p_awdev) \
        struct awbl_imx6ul_gpmi *p_dev = AW_CONTAINER_OF(p_awdev, struct awbl_imx6ul_gpmi, nandbus)

/*******************************************************************************
    enums
*******************************************************************************/
/* \brief BCH : Status Block Completion Codes */
enum BCH_COMPLETION_STATUS {
    BCH_COMPLETION_STATUS_GOOD = 0x00,
    BCH_COMPLETION_STATUS_ERASED = 0xFF,
    BCH_COMPLETION_STATUS_UNCORRECTABLE = 0xFE,
};

/**
 * enum dma_ctrl_flags - DMA flags to augment operation preparation,
 *  control completion, and communicate status.
 * @DMA_PREP_INTERRUPT - trigger an interrupt (callback) upon completion of
 *  this transaction
 * @DMA_CTRL_ACK - if clear, the descriptor cannot be reused until the client
 *  acknowledges receipt, i.e. has has a chance to establish any dependency
 *  chains
 * @DMA_PREP_PQ_DISABLE_P - prevent generation of P while generating Q
 * @DMA_PREP_PQ_DISABLE_Q - prevent generation of Q while generating P
 * @DMA_PREP_CONTINUE - indicate to a driver that it is reusing buffers as
 *  sources that were the result of a previous operation, in the case of a PQ
 *  operation it continues the calculation with new sources
 * @DMA_PREP_FENCE - tell the driver that subsequent operations depend
 *  on the result of this operation
 */
enum dma_ctrl_flags {
    DMA_PREP_INTERRUPT = (1 << 0),
    DMA_CTRL_ACK = (1 << 1),
    DMA_PREP_PQ_DISABLE_P = (1 << 2),
    DMA_PREP_PQ_DISABLE_Q = (1 << 3),
    DMA_PREP_CONTINUE = (1 << 4),
    DMA_PREP_FENCE = (1 << 5),
};

/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
enum dma_transfer_direction {
    DMA_MEM_TO_MEM,
    DMA_MEM_TO_DEV,
    DMA_DEV_TO_MEM,
    DMA_DEV_TO_DEV,
    DMA_TRANS_NONE,
};

/*
 * Constants for hardware specific CLE/ALE/NCE function
 *
 * These are bits which can be or'ed to set/clear multiple
 * bits in one go.
 */
enum gpmi_cmd_ctrl_flag {
    NAND_NCE = 0x01,            /* Select the chip by setting nCE to low */
    NAND_CLE = 0x02,            /* Select the command latch by setting CLE to high */
    NAND_ALE = 0x04,            /* Select the address latch by setting ALE to high */
    NAND_CTRL_CLE = (NAND_NCE | NAND_CLE),
    NAND_CTRL_ALE = (NAND_NCE | NAND_ALE),
    NAND_CMD_NONE = -1,
};

/*******************************************************************************
    struct
*******************************************************************************/
struct scatterlist {
    unsigned int    length;
    uint32_t        dma_address;
};

/*******************************************************************************
    function
*******************************************************************************/

static int pointer_is_align(const void *p,int align)
{
    uint32_t        t = (uint32_t )p;
    if( (t%align) == 0) {
        return 1;
    }
    return 0;
}

aw_local aw_err_t __imx6ul_gpmi_set_timing (
        struct awbl_imx6ul_gpmi         *p_dev,
        struct awbl_nand_timing         *p_tim)
{
    __IMX6UL_GPMI_DEVINFO_DECL(p_info, &(p_dev->nandbus.awbus.super));
    struct __imx6ul_gpmi_regs   *p_gpmi_regs = p_dev->p_gpmi_regs;

    uint8_t  tals, tds, tdh;
    uint32_t clk_rate, value, period;
    uint32_t dll_en, half_en;
    uint32_t tas_cycles, tds_cycles, tdh_cycles;
    uint64_t t64;
    clk_rate = aw_clk_rate_get(p_info->clk_io);
    /* gpmi timing0寄存器的tds、tdh同时作用于读写过程，因此在此处取较宽松的参数 */
    tds  = max(p_tim->tds, p_tim->trp);
    tdh  = max(p_tim->tdh, p_tim->trc - p_tim->trp);
    tals = p_tim->tals;
    t64        = (uint64_t)tds * clk_rate;
    tds_cycles = (uint32_t)((t64 + 1000000000 - 1) / 1000000000);
    t64        = (uint64_t)tdh * clk_rate;
    tdh_cycles = (uint32_t)((t64 + 1000000000 - 1) / 1000000000);
    t64        = (uint64_t)tals * clk_rate;
    tas_cycles = (uint32_t)((t64 + 1000000000 - 1) / 1000000000);
    value      = (tds_cycles << 0) | (tdh_cycles << 8) | (tas_cycles << 16);
    writel(value, &p_gpmi_regs->timing0.value);
    value = 0x9000u << 16;
    writel(value, &p_gpmi_regs->timing1.value);
    /* 如果条件满足，则配置为EDO模式 */
    value = (0x0F << 12) | (0x01 << 16) | (0x01 << 17);
    writel(value, &p_gpmi_regs->ctrl1.value_clr);
    if ((p_tim->trea + 4) >= tds) {
        period = 1000000000 / clk_rate;
        if (period > 12) {
            dll_en  = 0;
            half_en = 0;
        } else if ((period > 6) && (period <= 12)) {
            dll_en  = 1;
            half_en = 1;
        } else {
            dll_en  = 1;
            half_en = 0;
        }
        value = p_tim->trea + 4 - tds;
        value = (uint64_t)value * clk_rate * (half_en ? 2 : 1) / 125000000;
        value = ((value & 0x0F) << 12) | (half_en << 16) | (dll_en << 17);
        writel(value, &p_gpmi_regs->ctrl1.value_set);
    }
    return AW_OK;
}



/* The function can reset the GPMI and BCH */
aw_local aw_err_t __imx6ul_gpmi_bch_reset_block (uint32_t reg_base)
{
    uint32_t timeout;
    /* clear RESET */
    writel(__GPMI_BCH_REG_SFTRST, (void *)(reg_base + __GPMI_BCH_REG_CLR_ADDR));
    aw_udelay(1);
    timeout = 10000;
    while ((readl((void *)reg_base) & __GPMI_BCH_REG_SFTRST) && --timeout);

    /* clear CLKGATE */
    writel(__GPMI_BCH_REG_CLKGATE, (void *)(reg_base + __GPMI_BCH_REG_CLR_ADDR));

    /* set RESET*/
    writel(__GPMI_BCH_REG_SFTRST, (void *)(reg_base + __GPMI_BCH_REG_SET_ADDR));
    aw_udelay(1);
    timeout = 10000;
    while ((!(readl((void *)reg_base) & __GPMI_BCH_REG_CLKGATE)) && --timeout);

    /* clear RESET,CLKGATE */
    writel(__GPMI_BCH_REG_SFTRST, (void *)(reg_base + __GPMI_BCH_REG_CLR_ADDR));
    aw_udelay(1);
    timeout = 10000;
    while ((readl((void *)reg_base) & __GPMI_BCH_REG_SFTRST) && --timeout);

    writel(__GPMI_BCH_REG_CLKGATE, (void *)(reg_base + __GPMI_BCH_REG_CLR_ADDR));
    aw_udelay(1);
    timeout = 10000;
    while ((readl((void *)reg_base) & __GPMI_BCH_REG_CLKGATE) && --timeout);

    return AW_OK;
}

/* Configures the geometry for BCH.  */
aw_local int __imx6ul_bch_set_geometry(struct awbl_imx6ul_gpmi  *p_dev,
                                       awbl_nand_info_t         *p_info)
{
    struct bch_geometry        *bch_geo = &p_dev->bch_geo;
    unsigned int                block_count;
    unsigned int                block_size;
    unsigned int                metadata_size;
    unsigned int                ecc_strength;
    unsigned int                page_size;
    unsigned int                gf_len;
    struct __imx6ul_bch_regs   *p_bch_regs = p_dev ->p_bch_regs;

    awbl_imx6ul_bch_geometry_set(p_dev, p_info);

    block_count   = bch_geo->ecc_chunk_count - 1;
    block_size    = bch_geo->ecc_chunk_size;
    metadata_size = bch_geo->metadata_size;
    ecc_strength  = bch_geo->ecc_strength >> 1;
    page_size     = bch_geo->page_total_size;
    gf_len        = bch_geo->gf_len;


    /*
    * Due to erratum #2847 of the MX23, the BCH cannot be soft reset on this
    * chip, otherwise it will lock up. So we skip resetting BCH on the MX23.
    * On the other hand, the MX28 needs the reset, because one case has been
    * seen where the BCH produced ECC errors constantly after 10000
    * consecutive reboots. The latter case has not been seen on the MX23
    * yet, still we don't know if it could happen there as well.
    */
    __imx6ul_gpmi_bch_reset_block((uint32_t)p_bch_regs);

    /* Configure layout 0. */
    writel( __BCH_FLASH0LAYOUT0_NBLOCKS(block_count)
            | __BCH_FLASH0LAYOUT0_META_SIZE(metadata_size)
            | __BCH_FLASH0LAYOUT0_ECC0(ecc_strength)
            | __BCH_FLASH0LAYOUT0_GF(gf_len)
            | __BCH_FLASH0LAYOUT0_DATA0_SIZE(block_size) ,
            &p_bch_regs->flash0layout0.value);

    writel(__BCH_FLASH0LAYOUT1_PAGE_SIZE(page_size)
            | __BCH_FLASH0LAYOUT1_ECCN(ecc_strength)
            | __BCH_FLASH0LAYOUT1_GF(gf_len)
            | __BCH_FLASH0LAYOUT1_DATAN_SIZE(block_size),
             &p_bch_regs->flash0layout1.value);

    /* Set erase threshold to gf/2 for mx6qp and mx7 */

    /* Set *all* chip selects to use layout 0. */
    writel(0, &p_bch_regs->layoutselect.value);

    /* Enable interrupts. */
    writel(__BCH_CTRL_COMPLETE_IRQ_EN, &p_bch_regs->ctrl.value_set);

    return 0;
}

static int __imx6ul_gpmi_dma_prep_slave_sg(struct awbl_imx6ul_gpmi *p_dev,
        struct scatterlist *sgl,
        unsigned int count,
        enum dma_transfer_direction direction,
        enum dma_ctrl_flags flags)
{
    int                     append = flags & DMA_PREP_INTERRUPT;
    int                     idx ;
    awbl_imx6ul_dma_desc_t *ccw;
    int                     j;
    struct scatterlist     *sg = sgl;
    uint32_t               *pio;
    uint8_t                 dma_command;

    idx = append ? p_dev->dma_desc_count : 0;

    if (append) {
        if ( 1 > idx) {
            BUG_HERE;
        }
        ccw = &p_dev->p_dma_desc[idx -1];
        ccw ->next_cmd_desc = (uint32_t)&p_dev->p_dma_desc[idx];
        ccw ->bits |= IMX6UL_DMA_CMD_CHAIN(1);
        ccw ->bits &= ~IMX6UL_DMA_CMD_SEMAPHORE(1);
        ccw ->bits &= ~IMX6UL_DMA_CMD_IRQONCMPLT(1);
    }
    else {
        idx = 0;
    }

    if (idx >= CONFIG_GPMI_MAX_DMA_DESC_COUNT ) {
        BUG_HERE;
    }
    if (direction == DMA_TRANS_NONE) {
        pio = (uint32_t *)sgl;
        ccw = &p_dev->p_dma_desc[idx++];

        memset(ccw,0,sizeof(*ccw) );
        for (j = 0; j < count;) {
            ccw->pio_word[j++] = *pio++;
        }
        ccw ->bits = 0;
        ccw ->bits |= IMX6UL_DMA_CMD_IRQONCMPLT(1);
        ccw ->bits |= IMX6UL_DMA_CMD_SEMAPHORE(1);
        if (flags & DMA_CTRL_ACK) {
            ccw ->bits |= IMX6UL_DMA_CMD_WAIT4ENDCMD(1);
        }
        ccw ->bits |= IMX6UL_DMA_CMD_HALTONTERMINATE(1);
        ccw ->bits |= IMX6UL_DMA_CMD_TERM_FLUSH(1);
        ccw ->bits |= IMX6UL_DMA_CMD_CMDWORDS(count);
        ccw ->bits |= IMX6UL_DMA_CMD_COMMAND(IMX6UL_CMD_COMMAND_NO_DMA_XFER);
    }
    else {
        for (j = 0; j< count;j++) {
            sg = &sgl[j];
            ccw = &p_dev->p_dma_desc[idx++];
            memset(ccw,0,sizeof(*ccw) );

            ccw->next_cmd_desc = (uint32_t)(ccw+1);
            ccw->buf_addr = sg->dma_address;
            ccw->xfer_bytes = sg->length;

            ccw->bits = 0;
            ccw->bits |= IMX6UL_DMA_CMD_CHAIN(1);
            ccw->bits |= IMX6UL_DMA_CMD_HALTONTERMINATE(1);
            ccw->bits |= IMX6UL_DMA_CMD_TERM_FLUSH(1);

            dma_command = direction == DMA_DEV_TO_MEM ?
                    IMX6UL_CMD_COMMAND_DMA_WRITE:IMX6UL_CMD_COMMAND_DMA_READ;
            ccw->bits |= IMX6UL_DMA_CMD_COMMAND(dma_command);

            if (j + 1 == count) {
                ccw->bits &= ~IMX6UL_DMA_CMD_CHAIN(1);
                ccw->bits |= IMX6UL_DMA_CMD_IRQONCMPLT(1);
                ccw->bits |= IMX6UL_DMA_CMD_SEMAPHORE(1);
                if (flags & DMA_CTRL_ACK)
                    ccw->bits |= IMX6UL_DMA_CMD_WAIT4ENDCMD(1);
            }
        }
    }

    p_dev->dma_desc_count = idx;
    return 0;
}


aw_local int __imx6ul_gpmi_send_command(struct awbl_imx6ul_gpmi *p_dev)
{
    __IMX6UL_GPMI_DEVINFO_DECL(p_devinfo, &(p_dev->nandbus.awbus.super));
    struct awbl_imx6ul_nand_info *p_nand_info =
            ((struct awbl_nand_info *)p_dev->p_cur_chip)->p_platform_info;

    struct scatterlist                  sgl;
    int                                 chip = p_nand_info->gpmi_ce_id;
    uint32_t                            pio[3];
    int                                 err;

    aw_assert(p_nand_info);
    p_dev->dma_desc_count = 0;
    /* [1] send out the PIO words */
    pio[0] = __GPMI_SET_CTRL0_CMD_MODE(__GPMI_CTRL0_CMD_WRITE)
        | __GPMI_CTRL0_WORD_LENGTH
        | __GPMI_SET_CTRL0_CS(chip)
        | __GPMI_SET_CTRL0_LOCK_CS(0)
        | __GPMI_SET_CTRL0_ADDRESS(__GPMI_CTRL0_ADDRESS_CMD)
        | __GPMI_CTRL0_ADDRESS_INCREMENT
        | __GPMI_SET_CTRL0_XFER_COUNT(p_dev->command_length);
    pio[1] = pio[2] = 0;
    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                    (struct scatterlist *)pio,
                    3, DMA_TRANS_NONE, 0);

    /* [2] send out the COMMAND + ADDRESS string stored in @buffer */
    sgl.dma_address = (uint32_t)p_dev->cmd_buffer;
    sgl.length = p_dev->command_length;
    aw_cache_flush(p_dev->cmd_buffer,p_dev->command_length);
    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                &sgl, 1, DMA_MEM_TO_DEV,
                DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

    aw_cache_flush(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);
    /* [3] submit the DMA */
    err =  aw_imx6ul_dma_chan_start_sync(p_devinfo->dma_channel,
                                         p_dev->p_dma_desc,
                                         1,
                                         AW_WAIT_FOREVER);
    aw_cache_invalidate(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);
    return err;
}

aw_local int __imx6ul_gpmi_cmd_ctrl(
        struct awbl_imx6ul_gpmi *p_dev,
        uint32_t data, enum gpmi_cmd_ctrl_flag ctrl)
{
    int             ret = 0;
    /*
     * Every operation begins with a command byte and a series of zero or
     * more address bytes. These are distinguished by either the Address
     * Latch Enable (ALE) or Command Latch Enable (CLE) signals being
     * asserted. When MTD is ready to execute the command, it will deassert
     * both latch enables.
     *
     * Rather than run a separate DMA operation for every single byte, we
     * queue them up and run a single DMA operation for the entire series
     * of command and data bytes. NAND_CMD_NONE means the END of the queue.
     */
    if ((ctrl & (NAND_ALE | NAND_CLE))) {
        if (data != NAND_CMD_NONE)
            p_dev->cmd_buffer[p_dev->command_length++] = data;
        return ret;
    }

    if (!p_dev->command_length)
        return ret;

    ret = __imx6ul_gpmi_send_command(p_dev);
    p_dev->command_length = 0;
    return ret;
}

aw_local int __imx6ul_gpmi_read_buf(struct awbl_imx6ul_gpmi *p_dev, uint8_t *buf, int len)
{
    __IMX6UL_GPMI_DEVINFO_DECL(p_devinfo, &(p_dev->nandbus.awbus.super));
    struct awbl_imx6ul_nand_info *p_nand_info =
            ((struct awbl_nand_info *)p_dev->p_cur_chip)->p_platform_info;

    struct scatterlist                  sgl;
    int                                 chip = p_nand_info->gpmi_ce_id;
    uint32_t                            pio[2];
    int                                 err;

    aw_assert(pointer_is_align(buf,4) );
    p_dev->dma_desc_count = 0;
    /* [1] : send PIO */
    pio[0] = __GPMI_SET_CTRL0_CMD_MODE(__GPMI_CTRL0_CMD_READ)
        | __GPMI_CTRL0_WORD_LENGTH
        | __GPMI_SET_CTRL0_CS(chip)
        | __GPMI_SET_CTRL0_LOCK_CS(0)
        | __GPMI_SET_CTRL0_ADDRESS(__GPMI_CTRL0_ADDRESS_DATA)
        | __GPMI_SET_CTRL0_XFER_COUNT(len);
    pio[1] = 0;
    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                    (struct scatterlist *)pio,
                    2, DMA_TRANS_NONE, 0);
    sgl.dma_address = (uint32_t)buf;
    sgl.length = len;

    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                &sgl, 1, DMA_DEV_TO_MEM,
                DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

     /* [3] submit the DMA */
    aw_cache_flush(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);
    err =  aw_imx6ul_dma_chan_start_sync(p_devinfo->dma_channel,
                                         p_dev->p_dma_desc,
                                         1,
                                         AW_WAIT_FOREVER);
    aw_cache_invalidate(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);

    return err;
}

aw_local int __gpmi_gpmi_write_buf(struct awbl_imx6ul_gpmi *p_dev, const uint8_t *buf, int len)
{
    __IMX6UL_GPMI_DEVINFO_DECL(p_devinfo, &(p_dev->nandbus.awbus.super));
    struct awbl_imx6ul_nand_info *p_nand_info =
            ((struct awbl_nand_info *)p_dev->p_cur_chip)->p_platform_info;

    struct scatterlist                  sgl;
    int                                 chip = p_nand_info->gpmi_ce_id;
    uint32_t                            pio[2];
    int                                 err;

    aw_assert(pointer_is_align(buf,4) );
    p_dev->dma_desc_count = 0;
    /* [1] : send PIO */
    pio[0] = __GPMI_SET_CTRL0_CMD_MODE(__GPMI_CTRL0_CMD_WRITE)
        | __GPMI_CTRL0_WORD_LENGTH
        | __GPMI_SET_CTRL0_CS(chip)
        | __GPMI_SET_CTRL0_LOCK_CS(0)
        | __GPMI_SET_CTRL0_ADDRESS(__GPMI_CTRL0_ADDRESS_DATA)
        | __GPMI_SET_CTRL0_XFER_COUNT(len);
    pio[1] = 0;
    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                    (struct scatterlist *)pio,
                    2, DMA_TRANS_NONE, 0);
    sgl.dma_address = (uint32_t)buf;
    sgl.length = len;

    __imx6ul_gpmi_dma_prep_slave_sg(p_dev,
                &sgl, 1, DMA_MEM_TO_DEV,
                DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

     /* [3] submit the DMA */
    aw_cache_flush(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);
    err =  aw_imx6ul_dma_chan_start_sync(p_devinfo->dma_channel,
                                         p_dev->p_dma_desc,
                                         1,
                                         AW_WAIT_FOREVER);
    aw_cache_invalidate(p_dev->p_dma_desc,
            sizeof(awbl_imx6ul_dma_desc_t)*p_dev->dma_desc_count);
    return err;
}


/******************************************************************************/
aw_local void imx6ul_gpmi_reset (struct awbl_nandbus *p_bus);
aw_local aw_err_t __imx6ul_gpmi_hw_init (struct awbl_imx6ul_gpmi *p_dev)
{
    imx6ul_gpmi_reset(&(p_dev->nandbus));

    return AW_OK;
}

aw_local void __imx6ul_bch_irq(void *p_param)
{
    struct awbl_imx6ul_gpmi    *p_dev = (struct awbl_imx6ul_gpmi *)p_param;
    __IMX6UL_GPMI_DEVINFO_DECL(p_info, &(p_dev->nandbus.awbus.super));

    struct __imx6ul_bch_regs   *bch_regs = (struct __imx6ul_bch_regs *)
            p_info->bch_regbase;

    writel(__BCH_CTRL_COMPLETE_IRQ,&bch_regs->ctrl.value_clr);
    AW_SEMB_GIVE(p_dev->sem_bch_finished);
}

/*
 * Handles block mark swapping.
 * It can be called in swapping the block mark, or swapping it back,
 * because the the operations are the same.
 */
static void __imx6ul_bch_block_mark_swapping(struct awbl_imx6ul_gpmi *p_dev,
                void *payload, void *auxiliary)
{
    struct bch_geometry *nfc_geo = &p_dev->bch_geo;
    unsigned char *p;
    unsigned char *a;
    unsigned int  bit;
    unsigned char mask;
    unsigned char from_data;
    unsigned char from_oob;

    /*
     * If control arrives here, we're swapping. Make some convenience
     * variables.
     */
    bit = nfc_geo->block_mark_bit_offset;
    p   = payload + nfc_geo->block_mark_byte_offset;
    a   = auxiliary;

    /*
     * Get the byte from the data area that overlays the block mark. Since
     * the ECC engine applies its own view to the bits in the page, the
     * physical block mark won't (in general) appear on a byte boundary in
     * the data.
     */
    from_data = (p[0] >> bit) | (p[1] << (8 - bit));

    /* Get the byte from the OOB. */
    from_oob = a[0];

    /* Swap them. */
    a[0] = from_data;

    mask = (0x1 << bit) - 1;
    p[0] = (p[0] & mask) | (from_oob << bit);

    mask = ~0 << bit;
    p[1] = (p[1] & mask) | (from_oob >> (8 - bit));
}

static int __imx6ul_bch_encode(
        struct awbl_imx6ul_gpmi *p_dev,
        uint8_t *p_enc_data,
        uint8_t *p_payload,
        uint8_t *p_auxiliary)
{
    struct __imx6ul_bch_regs       *p_bch_regs = p_dev->p_bch_regs;
    int                             ret;

    aw_assert(pointer_is_align(p_enc_data,4) );
    aw_assert(pointer_is_align(p_payload,4) );
    aw_assert(pointer_is_align(p_auxiliary,4) );


    // set the decode address
    writel((uint32_t)p_enc_data, &p_bch_regs->encodeptr.value);
    writel((uint32_t)p_payload, &p_bch_regs->dataptr.value);
    writel((uint32_t)p_auxiliary, &p_bch_regs->metaptr.value);
    //开始解码
    AW_SEMB_RESET(p_dev->sem_bch_finished,0);
    writel(__BCH_CTRL_M2M_LAYOUT,&p_bch_regs->ctrl.value_clr);
    writel(__BCH_CTRL_M2M_ENCODE,&p_bch_regs->ctrl.value_set);
    writel(__BCH_CTRL_M2M_ENABLE,&p_bch_regs->ctrl.value_set);

    ret = AW_SEMB_TAKE(p_dev->sem_bch_finished,1000);
    return ret;
}

static int __imx6ul_bch_decode(
        struct awbl_imx6ul_gpmi *p_dev,
        uint8_t *p_enc_data,
        uint8_t *p_payload,
        uint8_t *p_auxiliary)
{
    struct __imx6ul_bch_regs       *p_bch_regs = p_dev->p_bch_regs;
    int                             ret;

    aw_assert(pointer_is_align(p_enc_data,4) );
    aw_assert(pointer_is_align(p_payload,4) );
    aw_assert(pointer_is_align(p_auxiliary,4) );

    // set the decode address
    writel((uint32_t)p_enc_data, &p_bch_regs->encodeptr.value);
    writel((uint32_t)p_payload, &p_bch_regs->dataptr.value);
    writel((uint32_t)p_auxiliary, &p_bch_regs->metaptr.value);
    //开始解码
    AW_SEMB_RESET(p_dev->sem_bch_finished,0);
    writel(__BCH_CTRL_M2M_LAYOUT,&p_bch_regs->ctrl.value_clr);
    writel(__BCH_CTRL_M2M_ENCODE,&p_bch_regs->ctrl.value_clr);
    writel(__BCH_CTRL_M2M_ENABLE,&p_bch_regs->ctrl.value_set);

    ret = AW_SEMB_TAKE(p_dev->sem_bch_finished,1000);
    return ret;
}

int imx6ul_gpmi_ecc_read_page (
        awbl_nand_platform_serv_t        *p_pf_serv,
        struct awbl_nandbus              *p_bus,
        uint32_t                          page_addr,
        uint8_t                          *p_buf,
        uint32_t                          len,
        uint8_t                          *oob_buf,
        uint32_t                          oob_size,
        aw_bool_t                         oob_required)
{
    __IMX6UL_GPMI_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct bch_geometry            *nfc_geo = &p_dev->bch_geo;
    struct __imx6ul_bch_regs       *bch_regs = p_dev->p_bch_regs;
    uint8_t                        *p_payload;
    uint8_t                        *p_auxiliary;
    unsigned int                    i;
    unsigned char                  *status;
    int                             max_bitflips = 0;
    int                             ret;
    int                             flag = 0;
    uint8_t                        *p_enc_data;
    uint32_t                        used_size;
    int                             user_data_buf_used = 0;

    // 确定各指针的值
    p_enc_data = (uint8_t *)p_dev->p_mux_buf;
    used_size = nfc_geo->page_total_size;
    used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);
    if (pointer_is_align(p_buf,p_dev->cache_line_size) &&
            len >= nfc_geo->payload_size) {
        user_data_buf_used = 1;
        p_payload = p_buf;
    } else {
        p_payload = p_enc_data + used_size;
        used_size += nfc_geo->payload_size;
        used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);
    }
    p_auxiliary = p_enc_data + used_size;
    used_size += nfc_geo->auxiliary_size;
    used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);
    if (used_size > p_dev->mux_buf_len) {
        BUG_HERE;
    }

    //读取页中的原始数据
    aw_cache_flush(p_enc_data,nfc_geo->page_total_size);
    ret = p_pf_serv->read_buf(p_pf_serv,
                              page_addr,
                              0,
                              p_enc_data,
                              nfc_geo->page_total_size,
                              NULL,
                              0,
                              AW_FALSE);

//    ret = __imx6ul_gpmi_read_buf(p_dev,p_enc_data,nfc_geo->page_total_size );
    if ( ret < 0 ) {
        goto cleanup;
    }
    aw_cache_invalidate(p_enc_data,nfc_geo->page_total_size);
    //使用BCH解码
    aw_cache_flush(p_payload,nfc_geo->payload_size);
    aw_cache_flush(p_auxiliary,nfc_geo->auxiliary_size);
    ret = __imx6ul_bch_decode(p_dev,p_enc_data,p_payload,p_auxiliary);
    if ( ret < 0 ) {
        goto cleanup;
    }
    aw_cache_invalidate(p_payload,nfc_geo->payload_size);
    aw_cache_invalidate(p_auxiliary,nfc_geo->auxiliary_size);

    /* handle the block mark swapping */
    __imx6ul_bch_block_mark_swapping(p_dev, p_payload, p_auxiliary);
    // check if we need copy the payload to the user's buffer
    if (!user_data_buf_used) {
        memcpy(p_buf,p_payload,nfc_geo->payload_size );
    }

    /* Loop over status bytes, accumulating ECC status. */
    status = p_auxiliary + nfc_geo->auxiliary_status_offset;
    for (i = 0; i < nfc_geo->ecc_chunk_count; i++, status++) {
        if (*status == BCH_COMPLETION_STATUS_GOOD) {
            continue;
        }

        if (*status == BCH_COMPLETION_STATUS_ERASED) {
            if (readl(&bch_regs->debug1.value)) {
                flag = 1;
            }
            continue;
        }

        if (*status == BCH_COMPLETION_STATUS_UNCORRECTABLE) {
            // 遇到了无法纠正的错误
            max_bitflips = -AW_EBADMSG;
            break;
        }

    }

    if (oob_required) {
        memcpy(oob_buf,p_enc_data+nfc_geo->payload_size,oob_size);
    }
    if (flag) {
        memset(p_buf, 0xff, len);
    }
    ret = max_bitflips;
cleanup:
    return ret;
}

int imx6ul_gpmi_ecc_write_page (
        awbl_nand_platform_serv_t        *p_pf_serv,
        struct awbl_nandbus              *p_bus,
        uint32_t                          page_addr,
        uint8_t                          *p_buf,
        uint32_t                          len,
        uint8_t                          *oob_buf,
        uint32_t                          oob_size,
        aw_bool_t                            oob_required)
{
    __IMX6UL_GPMI_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct bch_geometry            *nfc_geo = &p_dev->bch_geo;
    uint8_t                        *p_payload;
    uint8_t                        *p_auxiliary;
    int                             ret;
    uint8_t                        *p_enc_data;
    uint32_t                        used_size;

    // 确定各指针的值
    p_enc_data = (uint8_t *)p_dev->p_mux_buf;
    used_size = nfc_geo->page_total_size;
    used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);
    p_payload = p_enc_data + used_size;
    used_size += nfc_geo->payload_size;
    used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);
    p_auxiliary = p_enc_data + used_size;
    used_size += nfc_geo->auxiliary_size;
    used_size = AW_ROUND_UP(used_size,p_dev->cache_line_size);

    if (used_size > p_dev->mux_buf_len) {
        BUG_HERE;
    }

    //复制pay_load
    memcpy(p_payload,p_buf,len);
    //设置坏块标记
    if ( 0 != p_pf_serv->p_info->attr.bad_blk_byte_offs) {
        BUG_HERE;
    }
    p_auxiliary[0] = 0xFF;

    /* Handle block mark swapping. */
    __imx6ul_bch_block_mark_swapping(p_dev,
            p_payload, p_auxiliary);
    aw_cache_flush(p_payload,nfc_geo->payload_size);
    aw_cache_flush(p_auxiliary,nfc_geo->auxiliary_size);

    // 调用BCH进行编码
    ret = __imx6ul_bch_encode(p_dev,p_enc_data,p_payload,p_auxiliary);
    if ( ret < 0 ) {
        goto cleanup;
    }
    aw_cache_invalidate(p_enc_data,nfc_geo->page_total_size);

    if (oob_required) {
        // 只复制OOB中最后的未用到的字节
        memcpy(p_enc_data + nfc_geo->page_total_size - nfc_geo->oob_free_len,
                oob_buf + oob_size - nfc_geo->oob_free_len,
                nfc_geo->oob_free_len);
    } else {
        memset(p_enc_data + nfc_geo->page_total_size - nfc_geo->oob_free_len,
                0xFF,nfc_geo->oob_free_len);
    }
    //写入数据到nand flash
    aw_cache_flush(p_enc_data,nfc_geo->page_total_size);
//    ret = __gpmi_gpmi_write_buf(p_dev, p_enc_data, nfc_geo->page_total_size);

    ret = p_pf_serv->write_buf(p_pf_serv,
                               page_addr,
                               0,
                               p_enc_data,
                               nfc_geo->page_total_size,
                               NULL,
                               0,
                               AW_FALSE);
cleanup:
    return ret;
}


aw_local void imx6ul_gpmi_select (struct awbl_nandbus         *p_bus,
                                  void                        *p_arg)
{
    __IMX6UL_GPMI_DEV_DECL(          p_dev, &(p_bus->awbus.super));
    struct awbl_imx6ul_nand_info    *p_imx6ul_info = NULL;
    awbl_nand_info_t                *p_info = p_arg;

    aw_assert(p_bus && p_info);
    if (p_dev->p_cur_chip == p_info) {
        return;
    }
    p_dev->p_cur_chip = p_info;
    p_imx6ul_info = p_info->p_platform_info;
    /* 设置时序  */
    __imx6ul_gpmi_set_timing(p_dev, &(p_imx6ul_info->tim));
    __imx6ul_bch_set_geometry(p_dev, p_info);
}

/******************************************************************************/
aw_local aw_err_t imx6ul_gpmi_opt (struct awbl_nandbus *p_bus,
                                   struct awbl_nandbus_opt        *p_opt)
{
    __IMX6UL_GPMI_DEV_DECL(p_dev, &(p_bus->awbus.super));

    uint8_t    *ptr = (uint8_t *)p_opt->ptr;
    int         i;
    int         flag;
    uint8_t    *p_buf;
    int         ret = -AW_EINVAL;

    /* operation */
    switch (p_opt->type) {
    case NAND_CMD:
        for (i = 0;i<p_opt->len;i++) {
            ret = __imx6ul_gpmi_cmd_ctrl(p_dev,ptr[i],NAND_CTRL_CLE);
            if (ret) {
                goto cleanup;
            }
        }
        flag = 1;
        for (i = 0; i< sizeof(__g_nand_flash_cmd_addr)/sizeof(__g_nand_flash_cmd_addr[0]);
                i++) {
            if (ptr[p_opt->len-1] == __g_nand_flash_cmd_addr[i] ) {
                flag = 0;
                break;
            }
        }
        if (flag) {
            ret = __imx6ul_gpmi_cmd_ctrl(p_dev,NAND_CMD_NONE,NAND_NCE);
            if (ret) {
                goto cleanup;
            }
        }
        break;
    case NAND_ADDR:
        for(i = 0;i<p_opt->len;i++) {
            ret = __imx6ul_gpmi_cmd_ctrl(p_dev,ptr[i],NAND_CTRL_ALE);
            if (ret) {
                goto cleanup;
            }
        }
        ret = __imx6ul_gpmi_cmd_ctrl(p_dev,NAND_CMD_NONE,NAND_NCE);
        if (ret) {
            goto cleanup;
        }
        break;
    case NAND_RD_DATA:
        if (pointer_is_align(ptr,p_dev->cache_line_size) ) {
            p_buf = ptr;
        }
        else {
            p_buf = p_dev->p_mux_buf;
        }
        aw_cache_flush(p_buf,p_opt->len);
        ret = __imx6ul_gpmi_read_buf(p_dev,p_buf,p_opt->len);
        if (ret) {
            goto cleanup;
        }
        aw_cache_invalidate(p_buf,p_opt->len);
        if (!pointer_is_align(ptr,p_dev->cache_line_size) ) {
            memcpy(ptr,p_buf,p_opt->len);
        }
        break;
    case NAND_WR_DATA:
        if (pointer_is_align(ptr,p_dev->cache_line_size) ) {
            p_buf = ptr;
        }
        else {
            p_buf = p_dev->p_mux_buf;
            memcpy(p_buf,ptr,p_opt->len);
        }
        aw_cache_flush(p_buf,p_opt->len);
        ret = __gpmi_gpmi_write_buf(p_dev,p_buf,p_opt->len);
        if (ret) {
            goto cleanup;
        }
        break;
    default:
        ret = -AW_EINVAL;
        break;
    }
    ret = 0;
cleanup:
    return ret;
}

/******************************************************************************/
aw_local aw_bool_t imx6ul_gpmi_ready (struct awbl_nandbus *p_bus,
                                      void                *priv)
{
    __IMX6UL_GPMI_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct __imx6ul_gpmi_regs *p_gpmi_regs = p_dev->p_gpmi_regs;
    struct awbl_imx6ul_nand_info *p_nand_info =
            ((struct awbl_nand_info *)p_dev->p_cur_chip)->p_platform_info;
    int                                 chip = p_nand_info->gpmi_ce_id;

    aw_assert(priv == NULL);
    return (readl(&p_gpmi_regs->status.value) &
           (1u << (((uint32_t)(chip) & 0xFF) + 24)))
            ? AW_TRUE : AW_FALSE;
}


/******************************************************************************/
aw_local void imx6ul_gpmi_reset (struct awbl_nandbus *p_bus)
{
    __IMX6UL_GPMI_DEV_DECL(p_dev, &(p_bus->awbus.super));
    struct __imx6ul_gpmi_regs *p_gpmi_regs = p_dev->p_gpmi_regs;

    /* reset registers. */
    __imx6ul_gpmi_bch_reset_block((uint32_t) p_gpmi_regs);

    /* set NAND mode */
    writel(__GPMI_CTRL1_GPMI_MODE,&p_gpmi_regs->ctrl1.value_clr);

    /* Set The Mode to : Asynchronous mode */
    writel(__GPMI_CTRL1_SSYNCMODE,&p_gpmi_regs->ctrl1.value_clr);

    writel(__GPMI_CTRL1_WRN_DLY_SEL,&p_gpmi_regs->ctrl1.value_clr);

    /* set ready interrupt polarity */
    writel(__GPMI_CTRL1_ATA_IRQRDY_POLARITY,&p_gpmi_regs->ctrl1.value_set);

    /* disable write protect */
    writel(__GPMI_CTRL1_DEV_RESET,&p_gpmi_regs->ctrl1.value_set);


    /* allow DMA access all flash chip */
    writel(__GPMI_CTRL1_DECOUPLE_CS,&p_gpmi_regs->ctrl1.value_set);
}

/******************************************************************************/

void awbl_imx6ul_bch_geometry_set (struct awbl_imx6ul_gpmi *p_dev,
                                   struct awbl_nand_info   *p_info)
{
    struct bch_geometry    *geo = &p_dev->bch_geo;
    unsigned int            metadata_size;
    unsigned int            status_size;
    unsigned int            block_mark_bit_offset;
    int                     oob_free_len;
    int                     i;

    /*
     * The size of the metadata can be changed, though we set it to 10
     * bytes now. But it can't be too large, because we have to save
     * enough space for BCH.
     */
    geo->metadata_size = 1;

    /* The default for the length of Galois Field. */
    geo->gf_len = 13;

    /* The default for chunk size. */
    geo->ecc_chunk_size = 512;
    while (geo->ecc_chunk_size < p_info->attr.spare_size ) {
        geo->ecc_chunk_size *= 2; /* keep C >= O */
        geo->gf_len = 14;
    }

    geo->ecc_chunk_count = p_info->attr.page_size / geo->ecc_chunk_size;

    /* We use the same ECC strength for all chunks. */
    geo->ecc_strength = p_info->ecc_strength;

    geo->page_total_size = p_info->attr.page_size + p_info->attr.spare_size;
    geo->payload_size = p_info->attr.page_size;

    /* 计算oob可用字节 */
    {
        /* check if the oob space is enough */
        oob_free_len = geo->page_total_size - geo->payload_size -
                 (geo->ecc_chunk_count * geo->gf_len * geo->ecc_strength +7)/8
                        -1 ;
        if (oob_free_len <= 0) {
            BUG_HERE;
        }

        geo->oob_free_len = oob_free_len;
        oob_free_len = 0;
        for (i = 0; i < p_info->p_nand_ooblayout->cnt_freepos; i++) {
            oob_free_len += p_info->p_nand_ooblayout->ptr_freepos[i].length;
        }
        aw_assert(geo->oob_free_len >= oob_free_len);
        if (oob_free_len < geo->oob_free_len) {
            geo->oob_free_len = oob_free_len;
        }
    }
    /*
     * The auxiliary buffer contains the metadata and the ECC status. The
     * metadata is padded to the nearest 32-bit boundary. The ECC status
     * contains one byte for every ECC chunk, and is also padded to the
     * nearest 32-bit boundary.
     */
    metadata_size = AW_ROUND_UP(geo->metadata_size, 4);
    status_size   = AW_ROUND_UP(geo->ecc_chunk_count, 4);

    geo->auxiliary_size = metadata_size + status_size;
    geo->auxiliary_status_offset = metadata_size;

    /*
     * We need to compute the byte and bit offsets of
     * the physical block mark within the ECC-based view of the page.
     *
     * NAND chip with 2K page shows below:
     *                                             (Block Mark)
     *                                                   |      |
     *                                                   |  D   |
     *                                                   |<---->|
     *                                                   V      V
     *    +---+----------+-+----------+-+----------+-+----------+-+
     *    | M |   data   |E|   data   |E|   data   |E|   data   |E|
     *    +---+----------+-+----------+-+----------+-+----------+-+
     *
     * The position of block mark moves forward in the ECC-based view
     * of page, and the delta is:
     *
     *                   E * G * (N - 1)
     *             D = (---------------- + M)
     *                          8
     *
     * With the formula to compute the ECC strength, and the condition
     *       : C >= O         (C is the ecc chunk size)
     *
     * It's easy to deduce to the following result:
     *
     *         E * G       (O - M)      C - M         C - M
     *      ----------- <= ------- <=  --------  <  ---------
     *           8            N           N          (N - 1)
     *
     *  So, we get:
     *
     *                   E * G * (N - 1)
     *             D = (---------------- + M) < C
     *                          8
     *
     *  The above inequality means the position of block mark
     *  within the ECC-based view of the page is still in the data chunk,
     *  and it's NOT in the ECC bits of the chunk.
     *
     *  Use the following to compute the bit position of the
     *  physical block mark within the ECC-based view of the page:
     *          (page_size - D) * 8
     *
     *  --Huang Shijie
     */
    block_mark_bit_offset = p_info->attr.page_size * 8 -
        (geo->ecc_strength * geo->gf_len * (geo->ecc_chunk_count - 1)
                + geo->metadata_size * 8);

    geo->block_mark_byte_offset = block_mark_bit_offset / 8;
    geo->block_mark_bit_offset  = block_mark_bit_offset % 8;

    return;
}
/******************************************************************************/
aw_local void imx6ul_gpmi_inst_init2 (struct awbl_dev *p_awdev)
{
    __IMX6UL_GPMI_DEVINFO_DECL(p_info, p_awdev);
    struct awbl_nandbus       *p_bus;
    struct awbl_imx6ul_gpmi   *p_dev =
                 AW_CONTAINER_OF(p_awdev, struct awbl_imx6ul_gpmi, nandbus);

    /* platform initialization */
    if (p_info->pfn_plfm_init != NULL) {
        p_info->pfn_plfm_init();
    }

    p_dev->p_gpmi_regs = (struct __imx6ul_gpmi_regs *)p_info->gpmi_regbase;
    p_dev->p_bch_regs  = (struct __imx6ul_bch_regs *)p_info->bch_regbase;

    /* initialize hardware */
    __imx6ul_gpmi_hw_init(p_dev);

    p_dev->cache_line_size = AW_CACHE_LINE_SIZE;
    p_dev->p_mux_buf = aw_mem_align(6144,p_dev->cache_line_size);
    p_dev->mux_buf_len = 6144;

    p_dev->p_dma_desc = (awbl_imx6ul_dma_desc_t *)aw_mem_align (
                    sizeof(awbl_imx6ul_dma_desc_t)*CONFIG_GPMI_MAX_DMA_DESC_COUNT,
                    p_dev->cache_line_size);

    p_dev->cmd_buffer = (uint8_t *)aw_mem_align (
                    CONGIG_GPMI_CMD_BUF_SIZE,p_dev->cache_line_size);
    p_dev->command_length = 0;

    if ((p_dev->p_dma_desc == NULL) ||
            (p_dev->p_mux_buf == NULL) ||
            (p_dev->cmd_buffer == NULL)) {
        AW_DBGF(("Gpmi dma safe memory alloc failed!\r\n"));
        return;
    }

    p_bus = &(p_dev->nandbus);

    p_bus->serv.pfn_opt    = imx6ul_gpmi_opt;
    p_bus->serv.pfn_ready  = imx6ul_gpmi_ready;
    p_bus->serv.pfn_reset  = imx6ul_gpmi_reset;
    p_bus->serv.pfn_select = imx6ul_gpmi_select;

    /**
     * 如果ecc_support不为AWBL_NAND_ECC_NONE，
     * 则需要填写NandFlash控制器中ecc_ctrl的相关内容
     */
    awbl_nandbus_create(p_bus, p_info->nandbus_index);

    AW_SEMB_INIT(p_dev->sem_bch_finished,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    aw_int_connect(47,__imx6ul_bch_irq,p_dev);
    aw_int_enable(47);

    return;
}
/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_imx6ul_gpmi_drvfuncs = {
        NULL,
        imx6ul_gpmi_inst_init2,
        NULL
};

aw_local aw_const struct awbl_drvinfo __g_imx6ul_gpmi_drv_registration = {
    AWBL_VER_1,                           /* awb_ver */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,  /* bus_id */
    AWBL_IMX6UL_GPMI_NAME,                 /* p_drvname */
    &__g_imx6ul_gpmi_drvfuncs,             /* p_busfuncs */
    NULL,                                 /* p_methods */
    NULL                                  /* pfunc_drv_probe */
};


void awbl_imx6ul_gpmi_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register(&__g_imx6ul_gpmi_drv_registration);
    aw_assert (err == 0);
    return;
}
