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

/* USB��Ƶ�豸���*/
typedef void* uac_handle;
/* Ĭ��PCM*/
#define UAC_PCM_DEFAULT  -1

/* �û�Ӳ�������ṹ��*/
struct uac_hw_usr_params {
    int      pcm_format_size;  /* PCM��ʽ����*/
    uint32_t channels;         /* ͨ������*/
    uint32_t rate;             /* ������*/
    uint32_t period_frames;    /* һ��������֡����*/
    uint32_t buffer_periods;   /* һ���������������*/
    uint32_t bits_per_sample;  /* ������λ��*/
};

/**
 * UAC�豸�򿪺���
 *
 * @param uac_dev_idx USB��Ƶ�豸����(UAC_DEV_DEFAULTΪĬ������)
 * @param pcm_idx     PCM����������(UAC_PCM_DEFAULTΪĬ��PCM)
 * @param stream      ����������(�����ط�)
 *
 * @return �ɹ����ز������
 */
uac_handle usbh_audio_dev_open(int uac_dev_idx, int pcm_idx, int stream);
/** UAC�豸�رպ���*/
usb_err_t usbh_audio_dev_close(uac_handle handle);
/** UAC�豸����Ӳ������*/
usb_err_t usbh_audio_hw_params_set(uac_handle                handle,
                                   struct uac_hw_usr_params *usr_params);
/** UAC�豸׼������*/
usb_err_t usbh_audio_dev_prepare(uac_handle handle);
/**
 * USB��Ƶ�豸д����
 *
 * @param handle      USB��Ƶ�豸���
 * @param buffer      ���ݻ���
 * @param buffer_size �����С
 *
 * @return �ɹ����سɹ�������ֽ���
 */
usb_err_t usbh_audio_dev_write(uac_handle handle, char *buffer, uint32_t buffer_size);



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
