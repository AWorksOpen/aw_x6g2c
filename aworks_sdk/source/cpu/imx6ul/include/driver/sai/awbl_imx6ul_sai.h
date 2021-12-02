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
#ifndef __AWBL_IMX6UL_SAI_H
#define __AWBL_IMX6UL_SAI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "awbus_lite.h"
#include "aw_pool.h"

#define AWBL_IMX6UL_SAI_NAME   "awbl_imx6ul_sai"

/* IMX6UL support 3 SAI, SAI1 ~ SAI3 */
#define AWBL_IMX6UL_SAI_NUM     3

#define AW_BL_IMX6UL_SAI_MCLK_SEL_0   0
#define AW_BL_IMX6UL_SAI_MCLK_SEL_1   1
#define AW_BL_IMX6UL_SAI_MCLK_SEL_2   2
#define AW_BL_IMX6UL_SAI_MCLK_SEL_3   3

typedef void (* aw_pfn_buffer_complete_t)(void *param1,void *param2);

/** \brief SAI 配置   */
typedef struct awbl_imx6ul_sai_config {
    uint16_t    flags;        /**< \brief 控制标志  */
    uint8_t     width;        /**< \brief 数据宽度，8-bit, 16-bit, 32-bit */
    uint8_t     channels;     /**< \brief 声道数             */
    uint32_t    sample_rate;  /**< \brief 采样率，如 44100 */
} awbl_imx6ul_sai_config_t;

/** \brief SAI 传输缓冲区   */
typedef struct awbl_imx6ul_sai_buffer {
    const void                 *p_buf;          /**< \brief 数据缓冲区     */
    int                         len;            /**< \brief 数据个数      */
    aw_pfn_buffer_complete_t    fn_complete;    /**< \brief 传输完成回调函数   */
    void                       *p_arg1;         /**< \brief 回调函数的参数 */
    void                       *p_arg2;         /**< \brief 回调函数的参数 */
} awbl_imx6ul_sai_buffer_t;

typedef struct sai_buffer_internal{
    struct sai_buffer_internal *next_fill;
    struct sai_buffer_internal *next_playing;
    const void                 *p_buf;          /**< \brief 数据缓冲区     */
    int                         len;            /**< \brief 数据个数      */
    aw_pfn_buffer_complete_t    fn_complete;    /**< \brief 传输完成回调函数      */
    void                       *p_arg1;         /**< \brief 回调函数的参数 */
    void                       *p_arg2;         /**< \brief 回调函数的参数 */
    int                         filled_count;
    int                         played_count;
    /** \brief 填充的时候是否发生了不完全填充 */
    /* 也就是说填充DMA缓冲区的时候，这个缓冲区的数据已经用完，但是未填满，
     * 且后续没有其它缓冲区
     */
    int                         not_fill_enough;
}sai_buffer_internal_t;
/**
 * \brief imx6ul SAI 设备信息
 */
struct awbl_imx6ul_sai_devinfo {

    uint32_t        regbase;            /**< \brief 寄存器基地址        */
    int             inum;               /**< \brief 中断号       */
    aw_clk_id_t     ref_clk_id;         /**< \brief refence clock id */
    uint8_t         mclk_sel;           /**< \brief clk_sel:0 ~ 3 */

    void (*pfn_plfm_init)(void);        /**< \brief 平台相关初始化   */
    int             tx_dma_event;
    int             use_dma_in_debug;   /**< \brief 在调试器模式下使用dma   */
};

/**
 * \brief imx6ul dma 设备实例
 */
struct awbl_imx6ul_sai_dev {

    struct awbl_dev                 dev;
    void                           *p_regs;
    AW_MUTEX_DECL(mutex_dev);
    aw_bool_t                          busy;

    int                             cur_idx;
    void                           *p_tx_dma_chan;
    uint8_t                        *p_tx_dma_buffer;

    sai_buffer_internal_t          *p_fill_list;
    sai_buffer_internal_t          *p_playing_list;
    aw_pool_t                       pool_sai_buffer_internal;
    uint8_t                         sai_buffer_pool_buffer[512];

    // 负责填充数据的任务
    AW_TASK_DECL(task_fill_audio_data,1024);
    AW_SEMB_DECL(semb_has_valid_audio_data);
    AW_SEMC_DECL(semc_seg_audio_data_played);
};

/**
 * \brief config a channel
 *
 * \param[in] sai_id        : The SAI channel
 * \param[in] p_config      : the config
 *
 * \return AW_OK
 */
aw_err_t  aw_imx6ul_sai_tx_config (int unit_id,
                                awbl_imx6ul_sai_config_t *p_config);

/**
 * \brief append a buffer to sai
 *
 * \param[in] sai_id        : The SAI channel
 * \param[in] p_trans       : The data to be transferred
 *
 * \return AW_OK
 */
aw_err_t  aw_imx6ul_sai_tx_append_buffer (int unit_id,
                                        awbl_imx6ul_sai_buffer_t *p_trans);

/**
 * \brief Stop a SAI transfer (if the channel is in transfer)
 *
 * \param[in] sai_id : The SAI channel to be stopped
 *
 * \return AW_OK
 */
aw_err_t aw_imx6ul_sai_tx_stop (int unit_id);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


