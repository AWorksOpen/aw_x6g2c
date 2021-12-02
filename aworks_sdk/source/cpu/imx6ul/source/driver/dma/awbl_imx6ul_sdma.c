/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul SDMA 驱动源文件
 *
 *    注意: 通道0 已经被内部占用, SDMA仅实现异步模式，未实现循环模式
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  lnk, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include <string.h>

#include "aw_assert.h"

#include "driver/dma/awbl_imx6ul_sdma.h"
#include "awbl_imx6ul_sdma_internal.h"

#define PAGE_SIZE               4096

static void __clear_bit(int bit,void *addr)
{
    uint32_t               *p = (uint32_t *)addr;
    register uint32_t       val;

    p += bit/32;
    bit %=32;
    
    val = readl(p);
    val &=  ~(1<<bit);
    
    writel(val,p);
}


static void __set_bit(int bit,void *addr)
{
    uint32_t               *p = (uint32_t *)addr;
    register uint32_t       val;

    p += bit/32;
    bit %=32;
    
    val = readl(p);

    val |= (1<<bit);
    writel(val,p);
}

#define BIT                     AW_BIT

#define NUM_BD (int)(PAGE_SIZE / sizeof(struct sdma_buffer_descriptor))
#define SDMA_BD_MAX_CNT 0xfffc /* align with 4 bytes */

#define swap(x,y) do { typeof(x) z = x; x = y; y = z; } while (0)

#define min_t(type, x, y) ({            \
    type __min1 = (x);          \
    type __min2 = (y);          \
    __min1 < __min2 ? __min1 : __min2; })

/** \brief get awbl_imx6ul_sdma_chan_dev from awbl_dev */
#define __TO_GET_SDMA_DEV(p_dev)   \
       (awbl_imx6ul_sdma_dev_t *)(p_dev)

/** \brief get awbl_imx6ul_sdma_devinfo_t from awbl_dev */
#define __TO_GET_SDMA_DEVINFO(p_dev)   \
       (awbl_imx6ul_dma_devinfo_t *)AWBL_DEVINFO_GET(p_dev)


#define SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V1 34
#define SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V2 38
#define SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V3 41
#define SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V4 42

/* SDMA registers */
#define SDMA_H_C0PTR        0x000
#define SDMA_H_INTR         0x004
#define SDMA_H_STATSTOP     0x008
#define SDMA_H_START        0x00c
#define SDMA_H_EVTOVR       0x010
#define SDMA_H_DSPOVR       0x014
#define SDMA_H_HOSTOVR      0x018
#define SDMA_H_EVTPEND      0x01c
#define SDMA_H_DSPENBL      0x020
#define SDMA_H_RESET        0x024
#define SDMA_H_EVTERR       0x028
#define SDMA_H_INTRMSK      0x02c
#define SDMA_H_PSW          0x030
#define SDMA_H_EVTERRDBG    0x034
#define SDMA_H_CONFIG       0x038
#define SDMA_ONCE_ENB       0x040
#define SDMA_ONCE_DATA      0x044
#define SDMA_ONCE_INSTR     0x048
#define SDMA_ONCE_STAT      0x04c
#define SDMA_ONCE_CMD       0x050
#define SDMA_EVT_MIRROR     0x054
#define SDMA_ILLINSTADDR    0x058
#define SDMA_CHN0ADDR       0x05c
#define SDMA_ONCE_RTB       0x060
#define SDMA_XTRIG_CONF1    0x070
#define SDMA_XTRIG_CONF2    0x074
#define SDMA_CHNENBL0_IMX35 0x200
#define SDMA_CHNENBL0_IMX31 0x080
#define SDMA_CHNPRI_0       0x100


/*
 * Buffer descriptor status values.
 */
#define BD_DONE  0x01
#define BD_WRAP  0x02
#define BD_CONT  0x04
#define BD_INTR  0x08
#define BD_RROR  0x10
#define BD_LAST  0x20
#define BD_EXTD  0x80

#define IMX_DMA_SG_LOOP     BIT(0)

/*
 * Data Node descriptor status values.
 */
#define DND_END_OF_FRAME  0x80
#define DND_END_OF_XFER   0x40
#define DND_DONE          0x20
#define DND_UNUSED        0x01

/*
 * IPCV2 descriptor status values.
 */
#define BD_IPCV2_END_OF_FRAME  0x40

#define IPCV2_MAX_NODES        50
/*
 * Error bit set in the CCB status field by the SDMA,
 * in setbd routine, in case of a transfer error
 */
#define DATA_ERROR  0x10000000

/*
 * Buffer descriptor commands.
 */
#define C0_ADDR             0x01
#define C0_LOAD             0x02
#define C0_DUMP             0x03
#define C0_SETCTX           0x07
#define C0_GETCTX           0x03
#define C0_SETDM            0x01
#define C0_SETPM            0x04
#define C0_GETDM            0x02
#define C0_GETPM            0x08
/*
 * Change endianness indicator in the BD command field
 */
#define CHANGE_ENDIANNESS   0x80

#define SDMA_H_CONFIG_DSPDMA    BIT(12) /* indicates if the DSPDMA is used */
#define SDMA_H_CONFIG_RTD_PINS  BIT(11) /* indicates if Real-Time Debug pins are enabled */
#define SDMA_H_CONFIG_ACR       BIT(4)  /* indicates if AHB freq /core freq = 2 or 1 */
#define SDMA_H_CONFIG_CSM       (3)       /* indicates which context switch mode is selected*/


static struct sdma_script_start_addrs sdma_script_imx6sx = {
    .ap_2_ap_addr = 642,
    .uart_2_mcu_addr = 817,
    .mcu_2_app_addr = 747,
    .uartsh_2_mcu_addr = 1032,
    .mcu_2_shp_addr = 960,
    .app_2_mcu_addr = 683,
    .shp_2_mcu_addr = 891,
    .spdif_2_mcu_addr = 1100,
    .mcu_2_spdif_addr = 1134,
};

static struct sdma_driver_data sdma_imx6sx = {
    .chnenbl0 = SDMA_CHNENBL0_IMX35,
    .num_events = 48,
    .script_addrs = &sdma_script_imx6sx,
};

static int sdma_chan_start_sync(void *chan,uint32_t timeout);

static inline uint32_t chnenbl_ofs(awbl_imx6ul_sdma_dev_t *sdma, unsigned int event)
{
    uint32_t chnenbl0 = sdma->drvdata->chnenbl0;
    return chnenbl0 + event * 4;
}

static int sdma_set_channel_priority(struct sdma_channel *sdmac,
        unsigned int priority)
{
    awbl_imx6ul_sdma_dev_t *sdma = sdmac->sdma;
    int                     channel = sdmac->channel;

    if (priority < MXC_SDMA_MIN_PRIORITY
        || priority > MXC_SDMA_MAX_PRIORITY) {
        return -AW_EINVAL;
    }

    writel(priority, sdma->regs + SDMA_CHNPRI_0 + 4 * channel);

    return 0;
}

static int sdma_request_channel(struct sdma_channel *sdmac)
{
    awbl_imx6ul_sdma_dev_t      *sdma = sdmac->sdma;
    int                         channel = sdmac->channel;
    int                         ret = -AW_EBUSY;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    if(NULL != sdmac->bd ) {
        AW_ERRF(("%s@%d::err aw_cache_dma_align \r\n", __func__,__LINE__));
        ret = -AW_EBUSY;
        goto cleanup;
    }

    sdmac->bd = aw_cache_dma_align(PAGE_SIZE,32);
    if( NULL == sdmac->bd) {
        AW_ERRF(("%s@%d::err aw_cache_dma_align \r\n", __func__,__LINE__));
        ret = -AW_ENOMEM;
        goto cleanup;
    }
    sdmac->bd_phys = (dma_addr_t)sdmac->bd;

    memset(sdmac->bd, 0, PAGE_SIZE);

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    sdma->channel_control[channel].base_bd_ptr = sdmac->bd_phys;
    sdma->channel_control[channel].current_bd_ptr = sdmac->bd_phys;
    AW_MUTEX_UNLOCK(sdma->dev_lock);

    sdma_set_channel_priority(sdmac, MXC_SDMA_DEFAULT_PRIORITY);
    ret = 0;
cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

static int sdma_config_ownership(struct sdma_channel *sdmac,
        aw_bool_t event_override, aw_bool_t mcu_override, aw_bool_t dsp_override)
{
    awbl_imx6ul_sdma_dev_t  *sdma = sdmac->sdma;
    int                     channel = sdmac->channel;
    unsigned long           evt, mcu, dsp;

    if (event_override && mcu_override && dsp_override)
        return -AW_EINVAL;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    evt = readl(sdma->regs + SDMA_H_EVTOVR);
    mcu = readl(sdma->regs + SDMA_H_HOSTOVR);
    dsp = readl(sdma->regs + SDMA_H_DSPOVR);

    if (dsp_override)
        __clear_bit(channel, &dsp);
    else
        __set_bit(channel, &dsp);

    if (event_override)
        __clear_bit(channel, &evt);
    else
        __set_bit(channel, &evt);

    if (mcu_override)
        __clear_bit(channel, &mcu);
    else
        __set_bit(channel, &mcu);

    writel(evt, sdma->regs + SDMA_H_EVTOVR);
    writel(mcu, sdma->regs + SDMA_H_HOSTOVR);
    writel(dsp, sdma->regs + SDMA_H_DSPOVR);
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    return 0;
}

static int sdma_init(awbl_imx6ul_sdma_dev_t *sdma)
{
    int                 i, ret = AW_ERROR, ccbsize;
    dma_addr_t          ccb_phys;


    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    /* Be sure SDMA has not started yet */
    writel(0, sdma->regs + SDMA_H_C0PTR);

    ccbsize = MAX_DMA_CHANNELS * sizeof (struct sdma_channel_control)
        + sizeof(struct sdma_context_data);

    sdma->channel_control = aw_cache_dma_align( ccbsize, 32);
    if ( NULL == sdma->channel_control ) {
        ret = -AW_ENOMEM;
        AW_ERRF(("%s@%d::err aw_cache_dma_align \r\n", __func__,__LINE__));
        goto cleanup;
    }
    ccb_phys = (dma_addr_t)sdma->channel_control;

    sdma->context = (void *)sdma->channel_control +
        MAX_DMA_CHANNELS * sizeof (struct sdma_channel_control);
    sdma->context_phys = ccb_phys +
        MAX_DMA_CHANNELS * sizeof (struct sdma_channel_control);

    /* Zero-out the CCB structures array just allocated */
    memset(sdma->channel_control, 0,
            MAX_DMA_CHANNELS * sizeof (struct sdma_channel_control));

    /* disable all channels */
    for (i = 0; i < sdma->drvdata->num_events; i++) {
        writel(0, sdma->regs + chnenbl_ofs(sdma, i));
    }

    /* All channels have priority 0 */
    for (i = 0; i < MAX_DMA_CHANNELS; i++)
        writel(0, sdma->regs + SDMA_CHNPRI_0 + i * 4);

    ret = sdma_request_channel(&sdma->channel[0]);
    if (ret) {
        AW_ERRF(("%s@%d::err sdma_request_channel \r\n", __func__,__LINE__));
        goto cleanup;
    }


    sdma_config_ownership(&sdma->channel[0], AW_FALSE, AW_TRUE, AW_FALSE);

    /* Set Command Channel (Channel Zero) */
    writel(0x4050, sdma->regs + SDMA_CHN0ADDR);

    /* Set bits of CONFIG register but with static context switching */
    /* FIXME: Check whether to set ACR bit depending on clock ratios */
    writel(0, sdma->regs + SDMA_H_CONFIG);

    writel(ccb_phys, sdma->regs + SDMA_H_C0PTR);

    /* Initializes channel's priorities */
    sdma_set_channel_priority(&sdma->channel[0], 7);
    ret = 0;
cleanup:
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    return ret;
}

static void sdma_add_scripts(awbl_imx6ul_sdma_dev_t *sdma,
        const struct sdma_script_start_addrs *addr)
{
    int32_t *addr_arr = (int32_t *)addr;
    int32_t *saddr_arr = (int32_t *)sdma->script_addrs;
    int i;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    /* use the default firmware in ROM if missing external firmware */
    if (!sdma->script_number)
        sdma->script_number = SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V1;

    for (i = 0; i < sdma->script_number; i++) {
        if (addr_arr[i] > 0) {
            saddr_arr[i] = addr_arr[i];
        }
    }
    AW_MUTEX_UNLOCK(sdma->dev_lock);
}

static void sdma_enable_channel(awbl_imx6ul_sdma_dev_t *sdma, int channel)
{
    writel(BIT(channel), sdma->regs + SDMA_H_START);
}

/*
 * sdma_run_channel0 - run a channel and wait till it's done
 */
static int sdma_run_channel0(awbl_imx6ul_sdma_dev_t *sdma)
{
    int             ret;

    ret = sdma_chan_start_sync(&sdma->channel[0],20000);

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    /* Set bits of CONFIG register with dynamic context switching */
    if (readl(sdma->regs + SDMA_H_CONFIG) == 0) {
        writel(SDMA_H_CONFIG_CSM, sdma->regs + SDMA_H_CONFIG);
    }
    AW_MUTEX_UNLOCK(sdma->dev_lock);

    return ret;
}

static int sdma_load_script(awbl_imx6ul_sdma_dev_t *sdma,
        void *buf, int size, uint32_t address)
{
    struct sdma_buffer_descriptor   *bd0 = sdma->channel[0].bd;
    void                            *buf_virt = NULL;
    dma_addr_t                      buf_phys;
    int                             ret = -AW_ENOMEM;


    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    buf_virt = aw_cache_dma_align( size, 32);
    if ( NULL == buf_virt ) {
        AW_ERRF(("%s@%d::err aw_cache_dma_align \r\n", __func__,__LINE__));
        goto cleanup;
    }
    buf_phys = (dma_addr_t)buf_virt;

    AW_MUTEX_LOCK(sdma->channel[0].chan_lock,AW_WAIT_FOREVER);

    bd0->mode.command = C0_SETPM;
    bd0->mode.status = BD_DONE | BD_INTR | BD_WRAP | BD_EXTD;
    bd0->mode.count = size / 2;
    bd0->buffer_addr = buf_phys;
    bd0->ext_buffer_addr = address;

    memcpy(buf_virt, buf, size);

    AW_MUTEX_UNLOCK(sdma->channel[0].chan_lock);
    ret = sdma_run_channel0(sdma);

cleanup:
    aw_cache_dma_free(buf_virt);
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    return ret;
}

#define SDMA_FIRMWARE_MAGIC 0x414d4453
static int sdma_load_firmware(awbl_imx6ul_sdma_dev_t *sdma,
        const struct sdma_firmware_header *header)
{
    const struct sdma_script_start_addrs    *addr;
    unsigned short                          *ram_code;
    int                                     ret = -AW_EINVAL;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    if (header->magic != SDMA_FIRMWARE_MAGIC) {
        AW_ERRF(("%s@%d::err invalid firmware\r\n", __func__,__LINE__));
        goto cleanup;
    }

    switch (header->version_major) {
        case 1:
            sdma->script_number = SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V1;
            break;
        case 2:
            sdma->script_number = SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V2;
            break;
        case 3:
            sdma->script_number = SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V3;
            break;
        case 4:
            sdma->script_number = SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V4;
            break;
        default:
            AW_ERRF(("%s@%d::err unknown firmware version\r\n",
                    __func__,__LINE__));
            goto cleanup;
    }

    addr = (void *)header + header->script_addrs_start;
    ram_code = (void *)header + header->ram_code_start;


    /* download the RAM image for SDMA */
    ret = sdma_load_script(sdma, ram_code,
            header->ram_code_size,
            addr->ram_code_start_addr);
    if( ret ) {
        AW_ERRF(("%s@%d::err unknown firmware version\r\n",
                    __func__,__LINE__));
        goto cleanup;
    }

    sdma_add_scripts(sdma, addr);

    AW_INFOF(("%s@%d::info loaded firmware %d.%d\n",
            __func__,__LINE__,
            header->version_major,
            header->version_minor));
    ret = 0;
cleanup:
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    return ret;
}



static int sdma_chan_stop(void *chan)
{
    struct sdma_channel    *sdmac = (struct sdma_channel *)chan;
    awbl_imx6ul_sdma_dev_t *sdma = sdmac->sdma;
    int channel = sdmac->channel;


    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdmac->status = DMA_ERROR;
    AW_MUTEX_UNLOCK(sdmac->chan_lock);

    writel(BIT(channel), sdma->regs + SDMA_H_STATSTOP);

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdmac->fn_callback = NULL;
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return  0;
}

static void sdma_event_disable(struct sdma_channel *sdmac, unsigned int event)
{
    awbl_imx6ul_sdma_dev_t  *sdma = sdmac->sdma;
    int                     channel = sdmac->channel;
    uint32_t                     chnenbl = chnenbl_ofs(sdma, event);
    unsigned long           v;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    v = readl(sdma->regs + chnenbl);
    __clear_bit(channel, &v);
    writel(v, sdma->regs + chnenbl);
    AW_MUTEX_UNLOCK(sdma->dev_lock);
}

static int sdma_free_chan_resources(void *chan)
{
    struct sdma_channel     *sdmac = (struct sdma_channel*)chan;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdma_chan_stop(sdmac);

    if (sdmac->event_id0 >= 0) {
        sdma_event_disable(sdmac, sdmac->event_id0);
        sdmac->event_id0 = 0;
    }
    if (sdmac->event_id1) {
        sdma_event_disable(sdmac, sdmac->event_id1);
        sdmac->event_id1 = 0;
    }

    sdmac->event_id0 = 0;
    sdmac->event_id1 = 0;

    sdma_set_channel_priority(sdmac, 0);

    aw_cache_dma_free(sdmac->bd);
    sdmac->bd = NULL;
    sdmac->bd_phys = 0;
    sdmac->alloced = 0;
    AW_MUTEX_UNLOCK(sdmac->chan_lock);

    return 0;
}

void * sdma_alloc_chan_resources(awbl_dev_t *p_dev,struct imx_dma_data *data)
{
    awbl_imx6ul_sdma_dev_t      *sdma = __TO_GET_SDMA_DEV(p_dev);
    struct sdma_channel         *sdmac = NULL;
    int                         i;
    int                         prio, ret = -AW_ENOSPC;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);

    //首先查找一个没有被分配的通道
    for(i = 1;i<MAX_DMA_CHANNELS;i++ ) {
        sdmac = &sdma->channel[i];
        AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
        if(!sdmac->alloced ) {
            break;
        }
        AW_MUTEX_UNLOCK(sdmac->chan_lock);
    }

    if(MAX_DMA_CHANNELS <= i) {
        sdmac = NULL;
        goto cleanup;
    }

    switch (data->priority) {
    case DMA_PRIO_HIGH:
        prio = 3;
        break;
    case DMA_PRIO_MEDIUM:
        prio = 2;
        break;
    case DMA_PRIO_LOW:
    default:
        prio = 1;
        break;
    }

    sdmac->peripheral_type = data->peripheral_type;
    sdmac->event_id0 = data->dma_request;
    sdmac->event_id1 = data->dma_request2;

    ret = sdma_request_channel(sdmac);
    if (ret) {
        AW_ERRF(("%s@%d::err \r\n", __func__,__LINE__));
        goto chan_cleanup;
    }


    ret = sdma_set_channel_priority(sdmac, prio);
    if (ret) {
        AW_ERRF(("%s@%d::err \r\n", __func__,__LINE__));
        goto chan_cleanup;
    }

    sdmac->alloced = 1;
chan_cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
cleanup:
    if(0 != ret && NULL != sdmac) {
        sdma_free_chan_resources(sdmac);
        sdmac = NULL;
    }
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    return sdmac;
}


static void sdma_event_enable(struct sdma_channel *sdmac, unsigned int event)
{
    awbl_imx6ul_sdma_dev_t  *sdma = sdmac->sdma;
    int channel = sdmac->channel;
    unsigned long v = 0;
    uint32_t chnenbl = chnenbl_ofs(sdma, event);

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    v = readl(sdma->regs + chnenbl);
    __set_bit(channel,&v);
    writel(v, sdma->regs + chnenbl);
    AW_MUTEX_UNLOCK(sdma->dev_lock);
}

/*
 * sets the pc of SDMA script according to the peripheral type
 */
static void sdma_get_pc(struct sdma_channel *sdmac,
        enum sdma_peripheral_type peripheral_type)
{
    awbl_imx6ul_sdma_dev_t *sdma = sdmac->sdma;
    int per_2_emi = 0, emi_2_per = 0;
    /*
     * These are needed once we start to support transfers between
     * two peripherals or memory-to-memory transfers
     */
    int per_2_per = 0, emi_2_emi = 0;

    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);

    sdmac->pc_from_device = 0;
    sdmac->pc_to_device = 0;
    sdmac->device_to_device = 0;
    sdmac->pc_to_pc = 0;

    switch (peripheral_type) {
    case IMX_DMATYPE_MEMORY:
        emi_2_emi = sdma->script_addrs->ap_2_ap_addr;
        break;
    case IMX_DMATYPE_DSP:
        emi_2_per = sdma->script_addrs->bp_2_ap_addr;
        per_2_emi = sdma->script_addrs->ap_2_bp_addr;
        break;
    case IMX_DMATYPE_FIRI:
        per_2_emi = sdma->script_addrs->firi_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_firi_addr;
        break;
    case IMX_DMATYPE_UART:
        per_2_emi = sdma->script_addrs->uart_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_app_addr;
        break;
    case IMX_DMATYPE_UART_SP:
        per_2_emi = sdma->script_addrs->uartsh_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_shp_addr;
        break;
    case IMX_DMATYPE_ATA:
        per_2_emi = sdma->script_addrs->ata_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_ata_addr;
        break;
    case IMX_DMATYPE_CSPI:
        per_2_emi = sdma->script_addrs->app_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_ecspi_addr;
        break;
    case IMX_DMATYPE_EXT:
    case IMX_DMATYPE_SSI:
    case IMX_DMATYPE_SAI:
        per_2_emi = sdma->script_addrs->app_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_app_addr;
        break;
    case IMX_DMATYPE_SSI_DUAL:
        per_2_emi = sdma->script_addrs->ssish_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_ssish_addr;
        break;
    case IMX_DMATYPE_SSI_SP:
    case IMX_DMATYPE_MMC:
    case IMX_DMATYPE_SDHC:
    case IMX_DMATYPE_CSPI_SP:
    case IMX_DMATYPE_ESAI:
    case IMX_DMATYPE_MSHC_SP:
        per_2_emi = sdma->script_addrs->shp_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_shp_addr;
        break;
    case IMX_DMATYPE_ASRC:
        per_2_emi = sdma->script_addrs->asrc_2_mcu_addr;
        emi_2_per = sdma->script_addrs->asrc_2_mcu_addr;
        per_2_per = sdma->script_addrs->per_2_per_addr;
        break;
    case IMX_DMATYPE_ASRC_SP:
        per_2_emi = sdma->script_addrs->shp_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_shp_addr;
        per_2_per = sdma->script_addrs->per_2_per_addr;
        break;
    case IMX_DMATYPE_MSHC:
        per_2_emi = sdma->script_addrs->mshc_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_mshc_addr;
        break;
    case IMX_DMATYPE_CCM:
        per_2_emi = sdma->script_addrs->dptc_dvfs_addr;
        break;
    case IMX_DMATYPE_SPDIF:
        per_2_emi = sdma->script_addrs->spdif_2_mcu_addr;
        emi_2_per = sdma->script_addrs->mcu_2_spdif_addr;
        break;
    case IMX_DMATYPE_IPU_MEMORY:
        emi_2_per = sdma->script_addrs->ext_mem_2_ipu_addr;
        break;
    case IMX_DMATYPE_HDMI:
        emi_2_per = sdma->script_addrs->hdmi_dma_addr;
        break;
    default:
        break;
    }

    sdmac->pc_from_device = per_2_emi;
    sdmac->pc_to_device = emi_2_per;
    sdmac->device_to_device = per_2_per;
    sdmac->pc_to_pc = emi_2_emi;

    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    AW_MUTEX_UNLOCK(sdma->dev_lock);
}

static void sdma_set_watermarklevel_for_p2p(struct sdma_channel *sdmac)
{
    int     lwml;
    int     hwml;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    lwml = sdmac->watermark_level & 0xff;
    hwml = (sdmac->watermark_level >> 16) & 0xff;

    if (sdmac->event_id0 > 31) {
        sdmac->event_mask[0] |= 0;
        __set_bit(28, &sdmac->watermark_level);
        sdmac->event_mask[1] |=
                BIT(sdmac->event_id0 % 32);
    } else {
        sdmac->event_mask[0] |= 0;
        sdmac->event_mask[1] |=
                BIT(sdmac->event_id0 % 32);
    }
    if (sdmac->event_id1 > 31) {
        sdmac->event_mask[1] |= 0;
        __set_bit(29, &sdmac->watermark_level);
        sdmac->event_mask[0] |=
            BIT(sdmac->event_id1 % 32);
    } else {
        sdmac->event_mask[1] |= 0;
        sdmac->event_mask[0] |=
            BIT(sdmac->event_id1 % 32);
    }

    /*
     * If LWML(src_maxburst) > HWML(dst_maxburst), we need
     * swap LWML and HWML of INFO(A.3.2.5.1), also need swap
     * r0(event_mask[1]) and r1(event_mask[0]).
     */
    if (lwml > hwml) {
        sdmac->watermark_level &= ~0xff00ff;
        sdmac->watermark_level |= hwml;
        sdmac->watermark_level |= lwml << 16;
        swap(sdmac->event_mask[0], sdmac->event_mask[1]);
    }
    /* BIT 11:
     * 1 : Source on SPBA
     * 0 : Source on AIPS
     */
    __set_bit(11, &sdmac->watermark_level);
    /* BIT 12:
     * 1 : Destination on SPBA
     * 0 : Destination on AIPS
     */
    __set_bit(12, &sdmac->watermark_level);
    __set_bit(31, &sdmac->watermark_level);
    /* BIT 31:
     * 1 : Amount of samples to be transferred is
     * unknown and script will keep on transferring
     * samples as long as both events are detected
     * and script must be manually stopped by the
     * application.
     * 0 : The amount of samples to be is equal to
     * the count field of mode word
     *
     */
    __set_bit(25, &sdmac->watermark_level);
    __clear_bit(24, &sdmac->watermark_level);

    AW_MUTEX_UNLOCK(sdmac->chan_lock);
}

static int sdma_load_context(struct sdma_channel *sdmac)
{
    struct sdma_engine             *sdma = sdmac->sdma;
    int                             channel = sdmac->channel;
    int                             load_address;
    struct sdma_context_data       *context = sdma->context;
    struct sdma_buffer_descriptor  *bd0 = sdma->channel[0].bd;
    int                             ret;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    if (sdmac->direction == DMA_DEV_TO_MEM)
        load_address = sdmac->pc_from_device;
    else if (sdmac->direction == DMA_DEV_TO_DEV)
        load_address = sdmac->device_to_device;
    else if (sdmac->direction == DMA_MEM_TO_MEM)
        load_address = sdmac->pc_to_pc;
    else
        load_address = sdmac->pc_to_device;

    if (load_address < 0) {
        ret = load_address;
        goto cleanup;
    }

    //aw_kprintf("load_address = %d\n", load_address);
    //aw_kprintf("wml = 0x%08x\n", (uint32_t)sdmac->watermark_level);
    //aw_kprintf("shp_addr = 0x%08x\n", sdmac->shp_addr);
    //aw_kprintf("per_addr = 0x%08x\n", sdmac->per_addr);
    //aw_kprintf("event_mask0 = 0x%08x\n", (uint32_t)sdmac->event_mask[0]);
    //aw_kprintf("event_mask1 = 0x%08x\n", (uint32_t)sdmac->event_mask[1]);

    AW_MUTEX_LOCK(sdma->channel[0].chan_lock,AW_WAIT_FOREVER);

    memset(context, 0, sizeof(*context));
    context->channel_state.pc = load_address;

    /* Send by context the event mask,base address for peripheral
     * and watermark level
     */
    if (sdmac->peripheral_type == IMX_DMATYPE_HDMI) {
        context->gReg[4] = sdmac->per_addr;
        context->gReg[6] = sdmac->shp_addr;
    } else {
        context->gReg[0] = sdmac->event_mask[1];
        context->gReg[1] = sdmac->event_mask[0];
        context->gReg[2] = sdmac->per_addr;
        context->gReg[6] = sdmac->shp_addr;
        context->gReg[7] = sdmac->watermark_level;
    }

    bd0->mode.command = C0_SETDM;
    bd0->mode.status = BD_DONE | BD_INTR | BD_WRAP | BD_EXTD;
    bd0->mode.count = sizeof(*context) / 4;
    bd0->buffer_addr = sdma->context_phys;
    bd0->ext_buffer_addr = 2048 + (sizeof(*context) / 4) * channel;

    AW_MUTEX_UNLOCK(sdma->channel[0].chan_lock);
    ret = sdma_run_channel0(sdma);

cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

static int sdma_config_channel(struct sdma_channel *sdmac)
{
    int ret = -AW_EINVAL;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);

    if (sdmac->event_id0 >= sdmac->sdma->drvdata->num_events) {
        goto cleanup;

    }
    if (sdmac->event_id1 >= sdmac->sdma->drvdata->num_events) {
       goto cleanup;
    }
    sdma_chan_stop(sdmac);

    sdmac->event_mask[0] = 0;
    sdmac->event_mask[1] = 0;
    sdmac->shp_addr = 0;
    sdmac->per_addr = 0;

    if (sdmac->event_id0 >= 0) {
        sdma_event_enable(sdmac, sdmac->event_id0);
    }

    if (sdmac->event_id1) {
        sdma_event_enable(sdmac, sdmac->event_id1);
    }

    switch (sdmac->peripheral_type) {
    case IMX_DMATYPE_DSP:
        sdma_config_ownership(sdmac, AW_FALSE, AW_TRUE, AW_TRUE);
        break;
    case IMX_DMATYPE_MEMORY:
        sdma_config_ownership(sdmac, AW_FALSE, AW_TRUE, AW_FALSE);
        break;
    default:
        sdma_config_ownership(sdmac, AW_TRUE, AW_TRUE, AW_FALSE);
        break;
    }

    sdma_get_pc(sdmac, sdmac->peripheral_type);

    if ((sdmac->peripheral_type != IMX_DMATYPE_MEMORY) &&
            (sdmac->peripheral_type != IMX_DMATYPE_DSP)) {
        /* Handle multiple event channels differently */
        if (sdmac->event_id1) {
            if (sdmac->peripheral_type == IMX_DMATYPE_ASRC_SP ||
                sdmac->peripheral_type == IMX_DMATYPE_ASRC)
                sdma_set_watermarklevel_for_p2p(sdmac);
        } else
            __set_bit(sdmac->event_id0, sdmac->event_mask);

        /* Watermark Level */
        sdmac->watermark_level |= sdmac->watermark_level;
        /* Address */
        if (sdmac->direction == DMA_DEV_TO_DEV ||
             (sdmac->peripheral_type == IMX_DMATYPE_HDMI)) {
            sdmac->shp_addr = sdmac->per_address2;
            sdmac->per_addr = sdmac->per_address;
        } else {
            sdmac->shp_addr = sdmac->per_address;
        }
    } else {
        sdmac->watermark_level = 0; /* FIXME: M3_BASE_ADDRESS */
    }

    ret = sdma_load_context(sdmac);
cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

int sdma_control(void *chan, enum dma_ctrl_cmd cmd,
        unsigned long arg)
{
    struct sdma_channel        *sdmac = (struct sdma_channel *)chan;
    struct dma_slave_config    *dmaengine_cfg = (void *)arg;
    int                         ret = -AW_EINVAL;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    switch (cmd) {
    case DMA_TERMINATE_ALL:
        sdma_chan_stop(sdmac);
        ret = 0;
        break;
    case DMA_SLAVE_CONFIG:
        if (dmaengine_cfg->direction == DMA_DEV_TO_MEM) {
            sdmac->per_address = dmaengine_cfg->src_addr;
            sdmac->watermark_level = dmaengine_cfg->src_maxburst *
                        dmaengine_cfg->src_addr_width;
            sdmac->word_size = dmaengine_cfg->src_addr_width;
        } else if (dmaengine_cfg->direction == DMA_DEV_TO_DEV) {
            sdmac->per_address = dmaengine_cfg->src_addr;
            sdmac->per_address2 = dmaengine_cfg->dst_addr;
            sdmac->watermark_level =
                dmaengine_cfg->src_maxburst & 0xff;
            sdmac->watermark_level |=
                (dmaengine_cfg->dst_maxburst & 0xff) << 16;
            sdmac->word_size = dmaengine_cfg->dst_addr_width;
        } else if (dmaengine_cfg->direction == DMA_MEM_TO_DEV) {
            sdmac->per_address = dmaengine_cfg->dst_addr;
            sdmac->watermark_level = dmaengine_cfg->dst_maxburst *
                        dmaengine_cfg->dst_addr_width;
            sdmac->word_size = dmaengine_cfg->dst_addr_width;
        } else if (sdmac->peripheral_type == IMX_DMATYPE_HDMI) {
            sdmac->per_address = dmaengine_cfg->src_addr;
            sdmac->per_address2 = dmaengine_cfg->dst_addr;
            sdmac->watermark_level = 0;
        } else if (dmaengine_cfg->direction == DMA_MEM_TO_MEM) {
            sdmac->word_size = dmaengine_cfg->dst_addr_width;
        }

        sdmac->direction = dmaengine_cfg->direction;
        ret = sdma_config_channel(sdmac);
        break;
    default:
        ret = -AW_ENOSYS;
        break;
    }

    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

static int check_bd_buswidth(struct sdma_buffer_descriptor *bd,
                 struct sdma_channel *sdmac, int count,
                 dma_addr_t dma_dst, dma_addr_t dma_src)
{
    int ret = 0;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    switch (sdmac->word_size) {
    case DMA_SLAVE_BUSWIDTH_4_BYTES:
        bd->mode.command = 0;
        if ((count | dma_dst | dma_src) & 3)
            ret = -AW_EINVAL;
        break;
    case DMA_SLAVE_BUSWIDTH_2_BYTES:
        bd->mode.command = 2;
        if ((count | dma_dst | dma_src) & 1)
            ret = -AW_EINVAL;
        break;
    case DMA_SLAVE_BUSWIDTH_1_BYTE:
         bd->mode.command = 1;
         break;
    default:
         ret = -AW_EINVAL;
    }
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

static int sdma_transfer_init(struct sdma_channel *sdmac,
                  enum dma_transfer_direction direction)
{
    int ret = 0;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdmac->status = DMA_IN_PROGRESS;
    sdmac->buf_tail = 0;
    sdmac->flags = 0;
    sdmac->direction = direction;

    ret = sdma_load_context(sdmac);
    if (ret) {
        goto cleanup;
    }

    sdmac->chn_count = 0;
cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

int sdma_prep_memcpy(void *chan,
        dma_addr_t dma_dst,
        dma_addr_t dma_src,
        size_t len,
        unsigned long flags)
{
    struct sdma_channel *sdmac = (struct sdma_channel *)chan;
    struct sdma_engine *sdma = sdmac->sdma;
    int channel = sdmac->channel;
    size_t count;
    int i = 0, param;
    struct sdma_buffer_descriptor *bd;
    int     ret = -AW_EINVAL;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    if (!chan || !len || sdmac->status == DMA_IN_PROGRESS) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }

    if (len >= NUM_BD * SDMA_BD_MAX_CNT) {
        AW_ERRF(("%s@%d::err channel%d: maximum bytes exceeded:%zu > %d\n",
                __func__,__LINE__,
                channel, len, NUM_BD * SDMA_BD_MAX_CNT));
        goto cleanup;
    }

    //aw_kprintf("memcpy: %pad->%pad, len=%zu, channel=%d.\n",
    //    (void *)dma_src, (void *)dma_dst, len, channel);

    ret = sdma_transfer_init(sdmac, DMA_MEM_TO_MEM);
    if (ret) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }


    do {
        count = min_t(size_t, len, SDMA_BD_MAX_CNT);
        bd = &sdmac->bd[i];
        bd->buffer_addr = dma_src;
        bd->ext_buffer_addr = dma_dst;
        bd->mode.count = count;

        ret = check_bd_buswidth(bd, sdmac, count, dma_dst, dma_src);
        if (ret) {
            AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
            goto cleanup;
        }


        dma_src += count;
        dma_dst += count;
        len -= count;
        i++;

        param = BD_DONE | BD_EXTD | BD_CONT;
        /* last bd */
        if (!len) {
            param |= BD_INTR;
            param |= BD_LAST;
            param &= ~BD_CONT;
        }

        //aw_kprintf( "entry %d: count: %d dma: 0x%u %s%s\n",
        //        i, count, bd->buffer_addr,
        //        param & BD_WRAP ? "wrap" : "",
        //        param & BD_INTR ? " intr" : "");

        bd->mode.status = param;
        sdmac->chn_count += count;
    } while (len);

    sdmac->num_bd = i;
    AW_MUTEX_LOCK(sdma->dev_lock,AW_WAIT_FOREVER);
    sdma->channel_control[channel].current_bd_ptr = sdmac->bd_phys;
    AW_MUTEX_UNLOCK(sdma->dev_lock);
    ret = 0;

cleanup:
    if(ret ) {
        sdmac->status = DMA_ERROR;
    }
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}


static int sdma_prep_sg(
        void *chan,
        struct scatterlist *dst_sg, unsigned int dst_nents,
        struct scatterlist *src_sg, unsigned int src_nents,
        enum dma_transfer_direction direction)
{
    struct sdma_channel *sdmac = (struct sdma_channel *)chan;
    struct sdma_engine *sdma = sdmac->sdma;
    int ret, i, count;
    int channel = sdmac->channel;

    ret = 0;
    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);

    if (sdmac->status == DMA_IN_PROGRESS) {
        ret = -AW_EBUSY;
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }

    //aw_kprintf("setting up %d entries for channel %d.\n",
    //        src_nents, channel);

    if (src_nents > NUM_BD) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        ret = -AW_EINVAL;
        goto cleanup;
    }

        if (direction == DMA_MEM_TO_MEM) {
            if(dst_nents < src_nents) {
                AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
                ret = -AW_EINVAL;
                goto cleanup;
            }
            if(NULL == dst_sg ) {
                AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
                ret = -AW_EINVAL;
                goto cleanup;
            }
            for(i = 0; i< src_nents; i++) {
                if(src_sg[i].length != dst_sg[i].length ) {
                    AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
                    ret = -AW_EINVAL;
                    goto cleanup;
                }
            }
        }

    ret = sdma_transfer_init(sdmac, direction);
    if (ret) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }


    for(i = 0; i< src_nents; i++) {
        struct sdma_buffer_descriptor *bd = &sdmac->bd[i];
        int param;

        bd->buffer_addr = src_sg->dma_address;

        if (direction == DMA_MEM_TO_MEM) {
            bd->ext_buffer_addr = dst_sg->dma_address;
        }

        count = src_sg->length;

        if (count > SDMA_BD_MAX_CNT) {
            AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
            AW_ERRF(("SDMA channel %d: maximum bytes for sg entry exceeded: %d > %d\n",
                    channel, count, SDMA_BD_MAX_CNT));
            ret = -AW_EINVAL;
            goto cleanup;
        }

        bd->mode.count = count;
        sdmac->chn_count += count;

        if (direction == DMA_MEM_TO_MEM) {
            ret = check_bd_buswidth(bd, sdmac, count,
                        dst_sg->dma_address,
                        src_sg->dma_address);
        }
        else {
            ret = check_bd_buswidth(bd, sdmac, count, 0,
                        src_sg->dma_address);
        }
        if (ret) {
            AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
            ret = -AW_EINVAL;
            goto cleanup;
        }

        param = BD_DONE | BD_EXTD | BD_CONT;

        if (i + 1 == src_nents) {
            param |= BD_INTR;
            param |= BD_LAST;
            param &= ~BD_CONT;
        }

        //aw_kprintf("entry %d: count: %d dma: 0x%x %s%s\n",
        //        i, count, src_sg->dma_address,
        //        param & BD_WRAP ? "wrap" : "",
        //        param & BD_INTR ? " intr" : "");

        bd->mode.status = param;
        src_sg ++;
        if (direction == DMA_MEM_TO_MEM) {
            dst_sg++;
        }
    }

    sdmac->num_bd = src_nents;
    sdma->channel_control[channel].current_bd_ptr = sdmac->bd_phys;
    ret = 0;
cleanup:
    if(0 != ret) {
        sdmac->status = DMA_ERROR;
    }
    AW_MUTEX_UNLOCK(sdmac->chan_lock);

    return ret;
}

static int sdma_prep_memcpy_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        unsigned long flags)
{
    return sdma_prep_sg(chan, dst_sg, dst_count, src_sg, src_count,
               DMA_MEM_TO_MEM);
}

static int sdma_prep_slave_sg(void *chan,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction)
{
    return sdma_prep_sg(chan, NULL, 0, src_sg, src_count,
               direction);
}

static int sdma_prep_dma_cyclic(
        void *chan, dma_addr_t dma_addr, size_t buf_len,
        size_t period_len, enum dma_transfer_direction direction)
{
    struct sdma_channel *sdmac = (struct sdma_channel *)chan;
    struct sdma_engine *sdma = sdmac->sdma;
    int channel = sdmac->channel;
    int ret = 0, i = 0, buf = 0;
    int num_periods;

    if (sdmac->status == DMA_IN_PROGRESS) {
        ret = -AW_EBUSY;
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }

    sdmac->status = DMA_IN_PROGRESS;

    sdmac->buf_tail = 0;
    sdmac->period_len = period_len;

    sdmac->flags |= IMX_DMA_SG_LOOP;
    sdmac->direction = direction;
    ret = sdma_load_context(sdmac);
    if (ret) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        goto cleanup;
    }

    if (sdmac->peripheral_type == IMX_DMATYPE_HDMI) {
        ret = 0;
        goto cleanup;
    }
    else {
        num_periods = buf_len / period_len;
    }

    if (num_periods > NUM_BD) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        ret = -AW_EINVAL;
        goto cleanup;
    }

    if (period_len > SDMA_BD_MAX_CNT) {
        AW_ERRF(("%s@%d::err \n", __func__,__LINE__));
        ret = -AW_EINVAL;
        goto cleanup;
    }

    if (sdmac->peripheral_type == IMX_DMATYPE_UART) {
        sdmac->chn_count = period_len;
    }

    while (buf < buf_len) {
        struct sdma_buffer_descriptor *bd = &sdmac->bd[i];
        int param;

        bd->buffer_addr = dma_addr;

        bd->mode.count = period_len;

        if (sdmac->word_size > DMA_SLAVE_BUSWIDTH_4_BYTES) {
            ret = -AW_EINVAL;
            goto cleanup;
        }
        if (sdmac->word_size == DMA_SLAVE_BUSWIDTH_4_BYTES)
            bd->mode.command = 0;
        else
            bd->mode.command = sdmac->word_size;

        param = BD_DONE | BD_EXTD | BD_CONT | BD_INTR;
        if (i + 1 == num_periods)
            param |= BD_WRAP;

        //aw_kprintf("entry %d: count: %d dma: %pad %s%s\n",
        //        i, period_len, &dma_addr,
        //        param & BD_WRAP ? "wrap" : "",
        //        param & BD_INTR ? " intr" : "");

        bd->mode.status = param;

        dma_addr += period_len;
        buf += period_len;

        i++;
    }

    sdmac->num_bd = num_periods;
    sdma->channel_control[channel].current_bd_ptr = sdmac->bd_phys;

cleanup:
    if(ret) {
        sdmac->status = DMA_ERROR;
    }
   return ret;
}

static int sdma_get_transfer_count(void *chan)
{
    struct sdma_channel *sdmac = (struct sdma_channel *)chan;
    return  sdmac->chn_real_count;
}

static void sdma_chan_start_sync_callback(void *pVoid,unsigned int len)
{
    aw_semb_id_t            sem_id;

    sem_id = (aw_semb_id_t)pVoid;

    aw_semb_give(sem_id);
}

static int sdma_chan_start_sync(void *chan,uint32_t timeout)
{
    struct sdma_channel    *sdmac = (struct sdma_channel *)chan;
    struct sdma_engine     *sdma = sdmac->sdma;
    int                     ret = -AW_EAGAIN;
    aw_semb_id_t            sem_id;
    AW_SEMB_DECL(sem);

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    if(NULL != sdmac->fn_callback ) {
        ret = -AW_EDEADLK;
        goto cleanup;
    }

    if (sdmac->flags & IMX_DMA_SG_LOOP) {
        ret = -AW_ENOTSUP;
        goto cleanup;
    }

    if(0 == timeout) {
        timeout  = 1;
    }

    if (sdmac->status != DMA_IN_PROGRESS) {
        ret = -AW_EBUSY;
    }

    sem_id = AW_SEMB_INIT(sem,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    sdmac->fn_callback = sdma_chan_start_sync_callback;
    sdmac->param = (void *)sem_id;
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    sdma_enable_channel(sdma, sdmac->channel);
    ret = aw_semb_take(sem_id, timeout);

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdmac->fn_callback = NULL;
    sdmac->param = 0;
cleanup:
     AW_MUTEX_UNLOCK(sdmac->chan_lock);
    return ret;
}

static int sdma_chan_start_async(void *chan,
        pfn_dma_callback_t callback,void *param)
{
    struct sdma_channel    *sdmac = (struct sdma_channel *)chan;
    struct sdma_engine     *sdma = sdmac->sdma;
    int                     ret = -AW_EAGAIN;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    if(NULL != sdmac->fn_callback ) {
        ret = -AW_EDEADLK;
        goto cleanup;
    }
    if (sdmac->status != DMA_IN_PROGRESS) {
        ret = -AW_EBUSY;
        goto cleanup;
    }

    sdmac->fn_callback = callback;
    sdmac->param = param;
    sdmac->chn_real_count = 0;
    ret = 0;
 cleanup:
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    if(0 == ret) {
        sdma_enable_channel(sdma, sdmac->channel);
    }

    return ret;
}



static void sdma_handle_channel_loop(struct sdma_channel *sdmac)
{
    //调用回调函数
    if(NULL != sdmac->fn_callback) {
        sdmac->fn_callback(sdmac->param,sdmac->chn_real_count);
    }
}

static void mxc_sdma_handle_channel_normal(struct sdma_channel *sdmac)
{
    struct sdma_buffer_descriptor *bd;
    int i, error = 0;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    sdmac->chn_real_count = 0;
    /*
     * non loop mode. Iterate over all descriptors, collect
     * errors and call callback function
     */
    for (i = 0; i < sdmac->num_bd; i++) {
        bd = &sdmac->bd[i];

         if (bd->mode.status & (BD_DONE | BD_RROR))
            error = -AW_EIO;
         sdmac->chn_real_count += bd->mode.count;
    }

    if (error)
        sdmac->status = DMA_ERROR;
    else
        sdmac->status = DMA_COMPLETE;

    AW_MUTEX_UNLOCK(sdmac->chan_lock);
    //调用回调函数
    if(NULL != sdmac->fn_callback) {
        sdmac->fn_callback(sdmac->param,sdmac->chn_real_count);
    }

}

static void sdma_update_channel_loop(struct sdma_channel *sdmac)
{
    struct sdma_buffer_descriptor *bd;

    AW_MUTEX_LOCK(sdmac->chan_lock,AW_WAIT_FOREVER);
    /*
     * loop mode. Iterate over descriptors, re-setup them and
     * call callback function.
     */
    while (1) {
        bd = &sdmac->bd[sdmac->buf_tail];

        if (bd->mode.status & BD_DONE)
            break;

        if (bd->mode.status & BD_RROR)
            sdmac->status = DMA_ERROR;

        bd->mode.status |= BD_DONE;
        sdmac->buf_tail++;
        sdmac->buf_tail %= sdmac->num_bd;
        if (sdmac->peripheral_type == IMX_DMATYPE_UART) {
            /* restore mode.count after counter readed */
            sdmac->chn_real_count = bd->mode.count;
            bd->mode.count = sdmac->chn_count;
        }
        else {
            sdmac->chn_real_count = bd->mode.count;
        }

        sdma_handle_channel_loop(sdmac);
    }
    AW_MUTEX_UNLOCK(sdmac->chan_lock);
}

/**
 * \brief 第2阶段初始化
 */
extern const unsigned char imx6ul_sdma_fw[];

static void sdma_intr_task_fn(void *p)
{
    awbl_imx6ul_sdma_dev_t     *sdma =(awbl_imx6ul_sdma_dev_t *)p;
    unsigned long               stat;
    int                         channel;
    struct sdma_channel        *sdmac;

    while(1) {
        AW_SEMB_TAKE(sdma->dma_intr_semb,AW_WAIT_FOREVER);
        stat = readl(sdma->regs + SDMA_H_INTR);
        /* not interested in channel 0 interrupts */
        writel(stat, sdma->regs + SDMA_H_INTR);

        for (channel = 0; channel < MAX_DMA_CHANNELS; channel++) {
            if (stat & (1 << channel)) {
                sdmac = &sdma->channel[channel];
                if (sdmac->flags & IMX_DMA_SG_LOOP) {
                    if (sdmac->peripheral_type != IMX_DMATYPE_HDMI) {
                        sdma_update_channel_loop(sdmac);
                    }
                    else {
                        sdma_handle_channel_loop(sdmac);
                    }
                }
                else {
                    mxc_sdma_handle_channel_normal(sdmac);
                }
            }
        }
        aw_int_enable(sdma->inum);
    }
}

/**
 * \brief ISR for imx6ul timer interrupts
 *
 * This routine handles the imx6ul timer interrupts. This function
 * is hooked as the ISR in the second level initialization routine.
 */
aw_local void __imx6ul_sdma_isr (void *p)
{
    awbl_imx6ul_sdma_dev_t     *sdma =(awbl_imx6ul_sdma_dev_t *)p;
    aw_int_disable(sdma->inum);

    AW_SEMB_GIVE(sdma->dma_intr_semb);
}

#define AIPS1_BASE_ADDR 0x0207C000
#define AIPS2_BASE_ADDR 0x0217C000
static void aips_access_config(void)
{
    /* Set all MPROTx to be non-bufferable, trusted for R/W, not forced to user-mode. */
    writel(0x77777777,AIPS1_BASE_ADDR);
    //*(volatile int *)(AIPS1_BASE_ADDR + 0x04) = 0x77777777;

    writel(0x77777777,AIPS2_BASE_ADDR);
    //*(volatile int *)(AIPS2_BASE_ADDR + 0x04) = 0x77777777;

    /*Set all OPACRx to be non-buuferable, writable, accessible for non-trusted master */
    writel(0,AIPS1_BASE_ADDR + 0x40);
    writel(0,AIPS1_BASE_ADDR + 0x44);
    writel(0,AIPS1_BASE_ADDR + 0x48);
    writel(0,AIPS1_BASE_ADDR + 0x4C);
    writel(0,AIPS1_BASE_ADDR + 0x50);

    writel(0,AIPS2_BASE_ADDR + 0x40);
    writel(0,AIPS2_BASE_ADDR + 0x44);
    writel(0,AIPS2_BASE_ADDR + 0x48);
    writel(0,AIPS2_BASE_ADDR + 0x4C);
    writel(0,AIPS2_BASE_ADDR + 0x50);
}

aw_local void __imx6ul_sdma_inst_init2 (awbl_dev_t *p_dev)
{

    awbl_imx6ul_sdma_dev_t     *sdma = __TO_GET_SDMA_DEV(p_dev);
    awbl_imx6ul_dma_devinfo_t  *p_sdma_devinfo = __TO_GET_SDMA_DEVINFO(p_dev);
    int32_t                    *saddr_arr;
    int                         ret = AW_ERROR;
    int                         i;

    aips_access_config();
    if (p_sdma_devinfo->pfunc_plfm_init) {
        p_sdma_devinfo->pfunc_plfm_init();
    }

    AW_MUTEX_INIT(sdma->dev_lock,AW_SEM_Q_FIFO);

    sdma->regs          = (void *)p_sdma_devinfo->regbase;
    sdma->inum          = p_sdma_devinfo->inum;
    sdma->drvdata       = &sdma_imx6sx;

    sdma->script_addrs = aw_mem_alloc(sizeof(*sdma->script_addrs) );
    if (NULL == sdma->script_addrs) {
        ret = -AW_ENOMEM;
        goto cleanup;
    }
    /* initially no scripts available */
    saddr_arr = (int32_t *)sdma->script_addrs;
    for (i = 0; i < SDMA_SCRIPT_ADDRS_ARRAY_SIZE_V1; i++) {
        saddr_arr[i] = -AW_EINVAL;
    }

    /* Initialize channel parameters */
    for (i = 0; i < MAX_DMA_CHANNELS; i++) {
        struct sdma_channel *sdmac = &sdma->channel[i];

        sdmac->sdma = sdma;
        AW_MUTEX_INIT(sdmac->chan_lock,AW_SEM_Q_FIFO);
        sdmac->channel = i;
    }
    ret = sdma_init(sdma);
    if (ret) {
        AW_ERRF(("%s@%d::err \r\n", __func__,__LINE__));
        goto cleanup;
    }

    AW_SEMB_INIT(sdma->dma_intr_semb,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    AW_TASK_INIT(sdma->dma_intr_task,"dma_intr_task",
            AW_TASK_SYS_PRIORITY(1),DMA_INTR_TASK_STACK_SIZE,
            sdma_intr_task_fn,sdma);
    AW_TASK_STARTUP(sdma->dma_intr_task);

    aw_int_connect(sdma->inum,__imx6ul_sdma_isr,sdma);
    aw_int_enable(sdma->inum);

    sdma_add_scripts(sdma, sdma->drvdata->script_addrs);
    sdma_load_firmware(sdma,(const struct sdma_firmware_header *)imx6ul_sdma_fw);
    ret = 0;
cleanup:
    return ;
}

/**
 * \brief driver functions (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvfuncs __g_sdma_drvfuncs = {
    NULL,                      /**< \brief 1st inst init */
    __imx6ul_sdma_inst_init2,  /**< \brief 2ed inst init */
    NULL                       /**< \brief inst int connect */
};


static const imx6ul_sdma_serv_func_t __g_sdma_serv_func = {
        sdma_alloc_chan_resources,
        sdma_control,
        sdma_prep_memcpy,
        sdma_prep_sg,
        sdma_prep_memcpy_sg,
        sdma_prep_slave_sg,
        sdma_prep_dma_cyclic,
        sdma_chan_start_sync,
        sdma_chan_start_async,
        sdma_get_transfer_count,
        sdma_chan_stop,
        sdma_free_chan_resources,
};
/**
 * \brief method "awbl_adcserv_get" handler
 */
aw_local aw_err_t __imx6ul_sdma_serv_get (
        struct awbl_dev *p_dev, void *p_arg)
{
     awbl_imx6ul_sdma_dev_t            *sdma = __TO_GET_SDMA_DEV(p_dev);
     struct awbl_imx6ul_sdma_service   *p;

     p = &sdma->sdma_serv;
     p->p_dev = p_dev;
     p->p_serv_func = &__g_sdma_serv_func;

     *(struct awbl_imx6ul_sdma_service **)p_arg = p;
    return AW_OK;
}

/* driver methods (must defined as aw_const) */
aw_local aw_const struct awbl_dev_method __g_imx6ul_sdma_dev_methods[] = {
    AWBL_METHOD(awbl_imx6ul_sdma_serv_func_get, __imx6ul_sdma_serv_get),
    AWBL_METHOD_END
};

/**
 * \brief driver registration (must defined as aw_const)
 */
aw_local aw_const awbl_plb_drvinfo_t __g_sdma_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_SDMA_NAME,                  /* p_drvname */
        &__g_sdma_drvfuncs,                     /* p_busfuncs */
        __g_imx6ul_sdma_dev_methods,           /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
/**
 * \brief SDMA 驱动注册
 */
void awbl_imx6ul_sdma_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_sdma_drv_registration);
}

/* end of file */

