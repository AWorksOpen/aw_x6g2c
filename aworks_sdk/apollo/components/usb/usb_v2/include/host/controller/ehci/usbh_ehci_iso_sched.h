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

#ifndef __USBH_EHCI_ISO_SCHED_H
#define __USBH_EHCI_ISO_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

#define NO_FRAME    29999           /* 尚未分配帧*/

/* USB ISO打印函数*/
#define __USB_ISO_TRACE(info, ...) \
            do { \
                usb_printf("USBH-ISO:"info, ##__VA_ARGS__); \
            } while (0)

/* 通用带宽分配常量/支持*/
#define BitTime(bytecount) (7 * 8 * bytecount / 6)
#define NS_TO_US(ns)    USB_DIV_ROUND_UP(ns, 1000L)
#define USB2_HOST_DELAY 5
#define HS_NSECS(bytes) ( ((55 * 8 * 2083)/1000) \
    + ((2083UL * (3167 + BitTime (bytes)))/1000) \
    + USB2_HOST_DELAY)
#define HS_NSECS_ISO(bytes) (((38 * 8 * 2083) \
    + (2083UL * (3 + BitTime(bytes))))/1000 \
    + USB2_HOST_DELAY)
#define HS_USECS(bytes) NS_TO_US (HS_NSECS(bytes))
#define HS_USECS_ISO(bytes) NS_TO_US(HS_NSECS_ISO(bytes))

/* 高速端点描述符中编码的高带宽乘法器*/
#define hb_mult(wMaxPacketSize) (1 + (((wMaxPacketSize) >> 11) & 0x03))
/* ...和包大小，用于任何类型的端点描述符*/
#define max_packet(wMaxPacketSize) ((wMaxPacketSize) & 0x07ff)

/* 为分割事务的高速设备和全/低速设备的周期传输调度和预算信息*/
struct ehci_per_sched {
    struct usbh_device    *udev;           /* 访问事务转换器*/
    struct usbh_endpoint  *ep;

    uint16_t               cs_mask;         /* C-mask和 S-mask 字节*/
    uint16_t               phase;           /* 真实时期，帧部分*/
    uint8_t                bw_phase;        /* 一样，为带宽保留*/
    uint8_t                phase_uf;        /* uframe part of the phase */
    uint8_t                usecs, c_usecs;  /* 高速总线上的时间*/
    uint8_t                bw_uperiod;      /* 带宽预留的微帧周期*/
    uint16_t               period;          /* 帧的真实周期*/
    uint8_t                bw_period;       /* 带宽帧周期*/
};

/* 一个等时传输事务的描述符(如果是高速最多3KB)*/
struct ehci_iso_packet {
    /* These will be copied to iTD when scheduling */
    uint64_t         bufp;               /* itd->hw_bufp{,_hi}[pg] |= */
    uint32_t         transaction;        /* itd->hw_transaction[i] |= */
    uint8_t          cross;              /* 缓存跨页*/
    /* 用于全速输出分割*/
    uint32_t         buf1;
};

/* 来自等时USB请求块的数据包临时调度数据(两种速度)。每个数据包是设备的
 * 一个逻辑USB事务(不是分割事务)，从stream->next_uframe 开始*/
struct ehci_iso_sched {
    struct usb_list_head    td_list;         /* 传输描述符链表*/
    unsigned                span;            /* 这次调度在帧周期中的帧/微帧跨度*/
    unsigned                first_packet;
    struct ehci_iso_packet *packet;
};

/* EHCI等时流结构体*/
struct ehci_iso_stream {
    struct usbh_ehci_qh  *hw;
    struct usbh_device   *udev;              /* 访问事务转换器*/
    struct usb_list_head  td_list;           /* 入队列的等时传输描述符/分割事务等时传输描述符*/
    struct usb_list_head  free_list;         /* 空闲的等时传输描述符/分割事务等时传输描述符*/
    uint32_t              next_uframe;

    usb_bool_t            release;           /* 是否释放*/
    uint8_t               bEndpointAddress;  /* 端点地址*/
    uint8_t               highspeed;         /* 高速传输*/
    uint16_t              uperiod;           /* 微帧周期*/
    uint32_t              bandwidth;         /* 带宽*/
    uint16_t              maxp;              /* 最大包大小*/
    uint32_t              splits;            /* 分割事务标志*/
    int                   refcount;          /* 2.6*/
    struct usbh_endpoint *ep;                /* 2.6*/
    uint8_t               usecs, c_usecs;    /* 2.6*/
    uint16_t              cs_mask;           /* 2.6*/
    uint8_t               interval;          /* 2.6 微帧周期*/
    uint8_t               f_interval;        /* 2.6 帧周期*/
    uint16_t              depth;             /* 2.6*/
    uint16_t              tt_usecs;          /* 2.6 在全速/低速总线上的时间*/

    struct ehci_per_sched ps;                /* 调度信息 4.x.y*/

    /* 这是用于初始化等时传输描述符的 hw_bufp 字段*/
    uint32_t              buf0;
    uint32_t              buf1;
    uint32_t              buf2;

    /* 用于初始化分割等时传输描述符的事物转换器信息*/
    uint32_t              address;
};

/***************************************函数声明**************************************************/
/** 释放等时数据流*/
usb_err_t iso_stream_release(struct usbh_ehci *p_ehci, struct ehci_iso_stream *stream);
/** 扫描周期帧列表*/
void scan_periodic(struct usbh_ehci *p_ehci);
/**
 * 获取等时数据流
 *
 * @param p_ehci EHCI结构体
 * @param p_trp  相关的等时传输请求包
 *
 * @return 成功返回初始化好的等时数据流结构体
 */
struct ehci_iso_stream *__iso_stream_get (struct usbh_ehci *p_ehci,
                                          struct usbh_endpoint *p_ep);
/**
 * 等时传输请求
 *
 * @param p_ehci EHCI结构体
 * @param p_trp  传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t __iso_req(struct usbh_ehci *p_ehci, struct usbh_trp *p_trp,
                    struct ehci_iso_stream  *stream);
/**
 * 生成一个等时调度
 *
 * @param p_ehci EHCI结构体
 * @param p_trp  相关的等时传输请求包
 * @param stream 等时数据流
 *
 * @return 成功返回USB_OK
 */
usb_err_t __iso_sched_make(struct usbh_ehci *p_ehci, struct usbh_trp *p_trp,
                           struct ehci_iso_stream  *stream);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

