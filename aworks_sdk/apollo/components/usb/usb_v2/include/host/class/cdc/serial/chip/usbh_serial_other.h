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
 * ��ʼ��
 *
 * @param p_userial USB���ڽṹ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_other_init(struct usbh_serial *p_userial);
/**
 * �Ƴ�����
 *
 * @param p_userial USBת�����豸
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_other_uninit(struct usbh_serial *p_userial);
#endif
