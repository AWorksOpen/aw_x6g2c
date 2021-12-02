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
#ifndef __UVC_OPT_H
#define __UVC_OPT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uvc/uvc_stream.h"
#include "host/class/uvc/uvc_driver.h"
#include "host/class/uvc/uvc_entity.h"

typedef enum{
    UVC_DEV_STREAM_CLOSE = 0,
    UVC_DEV_STREAM_OPEN,
    UVC_DEV_STREAM_CONFIG,
    UVC_DEV_STREAM_START,
    UVC_DEV_STREAM_STOP,
    UVC_DEV_STREAM_SUSPEND,
} uvc_opt_status;

/* 可选配置标志*/
#define UVC_BRIGHTNESS_CFG (1 << 0)
#define UVC_CONTRAST_CFG   (1 << 1)
#define UVC_HUE_CFG        (1 << 2)
#define UVC_SATURARION_CFG (1 << 3)
#define UVC_SHARPNESS_CFG  (1 << 4)
#define UVC_GAMA_CFG       (1 << 5)
#define UVC_WB_CFG         (1 << 6)
#define UVC_PLF_CFG        (1 << 7)

/* UVC格式配置结构体*/
typedef struct uvc_format_cfg {
    uint32_t                type;  /* 格式类型*/
    struct uvc_pix_format   pix;   /* 像素点格式*/

}uvc_format_cfg_t;

/* UVC配置结构体*/
struct uvc_cfg {
    /* 以下为必选配置*/
    uint32_t         usb_camera_type;    /* USB摄像头类型*/
    uint32_t         video_resolution;   /* 视频的分辨率  */
    uvc_pix_format_t format;             /* 输出格式*/
    /* 以下为可选配置*/
    int              brightness;         /* 亮度*/
    int              contrast;           /* 对比度*/
    int              hue;                /* 色调*/
    int              saturation;         /* 饱和度*/
    int              sharpness;          /* 锐度*/
    int              gama;               /* 伽马*/
    int              wb;                 /* 白平衡*/
    int              plf;                /* Power Line Frequency，灯管频率*/
};

/** 获取数据流状态*/
uvc_opt_status uvc_dev_stream_status_get(struct uvc_streaming *stream);
/**
 * UVC数据流打开函数
 *
 * @param stream_name UVC数据流名字
 *
 * @return 成功返回UVC数据流
 */
struct uvc_streaming * uvc_dev_stream_open(char *stream_name);
/**
 * UVC数据流关闭函数
 *
 * @param stream UVC数据流
 *
 * @return 成功返回USB_OK
 */
usb_err_t uvc_dev_stream_close(struct uvc_streaming *stream);
/** 数据流获取图像*/
usb_err_t uvc_dev_stream_video_get(struct uvc_streaming *stream,
                                   void                **p_buf,
                                   struct uvc_timeval   *timeval);
/** 数据流释放图像*/
usb_err_t uvc_dev_stream_video_put(struct uvc_streaming *stream, void* p_buf);
/**
 * 获取UVC数据流配置
 *
 * @param stream UVC数据流
 * @param p_cfg  摄像头配置结构体
 *
 * @return 成功返回USB_OK
 */
usb_err_t uvc_dev_stream_cfg_get(struct uvc_streaming *stream, struct uvc_cfg *p_cfg);
/**
 * UVC数据流配置
 *
 * @param stream UVC数据流
 * @param p_cfg  摄像头配置数据
 * @param flag   可选配置标志
 *
 * @return 成功返回USB_OK
 */
usb_err_t uvc_dev_stream_cfg_set(struct uvc_streaming *stream,
                                 struct uvc_cfg       *p_cfg,
                                 uint32_t              flag);
/** 挂起UVC数据流*/
usb_err_t uvc_dev_stream_suspend(struct uvc_streaming *stream);
/** 恢复UVC数据流*/
usb_err_t uvc_dev_stream_resume(struct uvc_streaming *stream);
/**
 * 启动UVC数据流
 *
 * @param stream  UVC数据流
 * @param trp_num 传输请求包数量(2~5)
 *
 * @return 成功返回USB_OK
 */
usb_err_t uvc_dev_stream_start(struct uvc_streaming *stream, uint8_t trp_num);
/** 停止UVC数据流*/
usb_err_t uvc_dev_stream_stop(struct uvc_streaming *stream);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif
