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
#ifndef __USBH_DEV_H
#define __USBH_DEV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "list/usb_list.h"
#include "usb_common_def.h"

/* USB�豸���ӿ�����*/
#define USBH_MAX_INTERFACES   32
/* USB�豸�����������*/
#define USBH_MAX_CONFIG       5
/* USB�豸���˵���Ŀ*/
#define USBH_MAX_EP_NUM       32

#define USBH_TRP_SHORT_NOT_OK  0x00000001    /* �̰�����*/
#define USBH_TRP_ISO_ASAP      0x00000002    /* Start ISO transfer at the earliest */
#define USBH_TRP_ZERO_PACKET   0x00000004    /* ��0��������������*/
#define USBH_TRP_NO_INTERRUPT  0x00000008    /* no interrupt needed,except for error */

#define AWBL_USBDEV_INJECT       1          /* �豸�����־*/
#define AWBL_USBDEV_EJECT        2          /* �豸�γ���־*/
#define AWBL_USBH_DEV_UNKNOWN   (1 << 8)    /* δ֪����*/

#define USBDEV_INJECT      1    /* �豸�����־*/
#define USBDEV_EJECT       2    /* �豸�γ���־*/
#define USBDEV_EXIST       4    /* �豸��ǰ�Ƿ���ڱ�־*/
#define USBDEV_IGONRE_PID  0    /* ����ʹ��PID*/
#define USBDEV_IGONRE_VID  0    /* ����ʹ��VID*/

/* ��ʱʱ�䣬���룬 ���ڷ���/���տ�����Ϣ*/
#define USB_CTRL_GET_TIMEOUT    5000
#define USB_CTRL_SET_TIMEOUT    5000

/* USB�豸�������ýṹ��*/
struct usbh_dev_spec_config_info{
    uint16_t pid;
    uint16_t vid;
    uint16_t config_num;
};

typedef void* usbh_monitor_handle;

/** �豸����*/
#define USBH_DEV_HUB     (1 << 0)  /* USB�������豸*/
#define USBH_DEV_UMS     (1 << 1)  /* USB�������洢�豸*/
#define USBH_DEV_UVC     (1 << 2)  /* USB����ͷ�豸*/
#define USBH_DEV_USERIAL (1 << 3)  /* USB�����豸*/
#define USBH_DEV_UNIC    (1 << 4)  /* USB����*/
#define USBH_DEV_UHID    (1 << 5)  /* USB����ӿ��豸*/
#define USBH_DEV_UAC     (1 << 7)  /* USB��Ƶ�豸*/
#define USBH_DEV_UNKNOWN (1 << 8)  /* δ֪����*/

/** USB�豸���ӽṹ��*/
struct usbh_dev_monitor {
    uint16_t             pid;                                            /* ��ص��豸��PID*/
    uint16_t             vid;                                            /* ��ص��豸��VID*/
    uint8_t              type;                                           /* �������*/
    void               (*user_callback) (void *p_arg, uint8_t evt_type); /* �û��ص�����*/
    void                *p_arg;                                          /* �ص���������*/
    struct usb_list_head node;                                           /* �ڵ�*/
    uint32_t             dev_type;                                       /* �豸����*/
    void                *job;                                            /* �û�����*/
    uint8_t              state;                                          /* ״̬*/
};

/** USB ��ʱ��������*/
struct usb_iso_packet_descriptor {
    uint32_t offset;         /* ƫ��*/
    uint32_t  length;        /* ��������ĳ���*/
    uint32_t actual_length;  /* ʵ�ʴ���ĳ���*/
    int status;              /* ״̬*/
};

/** USB ���������*/
struct usbh_trp {
    struct usbh_endpoint             *p_ep;                    /* �����������ض˵�*/
    struct usb_ctrlreq               *p_ctrl;                  /* SETUP��*/
    void                             *p_data;                  /* ���ݻ���*/
    size_t                            len;                     /* Ҫ����ĳ���*/
    void                             *p_ctrl_dma;              /* SETUP��ӳ���DMA�ڴ�*/
    void                             *p_data_dma;              /* ���ݹ���ӳ���DMA�ڴ�*/
    void                            (*pfn_done) (void *p_arg); /* ������ɻص�����*/
    void                             *p_arg;                   /* ������ɻص���������*/
    size_t                            act_len;                 /* ʵ�ʴ��䳤��*/
    int                               status;                  /* ����״̬*/
    int                               flag;                    /* �����־*/
    void                             *priv;                    /* �û�˽������*/
    int                               interval;                /* ����ʱ������(�ж�/��ʱ)*/
    void                             *hcpriv;                  /* ������������˽������*/
    int                               error_count;             /* (����)��ʱ����Ĵ�������*/
    int                               start_frame;             /* ��ʱ��ʼ֡*/
    int                               number_of_packets;       /* (����)ͬ����������(add by CYX at 9/17-2019)*/
    struct usb_iso_packet_descriptor *iso_frame_desc;          /* ��ʱ��������(add by CYX at 9/17-2019)*/
    struct usb_list_head              node;                    /* ��ǰUSB����������Ľڵ�*/
};

/* USB �˵�ṹ��*/
struct usbh_endpoint {
    struct usbh_device       *p_udev;    /* �����豸�ṹ��*/
    struct usb_endpoint_desc *p_desc;    /* USB�˵�����������*/
    usb_bool_t                enabled;   /* ʹ�ܱ�־λ*/
    int                       bandwidth; /* �˵��������*/
    int                       interval;  /* ����ʱ������(�ж�/��ʱ)*/
    struct usb_list_head      node;      /* ���˵�ṹ��ڵ�*/
    usb_sem_handle_t          sem;       /* �ź���*/
    void                     *priv;      /* ˽��������*/
    int                       extralen;  /* ������������ĳ���*/
    uint8_t                  *extra;     /* �����������(���磬�ض������������ض�����������) */
};

/** USB �ӿڽṹ��*/
struct usbh_interface {
    struct usb_list_head        node;      /* ��ǰ�ӿڽڵ�*/
    struct usb_interface_desc  *p_desc;    /* �ӿ�������*/
    struct usbh_endpoint       *eps;       /* �ӿ��µĶ˵�*/
    int                         extralen;  /* ������������ĳ���*/
    uint8_t                    *extra;     /* �����������(���磬�ض������������ض�����������) */
};

/* USB ���ýṹ��*/
struct usbh_config {
    struct usb_config_desc          *p_desc;    /* ����������*/
    char                            *string;    /* ��������*/
    struct usbh_function            *funs;      /* �ӿڹ��ܽṹ��*/
    int                              nfuns;     /* �ӿڹ�����*/
    int                              extralen;  /* ������������ĳ���*/
    uint8_t                         *extra;     /* �����������(���磬�ض������������ض�����������) */
};

/** USB �豸�ṹ��*/
struct usbh_device {
    struct usbh                    *p_hc;                 /* USB�����ṹ��*/
    char                            name[32];             /* �豸����*/
    int                             ref;                  /* �豸���ü���*/
    usb_mutex_handle_t              lock;                 /* �豸������*/
    struct usb_list_head            node;                 /* ��ǰ�豸�ṹ��ڵ�*/
    uint8_t                         addr;                 /* �豸��ַ*/
    uint8_t                         speed;                /* �豸�ٶ�*/
    struct usbh_hub                *p_hub;                /* ����������*/
    uint8_t                         port;                 /* �����������˿ں�*/
    uint8_t                         status;               /* �豸״̬*/
#define USBH_DEV_STA_CONN       0x01                      /* �豸������״̬*/
#define USBH_DEV_STA_INJECT     0x02                      /* �豸�Ѳ���״̬*/
#define USBH_DEV_STA_CFG        0x04                      /* �豸������״̬*/
/* �ж��豸�Ƿ�������״̬*/
#define USBH_IS_DEV_CONN(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_CONN)
/* �ж��豸�Ƿ��ǲ���״̬*/
#define USBH_IS_DEV_INJECT(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_INJECT)
/* �ж��豸�Ƿ�������״̬*/
#define USBH_IS_DEV_CFG(p_dev) \
            ((p_dev)->status & USBH_DEV_STA_CFG)
/* �ж��豸״̬�Ƿ���Ч*/
#define USBH_IS_DEV_VALID(p_dev) \
            ((p_dev)->status == (USBH_DEV_STA_CONN | \
                                 USBH_DEV_STA_CFG | \
                                 USBH_DEV_STA_INJECT))
    struct usb_ctrlreq        *p_ctrl;                   /* ��������(SETUP)*/
    struct usb_device_desc    *p_desc;                   /* USB�豸����������*/
    char                      *p_str_mft;                /* �豸������*/
    char                      *p_str_pdt;                /* �豸����*/
    char                      *p_str_snum;               /* �豸���к�*/
    struct usbh_config         cfg;                      /* USB�豸�����ýṹ��*/
    struct usbh_endpoint       ep0;                      /* �˵�0*/
    struct usb_endpoint_desc   ep0_desc;                 /* �˵�0����������*/
    struct usbh_endpoint      *ep_in[16];                /* �豸����˵�*/
    struct usbh_endpoint      *ep_out[16];               /* �豸����˵�*/
    uint16_t                   langid;                   /* ����ID*/
    uint32_t                   quirks;                   /* �豸����*/
    uint32_t                   dev_type;                 /* �豸����*/
    struct usbh_tt            *tt;                       /* ����ת����(��/ȫ���豸�ӵ����ټ�����)*/
    int                        ttport;                   /* �豸������ת�����������Ķ˿ں�*/
};

/** USB���ܽṹ��*/
struct usbh_function {
    char                                 name[16];    /* �豸����*/
    struct usbh_device                  *p_udev;      /* ��ص�USB�豸�ṹ��*/
    struct usb_list_head                 node;        /* �����ܽڵ�*/
    void                                *user_priv;   /* �û�˽����Ϣ*/

    uint8_t                              ifirst;      /* ��һ���ӿڵı��*/
    uint8_t                              nitfs;       /* �ӿڵ�����*/
    uint8_t                              nitfs_type;  /* �ӿ����͵�����(��ͨ�ӿڸ�ֵΪ1�����Ͻӿڸ�ֵΪ���Ͻӿڰ����Ľӿ���������)*/
    uint8_t                              clss;        /* ��*/
    uint8_t                              sclss;       /* ����*/
    uint8_t                              proto;       /* Э��*/
    uint8_t                              type;        /* �ӿ�����(��ͨ�ӿڻ����Ͻӿ�)*/

    struct usb_list_head                 itfs;        /* �ӿ�����*/
    void                                *driver_priv; /* ����˽����Ϣ*/
};

/***************************************��������************************************************/
/**
 * Ѱ���ض���������
 *
 * @param buffer ����
 * @param size   ���ݴ�С
 * @param dt1    Ҫ�ҵ�������������1
 * @param dt2    Ҫ�ҵ�������������2
 * @param n      ����������������
 *
 * @return ���������ݴ�С(�ֽ�)
 */
int find_next_descriptor(unsigned char *buffer,
                         int            size,
                         int            dt1,
                         int            dt2,
                         int           *num_skipped);
/** ��ʼ��USB�����豸��*/
usb_err_t usbh_dev_lib_init (void);
/**
 * ����һ��USB�豸
 *
 * @param p_hc  USB�����ṹ��
 * @param p_hub �����������ṹ��
 * @param port  �˿ں�
 *
 * @return �ɹ����ش����ɹ���USB�豸�ṹ��
 */
struct usbh_device *usbh_dev_create (struct usbh     *p_hc,
                                     struct usbh_hub *p_hub,
                                     uint8_t          port);
/** ����USB�豸�ṹ��*/
usb_err_t usbh_dev_destroy (struct usbh_device *p_udev);
/** USB�豸���뺯��*/
usb_err_t usbh_dev_inject (struct usbh_device *p_udev);
/** USB�豸��������*/
usb_err_t usbh_dev_eject (struct usbh_device *p_udev);
/** ��ȡ�豸����*/
uint32_t usbh_dev_type_get(struct usbh_device *p_udev);
/** �����豸����*/
usb_err_t usbh_dev_type_set(struct usbh_device *p_udev, uint32_t dev_type);
/** ����USB�豸������*/
usb_err_t usbh_dev_ref_get (struct usbh_device *p_udev);
/** ����USB�豸������*/
usb_err_t usbh_dev_ref_put (struct usbh_device *p_udev);
/** ����USB�����û�����*/
usb_err_t usbh_func_usrdata_set(struct usbh_function *p_fun, void *usrdata);
/** ��ȡUSB�����û�����*/
void *usbh_func_usrdata_get(struct usbh_function *p_fun);
/** ����USB������������*/
usb_err_t usbh_func_drvdata_set(struct usbh_function *p_fun, void *usrdata);
/** ��ȡUSB������������*/
void *usbh_func_drvdata_get(struct usbh_function *p_fun);
/**
 * USB��ȡ����������
 *
 * @param p_dev �豸�ṹ��
 * @param type  ��������������
 * @param value ����
 * @param index ����
 * @param len   Ҫд/�������ݳ���
 * @param p_buf ���ݻ���
 *
 * @return �ɹ�����ʵ�ʴ��䳤�ȣ�ʧ�ܷ��ش��������״̬
 */
usb_err_t usbh_dev_desc_get (struct usbh_device *p_dev,
                             uint16_t            type,
                             uint16_t            value,
                             uint16_t            index,
                             uint16_t            len,
                             void               *p_buf);
/**
 * USB������ȡ�豸�ַ���������
 *
 * @param p_udev Ŀ���豸
 * @param index  ����������
 *
 * @return �ɹ����ػ�ȡ�����ַ���
 */
char *usbh_dev_string_get (struct usbh_device *p_udev,
                           uint8_t             index);
/**
 * ��ȡ�豸��ǰ�����ñ��
 *
 * @param p_dev USB�豸
 *
 * @return �ɹ��������ñ��
 */
int usbh_dev_config_num_get(struct usbh_device *p_dev);
/**
 * USB�����豸��ַ����
 *
 * @param p_dev ���õ�ַ���豸�ṹ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_dev_addr_set (struct usbh_device *p_dev);
/**
 * USB���������豸����������
 *
 * @param p_dev Ŀ���豸
 * @param val   ���ñ��
 *
 * ���أ��ɹ�����USB_OK
 */
usb_err_t usbh_dev_cfg_set (struct usbh_device *p_dev,
                            uint8_t             val);
/**
 * Ѱ��USB�����ϵ��豸
 *
 * @param vid      �豸VID
 * @param pid      �豸PID
 * @param is_print �Ƿ����Ϣ��ӡ����
 *
 * @return ���ض�Ӧ��PID��VID���豸
 */
void *usbh_dev_find(uint16_t vid, uint16_t pid, uint32_t dev_type, usb_bool_t is_print);
/** ͨ��USB�豸�����Ҷ�Ӧ��USB�豸*/
void *usbh_dev_find_byname(char *p_name);
/** USB�豸���³�ʼ��*/
usb_err_t usbh_dev_reinit(void *handle);
/**
 * ���USB�豸�Ƿ�������״̬
 *
 * @param p_udev Ҫ����USB�豸
 *
 * @return ���ӷ���USB_TRUE��û���ӷ���USB_FALSE
 */
usb_bool_t usbh_dev_connect_check (struct usbh_device *p_udev);
/**
 * ͨ���ӿں��ҵ���Ӧ�Ľӿڽṹ��
 *
 * @param p_udev  USB�豸�ṹ��
 * @param intfnum �ӿڱ��
 * @param altnum  �ӿڵı������ú�
 *
 * @return �ҵ����ؽӿڽṹ�塣
 */
struct usbh_interface *usbh_dev_intfnum_to_intf(struct usbh_device *p_udev,
                                                uint32_t            intfnum,
                                                uint32_t            altnum);
/**
 * ͨ���ӿں��ҵ���Ӧ�Ĺ��ܽṹ��
 *
 * @param p_udev  USB�豸�ṹ��
 * @param intfnum �ӿڱ��
 *
 * @return �ҵ����ع��ܽṹ�塣
 */
struct usbh_function *usbh_dev_intfnum_to_func(struct usbh_device *p_udev, uint32_t intfnum);

/**
 * ��λһ���˵��״̬
 *
 * @param p_udev Ҫ��λ�˵���豸
 * @param epaddr �˵��ַ
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_dev_ep_reset(struct usbh_device *p_udev, unsigned int epaddr);

/**
 * USB�豸���ע�ắ��
 *
 * @param vid           Ҫ��ص��豸��PID
 * @param pid           Ҫ��ص��豸��VID
 * @param type          �������
 * @param user_callback �û��ص�����
 * @param data          �ص���������
 *
 * @return �ɹ����ؼ�ؾ��
 */
usbh_monitor_handle usbh_dev_monitor_register(uint16_t vid,
                                              uint16_t pid,
                                              uint32_t dev_type,
                                              uint8_t  monitor_type,
                                              void   (*user_callback) (void *p_arg, uint8_t type),
                                              void    *p_arg);
usbh_monitor_handle awbl_usbh_dev_monitor_register(uint16_t vid,
                                                   uint16_t pid,
                                                   uint32_t dev_type,
                                                   uint8_t  monitor_type,
                                                   void   (*user_callback) (void *p_arg, uint8_t type),
                                                   void    *p_arg);
/** USB�豸���ע������*/
usb_err_t usbh_dev_monitor_unregister(usbh_monitor_handle handle);
/**
 * ����USB�豸���ýӿ�
 *
 * @param p_udev  USB�豸�ṹ��
 * @param intfnum �ӿڱ��
 * @param altnum  �ӿڵı������ú�
 *
 * @return �ɹ�����0��
 */
int usbh_func_intf_set(struct usbh_function *p_fun,int intfnum, int altnum);
/** ��ȡUSB ���ܵ�һ���ӿ�*/
struct usbh_interface *usbh_func_first_intf_get(struct usbh_function *p_ufunc);
/**
 * ͨ���ӿ�Ѱ�Ҷ˵�
 *
 * @param p_itf  USB�ӿڽṹ��
 * @param epaddr �˵��ַ
 *
 * @return �ɹ������ҵ��Ķ˵�
 */
struct usbh_endpoint *usbh_intf_ep_find(struct usbh_interface *p_itf,
                                        uint8_t                epaddr);
/**
 * ����˵�ֹͣ״̬
 *
 * @param p_ep Ҫ���ֹͣ״̬�Ķ˵�
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ep_halt_clear (struct usbh_endpoint *p_ep);

/**
 * USB�������ƴ���
 *
 * @param p_ep    ʹ�ÿ��ƴ���Ķ˵�
 * @param type    ���䷽��|��������|����Ŀ��
 * @param req     �����USB����
 * @param val     ����
 * @param idx     ����
 * @param len     Ҫд/�������ݳ���
 * @param p_data  Ҫд/�������ݻ���
 * @param timeout ��ʱʱ��
 * @param flag    ��־
 *
 * @return ���û�����ݣ��ɹ����� 0����������ݣ��ɹ����ش�������ݴ�С
 */
usb_err_t usbh_ctrl_trp_sync (struct usbh_endpoint *p_ep,
                              uint8_t               type,
                              uint8_t               req,
                              uint16_t              val,
                              uint16_t              idx,
                              uint16_t              len,
                              void                 *p_data,
                              int                   timeout,
                              int                   flag);
/**
 * USB�����ύ���������
 *
 * @param p_ep    ʹ�õĶ˵�
 * @param p_ctrl  ���ƴ�������ṹ��
 * @param p_data  Ҫд/�������ݻ���
 * @param len     Ҫд/�������ݳ���
 * @param timeout ��ʱʱ��
 * @param flag    ��־
 *
 * @return �ɹ�����ʵ�ʴ��䳤�ȣ�ʧ�ܷ��ش��������״̬
 */
usb_err_t usbh_trp_sync (struct usbh_endpoint  *p_ep,
                         struct usb_ctrlreq    *p_ctrl,
                         void                  *p_data,
                         int                    len,
                         int                    timeout,
                         int                    flag);
/** �����������ɻص�����*/
void usbh_trp_done (struct usbh_trp *p_trp, int status);
/**
 * �ύ���������
 *
 * @param p_trp Ҫ�ύ�Ĵ��������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_trp_submit (struct usbh_trp *p_trp);
/**
 * ȡ�����������
 *
 * @param p_trp Ҫȡ���Ĵ��������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_trp_cancel (struct usbh_trp *p_trp);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBH_DEV_H */

