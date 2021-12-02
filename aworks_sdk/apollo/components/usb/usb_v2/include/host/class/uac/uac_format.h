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

/* PCM��ʽ���ݽṹ��*/
struct pcm_format_data {
    uint8_t width;      /* λ��*/
    uint8_t phys;       /* ����λ��*/
    char    le;         /* 0 = ���, 1 = С��, -1 = ����*/
    char    signd;      /* 0 = �޷���, 1 = �з���, -1 = ���� */
    uint8_t silence[8]; /* Ҫ���ľ������� */
};

/* ��Ƶ��ʽ�ṹ��*/
struct audioformat {
    struct usb_list_head       list;                  /* ��ǰ��ʽ�ڵ�*/
    uint32_t                   formats;               /* ��ʽλ��*/
    uint32_t                   channels;              /* ͨ����*/
    uint32_t                   fmt_type;              /* USB��Ƶ��ʽ����(1-3)*/
    uint32_t                   frame_size;            /* ÿһ֡��������С */
    int                        iface;                 /* �ӿں�*/
    uint8_t                    altsetting;            /* �������ñ��*/
    uint8_t                    attributes;            /* ��Ƶ�˵�����*/
    uint8_t                    endpoint;              /* �˵��*/
    uint8_t                    ep_attr;               /* �˵�����*/
    uint8_t                    datainterval;          /* ���ݰ���� */
    uint8_t                    protocol;              /* UAC�汾��1��2*/
    uint32_t                   maxpacksize;           /*������С*/
    uint32_t                   rates;                 /* ����������λ*/
    uint32_t                   rate_min, rate_max;    /* ��С/��������*/
    uint32_t                   nr_rates;              /* �����ʱ��������*/
    uint32_t                  *rate_table;            /* �����ʱ�*/
    uint8_t                    clock;                 /* ��ص�ʱ��*/
    usb_bool_t                 dsd_dop;               /* add DOP headers in case of DSD samples */
    usb_bool_t                 dsd_bitrev;            /* reverse the bits of each DSD sample */
};

/** ��ȡ��ʽ����λ��*/
usb_err_t uac_pcm_format_physical_width(int format);
/** ��ȡPCM��ʽ���*/
int uac_get_pcm_format_size_bit(uint32_t format_size);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
