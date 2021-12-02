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

/* UVC驱动名字*/
#define AWBL_USBH_UVC_DRV_NAME   "awbl_usbh_uvc"

/**
 * UVC打开函数
 *
 * @param p_cookie 摄像头服务自定义数据
 *
 * @return 成功返回AW_OK
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

/** UVC关闭函数*/
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
 * UVC获取视频缓存
 *
 * @param p_cookie 摄像头服务自定义数据
 * @param p_buf    缓存地址的地址
 *
 * @return 成功返回AW_OK
 */
static aw_err_t __uvc_op_get_video_buf(void *p_cookie, uint32_t **p_buf)
{
    aw_err_t ret;
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    /* 检查参数合法性*/
    if(p_stream == NULL)
        return -AW_EINVAL;
    /* 获取图像*/
    ret = uvc_dev_stream_video_get(p_stream, (void **)p_buf, NULL);
    return ret;
}

static aw_err_t __uvc_release_photo_buf(void *p_cookie, uint32_t addr)
{
    return AW_OK;
}

/**
 * UVC释放视频缓存
 *
 * @param p_cookie 摄像头服务自定义数据
 * @param addr     缓存地址
 *
 * @return 成功返回AW_OK
 */
static aw_err_t __uvc_op_release_video_buf(void *p_cookie, uint32_t addr)
{
    int ret;
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    /* 检查参数合法性*/
    if(p_stream == NULL)
        return -AW_EINVAL;
    ret = uvc_dev_stream_video_put(p_stream, (uint8_t *)addr);
    return ret;
}

/**
 * UVC配置
 *
 * @param p_cookie 摄像头服务自定义数据
 * @param p_cfg    用户配置数据
 * @param option   选项
 *
 * 返回：成功返回AW_OK
 */
static aw_err_t __uvc_cfg_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option)
{
    struct uvc_streaming *p_stream = (struct uvc_streaming *)p_cookie;
    struct uvc_cfg        p_uvc_cfg;
    aw_err_t              ret;

    if((p_stream == NULL) || (p_cfg == NULL))
        return -AW_EINVAL;

    /* 摄像头支持的配置 */
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

/* UVC摄像头服务函数集*/
aw_local struct awbl_camera_servfuncs __g_uvc_servfuncs = {
        __uvc_open,                    /* UVC打开函数*/
        __uvc_close,                   /* UVC关闭函数*/
        __uvc_get_photo_buf,           /* UVC获取图片缓存*/
        __uvc_op_get_video_buf,        /* UVC获取视频图像缓存函数*/
        __uvc_release_photo_buf,       /* UVC释放图片缓存*/
        __uvc_op_release_video_buf,    /* UVC释放视频图像缓存*/
        __uvc_cfg_set,                 /* UVC设置配置函数*/
        __uvc_cfg_get,                 /* UVC获取配置函数*/
};

/* USB功能驱动信息表*/
aw_local const struct awbl_usbh_fundrv_info_tab __uvc_drv_info_tab[] = {

    {
        .flags = USB_UVC_DEVICE_MATCH_INT_INFO,                                  /* 驱动匹配标志(匹配类，子类和协议)*/
        .iclss = USB_CLASS_VIDEO,                                                /* 视频类*/
        .isclss = VSC_VIDEO_INTERFACE_COLLECTION,                                /* 视频接口集合子类*/
        .iproto = 0
    },
    AWBL_USBH_FUNDRV_INFO_END
};

/** 驱动探测函数*/
aw_local aw_bool_t __drv_probe (awbl_dev_t *p_awdev)
{
    /* 通过基础子类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    /* 检查是不是一个有效的UVC设备*/
    return uvc_dev_check_valid(p_fun->usb_func);
}

/** 第一阶段初始化*/
aw_local void __drv_init1 (struct awbl_dev *p_awdev)
{
    return ;
}

/** 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_awdev)
{
    /* 通过基础设备类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct uvc_device    *p_udev   = NULL;
    struct uvc_streaming *p_stream = NULL;
    char                 *p_name   = NULL;
    int                   num, i;
    aw_err_t              ret;

    __AUVC_TRACE("find a generic UVC device\r\n");

    /* 创建一个UVC设备*/
    p_udev = uvc_dev_create(p_fun->usb_func);
    if(p_udev == NULL){
        __AUVC_TRACE("UVC device create failed\r\n");
        return;
    }
    /* 设置驱动私有数据*/
    usbh_func_drvdata_set(p_fun->usb_func, p_udev);
    /* 获取摄像头数据流数量*/
    num = uvc_dev_nstream_get(p_udev);
    __AUVC_TRACE("UVC device has %d camera stream(s)\r\n", num);


    for(i = 0;i < num; i++){
        p_name = uvc_dev_stream_name_get(p_udev, i + 1);
        p_stream = uvc_dev_stream_open(p_name);
        if(p_stream != NULL){
            /* 创建摄像头服务*/
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

/** 第三阶段初始化*/
aw_local void __drv_connect (struct awbl_dev *p_awdev)
{
    return ;
}

/* UVC设备移除函数*/
aw_local aw_err_t __drv_remove (struct awbl_dev *p_awdev, void *arg){
    /* 通过基础子类获取USB功能结构体*/
    __AWBL_USBH_FUN_DECL(p_fun, p_awdev);
    struct uvc_device    *p_udev   = NULL;
    int                   i,num;
    char                 *p_name;
    struct uvc_streaming *p_stream = NULL;

    p_udev = (struct uvc_device *)usbh_func_drvdata_get(p_fun->usb_func);
    if(p_udev == NULL){
        return AW_OK;
    }
    /* 获取摄像头数据流数量*/
    num = uvc_dev_nstream_get(p_udev);

    for(i = 0;i < num;i++){
        p_name = uvc_dev_stream_name_get(p_udev, i + 1);
        p_stream = uvc_dev_stream_open(p_name);
        if(p_name != NULL){
            /* 删除摄像头服务*/
            aw_camera_serve_delete(p_name);
            uvc_dev_stream_close(p_stream);
            uvc_dev_stream_close(p_stream);
        }
    }

    /* 释放UVC设备结构体*/
    uvc_dev_destory(p_udev);
    __AUVC_TRACE("UVC device removed.\r\n");
    return AW_OK;
}

/* UVC服务*/
aw_local aw_const struct awbl_dev_method __g_uvc_methods[] = {
        /* 移除设备时将调用此方法 , 返回AW_OK表示成功，返回其它值表示移除失败 */
        AWBL_METHOD(awbl_drv_unlink, __drv_remove),       /* 移除服务*/
        AWBL_METHOD_END
};

/* 初始化函数集*/
aw_local aw_const struct awbl_drvfuncs __g_uvc_drvfuncs = {
        __drv_init1,      /* 第一阶段初始化*/
        __drv_init2,      /* 第二阶段初始化*/
        __drv_connect     /* 第三阶段初始化*/
};

/* USB功能驱动信息*/
aw_local aw_const struct awbl_usbh_fundrv_info __g_uvc_drv_info = {
    {
        AWBL_VER_1,                             /* AWbus版本号*/
        AWBL_BUSID_USBH | AWBL_DEVID_DEVICE,    /* 总线ID*/
        AWBL_USBH_UVC_DRV_NAME,                 /* 驱动名字*/
        &__g_uvc_drvfuncs,                      /* 初始化函数集*/
        __g_uvc_methods,                        /* 服务方法*/
        __drv_probe,                            /* 探测函数*/
    },
    __uvc_drv_info_tab                          /* 功能信息表*/
};

/** UVC驱动注册*/
void awbl_usbh_uvc_drv_register (void)
{
    /* 驱动注册*/
    awbl_drv_register((struct awbl_drvinfo *)&__g_uvc_drv_info);
}

