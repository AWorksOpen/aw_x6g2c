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
#ifndef __USBD_H
#define __USBD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"

/* 打印函数*/
#define __USBD_TRACE(info, ...) \
            do { \
                usb_printf("USBD:"info, ##__VA_ARGS__); \
            } while (0)

/** 控制器上报的事件类型*/
#define USBD_EVT_CONNECT           0x00000001           /* 连接*/
#define USBD_EVT_DISCONNECT        0x00000002           /* 断开*/
#define USBD_EVT_BUS_RST           0x00000004           /* 总线复位*/
#define USBD_EVT_SUSPENDED         0x00000008           /* 挂起*/
#define USBD_EVT_RESUME            0x00000010           /* 回复*/
#define USBD_EVT_SETUP_PKT         0x00000020           /* setup包*/
#define USBD_EVT_UPDATE_SPEED      0x00000040           /* 更新速度*/
#define USBD_EVT_ERROR             0x00000080           /* 错误*/
#define USBD_EVT_DC_STOP           0x00000100           /* 设备控制器停止*/

/** 控制器状态*/
#define USBD_ST_NOTATTACHED       0           /* 断开*/
#define USBD_ST_ATTACHED          1           /* 连接但还没有枚举*/
#define USBD_ST_POWERED           2           /* 上电*/
#define USBD_ST_DEFAULT           3           /* 默认*/
#define USBD_ST_ADDRESS           4           /* 设置地址*/
#define USBD_ST_CONFIGURED        5           /* 配置*/
#define USBD_ST_SUSPENDED         6           /* 挂起*/

/** 控制器速度*/
#define USBD_SPEED_UNKNOWN        0
#define USBD_SPEED_LOW            1           /* usb 1.1 */
#define USBD_SPEED_FULL           2           /* usb 1.1 */
#define USBD_SPEED_HIGH           3           /* usb 2.0 */
#define USBD_SPEED_WIRELESS       4           /* 无线 (usb 2.5) */
#define USBD_SPEED_SUPER          5           /* usb 3.0 */

/** 传输标志*/
#define USBD_SHORT_NOT_OK      0x00000001     /* 短包错误标志*/
#define USBD_ZERO_PACKET       0x00000002     /* 用一个短包来结束一次传输*/
#define USBD_NO_INTERRUPT      0x00000004     /* 不需要中断，除非错误*/

/** 端点类型支持标志*/
#define USBD_EP_SUPPORT_CTRL    0x01
#define USBD_EP_SUPPORT_ISO     0x02
#define USBD_EP_SUPPORT_BULK    0x04
#define USBD_EP_SUPPORT_INT     0x08
#define USBD_EP_SUPPORT_ALL     0x0F

/** 端点最大包大小限制*/
#define USBD_EP_MPS_NO_LIMT     0x00

/** 控制器配置标志*/
#define USBD_CFG_SELF_POWERED     0x0200
#define USBD_CFG_REMOTE_WAKEUP    0x0400
#define USBD_CFG_MAX_POWER        0x01FF
#define USBD_CFG_MAX_POWER_MA(v)  (v & 0x1FF)

/** USB设备任务信息*/
#define USBD_TASK_STACK_SIZE    2048
#define USBD_TASK_PRIO          0

struct usbd_event {
    usb_mutex_handle_t      lock;
    usb_sem_handle_t        semb;
    uint32_t                event;
};

/** USB设备传输结构体*/
struct usbd_trans {
    struct usbd_ep  *p_hw;
    void            *p_buf;
    void            *p_buf_dma;
    size_t           len;
    void            (*complete) (void *p_arg);
    void            *p_arg;
    size_t           act_len;            /* 实际传输的字节数*/
    int              status;             /* 传输状态*/
    int              flag;
};

/** USB设备端点*/
struct usbd_ep {
    uint8_t                 ep_addr;       /* 端点地址*/
    uint8_t                 type_support;  /* 支持类型*/
    uint8_t                 cur_type;      /* 当前类型*/
    uint16_t                mps_limt;      /* 最大包大小限制*/
    uint16_t                cur_mps;       /* 当前最大包大小*/
    struct usb_list_head    node;          /* 端点节点*/
    usb_bool_t              enable;        /* 使能标志位*/
    usb_bool_t              used;          /* 使用标志位*/
};

/** USB设备端点0*/
struct usbd_ep0 {
    struct usbd       *p_usbd;
    struct usbd_ep    *p_hw;
    struct usbd_trans  req;
};

/** USB设备控制器*/
struct usbd {
    struct usbd_obj         *p_obj;
    const struct usbd_cb    *p_cb;
    usb_task_handle_t        usbd_task;
    usb_mutex_handle_t       mutex;
    usb_sem_handle_t         start;
    usb_sem_handle_t         done;
    //aw_spinlock_isr_t             lock;
    usb_bool_t               run;
    uint8_t                  state;
    uint8_t                  resume_state;
    uint8_t                  speed;
    uint8_t                  addr;
    uint8_t                  mps0;
    struct usb_list_head     ep_list;
    struct usbd_ep0          ep0_in;
    struct usbd_ep0          ep0_out;
    usb_bool_t               need_sta;
    struct usb_list_head     node;
    struct usbd_event        event;
    struct usb_ctrlreq       setup;
    void                    *usr_priv;
};

/** 控制器信息*/
struct usbd_info {
    const char  *name;   /* 控制器名字*/
    uint32_t     flag;   /* 标志*/
};

/** 控制器驱动函数集*/
struct usbd_drv {
    /* 控制器复位*/
    void (*reset) (struct usbd *p_usbd);
    /* 控制器启动*/
    usb_err_t (*run) (struct usbd *p_usbd);
    /* 控制器停止*/
    usb_err_t (*stop) (struct usbd *p_usbd);
    /* 传输请求*/
    usb_err_t (*transfer_req) (struct usbd       *p_usbd,
                               struct usbd_trans *p_trans);
    /* 传输请求*/
    usb_err_t (*transfer_cancel) (struct usbd       *p_usbd,
                                  struct usbd_trans *p_trans);
    /* 端点使能*/
    usb_err_t (*ep_enable) (struct usbd    *p_usbd,
                            struct usbd_ep *p_ep);
    /* 端点禁能*/
    usb_err_t (*ep_disable) (struct usbd    *p_usbd,
                             struct usbd_ep *p_ep);

    /* 复位端点，清除所有端点的状态和配置，调用之后必须再使能一次端点*/
    usb_err_t (*ep_reset) (struct usbd    *p_usbd,
                           struct usbd_ep *p_ep);
    /* 设置设备地址*/
    usb_err_t (*set_address) (struct usbd  *p_usbd,
                              uint8_t       addr);
    /* 设置配置*/
    usb_err_t (*set_config) (struct usbd  *p_usbd,
                             usb_bool_t    set);
    /* 获取或设置端点状态*/
    usb_err_t (*ep_status) (struct usbd    *p_usbd,
                            uint8_t         ep_addr,
                            usb_bool_t      is_set,
                            uint16_t       *status);
    /* 设置或获取端点特性*/
    usb_err_t (*ep_feature) (struct usbd    *p_usbd,
                             uint8_t         ep_addr,
                             usb_bool_t      is_set);
    /* 唤醒控制器*/
    usb_err_t (*wakeup)(struct usbd *p_usbd);
    /* 上拉或断开 D+/D-上的电阻*/
    usb_err_t (*pullup)(struct usbd *p_usbd, usb_bool_t on);
};

/** 控制器控制函数集*/
struct usbd_cb {
    /* 接收一个控制SETUP包*/
    int (*ctrl_setup) (struct usbd        *p_usbd,
                       struct usb_ctrlreq *p_setup,
                       void               *p_buf,
                       int                 buf_len);
    /* 总线复位*/
    void (*reset) (struct usbd *p_usbd);
    /* 总线挂起*/
    void (*suspend) (struct usbd *p_usbd);
    /* 总线恢复*/
    void (*resume) (struct usbd  *p_usbd);
    /* 总线断开*/
    void (*disconnect) (struct usbd *p_usbd);
};

/***************************************函数声明**************************************************/
/** 设置USB设备控制器私有数据*/
usb_err_t usbd_set_priv(struct usbd *p_usbd, void *data);
/** 创建一个USB设备控制器*/
int usbd_create (struct usbd *p_usbd);
/** 处理USB设备控制器事件*/
int usbd_event_handle (struct usbd  *p_usbd,
                       uint32_t      event,
                       void         *arg);
/** 注册USB控制器端点*/
int usbd_ep_register (struct usbd    *p_usbd,
                      struct usbd_ep *p_ep,
                      uint8_t         ep_addr,
                      uint8_t         type_support,
                      uint16_t        mps_limt);
/**
 * 端点申请
 *
 * @param p_usbd USB设备控制器
 * @param addr   端点地址
 * @param type   端点类型
 * @param mps    端点最大包长度
 *
 * @return 成功返回 USB设备端点结构体
 */
 struct usbd_ep *usbd_ep_alloc (struct usbd  *p_usbd,
                                uint8_t       addr,
                                uint8_t       type,
                                uint16_t      mps);
/**
 * 释放USB设备控制器端点
 *
 * @param p_usbd USB设备控制器
 * @param p_ep   要释放的端点
 *
 * @return 成功返回USB_OK*/
int usbd_ep_free (struct usbd    *p_usbd,
                  struct usbd_ep *p_ep);
/**
 * 使能设备端点
 *
 * @param p_usbd USB设备控制器
 * @param p_ep   要禁能的端点
 *
 * @return 成功返回USB_OK
 */
int usbd_ep_enable (struct usbd    *p_usbd,
                    struct usbd_ep *p_ep);
/**
 * 禁能设备端点
 *
 * @param p_usbd USB设备控制器
 * @param p_ep   要禁能的端点
 *
 * @return 成功返回USB_OK
 */
int usbd_ep_disable (struct usbd    *p_usbd,
                     struct usbd_ep *p_ep);
/** 复位USB控制器端点*/
int usbd_ep_reset (struct usbd    *p_usbd,
                   struct usbd_ep *p_ep);
/** 设置端点特性*/
int usbd_ep_feature (struct usbd    *p_usbd,
                     struct usbd_ep *p_ep,
                     usb_bool_t      set);
/**
 * 通过名字寻找对应的USB设备控制器
 *
 * @param name 名字
 *
 * @return 找到返回USB设备控制器结构体，找不到返回NULL
 */
struct usbd *usbd_find_dev_controller (const char *name);
/**
 * 启动USB设备对象
 *
 * @param p_ubd USB设备控制器
 * @param p_obj USB设备对象
 * @param p_cb  USB设备控制器驱动函数集
 *
 * @return 成功返回USB_OK
 */
int usbd_start (struct usbd          *p_usbd,
                struct usbd_obj      *p_obj,
                const struct usbd_cb *p_cb);
/** 停止USB设备控制器*/
int usbd_stop (struct usbd  *p_usbd);
/**
 * USB设备传输请求函数
 *
 * @param p_usbd  USB设备控制器
 * @param p_trans USB设备传输
 *
 * @return 成功返回USB_OK
 */
int usbd_trans_req (struct usbd        *p_usbd,
                    struct usbd_trans  *p_trans);
/** USB设备传输取消*/
int usbd_trans_cancel (struct usbd        *p_usbd,
                       struct usbd_trans  *p_trans);
/** 获取USB设备控制器属性*/
int usbd_attribute_get (struct usbd  *p_usbd,
                        uint32_t     *attr);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBD_H */

