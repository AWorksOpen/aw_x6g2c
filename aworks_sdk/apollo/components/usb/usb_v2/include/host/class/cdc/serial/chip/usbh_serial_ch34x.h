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
/* �����ʼ�������*/
/*******************************/
#define CH34x_BAUDBASE_FACTOR 1532620800
#define CH34x_BAUDBASE_DIVMAX 3

#define CH34x_BIT_RTS (1 << 6)
#define CH34x_BIT_DTR (1 << 5)

#define CH34x_BITS_MODEM_STAT 0x0f /* ����λ*/

/* CH34x���ڽṹ��*/
struct usbh_serial_ch34x{
    struct usbh_function *p_fun; /* ��Ӧ�Ĺ����豸�ṹ��*/
    usb_mutex_handle_t    lock;  /* ������*/
    uint8_t line_control;        /* ���� line control value RTS/DTR */
    uint8_t line_status;         /* modem ��������ļ����λ*/
};

/**
 * USBת����ch34xоƬ��ʼ��
 *
 * @param p_userial USBת�����豸
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_ch34x_init(struct usbh_serial *p_userial);
/**
 * CH34x �Ƴ�����
 *
 * @param p_userial USBת�����豸
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_ch34x_uninit(struct usbh_serial *p_userial);

#endif
