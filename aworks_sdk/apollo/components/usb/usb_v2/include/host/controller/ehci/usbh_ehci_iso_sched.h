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

#ifndef __USBH_EHCI_ISO_SCHED_H
#define __USBH_EHCI_ISO_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

#define NO_FRAME    29999           /* ��δ����֡*/

/* USB ISO��ӡ����*/
#define __USB_ISO_TRACE(info, ...) \
            do { \
                usb_printf("USBH-ISO:"info, ##__VA_ARGS__); \
            } while (0)

/* ͨ�ô�����䳣��/֧��*/
#define BitTime(bytecount) (7 * 8 * bytecount / 6)
#define NS_TO_US(ns)    USB_DIV_ROUND_UP(ns, 1000L)
#define USB2_HOST_DELAY 5
#define HS_NSECS(bytes) ( ((55 * 8 * 2083)/1000) \
    + ((2083UL * (3167 + BitTime (bytes)))/1000) \
    + USB2_HOST_DELAY)
#define HS_NSECS_ISO(bytes) (((38 * 8 * 2083) \
    + (2083UL * (3 + BitTime(bytes))))/1000 \
    + USB2_HOST_DELAY)
#define HS_USECS(bytes) NS_TO_US (HS_NSECS(bytes))
#define HS_USECS_ISO(bytes) NS_TO_US(HS_NSECS_ISO(bytes))

/* ���ٶ˵��������б���ĸߴ���˷���*/
#define hb_mult(wMaxPacketSize) (1 + (((wMaxPacketSize) >> 11) & 0x03))
/* ...�Ͱ���С�������κ����͵Ķ˵�������*/
#define max_packet(wMaxPacketSize) ((wMaxPacketSize) & 0x07ff)

/* Ϊ�ָ�����ĸ����豸��ȫ/�����豸�����ڴ�����Ⱥ�Ԥ����Ϣ*/
struct ehci_per_sched {
    struct usbh_device    *udev;           /* ��������ת����*/
    struct usbh_endpoint  *ep;

    uint16_t               cs_mask;         /* C-mask�� S-mask �ֽ�*/
    uint16_t               phase;           /* ��ʵʱ�ڣ�֡����*/
    uint8_t                bw_phase;        /* һ����Ϊ������*/
    uint8_t                phase_uf;        /* uframe part of the phase */
    uint8_t                usecs, c_usecs;  /* ���������ϵ�ʱ��*/
    uint8_t                bw_uperiod;      /* ����Ԥ����΢֡����*/
    uint16_t               period;          /* ֡����ʵ����*/
    uint8_t                bw_period;       /* ����֡����*/
};

/* һ����ʱ���������������(����Ǹ������3KB)*/
struct ehci_iso_packet {
    /* These will be copied to iTD when scheduling */
    uint64_t         bufp;               /* itd->hw_bufp{,_hi}[pg] |= */
    uint32_t         transaction;        /* itd->hw_transaction[i] |= */
    uint8_t          cross;              /* �����ҳ*/
    /* ����ȫ������ָ�*/
    uint32_t         buf1;
};

/* ���Ե�ʱUSB���������ݰ���ʱ��������(�����ٶ�)��ÿ�����ݰ����豸��
 * һ���߼�USB����(���Ƿָ�����)����stream->next_uframe ��ʼ*/
struct ehci_iso_sched {
    struct usb_list_head    td_list;         /* ��������������*/
    unsigned                span;            /* ��ε�����֡�����е�֡/΢֡���*/
    unsigned                first_packet;
    struct ehci_iso_packet *packet;
};

/* EHCI��ʱ���ṹ��*/
struct ehci_iso_stream {
    struct usbh_ehci_qh  *hw;
    struct usbh_device   *udev;              /* ��������ת����*/
    struct usb_list_head  td_list;           /* ����еĵ�ʱ����������/�ָ������ʱ����������*/
    struct usb_list_head  free_list;         /* ���еĵ�ʱ����������/�ָ������ʱ����������*/
    uint32_t              next_uframe;

    usb_bool_t            release;           /* �Ƿ��ͷ�*/
    uint8_t               bEndpointAddress;  /* �˵��ַ*/
    uint8_t               highspeed;         /* ���ٴ���*/
    uint16_t              uperiod;           /* ΢֡����*/
    uint32_t              bandwidth;         /* ����*/
    uint16_t              maxp;              /* ������С*/
    uint32_t              splits;            /* �ָ������־*/
    int                   refcount;          /* 2.6*/
    struct usbh_endpoint *ep;                /* 2.6*/
    uint8_t               usecs, c_usecs;    /* 2.6*/
    uint16_t              cs_mask;           /* 2.6*/
    uint8_t               interval;          /* 2.6 ΢֡����*/
    uint8_t               f_interval;        /* 2.6 ֡����*/
    uint16_t              depth;             /* 2.6*/
    uint16_t              tt_usecs;          /* 2.6 ��ȫ��/���������ϵ�ʱ��*/

    struct ehci_per_sched ps;                /* ������Ϣ 4.x.y*/

    /* �������ڳ�ʼ����ʱ������������ hw_bufp �ֶ�*/
    uint32_t              buf0;
    uint32_t              buf1;
    uint32_t              buf2;

    /* ���ڳ�ʼ���ָ��ʱ����������������ת������Ϣ*/
    uint32_t              address;
};

/***************************************��������**************************************************/
/** �ͷŵ�ʱ������*/
usb_err_t iso_stream_release(struct usbh_ehci *p_ehci, struct ehci_iso_stream *stream);
/** ɨ������֡�б�*/
void scan_periodic(struct usbh_ehci *p_ehci);
/**
 * ��ȡ��ʱ������
 *
 * @param p_ehci EHCI�ṹ��
 * @param p_trp  ��صĵ�ʱ���������
 *
 * @return �ɹ����س�ʼ���õĵ�ʱ�������ṹ��
 */
struct ehci_iso_stream *__iso_stream_get (struct usbh_ehci *p_ehci,
                                          struct usbh_endpoint *p_ep);
/**
 * ��ʱ��������
 *
 * @param p_ehci EHCI�ṹ��
 * @param p_trp  ���������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t __iso_req(struct usbh_ehci *p_ehci, struct usbh_trp *p_trp,
                    struct ehci_iso_stream  *stream);
/**
 * ����һ����ʱ����
 *
 * @param p_ehci EHCI�ṹ��
 * @param p_trp  ��صĵ�ʱ���������
 * @param stream ��ʱ������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t __iso_sched_make(struct usbh_ehci *p_ehci, struct usbh_trp *p_trp,
                           struct ehci_iso_stream  *stream);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

