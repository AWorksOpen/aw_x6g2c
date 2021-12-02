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
#ifndef __USBH_H
#define __USBH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "list/usb_list.h"
#include "usb_common.h"
#include "usb_common_def.h"
#include "host/core/usbh_dev.h"
#include "host/class/hub/usbh_hub.h"

/* USB������Ϣ�ṹ��*/
struct usbh_lib_info{
    char version[32];
    char build_date[32];
    char modified_date[32];
};

/* USBH�ڴ��¼�ṹ��*/
struct usbh_mem_record {
    char     drv_name[32];
    uint32_t mem_alloc_num;
    uint32_t mem_free_num;
    uint32_t mem_total_num;
    uint32_t mutex_alloc_num;
    uint32_t mutex_free_num;
    uint32_t sem_alloc_num;
    uint32_t sem_free_num;
};

/* USB �����ṹ��*/
struct usbh {
    uint8_t              host_index;  /* ��������*/
    uint32_t             map[4];      /* �豸��ַ��*/
    struct usbh_hub      rh;          /* ��������*/
    usb_mutex_handle_t   lock;        /* ������*/
    struct usb_list_head node;        /* ��ǰ�����������ڵ�*/
    void                *controller;  /* ����������*/
    usb_bool_t           is_init;     /* �Ƿ��ʼ��*/
    uint8_t             *user_priv;   /* �û�˽������*/
};

/* USB���������ص�����*/
struct usb_ctrllor_drv {
    /* ������*/
    usb_err_t (*pfn_request) (struct usbh     *p_hc,
                              struct usbh_trp *p_trp);

    /* ȡ������*/
    usb_err_t (*pfn_cancel) (struct usbh     *p_hc,
                             struct usbh_trp *p_trp);
    /* ������������*/
    usb_err_t (*pfn_rh_ctrl) (struct usbh        *p_hc,
                              uint8_t             type,
                              uint8_t             req,
                              uint16_t            val,
                              uint16_t            idx,
                              uint16_t            len,
                              void               *p_data);
    /* ��ȡ��������״̬*/
    usb_err_t (*pfn_rh_check) (struct usbh      *p_hc,
                               uint16_t         *p_change);
    /* ʹ�ܶ˵�*/
    usb_err_t (*pfn_ep_en) (struct usbh          *p_hc,
                            struct usbh_endpoint *p_ep);
    /* ���ܶ˵�*/
    usb_err_t (*pfn_ep_dis) (struct usbh          *p_hc,
                             struct usbh_endpoint *p_ep);
    /* ��ȡ��ǰ֡���*/
    int (*pfn_framenum_get)(struct usbh *p_hc);
};

/* ��������������*/
enum{
    UNKNOW = 0,
    OHCI   = 1,
    EHCI   = 2,
    XHCI   = 3,
    VENDOR = 4,
};

/* �������������ֽṹ��*/
struct usbh_ctrllor_type{
    uint8_t type;
    char    name[8];
};

/* USB �����������ṹ��ͷ��*/
struct usbh_ctrllor_head {
    uint8_t                 ctrllor_type;    /* ����������*/
    struct usb_ctrllor_drv *ctrllor_drv;     /* ����������������*/
    struct usbh            *p_hc;            /* USB����*/
    usb_bool_t              is_init;         /* �������Ƿ��Ѿ���ʼ��*/
};

/* ��ȡ�豸VID*/
#define USBH_DEV_VID_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->idVendor))
/* ��ȡ�豸PID*/
#define USBH_DEV_PID_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->idProduct))
/* ��ȡ�豸�ٶ�*/
#define USBH_DEV_SPEED_GET(p_fun)          ((p_fun)->p_udev->speed)
/* ��ȡ�豸��*/
#define USBH_DEV_CLASS_GET(p_fun)          ((p_fun)->p_udev->p_desc->bDeviceClass)
/* ��ȡ�豸����*/
#define USBH_DEV_SUB_CLASS_GET(p_fun)      ((p_fun)->p_udev->p_desc->bDeviceSubClass)
/* ��ȡ�豸Э��*/
#define USBH_DEV_PROTOCOL_GET(p_fun)       ((p_fun)->p_udev->p_desc->bDeviceProtocol)
/* ��ȡ�豸��������*/
#define USBH_DEV_NCFGS_GET(p_fun)          ((p_fun)->p_udev->p_desc->bNumConfigurations)
/* ��ȡ�豸��*/
#define USBH_DEV_NAME_GET(p_fun)           ((p_fun)->p_udev->name)
/* ��ȡ�豸����*/
#define USBH_DEV_TYPE_GET(p_fun)           ((p_fun)->p_udev->dev_type)
/* ��ȡ�豸�汾*/
#define USBH_DEV_BCD_GET(p_fun)            (USB_CPU_TO_LE16((p_fun)->p_udev->p_desc->bcdDevice))
/* ��ȡ�豸���������ַ���*/
#define USBH_DEV_PDTSTR_GET(p_fun)         ((p_fun)->p_udev->p_str_pdt)
/* ��ȡ�豸��ǰ���ýӿ�����*/
#define USBH_DEV_CFG_INTFNUM_GET(p_fun)    ((p_fun)->p_udev->cfg.p_desc->bNumInterfaces)

/* ��ȡ���ܵ�һ���ӿں�*/
#define USBH_FUNC_FIRST_INTFNUM_GET(p_fun) ((p_fun)->ifirst)
/* ��ȡ���ܽӿ�����*/
#define USBH_FUNC_NINTF_GET(p_fun)         ((p_fun)->nitfs)
/* ��ȡ���ܽӿ�Э��*/
#define USBH_FUNC_PROTO_GET(p_fun)         ((p_fun)->proto)
/* ��ȡ���ܽӿ���*/
#define USBH_FUNC_CLASS_GET(p_fun)         ((p_fun)->clss)
/* ��ȡ���ܽӿ�����*/
#define USBH_FUNC_SUBCLASS_GET(p_fun)      ((p_fun)->sclss)

/* ��ȡ�������˿�����*/
#define USBH_HUB_PORT_NUM_GET(p_hub)       ((p_hub)->nports)

/* ��ȡ�ӿ�����*/
#define USBH_INTF_SUB_CLASS_GET(p_intf)    ((p_intf)->p_desc->bInterfaceSubClass)
/* ��ȡ�ӿ���*/
#define USBH_INTF_CLASS_GET(p_intf)        ((p_intf)->p_desc->bInterfaceClass)
/* ��ȡ�ӿڶ˵�����*/
#define USBH_INTF_EP_NUM_GET(p_intf)       ((p_intf)->p_desc->bNumEndpoints)
/* ��ȡ�ӿڱ��*/
#define USBH_INTF_NUM_GET(p_intf)          ((p_intf)->p_desc->bInterfaceNumber)
/* ��ȡ�ӿڱ������ñ��*/
#define USBH_INTF_ALT_NUM_GET(p_intf)      ((p_intf)->p_desc->bAlternateSetting)

/* ��ȡ�˵�����*/
#define USBH_EP_TYPE_GET(p_ep)             ((p_ep)->p_desc->bmAttributes & 0x03)
/* ��ȡ�˵㷽��*/
#define USBH_EP_DIR_GET(p_ep)              ((p_ep)->p_desc->bEndpointAddress & 0x80)
/* ���˵��ǲ����ж϶˵�*/
#define USBH_EP_INT_GET(p_ep)              (((p_ep)->p_desc->bmAttributes & USB_EP_TYPE_INT) == USB_EP_TYPE_INT)
/* ���˵��ǲ��ǵ�ʱ�˵�*/
#define USBH_EP_ISO_GET(p_ep)              (((p_ep)->p_desc->bmAttributes & USB_EP_TYPE_ISO) == USB_EP_TYPE_ISO)
/* ��ȡ�˵��ַ*/
#define USBH_EP_ADDR_GET(p_ep)             ((p_ep)->p_desc->bEndpointAddress & 0x0F)
/* ��ȡ�˵�����*/
#define USBH_EP_ATTR_GET(p_ep)             ((p_ep)->p_desc->bmAttributes)
/* ��ȡ�˵�������С*/
#define USBH_EP_MPS_GET(p_ep)              (USB_CPU_TO_LE16((p_ep)->p_desc->wMaxPacketSize))
/* ��ȡ�˵���ѯ����*/
#define USBH_EP_INTVAL_GET(p_ep)           ((p_ep)->p_desc->bInterval)
/* ���˵��ǲ��������ж϶˵�*/
static inline int USBH_EP_IS_INT_IN(const struct usbh_endpoint *ep)
{
    return USBH_EP_DIR_GET(ep) && USBH_EP_INT_GET(ep);
}

static inline int USBH_EP_IS_ISO_IN(const struct usbh_endpoint *ep)
{
    return USBH_EP_DIR_GET(ep) && USBH_EP_ISO_GET(ep);
}

/***************************************��������**************************************************/
/** ��ȡUSBЭ��ջ��Ϣ*/
void usbh_lib_info_get(struct usbh_lib_info *p_usbh_info);
/**
 * ��ʼ��USB������
 *
 * @param ctrllor_num ֧�ֵ���������������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_core_lib_init(uint8_t ctrllor_num);
/** USB�����ⷴ��ʼ��*/
usb_err_t usbh_core_lib_uninit(void);
/** ͨ��������ȡUSB����������*/
struct usbh *usbh_hc_get(uint8_t hc_index);
/** ���������û�˽������*/
usb_err_t usbh_usrdata_set(struct usbh *p_hc, void *usrdata);
/** ��ȡ�����û�˽������*/
void* usbh_usrdata_get(struct usbh *p_hc);
/**
 * ����USB����������
 *
 * @param hc_index �����������ṹ������
 *
 * @return �ɹ����ش�����USB�����ṹ��
 */
struct usbh *usbh_create(uint8_t host_index);
/**
 * ����USB����������
 *
 * @param p_hc Ҫ���ٵ������������ṹ��
 */
void usbh_destory(struct usbh *p_hc);
/** ����USB ����������*/
usb_err_t usbh_ctrllor_set(struct usbh *p_hc, void *ctrllor);
/** ��ȡUSB ����������*/
void *usbh_ctrllor_get(struct usbh *p_hc);
/** USB���������豸��ַ*/
int usbh_dev_addr_alloc (struct usbh *p_hc);
/** �ͷ��豸��ַ*/
usb_err_t usbh_dev_addr_free (struct usbh *p_hc, int addr);
/** ��ȡ��ǰ֡���*/
usb_err_t usbh_frame_num_get(struct usbh *p_hc);
/**
 * USB�����˵�ʹ��
 *
 * @param p_hc USB�����ṹ��
 * @param p_ep Ҫʹ�ܵĶ˵�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ep_enable (struct usbh *p_hc, struct usbh_endpoint *p_ep);
/**
 * USB�����˵���ú���
 *
 * @param p_hc USB�����ṹ��
 * @param p_ep Ҫ���õĶ˵�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ep_disable (struct usbh *p_hc, struct usbh_endpoint *p_ep);
/**
 * USB���������亯��
 *
 * @param p_hc  USB�����ṹ��
 * @param p_trp Ҫ�ύ�Ĵ��������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_request (struct usbh *p_hc, struct usbh_trp *p_trp);
/**
 * USB����ȡ������
 *
 * @param p_hc  USB�����ṹ��
 * @param p_trp Ҫȡ���Ĵ��������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_cancel (struct usbh     *p_hc,
                       struct usbh_trp *p_trp);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_H */

