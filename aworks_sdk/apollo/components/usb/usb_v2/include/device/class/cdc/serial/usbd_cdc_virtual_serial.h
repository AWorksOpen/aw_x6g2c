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


#ifndef __USBD_CDC_VIRTUAL_SERIAL_H
#define __USBD_CDC_VIRTUAL_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"
#include "rngbuf/usb_rngbuf.h"
#include "device/core/usbd.h"
#include "device/core/usbd_obj.h"
#include <string.h>
#include <stdio.h>

/* 打印函数*/
#define __USBD_CDC_VS_TRACE(info, ...) \
            do { \
                usb_printf("USBD-VS:"info, ##__VA_ARGS__); \
            } while (0)

#define USBD_CDC_SERIAL_TASK_SIZE         1024     /* 数据接收任务堆栈 */
//#define USBD_CDC_PASS_BACK                     /* 是否开启CDC回传--定义则开启 */

/* 接收/发送缓存大小*/
#define USBD_CDC_RX_RNGBUF_SIZE      (1024)
#define USBD_CDC_TX_RNGBUF_SIZE      (1024)

/* CDC数据接收/发送交换缓冲区字节大小 */
#define USBD_CDC_RX_SWAP_SIZE        (1024)
#define USBD_CDC_TX_SWAP_SIZE        (1024)

enum{
    USBD_CDC_VS_TRANFER_ERROR = 0,
    USBD_CDC_VS_TRANFER_SYNC,
    USBD_CDC_VS_TRANFER_ASYNC
};

/* cdc串口参数结构体 line_coding */
typedef struct usbd_cdc_line_coding {
  uint32_t  baud_rate;
  uint8_t   stop_bits;
  uint8_t   parity;
  uint8_t   word_length;
} __attribute__ ((packed)) usbd_cdc_line_coding_t;

/* USB设备CDC 虚拟串口结构体 */
typedef struct usbd_cdc_vs {
    struct usbd_obj_fun          cfun;          /* 命令接口*/
    struct usbd_obj_pipe         intr;          /* 中断管道*/
    struct usbd_obj_fun          dfun;          /* 数据接口*/
    struct usbd_obj_pipe         in;            /* 输入管道*/
    struct usbd_obj_pipe         out;           /* 输出管道*/
    uint8_t                      id;            /* 设备id编号 */
    char                         name[32];      /* 虚拟串口名字*/
    usbd_cdc_line_coding_t       line_control;  /* 串口配置*/
    usb_mutex_handle_t           mutex;         /* 设备互斥锁*/
    struct usb_list_head         node;          /* 设备节点 */
    char                        *p_rbuf;        /* 接收缓存*/
    usb_sem_handle_t             sync_semb;     /* 信号量*/
    usb_task_handle_t            rx_task;       /* 接收任务*/
    struct usb_rngbuf            r_rngbuf;      /* 环形缓冲区*/
    uint8_t                      tx_mode;       /* 发送模式*/
    uint8_t                      rx_mode;       /* 接收模式*/

    int                        (*pfn_rx_callback)(void *p_buf, int len, void *p_arg);
    void                        *p_rxcb_arg;
}usbd_cdc_vs_t;


/**
 * 设置虚拟串口接收模式
 *
 * @param p_vs USB虚拟串口结构体
 * @param mode 接收模式(同步或异步)
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_cdc_vs_set_rxmode(struct usbd_cdc_vs *p_vs, uint8_t mode);
/**
 * 设置虚拟串口发送模式
 *
 * @param p_vs USB虚拟串口结构体
 * @param mode 接收模式(同步或异步)
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_cdc_vs_set_txmode(struct usbd_cdc_vs *p_vs, uint8_t mode);
/** 检查接收模式是否是异步模式*/
usb_bool_t usbd_cdc_vs_rx_is_async(struct usbd_cdc_vs *p_vs);
/** 检查发送模式是否是异步模式*/
usb_bool_t usbd_cdc_vs_tx_is_async(struct usbd_cdc_vs *p_vs);
/**
 * 创建一个虚拟串口
 *
 * @param p_obj    USB设备实例
 * @param p_vs     USB虚拟串口结构体
 * @param id       虚拟串口ID
 * @param p_name   虚拟串口名字
 * @param buf_size 缓存大小
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_cdc_vs_create (struct usbd_obj    *p_obj,
                              struct usbd_cdc_vs *p_vs,
                              uint8_t             id,
                              const char         *p_name,
                              int                 buf_size);
/** 销毁虚拟串口设备*/
usb_err_t usbd_cdc_vs_destroy (struct usbd_cdc_vs *p_vs);
/**
 * USB设备异步写数据
 *
 * @param p_vs    USB设备虚拟串口结构体
 * @param p_trans USB设备传输结构体
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_cdc_vs_write_async (struct usbd_cdc_vs     *p_vs,
                                   struct usbd_obj_trans  *p_trans);
/** 异步读 */
usb_err_t usbd_cdc_vs_read_async (struct usbd_cdc_vs  *p_vs,
                                  void                *p_buf,
                                  size_t               nbytes,
                                  size_t              *p_act_len);
/**
 * USB设备同步写数据
 *
 * @param p_vs     USB设备虚拟串口结构体
 * @param p_buffer 要发送的数据缓存
 * @param nbytes   要发送的数据长度
 * @param timeout  超时时间
 *
 * @return 传输成功返回实际传输的字节数，失败返回错误代码
 */
usb_err_t usbd_cdc_vs_write_sync (struct usbd_cdc_vs *p_vs,
                                  void               *p_buffer,
                                  size_t              nbytes,
                                  int                 timeout);
/**
 * USB设备同步读数据
 *
 * @param p_vs    USB设备虚拟串口结构体
 * @param p_buf   要接收的数据缓存
 * @param nbytes  要接收的数据长度
 * @param timeout 超时时间
 *
 * @return 传输成功返回实际传输的字节数，失败返回错误代码
 */
usb_err_t usbd_cdc_vs_read_sync (struct usbd_cdc_vs  *p_vs,
                                 void                *p_buf,
                                 size_t               nbytes,
                                 int                  timeout);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */

