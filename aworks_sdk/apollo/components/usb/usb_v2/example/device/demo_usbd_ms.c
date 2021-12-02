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
 * \brief U����ʾ���̣�USB device �������洢����ʾ��
 *
 * - �������裺
 *   1. ���������� aw_prj_params.h ͷ�ļ���ʹ��
 *      - ��Ӧ����ƽ̨��USBD��
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 *   3. �������к󽫿�����USB DEVICE0�ӿ������ʹ��USB������
 *
 * - ʵ������
 *   1. �����豸����������һ������Ϊ "AWorks" �Ŀ��ƶ��洢�豸
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

/** \brief ʹ��SD������RAM_DISKģ��U��
 * ��ʹ��SD������Ҫ��aw_prj_params.h��ʹ��SD���豸
 * ���Ұ��� SD0 ��Ҫ����SD��
 */
#define __MS_USE_TYPE   __MS_RAM_DISK
//#define __MS_USE_TYPE   __MS_SDCARD

#define __USB_DEV_NAME  DE_USB_DEV_NAME

#if __MS_USE_TYPE == __MS_SDCARD
#include "aw_sdcard.h"

#define __DISK_NAME     DE_SD_DEV_NAME
#endif

/** \brief ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
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


/** \brief �������洢��ʵ������ */
static struct usbd_ms      __g_usbd_ms;

#if __MS_USE_TYPE == __MS_RAM_DISK
/** \brief ram_diskʵ������ */
static struct aw_ram_disk     __g_ramdisk;

/** \briefram_disk�ռ��С����U�̿ռ��С�� */
static char                   __g_ram[100 * 1024];
#endif

/** �鴫����ɺ���*/
aw_local void __bd_xfer_done (struct aw_blk_arp *p_arp)
{
    (void)usb_sem_give((usb_sem_handle_t)p_arp->priv);
}

/**
 * USB�豸�������������
 *
 * @param p_lun   USB�������豸�߼�����
 * @param p_buf   �����ݻ���
 * @param blk_no  ������ʼ����
 * @param nblks   ������
 *
 * @return �ɹ����ض����ֽ���
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
 * USB�豸��������д����
 *
 * @param p_lun   USB�������豸�߼�����
 * @param p_buf   д���ݻ���
 * @param blk_no  д����ʼ����
 * @param nblks   ������
 *
 * @return �ɹ�����д���ֽ���
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
 * \brief USB device �������洢�� demo
 * \return ��
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
     * USB �豸�����ʼ��
     * ������������ awbl_hwconf_xxxxxx_usbd.h �ж���
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

    /* �洢������Ŀ�����������192�� */
    ret = aw_ram_disk_create(&__g_ramdisk,
                             disk,
                             512,                /* һ����Ĵ�С */
                             __g_ram,
                             sizeof(__g_ram),    /* �洢���ܴ�С */
                             NULL);
    if (ret != AW_OK) {
        return;
    }

    /* �� fat ��ʽ�� ram disk */
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
    /* ���һ���߼���Ԫ*/
    lun_num = usbd_ms_lun_add(&__g_usbd_ms);
    if (lun_num < 0) {
        AW_ERRF(("__g_usbd_ms_lun init failed: %d\n", ret));
        return;
    }
    /* ��ȡ���豸*/
    p_blk = aw_blk_dev_get(disk);
    if(p_blk == NULL){
        AW_ERRF(("blk dev find failed: %d\n", ret));
        return;
    }

    /* ��ȡ���豸����*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_ATTRIBUTE, &tmp);
    if (ret == -USB_ENOTSUP) {
        tmp = AW_BD_ARRT_REMOVABLE;
    } else if (ret < 0) {
        return;
    }

    /* ��ȡ���豸���С*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_BSIZE, &bsize);
    if (ret < 0) {
        return;
    }

    /* ��ȡ���豸������*/
    ret = aw_blk_dev_ioctl(p_blk, AW_BD_GET_BLKS, &nblks);
    if (ret < 0) {
        return;
    }
    ro = (tmp & AW_BD_ARRT_READ_ONLY) ? AW_TRUE : AW_FALSE;
    rm = (tmp & AW_BD_ARRT_REMOVABLE) ? AW_TRUE : AW_FALSE;

    /* ͨ���߼���Ԫ�Ż�ȡ�߼���Ԫ*/
    p_lun = usbd_ms_find_lun(&__g_usbd_ms, lun_num);
    if(p_lun == NULL){
        return;
    }
    /* ����߼���Ԫ��Ϣ*/
    usbd_ms_lun_add_info(p_lun, bsize, nblks, ro, rm, (void *)p_blk);
    /* ����USB�豸����*/
    ret = usbd_obj_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    return ;
}

/** [src_usbd_ms] */

/* end of file */
