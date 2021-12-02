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

/* USB��Ƶ�����ṹ��*/
struct uac_substream {
    struct uac_stream                *stream;         /* ������UAC�豸������*/
    struct usbh_function             *p_fun;          /* USB�ӿڹ��ܽṹ��*/
    struct uac_pcm_substream         *pcm_substream;  /* PCM����*/
    int                               direction;      /* PCM���򣺻طŻ򲶻�*/
    int                               interface;      /* ��ǰ�ӿں�*/
    int                               endpoint;       /* �˵��*/
    struct audioformat               *cur_audiofmt;   /* ��ǰ����Ƶ��ʽָ��*/
    int                               pcm_format_idx; /* ��ǰ��Ƶ��ʽ����*/
    uint32_t                          channels;       /* ��ǰ��ͨ����*/
    uint32_t                          channels_max;   /* ������Ƶ��ʽ�����ͨ��*/
    uint32_t                          cur_rate;       /* ��ǰ������*/
    uint32_t                          period_bytes;   /* ��ǰ�����ֽ�*/
    uint32_t                          period_frames;  /* ��ǰÿ�����ڵ�֡�� */
    uint32_t                          buffer_periods; /* ��ǰÿ�������������*/
    uint32_t                          altsetting;     /* USB���ݸ�ʽ: �������ú�*/
    uint32_t                          txfr_quirk:1;   /* allow sub-frame alignment */
    uint32_t                          fmt_type;       /* USB ��Ƶ��ʽ���� (1-3) */
    uint32_t                          pkt_offset_adj; /* Ҫ�����ݰ���ͷɾ�����ֽ���(���ڲ��Ϲ���豸)*/
    uint32_t                          maxframesize;   /* ���֡��*/
    uint32_t                          curframesize;   /* ��ǰ֡��*/

    usb_bool_t                        running;        /* ����״̬ */

    uint32_t                          hwptr_done;     /* �����л�ȡ���û����ݵ�λ��*/
    uint32_t                          transfer_done;  /* ���ϴ����ڸ��º����֡��*/
    uint32_t                          frame_limit;    /* USB�����������֡��������*/

    /* ��������������ݺ�ͬ���˵�*/
    uint32_t                          ep_num;         /* �˵��*/
    struct uac_endpoint              *data_endpoint;
    struct uac_endpoint              *sync_endpoint;
    uint32_t                          flags;
    usb_bool_t                        need_setup_ep;  /* �Ƿ����úö˵�׼���շ�����*/
    uint32_t                          speed;          /* USB�豸�ٶ�*/

    uint32_t                          formats;        /* ��ʽλ��*/
    uint32_t                          num_formats;    /* ֧�ֵ���Ƶ�ĸ�ʽ������(����)*/
    struct usb_list_head              fmt_list;       /* ��ʽ����*/
    usb_mutex_handle_t                fmt_list_lock;  /* ��ʽ��������*/
    struct uac_pcm_hw_constraint_list rate_list;      /* ���޲�����*/

    int last_frame_number;                            /* ���µ�����֡���*/
    int last_delay;                                   /* �������µ���ʱ����֡����*/

    struct {
        int marker;
        int channel;
        int byte_idx;
    } dsd_dop;
    /* �Ƿ��������ʱ��*/
    usb_bool_t                         trigger_tstamp_pending_update;
};

/* USB��Ƶ�豸�������ṹ��*/
struct uac_stream {
    struct usbh_audio        *p_uaudio;    /* USB��Ƶ�豸*/
    struct uac_pcm           *pcm;         /* PCMʵ��*/
    int                       pcm_index;   /* PCM����*/
    uint32_t                  fmt_type;    /* USB��Ƶ��ʽ����(1-3)*/
    struct uac_substream      substream[2];/* USB��Ƶ�豸��������*/
    struct usb_list_head      list;        /* ����������ڵ�*/
};

/**
 * ��Ӷ˵㵽оƬʵ�壬���һ�����Ѿ�������˵㣬��׷�ӡ�û���򴴽�һ���µ�PCM��
 *
 * @param p_uaudio   USB��Ƶ�豸�ṹ��
 * @param p_fun      �ض��Ľӿڹ��ܽṹ��
 * @param stream     ����������
 * @param fp         ��Ƶ��ʽ
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uac_add_audio_stream(struct usbh_audio    *p_uaudio,
                               struct usbh_function *p_fun,
                               int                   stream,
                               struct audioformat   *fp);
/** �ͷ�USB��Ƶ������*/
usb_err_t uac_dev_stream_release(struct usbh_audio *p_uaudio);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

