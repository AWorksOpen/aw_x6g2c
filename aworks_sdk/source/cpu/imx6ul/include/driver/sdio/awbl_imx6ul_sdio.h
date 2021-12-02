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
 * \brief imx6ul SDIO driver
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-5  sss, first implementation
 * - 1.01 16-09-09  sni, fix bugs of sdio irq process,the sd_status is unsafe
 *                      because sd_status may be modified in the isr and task at
 *                      the same time.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_SDIO_H
#define __AWBL_IMX6UL_SDIO_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbl_sdiobus.h"


#define AWBL_IMX6UL_SDIO_NAME               "imx6ul_sdio"

#define __SDIO_SPI_TRANS_TASK_STACK_SIZE  4096*4
/**
 * \brief imx6ul sdio 设备实例
 */
typedef struct awbl_imx6ul_sdio_dev {
    /** \brief 继承自 AWBus SDIO 主机控制器 */
    awbl_sdio_host_t        host;
    AW_MUTEX_DECL(dev_mux);
    AW_TASK_DECL(imx6ul_sdio_trans_task,
            __SDIO_SPI_TRANS_TASK_STACK_SIZE);
    AW_SEMB_DECL(sem_sync);
    awbl_sdio_msg_t        *p_cur_msg;
    uint8_t                *p_dma_buf;
    int                     dma_buf_len;
    volatile uint32_t       sd_status;          /**< SDMMC传输状态*/
    AW_SEMB_DECL(cc_sem);                       /**< MMC信号量实体 */
    aw_semb_id_t            cc_sem_id;          /**< MMC信号量ID  */
    AW_SEMB_DECL(tc_sem);                       /**< MMC信号量实体*/
    aw_semb_id_t            tc_sem_id;          /**< MMC信号量ID  */

    /** \brief 卡插入状态 */
    uint8_t                 cd_state;
} awbl_imx6ul_sdio_dev_t;

/* sdio host 设备信息  */
typedef struct awbl_imx6ul_sdio_dev_info {
    awbl_sdio_host_info_t   host_info;
    uint32_t                regbase;
    aw_clk_id_t             ref_clk_id;         /**< 输入时钟的ID */
    uint32_t                task_trans_prio;    /**< 传输任务优先级 */

    /** 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);
    int                    inum;                /**< SDMMC中断号      */

    /** 获取卡是否插入 */
    aw_bool_t                 (*pfn_cd) (awbl_imx6ul_sdio_dev_t *p_dev);
} awbl_imx6ul_sdio_dev_info_t;

void awbl_imx6ul_sdio_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_SDIO_H */
