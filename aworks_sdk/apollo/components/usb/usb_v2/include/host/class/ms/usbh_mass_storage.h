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
#ifndef __USBH_MASS_STORAGE_H
#define __USBH_MASS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "usb_adapter_inter.h"
#include "refcnt/usb_refcnt.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

/* 大容量存储设备接口协议代码*/
#define USBH_MS_PR_CBI               0x00    /* 控制/批量/中断*/
#define USBH_MS_PR_CB                0x01    /* 控制/批量*/
#define USBH_MS_PR_BULK              0x50    /* 批量*/
#define USBH_MS_PR_ANY               0xFF    /* 不关心协议*/

/* 大容量存储设备设备类型*/
#define USBH_MS_SC_RBC               0x01    /* flash 设备*/
#define USBH_MS_SC_8020              0x02    /* CD's DVD's*/
#define USBH_MS_SC_QIC               0x03    /* 磁带*/
#define USBH_MS_SC_UFI               0x04    /* Floppy*/
#define USBH_MS_SC_8070              0x05    /* Floppies(mostly)*/
#define USBH_MS_SC_SCSI              0x06    /* SCSI 设备*/
#define USBH_MS_SC_RAMD              0x13    /* Ram 磁盘设备*/


/* 大容量存储设备库结构体*/
struct ums_lib{
    struct usb_list_head    ums_lib_dev_list;       /* 管理所有设备链表*/
    usb_bool_t              is_lib_init;            /* 是否初始化库*/
    usb_bool_t              is_lib_uniniting;       /* 是否移除库*/
    usb_mutex_handle_t      ums_lib_lock;           /* 互斥锁*/
    uint8_t                 ums_lib_dev_num;        /* 当前存在设备的数量*/
    int                     refcnt;                 /* 引用计数*/
    struct usbh_mem_record  ums_mem_record;         /* 内存记录*/
};

/** 大容量存储设备逻辑单元结构体 */
struct usbh_ms_lun {
    struct usbh_mass_storage   *p_ms;      /* 相关的大容量存储设备*/
    char                        name[32];  /* 逻辑单元名字*/
    uint8_t                     lun;       /* 逻辑单元号*/
    uint32_t                    nblks;     /* 块数量*/
    uint32_t                    bsize;     /* 块大小*/
    usb_bool_t                  wp;        /* 是否有写保护*/
    void                       *p_buf;     /* 数据缓存*/
    usb_bool_t                  is_init;   /* 是否初始化成功*/
    usb_bool_t                  is_used;   /* 是否被使用*/
    void                       *user_priv; /* 用户私有数据*/
};

struct usbh_ms_sclass {
    uint8_t    id;


    usb_err_t (*pfn_init) (struct usbh_ms_lun *p_lun);

    usb_err_t (*pfn_read) (struct usbh_ms_lun *p_lun,
                           uint32_t            blk,
                           uint32_t            nblks,
                           void               *p_buf);

    usb_err_t (*pfn_write) (struct usbh_ms_lun *p_lun,
                            uint32_t            blk,
                            uint32_t            nblks,
                            void               *p_data);

};

/** USB主机大容量存储设备*/
struct usbh_mass_storage {
    struct usbh_function   *p_fun;            /* 相关的功能接口*/
    usb_mutex_handle_t      lock;
    struct usbh_endpoint   *p_in;
    struct usbh_endpoint   *p_out;
    struct usbh_endpoint   *p_intr;           /* 中断端点*/

    struct usbh_ms_sclass  *p_sclass;         /* 大容量存储设备类别*/

    int                     id;               /* 大容量存储设备ID*/
    uint8_t                 nluns;            /* 逻辑分区数量*/
    int                     ref;              /* 引用计数*/
    uint8_t                 pro;              /* 接口协议*/
    int                     tag;
    struct usb_list_head    node;             /* 大容量存储设备节点*/
    struct usbh_ms_lun     *luns;             /* 逻辑分区*/
    usb_bool_t              removed;          /* 移除标志*/
    void                   *p_cbw;            /* 命令块包*/
    void                   *p_csw;            /* 命令状态包*/
};

/*******************************************函数声明**********************************************/
/** 初始化大容量存储设备库*/
usb_err_t usbh_ms_lib_init(void);
/** 获取当前存在的大容量存储设备数量*/
int usbh_ms_dev_num_get(void);
/**
 * 根据PID和VID获取大容量存储设备
 *
 * @param pid 指定PID,若为USBDEV_IGONRE_PID则忽略匹配pid
 * @param vid 指定VID,若为USBDEV_IGONRE_VID则忽略匹配vid
 * @param idx 索引，指明要获取第几个大容量存储设备(最小为1)
 *
 * @return 成功返回大容量存储设备结构体
 */
struct usbh_mass_storage *usbh_ms_dev_open(uint16_t pid, uint16_t vid, uint8_t idx);
/** 释放USB大容量存储设备*/
usb_err_t usbh_ms_dev_close(struct usbh_mass_storage *p_ms);
/** 从逻辑分区获取大容量存储设备*/
struct usbh_mass_storage *usbh_ms_dev_get_from_lun(struct usbh_ms_lun *p_lun);

/** 检查大容量存储设备是否已经移除*/
usb_bool_t usbh_ms_dev_removed_check(struct usbh_mass_storage *p_ms);
/**
 * 获取可用的逻辑单元
 *
 * @param p_ms     大容量存储设备
 * @param luns_num 逻辑单元号
 *
 * @return 成功返回逻辑单元结构体
 */
struct usbh_ms_lun *usbh_ms_valid_lun_get(struct usbh_mass_storage *p_ms, uint8_t luns_num);
/**
 * 释放可用的逻辑单元
 *
 * @param p_lun 要释放的逻辑单元
 *
 * @return 成功返回USB_OK
 */
usb_err_t usbh_ms_valid_lun_put(struct usbh_ms_lun *p_lun);
/** 获取最大逻辑单元数量*/
usb_err_t usbh_ms_max_lun_get(struct usbh_mass_storage *p_ms, uint8_t *nluns);
/** 获取设备当前的逻辑单元数量*/
int usbh_ms_lun_num_get(struct usbh_mass_storage *p_ms);

/** 获取分区块大小*/
uint32_t usbh_ms_lun_bsize_get(struct usbh_ms_lun *p_lun);
/** 获取分区块数量*/
uint32_t usbh_ms_lun_nblks_get(struct usbh_ms_lun *p_lun);

/** 设置分区名字*/
usb_err_t usbh_ms_lun_name_set(struct usbh_ms_lun *p_lun, char *name_new);
/** 获取分区名字*/
char *usbh_ms_lun_name_get(struct usbh_ms_lun *p_lun);

/** 检查分区是否写保护*/
usb_bool_t usbh_ms_lun_is_wp(struct usbh_ms_lun *p_lun);

/** 获取分区缓冲数据*/
usb_err_t usbh_ms_lun_databuf_get(struct usbh_ms_lun *p_lun, void *dest_buf, uint32_t size);
/** 分区缓冲写数据*/
usb_err_t usbh_ms_lun_databuf_set(struct usbh_ms_lun *p_lun, void *src_buf, uint32_t size);

/** 设置用户私有数据*/
usb_err_t usbh_ms_lun_usrdata_set(struct usbh_ms_lun *p_lun, void *usr_priv);
/** 获取用户私有数据*/
void* usbh_ms_lun_usrdata_get(struct usbh_ms_lun *p_lun);


/**
 * 创建一个大容量存储设备
 *
 * @param p_ufun       相关的USB功能接口
 * @param lun_buf_size U盘数据交互缓存，影响U盘读写性能，推荐16k
 *
 * @return 成功返回大容量存储设备结构体
 */
struct usbh_mass_storage *usbh_ms_dev_create(struct usbh_function *p_ufun,
                                             uint32_t              lun_buf_size);
/** 大容量存储设备销毁*/
usb_err_t usbh_ms_dev_destory(struct usbh_mass_storage *p_ms);

/**
 * 大容量存储设备块读函数
 *
 * @param p_lun   逻辑分区结构体
 * @param blk_no  起始块编号
 * @param nblks   块数量
 * @param p_buf   数据缓存(为NULL则用逻辑分区的缓存)
 *
 * @return 成功返回实际读写成功的字节数
 */
int usbh_ms_blk_read(struct usbh_ms_lun *p_lun,
                     uint32_t            blk_no,
                     uint32_t            nblks,
                     void               *p_buf);
/**
 * 大容量存储设备块写函数
 *
 * @param p_lun   逻辑分区结构体
 * @param blk_no  起始块编号
 * @param nblks   块数量
 * @param p_buf   数据缓存(为NULL则用逻辑分区的缓存)
 *
 * @return 成功返回实际读写成功的字节数
 */
int usbh_ms_blk_write(struct usbh_ms_lun *p_lun,
                      uint32_t            blk_no,
                      uint32_t            nblks,
                      void               *p_buf);

/** 获取USB大容量存储驱动内存记录*/
usb_err_t ums_lib_mem_record_get(struct usbh_mem_record *mem_record);
/**************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

