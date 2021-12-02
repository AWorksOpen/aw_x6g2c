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
#include "apollo.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_common.h"
#include "string.h"
#include "aw_serial.h"
#include "aw_rngbuf.h"
#include "host/awbl_usbh.h"
#include "host/class/cdc/serial/usbh_cdc_serial.h"
#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"

/* USBת������������*/
#define AWBL_USBH_SERIAL_DRV_NAME   "awbl_usbh_cdc_serial"

/* USBת���ڴ�ӡ����*/
#define __AUSB_SERIAL_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSBH-SERIAL:"info, ##__VA_ARGS__)); \
            } while (0)


/* CDC USBת��������Ϣƥ���*/
extern const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_serial_info_tab[];

/** �򿪶�Ӧ��USBת����*/
void *awbl_usbh_serial_open(int pid, int vid, int intfnum){
    return (void*)usbh_serial_dev_open(pid, vid, intfnum, 1);
}

/** �ر�USBת����*/
void awbl_usbh_serial_close (void *handle){
    usbh_serial_dev_close(handle);
}

/** ��ȡUSBת�������ݿ��ƿ�*/
aw_err_t aw_usb_serial_dcb_get (void *handle, struct usb_serial_dcb *p_dcb){
    struct usb_serial_dcb *p_dcb_tmp = NULL;

    p_dcb_tmp = usbh_serial_dev_dcb_get(handle);
    if(p_dcb_tmp == NULL){
        return -AW_EPERM;
    }
    p_dcb = p_dcb_tmp;

    return AW_OK;
}

/** ����USBת�������ݿ��ƿ�*/
aw_err_t aw_usb_serial_dcb_set (void *handle, struct usb_serial_dcb *p_dcb){
    return usbh_serial_dev_dcb_set(handle, p_dcb);
}

/** USBת����ͬ��д����*/
int aw_usb_serial_write_sync (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_send(handle, p_buffer, nbytes);
}

/** USBת����ͬ��������*/
int aw_usb_serial_read_sync (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_recieve_sync(handle, p_buffer, nbytes);
}

/** USBת�����첽������*/
int aw_usb_serial_read_async (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_recieve_async(handle, p_buffer, nbytes);
}

/** ��ȡUSBת�������ݴ��䳬ʱʱ��*/
aw_err_t aw_usb_serial_timeout_get (void *handle, uint8_t dir, uint32_t *timeout){
    return usbh_serial_dev_timeout_get(handle, dir, timeout);
}

/** ����USBת�������ݴ��䳬ʱʱ��*/
//aw_err_t aw_usb_serial_timeout_set (void *handle, uint8_t dir, uint32_t timeout){
//    return usbh_serial_timeout_set(handle, dir, timeout);
//}

aw_err_t aw_usb_serial_timeout_set (void *handle, uint32_t *timeout){
    usb_err_t ret;
    ret = usbh_serial_dev_timeout_set(handle, USB_SERIAL_PIPE_BOTH, *timeout);
    if(ret != USB_OK){
        return ret;
    }
    return USB_OK;
}

/** ��ȡUSBת�������ݵ��δ������ݻ���*/
aw_err_t aw_usb_serial_tranfersize_get (void *handle, uint8_t dir, uint32_t *tranfer_size){
    return usbh_serial_dev_tranfersize_get(handle, dir, tranfer_size);
}

/** ����USBת�������ݵ��δ������ݻ���*/
aw_err_t aw_usb_serial_tranfersize_set (void *handle, uint8_t dir, uint32_t tranfer_size){
    return usbh_serial_dev_tranfersize_set(handle, dir, tranfer_size);
}


/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* ͨ�������豸���ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_serial *p_usb_serial = NULL;

    /* ����һ��USBת�����豸*/
    p_usb_serial = usbh_serial_dev_create(p_fun->usb_func);
    if(p_usb_serial == NULL){
        __AUSB_SERIAL_TRACE("usb serial create failed\r\n");
        return;
    }
    usbh_func_drvdata_set(p_fun->usb_func, p_usb_serial);

    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return;
}

/* ��ʼ��������*/
aw_local aw_const struct awbl_drvfuncs __g_serial_drvfuncs = {
        __drv_init1,      /* ��һ�׶γ�ʼ��*/
        __drv_init2,      /* �ڶ��׶γ�ʼ��*/
        __drv_connect     /* �����׶γ�ʼ��*/
};

/** USBת�����豸�Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    aw_err_t ret;
    /* ͨ�����������ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_serial *p_usb_serial = usbh_func_drvdata_get(p_fun->usb_func);
    ret = usbh_serial_dev_destroy(p_usb_serial);
    return ret;
}

/* USBת���ڷ���*/
aw_local aw_const struct awbl_dev_method __g_serial_methods[] = {
        /* �Ƴ��豸ʱ�����ô˷��� , ����AW_OK��ʾ�ɹ�����������ֵ��ʾ�Ƴ�ʧ�� */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* �Ƴ�����*/
        AWBL_METHOD_END
};

/* USB����������Ϣ*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_serial_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus�汾��*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* ����ID*/
        AWBL_USBH_SERIAL_DRV_NAME,              /* ��������*/
        &__g_serial_drvfuncs,                   /* ��ʼ��������*/
        __g_serial_methods,                     /* ���񷽷�*/
        NULL,
    },
    awbl_usbh_cdc_serial_info_tab               /* ������Ϣ��*/
};

/** USBת������ע��*/
void awbl_usbh_cdc_serial_drv_register (void)
{
    /* ����ע��*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_serial_drv_info);
}






