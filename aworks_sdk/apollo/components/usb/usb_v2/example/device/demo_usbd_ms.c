/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief U盘演示例程（USB device 大容量存储器演示）
 *
 * - 操作步骤：
 *   1. 本例程需在 aw_prj_params.h 头文件里使能
 *      - 对应开发平台的USBD宏
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 *   3. 程序运行后将开发板USB DEVICE0接口与电脑使用USB线连接
 *
 * - 实验现象：
 *   1. 电脑设备管理器出现一个名称为 "AWorks" 的可移动存储设备
 *
 */


/** [src_usbd_ms] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "device/usbd_obj.h"
#include "device/class/ms/usbd_ms.h"
#include "fs/aw_ram_disk.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "string.h"
#include "aw_demo_config.h"

#define __MS_SDCARD     0
#define __MS_RAM_DISK   1

/** \brief 使用SD卡还是RAM_DISK模拟U盘
 * 若使用SD卡，需要在aw_prj_params.h中使能SD宏设备
 * 并且板子 SD0 需要插入SD卡
 */
#define __MS_USE_TYPE   __MS_RAM_DISK
//#define __MS_USE_TYPE   __MS_SDCARD

#define __USB_DEV_NAME  DE_USB_DEV_NAME

#if __MS_USE_TYPE == __MS_SDCARD
#include "aw_sdcard.h"

#define __DISK_NAME     DE_SD_DEV_NAME
#endif

/** \brief 定义全局usb设备实例和usb设备信息结构体 */
static struct usbd_obj __g_usbd_obj;
const static struct usbd_obj_info __g_usbd_info = {
    0,
    0,
    0,
    64,
    0x0100,
    0x3068,
    0x0003,
    "ZLG-MS",
    "AWorks-ms",
    "20200828"
};


/** \brief 大容量存储器实例定义 */
static struct usbd_ms      __g_usbd_ms;

#if __MS_USE_TYPE == __MS_RAM_DISK
/** \brief ram_disk实例定义 */
static struct aw_ram_disk     __g_ramdisk;

/** \briefram_disk空间大小（即U盘空间大小） */
static char                   __g_ram[100 * 1024];
#endif

/** 块传输完成函数*/
aw_local void __bd_xfer_done (struct aw_blk_arp *p_arp)
{
    (void)usb_sem_give((usb_sem_handle_t)p_arp->priv);
}

/**
 * USB设备大容量块读函数
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
                        int                 nblks){
    struct aw_blk_arp  arp;
    int                ret;
    uint8_t            retry = 5;
    uint_t             bsize;
    struct aw_blk_dev *p_blk = (struct aw_blk_dev *)p_lun->usr_priv;

    if((p_lun == NULL) || (p_lun->usr_priv == NULL)){
        return -AW_EINVAL;
    }


    ret = aw_blk_dev_ioctl(p_blk,
                           AW_BD_GET_BSIZE,
                           &bsize);
    if (ret != AW_OK) {
        return ret;
    }

__retry:
    usb_sem_take(p_lun->blk_sync, USB_NO_WAIT);
    arp.p_dev    = p_blk;
    arp.blk_no   = blk_no;
    arp.nbytes   = bsize * nblks;
    arp.p_data   = (void *)p_buf;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = AW_BD_ARQ_READ;
    arp.priv     = p_lun->blk_sync;
    arp.pfn_done = __bd_xfer_done;
    arp.p_next   = NULL;

    ret = aw_blk_dev_areq(p_blk, &arp);
    usb_sem_take(p_lun->blk_sync, USB_WAIT_FOREVER);

    if ((ret == AW_OK) && (arp.error != AW_OK)) {
        ret = arp.error;
    }

    if (ret != AW_OK) {
        AW_ERRF(("block device transfer failed:%d, retry %d\r\n", ret, retry));
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    if (ret == AW_OK) {
        return (arp.nbytes - arp.resid) / p_lun->bsize;;
    }
    return ret;
}

/**
 * USB设备大容量块写函数
 *
 * @param p_lun   USB大容量设备逻辑分区
 * @param p_buf   写数据缓存
 * @param blk_no  写的起始块编号
 * @param nblks   块数量
 *
 * @return 成功返回写的字节数
 */
int usbd_ms_block_write (struct usbd_ms_lun *p_lun,
                         void               *p_buf,
                         int                 blk_no,
                         int                 nblks){
    struct aw_blk_arp  arp;
    int                ret;
    uint8_t            retry = 5;
    uint_t             bsize;
    struct aw_blk_dev *p_blk = (struct aw_blk_dev *)p_lun->usr_priv;

    if((p_lun == NULL) || (p_lun->usr_priv == NULL)){
        return -AW_EINVAL;
    }


    ret = aw_blk_dev_ioctl(p_blk,
                           AW_BD_GET_BSIZE,
                           &bsize);
    if (ret != AW_OK) {
        return ret;
    }

__retry:
    usb_sem_take(p_lun->blk_sync, USB_NO_WAIT);
    arp.p_dev    = p_blk;
    arp.blk_no   = blk_no;
    arp.nbytes   = bsize * nblks;
    arp.p_data   = (void *)p_buf;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = AW_BD_ARQ_WRITE;
    arp.priv     = p_lun->blk_sync;
    arp.pfn_done = __bd_xfer_done;
    arp.p_next   = NULL;

    ret = aw_blk_dev_areq(p_blk, &arp);
    usb_sem_take(p_lun->blk_sync, USB_WAIT_FOREVER);

    if ((ret == AW_OK) && (arp.error != AW_OK)) {
        ret = arp.error;
    }

    if (ret != AW_OK) {
        AW_ERRF(("block device transfer failed:%d, retry %d\r\n", ret, retry));
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    if (ret == AW_OK) {
        return (arp.nbytes - arp.resid) / p_lun->bsize;;
    }
    return ret;
}

/**
 * \brief USB device 大容量存储器 demo
 * \return 无
 */
void demo_usbd_ms_entry (void)
{
    int         ret, tmp;
    const char *disk;
    struct aw_blk_dev *p_blk = NULL;
    uint32_t  bsize;
    uint32_t  nblks;
    aw_bool_t rm;
    aw_bool_t ro;
    int lun_num = 0;
    struct usbd_ms_lun *p_lun = NULL;
#if __MS_USE_TYPE == __MS_SDCARD
    disk = __DISK_NAME;
    aw_blk_dev_wait_install(disk, AW_WAIT_FOREVER);
#endif

    /*
     * USB 设备对象初始化
     * 控制器名字在 awbl_hwconf_xxxxxx_usbd.h 中定义
     */
    ret = usbd_obj_init(&__g_usbd_obj,
                        &__g_usbd_info,
                        NULL,
                        DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return;
    }

#if __MS_USE_TYPE == __MS_RAM_DISK
    disk = __USB_DEV_NAME;

    /* 存储器分配的块数不能少于192个 */
    ret = aw_ram_disk_create(&__g_ramdisk,
                             disk,
                             512,                /* 一个块的大小 */
                             __g_ram,
                             sizeof(__g_ram),    /* 存储器总大小 */
                             NULL);
    if (ret != AW_OK) {
        return;
    }

    /* 用 fat 格式化 ram disk */
    ret = aw_make_fs(disk, "vfat", NULL);
    if (ret != AW_OK) {
        AW_ERRF(("disk make_fs failed: %d\n", ret));
        return;
    }

#endif

    ret = usbd_ms_create(&__g_usbd_obj,
                         &__g_usbd_ms,
                         "UDISK",
                         NULL,
                         20 * 1024);
    if (ret != AW_OK) {
        AW_ERRF(("aw_usbd_ms_create failed: %d\n", ret));
        return;
    }
    /* 添加一个逻辑单元*/
    lun_num = usbd_ms_lun_add(&__g_usbd_ms);
    if (lun_num < 0) {
        AW_ERRF(("__g_usbd_ms_lun init failed: %d\n", ret));
        return;
    }
    /* 获取块设备*/
    p_blk = aw_blk_dev_get(disk);
    if(p_blk == NULL){
        AW_ERRF(("blk dev find failed: %d\n", ret));
        return;
    }

    /* 获取块设备属性*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_ATTRIBUTE, &tmp);
    if (ret == -USB_ENOTSUP) {
        tmp = AW_BD_ARRT_REMOVABLE;
    } else if (ret < 0) {
        return;
    }

    /* 获取块设备块大小*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_BSIZE, &bsize);
    if (ret < 0) {
        return;
    }

    /* 获取块设备块数量*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_BLKS, &nblks);
    if (ret < 0) {
        return;
    }
    ro = (tmp & AW_BD_ARRT_READ_ONLY) ? AW_TRUE : AW_FALSE;
    rm = (tmp & AW_BD_ARRT_REMOVABLE) ? AW_TRUE : AW_FALSE;

    /* 通过逻辑单元号获取逻辑单元*/
    p_lun = usbd_ms_find_lun(&__g_usbd_ms, lun_num);
    if(p_lun == NULL){
        return;
    }
    /* 添加逻辑单元信息*/
    usbd_ms_lun_add_info(p_lun, bsize, nblks, ro, rm, (void *)p_blk);
    /* 启动USB设备对象*/
    ret = usbd_obj_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    return ;
}

/** [src_usbd_ms] */

/* end of file */
