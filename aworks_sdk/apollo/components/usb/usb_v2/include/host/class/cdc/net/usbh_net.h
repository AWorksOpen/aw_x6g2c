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
#ifndef __USBH_NET_H
#define __USBH_NET_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "host/class/cdc/usbh_cdc.h"

#define USBH_USBNET_ETHIF_NAME "usbnet-dev"

/* 网络设备类型*/
#define USB_NET_ETHER            1
#define USB_NET_WWAN             2

/* 硬件地址最大长度 */
#define USB_NETIF_MAX_HWADDR_LEN 6

/* 网络设备标志*/
#define FLAG_NO_SETINTF        0x0010  /* 设备不能设置接口*/
#define FLAG_ETHER             0x0020  /* 以太网设备 */
#define FLAG_WLAN              0x0080  /* 无线局域网设备*/
#define FLAG_WWAN              0x0400  /* 无线广域网设备*/
#define FLAG_LINK_INTR         0x0800  /* 更新链接状态 */
#define FLAG_POINTTOPOINT      0x1000  /* 点对点设备*/

/* USB网络设备库结构体*/
struct usbh_net_lib{
    struct usb_list_head    unet_lib_dev_list;      /* 管理所有USB网络设备链表*/
    usb_bool_t              is_lib_init;            /* 是否初始化设备库*/
    usb_bool_t              is_lib_uniniting;       /* 是否移除设备库*/
    usb_mutex_handle_t      unet_lib_lock;          /* 互斥锁*/
    uint8_t                 unet_lib_dev_num;       /* 当前存在设备的数量*/
    int                     refcnt;                 /* 引用计数*/
    struct usbh_mem_record  unet_mem_record;        /* 内存记录*/
};

struct usbnet;
/* USB网络驱动驱动信息*/
struct usbnet_drv_info {
    char *pDescription;                      /* 描述信息*/
    int   flags;                             /* 标志*/
    /* 初始化设备*/
    usb_err_t (*bind)(struct usbnet *);
    /* 清除设备 */
    void (*unbind)(struct usbnet *, struct usbh_interface *);
    /* 状态轮询 */
    void (*status)(struct usbnet *, struct usbh_trp *);
    /* 接收模式改变(设备改变地址列表滤波器) */
    void (*set_rx_mode)(struct usbnet *);

    struct usbh_endpoint *p_in;              /* 输入端点*/
    struct usbh_endpoint *p_out;             /* 输出端点*/
};

/* 重用USB CDC 基础设施的一些标准的驱动程序(特别是根据CDC联合描述符
 * 使用多个接口)会得到一些帮助程序代码**/
struct usbh_cdc_state {
    struct usb_cdc_header_desc *pHeader;   /* CDC头描述符*/
    struct usb_cdc_union_desc  *pU;        /* CDC联合描述符*/
    struct usb_cdc_ether_desc  *pEther;    /* CDC以太网描述符*/
    struct usbh_interface      *pControl;  /* 控制接口*/
    struct usbh_interface      *pData;     /* 数据接口*/
};

/* USB网络设备*/
struct usbnet{
    struct usbh_function*    p_ufun;                           /* 相关USB功能接口*/
    struct usbh_interface*   p_cur_itf;                        /* 当前使用的USB设备接口*/
    struct usbh_config*      p_cur_cfg;                        /* 当前使用的USB设备的当前配置*/
    struct usbnet_drv_info*  p_drv_info;                       /* USB网络驱动驱动信息*/
    struct usbh_endpoint*    p_status_ep;                      /* 状态端点*/
    struct usbh_endpoint*    p_in_ep;                          /* 输入端点*/
    struct usbh_endpoint*    p_out_ep;                         /* 输出端点*/
    struct usb_list_head     node;                             /* 当前设备节点*/
    usb_mutex_handle_t       lock;                             /* 互斥锁*/
    unsigned long            data[5];
    char                     ethif_name[32];                   /* 以太网设备名字*/
    uint8_t                  hwaddr[USB_NETIF_MAX_HWADDR_LEN]; /* 硬件地址*/
    uint32_t                 hard_mtu;                         /* 硬件最大传输单元，计数任何额外的帧*/
    uint16_t                 hard_header_len;
    uint32_t                 rx_trp_size;                      /* 接收传输请求包大小*/
    uint32_t                 maxpacket;                        /* 最大包大小*/
    usb_sem_handle_t         bh_sem;                           /* 信号量*/
    usb_task_handle_t        bh;                               /* 处理接收包任务*/
    struct usbh_trp         *p_int_trp;                        /* 中断传输请求包*/
    uint16_t                 rx_qlen, tx_qlen;                 /* 发送/接收队列长度*/
    usb_bool_t               is_removed;                       /* 是否移除*/
};





/** 初始化USB网络设备库*/
usb_err_t usbh_net_lib_init(void);
/** 反初始化USB网络设备库*/
usb_err_t unet_lib_uninit(void);
/** 获取USB网络内存记录*/
usb_err_t unet_lib_mem_record_get(struct usbh_mem_record *mem_record);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif

