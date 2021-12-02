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
 * \brief Freescale i.MX6ULϵ��I2C��������
 *
 * \internal
 * \par modification history:
 * - 1.00 16-03-31  hgo, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_I2C_H
#define __AWBL_IMX6UL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_i2cbus.h"

#define AWBL_IMX6UL_I2C_NAME            "imx6ul_i2c"

/**
 * \brief imx6ul I2C �豸��Ϣ
 */
struct awbl_imx6ul_i2c_devinfo {

    /** \brief �̳��� AWBus I2C �������豸��Ϣ */
    struct awbl_i2c_master_devinfo i2c_master_devinfo;

    uint32_t    regbase;                /**< �Ĵ�������ַ */
    int         inum;                   /**< �жϺ� */
    aw_clk_id_t ref_clk_id;             /**< ����ʱ��ID */
    void (*pfunc_plfm_init)(void);      /**< ƽ̨��س�ʼ��������ʱ�ӡ���ʼ������ */
};


/**
 * \brief imx6ul I2C �豸ʵ��
 */
struct awbl_imx6ul_i2c_dev {

    /** \brief �̳��� AWBus I2C ���������� */
    struct awbl_i2c_master  super;

    /** \brief �������ݼ��� */
    size_t      data_ptr;

    /** \brief ����ͬ������������ź��� */
    AW_SEMC_DECL(i2c_sem);

    /* dir Ϊ0 ��p_buf��дbuf�� Ϊ1��p_bufΪ��buf */
    uint8_t    dir;

    /* ��䳬ʱʱ��ms */
    uint32_t    timeout;

    /* ��¼��������жϵ�tick�� */
    uint32_t    late_tick;

    /** \brief �ڿ�ʼ�µĴ���ǰ������Ҫ��С���ֿ��е�ʱ��(us) */
    uint32_t    bus_idle_time;

    /* ���ݸ��жϵ�дbuffer����  */
    struct awbl_trans_buf  *p_trans_buf;

    /* ���ݸ��жϵ�дbuffer���� */
    uint32_t                trans_buf_num;
};

/**
 * \brief imx6ul I2C master driver register
 */
void awbl_imx6ul_i2c_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_IMX6UL_I2C_H_ */
