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
 * 初始化USB主机库
 *
 * @param hub_stack_size 集线器任务栈大小
 * @param hub_task_prio  集线器任务优先级
 *
 * @return 成功返回AW_OK
 */
aw_err_t awbl_usbh_lib_init (int stk_size, int stk_prio)
{
    aw_err_t          ret;
    struct usbh_lib_info  p_usbh_info;
    /* 打印出USB库的信息*/
    usbh_lib_info_get(&p_usbh_info);
    aw_kprintf("USB lib version  : %s\r\n"
            "    build date   : %s\r\n"
            "    modified date: %s\r\n",p_usbh_info.version ,
            p_usbh_info.build_date, p_usbh_info.modified_date);
    /* 初始化USB主机库，支持两个主机控制器*/
    ret = usbh_core_lib_init(2);
    if (ret != USB_OK) {
        return ret;
    }
    /* 初始化USB集线器库*/
    ret = usbh_hub_lib_init(stk_size, stk_prio);
    if (ret != USB_OK) {
        return ret;
    }
    /* 初始化USB大容量存储设备库*/
    ret = usbh_ms_lib_init();
    if (ret != USB_OK) {
        return ret;
    }
    /* 初始化USB视频设备库，支持2个视频数据流，每个视频数据流分配3个图像缓存*/
    ret = uvc_lib_init(2, 3);
    if(ret != USB_OK){
        return ret;
    }
    /* 初始化USB转串口设备库*/
    ret = usbh_serial_lib_init();
    if(ret != USB_OK){
        return ret;
    }
    /* 初始化USB音频设备库*/
    ret = usbh_audio_lib_init();
    if(ret != USB_OK){
        return ret;
    }
    /* 初始化AWorks USB主机*/
    return awbl_usbh_host_list_init();
}

/** 检查设备和驱动是否匹配 */
aw_local aw_bool_t __usbh_dev_match (const struct awbl_drvinfo *p_awdrv,
                                     struct awbl_dev           *p_awdev)
{
    __AWBL_USBH_FUN_DECL( p_fun, p_awdev);
    __AWBL_USBH_FUN_DRV(  p_drv, p_awdrv);
    const struct awbl_usbh_fundrv_info_tab *p_info;

    /* 检查参数*/
    if(p_fun->usb_func == NULL){
        return AW_FALSE;
    }
    /* 检查总线状态*/
    if (p_awdev->p_devhcf->bus_type != AWBL_BUSID_USBH) {
        return AW_FALSE;
    }
    /* 根据匹配标志匹配信息*/
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
        /* 设备类*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_CLASS) &&
            (p_info->dclss != USBH_DEV_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* 设备子类*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS) &&
            (p_info->dsclss != USBH_DEV_SUB_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* 设备协议*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL) &&
            (p_info->dproto != USBH_DEV_PROTOCOL_GET(p_fun->usb_func))) {
            continue;
        }
        /* 自定义类*/
        if ((USBH_DEV_CLASS_GET(p_fun->usb_func) == USB_CLASS_VENDOR_SPEC) &&
            !(p_info->flags & AWBL_USBH_FUNDRV_MATCH_VENDOR) &&
            (p_info->flags & (AWBL_USBH_FUNDRV_MATCH_FUN_CLASS |
                              AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS |
                              AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL |
                              AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER))) {
            continue;
        }
        /* 接口类*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_CLASS) &&
            (p_info->iclss != USBH_FUNC_CLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* 接口子类*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS) &&
            (p_info->isclss != USBH_FUNC_SUBCLASS_GET(p_fun->usb_func))) {
            continue;
        }
        /* 接口协议*/
        if ((p_info->flags & AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL) &&
            (p_info->iproto != USBH_FUNC_PROTO_GET(p_fun->usb_func))) {
            continue;
        }
        /* 接口数量*/
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

/* 总线信息*/
aw_local aw_const struct awbl_bustype_info __g_usbh_bustype = {
    AWBL_BUSID_USBH,        /* 总线ID*/
    NULL,                   /* 总线初始化第一阶段*/
    NULL,                   /* 总线初始化第二阶段*/
    NULL,                   /* 总线初始化第三阶段*/
    NULL,                   /* 检查总线上的新设备 */
    __usbh_dev_match        /* 总线匹配函数*/
};

/** 初始化AWorks USB总线*/
void awbl_usbh_bus_init (void)
{
    /* 总线注册*/
    awbl_bustype_register(&__g_usbh_bustype);
}

