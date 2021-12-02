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
 * \brief IMX6UL QUAD SPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-23  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_QSPI_H
#define __AWBL_IMX6UL_QSPI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "awbl_spibus.h"
#include "aw_sem.h"

#include "awbl_imx6ul_qspi_op.h"

#define AWBL_IMX6UL_QSPI_NAME   "qspi-flash"

/**
 * \brief KEYSTONE SPI �豸��Ϣ
 */
typedef struct awbl_imx6ul_qspi_devinfo {

    /** \brief �̳��� AWBus SPI �������豸��Ϣ */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t regbase;               /**< \brief �Ĵ�������ַ */
    int      inum;                  /**< \brief �жϺ� */
    uint32_t in_freq;               /**< \brief ����ʱ��Ƶ�� */

    uint32_t bank_memmap_phy[5];    /**< \brief �ڴ�ӳ���ַ��������Щ��ַ����ֱ�ӷ���FLASH */
    uint8_t  bank_id;               /**< \brief BANK ID */

    uint8_t  addr_width;            /**< \brief ��ַ�����3��ʾ24λ��4��ʾ32λ */

    awbl_qspi_read_mode_t read_mode;
    uint8_t               read_opcode;

    uint8_t  program_opcode;
    uint8_t  erase_opcode;
    uint8_t  read_dummy;

    uint8_t  ddr_smp;               /**< \brief DDR mode sample point number */

    /** ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

} awbl_imx6ul_qspi_devinfo_t;

/**
 * \brief KEYSTONE SPI �豸ʵ��
 */
typedef struct awbl_imx6ul_qspi_dev {

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

    uint8_t     op;
    uint32_t    addr;

    aw_bool_t      is_cmd;
} awbl_imx6ul_qspi_dev_t;

/**
 * \brief KEYSTONE SPI master driver register
 */
void awbl_imx6ul_qspi_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX6UL_QSPI_H */

/* end of file */
