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
#ifndef __AWBL_USBH_CDC_SERIAL_H
#define __AWBL_USBH_CDC_SERIAL_H
#include "apollo.h"
#include "host/class/cdc/serial/usbh_cdc_serial.h"

void *awbl_usbh_serial_open(int pid, int vid, int intfnum);
void awbl_usbh_serial_close (void *handle);
aw_err_t aw_usb_serial_dcb_get (void *handle, struct usb_serial_dcb *p_dcb);
aw_err_t aw_usb_serial_dcb_set (void *handle, struct usb_serial_dcb *p_dcb);
int aw_usb_serial_write_sync (void *handle, void *p_buffer, size_t nbytes);
int aw_usb_serial_read_sync (void *handle, void *p_buffer, size_t nbytes);
int aw_usb_serial_read_async (void *handle, void *p_buffer, size_t nbytes);
aw_err_t aw_usb_serial_timeout_get (void *handle, uint8_t dir, uint32_t *timeout);
//aw_err_t aw_usb_serial_timeout_set (void *handle, uint8_t dir, uint32_t timeout);
aw_err_t aw_usb_serial_timeout_set (void *handle, uint32_t *timeout);
aw_err_t aw_usb_serial_tranfersize_get (void *handle, uint8_t dir, uint32_t *tranfer_size);
aw_err_t aw_usb_serial_tranfersize_set (void *handle, uint8_t dir, uint32_t tranfer_size);

#endif


