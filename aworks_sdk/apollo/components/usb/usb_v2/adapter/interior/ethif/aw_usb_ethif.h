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
#ifndef __AW_USB_ETHIF_H
#define __AW_USB_ETHIF_H

#include "usb_errno.h"

void* usb_ethif_create(char *p_name, uint8_t *p_hwaddr, int mtu, void* p_user_data);
usb_err_t usb_netif_is_link_up(void *p_nethif, usb_bool_t *p_up);
usb_err_t usb_netif_is_up(void *p_handle, usb_bool_t *p_up);
usb_err_t usb_netif_set_link_up(void *p_handle);
usb_err_t usb_netif_set_link_down(void *p_handle);
usb_err_t usb_netif_input(void *p_handle, void *data, uint32_t data_len);
usb_err_t usb_netif_remove(void *p_handle);
#endif
