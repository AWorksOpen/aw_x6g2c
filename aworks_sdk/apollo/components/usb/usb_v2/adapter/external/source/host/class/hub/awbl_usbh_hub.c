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
#include "aw_list.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "string.h"
//#include "aw_usb_common.h"
//#include "aw_usb_mem.h"
#include "host/awbl_usbh.h"
#include "string.h"
#include "stdio.h"

#include "usb_common_def.h"
#include "host/core/usbh.h"
#include "usb_adapter_inter.h"

/* ��ӡ����*/
#define __AUHUB_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSB-HUB:"info, ##__VA_ARGS__)); \
            } while (0)
#define __AUHUB_INFO(info) \
                do { \
                    AW_INFOF(info);\
                } while (0)

/** �Ƴ�����*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev)
{
    __AWBL_USBH_FUN_DECL( p_fun, p_awdev);
    struct usbh_hub *p_hub;

    if ((p_awdev == NULL) || (p_fun == NULL)) {
        return -AW_EINVAL;
    }

    if (p_fun->usb_func->driver_priv == NULL) {
        return AW_OK;
    }

    p_hub = (struct usbh_hub *)p_fun->usb_func->driver_priv;
    /* �رն˿�*/
    usbh_hub_port_disable(p_hub);
    /* ���ټ�����*/
    usbh_hub_destory(p_hub);
    __AUHUB_TRACE("removed a hub\r\n");
    return AW_OK;
}

/** ��һ�׶γ�ʼ��*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** �ڶ��׶γ�ʼ��*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    __AWBL_USBH_FUN_DECL(       p_fun, p_awdev);
    aw_err_t ret;
    struct usbh_hub *usb_hub = NULL;
    int hub_port_num = 0, i;

    /* ����һ��������*/
    usb_hub = usbh_hub_create(p_fun->usb_func);
    if(usb_hub == NULL){
        __AUHUB_TRACE("create hub failed\r\n");
        return;
    }
    /* ��ȡ�˿�����*/
    hub_port_num = USBH_HUB_PORT_NUM_GET(usb_hub);
    /* ʹ�����ж˿�*/
    for(i = 1; i < hub_port_num; i++){
        ret = usbh_hub_port_enable(usb_hub, i);
        if(ret != AW_OK){
            __AUHUB_TRACE("enable hub port failed\r\n");
            goto __failed;
        }
    }
    /* ����������*/
    ret = usbh_hub_start(usb_hub);
    if(ret != AW_OK){
        goto __failed;
    }
    __AUHUB_TRACE("found a new hub\r\n");
    return;

__failed:
    usbh_hub_destory(usb_hub);
    return;
}

/** �����׶γ�ʼ��*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/** ̽�⺯��*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    __AWBL_USBH_FUN_DECL(       p_fun, p_awdev);
    struct usbh_interface *p_itf;
    /* ��ȡ��һ���ӿ�*/
    p_itf = usbh_func_first_intf_get(p_fun->usb_func);
    if(p_itf == NULL){
        return AW_FALSE;
    }

    /* �������Ľӿ����������0��1������������ֻ��һ���˵�*/
    if ((USBH_INTF_SUB_CLASS_GET(p_itf) > 1) ||
            (USBH_INTF_EP_NUM_GET(p_itf) != 1)) {
        goto __failed;
    }

    /* �˵�������ж϶˵�*/
    if ((USBH_EP_TYPE_GET(p_itf->eps) != USB_EP_TYPE_INT) ||
        (USBH_EP_DIR_GET(p_itf->eps) != USB_DIR_IN)) {
        goto __failed;
    }

    return AW_TRUE;

__failed:
    __AUHUB_TRACE("bad descriptor, ignore it\r\n");
    return AW_FALSE;
}

/* method*/
aw_local aw_const struct awbl_dev_method __g_drv_methods[] = {
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),
        AWBL_METHOD_END
};

/* ����ƥ����Ϣ��*/
aw_local aw_const struct awbl_usbh_fundrv_info_tab __fun_drv_info_tab[] = {
    {
        .flags  = AWBL_USBH_FUNDRV_MATCH_DEV_CLASS |
                  AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER,
        .dclss = USB_CLASS_HUB,
        .inum = 1
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/* ���׶γ�ʼ��*/
aw_local aw_const struct awbl_drvfuncs __g_drv_funcs = {
        __drv_init1,
        __drv_init2,
        __drv_connect
};

/* USB����������Ϣ�ṹ��*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_usbh_hub_drv = {
    {
        AWBL_VER_1,                             /* AWBus �汾 */
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* USB���� | ��ͨ�豸*/
        "usbh hub driver",                      /* �������� */
        &__g_drv_funcs,                         /* ������ʼ�������� */
        __g_drv_methods,                        /* ���񷽷�*/
        __drv_probe                             /* ����̽�⺯��*/
    },
    __fun_drv_info_tab                          /* USB����������Ϣ��*/
};

/** ����������ע�ắ��*/
void awbl_usbh_hub_drv_register (void)
{
    aw_err_t ret;
    /* ע�Ἧ��������*/
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_usbh_hub_drv);
    if (ret != AW_OK) {
        __AUHUB_TRACE("driver register failed %d.\n", ret);
    }
    aw_assert(ret == AW_OK);
}


