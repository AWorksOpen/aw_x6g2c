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
#ifndef __UAC_OPT_H_
#define __UAC_OPT_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common.h"
#include "usb_errno.h"
#include "list/usb_list.h"
#include <string.h>
#include <stdio.h>

/* USB音频设备句柄*/
typedef void* uac_handle;
/* 默认PCM*/
#define UAC_PCM_DEFAULT  -1

/* 用户硬件参数结构体*/
struct uac_hw_usr_params {
    int      pcm_format_size;  /* PCM格式长度*/
    uint32_t channels;         /* 通道数量*/
    uint32_t rate;             /* 采样率*/
    uint32_t period_frames;    /* 一个周期中帧数量*/
    uint32_t buffer_periods;   /* 一个缓存的周期数量*/
    uint32_t bits_per_sample;  /* 样本的位数*/
};

/**
 * UAC设备打开函数
 *
 * @param uac_dev_idx USB音频设备索引(UAC_DEV_DEFAULT为默认声卡)
 * @param pcm_idx     PCM数据流索引(UAC_PCM_DEFAULT为默认PCM)
 * @param stream      数据流方向(捕获或回放)
 *
 * @return 成功返回操作句柄
 */
uac_handle usbh_audio_dev_open(int uac_dev_idx, int pcm_idx, int stream);
/** UAC设备关闭函数*/
usb_err_t usbh_audio_dev_close(uac_handle handle);
/** UAC设备设置硬件参数*/
usb_err_t usbh_audio_hw_params_set(uac_handle                handle,
                                   struct uac_hw_usr_params *usr_params);
/** UAC设备准备函数*/
usb_err_t usbh_audio_dev_prepare(uac_handle handle);
/**
 * USB音频设备写函数
 *
 * @param handle      USB音频设备句柄
 * @param buffer      数据缓存
 * @param buffer_size 缓存大小
 *
 * @return 成功返回成功传输的字节数
 */
usb_err_t usbh_audio_dev_write(uac_handle handle, char *buffer, uint32_t buffer_size);



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
