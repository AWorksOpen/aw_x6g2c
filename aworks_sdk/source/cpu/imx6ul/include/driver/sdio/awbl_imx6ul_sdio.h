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
 * \brief imx6ul sdio �豸ʵ��
 */
typedef struct awbl_imx6ul_sdio_dev {
    /** \brief �̳��� AWBus SDIO ���������� */
    awbl_sdio_host_t        host;
    AW_MUTEX_DECL(dev_mux);
    AW_TASK_DECL(imx6ul_sdio_trans_task,
            __SDIO_SPI_TRANS_TASK_STACK_SIZE);
    AW_SEMB_DECL(sem_sync);
    awbl_sdio_msg_t        *p_cur_msg;
    uint8_t                *p_dma_buf;
    int                     dma_buf_len;
    volatile uint32_t       sd_status;          /**< SDMMC����״̬*/
    AW_SEMB_DECL(cc_sem);                       /**< MMC�ź���ʵ�� */
    aw_semb_id_t            cc_sem_id;          /**< MMC�ź���ID  */
    AW_SEMB_DECL(tc_sem);                       /**< MMC�ź���ʵ��*/
    aw_semb_id_t            tc_sem_id;          /**< MMC�ź���ID  */

    /** \brief ������״̬ */
    uint8_t                 cd_state;
} awbl_imx6ul_sdio_dev_t;

/* sdio host �豸��Ϣ  */
typedef struct awbl_imx6ul_sdio_dev_info {
    awbl_sdio_host_info_t   host_info;
    uint32_t                regbase;
    aw_clk_id_t             ref_clk_id;         /**< ����ʱ�ӵ�ID */
    uint32_t                task_trans_prio;    /**< �����������ȼ� */

    /** ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);
    int                    inum;                /**< SDMMC�жϺ�      */

    /** ��ȡ���Ƿ���� */
    aw_bool_t                 (*pfn_cd) (awbl_imx6ul_sdio_dev_t *p_dev);
} awbl_imx6ul_sdio_dev_info_t;

void awbl_imx6ul_sdio_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_SDIO_H */
