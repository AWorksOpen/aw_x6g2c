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
 * \brief Ti KeyStone Architecture SPI ����ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 14-04-01  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_ECSPI_H
#define __AWBL_IMX6UL_ECSPI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_spibus.h"
#include "aw_sem.h"

#define AWBL_IMX6UL_ECSPI_NAME   "imx6ul_ecspi"


/**
 * \brief KEYSTONE SPI �豸��Ϣ
 */
typedef struct awbl_imx6ul_ecspi_devinfo {

    /** \brief �̳��� AWBus SPI �������豸��Ϣ */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t         regbase;         /**< \brief �Ĵ�������ַ */
    int              inum;            /**< \brief �жϺ� */
    uint8_t          cs_channel;      /**< \brief channel0 ~ channel3 */
    uint8_t          cs_delay;        /**< \brief Ƭѡ��ʱ */
    uint8_t          delay_clksrc;    /**< \brief (Ƭѡ������)��ʱʱ��ѡ��0 - SPI Clock��1 - 32.768KHz */
    uint16_t         trans_delay;     /**< \brief �������ʱ */
    
    aw_clk_id_t         ref_clk_id;     /**< \brief the clock which the counter use */
    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

} awbl_imx6ul_ecspi_devinfo_t;

/**
 * \brief KEYSTONE SPI �豸ʵ��
 */
typedef struct awbl_imx6ul_ecspi_dev {

    /** \brief �̳��� AWBus SPI ���������� */
    struct awbl_spi_master  super;

    /** \brief ����ͬ������������ź��� */
    AW_SEMB_DECL(ecspi_sem_sync);

    uint32_t write_index;
    uint32_t read_nbytes;
    uint32_t    nbytes_to_recv; /**< /brief �����յ��ֽ��� */
    const void *p_write_buf;
    void *p_read_buf;
    void (*bpw_func) (void* p_arg);

} awbl_imx6ul_ecspi_dev_t;

/**
 * \brief KEYSTONE SPI master driver register
 */
void awbl_imx6ul_ecspi_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX6UL_ECSPI_H */

/* end of file */
