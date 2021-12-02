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
#ifndef __UAC_FORMAT_H_
#define __UAC_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uac/uac_driver.h"

/* PCM格式数据结构体*/
struct pcm_format_data {
    uint8_t width;      /* 位宽*/
    uint8_t phys;       /* 物理位宽*/
    char    le;         /* 0 = 大端, 1 = 小端, -1 = 其他*/
    char    signd;      /* 0 = 无符号, 1 = 有符号, -1 = 其他 */
    uint8_t silence[8]; /* 要填充的静音数据 */
};

/* 音频格式结构体*/
struct audioformat {
    struct usb_list_head       list;                  /* 当前格式节点*/
    uint32_t                   formats;               /* 格式位数*/
    uint32_t                   channels;              /* 通道数*/
    uint32_t                   fmt_type;              /* USB音频格式类型(1-3)*/
    uint32_t                   frame_size;            /* 每一帧的样本大小 */
    int                        iface;                 /* 接口号*/
    uint8_t                    altsetting;            /* 备用设置编号*/
    uint8_t                    attributes;            /* 音频端点属性*/
    uint8_t                    endpoint;              /* 端点号*/
    uint8_t                    ep_attr;               /* 端点属性*/
    uint8_t                    datainterval;          /* 数据包间隔 */
    uint8_t                    protocol;              /* UAC版本，1或2*/
    uint32_t                   maxpacksize;           /*最大包大小*/
    uint32_t                   rates;                 /* 采样率掩码位*/
    uint32_t                   rate_min, rate_max;    /* 最小/最大采样率*/
    uint32_t                   nr_rates;              /* 采样率表入口数量*/
    uint32_t                  *rate_table;            /* 采样率表*/
    uint8_t                    clock;                 /* 相关的时钟*/
    usb_bool_t                 dsd_dop;               /* add DOP headers in case of DSD samples */
    usb_bool_t                 dsd_bitrev;            /* reverse the bits of each DSD sample */
};

/** 获取格式物理位宽*/
usb_err_t uac_pcm_format_physical_width(int format);
/** 获取PCM格式宽度*/
int uac_get_pcm_format_size_bit(uint32_t format_size);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
