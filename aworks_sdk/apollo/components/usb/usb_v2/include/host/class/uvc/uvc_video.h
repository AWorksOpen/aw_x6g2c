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
    /* ʵ��� */
    UVC_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
#endif
    UVC_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
    UVC_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
    UVC_BUF_TYPE_SDR_CAPTURE          = 11,
    /* �����ã���Ҫ�� */
    UVC_BUF_TYPE_PRIVATE              = 0x80,
};

/* ����״̬*/
enum uvc_buffer_state {
    UVC_BUF_STATE_IDLE   = 0,
    UVC_BUF_STATE_QUEUED = 1,
    UVC_BUF_STATE_ACTIVE = 2,
    UVC_BUF_STATE_READY  = 3,
    UVC_BUF_STATE_DONE   = 4,
    UVC_BUF_STATE_ERROR  = 5,
};

/* ʱ�������*/
#define UVC_BUF_FLAG_TIMESTAMP_MASK        0x0000e000
#define UVC_BUF_FLAG_TIMESTAMP_UNKNOWN     0x00000000
#define UVC_BUF_FLAG_TIMESTAMP_MONOTONIC   0x00002000
#define UVC_BUF_FLAG_TIMESTAMP_COPY        0x00004000
/* ʱ���Դ*/
#define UVC_BUF_FLAG_TSTAMP_SRC_MASK       0x00070000
#define UVC_BUF_FLAG_TSTAMP_SRC_EOF        0x00000000
#define UVC_BUF_FLAG_TSTAMP_SRC_SOE        0x00010000

/* ����ʱ���*/
struct uvc_timeval {
    long ts_sec;         /* ��*/
    long ts_usec;        /* ΢��*/
};

/* UVC��Ƶ���нṹ��*/
struct uvc_video_queue {
    usb_mutex_handle_t   mutex;      /* ��������*/
    usb_bool_t           is_used;    /* �Ƿ�ʹ��*/

    uint32_t             flags;
    uint32_t             buf_used;   /* ����ʹ�õĸ���*/

    uint32_t             num_buffers;/* ������/ʹ�õĻ��������*/
    struct usb_list_head irqqueue;   /* �ж϶�������*/
};

/* USB����ṹ��*/
struct uvc_buffer {
    struct usb_list_head  queue;     /* ����ڵ�*/

    struct uvc_timeval    timeval;   /* ʱ��ֵ*/
    enum uvc_buffer_state state;     /* ����״̬*/
    uint32_t              error;     /* �������*/

    void                 *mem;       /* ����ָ��*/
    uint32_t              length;    /* ���泤��*/
    uint32_t              bytesused; /* ����������ֽ���*/
    uint8_t               is_used;   /* �Ƿ�ʹ�ñ�־*/

    uint32_t              pts;
};

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif


