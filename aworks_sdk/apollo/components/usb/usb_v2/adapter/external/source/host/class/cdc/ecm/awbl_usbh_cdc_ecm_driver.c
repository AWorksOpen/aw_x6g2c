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
#include "host/awbl_usbh.h"
#include "host/core/usbh.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_delay.h"
#include "aw_common.h"
#include "string.h"
//#include "host/class/cdc/awbl_usbh_usbnet.h"
//#include "host/class/cdc/ecm/awbl_usbh_cdc_ecm.h"
//#include "host/class/cdc/awbl_usbh_cdc.h"
#include "host/class/cdc/net/usbh_net.h"
#include <stdio.h>

/* ECM��������*/
#define AWBL_USBH_ECM_DRV_NAME   "awbl_usbh_cdc_ecm"
/* ȫ��������Ϣ*/
uint8_t __g_ecm_driver_type = 0;
/* CDC ECM������Ϣƥ���*/
extern const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_ecm_info_tab[];

/* ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return;
}

/* �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* ͨ�������豸���ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* USB�����ʼ��*/
    usbnet_probe(p_fun->usb_func, USB_ETHER);
}

/* �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return;
}

/* USB ECM�豸�Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    /* ͨ�����������ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_usbnet *dev = (struct usbh_usbnet *)p_fun->usb_func->driver_priv;

    return usbnet_remove(dev);
}

/* ��ʼ��������*/
aw_local aw_const struct awbl_drvfuncs __g_ecm_drvfuncs = {
        __drv_init1,      /* ��һ�׶γ�ʼ��*/
        __drv_init2,      /* �ڶ��׶γ�ʼ��*/
        __drv_connect     /* �����׶γ�ʼ��*/
};

/* ECM����*/
aw_local aw_const struct awbl_dev_method __g_ecm_methods[] = {
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* �Ƴ�����*/
        /* �Ƴ��豸ʱ�����ô˷��� , ����AW_OK��ʾ�ɹ�����������ֵ��ʾ�Ƴ�ʧ�� */
        AWBL_METHOD_END
};

/** ����̽�⺯��*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    /* ͨ�������豸���ȡUSB���ܽṹ��*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_interface *p_intf;

    p_intf = usbh_func_first_intf_get(p_fun->usb_func);
    if(p_intf == NULL){
        return AW_FALSE;
    }

    /* ��ΪME909s-821 ����2 NCM���ƽӿں����ݽӿ�*/
    if((USBH_DEV_VID_GET(p_fun->usb_func) == 0x12D1) &&
            (USBH_INTF_SUB_CLASS_GET(p_intf) == 0x06) &&
            (USBH_INTF_CLASS_GET(p_intf) == USB_CLASS_COMM)){
        __g_ecm_driver_type = USB_ETHER;
        return AW_TRUE;
    }
    return AW_FALSE;
}


/* USB����������Ϣ*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_ecm_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus�汾��*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* ����ID*/
        AWBL_USBH_ECM_DRV_NAME,                 /* ��������*/
        &__g_ecm_drvfuncs,                      /* ��ʼ��������*/
        __g_ecm_methods,                        /* ���񷽷�*/
        __drv_probe,                            /* ̽�⺯��*/
    },
    awbl_usbh_cdc_ecm_info_tab                  /* ������Ϣ��*/
};

/** ECM����ע��*/
void awbl_usbh_cdc_ecm_drv_register (void)
{
    /* ����ע��*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_ecm_drv_info);
}



