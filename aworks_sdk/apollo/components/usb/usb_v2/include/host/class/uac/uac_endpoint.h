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

/* ��Ƶ�˵���չ����*/
#define USB_EP_AUDIO_SIZE  9
/* USB��Ƶ���Ĵ������������*/
#define UAC_MAX_TRPS       12
/* ÿ������������������ݰ���*/
#define UAC_MAX_PACKS      6
/* ����ģʽ��ÿ������������������ݰ���*/
#define UAC_MAX_PACKS_HS   (UAC_MAX_PACKS * 8)
/* 1ms�ﾡ����Ҫ����������г���*/
#define UAC_MAX_QUEUE      18

#define SND_USB_ENDPOINT_TYPE_DATA     0
#define SND_USB_ENDPOINT_TYPE_SYNC     1

struct uac_endpoint;
/* UAC�������������*/
struct uac_trp_ctx {
    struct uac_endpoint      *ep;                            /* ������ض˵�*/
    int                       index;                         /* �����������������*/
    int                       packets;                       /* ÿ��USB��������������ݰ�����*/
    int                       packet_size[UAC_MAX_PACKS_HS]; /* ��һ�ε��ύ�İ��Ĵ�С*/
    uint32_t                  buffer_size;                   /* ���ݻ���Ĵ�С*/
    struct usbh_trp          *trp;                           /* USB���������*/
    struct usb_list_head      ready_list;                    /* ׼���õĻ������Ԥ������*/
};

/* UAC��Ƶ�豸�˵�ṹ��*/
struct uac_endpoint {
    struct usbh_audio         *p_uaudio;            /* ��������Ƶ�豸*/
    struct usbh_endpoint      *p_ep;                /* ��ض˵�*/
    int                        ep_num;              /* �˵��*/
    int                        iface;               /* �����ӿں�*/
    int                        altsetting;          /* �����ӿڵı������ú�*/
    int                        type;                /* ��Ƶ�˵������*/
    int                        refcnt;              /* ���ü���*/
    struct usb_list_head       list;                /* ��Ƶ�˵�ڵ�*/
    struct usb_list_head       ready_playback_trps; /* ׼���طŵĴ��������*/
    uint32_t                   syncinterval;        /* P for adaptive mode, 0 otherwise */
    uint32_t                   syncmaxsize;         /* ͬ���˵����С*/
    uint32_t                   datainterval;        /* log_2�����ݰ�ʱ����*/
    uint32_t                   maxpacksize;         /* ������С(�ֽ�)*/
    uint32_t                   maxframesize;        /* ���ݰ������Ƶ֡��С*/
    uint32_t                   curpacksize;         /* ��ǰ����С(�ֽڣ����ڲ���)*/
    uint32_t                   curframesize;        /* ��ǰ�������Ƶ֡��С(���ڲ���ģʽ)*/
    uint32_t                   fill_max:1;          /* �������������С*/
    uint32_t                   freqn;               /* �� Q16.16��ʽ��fs/fps����ͨ������ */
    uint32_t                   freqm;               /* �� Q16.16��ʽ��fs/fps��˲ʱ������*/
    uint32_t                   freqmax;             /* �������ʣ����ڻ������*/
    int                        freqshift;           /* how much to shift the feedback value to get Q16.16 */
    uint32_t                   phase;               /* phase accumulator */
    uint32_t                   ntrps;               /* ���������������*/
    struct uac_trp_ctx         trp[UAC_MAX_TRPS];   /* UAC���������*/
    uint32_t                   max_trp_frames;      /* �����������Ƶ���������*/
    struct uac_substream      *data_subs;           /* ��������*/
    void (*prepare_data_trp) (struct uac_substream *subs,
                              struct usbh_trp *trp);
    void (*retire_data_trp) (struct uac_substream *subs,
                             struct usbh_trp *trp);
    uint32_t                   stride;
    uint8_t                    silence_value;       /* ����������ֵ*/
    uint32_t                   flags;
    int                        skip_packets;        /* ��Щ�豸�����������ǰ��n����*/
};

/**
 * ���һ���˵㵽USB��Ƶ�豸
 *
 * @param chip      USB��Ƶ�豸�ṹ��
 * @param alts      USB�ӿ�
 * @param ep_num    Ҫʹ�õĶ˵��
 * @param direction PCM����������(�طŻ��߲���)
 * @param type      ��Ƶ�˵�����
 *
 * @return �ɹ�����USB��Ƶ�˵�ṹ��
 */
struct uac_endpoint *uac_dev_endpoint_add(struct usbh_audio     *chip,
                                          struct usbh_interface *alts,
                                          int                    ep_num,
                                          int                    direction,
                                          int                    type);
/** �ͷ�USB��Ƶ�˵�*/
usb_err_t uac_dev_endpoint_release(struct usbh_audio *p_uaudio);
/** ���ö˵����*/
usb_err_t uac_substream_endpoint_config(struct uac_substream *subs);
/**
 * ���������˵�
 *
 * @param subs      USB��Ƶ����
 * @param can_sleep �Ƿ�������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uac_substream_endpoint_start(struct uac_substream *subs,
                                       usb_bool_t            can_sleep);
/** ֹͣ�˵�*/
void uac_endpoint_stop(struct uac_endpoint *ep);
/**
 * �ͷ�һ���˵�
 *
 * @param ep ��Ƶ�豸�˵�
 */
void uac_endpoint_release(struct uac_endpoint *ep);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


