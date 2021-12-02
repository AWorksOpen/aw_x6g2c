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
#ifndef __UAC_DRIVER_H__
#define __UAC_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "host/core/usbh.h"
#include "host/core/usbh_dev.h"
#include "host/class/uac/uac.h"
#include "host/class/uac/uac_pcm.h"
#include "usb_errno.h"

/* UAC设备的VID和PID*/
#define UAC_ID(vendor, product) (((vendor) << 16) | (product))
/* 获取UAC设备的VID*/
#define UAC_ID_VENDOR(id)       ((id) >> 16)
/* 获取UAC设备的PID*/
#define UAC_ID_PRODUCT(id)      ((uint16_t)(id))
/* 默认声卡*/
#define UAC_DEV_DEFAULT           -1

#define combine_word(s)    ((*(s)) | ((uint32_t)(s)[1] << 8))
#define combine_triple(s)  (combine_word(s) | ((uint32_t)(s)[2] << 16))
#define combine_quad(s)    (combine_triple(s) | ((uint32_t)(s)[3] << 24))

/* 声卡模式*/
enum {
    UAC_SND_PCM_STREAM_PLAYBACK = 0,    /* 播放模式*/
    UAC_SND_PCM_STREAM_CAPTURE,         /* 捕获模式(录音)*/
    UAC_SND_PCM_STREAM_LAST     = UAC_SND_PCM_STREAM_PLAYBACK,
};

/* 时间戳模式*/
enum {
    UAC_SND_PCM_TSTAMP_NONE    = 0,
    UAC_SND_PCM_TSTAMP_ENABLE,
    UAC_SND_PCM_TSTAMP_LAST    = UAC_SND_PCM_TSTAMP_ENABLE,
};

/* USB音频设备结构体*/
struct usbh_audio{
    int                    index;            /* 音频设备索引*/
    struct usbh_function  *p_fun;            /* USB控制接口功能结构体*/
    union{
        uint16_t           id[2];            /* 音频设备PID*/
        uint32_t           uac_id;
    };
    struct usb_list_head   node;             /* 音频设备节点*/
    char                   name[80];         /* 音频设备名字*/
    struct usbh_interface *ctrl_intf;        /* 控制接口*/
    int                    ctrl_intf_num;    /* 控制接口数量*/
    usb_bool_t             txfr_quirk;       /* 是否有传输兼容*/
    int                    pcm_devs;         /* PCM设备号*/
    struct usb_list_head   pcm_stream_list;  /* PCM数据流链表*/
    usb_mutex_handle_t     pcm_stream_mutex; /* PCM数据流互斥锁*/
    int                    pcm_stream_num;   /* PCM数据流数量*/
    struct usb_list_head   uac_ep_list;      /* 音频相关端点的链表*/
    usb_mutex_handle_t     uac_ep_mutex;     /* 端音频相关端点链表互斥锁*/
    int                    ref;              /* 引用计数*/
    usb_bool_t             is_removed;       /* 是否被移除*/
    usb_bool_t             is_init;          /* 是否初始化完成*/
};

/** USB音频驱动库相关初始化*/
usb_err_t usbh_audio_lib_init(void);
/**
 * 获取USB音频设备索引
 *
 * @param pid 设备PID
 * @param vid 设备VID
 *
 * @return 成功返回设备索引
 */
int usbh_audio_dev_idx_get(uint16_t pid, uint16_t vid);
/**
 * 寻找音频设备
 *
 * @param uac_dev_idx 音频设备索引(若设置为UAC_DEV_DEFAULT则获取第一个设备)
 *
 * @return 成功返回USB音频结构体
 */
struct usbh_audio *usbh_audio_dev_find(int uac_dev_idx);
/**
 * 创建一个USB音频设备
 *
 * @param p_fun USB接口功能
 *
 * @return 成功返回USB_OK
 */
struct usbh_audio * usbh_audio_dev_create(struct usbh_function *p_fun);
/** 销毁一个USB音频设备*/
usb_err_t usbh_audio_dev_destory(struct usbh_audio *p_uaudio);
/**
 * UAC控制传输
 *
 * @param p_fun       相关的功能接口
 * @param request     具体的USB请求
 * @param requesttype 传输方向|请求类型|请求目标
 * @param value       参数
 * @param index       索引
 * @param data        数据(可以为空)
 * @param size        数据长度
 *
 * @return 如果有数据传输，成功返回成功传输的字节数
 *         如果没有数据传输，成功返回USB_OK
 */
usb_err_t uac_ctl_msg(struct usbh_function *p_fun,
                      uint8_t               request,
                      uint8_t               requesttype,
                      uint16_t              value,
                      uint16_t              index,
                      void                 *data,
                      uint16_t              size);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

