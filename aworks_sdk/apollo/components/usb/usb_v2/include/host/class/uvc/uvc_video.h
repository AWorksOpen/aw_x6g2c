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
#ifndef __UVC_VIDEO_H
#define __UVC_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_adapter_inter.h"
#include "host/class/uvc/uvc_stream.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

#define UVC_QUEUE_DISCONNECTED      (1 << 0)
#define UVC_QUEUE_DROP_CORRUPTED    (1 << 1)

enum uvc_buf_type {
    UVC_BUF_TYPE_VIDEO_CAPTURE        = 1,
    UVC_BUF_TYPE_VIDEO_OUTPUT         = 2,
    UVC_BUF_TYPE_VIDEO_OVERLAY        = 3,
    UVC_BUF_TYPE_VBI_CAPTURE          = 4,
    UVC_BUF_TYPE_VBI_OUTPUT           = 5,
    UVC_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
    UVC_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
#if 1
    /* 实验的 */
    UVC_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
#endif
    UVC_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
    UVC_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
    UVC_BUF_TYPE_SDR_CAPTURE          = 11,
    /* 已弃用，不要用 */
    UVC_BUF_TYPE_PRIVATE              = 0x80,
};

/* 缓存状态*/
enum uvc_buffer_state {
    UVC_BUF_STATE_IDLE   = 0,
    UVC_BUF_STATE_QUEUED = 1,
    UVC_BUF_STATE_ACTIVE = 2,
    UVC_BUF_STATE_READY  = 3,
    UVC_BUF_STATE_DONE   = 4,
    UVC_BUF_STATE_ERROR  = 5,
};

/* 时间戳类型*/
#define UVC_BUF_FLAG_TIMESTAMP_MASK        0x0000e000
#define UVC_BUF_FLAG_TIMESTAMP_UNKNOWN     0x00000000
#define UVC_BUF_FLAG_TIMESTAMP_MONOTONIC   0x00002000
#define UVC_BUF_FLAG_TIMESTAMP_COPY        0x00004000
/* 时间戳源*/
#define UVC_BUF_FLAG_TSTAMP_SRC_MASK       0x00070000
#define UVC_BUF_FLAG_TSTAMP_SRC_EOF        0x00000000
#define UVC_BUF_FLAG_TSTAMP_SRC_SOE        0x00010000

/* 缓存时间戳*/
struct uvc_timeval {
    long ts_sec;         /* 秒*/
    long ts_usec;        /* 微秒*/
};

/* UVC视频队列结构体*/
struct uvc_video_queue {
    usb_mutex_handle_t   mutex;      /* 保护队列*/
    usb_bool_t           is_used;    /* 是否被使用*/

    uint32_t             flags;
    uint32_t             buf_used;   /* 缓存使用的个数*/

    uint32_t             num_buffers;/* 被分配/使用的缓存的数量*/
    struct usb_list_head irqqueue;   /* 中断队列链表*/
};

/* USB缓存结构体*/
struct uvc_buffer {
    struct usb_list_head  queue;     /* 缓存节点*/

    struct uvc_timeval    timeval;   /* 时间值*/
    enum uvc_buffer_state state;     /* 缓存状态*/
    uint32_t              error;     /* 错误代码*/

    void                 *mem;       /* 缓存指针*/
    uint32_t              length;    /* 缓存长度*/
    uint32_t              bytesused; /* 缓存已填充字节数*/
    uint8_t               is_used;   /* 是否被使用标志*/

    uint32_t              pts;
};

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif


