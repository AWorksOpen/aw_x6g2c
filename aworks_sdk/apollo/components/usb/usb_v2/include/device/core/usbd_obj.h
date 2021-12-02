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


#ifndef __USBD_OBJ_H
#define __USBD_OBJ_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "device/core/usbd.h"
#include "usb_errno.h"
#include "list/usb_list.h"

/** \brief 管道方向定义 */
#define USB_PIPE_DIR_IN      USB_DIR_IN       /**< \brief 到主机*/
#define USB_PIPE_DIR_OUT     USB_DIR_OUT      /**< \brief 到设备*/

/** \brief 管道类型定义 */
#define USB_PIPE_TYPE_CTRL   USB_EP_TYPE_CTRL     /**< \brief 控制管道 */
#define USB_PIPE_TYPE_ISO    USB_EP_TYPE_ISO      /**< \brief 等时管道 */
#define USB_PIPE_TYPE_BULK   USB_EP_TYPE_BULK     /**< \brief 批量管道 */
#define USB_PIPE_TYPE_INT    USB_EP_TYPE_INT      /**< \brief 中断管道 */

/** \brief USBD传输结构定义 */
struct usbd_obj_trans {
    struct usbd_obj_pipe *p_pipe;    /**< \brief 管道指针 */
    struct usbd_trans     tr;        /**< \brief 传输结构 */
};

/** \brief USBD对象管道信息结构定义 */
struct usbd_obj_pipe_info {
    uint8_t  num;       /**< \brief 端点地址    0为自动分配 */
    uint8_t  dir;       /**< \brief 端点方向    1:IN 0:OUT */
    uint8_t  attr;      /**< \brief 传输类型和补充信息 */
    uint8_t  inter;     /**< \brief 中断和等时端点的服务时距 */
    uint16_t mps;       /**< \brief 端点每个事务允许最大数据字节数 */
};

/** \brief USBD对象管道类型定义 */
struct usbd_obj_pipe {
    struct usbd_ep                  *p_hw;      /**< \brief 端点指针 */
    const struct usbd_obj_pipe_info *p_info;    /**< \brief 管道信息指针 */
    struct usbd_obj_fun             *p_fun;     /**< \brief 接口指针 */
    uint8_t                          addr;      /**< \brief 管道地址 */
    struct usb_list_head             node;      /**< \brief 节点 */
    usb_sem_handle_t                 sync;
};

/** \brief 接口联合描述符*/
struct usbd_obj_iad {
    uint8_t    first_if;      /* 第一个接口编号*/
    uint8_t    if_count;      /* 接口数量*/
    uint8_t    fun_class;     /* 接口类*/
    uint8_t    fun_subclass;  /* 接口子类*/
    uint8_t    fun_protocol;  /* 接口协议*/
    uint8_t    ifunc;
    usb_bool_t used;         /* 是否被使用*/
};

/** \brief USBD对象接口信息 */
struct usbd_obj_fun_info {
    uint8_t       clss;         /**< \brief 类代码 */
    uint8_t       sub_clss;     /**< \brief 子类代码 */
    uint8_t       protocol;     /**< \brief 协议代码 */

    /** \brief 替代设置回调 */
    int (*pfn_alt_set) (struct usbd_obj_fun *p_fun,
                        usb_bool_t           set);

    /** \brief setup控制传输回调 */
    int (*pfn_ctrl_setup) (struct usbd_obj_fun *p_fun,
                           struct usb_ctrlreq  *p_setup,
                           void                *p_buf,
                           int                  buf_len);
};

/** \brief USBD对象接口结构定义 */
struct usbd_obj_fun {
    struct usbd_obj                *p_obj;      /**< \brief USBD设备指针 */
    const char                     *name;       /**< \brief 接口名称 */
    const struct usbd_obj_fun_info *p_info;     /**< \brief 接口 信息 */
    uint8_t                         num;        /**< \brief 接口编号 */
    uint8_t                         alt;        /**< \brief 替代设置编号 */
    uint8_t                         num_alt;    /**< \brief 只有设置0有效*/
    struct usbd_obj_fun            *p_next_alt; /**< \brief 最后一个为NULL*/
    usb_bool_t                      need_iad;
    uint8_t                        *p_spec_des; /**< \brief 类自定义描述符*/
    uint8_t                         spec_size;  /**< \brief 类自定义描述符大小*/
    usb_bool_t                      set;        /**< \brief 是否设置 */
    uint8_t                         neps;       /**< \brief 端点数量 */
    struct usb_list_head            pipes;      /**< \brief 管道链表 */
    struct usb_list_head            node;       /**< \brief 节点 */
    uint8_t                         strid;      /**< \brief 字符串索引 */
};

/* 接口联合描述符最大数量*/
#define USBD_MAX_IAD_NUM     10

/** \brief USBD对象配置结构定义 */
struct usbd_obj_cfg {
    struct usbd_obj       *p_obj;                     /**< \brief USBD设备指针 */
    struct usbd_obj_iad    iads[USBD_MAX_IAD_NUM];    /**< \brief 接口联合描述符*/
    struct usb_list_head   itfs;                      /**< \brief 接口链表 */
    uint8_t                nitfs;                     /**< \brief 接口数量 */
    uint8_t                value;                     /**< \brief 配置编号 */
    struct usb_list_head   node;                      /**< \brief 节点 */
};

/** \brief USBD对象信息结构定义 */
struct usbd_obj_info {
    uint8_t          clss;          /**< \brief 类代码 */
    uint8_t          sub_clss;      /**< \brief 子类代码 */
    uint8_t          protocol;      /**< \brief 协议代码 */
    uint8_t          mps0;          /**< \brief 端点0包最大尺寸 */
    uint16_t         bcd;           /**< \brief 设备版本号 */
    uint16_t         vid;           /**< \brief 厂商ID */
    uint16_t         pid;           /**< \brief 产品ID */
    const char      *manufacturer;  /**< \brief 制造商字符串描述 */
    const char      *product;       /**< \brief 产品字符串描述 */
    const char      *serial;        /**< \brief 序列号字符串描述 */
};

/** \brief USBD对象结构定义 */
struct usbd_obj {
    struct usbd                *p_usbd;     /**< \brief USBD设备指针 */
    const struct usbd_obj_info *p_info;     /**< \brief 设备信息 */
    struct usbd_obj_cfg        *p_cur_cfg;  /**< \brief 当前配置指针 */
    struct usbd_obj_cfg         def_cfg;    /**< \brief 默认配置 */
    struct usb_list_head        cfgs;       /**< \brief 配置链表 */
    uint8_t                     ncfgs;      /**< \brief 配置数量 */
    const struct usbd_obj_cb   *p_cb;       /**< \brief 设备类回调函数列表 */
    usb_mutex_handle_t          lock;
    uint8_t                     status;     /**< \brief 设备状态 */
    uint8_t                     strid_mf;   /**< \brief 厂商字符串索引 */
    uint8_t                     strid_pd;   /**< \brief 产品字符串索引 */
    uint8_t                     strid_sn;   /**< \brief 序列号字符串索引 */
};

/** \brief USB设备类回调函数列表 */
struct usbd_obj_cb {
    /** \brief 控制器链接到设备 */
    int (*pfn_link) (struct usbd_obj *p_obj);
    /** \brief 控制器和设备断开链接 */
    int (*pfn_unlink) (struct usbd_obj *p_obj);
    /** \brief 接收到SETUP包 */
    int (*pfn_ctrl_setup) (struct usbd_obj    *p_obj,
                           struct usb_ctrlreq *p_setup,
                           void               *p_buf,
                           int                 buf_len);
    /** \brief 设置或清除配置 */
    int (*pfn_config_set) (struct usbd_obj      *p_obj,
                           struct usbd_obj_cfg  *p_cfg,
                           usb_bool_t        set);
    /** \brief 总线挂起 */
    void (*pfn_suspend) (struct usbd_obj *p_obj);
    /** \brief 总线恢复 */
    void (*pfn_resume) (struct usbd_obj  *p_obj);
    /** \brief 总线断开连接 */
    void (*pfn_disconnect) (struct usbd_obj *p_obj);
};

/***************************************函数声明**************************************************/
/**
 * \brief USB device对象初始化函数
 *
 * @param p_obj      USBD对象指针
 * @param p_info     USBD设备信息结构指针
 * @param p_cb       USBD回调函数列表指针
 * @param controller USB device控制器名称
 *
 * @return USB_OK    初始化成功
 */
int usbd_obj_init (struct usbd_obj            *p_obj,
                   const struct usbd_obj_info *p_info,
                   const struct usbd_obj_cb   *p_cb,
                   const char                 *controller);
/**
 * 初始化接口功能
 *
 * @param p_fun  USB接口结构
 * @param name   接口名字
 * @param p_info USB接口信息
 *
 * @return 成功返回USB_OK
 */
int usbd_obj_fun_init (struct usbd_obj_fun            *p_fun,
                       const char                     *name,
                       const struct usbd_obj_fun_info *p_info);
/**
 * 添加功能接口特定描述符
 *
 * @param p_fun  USB功能接口结构体
 * @param p_desc 特定描述符
 * @param size   特定描述符大小
 *
 * @return 成功返回USB_OK
 */
int usbd_fun_spec_desc_add (struct usbd_obj_fun *p_fun,
                            uint8_t             *p_desc,
                            uint8_t              size);
/**
 * 创建一个接口联合描述符
 *
 * @param p_fun     USB接口功能结构体
 * @param cfg_value 配置编号(0表示默认第一个配置)
 * @param count     包含的接口数量
 *
 * @return 成功返回USB_OK
 */
int usbd_fun_iad_create(struct usbd_obj_fun *p_fun,
                        uint8_t              cfg_value,
                        uint8_t              count);
/** 删除USB设备接口功能*/
int usbd_fun_del (struct usbd_obj     *p_obj,
                  uint8_t              value,
                  struct usbd_obj_fun *p_fun);
/**
 * 创建USB管道
 *
 * @param p_fun  USB设备接口结构体
 * @param p_pipe USB设备管道定义
 * @param p_info USB管道信息结构
 *
 * @return 成功返回USB_OK
 */
int usbd_obj_pipe_create (struct usbd_obj_fun              *p_fun,
                          struct usbd_obj_pipe             *p_pipe,
                          const struct usbd_obj_pipe_info  *p_info);
/**
 * 把USB接口添加到USB设备对象
 *
 * @param p_obj USB设备对象
 * @param value 配置编号(0表示使用USB对象的默认配置，每个USB对象都有一个默认配置)
 * @param p_fun USB接口结构体
 *
 * @return 成功返回USB_OK
 */
int usbd_obj_fun_add (struct usbd_obj     *p_obj,
                      uint8_t              value,
                      struct usbd_obj_fun *p_fun);
/**
 * 销毁USB管道
 *
 * @param p_fun  USB设备接口结构体
 * @param p_pipe USB设备管道定义
 *
 * @return 成功返回USB_OK
 */
int usbd_obj_pipe_destory (struct usbd_obj_fun *p_fun,
                           struct usbd_obj_pipe *p_pipe);
/**
 * 启动USB设备对象
 *
 * @param p_obj USB设备对象
 *
 * @return 成功返回USB_OK
 */
int usbd_obj_start (struct usbd_obj *p_obj);
/**
 * 填充传输请求
 *
 * @param p_trans  传输请求结构体
 * @param p_pipe   进行传输的管道
 * @param p_buf    数据缓存
 * @param len      数据长度
 * @param flag     标志
 * @param complete 传输完成回调函数
 * @param p_arg    传输完成回调函数参数
 */
void usbd_fill_trans (struct usbd_obj_trans *p_trans,
                      struct usbd_obj_pipe  *p_pipe,
                      void                  *p_buf,
                      size_t                 len,
                      int                    flag,
                      void                 (*complete) (void *p_arg),
                      void                  *p_arg);
/**
 * USB设备异步传输函数
 *
 * @param p_obj   USB设备对象
 * @param p_trans USB传输请求结构体
 *
 * @return 成功返回USB_OK
 */
int usbd_trans_async (struct usbd_obj        *p_obj,
                      struct usbd_obj_trans  *p_trans);
/**
 * USB设备同步传输函数
 *
 * @param p_obj   USB设备对象
 * @param p_pipe  使用的USB管道
 * @param p_buf   数据缓存
 * @param len     数据长度
 * @param flag    标志
 * @param timeout 超时时间
 *
 * @return 传输成功返回传输的数据长度，传输失败返回错误代码
 */
int usbd_trans_sync (struct usbd_obj      *p_obj,
                     struct usbd_obj_pipe *p_pipe,
                     void                 *p_buf,
                     size_t                len,
                     int                   flag,
                     int                   timeout);
/**
 * 检查USB设备功能接口是否可用
 *
 * @param p_fun 要检查的USB设备功能接口
 *
 * @return 可用返回USB_TRUE，不可用返回USB_FALSE
 */
usb_bool_t usbd_obj_fun_valid (struct usbd_obj_fun *p_fun);
/** 停止USB设备对象管道*/
int usbd_obj_pipe_stall (struct usbd_obj       *p_obj,
                         struct usbd_obj_pipe  *p_pipe,
                         usb_bool_t             set);
/** 复位USB设备对象管道*/
int usbd_obj_pipe_reset (struct usbd_obj       *p_obj,
                         struct usbd_obj_pipe  *p_pipe);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBD_OBJ_H */
