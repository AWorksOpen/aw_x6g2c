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
#ifndef __USBH_CDC_SERIAL_H
#define __USBH_CDC_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "rngbuf/usb_rngbuf.h"

#define USB_SERIAL_DEFAULT_INTF -1

#define USB_SERIAL_PIPE_IN    0
#define USB_SERIAL_PIPE_OUT   1
#define USB_SERIAL_PIPE_BOTH  2

/* USB转串口使用的停止位数取值*/
#define USB_SERIAL_ONESTOPBIT     (0)    /** \brief 1位停止位  */
#define USB_SERIAL_ONE5STOPTS     (1)    /** \brief 1.5 位停止位  */
#define USB_SERIAL_TWOSTOPBITS    (2)    /** \brief 2   位停止位  */
#define USB_SERIAL_STOP_INVALID   (0x3)  /** \brief 3是无效停止位  */

/* USB转串口校验方法取值*/
#define USB_SERIAL_EVENPARITY     (0)    /** \breif 偶效验  */
#define USB_SERIAL_ODDPARITY      (1)    /** \breif 奇效验  */

/* USB转串口数据位*/
#define USB_SERIAL_CSIZE           0xc     /* bits 3 and 4 encode the character size */
#define USB_SERIAL_CS5             0x5     /* 5 bits */
#define USB_SERIAL_CS6             0x6     /* 6 bits */
#define USB_SERIAL_CS7             0x7     /* 7 bits */
#define USB_SERIAL_CS8             0x8     /* 8 bits */

/* USB转串口设备库结构体*/
struct usbh_serial_lib{
    struct usb_list_head    userial_lib_dev_list;   /* 管理所有USB转串口设备链表*/
    usb_bool_t              is_lib_init;            /* 是否初始化设备库*/
    usb_bool_t              is_lib_uniniting;       /* 是否移除设备库*/
    usb_mutex_handle_t      userial_lib_lock;       /* 互斥锁*/
    uint8_t                 userial_lib_dev_num;    /* 当前存在设备的数量*/
    int                     refcnt;                 /* 引用计数*/
    struct usbh_mem_record  userial_mem_record;     /* 内存记录*/
};

struct usbh_serial;
/* USB转串口设备信息结构体*/
struct usbh_serial_dev_info{
    uint16_t    pid;                           /* 设备pid*/
    uint16_t    vid;                           /* 设备vid*/
    char       *type;                          /* 设备类型*/
    usb_err_t (*init)(struct usbh_serial *);   /* 初始化函数*/
    usb_err_t (*uninit)(struct usbh_serial *); /* 反初始化函数*/
};

/* USB串口配置结构描述*/
struct usb_serial_dcb {
    uint32_t                baud_rate;           /* 波特率*/

    uint32_t                byte_size:4;         /* 指定当前端口使用的数据位(范围5~8)*/
    uint32_t                f_parity :1;         /* 是否允许奇偶校验,为0不允许校验，为1则校验方式看parity的值*/
    uint32_t                parity   :1;         /* 指定端口数据传输的校验方法*/
    uint32_t                stop_bits:2;         /* 指定端口当前使用的停止位数*/
    uint32_t                      dtr:1;         /* 数据终端准备*/
    uint32_t                      rts:1;         /* 请求发送*/
    uint32_t                  crtscts:1;         /* 流控制*/
    uint32_t                    ixoff:1;
    uint32_t                f_dummy:24;
};

/* USB串口管道结构体*/
struct usbh_serial_pipe {
    struct usbh_endpoint *pipe;
    void                 *p_buf;
    uint32_t              size;        /* 一次传输的数据大小*/
    uint32_t              timeout;     /* 发送/接收超时时间*/
    usb_mutex_handle_t    lock;        /* USB管道互斥锁*/
};

/* USB串口结构体*/
struct usbh_serial {
    char                       name[32];     /* USB串口设备名字*/
    struct usbh_function      *p_fun;        /* 相应的功能设备结构体*/
    uint8_t                    nports;       /* 端口数量*/
    void                      *p_drv_handle; /* 私有数据(放相关芯片类型的结构体)*/
    struct usbh_serial_pipe    p_in;         /* 输入管道*/
    struct usbh_serial_pipe    p_out;        /* 输出管道*/
    struct usbh_endpoint      *p_intr;       /* 中断端点*/
    struct usb_list_head       node;         /* 当前USB串口节点*/
    usb_mutex_handle_t         lock;         /* USB串口互斥锁*/
    int                        ref;          /* USB串口引用计数*/
    struct usb_serial_dcb      cur_dcb;      /* 当前的数据控制块*/
    usb_bool_t                 removed;      /* 移除状态*/
    usb_bool_t                 is_init;      /* 是否已经初始化完成*/

    struct usbh_trp           *rx_async_trp; /* 异步接收请求包*/
    void                      *rx_async_buf; /* 异步接收缓存*/
    /* 异步接收回调函数*/
    void                     (*rx_async_cb)(void *buf, int len);

    /* 同步读回调函数*/
    int (*read_sync) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* 同步读回调函数*/
    int (*read_async) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* 同步写回调函数*/
    int (*write_sync) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* 设置串口回调函数*/
    usb_err_t (*dcb_set) (struct usbh_serial *usb_serial, struct usb_serial_dcb *p_dcb);
    /* 读回调函数*/
    usb_err_t (*intr) (struct usbh_serial *usb_serial, int len);
};

/***************************************函数声明**************************************************/
/** 获取USB转串口驱动内存记录*/
usb_err_t userial_lib_mem_record_get(struct usbh_mem_record *mem_record);
/** USB转串口设备库相关初始化*/
usb_err_t usbh_serial_lib_init (void);
/** 反初始化USB转串口设备库*/
usb_err_t usbh_serial_lib_uninit(void);
/** 获取当前存在的USB转串口设备数量*/
int usbh_serial_dev_num_get(void);
/** 获取USB转串口设备的端口数量*/
int usbh_serial_dev_nport_get(struct usbh_serial *p_userial);
/** 创建一个USB转串口设备*/
struct usbh_serial *usbh_serial_dev_create(struct usbh_function *p_ufun);
/** 销毁USB转串口设备*/
usb_err_t usbh_serial_dev_destroy(struct usbh_serial *p_userial);
/**
 * 打开对应的USB串口
 *
 * @param pid     USB转串设备的厂品ID
 * @param vid     USB转串设备的供应商ID
 * @param intfnum 指定特定的接口(不需要则设置为-1)
 * @param idx 索引，指明要获取第几个USB转串口设备(最小为1)
 *
 * @return 成功返回找到的USB串口结构体
 */
struct usbh_serial *usbh_serial_dev_open(int pid, int vid, int intfnum, uint8_t idx);
/** 关闭USB转串口设备*/
void usbh_serial_dev_close (struct usbh_serial *p_userial);
/**
 * 设置USB转串口数据控制块
 *
 * @param p_userial USB转串口设备
 * @param p_dcb     串口数据控制块
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_dcb_set (struct usbh_serial *p_userial, struct usb_serial_dcb *p_dcb);
/**
 * 获取USB转串口设备数据控制块
 *
 * @param p_userial USB转串口设备
 *
 * @return 成功返回USB转串口配置结构体
 */
struct usb_serial_dcb *usbh_serial_dev_dcb_get (struct usbh_serial *p_userial);
/**
 * USB转串口设备发送数据(同步)
 *
 * @param p_userial   USB转串口设备
 * @param p_buffer    要写的数据的缓存
 * @param nbytes      要写的数据的长度
 *
 * @return 成功返回发送的数据的长度
 */
int usbh_serial_dev_send (struct usbh_serial *p_userial,
                          void               *p_buffer,
                          size_t              nbytes);
/**
 * USB转串口设备读数据(同步)
 *
 * @param p_userial   USB转串口设备
 * @param p_buffer    读回的数据的缓存
 * @param nbytes      读回的数据的长度
 *
 * @return 成功返回读回的数据的长度
 */
int usbh_serial_dev_recieve_sync (struct usbh_serial *p_userial,
                                  void               *p_buffer,
                                  size_t              nbytes);
/**
 * USB转串口设备读数据(异步)
 *
 * @param p_userial   USB转串口设备
 * @param p_buffer    读回的数据的缓存
 * @param nbytes      读回的数据的长度
 *
 * @return 成功返回读回的数据的长度
 */
int usbh_serial_dev_recieve_async (struct usbh_serial *p_userial,
                                   void               *p_buffer,
                                   size_t              nbytes);
/**
 * USB转串口设备停止传输数据
 *
 * @param p_userial   USB转串口设备
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_stop(struct usbh_serial *p_userial);
/**
 * 获取USB转串口管道单次传输大小
 *
 * @param usb_serial   USB串口结构体
 * @param pipe_dir     管道方向
 * @param tranfer_size 要返回的传输大小
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_tranfersize_get(struct usbh_serial *p_userial,
                                          uint8_t             pipe_dir,
                                          uint32_t           *tranfer_size);
/**
 * 设置USB串口单次传输大小
 *
 * @param usb_serial   USB串口结构体
 * @param pipe_dir     管道方向
 * @param tranfer_size 传输大小
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_tranfersize_set(struct usbh_serial *p_userial,
                                          uint8_t             pipe_dir,
                                          uint32_t            tranfer_size);
/**
 * 设置USB转串口设备设置超时参数
 *
 * @param p_userial USB转串口设备
 * @param pipe_dir  管道方向
 * @param timeout   要设置的时间
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_timeout_set (struct usbh_serial *p_userial,
                                       uint8_t             pipe_dir,
                                       uint32_t            timeout);
/**
 * 获取USB转串口设备获取超时参数
 *
 * @param p_userial   USB转串口设备
 * @param pipe_dir    管道方向
 * @param timeout     要获取的时间
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_timeout_get (struct usbh_serial *p_userial,
                                       uint8_t             pipe_dir,
                                       uint32_t           *timeout);
/**
 * 设置异步接收回调函数
 *
 * @param p_userial   USB转串口设备
 * @param rx_async_cb 要设置的回调函数
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_serial_dev_rxcb_set(struct usbh_serial *p_userial,
                                   void (*rx_async_cb)(void *buf, int len));
/**********************************************************************************************/
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

