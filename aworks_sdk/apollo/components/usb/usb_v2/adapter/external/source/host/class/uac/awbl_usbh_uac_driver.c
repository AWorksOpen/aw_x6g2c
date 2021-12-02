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
#include "host/class/uac/uac_driver.h"
#include "host/class/uac/uac.h"

/* UAC��������*/
#define AWBL_USBH_UAC_DRV_NAME   "awbl_usbh_uac"

/* ��ӡ����*/
#define __AUAC_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSB-UAC:"info, ##__VA_ARGS__)); \
            } while (0)

/* UAC�豸����ƥ����Ϣ��*/
aw_local const struct awbl_usbh_fundrv_info_tab __uac_drv_info_tab[] = {

    {
        AWBL_USBH_FUNDRV_MATCH_FUN_CLASS,
        0,0,0,0,0,
        USB_CLASS_AUDIO,
        0,0,0
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev){
    struct usbh_audio *p_uaudio = NULL;

    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* ����һ��USB��Ƶ�豸*/
    p_uaudio = usbh_audio_dev_create(p_fun->usb_func);
    if(p_uaudio != NULL){
        __AUAC_TRACE("create usb audio device success\r\n");
    }
    else{
        __AUAC_TRACE("create usb audio device failed\r\n");
    }
    /* ��������˽������*/
    usbh_func_drvdata_set(p_fun->usb_func, p_uaudio);
    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/* UAC�豸�Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    aw_err_t             ret;
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_audio   *p_uaudio = NULL;
    p_uaudio = (struct usbh_audio *)usbh_func_drvdata_get(p_fun->usb_func);

    ret = usbh_audio_dev_destory(p_uaudio);
    return ret;
}

/* UAC����*/
aw_local aw_const struct awbl_dev_method __g_uac_methods[] = {
        /* �Ƴ��豸ʱ�����ô˷��� , ����AW_OK��ʾ�ɹ�����������ֵ��ʾ�Ƴ�ʧ�� */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* �Ƴ�����*/
        AWBL_METHOD_END
};

/* ��ʼ��������*/
aw_local aw_const struct awbl_drvfuncs __g_uac_drvfuncs = {
        __drv_init1,      /* ��һ�׶γ�ʼ��*/
        __drv_init2,      /* �ڶ��׶γ�ʼ��*/
        __drv_connect     /* �����׶γ�ʼ��*/
};

/* USB����������Ϣ*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_uac_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus�汾��*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* ����ID*/
        AWBL_USBH_UAC_DRV_NAME,                 /* ��������*/
        &__g_uac_drvfuncs,                      /* ��ʼ��������*/
        __g_uac_methods,                        /* ���񷽷�*/
        NULL
    },
    __uac_drv_info_tab                          /* ������Ϣ��*/
};

/** UAC����ע��*/
void awbl_usbh_uac_drv_register (void)
{
    /* ����ע��*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_uac_drv_info);
}
