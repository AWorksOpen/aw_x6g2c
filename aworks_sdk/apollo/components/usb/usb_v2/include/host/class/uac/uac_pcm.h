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
 * PCM(Pulse-code modulation)����������
 * ������ģ���źţ�PCM���ǰ�������ģ��ת���������ź�
 * ����ԭ��������һ���̶���Ƶ�ʶ�ģ���źŽ��в�������������ź��ڲ����Ͽ�����
 * һ�������ķ�ֵ��һ�ĵ����壬����Щ����ķ�ֵ����һ�����ȵ���������Щ������
 * ֵ����������������䣬������¼���洢���ʣ�������Щ�����������Ƶ��
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

#define SNDRV_PCM_RATE_CONTINUOUS   (1<<30)     /* �����Ĳ�����*/
#define SNDRV_PCM_RATE_KNOT         (1<<31)     /* supports more non-continuos rates */

/**
 * USB��Ƶ������
 *
 * ��Ƶ��������ָ�豸��һ�����ڶ������źŵĲ�������
 *
 * �ɼ�һ֡��Ƶ��������Ҫ��ʱ����1000(ms)/�����ʺ���
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

#define UAC_PCM_STATE_OPEN          0  /* ��������״̬*/
#define UAC_PCM_STATE_SETUP         1  /* ��������ʼ��״̬*/
#define UAC_PCM_STATE_PREPARED      2  /* ��������������״̬*/
#define UAC_PCM_STATE_RUNNING       3  /* ����������������un����*/
#define UAC_PCM_STATE_XRUN          4  /* stream reached an xrun */
#define UAC_PCM_STATE_DRAINING      5  /* stream is draining */
#define UAC_PCM_STATE_PAUSED        6  /* ������ֹͣ״̬*/
#define UAC_PCM_STATE_SUSPENDED     7  /* Ӳ������*/
#define UAC_PCM_STATE_DISCONNECTED  8  /* Ӳ��δ����*/
#define UAC_PCM_STATE_LAST          UAC_PCM_STATE_DISCONNECTED

/* USB��Ƶ����ѡ��*/
#define UAC_PCM_TRIGGER_STOP          0
#define UAC_PCM_TRIGGER_START         1
#define UAC_PCM_TRIGGER_PAUSE_PUSH    3
#define UAC_PCM_TRIGGER_PAUSE_RELEASE 4
#define UAC_PCM_TRIGGER_SUSPEND       5
#define UAC_PCM_TRIGGER_RESUME        6
#define UAC_PCM_TRIGGER_DRAIN         7

/* USB��ƵPCM���ƽṹ��*/
struct uac_pcm_control {
    uint32_t appl_ptr;    /* �û�����ָ��(֡)���û�������䵽����ʱ��λ�ã����ֵ��һֱ����ֱ��boundary*/
    uint32_t avail_min;   /* ���ڻ��ѵ���С��Ч֡��*/
};

/* USB��ƵPCM״̬�ṹ��*/
struct uac_pcm_status {
    int                 state;           /* PCM״̬*/
    int                 hw_ptr;          /* ʵ����ɴ����֡�����������ֵ��һֱ����ֱ��boundary */
    struct usb_timespec tstamp;          /* ��ǰ��ʱ���*/
    int                 suspended_state; /* ����������״̬*/
    struct usb_timespec audio_tstamp;    /* ���ŵ���ʱ��*/
};

/* PCM����ʱ��ṹ��*/
struct uac_pcm_runtime {
    /* Ӳ������*/
    struct uac_pcm_hardware       hw;                /* PCMӲ������*/
    struct uac_pcm_hw_constraints hw_constraints;    /* PCMӲ����������*/
    /* ��Ƶ��ʽ����*/
    uint32_t                      channels;          /* ͨ��*/
    int                           format_idx;        /* ��ʽ����*/
    uint32_t                      rate;              /* ������*/
    uint32_t                      periods;           /* ������*/
    uint32_t                      frame_bits;        /* ֡λ��*/
    uint32_t                      sample_bits;       /* ����λ��*/
    uint32_t                      f_period_size;     /* ���ڴ�С(��֡Ϊ��λ)*/
    uint32_t                      f_buffer_size;     /* �����С(��֡Ϊ��λ)*/
    char                         *buffer;            /* ����*/
    uint32_t                      buffer_size;       /* �����С(���ֽ�Ϊ��λ)*/
    /* ���������س�Ա*/
    uint32_t                      min_align;         /* ��ʽ��С����*/
    uint32_t                      byte_align;        /* �ֽڶ���*/
    uint32_t                      period_step;
    uint32_t                      start_threshold;   /* ������(��֡Ϊ��λ)*/
    uint32_t                      stop_threshold;    /* ֹͣ��(��֡Ϊ��λ)*/
    uint32_t                      boundary;          /* �߽磬������޶��ڣ�buffer_frame_size�������������ֵ*/
    struct uac_pcm_status         status;            /* ״̬�ṹ��(ʵ�ʴ���)*/
    struct uac_pcm_control        control;           /* ���ƽṹ��(�û�)*/
    uint32_t                      hw_ptr_base;       /* ��������������λ��*/
    uint32_t                      hw_ptr_interrupt;  /* �ж�ʱ���λ��*/
    uint32_t                      hw_ptr_wrap;       /* ÿ��Խ�߽�ʱ���ͼ�һ�α߽�ֵ*/
    uint32_t                      twake;             /* ����֡��*/
    uint32_t                      avail_max;         /* ������֡��*/
    /* ʱ������*/
    int                           tstamp_mode;       /* ʱ���ģʽ*/
    struct usb_timespec           trigger_tstamp;    /* ����ʱ���*/
    struct usb_timespec           driver_tstamp;     /* ����ʱ���*/
    int                           delay;             /* �ӳٵ�֡��*/
    /* ������ر���*/
    uint32_t                      silence_start;     /* Ҫ���þ�����֡���*/
    uint32_t                      silence_filled;    /* �Ѿ����þ�����֡������*/
    uint32_t                      silence_size;      /* ��Ҫ��侲�����ݴ�С(��֡Ϊ��λ)*/
    uint32_t                      silence_threshold; /* ������(��֡Ϊ��λ)*/

    void                         *private_data;      /* ˽������*/
};

struct uac_pcm_substream;
/* PCM����������*/
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

/* PCM�����ṹ��*/
struct uac_pcm_substream {
    struct uac_pcm           *pcm;                 /* PCMʵ��*/
    struct uac_pcm_str       *pstr;                /* ���������ṹ��*/
    void                     *private_data;        /* ˽������*/
    usb_mutex_handle_t        mutex;               /* ������*/
    int                       number;              /* �������*/
    char                      name[32];            /* ��������*/
    int                       stream;              /* ��(����) */
    size_t                    buffer_bytes_max;    /* ����λ����ڴ�С*/
    const struct uac_pcm_ops *ops;                 /* PCM����������*/
    struct uac_pcm_runtime   *runtime;             /* ����ʱ��*/
    struct uac_pcm_substream *next;                /* ��һ������ */
    usb_bool_t                is_used;             /* �Ƿ�ʹ��*/
    uint32_t                  hw_opened: 1;        /* �Ƿ��Ǵ�״̬*/
    uint32_t                  hw_param_set: 1;     /* �Ƿ�������Ӳ������*/
};

/* PCM���ṹ��*/
struct uac_pcm_str {
    int                       stream;            /* ��(����) */
    struct uac_pcm           *pcm;               /* PCMʵ��*/
    uint32_t                  substream_count;   /* ��������*/
    uint32_t                  substream_opened;  /* �����򿪵�����(���ܴ��ڶ����������)*/
    struct uac_pcm_substream *substream;         /* ����ָ��*/
};

/* PCMʵ��ṹ��*/
struct uac_pcm {
    int                  device;        /* �豸��*/
    uint32_t             info_flags;
    char                 id[64];        /* PCM ID*/
    char                 name[80];      /* PCM ����*/
    struct uac_pcm_str   streams[2];    /* PCM������*/
    void                *private_data;
};

/** �ֽ�ת��Ϊ֡*/
static inline uint32_t bytes_to_frames(struct uac_pcm_runtime *runtime, uint32_t size){
    return size * 8 / runtime->frame_bits;
}

/** ֡ת��Ϊ�ֽ�*/
static inline int frames_to_bytes(struct uac_pcm_runtime *runtime, int size){
    return size * runtime->frame_bits / 8;
}

static inline uint32_t pcm_format_to_bits(int pcm_format){
    return 1ULL << (int) pcm_format;
}

/** ����Ƿ��ֽڶ���*/
static inline int frame_aligned(struct uac_pcm_runtime *runtime, ssize_t bytes){
    return bytes % runtime->byte_align == 0;
}

/** ���PCM���������Ƿ������������ǲ���������*/
static inline int uac_pcm_running(struct uac_pcm_substream *substream){
    return ((substream->runtime->status.state == UAC_PCM_STATE_RUNNING) ||
        ((substream->runtime->status.state == UAC_PCM_STATE_DRAINING) &&
         (substream->stream == 0)));
}

/** ��ȡ����(�ɶ�)�ĻطŻ���ռ�*/
static inline uint32_t uac_pcm_capture_avail(struct uac_pcm_runtime *runtime){
    int avail = runtime->status.hw_ptr - runtime->control.appl_ptr;
    if (avail < 0){
        avail += runtime->boundary;
    }
    return avail;
}

/** ��ȡ����(��д)�Ļ���֡����*/
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
 * ����Ƿ������ݴ����ڻطŻ�����
 *
 * @return ����0��ʾû�������ڻ�����
 */
static inline int uac_pcm_playback_data(struct uac_pcm_substream *substream){
    struct uac_pcm_runtime *runtime = substream->runtime;

    if (runtime->stop_threshold >= runtime->boundary){
        return 1;
    }
    return uac_pcm_playback_avail(runtime) < runtime->f_buffer_size;
}

/** ��ȡ���л���ָ�뵽�û�ָ���֡��*/
static inline int uac_pcm_playback_hw_avail(struct uac_pcm_runtime *runtime){
    return runtime->f_buffer_size - uac_pcm_playback_avail(runtime);
}

/** ��ȡʱ��*/
static inline void uac_pcm_gettime(struct uac_pcm_runtime *runtime,
                                   struct usb_timespec    *tv){
    usb_get_timespec(tv);
}

/** ����PCM����������*/
void uac_set_pcm_ops(struct uac_pcm *pcm, int stream);
/**
 * ����һ���µ�PCM������
 *
 * @param pcm             PCMʵ��
 * @param stream          ����������
 * @param substream_count ��������������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uac_pcm_new_stream(struct uac_pcm *pcm,
                             int             stream,
                             int             substream_count);
/** ����һ���µ�PCM*/
usb_err_t uac_pcm_new(const char      *id,
                      int              device,
                      int              playback_count,
                      int              capture_count,
                      struct uac_pcm **rpcm);
/** �ͷ���ƵPCM*/
void uac_pcm_free(struct uac_pcm *pcm);
/**
 * ��һ��PCM������
 *
 * @param pcm        PCM�豸
 * @param stream     Ҫ�򿪵����������ķ���
 * @param rsubstream ���ش򿪵���������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uac_pcm_open_substream(struct uac_pcm            *pcm,
                                 int                        stream,
                                 struct uac_pcm_substream **rsubstream);
/** �ر�һ��PCM��������*/
usb_err_t uac_pcm_close_substream(struct uac_pcm_substream *p_substream);
/** ��λPCM*/
usb_err_t uac_pcm_reset(struct uac_pcm_substream *substream,
                        int                       state,
                        usb_bool_t                complete_reset);
/**
 * USB��ƵPCMд����
 *
 * @param substream   PCM���������ṹ��
 * @param buffer      ����
 * @param buffer_size �����֡��С
 *
 * @param �ɹ����سɹ������֡��
 */
int uac_pcm_write(struct uac_pcm_substream *substream,
                  char                     *buffer,
                  uint32_t                  buffer_frame_size);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


