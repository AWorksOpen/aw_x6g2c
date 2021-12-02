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
#ifndef __USBH_HUB_H
#define __USBH_HUB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "host/core/usbh_dev.h"
#include "list/usb_list.h"

/* USB������������*/
#define USBH_ROOT_HUB_NUM     2
/* USB���������˿�����*/
#define USBH_HUB_MAX_PORTS    15

/**
 * USB TT(Transaction Translators)
 * ����/ȫ���豸������ټ�����ʱ���ͻ�ʹ������ת����
 */
struct usbh_tt {
    struct usbh_function *p_hub;      /* ���θ��ټ�����*/
    int                   multi;      /* �Ƿ�ÿ���˿ڶ���һ������ת����*/
    uint32_t              think_time; /* ˼��ʱ��(����)*/
};

/** USB �������ṹ��*/
struct usbh_hub {
    struct usbh_function   *p_fun;                        /* ��ص�USB���ܽṹ��*/
    struct usb_list_head    evt;                          /* �¼�����*/
    struct usb_list_head    node;                         /* ��ǰ�������ڵ�*/
    struct usbh_trp         trp;                          /* ���������������*/
    struct usbh_device     *ports[USBH_HUB_MAX_PORTS];    /* �������˿��豸*/
    uint32_t               *p_sta;                        /* ������״̬*/
    uint8_t                 err_cnt;                      /* �������*/
    uint8_t                 nports;                       /* �˿�����*/
    uint8_t                 pwrtim;                       /* �ϵ�ʱ��*/
    usb_mutex_handle_t      lock;                         /* ������*/
    struct usbh_endpoint   *ep;                           /* �������Ķ˵�*/
    /* ���������ֶ������ڸ�������*/
    struct usbh_tt          tt;                           /* ����ת����(��/ȫ���豸�ӵ����ټ�����)*/
    uint8_t                 speed;                        /* ���������ٶ�*/
};

/***************************************��������**************************************************/
/**
 * �ڲ���������������
 *
 * @param stk_size ջ��С
 * @param stk_prio �������ȼ�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_hub_lib_init (int stk_size, int stk_prio);
/**
 * ��ʼ����������
 *
 * @param p_hc    ����������
 * @param nports  �˿�����
 * @param speed   ���������ٶ�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_rh_init (struct usbh     *p_hc,
                        uint8_t          nports,
                        uint8_t          speed);
/**
 * USB ���������������ƺ���
 *
 * @param p_hc   USB�����ṹ��
 * @param type   ������������
 * @param req    ��������
 * @param val    ֵ
 * @param idx    ����
 * @param len    ���ݳ���
 * @param p_data ���ݻ���
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_rh_ctrl (struct usbh        *p_hc,
                        uint8_t             type,
                        uint8_t             req,
                        uint16_t            val,
                        uint16_t            idx,
                        uint16_t            len,
                        void               *p_data);
/** USB��������������麯��*/
usb_err_t usbh_rh_check(struct usbh *p_hc, uint16_t *p_change);
/** USB�������������仯����*/
void usbh_rh_change (struct usbh *p_hc, uint16_t change);
/** ����һ����ͨ�ļ�����*/
struct usbh_hub *usbh_hub_create(struct usbh_function *p_ufunc);
/** ����һ��������*/
void usbh_hub_destory(struct usbh_hub *p_hub);
/** ����������*/
usb_err_t usbh_hub_start(struct usbh_hub *p_hub);
/**
 * ʹ�ܼ������˿�
 *
 * @param p_usb_hub �������ṹ��
 * @param port_num  �˿ں�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_hub_port_enable(struct usbh_hub *p_hub, int port_num);
/** ���ܼ������˿�*/
usb_err_t usbh_hub_port_disable(struct usbh_hub *p_hub);
/**
 * ��鼯�����˿��Ƿ�����
 *
 * @param p_usb_hub �������ṹ��
 * @param port_num  �˿ں�
 *
 * @return �ɹ�����USB_OK
 */
usb_bool_t usbh_hub_port_connect_check (struct usbh_hub *p_hub,
                                        uint8_t          port);
/**
 * �������˿����³�ʼ��
 *
 * @param p_usb_hub �������ṹ��
 * @param port_num  �˿ں�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_hub_port_reinit(struct usbh_hub *p_hub, int port);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_HUB_H */
