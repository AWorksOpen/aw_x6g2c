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
#ifndef __USB_SYS_DEV_H
#define __USB_SYS_DEV_H
#include "host/core/usbh_dev.h"
#include "usb_errno.h"

/** �ⲿ�����豸����*/
usb_err_t sys_dev_create(struct usbh_function *p_usb_func);
/** �ⲿ�����豸����*/
usb_err_t sys_dev_destory(struct usbh_function *p_usb_func);


#endif
