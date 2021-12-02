/*******************************************************************************
*                                 Apollo
*                       ----------------------------
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
 * \brief AW imx6ul SDMA 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "aw_imx6ul_sdma.h"
 * \endcode
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-30  jaz, first implementation
 * \endinternal
 */

#ifndef __AW_IMX6UL_SDMA_H
#define __AW_IMX6UL_SDMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"

//类型定义
typedef uint32_t            dma_addr_t;

#define MAX_DMA_CHANNELS 32
#define MXC_SDMA_DEFAULT_PRIORITY 1
#define MXC_SDMA_MIN_PRIORITY 1
#define MXC_SDMA_MAX_PRIORITY 7

/** \brief SDMA 驱动名 */
#define AWBL_IMX6UL_SDMA_NAME                  "imx6ul_sdma"

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

/**
 * enum dma_slave_buswidth - defines bus with of the DMA slave
 * device, source or target buses
 */
enum dma_slave_buswidth {
    DMA_SLAVE_BUSWIDTH_UNDEFINED = 0,
    DMA_SLAVE_BUSWIDTH_1_BYTE = 1,
    DMA_SLAVE_BUSWIDTH_2_BYTES = 2,
    DMA_SLAVE_BUSWIDTH_4_BYTES = 4,
    DMA_SLAVE_BUSWIDTH_8_BYTES = 8,
};

/**
 * struct dma_slave_config - dma slave channel runtime config
 * @direction: whether the data shall go in or out on this slave
 * channel, right now. DMA_MEM_TO_DEV and DMA_DEV_TO_MEM are
 * legal values.
 * @src_addr: this is the physical address where DMA slave data
 * should be read (RX), if the source is memory this argument is
 * ignored.
 * @dst_addr: this is the physical address where DMA slave data
 * should be written (TX), if the source is memory this argument
 * is ignored.
 * @src_addr_width: this is the width in bytes of the source (RX)
 * register where DMA data shall be read. If the source
 * is memory this may be ignored depending on architecture.
 * Legal values: 1, 2, 4, 8.
 * @dst_addr_width: same as src_addr_width but for destination
 * target (TX) mutatis mutandis.
 * @src_maxburst: the maximum number of words (note: words, as in
 * units of the src_addr_width member, not bytes) that can be sent
 * in one burst to the device. Typically something like half the
 * FIFO depth on I/O peripherals so you don't overflow it. This
 * may or may not be applicable on memory sources.
 * @dst_maxburst: same as src_maxburst but for destination target
 * mutatis mutandis.
 * @device_fc: Flow Controller Settings. Only valid for slave channels. Fill
 * with 'true' if peripheral should be flow controller. Direction will be
 * selected at Runtime.
 * @slave_id: Slave requester id. Only valid for slave channels. The dma
 * slave peripheral will have unique id as dma requester which need to be
 * pass as slave config.
 *
 * This struct is passed in as configuration data to a DMA engine
 * in order to set up a certain channel for DMA transport at runtime.
 * The DMA device/engine has to provide support for an additional
 * command in the channel config interface, DMA_SLAVE_CONFIG
 * and this struct will then be passed in as an argument to the
 * DMA engine device_control() function.
 *
 * The rationale for adding configuration information to this struct
 * is as follows: if it is likely that most DMA slave controllers in
 * the world will support the configuration option, then make it
 * generic. If not: if it is fixed so that it be sent in static from
 * the platform data, then prefer to do that. Else, if it is neither
 * fixed at runtime, nor generic enough (such as bus mastership on
 * some CPU family and whatnot) then create a custom slave config
 * struct and pass that, then make this config a member of that
 * struct, if applicable.
 */
struct dma_slave_config {
    enum dma_transfer_direction direction;
    dma_addr_t src_addr;
    dma_addr_t dst_addr;
    enum dma_slave_buswidth src_addr_width;
    enum dma_slave_buswidth dst_addr_width;
    uint32_t src_maxburst;
    uint32_t dst_maxburst;
    aw_bool_t device_fc;
    unsigned int slave_id;
};

/**
 * enum dma_status - DMA transaction status
 * @DMA_COMPLETE: transaction completed
 * @DMA_IN_PROGRESS: transaction not yet processed
 * @DMA_PAUSED: transaction is paused
 * @DMA_ERROR: transaction failed
 */
enum dma_status {
    DMA_COMPLETE,
    DMA_IN_PROGRESS,
    DMA_PAUSED,
    DMA_ERROR,
};

/**
 * enum dma_ctrl_cmd - DMA operations that can optionally be exercised
 * on a running channel.
 * @DMA_TERMINATE_ALL: terminate all ongoing transfers
 * @DMA_PAUSE: pause ongoing transfers
 * @DMA_RESUME: resume paused transfer
 * @DMA_SLAVE_CONFIG: this command is only implemented by DMA controllers
 * that need to runtime reconfigure the slave channels (as opposed to passing
 * configuration data in statically from the platform). An additional
 * argument of struct dma_slave_config must be passed in with this
 * command.
 * @FSLDMA_EXTERNAL_START: this command will put the Freescale DMA controller
 * into external start mode.
 */
enum dma_ctrl_cmd {
    DMA_TERMINATE_ALL,
    DMA_PAUSE,
    DMA_RESUME,
    DMA_SLAVE_CONFIG,
    FSLDMA_EXTERNAL_START,
};

/*
 * This enumerates peripheral types. Used for SDMA.
 */
enum sdma_peripheral_type {
    IMX_DMATYPE_SSI,    /* MCU domain SSI */
    IMX_DMATYPE_SSI_SP, /* Shared SSI */
    IMX_DMATYPE_MMC,    /* MMC */
    IMX_DMATYPE_SDHC,   /* SDHC */
    IMX_DMATYPE_UART,   /* MCU domain UART */
    IMX_DMATYPE_UART_SP,    /* Shared UART */
    IMX_DMATYPE_FIRI,   /* FIRI */
    IMX_DMATYPE_CSPI,   /* MCU domain CSPI */
    IMX_DMATYPE_CSPI_SP,    /* Shared CSPI */
    IMX_DMATYPE_SIM,    /* SIM */
    IMX_DMATYPE_ATA,    /* ATA */
    IMX_DMATYPE_CCM,    /* CCM */
    IMX_DMATYPE_EXT,    /* External peripheral */
    IMX_DMATYPE_MSHC,   /* Memory Stick Host Controller */
    IMX_DMATYPE_MSHC_SP,    /* Shared Memory Stick Host Controller */
    IMX_DMATYPE_DSP,    /* DSP */
    IMX_DMATYPE_MEMORY, /* Memory */
    IMX_DMATYPE_FIFO_MEMORY,/* FIFO type Memory */
    IMX_DMATYPE_SPDIF,  /* SPDIF */
    IMX_DMATYPE_IPU_MEMORY, /* IPU Memory */
    IMX_DMATYPE_ASRC,   /* ASRC */
    IMX_DMATYPE_ESAI,   /* ESAI */
    IMX_DMATYPE_SSI_DUAL,   /* SSI Dual FIFO */
    IMX_DMATYPE_ASRC_SP,    /* Shared ASRC */
    IMX_DMATYPE_HDMI,   /* HDMI Audio */
    IMX_DMATYPE_SAI,        /* SAI Audio */
};

enum imx_dma_prio {
    DMA_PRIO_HIGH = 0,
    DMA_PRIO_MEDIUM = 1,
    DMA_PRIO_LOW = 2
};

struct imx_dma_data {
    int                         dma_request; /* DMA request line */
    int                         dma_request2; /* secondary DMA request line */
    enum sdma_peripheral_type   peripheral_type;
    enum imx_dma_prio           priority;
};

struct scatterlist {
    unsigned int    length;
    dma_addr_t      dma_address;
};

/**
 * \brief initialize AWBus imx6ul sdma service
 *
 * \attention the function should be called
 * after awbl_dev_init1() and before awbl_dev_init2()
 */
void awbl_imx6ul_sdma_init (void);

typedef void (*pfn_dma_callback_t)(void *param,unsigned int len);
void * imx6ul_sdma_alloc_chan(struct imx_dma_data *data);
int imx6ul_sdma_control(void *chan, enum dma_ctrl_cmd cmd,unsigned long arg);

int imx6ul_sdma_prep_memcpy(void *chan,
        dma_addr_t dma_dst,
        dma_addr_t dma_src,
        size_t len,
        unsigned long flags);

int imx6ul_sdma_prep_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

int imx6ul_sdma_prep_memcpy_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        unsigned long flags);

int imx6ul_sdma_prep_slave_sg(void *chan,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

int imx6ul_sdma_prep_dma_cyclic(void *chan,
        dma_addr_t dma_addr, size_t buf_len,
        size_t period_len, enum dma_transfer_direction direction);

int imx6ul_sdma_chan_start_sync(void *chan,uint32_t timeout);
int imx6ul_sdma_chan_start_async(void *chan,
        pfn_dma_callback_t callback,void *param);

int imx6ul_sdma_get_transfer_count(void *chan);
int imx6ul_sdma_chan_stop(void *chan);
int imx6ul_sdma_free_chan(void *chan);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ADC_H */

/* end of file */
