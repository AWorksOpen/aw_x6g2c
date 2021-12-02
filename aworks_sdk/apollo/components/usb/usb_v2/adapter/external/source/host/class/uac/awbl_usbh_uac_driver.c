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

/* UAC驱动名字*/
#define AWBL_USBH_UAC_DRV_NAME   "awbl_usbh_uac"

/* 打印函数*/
#define __AUAC_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSB-UAC:"info, ##__VA_ARGS__)); \
            } while (0)

/* UAC设备驱动匹配信息表*/
aw_local const struct awbl_usbh_fundrv_info_tab __uac_drv_info_tab[] = {

    {
        AWBL_USBH_FUNDRV_MATCH_FUN_CLASS,
        0,0,0,0,0,
        USB_CLASS_AUDIO,
        0,0,0
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/** 第一阶段初始化*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev){
    struct usbh_audio *p_uaudio = NULL;

    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* 创建一个USB音频设备*/
    p_uaudio = usbh_audio_dev_create(p_fun->usb_func);
    if(p_uaudio != NULL){
        __AUAC_TRACE("create usb audio device success\r\n");
    }
    else{
        __AUAC_TRACE("create usb audio device failed\r\n");
    }
    /* 设置驱动私有数据*/
    usbh_func_drvdata_set(p_fun->usb_func, p_uaudio);
    return;
}

/** 第三阶段初始化*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/* UAC设备移除函数*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    aw_err_t             ret;
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct usbh_audio   *p_uaudio = NULL;
    p_uaudio = (struct usbh_audio *)usbh_func_drvdata_get(p_fun->usb_func);

    ret = usbh_audio_dev_destory(p_uaudio);
    return ret;
}

/* UAC服务*/
aw_local aw_const struct awbl_dev_method __g_uac_methods[] = {
        /* 移除设备时将调用此方法 , 返回AW_OK表示成功，返回其它值表示移除失败 */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* 移除服务*/
        AWBL_METHOD_END
};

/* 初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_uac_drvfuncs = {
        __drv_init1,      /* 第一阶段初始化*/
        __drv_init2,      /* 第二阶段初始化*/
        __drv_connect     /* 第三阶段初始化*/
};

/* USB功能驱动信息*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_uac_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus版本号*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* 总线ID*/
        AWBL_USBH_UAC_DRV_NAME,                 /* 驱动名字*/
        &__g_uac_drvfuncs,                      /* 初始化函数集*/
        __g_uac_methods,                        /* 服务方法*/
        NULL
    },
    __uac_drv_info_tab                          /* 功能信息表*/
};

/** UAC驱动注册*/
void awbl_usbh_uac_drv_register (void)
{
    /* 驱动注册*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_uac_drv_info);
}
