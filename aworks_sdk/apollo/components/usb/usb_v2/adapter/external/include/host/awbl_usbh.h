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
#ifndef __AWBL_USBH_H
#define __AWBL_USBH_H
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"

#include "host/core/usbh.h"
#include "usb_common_def.h"
#include "list/usb_list.h"
#include "usb_adapter_inter.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* 通过基类获取USB功能结构体*/
#define __AWBL_USBH_FUN_DECL(p_fun, p_awdev) \
    struct awbl_usbh_function *p_fun = \
        (struct awbl_usbh_function *)AW_CONTAINER_OF( \
            p_awdev, struct awbl_usbh_function, awdev)
/* 通过驱动信息获取USB 功能驱动信息*/
#define __AWBL_USBH_FUN_DRV(p_drv, p_awdrv) \
    struct awbl_usbh_fundrv_info *p_drv = \
        (struct awbl_usbh_fundrv_info *)AW_CONTAINER_OF( \
            p_awdrv, struct awbl_usbh_fundrv_info, awdrv)

struct awbl_usbh_trp;

/** USB 主机结构体*/
struct awbl_usbh {
    struct awbl_busctlr         super;    /* 总线控制器*/
    AW_MUTEX_DECL(mutex);                 /* 声明一个互斥锁*/
    struct usbh                *hc;
    struct aw_delayed_work      rh_work;
    struct awbl_usbh           *p_next;
};

/** USB 主机信息*/
struct awbl_usbh_info {
    uint8_t     bus_index;
};

/* 设备特定的配置信息*/
struct awbl_usbh_dev_spec_config_info{
    uint16_t pid;
    uint16_t vid;
    uint16_t config_num;   /* 配置编号*/
};

#define AWBL_USBH_FUNDRV_INFO_END  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/** USB功能驱动信息匹配表*/
struct awbl_usbh_fundrv_info_tab {
    uint16_t                    flags;                 /* 匹配标志*/
#define AWBL_USBH_FUNDRV_MATCH_VENDOR          0x0001  /* 匹配VID*/
#define AWBL_USBH_FUNDRV_MATCH_PRODUCT         0x0002  /* 匹配PID*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_CLASS       0x0010  /* 匹配设备类代码*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS    0x0020  /* 匹配设备子类代码*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL    0x0040  /* 匹配设备协议代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_CLASS       0x0080  /* 匹配接口类代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS    0x0100  /* 匹配接口子类代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL    0x0200  /* 匹配接口协议代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER      0x0400  /* 匹配设备接口数量*/
    uint16_t                    vid;
    uint16_t                    pid;

    uint8_t                     dclss;
    uint8_t                     dsclss;
    uint8_t                     dproto;

    uint8_t                     iclss;
    uint8_t                     isclss;
    uint8_t                     iproto;
    uint8_t                     inum;
};

/** USB 设备驱动信息*/
struct awbl_usbh_fundrv_info {
    struct awbl_drvinfo                     awdrv;
    const struct awbl_usbh_fundrv_info_tab *p_tab;
};

/** USB功能结构体*/
struct awbl_usbh_function {
    struct awbl_dev       awdev;       /* 基础设备类*/
    struct awbl_devhcf    devhcf;      /* 设备硬件信息*/
    struct usbh_function *usb_func;    /* USB内部功能结构体*/
};

aw_err_t awbl_usbh_lib_init (int stk_size, int stk_prio);
aw_err_t awbl_usbh_host_list_init (void);
/**
 * 创建一个USB主机
 *
 * @param p_hc   USB主机结构体
 *
 * @return 成功返回AW_OK
 */
aw_err_t awbl_usbh_host_create (struct awbl_usbh *p_hc);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_H */
