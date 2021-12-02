/*******************************************************************************
*                                    USB
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "host/class/uvc/uvc_opt.h"
#include "apollo.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "host/awbl_usbh.h"
#include "host/class/uvc/awbl_usbh_uvcvideo.h"
#include "aw_mem.h"
#include "aw_common.h"
#include "string.h"
#include <stdio.h>

/* UVC��������*/
#define AWBL_USBH_UVC_DRV_NAME   "awbl_usbh_uvc"

/**
 * UVC�򿪺���
 *
 * @param p_cookie ����ͷ�����Զ�������
 *
 * @return �ɹ�����AW_OK
 */
static aw_err_t __uvc_open(void *p_cookie)
{
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    void                 *handle;

    if(p_stream == NULL){
        return -AW_EINVAL;
    }
    handle = uvc_dev_stream_open(p_stream->name);
    if(handle == NULL){
        return -AW_EPERM;
    }

    return AW_OK;
}

/** UVC�رպ���*/
static aw_err_t __uvc_close(void *p_cookie)
{
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;

    if(p_stream == NULL){
        return -AW_EINVAL;
    }

    return uvc_dev_stream_close(p_stream);
}

static aw_err_t __uvc_get_photo_buf(void *p_cookie, uint32_t **p_buf)
{
    return AW_OK;
}

/**
 * UVC��ȡ��Ƶ����
 *
 * @param p_cookie ����ͷ�����Զ�������
 * @param p_buf    �����ַ�ĵ�ַ
 *
 * @return �ɹ�����AW_OK
 */
static aw_err_t __uvc_op_get_video_buf(void *p_cookie, uint32_t **p_buf)
{
    aw_err_t ret;
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    /* �������Ϸ���*/
    if(p_stream == NULL)
        return -AW_EINVAL;
    /* ��ȡͼ��*/
    ret = uvc_dev_stream_video_get(p_stream, (void **)p_buf, NULL);
    return ret;
}

static aw_err_t __uvc_release_photo_buf(void *p_cookie, uint32_t addr)
{
    return AW_OK;
}

/**
 * UVC�ͷ���Ƶ����
 *
 * @param p_cookie ����ͷ�����Զ�������
 * @param addr     �����ַ
 *
 * @return �ɹ�����AW_OK
 */
static aw_err_t __uvc_op_release_video_buf(void *p_cookie, uint32_t addr)
{
    int ret;
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    /* �������Ϸ���*/
    if(p_stream == NULL)
        return -AW_EINVAL;
    ret = uvc_dev_stream_video_put(p_stream, (uint8_t *)addr);
    return ret;
}

/**
 * UVC����
 *
 * @param p_cookie ����ͷ�����Զ�������
 * @param p_cfg    �û���������
 * @param option   ѡ��
 *
 * ���أ��ɹ�����AW_OK
 */
static aw_err_t __uvc_cfg_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    struct uvc_cfg        p_uvc_cfg;
    aw_err_t              ret;

    if((p_stream == NULL) || (p_cfg == NULL))
        return -AW_EINVAL;

    /* ����ͷ֧�ֵ����� */
    *option &= AW_CAMERA_RESOLUTION|AW_CAMERA_FORMAT|AW_CAMERA_USB_TYPE;

    p_uvc_cfg.video_resolution = p_cfg->video_resolution;
    if(p_cfg->format == YUV422){
        p_uvc_cfg.format = UVC_YUV422;
    }
    else{
        return -AW_EINVAL;
    }
    p_uvc_cfg.usb_camera_type = p_cfg->usb_camera_type;

    ret = uvc_dev_stream_cfg_set(p_stream, &p_uvc_cfg, 0);
    if(ret == AW_OK){
        return uvc_dev_stream_start(p_stream, 2);
    }
    return -AW_EPERM;
}

static aw_err_t __uvc_cfg_get(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    return AW_OK;
}

/* UVC����ͷ��������*/
aw_local struct awbl_camera_servfuncs __g_uvc_servfuncs = {
        __uvc_open,                    /* UVC�򿪺���*/
        __uvc_close,                   /* UVC�رպ���*/
        __uvc_get_photo_buf,           /* UVC��ȡͼƬ����*/
        __uvc_op_get_video_buf,        /* UVC��ȡ��Ƶͼ�񻺴溯��*/
        __uvc_release_photo_buf,       /* UVC�ͷ�ͼƬ����*/
        __uvc_op_release_video_buf,    /* UVC�ͷ���Ƶͼ�񻺴�*/
        __uvc_cfg_set,                 /* UVC�������ú���*/
        __uvc_cfg_get,                 /* UVC��ȡ���ú���*/
};

/* USB����������Ϣ��*/
aw_local const struct awbl_usbh_fundrv_info_tab __uvc_drv_info_tab[] = {

    {
        .flags = USB_UVC_DEVICE_MATCH_INT_INFO,                                  /* ����ƥ���־(ƥ���࣬�����Э��)*/
        .iclss = USB_CLASS_VIDEO,                                                /* ��Ƶ��*/
        .isclss = VSC_VIDEO_INTERFACE_COLLECTION,                                /* ��Ƶ�ӿڼ�������*/
        .iproto = 0
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/** ����̽�⺯��*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    /* ͨ�����������ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* ����ǲ���һ����Ч��UVC�豸*/
    return uvc_dev_check_valid(p_fun->usb_func);
}

/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* ͨ�������豸���ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct uvc_device    *p_udev   = NULL;
    struct uvc_streaming *p_stream = NULL;
    char                 *p_name   = NULL;
    int                   num, i;
    aw_err_t              ret;

    __AUVC_TRACE("find a generic UVC device\r\n");

    /* ����һ��UVC�豸*/
    p_udev = uvc_dev_create(p_fun->usb_func);
    if(p_udev == NULL){
        __AUVC_TRACE("UVC device create failed\r\n");
        return;
    }
    /* ��������˽������*/
    usbh_func_drvdata_set(p_fun->usb_func, p_udev);
    /* ��ȡ����ͷ����������*/
    num = uvc_dev_nstream_get(p_udev);
    __AUVC_TRACE("UVC device has %d camera stream(s)\r\n", num);


    for(i = 0;i < num; i++){
        p_name = uvc_dev_stream_name_get(p_udev, i + 1);
        p_stream = uvc_dev_stream_open(p_name);
        if(p_stream != NULL){
            /* ��������ͷ����*/
            ret = aw_camera_serve_create(p_name, (void *)p_stream, &__g_uvc_servfuncs);
            if(ret != AW_OK){
                __AUVC_TRACE("UVC camera serve create failed\r\n");
                return;
            }
            else{
                __AUVC_TRACE("UVC camera \"%s\" serve create success\r\n", p_name);
            }
        }
    }
    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/* UVC�豸�Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    /* ͨ�����������ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct uvc_device    *p_udev   = NULL;
    int                   i,num;
    char                 *p_name;
    struct uvc_streaming *p_stream = NULL;

    p_udev = (struct uvc_device *)usbh_func_drvdata_get(p_fun->usb_func);
    if(p_udev == NULL){
        return AW_OK;
    }
    /* ��ȡ����ͷ����������*/
    num = uvc_dev_nstream_get(p_udev);

    for(i = 0;i < num;i++){
        p_name = uvc_dev_stream_name_get(p_udev, i + 1);
        p_stream = uvc_dev_stream_open(p_name);
        if(p_name != NULL){
            /* ɾ������ͷ����*/
            aw_camera_serve_delete(p_name);
            uvc_dev_stream_close(p_stream);
            uvc_dev_stream_close(p_stream);
        }
    }

    /* �ͷ�UVC�豸�ṹ��*/
    uvc_dev_destory(p_udev);
    __AUVC_TRACE("UVC device removed.\r\n");
    return AW_OK;
}

/* UVC����*/
aw_local aw_const struct awbl_dev_method __g_uvc_methods[] = {
        /* �Ƴ��豸ʱ�����ô˷��� , ����AW_OK��ʾ�ɹ�����������ֵ��ʾ�Ƴ�ʧ�� */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* �Ƴ�����*/
        AWBL_METHOD_END
};

/* ��ʼ��������*/
aw_local aw_const struct awbl_drvfuncs __g_uvc_drvfuncs = {
        __drv_init1,      /* ��һ�׶γ�ʼ��*/
        __drv_init2,      /* �ڶ��׶γ�ʼ��*/
        __drv_connect     /* �����׶γ�ʼ��*/
};

/* USB����������Ϣ*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_uvc_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus�汾��*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* ����ID*/
        AWBL_USBH_UVC_DRV_NAME,                 /* ��������*/
        &__g_uvc_drvfuncs,                      /* ��ʼ��������*/
        __g_uvc_methods,                        /* ���񷽷�*/
        __drv_probe,                            /* ̽�⺯��*/
    },
    __uvc_drv_info_tab                          /* ������Ϣ��*/
};

/** UVC����ע��*/
void awbl_usbh_uvc_drv_register (void)
{
    /* ����ע��*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_uvc_drv_info);
}

