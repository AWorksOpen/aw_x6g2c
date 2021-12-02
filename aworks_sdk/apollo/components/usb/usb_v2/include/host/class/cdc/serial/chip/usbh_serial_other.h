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
#ifndef __USBH_SERIAL_OTHER_H
#define __USBH_SERIAL_OTHER_H
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

/**
 * 初始化
 *
 * @param p_userial USB串口结构体
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_other_init(struct usbh_serial *p_userial);
/**
 * 移除函数
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_other_uninit(struct usbh_serial *p_userial);
#endif
