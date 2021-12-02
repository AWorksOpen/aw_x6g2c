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

/* USB转串口驱动名字*/
#define AWBL_USBH_SERIAL_DRV_NAME   "awbl_usbh_cdc_serial"

/* USB转串口打印函数*/
#define __AUSB_SERIAL_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSBH-SERIAL:"info, ##__VA_ARGS__)); \
            } while (0)


/* CDC USB转串驱动信息匹配表*/
extern const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_serial_info_tab[];

/** 打开对应的USB转串口*/
void *awbl_usbh_serial_open(int pid, int vid, int intfnum){
    return (void*)usbh_serial_dev_open(pid, vid, intfnum, 1);
}

/** 关闭USB转串口*/
void awbl_usbh_serial_close (void *handle){
    usbh_serial_dev_close(handle);
}

/** 获取USB转串口数据控制块*/
aw_err_t aw_usb_serial_dcb_get (void *handle, struct usb_serial_dcb *p_dcb){
    struct usb_serial_dcb *p_dcb_tmp = NULL;

    p_dcb_tmp = usbh_serial_dev_dcb_get(handle);
    if(p_dcb_tmp == NULL){
        return -AW_EPERM;
    }
    p_dcb = p_dcb_tmp;

    return AW_OK;
}

/** 设置USB转串口数据控制块*/
aw_err_t aw_usb_serial_dcb_set (void *handle, struct usb_serial_dcb *p_dcb){
    return usbh_serial_dev_dcb_set(handle, p_dcb);
}

/** USB转串口同步写函数*/
int aw_usb_serial_write_sync (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_send(handle, p_buffer, nbytes);
}

/** USB转串口同步读函数*/
int aw_usb_serial_read_sync (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_recieve_sync(handle, p_buffer, nbytes);
}

/** USB转串口异步读函数*/
int aw_usb_serial_read_async (void *handle, void *p_buffer, size_t nbytes){
    return usbh_serial_dev_recieve_async(handle, p_buffer, nbytes);
}

/** 获取USB转串口数据传输超时时间*/
aw_err_t aw_usb_serial_timeout_get (void *handle, uint8_t dir, uint32_t *timeout){
    return usbh_serial_dev_timeout_get(handle, dir, timeout);
}

/** 设置USB转串口数据传输超时时间*/
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

/** 获取USB转串口数据单次传输数据缓存*/
aw_err_t aw_usb_serial_tranfersize_get (void *handle, uint8_t dir, uint32_t *tranfer_size){
    return usbh_serial_dev_tranfersize_get(handle, dir, tranfer_size);
}

/** 设置USB转串口数据单次传输数据缓存*/
aw_err_t aw_usb_serial_tranfersize_set (void *handle, uint8_t dir, uint32_t tranfer_size){
    return usbh_serial_dev_tranfersize_set(handle, dir, tranfer_size);
}


/** 第一阶段初始化*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return;
}

/** 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* 通过基础设备类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_serial *p_usb_serial = NULL;

    /* 创建一个USB转串口设备*/
    p_usb_serial = usbh_serial_dev_create(p_fun->usb_func);
    if(p_usb_serial == NULL){
        __AUSB_SERIAL_TRACE("usb serial create failed\r\n");
        return;
    }
    usbh_func_drvdata_set(p_fun->usb_func, p_usb_serial);

    return;
}

/** 第三阶段初始化*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return;
}

/* 初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_serial_drvfuncs = {
        __drv_init1,      /* 第一阶段初始化*/
        __drv_init2,      /* 第二阶段初始化*/
        __drv_connect     /* 第三阶段初始化*/
};

/** USB转串口设备移除函数*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    aw_err_t ret;
    /* 通过基础子类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_serial *p_usb_serial = usbh_func_drvdata_get(p_fun->usb_func);
    ret = usbh_serial_dev_destroy(p_usb_serial);
    return ret;
}

/* USB转串口服务*/
aw_local aw_const struct awbl_dev_method __g_serial_methods[] = {
        /* 移除设备时将调用此方法 , 返回AW_OK表示成功，返回其它值表示移除失败 */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* 移除服务*/
        AWBL_METHOD_END
};

/* USB功能驱动信息*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_serial_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus版本号*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* 总线ID*/
        AWBL_USBH_SERIAL_DRV_NAME,              /* 驱动名字*/
        &__g_serial_drvfuncs,                   /* 初始化函数集*/
        __g_serial_methods,                     /* 服务方法*/
        NULL,
    },
    awbl_usbh_cdc_serial_info_tab               /* 功能信息表*/
};

/** USB转串驱动注册*/
void awbl_usbh_cdc_serial_drv_register (void)
{
    /* 驱动注册*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_serial_drv_info);
}






