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
#ifndef __USBH_NET_H
#define __USBH_NET_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"
#include "host/class/cdc/usbh_cdc.h"

#define USBH_USBNET_ETHIF_NAME "usbnet-dev"

/* �����豸����*/
#define USB_NET_ETHER            1
#define USB_NET_WWAN             2

/* Ӳ����ַ��󳤶� */
#define USB_NETIF_MAX_HWADDR_LEN 6

/* �����豸��־*/
#define FLAG_NO_SETINTF        0x0010  /* �豸�������ýӿ�*/
#define FLAG_ETHER             0x0020  /* ��̫���豸 */
#define FLAG_WLAN              0x0080  /* ���߾������豸*/
#define FLAG_WWAN              0x0400  /* ���߹������豸*/
#define FLAG_LINK_INTR         0x0800  /* ��������״̬ */
#define FLAG_POINTTOPOINT      0x1000  /* ��Ե��豸*/

/* USB�����豸��ṹ��*/
struct usbh_net_lib{
    struct usb_list_head    unet_lib_dev_list;      /* ��������USB�����豸����*/
    usb_bool_t              is_lib_init;            /* �Ƿ��ʼ���豸��*/
    usb_bool_t              is_lib_uniniting;       /* �Ƿ��Ƴ��豸��*/
    usb_mutex_handle_t      unet_lib_lock;          /* ������*/
    uint8_t                 unet_lib_dev_num;       /* ��ǰ�����豸������*/
    int                     refcnt;                 /* ���ü���*/
    struct usbh_mem_record  unet_mem_record;        /* �ڴ��¼*/
};

struct usbnet;
/* USB��������������Ϣ*/
struct usbnet_drv_info {
    char *pDescription;                      /* ������Ϣ*/
    int   flags;                             /* ��־*/
    /* ��ʼ���豸*/
    usb_err_t (*bind)(struct usbnet *);
    /* ����豸 */
    void (*unbind)(struct usbnet *, struct usbh_interface *);
    /* ״̬��ѯ */
    void (*status)(struct usbnet *, struct usbh_trp *);
    /* ����ģʽ�ı�(�豸�ı��ַ�б��˲���) */
    void (*set_rx_mode)(struct usbnet *);

    struct usbh_endpoint *p_in;              /* ����˵�*/
    struct usbh_endpoint *p_out;             /* ����˵�*/
};

/* ����USB CDC ������ʩ��һЩ��׼����������(�ر��Ǹ���CDC����������
 * ʹ�ö���ӿ�)��õ�һЩ�����������**/
struct usbh_cdc_state {
    struct usb_cdc_header_desc *pHeader;   /* CDCͷ������*/
    struct usb_cdc_union_desc  *pU;        /* CDC����������*/
    struct usb_cdc_ether_desc  *pEther;    /* CDC��̫��������*/
    struct usbh_interface      *pControl;  /* ���ƽӿ�*/
    struct usbh_interface      *pData;     /* ���ݽӿ�*/
};

/* USB�����豸*/
struct usbnet{
    struct usbh_function*    p_ufun;                           /* ���USB���ܽӿ�*/
    struct usbh_interface*   p_cur_itf;                        /* ��ǰʹ�õ�USB�豸�ӿ�*/
    struct usbh_config*      p_cur_cfg;                        /* ��ǰʹ�õ�USB�豸�ĵ�ǰ����*/
    struct usbnet_drv_info*  p_drv_info;                       /* USB��������������Ϣ*/
    struct usbh_endpoint*    p_status_ep;                      /* ״̬�˵�*/
    struct usbh_endpoint*    p_in_ep;                          /* ����˵�*/
    struct usbh_endpoint*    p_out_ep;                         /* ����˵�*/
    struct usb_list_head     node;                             /* ��ǰ�豸�ڵ�*/
    usb_mutex_handle_t       lock;                             /* ������*/
    unsigned long            data[5];
    char                     ethif_name[32];                   /* ��̫���豸����*/
    uint8_t                  hwaddr[USB_NETIF_MAX_HWADDR_LEN]; /* Ӳ����ַ*/
    uint32_t                 hard_mtu;                         /* Ӳ������䵥Ԫ�������κζ����֡*/
    uint16_t                 hard_header_len;
    uint32_t                 rx_trp_size;                      /* ���մ����������С*/
    uint32_t                 maxpacket;                        /* ������С*/
    usb_sem_handle_t         bh_sem;                           /* �ź���*/
    usb_task_handle_t        bh;                               /* ������հ�����*/
    struct usbh_trp         *p_int_trp;                        /* �жϴ��������*/
    uint16_t                 rx_qlen, tx_qlen;                 /* ����/���ն��г���*/
    usb_bool_t               is_removed;                       /* �Ƿ��Ƴ�*/
};





/** ��ʼ��USB�����豸��*/
usb_err_t usbh_net_lib_init(void);
/** ����ʼ��USB�����豸��*/
usb_err_t unet_lib_uninit(void);
/** ��ȡUSB�����ڴ��¼*/
usb_err_t unet_lib_mem_record_get(struct usbh_mem_record *mem_record);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif

