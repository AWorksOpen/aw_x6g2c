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
#include "aworks.h"
#include "aw_mem.h"
#include "host/awbl_usbh.h"
#include "host/core/usbh.h"
#include "string.h"

/** 外部关联设备创建*/
usb_err_t sys_dev_create(struct usbh_function *p_usb_func){
    struct usbh_device *p_udev  = p_usb_func->p_udev;
    struct awbl_usbh   *p_aw_hc = (struct awbl_usbh *)usbh_usrdata_get(p_udev->p_hc);
    usb_err_t           ret;

    if(p_aw_hc == NULL){
        return -USB_EINVAL;
    }

    struct awbl_usbh_function *p_fun = aw_mem_alloc(sizeof(struct awbl_usbh_function));
    if((p_usb_func == NULL) || (p_fun == NULL) || (p_udev == NULL)){
        return -USB_ENOMEM;
    }
    memset(p_fun, 0, sizeof(struct awbl_usbh_function));

    p_fun->usb_func = p_usb_func;

    /* 设备平台信息*/
    p_fun->awdev.p_parentbus = p_aw_hc->super.super.p_subbus;
    p_fun->awdev.p_devhcf    = &p_fun->devhcf;

    p_fun->devhcf.p_name    = p_usb_func->name;
    p_fun->devhcf.unit      = p_udev->addr;
    p_fun->devhcf.bus_type  = AWBL_BUSID_USBH;
    p_fun->devhcf.bus_index = p_usb_func->p_udev->p_hc->host_index;
    p_fun->devhcf.p_dev     = &p_fun->awdev;
    p_fun->devhcf.p_devinfo = NULL;
    usbh_func_usrdata_set(p_usb_func, p_fun);

    ret = awbl_dev_announce(&p_fun->awdev);
    if(ret == USB_OK){
        if (p_fun->awdev.p_driver == NULL) {
            return -USB_ENOTSUP;
        }
        return USB_OK;
    }

    return -USB_EPERM;
}

/** 外部关联设备销毁*/
usb_err_t sys_dev_destory(struct usbh_function *p_usb_func){
    usb_err_t ret = USB_OK;

    if(p_usb_func == NULL){
        return -USB_EINVAL;
    }

    struct awbl_usbh_function *p_fun = (struct awbl_usbh_function *)usbh_func_usrdata_get(p_usb_func);

    ret = awbl_dev_remove_announce(&p_fun->awdev);
    if(ret == USB_OK){
        aw_mem_free(p_fun);
    }
    return ret;
}



