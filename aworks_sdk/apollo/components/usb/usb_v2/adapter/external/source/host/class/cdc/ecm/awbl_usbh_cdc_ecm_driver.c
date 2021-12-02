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

/* ECM驱动名字*/
#define AWBL_USBH_ECM_DRV_NAME   "awbl_usbh_cdc_ecm"
/* 全局驱动信息*/
uint8_t __g_ecm_driver_type = 0;
/* CDC ECM驱动信息匹配表*/
extern const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_ecm_info_tab[];

/* 第一阶段初始化*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return;
}

/* 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* 通过基础设备类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* USB网络初始化*/
    usbnet_probe(p_fun->usb_func, USB_ETHER);
}

/* 第三阶段初始化*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return;
}

/* USB ECM设备移除函数*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    /* 通过基础子类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_usbnet *dev = (struct usbh_usbnet *)p_fun->usb_func->driver_priv;

    return usbnet_remove(dev);
}

/* 初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_ecm_drvfuncs = {
        __drv_init1,      /* 第一阶段初始化*/
        __drv_init2,      /* 第二阶段初始化*/
        __drv_connect     /* 第三阶段初始化*/
};

/* ECM服务*/
aw_local aw_const struct awbl_dev_method __g_ecm_methods[] = {
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* 移除服务*/
        /* 移除设备时将调用此方法 , 返回AW_OK表示成功，返回其它值表示移除失败 */
        AWBL_METHOD_END
};

/** 驱动探测函数*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    /* 通过基础设备类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_interface *p_intf;

    p_intf = usbh_func_first_intf_get(p_fun->usb_func);
    if(p_intf == NULL){
        return AW_FALSE;
    }

    /* 华为ME909s-821 配置2 NCM控制接口和数据接口*/
    if((USBH_DEV_VID_GET(p_fun->usb_func) == 0x12D1) &&
            (USBH_INTF_SUB_CLASS_GET(p_intf) == 0x06) &&
            (USBH_INTF_CLASS_GET(p_intf) == USB_CLASS_COMM)){
        __g_ecm_driver_type = USB_ETHER;
        return AW_TRUE;
    }
    return AW_FALSE;
}


/* USB功能驱动信息*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_ecm_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus版本号*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* 总线ID*/
        AWBL_USBH_ECM_DRV_NAME,                 /* 驱动名字*/
        &__g_ecm_drvfuncs,                      /* 初始化函数集*/
        __g_ecm_methods,                        /* 服务方法*/
        __drv_probe,                            /* 探测函数*/
    },
    awbl_usbh_cdc_ecm_info_tab                  /* 功能信息表*/
};

/** ECM驱动注册*/
void awbl_usbh_cdc_ecm_drv_register (void)
{
    /* 驱动注册*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_ecm_drv_info);
}



