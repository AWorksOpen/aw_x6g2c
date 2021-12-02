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
#ifndef __USBH_SERIAL_CP210x_H
#define __USBH_SERIAL_CP210x_H
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

/* 配置请求类型*/
#define REQTYPE_HOST_TO_INTERFACE   0x41
#define REQTYPE_INTERFACE_TO_HOST   0xc1

#define CP210X_SET_LINE_CTL 0x03     /* 设置配置*/
#define CP210X_GET_LINE_CTL 0x04     /* 获取配置*/
#define CP210X_SET_FLOW     0x13     /* 设置控制流*/
#define CP210X_GET_FLOW     0x14     /* 获取控制流*/
#define CP210X_SET_BAUDRATE 0x1E     /* 设置波特率*/

/* CP210X_(设置|获取)_配置 */
/* 数据位*/
#define BITS_DATA_MASK  0X0f00
#define BITS_DATA_5     0X0500
#define BITS_DATA_6     0X0600
#define BITS_DATA_7     0X0700
#define BITS_DATA_8     0X0800
#define BITS_DATA_9     0X0900
/* 数据校验*/
#define BITS_PARITY_MASK    0x00f0
#define BITS_PARITY_NONE    0x0000
#define BITS_PARITY_ODD     0x0010
#define BITS_PARITY_EVEN    0x0020
#define BITS_PARITY_MARK    0x0030
#define BITS_PARITY_SPACE   0x0040
/* 停止位*/
#define BITS_STOP_MASK      0x000f
#define BITS_STOP_1     0x0000
#define BITS_STOP_1_5       0x0001
#define BITS_STOP_2     0x0002

/* cp210x串口结构体*/
struct usbh_serial_cp210x{
    struct usbh_function *p_fun; /* 相应的功能设备结构体*/
};

/**
 * USB转串口cp210x芯片初始化
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_cp210x_init(struct usbh_serial *p_userial);
/**
 * CP210x 移除函数
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_cp210x_uninit(struct usbh_serial *p_userial);
#endif
