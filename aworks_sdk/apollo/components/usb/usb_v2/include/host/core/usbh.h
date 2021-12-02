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
#ifndef __USBH_H
#define __USBH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "list/usb_list.h"
#include "usb_common.h"
#include "usb_common_def.h"
#include "host/core/usbh_dev.h"
#include "host/class/hub/usbh_hub.h"

/* USB主机信息结构体*/
struct usbh_lib_info{
    char version[32];
    char build_date[32];
    char modified_date[32];
};

/* USBH内存记录结构体*/
struct usbh_mem_record {
    char     drv_name[32];
    uint32_t mem_alloc_num;
    uint32_t mem_free_num;
    uint32_t mem_total_num;
    uint32_t mutex_alloc_num;
    uint32_t mutex_free_num;
    uint32_t sem_alloc_num;
    uint32_t sem_free_num;
};

/* USB 主机结构体*/
struct usbh {
    uint8_t              host_index;  /* 主机索引*/
    uint32_t             map[4];      /* 设备地址表*/
    struct usbh_hub      rh;          /* 根集线器*/
    usb_mutex_handle_t   lock;        /* 互斥锁*/
    struct usb_list_head node;        /* 当前主机控制器节点*/
    void                *controller;  /* 主机控制器*/
    usb_bool_t           is_init;     /* 是否初始化*/
    uint8_t             *user_priv;   /* 用户私有数据*/
};

/* USB主机驱动回调函数*/
struct usb_ctrllor_drv {
    /* 请求传输*/
    usb_err_t (*pfn_request) (struct usbh     *p_hc,
                              struct usbh_trp *p_trp);

    /* 取消传输*/
    usb_err_t (*pfn_cancel) (struct usbh     *p_hc,
                             struct usbh_trp *p_trp);
    /* 根集线器控制*/
    usb_err_t (*pfn_rh_ctrl) (struct usbh        *p_hc,
                              uint8_t             type,
                              uint8_t             req,
                              uint16_t            val,
                              uint16_t            idx,
                              uint16_t            len,
                              void               *p_data);
    /* 获取根集线器状态*/
    usb_err_t (*pfn_rh_check) (struct usbh      *p_hc,
                               uint16_t         *p_change);
    /* 使能端点*/
    usb_err_t (*pfn_ep_en) (struct usbh          *p_hc,
                            struct usbh_endpoint *p_ep);
    /* 禁能端点*/
    usb_err_t (*pfn_ep_dis) (struct usbh          *p_hc,
                             struct usbh_endpoint *p_ep);
    /* 获取当前帧编号*/
    int (*pfn_framenum_get)(struct usbh *p_hc);
};

/* 主机控制器类型*/
enum{
    UNKNOW = 0,
    OHCI   = 1,
    EHCI   = 2,
    XHCI   = 3,
    VENDOR = 4,
};

/* 控制器类型名字结构体*/
struct usbh_ctrllor_type{
    uint8_t type;
    char    name[8];
};

/* USB 主机控制器结构体头部*/
struct usbh_ctrllor_head {
    uint8_t                 ctrllor_type;    /* 控制器类型*/
    struct usb_ctrllor_drv *ctrllor_drv;     /* 控制器驱动函数集*/
    struct usbh            *p_hc;            /* USB主机*/
    usb_bool_t              is_init;         /* 控制器是否已经初始化*/
};

/* 获取设备VID*/
#define USBH_DEV_VID_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->idVendor))
/* 获取设备PID*/
#define USBH_DEV_PID_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->idProduct))
/* 获取设备速度*/
#define USBH_DEV_SPEED_GET(p_fun)          ((p_fun)->p_udev->speed)
/* 获取设备类*/
#define USBH_DEV_CLASS_GET(p_fun)          ((p_fun)->p_udev->p_desc->bDeviceClass)
/* 获取设备子类*/
#define USBH_DEV_SUB_CLASS_GET(p_fun)      ((p_fun)->p_udev->p_desc->bDeviceSubClass)
/* 获取设备协议*/
#define USBH_DEV_PROTOCOL_GET(p_fun)       ((p_fun)->p_udev->p_desc->bDeviceProtocol)
/* 获取设备配置数量*/
#define USBH_DEV_NCFGS_GET(p_fun)          ((p_fun)->p_udev->p_desc->bNumConfigurations)
/* 获取设备名*/
#define USBH_DEV_NAME_GET(p_fun)           ((p_fun)->p_udev->name)
/* 获取设备类型*/
#define USBH_DEV_TYPE_GET(p_fun)           ((p_fun)->p_udev->dev_type)
/* 获取设备版本*/
#define USBH_DEV_BCD_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->bcdDevice))
/* 获取设备产商描述字符串*/
#define USBH_DEV_PDTSTR_GET(p_fun)         ((p_fun)->p_udev->p_str_pdt)
/* 获取设备当前配置接口数量*/
#define USBH_DEV_CFG_INTFNUM_GET(p_fun)    ((p_fun)->p_udev->cfg.p_desc->bNumInterfaces)

/* 获取功能第一个接口号*/
#define USBH_FUNC_FIRST_INTFNUM_GET(p_fun) ((p_fun)->ifirst)
/* 获取功能接口数量*/
#define USBH_FUNC_NINTF_GET(p_fun)         ((p_fun)->nitfs)
/* 获取功能接口协议*/
#define USBH_FUNC_PROTO_GET(p_fun)         ((p_fun)->proto)
/* 获取功能接口类*/
#define USBH_FUNC_CLASS_GET(p_fun)         ((p_fun)->clss)
/* 获取功能接口子类*/
#define USBH_FUNC_SUBCLASS_GET(p_fun)      ((p_fun)->sclss)

/* 获取集线器端口数量*/
#define USBH_HUB_PORT_NUM_GET(p_hub)       ((p_hub)->nports)

/* 获取接口子类*/
#define USBH_INTF_SUB_CLASS_GET(p_intf)    ((p_intf)->p_desc->bInterfaceSubClass)
/* 获取接口类*/
#define USBH_INTF_CLASS_GET(p_intf)        ((p_intf)->p_desc->bInterfaceClass)
/* 获取接口端点数量*/
#define USBH_INTF_EP_NUM_GET(p_intf)       ((p_intf)->p_desc->bNumEndpoints)
/* 获取接口编号*/
#define USBH_INTF_NUM_GET(p_intf)          ((p_intf)->p_desc->bInterfaceNumber)
/* 获取接口备用设置编号*/
#define USBH_INTF_ALT_NUM_GET(p_intf)      ((p_intf)->p_desc->bAlternateSetting)

/* 获取端点类型*/
#define USBH_EP_TYPE_GET(p_ep)             ((p_ep)->p_desc->bmAttributes & 0x03)
/* 获取端点方向*/
#define USBH_EP_DIR_GET(p_ep)              ((p_ep)->p_desc->bEndpointAddress & 0x80)
/* 检查端点是不是中断端点*/
#define USBH_EP_INT_GET(p_ep)              (((p_ep)->p_desc->bmAttributes & USB_EP_TYPE_INT) == USB_EP_TYPE_INT)
/* 检查端点是不是等时端点*/
#define USBH_EP_ISO_GET(p_ep)              (((p_ep)->p_desc->bmAttributes & USB_EP_TYPE_ISO) == USB_EP_TYPE_ISO)
/* 获取端点地址*/
#define USBH_EP_ADDR_GET(p_ep)             ((p_ep)->p_desc->bEndpointAddress & 0x0F)
/* 获取端点属性*/
#define USBH_EP_ATTR_GET(p_ep)             ((p_ep)->p_desc->bmAttributes)
/* 获取端点最大包大小*/
#define USBH_EP_MPS_GET(p_ep)              (USB_CPU_TO_LE16((p_ep)->p_desc->wMaxPacketSize))
/* 获取端点轮询周期*/
#define USBH_EP_INTVAL_GET(p_ep)           ((p_ep)->p_desc->bInterval)
/* 检查端点是不是输入中断端点*/
static inline int USBH_EP_IS_INT_IN(const struct usbh_endpoint *ep)
{
    return USBH_EP_DIR_GET(ep) && USBH_EP_INT_GET(ep);
}

static inline int USBH_EP_IS_ISO_IN(const struct usbh_endpoint *ep)
{
    return USBH_EP_DIR_GET(ep) && USBH_EP_ISO_GET(ep);
}

/***************************************函数声明**************************************************/
/** 获取USB协议栈信息*/
void usbh_lib_info_get(struct usbh_lib_info *p_usbh_info);
/**
 * 初始化USB主机库
 *
 * @param ctrllor_num 支持的主机控制器数量
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_core_lib_init(uint8_t ctrllor_num);
/** USB主机库反初始化*/
usb_err_t usbh_core_lib_uninit(void);
/** 通过索引获取USB主机控制器*/
struct usbh *usbh_hc_get(uint8_t hc_index);
/** 设置主机用户私有数据*/
usb_err_t usbh_usrdata_set(struct usbh *p_hc, void *usrdata);
/** 获取主机用户私有数据*/
void* usbh_usrdata_get(struct usbh *p_hc);
/**
 * 创建USB主机控制器
 *
 * @param hc_index 主机控制器结构体索引
 *
 * @return 成功返回创建的USB主机结构体
 */
struct usbh *usbh_create(uint8_t host_index);
/**
 * 销毁USB主机控制器
 *
 * @param p_hc 要销毁的主机控制器结构体
 */
void usbh_destory(struct usbh *p_hc);
/** 设置USB 主机控制器*/
usb_err_t usbh_ctrllor_set(struct usbh *p_hc, void *ctrllor);
/** 获取USB 主机控制器*/
void *usbh_ctrllor_get(struct usbh *p_hc);
/** USB主机分配设备地址*/
int usbh_dev_addr_alloc (struct usbh *p_hc);
/** 释放设备地址*/
usb_err_t usbh_dev_addr_free (struct usbh *p_hc, int addr);
/** 获取当前帧编号*/
usb_err_t usbh_frame_num_get(struct usbh *p_hc);
/**
 * USB主机端点使能
 *
 * @param p_hc USB主机结构体
 * @param p_ep 要使能的端点
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ep_enable (struct usbh *p_hc, struct usbh_endpoint *p_ep);
/**
 * USB主机端点禁用函数
 *
 * @param p_hc USB主机结构体
 * @param p_ep 要禁用的端点
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ep_disable (struct usbh *p_hc, struct usbh_endpoint *p_ep);
/**
 * USB主机请求传输函数
 *
 * @param p_hc  USB主机结构体
 * @param p_trp 要提交的传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_request (struct usbh *p_hc, struct usbh_trp *p_trp);
/**
 * USB主机取消函数
 *
 * @param p_hc  USB主机结构体
 * @param p_trp 要取消的传输请求包
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_cancel (struct usbh     *p_hc,
                       struct usbh_trp *p_trp);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_H */

