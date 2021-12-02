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
#ifndef __USBD_MS_H
#define __USBD_MS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"
#include "device/core/usbd.h"
#include "device/core/usbd_obj.h"
#include <string.h>
#include <stdio.h>

/* 打印函数*/
#define __USBD_MS_TRACE(info, ...) \
            do { \
                usb_printf("USBD-MS:"info, ##__VA_ARGS__); \
            } while (0)

/** \brief 大容量存储器任务优先级和堆栈大小 */
#define USBD_MS_TASK_PRIO    8
#define USBD_MS_STACK_SIZE   2048

/** \brief 设备逻辑单元 */
struct usbd_ms_lun {
    void                   *usr_priv;  /**< \brief 用户私有数据*/
    int                     num;       /**< \brief 逻辑单元号 */
    struct usb_list_head    node;      /**< \brief 逻辑单元链表节点 */
    uint32_t                sd;        /**< \brief SCSI sense key */
    uint32_t                sd_inf;    /**< \brief SCSI sense信息 */
    usb_bool_t              inf_valid; /**< \brief sense info是否有效 */
    usb_bool_t              ro;        /**< \brief 是否只读 */
    usb_bool_t              rm;        /**< \brief 是否可移除 */
    usb_bool_t              pv;        /**< \brief prevent标志 */
    uint32_t                bsize;     /**< \brief 块大小 */
    uint32_t                nblks;     /**< \brief 块数量 */
//    usb_mutex_handle_t   lock;    //TODO 后续版本需要处理多线程通信问题
    usb_sem_handle_t        blk_sync;
    usb_sem_handle_t        usb_sync;
};

/** \brief 大容量存储器设备 */
struct usbd_ms {
    struct usbd_obj_fun     ufun;       /**< \brief USB功能接口*/
    struct usbd_obj_pipe    in;         /**< \brief IN数据管道 */
    struct usbd_obj_pipe    out;        /**< \brief OUT数据管道 */

    usb_task_handle_t       ms_task;    /**< \brief 大容量存储器任务 */
    usb_sem_handle_t        semb;
    usb_mutex_handle_t      mutex;

    uint8_t                 nluns;      /**< \brief 逻辑单元个数 */
    struct usb_list_head    luns;       /**< \brief 逻辑单元链表 */
    void                   *p_ubuf;     /**< \brief USBD缓冲区 */
    void                   *p_bbuf;     /**< \brief LUN缓冲区 */
    uint32_t                buf_size;   /**< \brief 缓冲区大小 */
    usb_bool_t              buf_auto;   /**< \brief 是否动态分配内存 */

    uint32_t                tag;        /**< \brief CBW/CSW标签 */
    uint32_t                dlen;       /**< \brief CBW数据长度 */
    uint32_t                cdlen;      /**< \brief CBW有效字节长度 */
    uint8_t                 cb[16];     /**< \brief CBW命令 */
    uint8_t                 dir;        /**< \brief CBW标志 */
    uint8_t                 clen;       /**< \brief CBW命令长度 */
    uint8_t                 sta;        /**< \brief CSW状态 */
    struct usbd_ms_lun     *p_lun;      /**< \brief 当前使用的逻辑单元 */

};


/**
 * 创建一个大容量USB设备
 *
 * @param p_obj     USB设备对象
 * @param p_ms      USB大容量存储设备对象
 * @param name      USB大容量存储设备名字
 * @param p_buf     缓存
 * @param buf_size  缓存大小
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_ms_create (struct usbd_obj     *p_obj,
                          struct usbd_ms      *p_ms,
                          const char          *name,
                          void                *p_buf,
                          int                  buf_size);
/** 销毁大容量存储USB设备*/
int usbd_ms_destroy (struct usbd_ms  *p_ms);
/** 添加一个逻辑单元*/
int usbd_ms_lun_add (struct usbd_ms *p_ms);
/**
 * 添加逻辑单元信息
 *
 * @param p_lun        逻辑单元
 * @param bsize        块大小
 * @param nblks        块数量
 * @param is_readonly  只读
 * @param is_removed   是否移除
 * @param priv         用户私有数据
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbd_ms_lun_add_info (struct usbd_ms_lun *p_lun,
                                uint32_t            bsize,
                                uint32_t            nblks,
                                usb_bool_t          is_readonly,
                                usb_bool_t          is_removed,
                                void               *priv);
/** 通过逻辑单元号获取逻辑单元*/
struct usbd_ms_lun * usbd_ms_find_lun(struct usbd_ms *p_ms, int num);
/**
 * USB设备大容量块读函数----需要用户适配
 *
 * @param p_lun   USB大容量设备逻辑分区
 * @param p_buf   读数据缓存
 * @param blk_no  读的起始块编号
 * @param nblks   块数量
 *
 * @return 成功返回读的字节数
 */
int usbd_ms_block_read (struct usbd_ms_lun *p_lun,
                        void               *p_buf,
                        int                 blk_no,
                        int                 nblks);
/** USB大容量设备块写函数----需要用户适配*/
int usbd_ms_block_write (struct usbd_ms_lun *p_lun,
                         void               *p_buf,
                         int                 blk_no,
                         int                 nblks);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __USBD_MS_H */
