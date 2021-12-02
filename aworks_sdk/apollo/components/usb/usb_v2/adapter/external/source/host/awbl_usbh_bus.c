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
#include "awbus_lite.h"
#include "aw_vdebug.h"
#include "host/awbl_usbh.h"
#include "host/core/usbh.h"
#include "host/class/ms/usbh_mass_storage.h"
#include "host/class/uac/uac_driver.h"
#include "host/class/uvc/uvc_driver.h"
#include "host/class/cdc/serial/usbh_cdc_serial.h"

/**
 * ��ʼ��USB������
 *
 * @param hub_stack_size ����������ջ��С
 * @param hub_task_prio  �������������ȼ�
 *
 * @return �ɹ�����AW_OK
 */
aw_err_t awbl_usbh_lib_init (int stk_size, int stk_prio)
{
    aw_err_t          ret;
    struct usbh_lib_info  p_usbh_info;
    /* ��ӡ��USB�����Ϣ*/
    usbh_lib_info_get(&p_usbh_info);
    aw_kprintf("USB lib version  : %s\r\n"
            "    build date   : %s\r\n"
            "    modified date: %s\r\n",p_usbh_info.version ,
            p_usbh_info.build_date, p_usbh_info.modified_date);
    /* ��ʼ��USB�����⣬֧����������������*/
    ret = usbh_core_lib_init(2);
    if (ret != USB_OK) {
        return ret;
    }
    /* ��ʼ��USB��������*/
    ret = usbh_hub_lib_init(stk_size, stk_prio);
    if (ret != USB_OK) {
        return ret;
    }
    /* ��ʼ��USB�������洢�豸��*/
    ret = usbh_ms_lib_init();
    if (ret != USB_OK) {
        return ret;
    }
    /* ��ʼ��USB��Ƶ�豸�⣬֧��2����Ƶ��������ÿ����Ƶ����������3��ͼ�񻺴�*/
    ret = uvc_lib_init(2, 3);
    if(ret != USB_OK){
        return ret;
    }
    /* ��ʼ��USBת�����豸��*/
    ret = usbh_serial_lib_init();
    if(ret != USB_OK){
        return ret;
    }
    /* ��ʼ��USB��Ƶ�豸��*/
    ret = usbh_audio_lib_init();
    if(ret != USB_OK){
        return ret;
    }
    /* ��ʼ��AWorks USB����*/
    return awbl_usbh_host_list_init();
}

/** ����豸�������Ƿ�ƥ�� */
aw_local aw_bool_t __usbh_dev_match (const struct awbl_drvinfo *p_awdrv,
                                     struct awbl_dev           *p_awdev)
{
    __AWBL_USBH_FUN_DECL( p_fun, p_awdev);
    __AWBL_USBH_FUN_DRV(  p_drv, p_awdrv);
    const struct awbl_usbh_fundrv_info_tab *p_info;

    /* ������*/
    if(p_fun->usb_func == NULL){
        return AW_FALSE;
    }
    /* �������״̬*/
    if (p_awdev->p_devhcf->bus_type != AWBL_BUSID_USBH) {
        return AW_FALSE;
    }
    /* ����ƥ���־ƥ����Ϣ*/
    for (p_info = p_drv->p_tab; p_info->flags != 0; p_info++) {
        /* VID*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_VENDOR) &&
            (p_info->vid != USBH_DEV_VID_GET(p_fun->usb_func))) {
            continue;
        }
        /* PID*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_PRODUCT) &&
            (p_info->pid != USBH_DEV_PID_GET(p_fun->usb_func))) {
            continue;
        }
        /* �豸��*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_CLASS) &&
            (p_info->dclss != USBH_DEV_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* �豸����*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS) &&
            (p_info->dsclss != USBH_DEV_SUB_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* �豸Э��*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL) &&
            (p_info->dproto != USBH_DEV_PROTOCOL_GET(p_fun->usb_func))) {
            continue;
        }
        /* �Զ�����*/
        if ((USBH_DEV_CLASS_GET(p_fun->usb_func) == USB_CLASS_VENDOR_SPEC) &&
            !(p_info->flags & AWBL_USBH_FUNDRV_MATCH_VENDOR) &&
            (p_info->flags & (AWBL_USBH_FUNDRV_MATCH_FUN_CLASS |
                              AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS |
                              AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL |
                              AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER))) {
            continue;
        }
        /* �ӿ���*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_CLASS) &&
            (p_info->iclss != USBH_FUNC_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* �ӿ�����*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS) &&
            (p_info->isclss != USBH_FUNC_SUBCLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* �ӿ�Э��*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL) &&
            (p_info->iproto != USBH_FUNC_PROTO_GET(p_fun->usb_func))) {
            continue;
        }
        /* �ӿ�����*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER) &&
            ((p_info->inum < USBH_FUNC_FIRST_INTFNUM_GET(p_fun->usb_func)) ||
             (p_info->inum > USBH_FUNC_FIRST_INTFNUM_GET(p_fun->usb_func) +
                     USBH_FUNC_NINTF_GET(p_fun->usb_func)))) {
            continue;
        }

        return AW_TRUE;
    }

    return AW_FALSE;
}

/* ������Ϣ*/
aw_local aw_const struct awbl_bustype_info __g_usbh_bustype = {
    AWBL_BUSID_USBH,        /* ����ID*/
    NULL,                   /* ���߳�ʼ����һ�׶�*/
    NULL,                   /* ���߳�ʼ���ڶ��׶�*/
    NULL,                   /* ���߳�ʼ�������׶�*/
    NULL,                   /* ��������ϵ����豸 */
    __usbh_dev_match        /* ����ƥ�亯��*/
};

/** ��ʼ��AWorks USB����*/
void awbl_usbh_bus_init (void)
{
    /* ����ע��*/
    awbl_bustype_register(&__g_usbh_bustype);
}

