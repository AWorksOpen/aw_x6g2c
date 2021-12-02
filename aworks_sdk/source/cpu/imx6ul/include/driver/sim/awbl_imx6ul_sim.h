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
 * \brief AWBus IMX6UL SIM driver
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_sim.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-12  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_SIM_H
#define __AWBL_IMX6UL_SIM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "awbl_sim.h"
#include "aw_spinlock.h"

/** \brief imx6ul sim ������ */
#define AWBL_IMX6UL_SIM_NAME          "imx6ul_sim"
#define __IMX6UL_SIM_TASK_STACK_SIZE  4096

/**
 * \brief imx6ul sim�豸��Ϣ
 */
typedef struct awbl_imx6ul_sim_devinfo {

    /** \brief SIM����������Ϣ */
    struct awbl_sim_servinfo servinfo;

    uint32_t         regbase;         /**< \brief �Ĵ�������ַ */
    int              inum;            /**< \brief �жϺ� */
    aw_clk_id_t      ref_clk_id;      /**< \brief the clock which the sim use to provide io clock*/
    uint8_t          port_index;      /**< \brief �˿����� */
    aw_bool_t           sven_low_active; /**< \brief ����SVEN���ŵ�״̬ */

    uint8_t  *p_rcv_buffer;
    uint32_t  rcv_buffer_size;
    uint8_t  *p_xmt_buffer;
    uint32_t  xmt_buffer_size;

    uint32_t  task_trans_prio;        /**< �����������ȼ� */

    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init) (void);

} awbl_imx6ul_sim_devinfo_t;

/**
 * \brief imx6ul sim�豸�ṹ�嶨��
 */
typedef struct awbl_imx6ul_sim_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev dev;

    /** \brief �������� */
    aw_spinlock_isr_t dev_lock;

    /** \brief ��Ƭ����ṹ�� */
    awbl_sim_service_t serv;

    /** \brief �������ź��� */
    AW_SEMB_DECL(semb);

    int clk_freq;            /**< \brief ʱ��Ƶ�� */

    int present;             /**< \brief */
    int state;               /**< \brief SIM������״̬ */
    int error;

    awbl_sim_timing_t timing_data;
    awbl_sim_baud_t   baud_rate;

    int      timeout;
    uint8_t  nack_threshold;
    aw_bool_t   nack_enable;
    uint32_t expected_rcv_cnt;
    uint8_t  is_fixed_len_rec;

    uint8_t td_x;
    uint8_t td;
    uint8_t history;
    uint8_t th;
    aw_bool_t  td_also;

    /** \brief remaining bytes to transmit for the current transfer */
    uint32_t xmt_remaining;

    /** \brief transmit position */
    uint32_t xmt_pos;

    /** \brief receive position / number of bytes received */
    uint32_t rcv_count;

//    bool_t   last_is_tx;
    uint16_t rcv_head;
    uint32_t port_detect_reg;
    uint32_t port_ctrl_reg;

    uint32_t quirks;
    uint8_t  checking_ts_timing;

    int tx_status;
    int rx_status;

    AW_TASK_DECL(sim_trans_task_decl, __IMX6UL_SIM_TASK_STACK_SIZE);

    /** \brief ����ͬ������������ź��� */
    AW_SEMB_DECL(sim_sem_sync);

} awbl_imx6ul_sim_dev_t;

/**
 * \brief imx6ul sim driver register
 */
void awbl_imx6ul_sim_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_SIM_H */

/* end of file */
