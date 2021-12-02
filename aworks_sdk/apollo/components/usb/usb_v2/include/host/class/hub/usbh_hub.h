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
#ifndef __USBH_HUB_H
#define __USBH_HUB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "host/core/usbh_dev.h"
#include "list/usb_list.h"

/* USB根集线器数量*/
#define USBH_ROOT_HUB_NUM     2
/* USB集线器最大端口数量*/
#define USBH_HUB_MAX_PORTS    15

/**
 * USB TT(Transaction Translators)
 * 当低/全速设备接入高速集线器时，就会使用事务转换器
 */
struct usbh_tt {
    struct usbh_function *p_hub;      /* 上游高速集线器*/
    int                   multi;      /* 是否每个端口都有一个事物转换器*/
    uint32_t              think_time; /* 思考时间(纳秒)*/
};

/** USB 集线器结构体*/
struct usbh_hub {
    struct usbh_function   *p_fun;                        /* 相关的USB功能结构体*/
    struct usb_list_head    evt;                          /* 事件链表*/
    struct usb_list_head    node;                         /* 当前集线器节点*/
    struct usbh_trp         trp;                          /* 集线器传输请求包*/
    struct usbh_device     *ports[USBH_HUB_MAX_PORTS];    /* 集线器端口设备*/
    uint32_t               *p_sta;                        /* 集线器状态*/
    uint8_t                 err_cnt;                      /* 错误计数*/
    uint8_t                 nports;                       /* 端口数量*/
    uint8_t                 pwrtim;                       /* 上电时间*/
    usb_mutex_handle_t      lock;                         /* 互斥锁*/
    struct usbh_endpoint   *ep;                           /* 集线器的端点*/
    /* 以下两个字段是用于根集线器*/
    struct usbh_tt          tt;                           /* 事务转换器(低/全速设备接到高速集线器)*/
    uint8_t                 speed;                        /* 集线器的速度*/
};

/***************************************函数声明**************************************************/
/**
 * 内部创建集线器任务
 *
 * @param stk_size 栈大小
 * @param stk_prio 任务优先级
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_hub_lib_init (int stk_size, int stk_prio);
/**
 * 初始化根集线器
 *
 * @param p_hc    主机控制器
 * @param nports  端口数量
 * @param speed   根集线器速度
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_rh_init (struct usbh     *p_hc,
                        uint8_t          nports,
                        uint8_t          speed);
/**
 * USB 主机根集线器控制函数
 *
 * @param p_hc   USB主机结构体
 * @param type   控制请求类型
 * @param req    具体请求
 * @param val    值
 * @param idx    索引
 * @param len    数据长度
 * @param p_data 数据缓存
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_rh_ctrl (struct usbh        *p_hc,
                        uint8_t             type,
                        uint8_t             req,
                        uint16_t            val,
                        uint16_t            idx,
                        uint16_t            len,
                        void               *p_data);
/** USB主机根集线器检查函数*/
usb_err_t usbh_rh_check(struct usbh *p_hc, uint16_t *p_change);
/** USB主机根集线器变化函数*/
void usbh_rh_change (struct usbh *p_hc, uint16_t change);
/** 创建一个普通的集线器*/
struct usbh_hub *usbh_hub_create(struct usbh_function *p_ufunc);
/** 销毁一个集线器*/
void usbh_hub_destory(struct usbh_hub *p_hub);
/** 启动集线器*/
usb_err_t usbh_hub_start(struct usbh_hub *p_hub);
/**
 * 使能集线器端口
 *
 * @param p_usb_hub 集线器结构体
 * @param port_num  端口号
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_hub_port_enable(struct usbh_hub *p_hub, int port_num);
/** 禁能集线器端口*/
usb_err_t usbh_hub_port_disable(struct usbh_hub *p_hub);
/**
 * 检查集线器端口是否连接
 *
 * @param p_usb_hub 集线器结构体
 * @param port_num  端口号
 *
 * @return 成功返回USB_OK
 */
usb_bool_t usbh_hub_port_connect_check (struct usbh_hub *p_hub,
                                        uint8_t          port);
/**
 * 集线器端口重新初始化
 *
 * @param p_usb_hub 集线器结构体
 * @param port_num  端口号
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_hub_port_reinit(struct usbh_hub *p_hub, int port);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_HUB_H */
