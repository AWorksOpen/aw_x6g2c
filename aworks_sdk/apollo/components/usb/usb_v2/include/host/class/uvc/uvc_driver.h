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
#ifndef __UVC_DRIVER_H
#define __UVC_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "time.h"
#include "host/class/uvc/uvc_stream.h"
#include "host/class/uvc/uvc_ctrls.h"

#define __UVC_TRACE_DEBUG(info) \
            do { \
            } while (0)

#define UVC_ALL_DEV -1

/* UVC�ؼ����ƴ��䳬ʱʱ��*/
#define UVC_CTRL_CONTROL_TIMEOUT    300

#define UVC_MATCH_VENDOR          0x0001  /* ƥ��VID*/
#define UVC_MATCH_PRODUCT         0x0002  /* ƥ��PID*/

/* UVC�ӿ��������*/
#define VSC_UNDEFINED                    0x00         /* δ��������*/
#define VSC_VIDEOCONTROL                 0x01         /* ��Ƶ��������*/
#define VSC_VIDEOSTREAMING               0x02         /* ��Ƶ������*/
#define VSC_VIDEO_INTERFACE_COLLECTION   0x03         /* ��Ƶ�ӿڼ�����*/

/* 2.4.2.2. ״̬������*/
#define UVC_STATUS_TYPE_CONTROL             1
#define UVC_STATUS_TYPE_STREAMING           2

/* �豸����*/
#define UVC_QUIRK_FIX_BANDWIDTH     0x00000080
#define UVC_QUIRK_STREAM_NO_FID     0x00000010
/* ------------------------------------------------------------------------
 * ȫ��Ψһ��ʶ��(Globally Unique Identifier,GUIDs)
 */
#define UVC_GUID_UVC_CAMERA \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_OUTPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}
#define UVC_GUID_UVC_SELECTOR \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}

#define UVC_GUID_FORMAT_MJPEG \
    { 'M',  'J',  'P',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2 \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2_ISIGHT \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0x00, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_NV12 \
    { 'N',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YV12 \
    { 'Y',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_I420 \
    { 'I',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_UYVY \
    { 'U',  'Y',  'V',  'Y', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y800 \
    { 'Y',  '8',  '0',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y8 \
    { 'Y',  '8',  ' ',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y10 \
    { 'Y',  '1',  '0',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y12 \
    { 'Y',  '1',  '2',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y16 \
    { 'Y',  '1',  '6',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BY8 \
    { 'B',  'Y',  '8',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BA81 \
    { 'B',  'A',  '8',  '1', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GBRG \
    { 'G',  'B',  'R',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GRBG \
    { 'G',  'R',  'B',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGGB \
    { 'R',  'G',  'G',  'B', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGBP \
    { 'R',  'G',  'B',  'P', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BGR3 \
    { 0x7d, 0xeb, 0x36, 0xe4, 0x4f, 0x52, 0xce, 0x11, \
     0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define UVC_GUID_FORMAT_M420 \
    { 'M',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define UVC_GUID_FORMAT_H264 \
    { 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

/* ÿ��USB��������������*/
#define UVC_MAX_PACKETS     32
/* �ж�URB�����״̬�����ֽڴ�С*/
#define UVC_MAX_STATUS_SIZE 16

/* USB�豸IDƥ��*/
struct usb_device_id {
    uint16_t match_flags;    /* ƥ���־*/
    uint16_t idVendor;       /* VID*/
    uint16_t idProduct;      /* PID*/
};

#define UVC_DEVICE_MATCH(vend, prod) \
    .match_flags =  UVC_MATCH_VENDOR | UVC_MATCH_PRODUCT, \
    .idVendor    = (vend), \
    .idProduct   = (prod)

/* UVC��ṹ��*/
struct uvc_lib{
    struct usb_list_head    uvc_lib_dev_list;       /* ��������UVC�豸����*/
    usb_bool_t              is_lib_init;            /* �Ƿ��ʼ��UVC�豸��*/
    usb_bool_t              is_lib_uniniting;       /* �Ƿ��Ƴ�UVC�豸��*/
    usb_mutex_handle_t      uvc_lib_lock;           /* UVC������*/
    uint8_t                 uvc_lib_stream_num_max; /* �ɴ���UVC�豸���������������*/
    uint8_t                 uvc_lib_stream_num;     /* ��ǰ����UVC�豸����������*/
    uint32_t                uvc_lib_stream_idx;     /* UVC�豸����������*/
    uint8_t                 uvc_lib_dev_num;        /* ��ǰ����UVC�豸������*/
    struct uvc_video_queue *uvc_lib_dev_queue;      /* UVC�������*/
    int                     refcnt;                 /* ���ü���*/
    struct usbh_mem_record  uvc_mem_record;         /* �ڴ��¼*/
};

/* UVC�豸�ṹ��*/
struct uvc_device{
    struct usbh_function  *uvc_fun;                /* ��ص�UBS���ܽṹ��(����һ��USB�豸����)*/
    struct usbh_interface *uvc_controlitf;         /* UVC���ƽӿ�����(����UVC�淶1.5��ÿ����Ƶ����ֻ��һ�����ƽӿ�)*/
    usb_mutex_handle_t     lock;                   /* ������*/
    uint16_t               uvc_version;            /* UVC�淶�汾*/
    uint32_t               clock_frequency;        /* ʱ��Ƶ��*/
    uint32_t               itfstream_altset_num;   /* UVC���ӿڱ������õ�����*/
    struct usb_list_head   streams;                /* ��Ƶ������*/
    struct usb_list_head   entities;               /* UVCʵ������(������Ԫ���ն�)*/
    struct usb_list_head   chains;                 /* ��Ƶ�豸����*/
    struct usb_list_head   node;                   /* ��ǰ��Ƶ�豸����*/
    int                    nstreams;               /* ��Ƶ������*/
    int                    nentities;              /* �ն�����*/
    struct usbh_trp        int_trp;                /* �жϴ��������*/
    struct usbh_endpoint  *int_ep;                 /* �ж϶˵�*/
    uint8_t               *status;                 /* UVC�豸�ж϶˵㻺��*/
    uint32_t               quirks;                 /* �豸����*/
    usb_bool_t             is_remove;              /* �Ƿ��Ƴ�*/
    int                    refcnt;                 /* ���ü���*/
};

/***************************************��������**************************************************/
/**
 * USB��Ƶ��������س�ʼ��
 *
 * @param stream_num_max ���֧��UVC�豸����������
 * @param stream_buf_num �豸�������������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uvc_lib_init(uint8_t stream_num_max, uint8_t stream_buf_num);
/** ����ʼ��USB��Ƶ������*/
usb_err_t uvc_lib_uninit(void);
/** ��ȡUVC�ڴ��¼*/
usb_err_t uvc_lib_mem_record_get(struct usbh_mem_record *mem_record);
/**
 * ���USB�豸�ǲ���һ��UVC�豸
 *
 * @param p_ufun USB���ܽṹ��
 *
 * @return ��UVC�豸����USB_TRUE�����Ƿ���UB_FALSE
 */
usb_bool_t uvc_dev_check_valid(struct usbh_function *p_ufun);
/** ����һ��UVC�豸*/
struct uvc_device *uvc_dev_create(struct usbh_function *p_ufun);
/**
 * ɾ��UVC�豸
 *
 * @param dev UVC�豸�ṹ��
 */
void uvc_dev_destory(struct uvc_device *uvc_dev);
/** ��ȡ��ǰ���ڵ�UVC�豸����*/
usb_err_t uvc_dev_num_get(void);
/**
 * ����PID��VID��ȡUVC�豸
 *
 * @param pid ָ��PID,��ΪUSBDEV_IGONRE_PID�����ƥ��pid
 * @param vid ָ��VID,��ΪUSBDEV_IGONRE_VID�����ƥ��vid
 * @param idx ������ָ��Ҫ��ȡ�ڼ���UVC�豸(��СΪ1)
 *
 * @return �ɹ�����UVC�ṹ��
 */
struct uvc_device *uvc_dev_open(uint16_t pid, uint16_t vid, uint8_t idx);
/** �ر�UVC�豸*/
void uvc_dev_close(struct uvc_device *uvc_dev);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif
