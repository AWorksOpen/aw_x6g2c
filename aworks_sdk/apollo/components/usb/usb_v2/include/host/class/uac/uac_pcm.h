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
/**
 * PCM(Pulse-code modulation)脉冲编码调制
 * 声音是模拟信号，PCM就是把声音从模拟转换成数字信号
 * 它的原理是利用一个固定的频率对模拟信号进行采样，采样后的信号在波形上看就像
 * 一串连续的幅值不一的的脉冲，把这些脉冲的幅值进行一定精度的量化，这些量化的
 * 值被连续的输出，传输，处理或记录到存储介质，所有这些组成了数字音频。
 */
#ifndef __UAC_PCM_H_
#define __UAC_PCM_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uac/uac_stream.h"
#include "host/class/uac/uac_hw_rule.h"
#include "usb_adapter_inter.h"
#include <string.h>
#include <stdio.h>

#define SNDRV_PCM_FORMAT_S8                 0
#define SNDRV_PCM_FORMAT_U8                 1
#define SNDRV_PCM_FORMAT_S16_LE             2
#define SNDRV_PCM_FORMAT_S16_BE             3
#define SNDRV_PCM_FORMAT_U16_LE             4
#define SNDRV_PCM_FORMAT_U16_BE             5
#define SNDRV_PCM_FORMAT_S24_LE             6          /* low three bytes */
#define SNDRV_PCM_FORMAT_S24_BE             7          /* low three bytes */
#define SNDRV_PCM_FORMAT_U24_LE             8          /* low three bytes */
#define SNDRV_PCM_FORMAT_U24_BE             9          /* low three bytes */
#define SNDRV_PCM_FORMAT_S32_LE             10
#define SNDRV_PCM_FORMAT_S32_BE             11
#define SNDRV_PCM_FORMAT_U32_LE             12
#define SNDRV_PCM_FORMAT_U32_BE             13
#define SNDRV_PCM_FORMAT_FLOAT_LE           14
#define SNDRV_PCM_FORMAT_FLOAT_BE           15          /* 4-byte float, IEEE-754 32-bit, range -1.0 to 1.0 */
#define SNDRV_PCM_FORMAT_FLOAT64_LE         16          /* 8-byte float, IEEE-754 64-bit, range -1.0 to 1.0 */
#define SNDRV_PCM_FORMAT_FLOAT64_BE         17          /* 8-byte float, IEEE-754 64-bit, range -1.0 to 1.0 */
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME_LE 18  /* IEC-958 subframe, Little Endian */
#define SNDRV_PCM_FORMAT_IEC958_SUBFRAME_BE 19  /* IEC-958 subframe, Big Endian */
#define SNDRV_PCM_FORMAT_MU_LAW             20
#define SNDRV_PCM_FORMAT_A_LAW              21
#define SNDRV_PCM_FORMAT_IMA_ADPCM          22
#define SNDRV_PCM_FORMAT_MPEG               23
#define SNDRV_PCM_FORMAT_GSM                24
#define SNDRV_PCM_FORMAT_SPECIAL            31
#define SNDRV_PCM_FORMAT_S24_3LE            32          /* in three bytes */
#define SNDRV_PCM_FORMAT_S24_3BE            33          /* in three bytes */
#define SNDRV_PCM_FORMAT_U24_3LE            34          /* in three bytes */
#define SNDRV_PCM_FORMAT_U24_3BE            35          /* in three bytes */
#define SNDRV_PCM_FORMAT_S20_3LE            36          /* in three bytes */
#define SNDRV_PCM_FORMAT_S20_3BE            37          /* in three bytes */
#define SNDRV_PCM_FORMAT_U20_3LE            38          /* in three bytes */
#define SNDRV_PCM_FORMAT_U20_3BE            39          /* in three bytes */
#define SNDRV_PCM_FORMAT_S18_3LE            40          /* in three bytes */
#define SNDRV_PCM_FORMAT_S18_3BE            41          /* in three bytes */
#define SNDRV_PCM_FORMAT_U18_3LE            42          /* in three bytes */
#define SNDRV_PCM_FORMAT_U18_3BE            43          /* in three bytes */
#define SNDRV_PCM_FORMAT_G723_24            44          /* 8 samples in 3 bytes */
#define SNDRV_PCM_FORMAT_G723_24_1B         45          /* 1 sample in 1 byte */
#define SNDRV_PCM_FORMAT_G723_40            46          /* 8 Samples in 5 bytes */
#define SNDRV_PCM_FORMAT_G723_40_1B         47          /* 1 sample in 1 byte */
#define SNDRV_PCM_FORMAT_DSD_U8             48          /* DSD, 1-byte samples DSD (x8) */
#define SNDRV_PCM_FORMAT_DSD_U16_LE         49          /* DSD, 2-byte samples DSD (x16), little endian */
#define SNDRV_PCM_FORMAT_DSD_U32_LE         50          /* DSD, 4-byte samples DSD (x32), little endian */
#define SNDRV_PCM_FORMAT_DSD_U16_BE         51          /* DSD, 2-byte samples DSD (x16), big endian */
#define SNDRV_PCM_FORMAT_DSD_U32_BE         52          /* DSD, 4-byte samples DSD (x32), big endian */
#define SNDRV_PCM_FORMAT_LAST               SNDRV_PCM_FORMAT_DSD_U32_BE

#define SNDRV_PCM_FMTBIT_S8         (1 << SNDRV_PCM_FORMAT_S8)
#define SNDRV_PCM_FMTBIT_U8         (1 << SNDRV_PCM_FORMAT_U8)
#define SNDRV_PCM_FMTBIT_S16_LE     (1 << SNDRV_PCM_FORMAT_S16_LE)
#define SNDRV_PCM_FMTBIT_S16_BE     (1 << SNDRV_PCM_FORMAT_S16_BE)
#define SNDRV_PCM_FMTBIT_S32_LE     (1 << SNDRV_PCM_FORMAT_S32_LE)
#define SNDRV_PCM_FMTBIT_FLOAT_LE   (1 << SNDRV_PCM_FORMAT_FLOAT_LE)
#define SNDRV_PCM_FMTBIT_MU_LAW     (1 << SNDRV_PCM_FORMAT_MU_LAW)
#define SNDRV_PCM_FMTBIT_A_LAW      (1 << SNDRV_PCM_FORMAT_A_LAW)
#define SNDRV_PCM_FMTBIT_SPECIAL    (1 << SNDRV_PCM_FORMAT_SPECIAL)

#define SNDRV_PCM_RATE_CONTINUOUS   (1<<30)     /* 连续的采样率*/
#define SNDRV_PCM_RATE_KNOT         (1<<31)     /* supports more non-continuos rates */

/**
 * USB音频采样率
 *
 * 音频采样率是指设备在一秒中内对声音信号的采样次数
 *
 * 采集一帧音频数据所需要的时间是1000(ms)/采样率毫秒
 */
#define SNDRV_PCM_RATE_5512         (1<<0)      /* 5512Hz */
#define SNDRV_PCM_RATE_8000         (1<<1)      /* 8000Hz */
#define SNDRV_PCM_RATE_11025        (1<<2)      /* 11025Hz */
#define SNDRV_PCM_RATE_16000        (1<<3)      /* 16000Hz */
#define SNDRV_PCM_RATE_22050        (1<<4)      /* 22050Hz */
#define SNDRV_PCM_RATE_32000        (1<<5)      /* 32000Hz */
#define SNDRV_PCM_RATE_44100        (1<<6)      /* 44100Hz */
#define SNDRV_PCM_RATE_48000        (1<<7)      /* 48000Hz */
#define SNDRV_PCM_RATE_64000        (1<<8)      /* 64000Hz */
#define SNDRV_PCM_RATE_88200        (1<<9)      /* 88200Hz */
#define SNDRV_PCM_RATE_96000        (1<<10)     /* 96000Hz */
#define SNDRV_PCM_RATE_176400       (1<<11)     /* 176400Hz */
#define SNDRV_PCM_RATE_192000       (1<<12)     /* 192000Hz */

#define UAC_PCM_RUNTIME_CHECK(sub)             ((sub == NULL) || ((sub)->runtime == NULL))
#define uac_pcm_substream_priv_get(substream)  ((substream)->private_data)

#define UAC_PCM_STATE_OPEN          0  /* 数据流打开状态*/
#define UAC_PCM_STATE_SETUP         1  /* 数据流初始化状态*/
#define UAC_PCM_STATE_PREPARED      2  /* 数据流可以启动状态*/
#define UAC_PCM_STATE_RUNNING       3  /* 数据流正在运行扎un给他*/
#define UAC_PCM_STATE_XRUN          4  /* stream reached an xrun */
#define UAC_PCM_STATE_DRAINING      5  /* stream is draining */
#define UAC_PCM_STATE_PAUSED        6  /* 数据流停止状态*/
#define UAC_PCM_STATE_SUSPENDED     7  /* 硬件挂起*/
#define UAC_PCM_STATE_DISCONNECTED  8  /* 硬件未连接*/
#define UAC_PCM_STATE_LAST          UAC_PCM_STATE_DISCONNECTED

/* USB音频触发选项*/
#define UAC_PCM_TRIGGER_STOP          0
#define UAC_PCM_TRIGGER_START         1
#define UAC_PCM_TRIGGER_PAUSE_PUSH    3
#define UAC_PCM_TRIGGER_PAUSE_RELEASE 4
#define UAC_PCM_TRIGGER_SUSPEND       5
#define UAC_PCM_TRIGGER_RESUME        6
#define UAC_PCM_TRIGGER_DRAIN         7

/* USB音频PCM控制结构体*/
struct uac_pcm_control {
    uint32_t appl_ptr;    /* 用户缓存指针(帧)，用户数据填充到缓存时的位置，这个值会一直增加直到boundary*/
    uint32_t avail_min;   /* 用于唤醒的最小有效帧数*/
};

/* USB音频PCM状态结构体*/
struct uac_pcm_status {
    int                 state;           /* PCM状态*/
    int                 hw_ptr;          /* 实际完成传输的帧的数量，这个值会一直增加直到boundary */
    struct usb_timespec tstamp;          /* 当前的时间戳*/
    int                 suspended_state; /* 数据流挂起状态*/
    struct usb_timespec audio_tstamp;    /* 播放的总时间*/
};

/* PCM运行时间结构体*/
struct uac_pcm_runtime {
    /* 硬件描述*/
    struct uac_pcm_hardware       hw;                /* PCM硬件描述*/
    struct uac_pcm_hw_constraints hw_constraints;    /* PCM硬件参数容器*/
    /* 音频格式描述*/
    uint32_t                      channels;          /* 通道*/
    int                           format_idx;        /* 格式索引*/
    uint32_t                      rate;              /* 采样率*/
    uint32_t                      periods;           /* 周期数*/
    uint32_t                      frame_bits;        /* 帧位数*/
    uint32_t                      sample_bits;       /* 样本位数*/
    uint32_t                      f_period_size;     /* 周期大小(以帧为单位)*/
    uint32_t                      f_buffer_size;     /* 缓存大小(以帧为单位)*/
    char                         *buffer;            /* 缓存*/
    uint32_t                      buffer_size;       /* 缓存大小(以字节为单位)*/
    /* 传输机制相关成员*/
    uint32_t                      min_align;         /* 格式最小对齐*/
    uint32_t                      byte_align;        /* 字节对齐*/
    uint32_t                      period_step;
    uint32_t                      start_threshold;   /* 启动点(以帧为单位)*/
    uint32_t                      stop_threshold;    /* 停止点(以帧为单位)*/
    uint32_t                      boundary;          /* 边界，在最大限度内，buffer_frame_size的最大整数倍的值*/
    struct uac_pcm_status         status;            /* 状态结构体(实际传输)*/
    struct uac_pcm_control        control;           /* 控制结构体(用户)*/
    uint32_t                      hw_ptr_base;       /* 缓存重新启动的位置*/
    uint32_t                      hw_ptr_interrupt;  /* 中断时间的位置*/
    uint32_t                      hw_ptr_wrap;       /* 每跨越边界时，就加一次边界值*/
    uint32_t                      twake;             /* 唤醒帧数*/
    uint32_t                      avail_max;         /* 最大可用帧数*/
    /* 时间戳相关*/
    int                           tstamp_mode;       /* 时间戳模式*/
    struct usb_timespec           trigger_tstamp;    /* 触发时间戳*/
    struct usb_timespec           driver_tstamp;     /* 驱动时间戳*/
    int                           delay;             /* 延迟的帧数*/
    /* 静音相关变量*/
    uint32_t                      silence_start;     /* 要设置静音的帧起点*/
    uint32_t                      silence_filled;    /* 已经设置静音的帧的数量*/
    uint32_t                      silence_size;      /* 需要填充静音数据大小(以帧为单位)*/
    uint32_t                      silence_threshold; /* 静音点(以帧为单位)*/

    void                         *private_data;      /* 私有数据*/
};

struct uac_pcm_substream;
/* PCM操作函数集*/
struct uac_pcm_ops {
    usb_err_t (*open)(struct uac_pcm_substream *substream);
    usb_err_t (*close)(struct uac_pcm_substream *substream);
    usb_err_t (*prepare)(struct uac_pcm_substream *substream);
    usb_err_t (*hw_params)(struct uac_pcm_substream *substream,
                          struct uac_pcm_hw_params *hw_params);
    usb_err_t (*hw_free)(struct uac_pcm_substream *substream);
    usb_err_t (*trigger)(struct uac_pcm_substream *substream, int cmd);
    int       (*pointer)(struct uac_pcm_substream *substream);
    usb_err_t (*fifo_size)(struct uac_pcm_substream * substream, void *arg);
    int       (*silence)(struct uac_pcm_substream *substream,
                         int                       channel,
                         uint32_t                  pos,
                         uint32_t                  count);
};

/* PCM子流结构体*/
struct uac_pcm_substream {
    struct uac_pcm           *pcm;                 /* PCM实体*/
    struct uac_pcm_str       *pstr;                /* 所属的流结构体*/
    void                     *private_data;        /* 私有数据*/
    usb_mutex_handle_t        mutex;               /* 互斥锁*/
    int                       number;              /* 子流编号*/
    char                      name[32];            /* 子流名字*/
    int                       stream;              /* 流(方向) */
    size_t                    buffer_bytes_max;    /* 最大环形缓冲期大小*/
    const struct uac_pcm_ops *ops;                 /* PCM操作函数集*/
    struct uac_pcm_runtime   *runtime;             /* 运行时间*/
    struct uac_pcm_substream *next;                /* 下一个子流 */
    usb_bool_t                is_used;             /* 是否被使用*/
    uint32_t                  hw_opened: 1;        /* 是否是打开状态*/
    uint32_t                  hw_param_set: 1;     /* 是否设置了硬件参数*/
};

/* PCM流结构体*/
struct uac_pcm_str {
    int                       stream;            /* 流(方向) */
    struct uac_pcm           *pcm;               /* PCM实体*/
    uint32_t                  substream_count;   /* 子流数量*/
    uint32_t                  substream_opened;  /* 子流打开的数量(可能存在多个子数据流)*/
    struct uac_pcm_substream *substream;         /* 子流指针*/
};

/* PCM实体结构体*/
struct uac_pcm {
    int                  device;        /* 设备号*/
    uint32_t             info_flags;
    char                 id[64];        /* PCM ID*/
    char                 name[80];      /* PCM 名字*/
    struct uac_pcm_str   streams[2];    /* PCM数据流*/
    void                *private_data;
};

/** 字节转换为帧*/
static inline uint32_t bytes_to_frames(struct uac_pcm_runtime *runtime, uint32_t size){
    return size * 8 / runtime->frame_bits;
}

/** 帧转换为字节*/
static inline int frames_to_bytes(struct uac_pcm_runtime *runtime, int size){
    return size * runtime->frame_bits / 8;
}

static inline uint32_t pcm_format_to_bits(int pcm_format){
    return 1ULL << (int) pcm_format;
}

/** 检查是否字节对齐*/
static inline int frame_aligned(struct uac_pcm_runtime *runtime, ssize_t bytes){
    return bytes % runtime->byte_align == 0;
}

/** 检查PCM子数据流是否在运行中且是播放数据流*/
static inline int uac_pcm_running(struct uac_pcm_substream *substream){
    return ((substream->runtime->status.state == UAC_PCM_STATE_RUNNING) ||
        ((substream->runtime->status.state == UAC_PCM_STATE_DRAINING) &&
         (substream->stream == 0)));
}

/** 获取可用(可读)的回放缓存空间*/
static inline uint32_t uac_pcm_capture_avail(struct uac_pcm_runtime *runtime){
    int avail = runtime->status.hw_ptr - runtime->control.appl_ptr;
    if (avail < 0){
        avail += runtime->boundary;
    }
    return avail;
}

/** 获取可用(可写)的缓存帧数量*/
static inline uint32_t uac_pcm_playback_avail(struct uac_pcm_runtime *runtime){
    int avail = runtime->status.hw_ptr + runtime->f_buffer_size - runtime->control.appl_ptr;
    if (avail < 0){
        avail += runtime->boundary;
    }
    else if ((uint32_t) avail >= runtime->boundary){
        avail -= runtime->boundary;
    }
    return avail;
}

/**
 * 检查是否有数据存在在回放缓存中
 *
 * @return 返回0表示没有数据在缓存中
 */
static inline int uac_pcm_playback_data(struct uac_pcm_substream *substream){
    struct uac_pcm_runtime *runtime = substream->runtime;

    if (runtime->stop_threshold >= runtime->boundary){
        return 1;
    }
    return uac_pcm_playback_avail(runtime) < runtime->f_buffer_size;
}

/** 获取运行缓存指针到用户指针的帧数*/
static inline int uac_pcm_playback_hw_avail(struct uac_pcm_runtime *runtime){
    return runtime->f_buffer_size - uac_pcm_playback_avail(runtime);
}

/** 获取时间*/
static inline void uac_pcm_gettime(struct uac_pcm_runtime *runtime,
                                   struct usb_timespec    *tv){
    usb_get_timespec(tv);
}

/** 设置PCM操作函数集*/
void uac_set_pcm_ops(struct uac_pcm *pcm, int stream);
/**
 * 创建一个新的PCM数据流
 *
 * @param pcm             PCM实体
 * @param stream          数据流方向
 * @param substream_count 子数据流的数量
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_pcm_new_stream(struct uac_pcm *pcm,
                             int             stream,
                             int             substream_count);
/** 创建一个新的PCM*/
usb_err_t uac_pcm_new(const char      *id,
                      int              device,
                      int              playback_count,
                      int              capture_count,
                      struct uac_pcm **rpcm);
/** 释放音频PCM*/
void uac_pcm_free(struct uac_pcm *pcm);
/**
 * 打开一个PCM的子流
 *
 * @param pcm        PCM设备
 * @param stream     要打开的子数据流的方向
 * @param rsubstream 返回打开的子数据流
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_pcm_open_substream(struct uac_pcm            *pcm,
                                 int                        stream,
                                 struct uac_pcm_substream **rsubstream);
/** 关闭一个PCM子数据流*/
usb_err_t uac_pcm_close_substream(struct uac_pcm_substream *p_substream);
/** 复位PCM*/
usb_err_t uac_pcm_reset(struct uac_pcm_substream *substream,
                        int                       state,
                        usb_bool_t                complete_reset);
/**
 * USB音频PCM写函数
 *
 * @param substream   PCM子数据流结构体
 * @param buffer      缓存
 * @param buffer_size 缓存的帧大小
 *
 * @param 成功返回成功传输的帧数
 */
int uac_pcm_write(struct uac_pcm_substream *substream,
                  char                     *buffer,
                  uint32_t                  buffer_frame_size);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


