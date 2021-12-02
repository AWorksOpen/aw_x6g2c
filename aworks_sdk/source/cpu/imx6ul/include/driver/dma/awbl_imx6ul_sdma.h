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
 * \brief imx6ul SDMA driver header file
 *
 *   SDMA ����ʵ����ΧI/O�豸���ڲ����ⲿ��������ݴ���
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  lnk, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_SDMA_H
#define __AWBL_IMX6UL_SDMA_H

#include "apollo.h"
#include "aw_sem.h"
#include "aw_imx6ul_sdma.h"
#include "awbl_imx6ul_sdma_service.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

struct sdma_engine;
struct sdma_channel {
    struct sdma_engine              *sdma;
    int                             alloced;
    unsigned int                    channel;
    enum dma_transfer_direction     direction;
    enum sdma_peripheral_type       peripheral_type;
    unsigned int                    event_id0;
    unsigned int                    event_id1;
    enum dma_slave_buswidth         word_size;
    unsigned int                    buf_tail;
    unsigned int                    num_bd;
    unsigned int                    period_len;
    struct sdma_buffer_descriptor   *bd;
    dma_addr_t                      bd_phys;
    unsigned int                    pc_from_device, pc_to_device;
    unsigned int                    device_to_device;
    unsigned int                    pc_to_pc;
    unsigned long                   flags;
    dma_addr_t                      per_address, per_address2;
    unsigned long                   event_mask[2];
    unsigned long                   watermark_level;
    uint32_t                             shp_addr, per_addr;
    enum dma_status                 status;
    unsigned int                    chn_count;
    unsigned int                    chn_real_count;

    pfn_dma_callback_t              fn_callback;            //!<�ص�����
    void                           *param;                  //!<���ûص������Ĳ���

    AW_MUTEX_DECL(chan_lock);
};

struct sdma_driver_data {
    int chnenbl0;
    int num_events;
    struct sdma_script_start_addrs  *script_addrs;
};

#define     DMA_INTR_TASK_STACK_SIZE        512
/**
 * \brief SDMA �豸�ṹ��
 */
typedef struct sdma_engine {
    struct awbl_dev                 dev;
    struct sdma_channel             channel[MAX_DMA_CHANNELS];
    struct sdma_channel_control     *channel_control;
    void                            *regs;
    struct sdma_context_data        *context;
    dma_addr_t                      context_phys;

    uint32_t                             script_number;
    struct sdma_script_start_addrs  *script_addrs;

    const struct sdma_driver_data   *drvdata;
    int                             inum;
    AW_TASK_DECL(dma_intr_task,DMA_INTR_TASK_STACK_SIZE);   //!< DMA�ж�����
    AW_SEMB_DECL(dma_intr_semb);                            //!< �����Ƿ���dma�жϲ���

    AW_MUTEX_DECL(dev_lock);
    struct awbl_imx6ul_sdma_service sdma_serv;

} awbl_imx6ul_sdma_dev_t;

/**
 * \brief imx6ul SDMA  �豸��Ϣ
 */
typedef struct awbl_imx6ul_sdma_devinfo {

    /** \brief APBH �Ĵ�������ַ */
    uint32_t regbase;

    /** \brief SDMA �жϺ� */
    uint32_t inum;

    /** \brief ƽ̨��س�ʼ�� */
    void (*pfunc_plfm_init)(void);
} awbl_imx6ul_dma_devinfo_t;






#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


