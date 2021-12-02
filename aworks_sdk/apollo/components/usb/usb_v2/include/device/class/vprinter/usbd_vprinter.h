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
#ifndef __USBD_VPRINTER_H
#define __USBD_VPRINTER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"
#include "device/core/usbd.h"
#include "device/core/usbd_obj.h"
#include <string.h>
#include <stdio.h>

/** USB�豸�����ӡ����ӡ����*/
#define __USBD_PRINTER_TRACE(info, ...) \
            do { \
                usb_printf("USBD-PRINTER:"info, ##__VA_ARGS__); \
            } while (0)


#define USB_GET_DEVICE_ID       0
#define USB_GET_PORT_STATUS     1
#define USB_SOFT_RESET          2

#define USB_PRINTER_NOT_ERROR     0x08
#define USB_PRINTER_SELECTED      0x10
#define USB_PRINTER_PAPER_EMPTY   0x20

#define USB_PNP_STRING_LEN        1024


/** USB�豸�����ӡ���ṹ��*/
typedef struct usbd_virtual_printer {
    struct usbd_obj_fun      fun;
    struct usbd_obj_pipe     in;              /**\brief ����˵�*/
    struct usbd_obj_pipe     out;             /**\brief ����˵�*/
    char                     name[32];
    uint8_t                  intf_num;        /**\brief �ӿڱ��*/

    usb_mutex_handle_t       mutex;
    usb_sem_handle_t         semb;            /**\brief �ź���*/
    struct usb_list_head     node;            /**\brief ��ǰ�豸�ڵ�*/
    char                     pnp_string[USB_PNP_STRING_LEN];
    uint8_t                  printer_status;  /**\brief ��ӡ��״̬*/
}usbd_vp_t;

/**
 * USB�豸�����ӡ��ͬ��������
 *
 * @param p_vp      USB�豸�����ӡ��ʵ��
 * @param p_buf     Ҫ���յ����ݻ���
 * @param nbytes    Ҫ���յ����ݳ���
 * @param timeout   ��ʱʱ��
 *
 * @return �ɹ�����ʵ�ʴ����ֽ���
 */
usb_err_t usbd_cdc_vp_read_sync(usbd_vp_t   *p_vp,
                                void        *p_buf,
                                size_t       nbytes,
                                int          timeout);
/**
 * ����һ����ӡ���豸
 *
 * @param p_obj     USB�豸ʵ��
 * @param p_printer USB��ӡ���豸ʵ��
 * @param p_name    USB��ӡ���豸����
 *
 * @return ������ӡ���豸�ɹ�����USB_OK
 */
usb_err_t usbd_printer_create(struct usbd_obj *p_obj,
                              usbd_vp_t       *p_printer,
                              const char      *p_name);
/**
 * ����һ����ӡ���豸
 *
 * @param p_obj     USB�豸ʵ��
 * @param p_printer USB��ӡ���豸ʵ��
 *
 * @return ���ٴ�ӡ���豸�ɹ�����USB_OK
 */
usb_err_t usbd_printer_destory(struct usbd_obj *p_obj, usbd_vp_t *p_printer);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __USBD_VPRINTER_H */

