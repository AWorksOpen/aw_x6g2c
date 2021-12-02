/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX6UL UART ����
 *
 * iMX6UL UART Ϊ iMX6UL ϵ��MCU��Ƭ��ͨ���첽�շ���������8��UART
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "imx6ul_uart"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_imx6ul_uart_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-9-9, sni, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_UART_H
#define __AWBL_IMX6UL_UART_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "aw_sem.h"
#include "aw_serial.h"
#include "awbl_sio.h"
#include "driver/serial/awbl_serial_private.h"
/**
 * \addtogroup grp_awbl_drv_imx6ul_uart IMX6UL UART����
 * \copydetails awbl_imx6ul_uart.h
 * @{
 */

/** \brief imx6ul UART ������ */
#define AWBL_IMX6UL_UART_NAME   "imx6ul_uart"

/**
 * \defgroup grp_awbl_drv_imx6ul_uart_regs IMX6UL UART�Ĵ�������
 * @{
 */

#pragma pack(push)
#pragma pack(1)

/** \brief imx6ul UART�Ĵ��� */
typedef struct
{
    uint32_t    URXD;           //!< UART Receiver Register
    uint32_t    _reserved0[15];
    uint32_t    UTXD;          //!< UART Transmitter Register
    uint32_t    _reserved1[15];
    uint32_t    UCR1;           //!< UART Control Register 1
    uint32_t    UCR2;           //!< UART Control Register 2
    uint32_t    UCR3;           //!< UART Control Register 3
    uint32_t    UCR4;           //!< UART Control Register 4
    uint32_t    UFCR;           //!< UART FIFO Control Register
    uint32_t    USR1;           //!< UART Status Register 1
    uint32_t    USR2;           //!< UART Status Register 2
    uint32_t    UESC;           //!< UART Escape Character Register
    uint32_t    UTIM;           //!< UART Escape Timer Register
    uint32_t    UBIR;           //!< UART BRM Incremental Register
    uint32_t    UBMR;           //!< UART BRM Modulator Register
    uint32_t    UBRC;           //!< UART Baud Rate Count Register
    uint32_t    ONEMS;          //!< UART One Millisecond Register
    uint32_t    UTS;            //!< UART Test Register
    uint32_t    UMCR;           //!< UART RS-485 Mode Control Register
} imx6ul_uart_regs_t;


#pragma pack(pop)

/** @} grp_awbl_drv_imx6ul_uart_regs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_uart_micro_defs IMX6UL UART�����궨��
 * @{
 */


/** @} grp_awbl_drv_imx6ul_uart_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_uart_structs IMX6UL UART������صĽṹ
 * @{
 */

/** \brief imx6ul uartƽ̨��ʼ������ָ�� */
typedef void (*pfn_uarts_plat_init_t)(void);

/** \brief imx6ul UART �豸��Ϣ */
typedef struct awbl_imx6ul_uart_chan_param {
    int             vector;        /**< \brief ƽ̨��ص��жϺ� */
    uint32_t        base_addr;     /**< \brief UART�Ĵ�������ַ�������ַ��*/
    aw_clk_id_t     ref_clk_id;    /**< \brief ����ʱ�ӵ�ID */
    char           *recv_buf;      /**< \brief receive buffer base */
    uint32_t        recv_size;     /**< \brief receive buffer size */
    uint32_t        init_baudrate; /**< \brief baud init */

    /** The RS485 Dir function */
    void (*pfn_rs485_dir) (uint8_t dir);
    pfn_uarts_plat_init_t       pfunc_plat_init;  /**< \brief ƽ̨��صĳ�ʼ������ */

} awbl_imx6ul_uart_chan_param_t;


/** \brief mm32f103 uart dev. one dev may contain multi chan.  */
struct awbl_imx6ul_uart_dev {

    /** \brief awbl device data */
    struct awbl_dev              dev;

    /** \brief aw serial data */
    struct awbl_serial           serial;

    AW_SEMB_DECL(w_semb);

    AW_SEMB_DECL(w_485semb);
};


/** @} grp_awbl_drv_imx6ul_uart_structs*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_uart_awbus_interface Awbus�ӿ�
 * @{
 */
/** \brief �û����ô˺���ע��UART���� */
void awbl_imx6ul_uart_drv_register (void);

/** @} grp_awbl_drv_imx6ul_uart_awbus_interface */
/** @} grp_awbl_drv_imx6ul_uart*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
