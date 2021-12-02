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


#ifndef __USBD_CDC_VIRTUAL_SERIAL_H
#define __USBD_CDC_VIRTUAL_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"
#include "rngbuf/usb_rngbuf.h"
#include "device/core/usbd.h"
#include "device/core/usbd_obj.h"
#include <string.h>
#include <stdio.h>

/* ��ӡ����*/
#define __USBD_CDC_VS_TRACE(info, ...) \
            do { \
                usb_printf("USBD-VS:"info, ##__VA_ARGS__); \
            } while (0)

#define USBD_CDC_SERIAL_TASK_SIZE         1024     /* ���ݽ��������ջ */
//#define USBD_CDC_PASS_BACK                     /* �Ƿ���CDC�ش�--�������� */

/* ����/���ͻ����С*/
#define USBD_CDC_RX_RNGBUF_SIZE      (1024)
#define USBD_CDC_TX_RNGBUF_SIZE      (1024)

/* CDC���ݽ���/���ͽ����������ֽڴ�С */
#define USBD_CDC_RX_SWAP_SIZE        (1024)
#define USBD_CDC_TX_SWAP_SIZE        (1024)

enum{
    USBD_CDC_VS_TRANFER_ERROR = 0,
    USBD_CDC_VS_TRANFER_SYNC,
    USBD_CDC_VS_TRANFER_ASYNC
};

/* cdc���ڲ����ṹ�� line_coding */
typedef struct usbd_cdc_line_coding {
  uint32_t  baud_rate;
  uint8_t   stop_bits;
  uint8_t   parity;
  uint8_t   word_length;
} __attribute__ ((packed)) usbd_cdc_line_coding_t;

/* USB�豸CDC ���⴮�ڽṹ�� */
typedef struct usbd_cdc_vs {
    struct usbd_obj_fun          cfun;          /* ����ӿ�*/
    struct usbd_obj_pipe         intr;          /* �жϹܵ�*/
    struct usbd_obj_fun          dfun;          /* ���ݽӿ�*/
    struct usbd_obj_pipe         in;            /* ����ܵ�*/
    struct usbd_obj_pipe         out;           /* ����ܵ�*/
    uint8_t                      id;            /* �豸id��� */
    char                         name[32];      /* ���⴮������*/
    usbd_cdc_line_coding_t       line_control;  /* ��������*/
    usb_mutex_handle_t           mutex;         /* �豸������*/
    struct usb_list_head         node;          /* �豸�ڵ� */
    char                        *p_rbuf;        /* ���ջ���*/
    usb_sem_handle_t             sync_semb;     /* �ź���*/
    usb_task_handle_t            rx_task;       /* ��������*/
    struct usb_rngbuf            r_rngbuf;      /* ���λ�����*/
    uint8_t                      tx_mode;       /* ����ģʽ*/
    uint8_t                      rx_mode;       /* ����ģʽ*/

    int                        (*pfn_rx_callback)(void *p_buf, int len, void *p_arg);
    void                        *p_rxcb_arg;
}usbd_cdc_vs_t;


/**
 * �������⴮�ڽ���ģʽ
 *
 * @param p_vs USB���⴮�ڽṹ��
 * @param mode ����ģʽ(ͬ�����첽)
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_cdc_vs_set_rxmode(struct usbd_cdc_vs *p_vs, uint8_t mode);
/**
 * �������⴮�ڷ���ģʽ
 *
 * @param p_vs USB���⴮�ڽṹ��
 * @param mode ����ģʽ(ͬ�����첽)
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_cdc_vs_set_txmode(struct usbd_cdc_vs *p_vs, uint8_t mode);
/** ������ģʽ�Ƿ����첽ģʽ*/
usb_bool_t usbd_cdc_vs_rx_is_async(struct usbd_cdc_vs *p_vs);
/** ��鷢��ģʽ�Ƿ����첽ģʽ*/
usb_bool_t usbd_cdc_vs_tx_is_async(struct usbd_cdc_vs *p_vs);
/**
 * ����һ�����⴮��
 *
 * @param p_obj    USB�豸ʵ��
 * @param p_vs     USB���⴮�ڽṹ��
 * @param id       ���⴮��ID
 * @param p_name   ���⴮������
 * @param buf_size �����С
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_cdc_vs_create (struct usbd_obj    *p_obj,
                              struct usbd_cdc_vs *p_vs,
                              uint8_t             id,
                              const char         *p_name,
                              int                 buf_size);
/** �������⴮���豸*/
usb_err_t usbd_cdc_vs_destroy (struct usbd_cdc_vs *p_vs);
/**
 * USB�豸�첽д����
 *
 * @param p_vs    USB�豸���⴮�ڽṹ��
 * @param p_trans USB�豸����ṹ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_cdc_vs_write_async (struct usbd_cdc_vs     *p_vs,
                                   struct usbd_obj_trans  *p_trans);
/** �첽�� */
usb_err_t usbd_cdc_vs_read_async (struct usbd_cdc_vs  *p_vs,
                                  void                *p_buf,
                                  size_t               nbytes,
                                  size_t              *p_act_len);
/**
 * USB�豸ͬ��д����
 *
 * @param p_vs     USB�豸���⴮�ڽṹ��
 * @param p_buffer Ҫ���͵����ݻ���
 * @param nbytes   Ҫ���͵����ݳ���
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ�����ʵ�ʴ�����ֽ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_cdc_vs_write_sync (struct usbd_cdc_vs *p_vs,
                                  void               *p_buffer,
                                  size_t              nbytes,
                                  int                 timeout);
/**
 * USB�豸ͬ��������
 *
 * @param p_vs    USB�豸���⴮�ڽṹ��
 * @param p_buf   Ҫ���յ����ݻ���
 * @param nbytes  Ҫ���յ����ݳ���
 * @param timeout ��ʱʱ��
 *
 * @return ����ɹ�����ʵ�ʴ�����ֽ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_cdc_vs_read_sync (struct usbd_cdc_vs  *p_vs,
                                 void                *p_buf,
                                 size_t               nbytes,
                                 int                  timeout);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */

