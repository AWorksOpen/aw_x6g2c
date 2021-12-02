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
 * \brief The IMX6UL AHB-to-APBH Bridge DMA driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-07, tee, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_DMA_H
#define __AWBL_IMX6UL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"

#include "aw_sem.h"                     /* 信号量服务 */

#define AWBL_IMX6UL_DMA_NAME   "imx6ul_dma"

/**
 * \name The DMA channel
 * Total support four channels
 * @{
 */

#define DMA_CHANNEL_0   0
#define DMA_CHANNEL_1   1
#define DMA_CHANNEL_2   2
#define DMA_CHANNEL_3   3

#define IMX6UL_DMA_CHANNEL_GPMI0     DMA_CHANNEL_0
#define IMX6UL_DMA_CHANNEL_GPMI1     DMA_CHANNEL_1
#define IMX6UL_DMA_CHANNEL_GPMI2     DMA_CHANNEL_2
#define IMX6UL_DMA_CHANNEL_GPMI3     DMA_CHANNEL_3

#define IMX6UL_DMA_CHANNEL_NUM           4

/** @} */

#define IMX6UL_DMA_TRANS_OK              0
#define IMX6UL_DMA_DESC_ERR              1
#define IMX6UL_DMA_NOT_INIT              2


/**
 * \name The DMA commands
 *
 * USE with the macro : IMX6UL_DMA_CMD_COMMAND(x)
 *
 * @{
 */

/** \brief Only requested PIO word transfers */
#define IMX6UL_CMD_COMMAND_NO_DMA_XFER       0

/** \brief DMA Write, perform a DMA transfer from the peripheral to mem */
#define IMX6UL_CMD_COMMAND_DMA_WRITE         1

/** \brief DMA Read, perform a DMA transfer from mem to the peripheral */
#define IMX6UL_CMD_COMMAND_DMA_READ          2

/** \brief Perform PIO word transfers, then perform a conditional branch */
#define IMX6UL_CMD_COMMAND_DMA_SENSE         3

/** @} */

/**
 * \name DMA Channel command descriptor
 * @{
 */

/** \brief The Number DMA bytes (16-bit) to transfer, 0 is max (65536) */
#define IMX6UL_DMA_CMD_XFER_COUNT(x)         ((x) << 16)

/** \brief The Number of CMDPIOWORDS (4-bit) 0 ~ 15 */
#define IMX6UL_DMA_CMD_CMDWORDS(x)           ((x) << 12)

#define IMX6UL_DMA_CMD_TERM_FLUSH(x)         ((x) << 9)

/** \brief when an error has occurred, terminate the dma transfer */
#define IMX6UL_DMA_CMD_HALTONTERMINATE(x)    ((x) << 8)

#define IMX6UL_DMA_CMD_WAIT4ENDCMD(x)        ((x) << 7)

/** \brief When a command finishes its DMA transfer, decrements the semaphore */
#define IMX6UL_DMA_CMD_SEMAPHORE(x)          ((x) << 6)

/** \brief for GPMI channel, Wait for ready line is asserted to 1 */
#define IMX6UL_DMA_CMD_NANDWAIT4READY(x)     ((x) << 5)

/** \brief for DMA arbiter */
#define IMX6UL_DMA_CMD_NANDLOCK(x)           ((x) << 4)

/** \brief When complete, The DMA channel will generate interrupt */
#define IMX6UL_DMA_CMD_IRQONCMPLT(x)         ((x) << 3)

/** \brief For DMA descriptor chain if there are still descriptor after this */
#define IMX6UL_DMA_CMD_CHAIN(x)              ((x) << 2)

/** \brief The DMA COMMOND */
#define IMX6UL_DMA_CMD_COMMAND(x)            ((x) << 0)

/** @} */

#if !defined(IMX6UL_DMA_MAX_PIO_WORDS)
#define IMX6UL_DMA_MAX_PIO_WORDS   (15)
#endif

/**
 * \brief The callback function type for dma transfer(async mode)
 *
 * \param[in] p_arg : The user arg for the function
 * \param[in] status : -AW_EIO , error happen; AW_OK , successful complete
 *
 * \return None
 */
typedef void (*awbl_imx6ul_dma_complete_cb_t) (void *p_arg, int status);

/**
 * \brief imx6ul dma 设备信息
 */
struct awbl_imx6ul_dma_devinfo {

    uint32_t regbase;             /**< \brief 寄存器基地址      */
    int      dma_inum;            /**< \brief DMA 中断号          */

    void (*pfn_plfm_init)(void);  /**< \brief 平台相关初始化 */
};

/**
 * \brief DMA descriptor
 */
typedef struct awbl_imx6ul_dma_desc {
    volatile uint32_t next_cmd_desc;
    volatile uint16_t bits;
    volatile uint16_t xfer_bytes;
    volatile uint32_t buf_addr;
    volatile uint32_t pio_word[IMX6UL_DMA_MAX_PIO_WORDS];
} awbl_imx6ul_dma_desc_t;

/**
 * \brief imx6ul DMA 通道结构
 */
typedef struct awbl_imx6ul_dma_chan {

    uint32_t                         regbase;
    uint32_t                         channel;

    awbl_imx6ul_dma_complete_cb_t    pfn_complete_cb;
    void                            *p_arg;
    int                              status;

    uint32_t                         trans_counter;
    awbl_imx6ul_dma_desc_t          *p_dma_desc;

} awbl_imx6ul_dma_chan_t;

/**
 * \brief imx6ul dma 设备实例
 */
struct awbl_imx6ul_dma_dev {

    struct awbl_dev        dma_dev;

    awbl_imx6ul_dma_chan_t chan[IMX6UL_DMA_CHANNEL_NUM];
};

/**
 * \brief Start a dma transfer (sync mode)
 *
 * \param[in] chan          : The channel num to transfer data
 * \param[in] p_desc        : The descriptor for dma transfer,
 *                            can use p_desc->next_cmd_desc chain more descriptor.
 * \param[in] dma_semaphore : The semaphore for the dma channel to increase.
 * \param[in] timeout       : timeout
 *
 * \return AW_OK if succeed, otherwise value for failed
 *
 * \note The dma_semaphore usually equal to the number of descriptors in chain.
 */
aw_err_t aw_imx6ul_dma_chan_start_sync (int                      chan,
                                        awbl_imx6ul_dma_desc_t  *p_desc,
                                        uint8_t                  dma_semaphore,
                                        uint32_t                 timeout);

/**
 * \brief Start a dma transfer (async mode)
 *
 * \param[in] chan          : The channel num to transfer data
 * \param[in] p_desc        : The descriptor for dma transfer,
 *                            can use p_desc->next_cmd_desc chain more descriptor.
 * \param[in] dma_semaphore : The semaphore for the dma channel to increase.
 * \param[in] timeout       : timeout
 *
 * \return AW_OK if succeed, otherwise value for failed
 *
 * \note The dma_semaphore usually equal to the number of descriptors in chain.
 *
 */
aw_err_t  aw_imx6ul_dma_chan_start_async (int                           chan,
                                          awbl_imx6ul_dma_desc_t       *p_desc,
                                          uint8_t                       dma_semaphore,
                                          awbl_imx6ul_dma_complete_cb_t pfn_complete,
                                          void                         *p_arg);

/**
 * \brief Stop a dma transfer (if the channel is in transfer)
 *
 * \param[in] chan : The channel num to be stop
 *
 * \return AW_OK if succeed, otherwise value for failed
 */
aw_err_t aw_imx6ul_dma_chan_stop (int chan);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


