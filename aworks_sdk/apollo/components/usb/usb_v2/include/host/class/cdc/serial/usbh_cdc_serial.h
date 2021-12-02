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
#ifndef __USBH_CDC_SERIAL_H
#define __USBH_CDC_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "rngbuf/usb_rngbuf.h"

#define USB_SERIAL_DEFAULT_INTF -1

#define USB_SERIAL_PIPE_IN    0
#define USB_SERIAL_PIPE_OUT   1
#define USB_SERIAL_PIPE_BOTH  2

/* USBת����ʹ�õ�ֹͣλ��ȡֵ*/
#define USB_SERIAL_ONESTOPBIT     (0)    /** \brief 1λֹͣλ  */
#define USB_SERIAL_ONE5STOPTS     (1)    /** \brief 1.5 λֹͣλ  */
#define USB_SERIAL_TWOSTOPBITS    (2)    /** \brief 2   λֹͣλ  */
#define USB_SERIAL_STOP_INVALID   (0x3)  /** \brief 3����Чֹͣλ  */

/* USBת����У�鷽��ȡֵ*/
#define USB_SERIAL_EVENPARITY     (0)    /** \breif żЧ��  */
#define USB_SERIAL_ODDPARITY      (1)    /** \breif ��Ч��  */

/* USBת��������λ*/
#define USB_SERIAL_CSIZE           0xc     /* bits 3 and 4 encode the character size */
#define USB_SERIAL_CS5             0x5     /* 5 bits */
#define USB_SERIAL_CS6             0x6     /* 6 bits */
#define USB_SERIAL_CS7             0x7     /* 7 bits */
#define USB_SERIAL_CS8             0x8     /* 8 bits */

/* USBת�����豸��ṹ��*/
struct usbh_serial_lib{
    struct usb_list_head    userial_lib_dev_list;   /* ��������USBת�����豸����*/
    usb_bool_t              is_lib_init;            /* �Ƿ��ʼ���豸��*/
    usb_bool_t              is_lib_uniniting;       /* �Ƿ��Ƴ��豸��*/
    usb_mutex_handle_t      userial_lib_lock;       /* ������*/
    uint8_t                 userial_lib_dev_num;    /* ��ǰ�����豸������*/
    int                     refcnt;                 /* ���ü���*/
    struct usbh_mem_record  userial_mem_record;     /* �ڴ��¼*/
};

struct usbh_serial;
/* USBת�����豸��Ϣ�ṹ��*/
struct usbh_serial_dev_info{
    uint16_t    pid;                           /* �豸pid*/
    uint16_t    vid;                           /* �豸vid*/
    char       *type;                          /* �豸����*/
    usb_err_t (*init)(struct usbh_serial *);   /* ��ʼ������*/
    usb_err_t (*uninit)(struct usbh_serial *); /* ����ʼ������*/
};

/* USB�������ýṹ����*/
struct usb_serial_dcb {
    uint32_t                baud_rate;           /* ������*/

    uint32_t                byte_size:4;         /* ָ����ǰ�˿�ʹ�õ�����λ(��Χ5~8)*/
    uint32_t                f_parity :1;         /* �Ƿ�������żУ��,Ϊ0������У�飬Ϊ1��У�鷽ʽ��parity��ֵ*/
    uint32_t                parity   :1;         /* ָ���˿����ݴ����У�鷽��*/
    uint32_t                stop_bits:2;         /* ָ���˿ڵ�ǰʹ�õ�ֹͣλ��*/
    uint32_t                      dtr:1;         /* �����ն�׼��*/
    uint32_t                      rts:1;         /* ������*/
    uint32_t                  crtscts:1;         /* ������*/
    uint32_t                    ixoff:1;
    uint32_t                f_dummy:24;
};

/* USB���ڹܵ��ṹ��*/
struct usbh_serial_pipe {
    struct usbh_endpoint *pipe;
    void                 *p_buf;
    uint32_t              size;        /* һ�δ�������ݴ�С*/
    uint32_t              timeout;     /* ����/���ճ�ʱʱ��*/
    usb_mutex_handle_t    lock;        /* USB�ܵ�������*/
};

/* USB���ڽṹ��*/
struct usbh_serial {
    char                       name[32];     /* USB�����豸����*/
    struct usbh_function      *p_fun;        /* ��Ӧ�Ĺ����豸�ṹ��*/
    uint8_t                    nports;       /* �˿�����*/
    void                      *p_drv_handle; /* ˽������(�����оƬ���͵Ľṹ��)*/
    struct usbh_serial_pipe    p_in;         /* ����ܵ�*/
    struct usbh_serial_pipe    p_out;        /* ����ܵ�*/
    struct usbh_endpoint      *p_intr;       /* �ж϶˵�*/
    struct usb_list_head       node;         /* ��ǰUSB���ڽڵ�*/
    usb_mutex_handle_t         lock;         /* USB���ڻ�����*/
    int                        ref;          /* USB�������ü���*/
    struct usb_serial_dcb      cur_dcb;      /* ��ǰ�����ݿ��ƿ�*/
    usb_bool_t                 removed;      /* �Ƴ�״̬*/
    usb_bool_t                 is_init;      /* �Ƿ��Ѿ���ʼ�����*/

    struct usbh_trp           *rx_async_trp; /* �첽���������*/
    void                      *rx_async_buf; /* �첽���ջ���*/
    /* �첽���ջص�����*/
    void                     (*rx_async_cb)(void *buf, int len);

    /* ͬ�����ص�����*/
    int (*read_sync) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* ͬ�����ص�����*/
    int (*read_async) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* ͬ��д�ص�����*/
    int (*write_sync) (struct usbh_serial *usb_serial, void *p_buf, int len);
    /* ���ô��ڻص�����*/
    usb_err_t (*dcb_set) (struct usbh_serial *usb_serial, struct usb_serial_dcb *p_dcb);
    /* ���ص�����*/
    usb_err_t (*intr) (struct usbh_serial *usb_serial, int len);
};

/***************************************��������**************************************************/
/** ��ȡUSBת���������ڴ��¼*/
usb_err_t userial_lib_mem_record_get(struct usbh_mem_record *mem_record);
/** USBת�����豸����س�ʼ��*/
usb_err_t usbh_serial_lib_init (void);
/** ����ʼ��USBת�����豸��*/
usb_err_t usbh_serial_lib_uninit(void);
/** ��ȡ��ǰ���ڵ�USBת�����豸����*/
int usbh_serial_dev_num_get(void);
/** ��ȡUSBת�����豸�Ķ˿�����*/
int usbh_serial_dev_nport_get(struct usbh_serial *p_userial);
/** ����һ��USBת�����豸*/
struct usbh_serial *usbh_serial_dev_create(struct usbh_function *p_ufun);
/** ����USBת�����豸*/
usb_err_t usbh_serial_dev_destroy(struct usbh_serial *p_userial);
/**
 * �򿪶�Ӧ��USB����
 *
 * @param pid     USBת���豸�ĳ�ƷID
 * @param vid     USBת���豸�Ĺ�Ӧ��ID
 * @param intfnum ָ���ض��Ľӿ�(����Ҫ������Ϊ-1)
 * @param idx ������ָ��Ҫ��ȡ�ڼ���USBת�����豸(��СΪ1)
 *
 * @return �ɹ������ҵ���USB���ڽṹ��
 */
struct usbh_serial *usbh_serial_dev_open(int pid, int vid, int intfnum, uint8_t idx);
/** �ر�USBת�����豸*/
void usbh_serial_dev_close (struct usbh_serial *p_userial);
/**
 * ����USBת�������ݿ��ƿ�
 *
 * @param p_userial USBת�����豸
 * @param p_dcb     �������ݿ��ƿ�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_dcb_set (struct usbh_serial *p_userial, struct usb_serial_dcb *p_dcb);
/**
 * ��ȡUSBת�����豸���ݿ��ƿ�
 *
 * @param p_userial USBת�����豸
 *
 * @return �ɹ�����USBת�������ýṹ��
 */
struct usb_serial_dcb *usbh_serial_dev_dcb_get (struct usbh_serial *p_userial);
/**
 * USBת�����豸��������(ͬ��)
 *
 * @param p_userial   USBת�����豸
 * @param p_buffer    Ҫд�����ݵĻ���
 * @param nbytes      Ҫд�����ݵĳ���
 *
 * @return �ɹ����ط��͵����ݵĳ���
 */
int usbh_serial_dev_send (struct usbh_serial *p_userial,
                          void               *p_buffer,
                          size_t              nbytes);
/**
 * USBת�����豸������(ͬ��)
 *
 * @param p_userial   USBת�����豸
 * @param p_buffer    ���ص����ݵĻ���
 * @param nbytes      ���ص����ݵĳ���
 *
 * @return �ɹ����ض��ص����ݵĳ���
 */
int usbh_serial_dev_recieve_sync (struct usbh_serial *p_userial,
                                  void               *p_buffer,
                                  size_t              nbytes);
/**
 * USBת�����豸������(�첽)
 *
 * @param p_userial   USBת�����豸
 * @param p_buffer    ���ص����ݵĻ���
 * @param nbytes      ���ص����ݵĳ���
 *
 * @return �ɹ����ض��ص����ݵĳ���
 */
int usbh_serial_dev_recieve_async (struct usbh_serial *p_userial,
                                   void               *p_buffer,
                                   size_t              nbytes);
/**
 * USBת�����豸ֹͣ��������
 *
 * @param p_userial   USBת�����豸
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_stop(struct usbh_serial *p_userial);
/**
 * ��ȡUSBת���ڹܵ����δ����С
 *
 * @param usb_serial   USB���ڽṹ��
 * @param pipe_dir     �ܵ�����
 * @param tranfer_size Ҫ���صĴ����С
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_tranfersize_get(struct usbh_serial *p_userial,
                                          uint8_t             pipe_dir,
                                          uint32_t           *tranfer_size);
/**
 * ����USB���ڵ��δ����С
 *
 * @param usb_serial   USB���ڽṹ��
 * @param pipe_dir     �ܵ�����
 * @param tranfer_size �����С
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_tranfersize_set(struct usbh_serial *p_userial,
                                          uint8_t             pipe_dir,
                                          uint32_t            tranfer_size);
/**
 * ����USBת�����豸���ó�ʱ����
 *
 * @param p_userial USBת�����豸
 * @param pipe_dir  �ܵ�����
 * @param timeout   Ҫ���õ�ʱ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_timeout_set (struct usbh_serial *p_userial,
                                       uint8_t             pipe_dir,
                                       uint32_t            timeout);
/**
 * ��ȡUSBת�����豸��ȡ��ʱ����
 *
 * @param p_userial   USBת�����豸
 * @param pipe_dir    �ܵ�����
 * @param timeout     Ҫ��ȡ��ʱ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_timeout_get (struct usbh_serial *p_userial,
                                       uint8_t             pipe_dir,
                                       uint32_t           *timeout);
/**
 * �����첽���ջص�����
 *
 * @param p_userial   USBת�����豸
 * @param rx_async_cb Ҫ���õĻص�����
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_serial_dev_rxcb_set(struct usbh_serial *p_userial,
                                   void (*rx_async_cb)(void *buf, int len));
/**********************************************************************************************/
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

