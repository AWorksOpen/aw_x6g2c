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
#ifndef __UVC_DRIVER_H
#define __UVC_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "time.h"
#include "host/class/uvc/uvc_stream.h"
#include "host/class/uvc/uvc_ctrls.h"

#define __UVC_TRACE_DEBUG(info) \
            do { \
            } while (0)

#define UVC_ALL_DEV -1

/* UVC控件控制传输超时时间*/
#define UVC_CTRL_CONTROL_TIMEOUT    300

#define UVC_MATCH_VENDOR          0x0001  /* 匹配VID*/
#define UVC_MATCH_PRODUCT         0x0002  /* 匹配PID*/

/* UVC接口子类代码*/
#define VSC_UNDEFINED                    0x00         /* 未定义子类*/
#define VSC_VIDEOCONTROL                 0x01         /* 视频控制子类*/
#define VSC_VIDEOSTREAMING               0x02         /* 视频流子类*/
#define VSC_VIDEO_INTERFACE_COLLECTION   0x03         /* 视频接口集子类*/

/* 2.4.2.2. 状态包类型*/
#define UVC_STATUS_TYPE_CONTROL             1
#define UVC_STATUS_TYPE_STREAMING           2

/* 设备兼容*/
#define UVC_QUIRK_FIX_BANDWIDTH     0x00000080
#define UVC_QUIRK_STREAM_NO_FID     0x00000010
/* ------------------------------------------------------------------------
 * 全局唯一标识符(Globally Unique Identifier,GUIDs)
 */
#define UVC_GUID_UVC_CAMERA \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_OUTPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}
#define UVC_GUID_UVC_SELECTOR \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}

#define UVC_GUID_FORMAT_MJPEG \
    { 'M',  'J',  'P',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2 \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2_ISIGHT \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0x00, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_NV12 \
    { 'N',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YV12 \
    { 'Y',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_I420 \
    { 'I',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_UYVY \
    { 'U',  'Y',  'V',  'Y', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y800 \
    { 'Y',  '8',  '0',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y8 \
    { 'Y',  '8',  ' ',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y10 \
    { 'Y',  '1',  '0',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y12 \
    { 'Y',  '1',  '2',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y16 \
    { 'Y',  '1',  '6',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BY8 \
    { 'B',  'Y',  '8',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BA81 \
    { 'B',  'A',  '8',  '1', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GBRG \
    { 'G',  'B',  'R',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GRBG \
    { 'G',  'R',  'B',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGGB \
    { 'R',  'G',  'G',  'B', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGBP \
    { 'R',  'G',  'B',  'P', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BGR3 \
    { 0x7d, 0xeb, 0x36, 0xe4, 0x4f, 0x52, 0xce, 0x11, \
     0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define UVC_GUID_FORMAT_M420 \
    { 'M',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define UVC_GUID_FORMAT_H264 \
    { 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

/* 每个USB请求块的最大包数量*/
#define UVC_MAX_PACKETS     32
/* 中断URB的最大状态缓存字节大小*/
#define UVC_MAX_STATUS_SIZE 16

/* USB设备ID匹配*/
struct usb_device_id {
    uint16_t match_flags;    /* 匹配标志*/
    uint16_t idVendor;       /* VID*/
    uint16_t idProduct;      /* PID*/
};

#define UVC_DEVICE_MATCH(vend, prod) \
    .match_flags =  UVC_MATCH_VENDOR | UVC_MATCH_PRODUCT, \
    .idVendor    = (vend), \
    .idProduct   = (prod)

/* UVC库结构体*/
struct uvc_lib{
    struct usb_list_head    uvc_lib_dev_list;       /* 管理所有UVC设备链表*/
    usb_bool_t              is_lib_init;            /* 是否初始化UVC设备库*/
    usb_bool_t              is_lib_uniniting;       /* 是否移除UVC设备库*/
    usb_mutex_handle_t      uvc_lib_lock;           /* UVC互斥锁*/
    uint8_t                 uvc_lib_stream_num_max; /* 可存在UVC设备数据流的最大数量*/
    uint8_t                 uvc_lib_stream_num;     /* 当前存在UVC设备数据流数量*/
    uint32_t                uvc_lib_stream_idx;     /* UVC设备数据流索引*/
    uint8_t                 uvc_lib_dev_num;        /* 当前存在UVC设备的数量*/
    struct uvc_video_queue *uvc_lib_dev_queue;      /* UVC缓存队列*/
    int                     refcnt;                 /* 引用计数*/
    struct usbh_mem_record  uvc_mem_record;         /* 内存记录*/
};

/* UVC设备结构体*/
struct uvc_device{
    struct usbh_function  *uvc_fun;                /* 相关的UBS功能结构体(描述一个USB设备功能)*/
    struct usbh_interface *uvc_controlitf;         /* UVC控制接口链表(根据UVC规范1.5，每个视频功能只有一个控制接口)*/
    usb_mutex_handle_t     lock;                   /* 互斥锁*/
    uint16_t               uvc_version;            /* UVC规范版本*/
    uint32_t               clock_frequency;        /* 时钟频率*/
    uint32_t               itfstream_altset_num;   /* UVC流接口备用设置的数量*/
    struct usb_list_head   streams;                /* 视频流链表*/
    struct usb_list_head   entities;               /* UVC实体链表(描述单元和终端)*/
    struct usb_list_head   chains;                 /* 视频设备链表*/
    struct usb_list_head   node;                   /* 当前视频设备链表*/
    int                    nstreams;               /* 视频流数量*/
    int                    nentities;              /* 终端数量*/
    struct usbh_trp        int_trp;                /* 中断传输请求包*/
    struct usbh_endpoint  *int_ep;                 /* 中断端点*/
    uint8_t               *status;                 /* UVC设备中断端点缓存*/
    uint32_t               quirks;                 /* 设备兼容*/
    usb_bool_t             is_remove;              /* 是否移除*/
    int                    refcnt;                 /* 引用计数*/
};

/***************************************函数声明**************************************************/
/**
 * USB视频驱动库相关初始化
 *
 * @param stream_num_max 最大支持UVC设备数据流数量
 * @param stream_buf_num 设备数据流缓存个数
 *
 * @return 成功返回USB_OK
 */
usb_err_t uvc_lib_init(uint8_t stream_num_max, uint8_t stream_buf_num);
/** 反初始化USB视频驱动库*/
usb_err_t uvc_lib_uninit(void);
/** 获取UVC内存记录*/
usb_err_t uvc_lib_mem_record_get(struct usbh_mem_record *mem_record);
/**
 * 检查USB设备是不是一个UVC设备
 *
 * @param p_ufun USB功能结构体
 *
 * @return 是UVC设备返回USB_TRUE，不是返回UB_FALSE
 */
usb_bool_t uvc_dev_check_valid(struct usbh_function *p_ufun);
/** 创建一个UVC设备*/
struct uvc_device *uvc_dev_create(struct usbh_function *p_ufun);
/**
 * 删除UVC设备
 *
 * @param dev UVC设备结构体
 */
void uvc_dev_destory(struct uvc_device *uvc_dev);
/** 获取当前存在的UVC设备数量*/
usb_err_t uvc_dev_num_get(void);
/**
 * 根据PID和VID获取UVC设备
 *
 * @param pid 指定PID,若为USBDEV_IGONRE_PID则忽略匹配pid
 * @param vid 指定VID,若为USBDEV_IGONRE_VID则忽略匹配vid
 * @param idx 索引，指明要获取第几个UVC设备(最小为1)
 *
 * @return 成功返回UVC结构体
 */
struct uvc_device *uvc_dev_open(uint16_t pid, uint16_t vid, uint8_t idx);
/** 关闭UVC设备*/
void uvc_dev_close(struct uvc_device *uvc_dev);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif
