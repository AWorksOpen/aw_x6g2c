/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __USBH_SERIAL_CH34x_H
#define __USBH_SERIAL_CH34x_H

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

/*******************************/
/* 波特率计算因子*/
/*******************************/
#define CH34x_BAUDBASE_FACTOR 1532620800
#define CH34x_BAUDBASE_DIVMAX 3

#define CH34x_BIT_RTS (1 << 6)
#define CH34x_BIT_DTR (1 << 5)

#define CH34x_BITS_MODEM_STAT 0x0f /* 所有位*/

/* CH34x串口结构体*/
struct usbh_serial_ch34x{
    struct usbh_function *p_fun; /* 相应的功能设备结构体*/
    usb_mutex_handle_t    lock;  /* 自旋锁*/
    uint8_t line_control;        /* 设置 line control value RTS/DTR */
    uint8_t line_status;         /* modem 控制输入的激活的位*/
};

/**
 * USB转串口ch34x芯片初始化
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_ch34x_init(struct usbh_serial *p_userial);
/**
 * CH34x 移除函数
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_ch34x_uninit(struct usbh_serial *p_userial);

#endif
