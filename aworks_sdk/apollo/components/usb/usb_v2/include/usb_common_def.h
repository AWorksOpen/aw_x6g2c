/*******************************************************************************
*                                    USB
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __USB_COMMON_DEF_H
#define __USB_COMMON_DEF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_common.h"


/** \brief USB���䷽�� */
#define USB_DIR_OUT                 0            /**< \brief OUT: to device */
#define USB_DIR_IN                  0x80         /**< \brief IN : to host */


/** \brief USB�������� */
#define USB_REQ_TYPE_MASK           (0x03 << 5)
#define USB_REQ_TYPE_STANDARD       (0x00 << 5)  /**< \brief USB��׼���� */
#define USB_REQ_TYPE_CLASS          (0x01 << 5)  /**< \brief USB������ */
#define USB_REQ_TYPE_VENDOR         (0x02 << 5)  /**< \brief USB�Զ������� */
#define USB_REQ_TYPE_RESERVED       (0x03 << 5)  /**< \brief ���� */


/** \brief USB����Ŀ�� */
#define USB_REQ_TAG_MASK               0x1f
#define USB_REQ_TAG_DEVICE             0x00      /**< \brief �豸 */
#define USB_REQ_TAG_INTERFACE          0x01      /**< \brief �ӿ� */
#define USB_REQ_TAG_ENDPOINT           0x02      /**< \brief �˵� */
#define USB_REQ_TAG_OTHER              0x03      /**< \brief ���� */
#define USB_REQ_TAG_PORT               0x04      /**< \brief �˿� */
#define USB_REQ_TAG_RPIPE              0x05      /**< \brief �ܵ� */


/** \brief USB���� */
#define USB_REQ_GET_STATUS             0x00      /**< \brief ��ȡ״̬ */
#define USB_REQ_CLEAR_FEATURE          0x01      /**< \brief ������� */
#define USB_REQ_SET_FEATURE            0x03      /**< \brief �������� */
#define USB_REQ_SET_ADDRESS            0x05      /**< \brief ���õ�ַ */
#define USB_REQ_GET_DESCRIPTOR         0x06      /**< \brief ��ȡ������ */
#define USB_REQ_SET_DESCRIPTOR         0x07      /**< \brief ���������� */
#define USB_REQ_GET_CONFIGURATION      0x08      /**< \brief ��ȡ���� */
#define USB_REQ_SET_CONFIGURATION      0x09      /**< \brief �������� */
#define USB_REQ_GET_INTERFACE          0x0A      /**< \brief ��ȡ�ӿ���Ϣ */
#define USB_REQ_SET_INTERFACE          0x0B      /**< \brief ���ýӿ� */
#define USB_REQ_SYNCH_FRAME            0x0C      /**< \brief ͬ��֡ */

#define USB_REQ_SET_ENCRYPTION         0x0D      /**< \brief ���ü��� */
#define USB_REQ_GET_ENCRYPTION         0x0E      /**< \brief ��ȡ���� */
#define USB_REQ_RPIPE_ABORT            0x0E      /**< \brief RPipe��ֹ */
#define USB_REQ_SET_HANDSHAKE          0x0F      /**< \brief �������� */
#define USB_REQ_RPIPE_RESET            0x0F      /**< \brief RPipe��λ */
#define USB_REQ_GET_HANDSHAKE          0x10      /**< \brief ��ȡ���� */
#define USB_REQ_SET_CONNECTION         0x11      /**< \brief �������� */
#define USB_REQ_SET_SECURITY_DATA      0x12      /**< \brief ���ü������� */
#define USB_REQ_GET_SECURITY_DATA      0x13      /**< \brief ��ȡ�������� */
#define USB_REQ_SET_WUSB_DATA          0x14      /**< \brief ��������USB���� */
#define USB_REQ_LOOPBACK_DATA_WRITE    0x15      /**< \brief �ػ�����д */
#define USB_REQ_LOOPBACK_DATA_READ     0x16      /**< \brief �ػ����ݶ� */
#define USB_REQ_SET_INTERFACE_DS       0x17      /**< \brief SET_INTERFACE_DS���� */
#define USB_REQ_SET_SEL                0x30      /**< \brief SET_SEL���� */
#define USB_REQ_SET_ISOCH_DELAY        0x31      /**< \brief SET_ISOCH_DELAY���� */


/** \brief USB�豸�ٶȶ��� */
#define USB_SPEED_UNKNOWN           0
#define USB_SPEED_LOW               1       /**< \brief ����(usb 1.1) */
#define USB_SPEED_FULL              2       /**< \brief ȫ��(usb 1.1) */
#define USB_SPEED_HIGH              3       /**< \brief ����(usb 2.0) */
#define USB_SPEED_WIRELESS          4       /**< \brief ����USB(usb 2.5) */
#define USB_SPEED_SUPER             5       /**< \brief ������(usb 3.0) */


/** \brief USB�豸�ඨ�� */
#define USB_CLASS_PER_INTERFACE        0x00  /**< \brief �ӿ������ж��� */
#define USB_CLASS_AUDIO                0x01  /**< \brief ��Ƶ�豸�� */
#define USB_CLASS_COMM                 0x02  /**< \brief ͨѶ�豸�� */
#define USB_CLASS_HID                  0x03  /**< \brief �˻������豸�� */
#define USB_CLASS_PHYSICAL             0x05  /**< \brief �����豸�� */
#define USB_CLASS_STILL_IMAGE          0x06  /**< \brief ͼ���豸�� */
#define USB_CLASS_PRINTER              0x07  /**< \brief ��ӡ���豸�� */
#define USB_CLASS_MASS_STORAGE         0x08  /**< \brief �������洢�� */
#define USB_CLASS_HUB                  0x09  /**< \brief �������� */
#define USB_CLASS_CDC_DATA             0x0a  /**< \brief CDC-Dateͨ���豸�� */
#define USB_CLASS_CSCID                0x0b  /**< \brief ���ܿ��豸�� */
#define USB_CLASS_CONTENT_SEC          0x0d  /**< \brief ��Ϣ��ȫ�豸�� */
#define USB_CLASS_VIDEO                0x0e  /**< \brief ��Ƶ�豸�� */
#define USB_CLASS_WIRELESS_CONTROLLER  0xe0  /**< \brief ���߿������� */
#define USB_CLASS_MISC                 0xef  /**< \brief ������ */
#define USB_CLASS_APP_SPEC             0xfe  /**< \brief Ӧ��ר�й淶 */
#define USB_CLASS_VENDOR_SPEC          0xff  /**< \brief �����Զ����� */
#define USB_SUBCLASS_VENDOR_SPEC       0xff  /**< \brief �����ض�����*/

/** \brief USB�˵��ַ����λ*/
#define USB_EP_NUM_MASK             0x0f
/** \brief USB�˵����� */
#define USB_EP_TYPE_MASK            0x03
#define USB_EP_TYPE_CTRL            0x00     /**< \brief ���ƶ˵� */
#define USB_EP_TYPE_ISO             0x01     /**< \brief ��ʱ�˵� */
#define USB_EP_TYPE_BULK            0x02     /**< \brief �����˵� */
#define USB_EP_TYPE_INT             0x03     /**< \brief �ж϶˵� */

/** \brief USB3.0�˵����� */
#define USB_SS_EP_SYNCTYPE          0x0c
#define USB_SS_EP_SYNC_NONE        (0 << 2)
#define USB_SS_EP_SYNC_ASYNC       (1 << 2)
#define USB_SS_EP_SYNC_ADAPTIVE    (2 << 2)
#define USB_SS_EP_SYNC_SYNC        (3 << 2)

/** \brief USB���������Ͷ��� */
#define USB_DT_DEVICE                  0x01  /**< \brief �豸������ */
#define USB_DT_CONFIG                  0x02  /**< \brief ���������� */
#define USB_DT_STRING                  0x03  /**< \brief �ַ��������� */
#define USB_DT_INTERFACE               0x04  /**< \brief �ӿ������� */
#define USB_DT_ENDPOINT                0x05  /**< \brief �˵�������(0�˵���)*/
#define USB_DT_DEVICE_QUALIFIER        0x06  /**< \brief �豸�޶������� */
#define USB_DT_OTHER_SPEED_CONFIG      0x07  /**< \brief ������������������ */
#define USB_DT_INTERFACE_POWER         0x08  /**< \brief �ӿڹ��������� */
#define USB_DT_OTG                     0x09  /**< \brief OTG������ */
#define USB_DT_DEBUG                   0x0a  /**< \brief ���������� */
#define USB_DT_INTERFACE_ASSOCIATION   0x0b  /**< \brief �ӿ����������� */
#define USB_DT_SECURITY                0x0c  /**< \brief ��ȫ�������� */
#define USB_DT_KEY                     0x0d  /**< \brief ����Կ�������� */
#define USB_DT_ENCRYPTION_TYPE         0x0e  /**< \brief �������������� */
#define USB_DT_BOS                     0x0f  /**< \brief �������豸Ŀ��洢 */
#define USB_DT_DEVICE_CAPABILITY       0x10  /**< \brief �豸���������� */
#define USB_DT_WIRELESS_ENDPOINT_COMP  0x11  /**< \brief ���߶˵��� */
#define USB_DT_WIRE_ADAPTER            0x21  /**< \brief DWA������ */
#define USB_DT_RPIPE                   0x22  /**< \brief RPipe������ */
#define USB_DT_CS_RADIO_CONTROL        0x23  /**< \brief Radio Control������ */
#define USB_DT_PIPE_USAGE              0x24  /**< \brief Pipe Usage������ */
#define USB_DT_SS_ENDPOINT_COMP        0x30  /**< \brief �����ٶ˵��� */


/** \brief USB�豸���Զ��� */
#define USB_DEV_SELF_POWERED        0   /**< \brief �Թ��� */
#define USB_DEV_REMOTE_WAKEUP       1   /**< \brief ֧��Զ�̻��� */


/** \brief USB�������������� */
#define USB_HUB_C_LOCAL_POWER      0
#define USB_HUB_C_OVER_CURRENT     1
#define USB_PORT_CONNECTION        0
#define USB_PORT_ENABLE            1
#define USB_PORT_SUSPEND           2
#define USB_PORT_OVER_CURRENT      3
#define USB_PORT_RESET             4
#define USB_PORT_POWER             8
#define USB_PORT_LOW_SPEED         9
#define USB_PORT_C_CONNECTION      16
#define USB_PORT_C_ENABLE          17
#define USB_PORT_C_SUSPEND         18
#define USB_PORT_C_OVER_CURRENT    19
#define USB_PORT_C_RESET           20
#define USB_PORT_C_TEST            21
#define USB_PORT_C_INDICATOR       22

/* USB����ӿ��豸�ӿ������Э�����*/
#define USB_INTERFACE_SUBCLASS_BOOT     1
#define USB_INTERFACE_PROTOCOL_KEYBOARD 1
#define USB_INTERFACE_PROTOCOL_MOUSE    2


#pragma pack (push, 1)


/**
 * \brief SETUP���ṹ����
 */
struct usb_ctrlreq {
    uint8_t     bRequestType;   /**< \brief �����������������͡����ն˵� */
    uint8_t     bRequest;       /**< \brief ȷ������ */
    uint16_t    wValue;         /**< \brief ����ר����Ϣ */
    uint16_t    wIndex;         /**< \brief ����ר����Ϣ */
    uint16_t    wLength;        /**< \brief ���ݽ׶����ݳ��� */
};



/**
 * \brief USB������ͷ��
 */
struct usb_desc_head {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ���������� */
};




/**
 * \brief USB�豸����������
 */
struct usb_device_desc {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ����01h�� */

    uint16_t    bcdUSB;             /**< \brief USB�淶�汾�� */
    uint8_t     bDeviceClass;       /**< \brief ����� */
    uint8_t     bDeviceSubClass;    /**< \brief ������� */
    uint8_t     bDeviceProtocol;    /**< \brief Э����� */
    uint8_t     bMaxPacketSize0;    /**< \brief �˵�0�����ߴ� */
    uint16_t    idVendor;           /**< \brief ����ID */
    uint16_t    idProduct;          /**< \brief ��ƷID */
    uint16_t    bcdDevice;          /**< \brief �豸�汾�� */
    uint8_t     iManufacturer;      /**< \brief �������ַ������������� */
    uint8_t     iProduct;           /**< \brief ��Ʒ�ַ������������� */
    uint8_t     iSerialNumber;      /**< \brief ���к��ַ������������� */
    uint8_t     bNumConfigurations; /**< \brief ������Ŀ */
};




/**
 * \brief USB��������������
 */
struct usb_config_desc {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ����02h�� */

    uint16_t    wTotalLength;       /**< \brief ��������������ȫ�������������ֽ��� */
    uint8_t     bNumInterfaces;     /**< \brief �����нӿ����� */
    uint8_t     bConfigurationValue;/**< \brief ���ñ�� */
    uint8_t     iConfiguration;     /**< \brief �����ַ������������� */
    uint8_t     bmAttributes;       /**< \brief �������ԣ��磺�Թ��硢Զ�̻��ѵȣ� */
    uint8_t     bMaxPower;          /**< \brief 2mA��λ�����߹��� */
};




/**
 * \brief USB �豸�޶�����������
 */
struct usb_qualifier_desc {
    uint8_t  bLength;               /**< \brief �������ֽڳ��� */
    uint8_t  bDescriptorType;       /**< \brief ����06h�� */

    uint16_t bcdUSB;                /**< \brief USB�淶�汾�� */
    uint8_t  bDeviceClass;          /**< \brief ����� */
    uint8_t  bDeviceSubClass;       /**< \brief ������� */
    uint8_t  bDeviceProtocol;       /**< \brief Э����� */
    uint8_t  bMaxPacketSize0;       /**< \brief �˵�0�����ߴ� */
    uint8_t  bNumConfigurations;    /**< \brief ������Ŀ */
    uint8_t  bRESERVED;             /**< \brief ���� */
};




/**
 * \brief USB�ӿ���������������
 */
struct usb_interface_assoc_desc {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ����0Bh�� */

    uint8_t     bFirstInterface;    /**< \brief �������ϵĵ�һ���ӿڵı�� */
    uint8_t     bInterfaceCount;    /**< \brief �������ϵĽӿڵ����� */
    uint8_t     bFunctionClass;     /**< \brief ����� */
    uint8_t     bFunctionSubClass;  /**< \brief ������� */
    uint8_t     bFunctionProtocol;  /**< \brief Э����� */
    uint8_t     iFunction;          /**< \brief �����ַ������������� */
};




/**
 * \brief USB�ӿ�����������
 */
struct usb_interface_desc {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ����04h�� */

    uint8_t     bInterfaceNumber;   /**< \brief �ӿڱ�� */
    uint8_t     bAlternateSetting;  /**< \brief ������ñ�� */
    uint8_t     bNumEndpoints;      /**< \brief ��֧�ֵĶ˵���Ŀ�����˵�0�⣩ */
    uint8_t     bInterfaceClass;    /**< \brief ����� */
    uint8_t     bInterfaceSubClass; /**< \brief ������� */
    uint8_t     bInterfaceProtocol; /**< \brief Э����� */
    uint8_t     iInterface;         /**< \brief �ӿ��ַ������������� */
};




/**
 * \brief USB�˵�����������
 */
struct usb_endpoint_desc {
    uint8_t     bLength;            /**< \brief �������ֽڳ��� */
    uint8_t     bDescriptorType;    /**< \brief ����05h�� */

    uint8_t     bEndpointAddress;   /**< \brief �˵��ַ�ͷ��� */
    uint8_t     bmAttributes;       /**< \brief ������Ϣ���紫�����ͣ� */
    uint16_t    wMaxPacketSize;     /**< \brief ��֧�ֵ������ߴ�
                                                                                                                                                ��12��11λ������ÿ΢֡����������
                                                00(ÿ΢֡1������)
                                                01(ÿ΢֡2������)
                                                10(ÿ΢֡3������)
                                                11(����)
                                                                                                                                                ��11λָ����ÿ�������������Ч�����ֽ���*/
    uint8_t     bInterval;          /**< \brief ��ѯ�˵��ʱ����
                                                                                                                              �����豸�����ٶ���֡(ȫ/����)��΢֡(����)��ʾ��λ(1ms��125us)
                                                                                                                              ȫ/����ͬ���˵㣬���ֵ������1~16����ѯ�˵��ʱ��Ϊ2^(binterval-1)΢֡
                                                                                                                              ȫ/�����ж϶˵㣬���ֵ������1~255��֡
                                                                                                                              �����ж϶˵㣬���ֵ������1����16����ѯ�˵��ʱ��Ϊ2^(binterval-1)΢֡
                                                                                                                              ��������/��������˵㣬bintervalָ���˵�����NAK�ʣ�0��ʾû��NAKs
                                                                                                                              ����ֵ��ʾÿbinterval΢֡�����һ��NAK�����ֵ��ѡ0~255*/

    /* ע�⣺��������������Ƶ�˵�*/
    uint8_t     bRefresh;
    uint8_t     bSynchAddress;
};



#pragma pack (pop)


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USB_COMMON_H */

/* end of file */
