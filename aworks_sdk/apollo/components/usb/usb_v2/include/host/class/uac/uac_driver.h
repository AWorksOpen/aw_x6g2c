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

/* UAC�豸��VID��PID*/
#define UAC_ID(vendor, product) (((vendor) << 16) | (product))
/* ��ȡUAC�豸��VID*/
#define UAC_ID_VENDOR(id)       ((id) >> 16)
/* ��ȡUAC�豸��PID*/
#define UAC_ID_PRODUCT(id)      ((uint16_t)(id))
/* Ĭ������*/
#define UAC_DEV_DEFAULT           -1

#define combine_word(s)    ((*(s)) | ((uint32_t)(s)[1] << 8))
#define combine_triple(s)  (combine_word(s) | ((uint32_t)(s)[2] << 16))
#define combine_quad(s)    (combine_triple(s) | ((uint32_t)(s)[3] << 24))

/* ����ģʽ*/
enum {
    UAC_SND_PCM_STREAM_PLAYBACK = 0,    /* ����ģʽ*/
    UAC_SND_PCM_STREAM_CAPTURE,         /* ����ģʽ(¼��)*/
    UAC_SND_PCM_STREAM_LAST     = UAC_SND_PCM_STREAM_PLAYBACK,
};

/* ʱ���ģʽ*/
enum {
    UAC_SND_PCM_TSTAMP_NONE    = 0,
    UAC_SND_PCM_TSTAMP_ENABLE,
    UAC_SND_PCM_TSTAMP_LAST    = UAC_SND_PCM_TSTAMP_ENABLE,
};

/* USB��Ƶ�豸�ṹ��*/
struct usbh_audio{
    int                    index;            /* ��Ƶ�豸����*/
    struct usbh_function  *p_fun;            /* USB���ƽӿڹ��ܽṹ��*/
    union{
        uint16_t           id[2];            /* ��Ƶ�豸PID*/
        uint32_t           uac_id;
    };
    struct usb_list_head   node;             /* ��Ƶ�豸�ڵ�*/
    char                   name[80];         /* ��Ƶ�豸����*/
    struct usbh_interface *ctrl_intf;        /* ���ƽӿ�*/
    int                    ctrl_intf_num;    /* ���ƽӿ�����*/
    usb_bool_t             txfr_quirk;       /* �Ƿ��д������*/
    int                    pcm_devs;         /* PCM�豸��*/
    struct usb_list_head   pcm_stream_list;  /* PCM����������*/
    usb_mutex_handle_t     pcm_stream_mutex; /* PCM������������*/
    int                    pcm_stream_num;   /* PCM����������*/
    struct usb_list_head   uac_ep_list;      /* ��Ƶ��ض˵������*/
    usb_mutex_handle_t     uac_ep_mutex;     /* ����Ƶ��ض˵���������*/
    int                    ref;              /* ���ü���*/
    usb_bool_t             is_removed;       /* �Ƿ��Ƴ�*/
    usb_bool_t             is_init;          /* �Ƿ��ʼ�����*/
};

/** USB��Ƶ��������س�ʼ��*/
usb_err_t usbh_audio_lib_init(void);
/**
 * ��ȡUSB��Ƶ�豸����
 *
 * @param pid �豸PID
 * @param vid �豸VID
 *
 * @return �ɹ������豸����
 */
int usbh_audio_dev_idx_get(uint16_t pid, uint16_t vid);
/**
 * Ѱ����Ƶ�豸
 *
 * @param uac_dev_idx ��Ƶ�豸����(������ΪUAC_DEV_DEFAULT���ȡ��һ���豸)
 *
 * @return �ɹ�����USB��Ƶ�ṹ��
 */
struct usbh_audio *usbh_audio_dev_find(int uac_dev_idx);
/**
 * ����һ��USB��Ƶ�豸
 *
 * @param p_fun USB�ӿڹ���
 *
 * @return �ɹ�����USB_OK
 */
struct usbh_audio * usbh_audio_dev_create(struct usbh_function *p_fun);
/** ����һ��USB��Ƶ�豸*/
usb_err_t usbh_audio_dev_destory(struct usbh_audio *p_uaudio);
/**
 * UAC���ƴ���
 *
 * @param p_fun       ��صĹ��ܽӿ�
 * @param request     �����USB����
 * @param requesttype ���䷽��|��������|����Ŀ��
 * @param value       ����
 * @param index       ����
 * @param data        ����(����Ϊ��)
 * @param size        ���ݳ���
 *
 * @return ��������ݴ��䣬�ɹ����سɹ�������ֽ���
 *         ���û�����ݴ��䣬�ɹ�����USB_OK
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

