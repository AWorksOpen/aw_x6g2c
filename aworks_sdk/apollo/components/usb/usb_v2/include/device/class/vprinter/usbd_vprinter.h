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

/** USB设备虚拟打印机打印函数*/
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


/** USB设备虚拟打印机结构体*/
typedef struct usbd_virtual_printer {
    struct usbd_obj_fun      fun;
    struct usbd_obj_pipe     in;              /**\brief 输入端点*/
    struct usbd_obj_pipe     out;             /**\brief 输出端点*/
    char                     name[32];
    uint8_t                  intf_num;        /**\brief 接口编号*/

    usb_mutex_handle_t       mutex;
    usb_sem_handle_t         semb;            /**\brief 信号量*/
    struct usb_list_head     node;            /**\brief 当前设备节点*/
    char                     pnp_string[USB_PNP_STRING_LEN];
    uint8_t                  printer_status;  /**\brief 打印机状态*/
}usbd_vp_t;

/**
 * USB设备虚拟打印机同步读数据
 *
 * @param p_vp      USB设备虚拟打印机实例
 * @param p_buf     要接收的数据缓存
 * @param nbytes    要接收的数据长度
 * @param timeout   超时时间
 *
 * @return 成功返回实际传输字节数
 */
usb_err_t usbd_cdc_vp_read_sync(usbd_vp_t   *p_vp,
                                void        *p_buf,
                                size_t       nbytes,
                                int          timeout);
/**
 * 创建一个打印机设备
 *
 * @param p_obj     USB设备实例
 * @param p_printer USB打印机设备实例
 * @param p_name    USB打印机设备名字
 *
 * @return 创建打印机设备成功返回USB_OK
 */
usb_err_t usbd_printer_create(struct usbd_obj *p_obj,
                              usbd_vp_t       *p_printer,
                              const char      *p_name);
/**
 * 销毁一个打印机设备
 *
 * @param p_obj     USB设备实例
 * @param p_printer USB打印机设备实例
 *
 * @return 销毁打印机设备成功返回USB_OK
 */
usb_err_t usbd_printer_destory(struct usbd_obj *p_obj, usbd_vp_t *p_printer);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __USBD_VPRINTER_H */

