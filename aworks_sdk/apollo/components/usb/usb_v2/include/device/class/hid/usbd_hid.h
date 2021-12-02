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
#ifndef __USBD_HID_H
#define __USBD_HID_H

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
#define __USBD_HID_TRACE(info, ...) \
            do { \
                usb_printf("USBD-HID:"info, ##__VA_ARGS__); \
            } while (0)

//#define AW_USBD_HID_PASS_BACK          /* �Ƿ���HID�ش�--�������� */
#define USB_USBD_HID_MSGQ_MAX_COUNT     64

/* HID �������*/
#define USB_HID_SUBCLASS_NONE               0x00
#define USB_HID_SUBCLASS_BOOT               0x01

/* HID Э�����*/
#define USB_HID_PROTOCOL_NONE               0x00
#define USB_HID_PROTOCOL_KEYBOARD           0x01
#define USB_HID_PROTOCOL_MOUSE              0x02

/* HID ���������� */
#define USB_HID_HID_DESCRIPTOR_TYPE         0x21
#define USB_HID_REPORT_DESCRIPTOR_TYPE      0x22
#define USB_HID_PHYSICAL_DESCRIPTOR_TYPE    0x23

/* HID �������*/
#define USB_HID_REQUEST_GET_REPORT          0x01
#define USB_HID_REQUEST_GET_IDLE            0x02
#define USB_HID_REQUEST_GET_PROTOCOL        0x03
#define USB_HID_REQUEST_SET_REPORT          0x09
#define USB_HID_REQUEST_SET_IDLE            0x0A
#define USB_HID_REQUEST_SET_PROTOCOL        0x0B

/* HID ��������*/
#define USB_HID_REPORT_INPUT                0x01
#define USB_HID_REPORT_OUTPUT               0x02
#define USB_HID_REPORT_FEATURE              0x03

/* Usage Pages */
#define USB_HID_USAGE_PAGE_UNDEFINED        0x00
#define USB_HID_USAGE_PAGE_GENERIC          0x01
#define USB_HID_USAGE_PAGE_SIMULATION       0x02
#define USB_HID_USAGE_PAGE_VR               0x03
#define USB_HID_USAGE_PAGE_SPORT            0x04
#define USB_HID_USAGE_PAGE_GAME             0x05
#define USB_HID_USAGE_PAGE_DEV_CONTROLS     0x06
#define USB_HID_USAGE_PAGE_KEYBOARD         0x07
#define USB_HID_USAGE_PAGE_LED              0x08
#define USB_HID_USAGE_PAGE_BUTTON           0x09
#define USB_HID_USAGE_PAGE_ORDINAL          0x0A
#define USB_HID_USAGE_PAGE_TELEPHONY        0x0B
#define USB_HID_USAGE_PAGE_CONSUMER         0x0C
#define USB_HID_USAGE_PAGE_DIGITIZER        0x0D
#define USB_HID_USAGE_PAGE_UNICODE          0x10
#define USB_HID_USAGE_PAGE_ALPHANUMERIC     0x14
/* ... */

/* Generic Desktop Page (0x01) */
#define USB_HID_USAGE_GENERIC_POINTER               0x01
#define USB_HID_USAGE_GENERIC_MOUSE                 0x02
#define USB_HID_USAGE_GENERIC_JOYSTICK              0x04
#define USB_HID_USAGE_GENERIC_GAMEPAD               0x05
#define USB_HID_USAGE_GENERIC_KEYBOARD              0x06
#define USB_HID_USAGE_GENERIC_KEYPAD                0x07
#define USB_HID_USAGE_GENERIC_X                     0x30
#define USB_HID_USAGE_GENERIC_Y                     0x31
#define USB_HID_USAGE_GENERIC_Z                     0x32
#define USB_HID_USAGE_GENERIC_WHEEL                 0x38
/* ... */

/**
 * \brief HID Report Item Macros
 */

/* Main Items */
#define USB_HID_INPUT(x)           0x81,x
#define USB_HID_OUTPUT(x)          0x91,x
#define USB_HID_FEATURE(x)         0xB1,x
#define USB_HID_COLLECTION(x)      0xA1,x
#define USB_HID_ENDCOLLECTION      0xC0

/* Data (Input, Output, Feature) */
#define USB_HID_DATA               0<<0
#define USB_HID_CONSTANT           1<<0
#define USB_HID_ARRAY              0<<1
#define USB_HID_VARIABLE           1<<1
#define USB_HID_ABSOLUTE           0<<2
#define USB_HID_RELATIVE           1<<2
#define USB_HID_NOWRAP             0<<3
#define USB_HID_WRAP               1<<3
#define USB_HID_LINEAR             0<<4
#define USB_HID_NONLINEAR          1<<4
#define USB_HID_PREFERREDSTATE     0<<5
#define USB_HID_NOPREFERRED        1<<5
#define USB_HID_NONULLPOSITION     0<<6
#define USB_HID_NULLSTATE          1<<6
#define USB_HID_NONVOLATILE        0<<7
#define USB_HID_VOLATILE           1<<7

/* Collection Data */
#define USB_HID_PHYSICAL           0x00
#define USB_HID_APPLICATION        0x01
#define USB_HID_LOGICAL            0x02
#define USB_HID_REPORT             0x03
#define USB_HID_NAMEDARRAY         0x04
#define USB_HID_USAGESWITCH        0x05
#define USB_HID_USAGEMODIFIER      0x06

/* Global Items */
#define USB_HID_USAGEPAGE(x)       0x05,x
#define USB_HID_USAGEPAGEVENDOR(x) 0x06,x,0xFF
#define USB_HID_LOGICALMIN(x)      0x15,x
#define USB_HID_LOGICALMINS(x)     0x16,(x&0xFF),((x>>8)&0xFF)
#define USB_HID_LOGICALMINL(x)     0x17,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define USB_HID_LOGICALMAX(x)      0x25,x
#define USB_HID_LOGICALMAXS(x)     0x26,(x&0xFF),((x>>8)&0xFF)
#define USB_HID_LOGICALMAXL(x)     0x27,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define USB_HID_PHYSICALMIN(x)     0x35,x
#define USB_HID_PHYSICALMINS(x)    0x36,(x&0xFF),((x>>8)&0xFF)
#define USB_HID_PHYSICALMINL(x)    0x37,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define USB_HID_PHYSICALMAX(x)     0x45,x
#define USB_HID_PHYSICALMAXS(x)    0x46,(x&0xFF),((x>>8)&0xFF)
#define USB_HID_PHYSICALMAXL(x)    0x47,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define USB_HID_UNITEXPONENT(x)    0x55,x
#define USB_HID_UNIT(x)            0x65,x
#define USB_HID_UNITS(x)           0x66,(x&0xFF),((x>>8)&0xFF)
#define USB_HID_UNITL(x)           0x67,(x&0xFF),((x>>8)&0xFF),((x>>16)&0xFF),((x>>24)&0xFF)
#define USB_HID_REPORTSIZE(x)      0x75,x
#define USB_HID_REPORTID(x)        0x85,x
#define USB_HID_REPORTCOUNT(x)     0x95,x
#define USB_HID_PUSH               0xA0
#define USB_HID_POP                0xB0

/* Local Items */
#define USB_HID_USAGE(x)           0x09,x
#define USB_HID_USAGEMIN(x)        0x19,x
#define USB_HID_USAGEMAX(x)        0x29,x

/* HID��������*/
struct usbd_hid_class_descriptor {
    uint8_t  type;
    uint16_t wlength;
} __attribute__ ((packed));

/* HID������*/
struct usbd_hid_descriptor {
    uint8_t  blength;
    uint8_t  btype;
    uint16_t bcd;
    uint8_t  bcountry;
    uint8_t  bnum;
    struct usbd_hid_class_descriptor desc[1];
} __attribute__ ((packed));

/** usb����ӿ��豸����*/
typedef struct usbd_hid {
    struct usbd_obj_fun      fun;
    struct usbd_obj_pipe     int_in;
    struct usbd_obj_pipe     int_out;
    char                     name[32];
    const char              *p_report;
    uint16_t                 rep_size;
    usb_mutex_handle_t       mutex;
    usb_sem_handle_t         semb;
    struct usb_list_head     node;
}usbd_hid_t;

/**
 * ����һ��USB HID�豸
 *
 * @param p_obj    USB�豸ʵ��
 * @param p_hid    USB HID�豸�ṹ��
 * @param p_name   USB HID�豸����
 * @param p_rpd    �ϱ�������ݵļ���
 * @param rpd_size �ϱ�������ݼ��ϵĴ�С
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_hid_create(struct usbd_obj  *p_obj,
                          struct usbd_hid  *p_hid,
                          const char       *p_name,
                          const char       *p_rpd,
                          uint16_t          rpd_size);
/**
 * USB HID�豸�ϱ�����
 *
 * @param p_hid    USB�豸 HID�ṹ��
 * @param p_report �ϱ������ݻ����ַ
 * @param nbytes   �ϱ������ݳ���
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_hid_report_send(struct usbd_hid *p_hid,
                               void            *p_report,
                               size_t           nbytes,
                               int              timeout);
/**
 * USB HID�豸���ձ�������
 *
 * @param p_hid    USB�豸 HID�ṹ��
 * @param p_report ��������ݻ����ַ
 * @param nbytes   ��������ݳ���
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_hid_report_receive(struct usbd_hid *p_hid,
                                  void            *p_report,
                                  size_t           nbytes,
                                  int              timeout);

/** HID�������ṹ��*/
typedef struct usbd_mouse_in_report {
    uint8_t     left     :1;    /* �����bits[0]*/
    uint8_t     right    :1;    /* �Ҽ���bits[1]*/
    uint8_t     center   :1;    /* �м���� bits[2]*/
    uint8_t     padding  :5;    /* bits[7:3]*/
    int8_t      x;              /* �������x*/
    int8_t      y;              /* �������y*/
    int8_t      wheel;          /* ����*/
}usbd_mouse_in_report_t;

/**
 * ����һ��HID ����豸
 *
 * @param p_obj  USB�豸ʵ��
 * @param p_hid  USB HID�豸ʵ��
 * @param p_name USB HID�豸����
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_hid_mouse_create(struct usbd_obj *p_obj,
                                struct usbd_hid *p_hid,
                                const char      *p_name);
/**
 * HID ����豸��������
 *
 * @param p_hid    USB HID�豸�ṹ��
 * @param p_report �����������
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_hid_mouse_send(struct usbd_hid        *p_hid,
                              usbd_mouse_in_report_t *p_report,
                              int                     timeout);
/** HID �����ϱ�����*/
typedef struct usbd_keyboard_in_report {
    uint8_t     ckey;              /* �����ض����ܼ�������ctrl��shift��at��win*/
    uint8_t     cst;               /* ��������ò��ûʲô��*/
    uint8_t     key[6];            /* ��ͨ����������ABCD��1234��F1��F2�����֧��ͬʱ����6����ͨ����*/
}usbd_keyboard_in_report_t;

typedef struct usbd_keyboard_out_report {
    uint8_t     led     :5;
    uint8_t     padding :3;
}usbd_keyboard_out_report_t;

/**
 * ����һ��HID �����豸
 *
 * @param p_obj  USB�豸ʵ��
 * @param p_hid  USB HID�豸
 * @param p_name �����豸������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbd_hid_keyboard_create(struct usbd_obj *p_obj,
                                   struct usbd_hid *p_hid,
                                   const char      *p_name);
/** ���̼�ֵת��*/
uint8_t usbd_hid_key_to_code(uint8_t key);
/**
 * HID �����豸��������
 *
 * @param p_hid    USB HID�豸
 * @param p_report ���͵����ݽṹ
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_hid_keyboard_send(struct usbd_hid           *p_hid,
                                 usbd_keyboard_in_report_t *p_report,
                                 int                        timeout);
/**
 * HID �����豸��������
 *
 * @param p_hid    USB HID�豸
 * @param p_report ���յ����ݽṹ
 * @param timeout  ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
usb_err_t usbd_hid_keyboard_receive (struct usbd_hid            *p_hid,
                                     usbd_keyboard_out_report_t *p_report,
                                     int                         timeout);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __USBD_HID_H */

