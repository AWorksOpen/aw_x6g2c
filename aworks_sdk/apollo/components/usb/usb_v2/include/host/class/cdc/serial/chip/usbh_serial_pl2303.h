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
#ifndef __USBH_SERIAL_PL2303_H
#define __USBH_SERIAL_PL2303_H
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define VENDOR_WRITE_REQUEST_TYPE   0x40
#define VENDOR_WRITE_REQUEST        0x01
#define VENDOR_READ_REQUEST_TYPE    0xc0
#define VENDOR_READ_REQUEST         0x01

#define SET_CONTROL_REQUEST_TYPE    0x21
#define SET_CONTROL_REQUEST         0x22
#define SET_LINE_REQUEST_TYPE       0x21
#define SET_LINE_REQUEST            0x20
#define GET_LINE_REQUEST_TYPE       0xa1
#define GET_LINE_REQUEST            0x21

#define CONTROL_DTR         0x01
#define CONTROL_RTS         0x02

/* pl2303芯片类型*/
enum pl2303_type {
    TYPE_01,    /* 类型 0 和 1 (区别未知) */
    TYPE_HX,    /* pl2303芯片的 HX 版本*/
    TYPE_COUNT
};

/* pl2303类型数据*/
struct pl2303_type_data {
    uint32_t max_baud_rate;
    unsigned long quirks;
};

/* pl2303串口私有数据*/
struct pl2303_serial_private {
    const struct pl2303_type_data *type;
    unsigned long                  quirks;
};

/* PL2303 串口结构体*/
struct usbh_serial_pl2303{
    struct usbh_function         *p_fun;         /* 相应的功能设备结构体*/
    usb_mutex_handle_t            lock;          /* 互斥锁*/
    struct pl2303_serial_private *priv;          /* pl2303串口私有数据*/
    uint8_t                       line_settings[7];
    uint8_t                       line_control;
};

/**
 * USB转串口pl2303芯片初始化
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_pl2303_init(struct usbh_serial *p_userial);
/**
 * pl2303 移除函数
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_pl2303_uninit(struct usbh_serial *p_userial);
#endif
