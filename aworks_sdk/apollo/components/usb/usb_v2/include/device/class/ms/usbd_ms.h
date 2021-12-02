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

/* ��ӡ����*/
#define __USBD_MS_TRACE(info, ...) \
            do { \
                usb_printf("USBD-MS:"info, ##__VA_ARGS__); \
            } while (0)

/** \brief �������洢���������ȼ��Ͷ�ջ��С */
#define USBD_MS_TASK_PRIO    8
#define USBD_MS_STACK_SIZE   2048

/** \brief �豸�߼���Ԫ */
struct usbd_ms_lun {
    void                   *usr_priv;  /**< \brief �û�˽������*/
    int                     num;       /**< \brief �߼���Ԫ�� */
    struct usb_list_head    node;      /**< \brief �߼���Ԫ����ڵ� */
    uint32_t                sd;        /**< \brief SCSI sense key */
    uint32_t                sd_inf;    /**< \brief SCSI sense��Ϣ */
    usb_bool_t              inf_valid; /**< \brief sense info�Ƿ���Ч */
    usb_bool_t              ro;        /**< \brief �Ƿ�ֻ�� */
    usb_bool_t              rm;        /**< \brief �Ƿ���Ƴ� */
    usb_bool_t              pv;        /**< \brief prevent��־ */
    uint32_t                bsize;     /**< \brief ���С */
    uint32_t                nblks;     /**< \brief ������ */
//    usb_mutex_handle_t   lock;    //TODO �����汾��Ҫ������߳�ͨ������
    usb_sem_handle_t        blk_sync;
    usb_sem_handle_t        usb_sync;
};

/** \brief �������洢���豸 */
struct usbd_ms {
    struct usbd_obj_fun     ufun;       /**< \brief USB���ܽӿ�*/
    struct usbd_obj_pipe    in;         /**< \brief IN���ݹܵ� */
    struct usbd_obj_pipe    out;        /**< \brief OUT���ݹܵ� */

    usb_task_handle_t       ms_task;    /**< \brief �������洢������ */
    usb_sem_handle_t        semb;
    usb_mutex_handle_t      mutex;

    uint8_t                 nluns;      /**< \brief �߼���Ԫ���� */
    struct usb_list_head    luns;       /**< \brief �߼���Ԫ���� */
    void                   *p_ubuf;     /**< \brief USBD������ */
    void                   *p_bbuf;     /**< \brief LUN������ */
    uint32_t                buf_size;   /**< \brief ��������С */
    usb_bool_t              buf_auto;   /**< \brief �Ƿ�̬�����ڴ� */

    uint32_t                tag;        /**< \brief CBW/CSW��ǩ */
    uint32_t                dlen;       /**< \brief CBW���ݳ��� */
    uint32_t                cdlen;      /**< \brief CBW��Ч�ֽڳ��� */
    uint8_t                 cb[16];     /**< \brief CBW���� */
    uint8_t                 dir;        /**< \brief CBW��־ */
    uint8_t                 clen;       /**< \brief CBW����� */
    uint8_t                 sta;        /**< \brief CSW״̬ */
    struct usbd_ms_lun     *p_lun;      /**< \brief ��ǰʹ�õ��߼���Ԫ */

};


/**
 * ����һ��������USB�豸
 *
 * @param p_obj     USB�豸����
 * @param p_ms      USB�������洢�豸����
 * @param name      USB�������洢�豸����
 * @param p_buf     ����
 * @param buf_size  �����С
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_ms_create (struct usbd_obj     *p_obj,
                          struct usbd_ms      *p_ms,
                          const char          *name,
                          void                *p_buf,
                          int                  buf_size);
/** ���ٴ������洢USB�豸*/
int usbd_ms_destroy (struct usbd_ms  *p_ms);
/** ���һ���߼���Ԫ*/
int usbd_ms_lun_add (struct usbd_ms *p_ms);
/**
 * ����߼���Ԫ��Ϣ
 *
 * @param p_lun        �߼���Ԫ
 * @param bsize        ���С
 * @param nblks        ������
 * @param is_readonly  ֻ��
 * @param is_removed   �Ƿ��Ƴ�
 * @param priv         �û�˽������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_ms_lun_add_info (struct usbd_ms_lun *p_lun,
                                uint32_t            bsize,
                                uint32_t            nblks,
                                usb_bool_t          is_readonly,
                                usb_bool_t          is_removed,
                                void               *priv);
/** ͨ���߼���Ԫ�Ż�ȡ�߼���Ԫ*/
struct usbd_ms_lun * usbd_ms_find_lun(struct usbd_ms *p_ms, int num);
/**
 * USB�豸�������������----��Ҫ�û�����
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
                        int                 nblks);
/** USB�������豸��д����----��Ҫ�û�����*/
int usbd_ms_block_write (struct usbd_ms_lun *p_lun,
                         void               *p_buf,
                         int                 blk_no,
                         int                 nblks);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __USBD_MS_H */
