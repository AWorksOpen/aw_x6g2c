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
#ifndef __UAC_ENDPOINT_H_
#define __UAC_ENDPOINT_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/core/usbh.h"
#include "host/class/uac/uac_format.h"
#include <string.h>
#include <stdio.h>

/* 音频端点拓展长度*/
#define USB_EP_AUDIO_SIZE  9
/* USB音频最大的传输请求包数量*/
#define UAC_MAX_TRPS       12
/* 每个传输请求包最大的数据包数*/
#define UAC_MAX_PACKS      6
/* 高速模式，每个传输请求包最大的数据包数*/
#define UAC_MAX_PACKS_HS   (UAC_MAX_PACKS * 8)
/* 1ms里尽量不要超过这个队列长度*/
#define UAC_MAX_QUEUE      18

#define SND_USB_ENDPOINT_TYPE_DATA     0
#define SND_USB_ENDPOINT_TYPE_SYNC     1

struct uac_endpoint;
/* UAC传输请求包环境*/
struct uac_trp_ctx {
    struct uac_endpoint      *ep;                            /* 传输相关端点*/
    int                       index;                         /* 传输请求包数字索引*/
    int                       packets;                       /* 每个USB传输请求包的数据包数量*/
    int                       packet_size[UAC_MAX_PACKS_HS]; /* 下一次的提交的包的大小*/
    uint32_t                  buffer_size;                   /* 数据缓存的大小*/
    struct usbh_trp          *trp;                           /* USB传输请求包*/
    struct usb_list_head      ready_list;                    /* 准备好的话会插入预备链表*/
};

/* UAC音频设备端点结构体*/
struct uac_endpoint {
    struct usbh_audio         *p_uaudio;            /* 所属的音频设备*/
    struct usbh_endpoint      *p_ep;                /* 相关端点*/
    int                        ep_num;              /* 端点号*/
    int                        iface;               /* 所属接口号*/
    int                        altsetting;          /* 所属接口的备用设置号*/
    int                        type;                /* 音频端点的类型*/
    int                        refcnt;              /* 引用计数*/
    struct usb_list_head       list;                /* 音频端点节点*/
    struct usb_list_head       ready_playback_trps; /* 准备回放的传输请求包*/
    uint32_t                   syncinterval;        /* P for adaptive mode, 0 otherwise */
    uint32_t                   syncmaxsize;         /* 同步端点包大小*/
    uint32_t                   datainterval;        /* log_2的数据包时间间隔*/
    uint32_t                   maxpacksize;         /* 最大包大小(字节)*/
    uint32_t                   maxframesize;        /* 数据包里的音频帧大小*/
    uint32_t                   curpacksize;         /* 当前包大小(字节，用于捕获)*/
    uint32_t                   curframesize;        /* 当前包里的音频帧大小(用于捕获模式)*/
    uint32_t                   fill_max:1;          /* 总是填充最大包大小*/
    uint32_t                   freqn;               /* 在 Q16.16格式中fs/fps的普通采样率 */
    uint32_t                   freqm;               /* 在 Q16.16格式中fs/fps的瞬时采样率*/
    uint32_t                   freqmax;             /* 最大采样率，用于缓存管理*/
    int                        freqshift;           /* how much to shift the feedback value to get Q16.16 */
    uint32_t                   phase;               /* phase accumulator */
    uint32_t                   ntrps;               /* 传输请求包的数量*/
    struct uac_trp_ctx         trp[UAC_MAX_TRPS];   /* UAC传输请求包*/
    uint32_t                   max_trp_frames;      /* 传输请求包音频的最大数量*/
    struct uac_substream      *data_subs;           /* 数据子流*/
    void (*prepare_data_trp) (struct uac_substream *subs,
                              struct usbh_trp *trp);
    void (*retire_data_trp) (struct uac_substream *subs,
                             struct usbh_trp *trp);
    uint32_t                   stride;
    uint8_t                    silence_value;       /* 静音的数据值*/
    uint32_t                   flags;
    int                        skip_packets;        /* 有些设备会忽略数据流前面n个包*/
};

/**
 * 添加一个端点到USB音频设备
 *
 * @param chip      USB音频设备结构体
 * @param alts      USB接口
 * @param ep_num    要使用的端点号
 * @param direction PCM数据流方向(回放或者捕获)
 * @param type      音频端点类型
 *
 * @return 成功返回USB音频端点结构体
 */
struct uac_endpoint *uac_dev_endpoint_add(struct usbh_audio     *chip,
                                          struct usbh_interface *alts,
                                          int                    ep_num,
                                          int                    direction,
                                          int                    type);
/** 释放USB音频端点*/
usb_err_t uac_dev_endpoint_release(struct usbh_audio *p_uaudio);
/** 配置端点参数*/
usb_err_t uac_substream_endpoint_config(struct uac_substream *subs);
/**
 * 启动子流端点
 *
 * @param subs      USB音频子流
 * @param can_sleep 是否能休眠
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_substream_endpoint_start(struct uac_substream *subs,
                                       usb_bool_t            can_sleep);
/** 停止端点*/
void uac_endpoint_stop(struct uac_endpoint *ep);
/**
 * 释放一个端点
 *
 * @param ep 音频设备端点
 */
void uac_endpoint_release(struct uac_endpoint *ep);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


