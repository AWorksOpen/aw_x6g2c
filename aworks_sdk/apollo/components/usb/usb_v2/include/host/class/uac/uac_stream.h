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
#ifndef __UAC_STREAM_H_
#define __UAC_STREAM_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uac/uac_driver.h"
#include "host/class/uac/uac_format.h"
#include "host/class/uac/uac_pcm.h"
#include "host/class/uac/uac_endpoint.h"
#include "host/class/uac/uac_hw_rule.h"
#include <string.h>
#include <stdio.h>

/* USB音频子流结构体*/
struct uac_substream {
    struct uac_stream                *stream;         /* 所属的UAC设备数据流*/
    struct usbh_function             *p_fun;          /* USB接口功能结构体*/
    struct uac_pcm_substream         *pcm_substream;  /* PCM子流*/
    int                               direction;      /* PCM方向：回放或捕获*/
    int                               interface;      /* 当前接口号*/
    int                               endpoint;       /* 端点号*/
    struct audioformat               *cur_audiofmt;   /* 当前的音频格式指针*/
    int                               pcm_format_idx; /* 当前音频格式索引*/
    uint32_t                          channels;       /* 当前的通道数*/
    uint32_t                          channels_max;   /* 所有音频格式的最大通道*/
    uint32_t                          cur_rate;       /* 当前采样率*/
    uint32_t                          period_bytes;   /* 当前周期字节*/
    uint32_t                          period_frames;  /* 当前每个周期的帧数 */
    uint32_t                          buffer_periods; /* 当前每个缓存的周期数*/
    uint32_t                          altsetting;     /* USB数据格式: 备用设置号*/
    uint32_t                          txfr_quirk:1;   /* allow sub-frame alignment */
    uint32_t                          fmt_type;       /* USB 音频格式类型 (1-3) */
    uint32_t                          pkt_offset_adj; /* 要从数据包开头删除的字节数(对于不合规的设备)*/
    uint32_t                          maxframesize;   /* 最大帧数*/
    uint32_t                          curframesize;   /* 当前帧数*/

    usb_bool_t                        running;        /* 运行状态 */

    uint32_t                          hwptr_done;     /* 缓存中获取到用户数据的位置*/
    uint32_t                          transfer_done;  /* 自上次周期更新后处理的帧数*/
    uint32_t                          frame_limit;    /* USB传输请求包的帧极限数量*/

    /* 这个数据流的数据和同步端点*/
    uint32_t                          ep_num;         /* 端点号*/
    struct uac_endpoint              *data_endpoint;
    struct uac_endpoint              *sync_endpoint;
    uint32_t                          flags;
    usb_bool_t                        need_setup_ep;  /* 是否配置好端点准备收发数据*/
    uint32_t                          speed;          /* USB设备速度*/

    uint32_t                          formats;        /* 格式位数*/
    uint32_t                          num_formats;    /* 支持的音频的格式的数量(链表)*/
    struct usb_list_head              fmt_list;       /* 格式链表*/
    usb_mutex_handle_t                fmt_list_lock;  /* 格式链表互斥锁*/
    struct uac_pcm_hw_constraint_list rate_list;      /* 极限采样率*/

    int last_frame_number;                            /* 最新的周期帧编号*/
    int last_delay;                                   /* 储存最新的延时的音帧数量*/

    struct {
        int marker;
        int channel;
        int byte_idx;
    } dsd_dop;
    /* 是否更新启动时间*/
    usb_bool_t                         trigger_tstamp_pending_update;
};

/* USB音频设备数据流结构体*/
struct uac_stream {
    struct usbh_audio        *p_uaudio;    /* USB音频设备*/
    struct uac_pcm           *pcm;         /* PCM实体*/
    int                       pcm_index;   /* PCM索引*/
    uint32_t                  fmt_type;    /* USB音频格式类型(1-3)*/
    struct uac_substream      substream[2];/* USB音频设备数据子流*/
    struct usb_list_head      list;        /* 数据流链表节点*/
};

/**
 * 添加端点到芯片实体，如果一个流已经有这个端点，则追加。没有则创建一个新的PCM流
 *
 * @param p_uaudio   USB音频设备结构体
 * @param p_fun      特定的接口功能结构体
 * @param stream     数据流方向
 * @param fp         音频格式
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_add_audio_stream(struct usbh_audio    *p_uaudio,
                               struct usbh_function *p_fun,
                               int                   stream,
                               struct audioformat   *fp);
/** 释放USB音频数据流*/
usb_err_t uac_dev_stream_release(struct usbh_audio *p_uaudio);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

