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
 * \brief AWBus-lite iMX6UL UART 驱动
 *
 * iMX6UL UART 为 iMX6UL 系列MCU的片上通用异步收发器，共用8个UART
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "imx6ul_uart"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_imx6ul_uart_devinfo
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
 * \addtogroup grp_awbl_drv_imx6ul_uart IMX6UL UART驱动
 * \copydetails awbl_imx6ul_uart.h
 * @{
 */

/** \brief imx6ul UART 驱动名 */
#define AWBL_IMX6UL_UART_NAME   "imx6ul_uart"

/**
 * \defgroup grp_awbl_drv_imx6ul_uart_regs IMX6UL UART寄存器定义
 * @{
 */

#pragma pack(push)
#pragma pack(1)

/** \brief imx6ul UART寄存器 */
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
 * \defgroup grp_awbl_drv_imx6ul_uart_micro_defs IMX6UL UART驱动宏定义
 * @{
 */


/** @} grp_awbl_drv_imx6ul_uart_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_uart_structs IMX6UL UART驱动相关的结构
 * @{
 */

/** \brief imx6ul uart平台初始化函数指针 */
typedef void (*pfn_uarts_plat_init_t)(void);

/** \brief imx6ul UART 设备信息 */
typedef struct awbl_imx6ul_uart_chan_param {
    int             vector;        /**< \brief 平台相关的中断号 */
    uint32_t        base_addr;     /**< \brief UART寄存器基地址（虚拟地址）*/
    aw_clk_id_t     ref_clk_id;    /**< \brief 输入时钟的ID */
    char           *recv_buf;      /**< \brief receive buffer base */
    uint32_t        recv_size;     /**< \brief receive buffer size */
    uint32_t        init_baudrate; /**< \brief baud init */

    /** The RS485 Dir function */
    void (*pfn_rs485_dir) (uint8_t dir);
    pfn_uarts_plat_init_t       pfunc_plat_init;  /**< \brief 平台相关的初始化函数 */

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
 * \addtogroup grp_awbl_drv_imx6ul_uart_awbus_interface Awbus接口
 * @{
 */
/** \brief 用户调用此函数注册UART驱动 */
void awbl_imx6ul_uart_drv_register (void);

/** @} grp_awbl_drv_imx6ul_uart_awbus_interface */
/** @} grp_awbl_drv_imx6ul_uart*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
