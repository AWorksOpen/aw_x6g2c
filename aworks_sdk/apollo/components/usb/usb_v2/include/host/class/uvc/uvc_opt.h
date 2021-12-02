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

/* ��ѡ���ñ�־*/
#define UVC_BRIGHTNESS_CFG (1 << 0)
#define UVC_CONTRAST_CFG   (1 << 1)
#define UVC_HUE_CFG        (1 << 2)
#define UVC_SATURARION_CFG (1 << 3)
#define UVC_SHARPNESS_CFG  (1 << 4)
#define UVC_GAMA_CFG       (1 << 5)
#define UVC_WB_CFG         (1 << 6)
#define UVC_PLF_CFG        (1 << 7)

/* UVC��ʽ���ýṹ��*/
typedef struct uvc_format_cfg {
    uint32_t                type;  /* ��ʽ����*/
    struct uvc_pix_format   pix;   /* ���ص��ʽ*/

}uvc_format_cfg_t;

/* UVC���ýṹ��*/
struct uvc_cfg {
    /* ����Ϊ��ѡ����*/
    uint32_t         usb_camera_type;    /* USB����ͷ����*/
    uint32_t         video_resolution;   /* ��Ƶ�ķֱ���  */
    uvc_pix_format_t format;             /* �����ʽ*/
    /* ����Ϊ��ѡ����*/
    int              brightness;         /* ����*/
    int              contrast;           /* �Աȶ�*/
    int              hue;                /* ɫ��*/
    int              saturation;         /* ���Ͷ�*/
    int              sharpness;          /* ���*/
    int              gama;               /* ٤��*/
    int              wb;                 /* ��ƽ��*/
    int              plf;                /* Power Line Frequency���ƹ�Ƶ��*/
};

/** ��ȡ������״̬*/
uvc_opt_status uvc_dev_stream_status_get(struct uvc_streaming *stream);
/**
 * UVC�������򿪺���
 *
 * @param stream_name UVC����������
 *
 * @return �ɹ�����UVC������
 */
struct uvc_streaming * uvc_dev_stream_open(char *stream_name);
/**
 * UVC�������رպ���
 *
 * @param stream UVC������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uvc_dev_stream_close(struct uvc_streaming *stream);
/** ��������ȡͼ��*/
usb_err_t uvc_dev_stream_video_get(struct uvc_streaming *stream,
                                   void                **p_buf,
                                   struct uvc_timeval   *timeval);
/** �������ͷ�ͼ��*/
usb_err_t uvc_dev_stream_video_put(struct uvc_streaming *stream, void* p_buf);
/**
 * ��ȡUVC����������
 *
 * @param stream UVC������
 * @param p_cfg  ����ͷ���ýṹ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uvc_dev_stream_cfg_get(struct uvc_streaming *stream, struct uvc_cfg *p_cfg);
/**
 * UVC����������
 *
 * @param stream UVC������
 * @param p_cfg  ����ͷ��������
 * @param flag   ��ѡ���ñ�־
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uvc_dev_stream_cfg_set(struct uvc_streaming *stream,
                                 struct uvc_cfg       *p_cfg,
                                 uint32_t              flag);
/** ����UVC������*/
usb_err_t uvc_dev_stream_suspend(struct uvc_streaming *stream);
/** �ָ�UVC������*/
usb_err_t uvc_dev_stream_resume(struct uvc_streaming *stream);
/**
 * ����UVC������
 *
 * @param stream  UVC������
 * @param trp_num �������������(2~5)
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uvc_dev_stream_start(struct uvc_streaming *stream, uint8_t trp_num);
/** ֹͣUVC������*/
usb_err_t uvc_dev_stream_stop(struct uvc_streaming *stream);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif
