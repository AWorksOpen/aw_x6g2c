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
#ifndef __USBD_H
#define __USBD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common_def.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "list/usb_list.h"

/* ��ӡ����*/
#define __USBD_TRACE(info, ...) \
            do { \
                usb_printf("USBD:"info, ##__VA_ARGS__); \
            } while (0)

/** �������ϱ����¼�����*/
#define USBD_EVT_CONNECT           0x00000001           /* ����*/
#define USBD_EVT_DISCONNECT        0x00000002           /* �Ͽ�*/
#define USBD_EVT_BUS_RST           0x00000004           /* ���߸�λ*/
#define USBD_EVT_SUSPENDED         0x00000008           /* ����*/
#define USBD_EVT_RESUME            0x00000010           /* �ظ�*/
#define USBD_EVT_SETUP_PKT         0x00000020           /* setup��*/
#define USBD_EVT_UPDATE_SPEED      0x00000040           /* �����ٶ�*/
#define USBD_EVT_ERROR             0x00000080           /* ����*/
#define USBD_EVT_DC_STOP           0x00000100           /* �豸������ֹͣ*/

/** ������״̬*/
#define USBD_ST_NOTATTACHED       0           /* �Ͽ�*/
#define USBD_ST_ATTACHED          1           /* ���ӵ���û��ö��*/
#define USBD_ST_POWERED           2           /* �ϵ�*/
#define USBD_ST_DEFAULT           3           /* Ĭ��*/
#define USBD_ST_ADDRESS           4           /* ���õ�ַ*/
#define USBD_ST_CONFIGURED        5           /* ����*/
#define USBD_ST_SUSPENDED         6           /* ����*/

/** �������ٶ�*/
#define USBD_SPEED_UNKNOWN        0
#define USBD_SPEED_LOW            1           /* usb 1.1 */
#define USBD_SPEED_FULL           2           /* usb 1.1 */
#define USBD_SPEED_HIGH           3           /* usb 2.0 */
#define USBD_SPEED_WIRELESS       4           /* ���� (usb 2.5) */
#define USBD_SPEED_SUPER          5           /* usb 3.0 */

/** �����־*/
#define USBD_SHORT_NOT_OK      0x00000001     /* �̰������־*/
#define USBD_ZERO_PACKET       0x00000002     /* ��һ���̰�������һ�δ���*/
#define USBD_NO_INTERRUPT      0x00000004     /* ����Ҫ�жϣ����Ǵ���*/

/** �˵�����֧�ֱ�־*/
#define USBD_EP_SUPPORT_CTRL    0x01
#define USBD_EP_SUPPORT_ISO     0x02
#define USBD_EP_SUPPORT_BULK    0x04
#define USBD_EP_SUPPORT_INT     0x08
#define USBD_EP_SUPPORT_ALL     0x0F

/** �˵�������С����*/
#define USBD_EP_MPS_NO_LIMT     0x00

/** ���������ñ�־*/
#define USBD_CFG_SELF_POWERED     0x0200
#define USBD_CFG_REMOTE_WAKEUP    0x0400
#define USBD_CFG_MAX_POWER        0x01FF
#define USBD_CFG_MAX_POWER_MA(v)  (v & 0x1FF)

/** USB�豸������Ϣ*/
#define USBD_TASK_STACK_SIZE    2048
#define USBD_TASK_PRIO          0

struct usbd_event {
    usb_mutex_handle_t      lock;
    usb_sem_handle_t        semb;
    uint32_t                event;
};

/** USB�豸����ṹ��*/
struct usbd_trans {
    struct usbd_ep  *p_hw;
    void            *p_buf;
    void            *p_buf_dma;
    size_t           len;
    void            (*complete) (void *p_arg);
    void            *p_arg;
    size_t           act_len;            /* ʵ�ʴ�����ֽ���*/
    int              status;             /* ����״̬*/
    int              flag;
};

/** USB�豸�˵�*/
struct usbd_ep {
    uint8_t                 ep_addr;       /* �˵��ַ*/
    uint8_t                 type_support;  /* ֧������*/
    uint8_t                 cur_type;      /* ��ǰ����*/
    uint16_t                mps_limt;      /* ������С����*/
    uint16_t                cur_mps;       /* ��ǰ������С*/
    struct usb_list_head    node;          /* �˵�ڵ�*/
    usb_bool_t              enable;        /* ʹ�ܱ�־λ*/
    usb_bool_t              used;          /* ʹ�ñ�־λ*/
};

/** USB�豸�˵�0*/
struct usbd_ep0 {
    struct usbd       *p_usbd;
    struct usbd_ep    *p_hw;
    struct usbd_trans  req;
};

/** USB�豸������*/
struct usbd {
    struct usbd_obj         *p_obj;
    const struct usbd_cb    *p_cb;
    usb_task_handle_t        usbd_task;
    usb_mutex_handle_t       mutex;
    usb_sem_handle_t         start;
    usb_sem_handle_t         done;
    //aw_spinlock_isr_t             lock;
    usb_bool_t               run;
    uint8_t                  state;
    uint8_t                  resume_state;
    uint8_t                  speed;
    uint8_t                  addr;
    uint8_t                  mps0;
    struct usb_list_head     ep_list;
    struct usbd_ep0          ep0_in;
    struct usbd_ep0          ep0_out;
    usb_bool_t               need_sta;
    struct usb_list_head     node;
    struct usbd_event        event;
    struct usb_ctrlreq       setup;
    void                    *usr_priv;
};

/** ��������Ϣ*/
struct usbd_info {
    const char  *name;   /* ����������*/
    uint32_t     flag;   /* ��־*/
};

/** ����������������*/
struct usbd_drv {
    /* ��������λ*/
    void (*reset) (struct usbd *p_usbd);
    /* ����������*/
    usb_err_t (*run) (struct usbd *p_usbd);
    /* ������ֹͣ*/
    usb_err_t (*stop) (struct usbd *p_usbd);
    /* ��������*/
    usb_err_t (*transfer_req) (struct usbd       *p_usbd,
                               struct usbd_trans *p_trans);
    /* ��������*/
    usb_err_t (*transfer_cancel) (struct usbd       *p_usbd,
                                  struct usbd_trans *p_trans);
    /* �˵�ʹ��*/
    usb_err_t (*ep_enable) (struct usbd    *p_usbd,
                            struct usbd_ep *p_ep);
    /* �˵����*/
    usb_err_t (*ep_disable) (struct usbd    *p_usbd,
                             struct usbd_ep *p_ep);

    /* ��λ�˵㣬������ж˵��״̬�����ã�����֮�������ʹ��һ�ζ˵�*/
    usb_err_t (*ep_reset) (struct usbd    *p_usbd,
                           struct usbd_ep *p_ep);
    /* �����豸��ַ*/
    usb_err_t (*set_address) (struct usbd  *p_usbd,
                              uint8_t       addr);
    /* ��������*/
    usb_err_t (*set_config) (struct usbd  *p_usbd,
                             usb_bool_t    set);
    /* ��ȡ�����ö˵�״̬*/
    usb_err_t (*ep_status) (struct usbd    *p_usbd,
                            uint8_t         ep_addr,
                            usb_bool_t      is_set,
                            uint16_t       *status);
    /* ���û��ȡ�˵�����*/
    usb_err_t (*ep_feature) (struct usbd    *p_usbd,
                             uint8_t         ep_addr,
                             usb_bool_t      is_set);
    /* ���ѿ�����*/
    usb_err_t (*wakeup)(struct usbd *p_usbd);
    /* ������Ͽ� D+/D-�ϵĵ���*/
    usb_err_t (*pullup)(struct usbd *p_usbd, usb_bool_t on);
};

/** ���������ƺ�����*/
struct usbd_cb {
    /* ����һ������SETUP��*/
    int (*ctrl_setup) (struct usbd        *p_usbd,
                       struct usb_ctrlreq *p_setup,
                       void               *p_buf,
                       int                 buf_len);
    /* ���߸�λ*/
    void (*reset) (struct usbd *p_usbd);
    /* ���߹���*/
    void (*suspend) (struct usbd *p_usbd);
    /* ���߻ָ�*/
    void (*resume) (struct usbd  *p_usbd);
    /* ���߶Ͽ�*/
    void (*disconnect) (struct usbd *p_usbd);
};

/***************************************��������**************************************************/
/** ����USB�豸������˽������*/
usb_err_t usbd_set_priv(struct usbd *p_usbd, void *data);
/** ����һ��USB�豸������*/
int usbd_create (struct usbd *p_usbd);
/** ����USB�豸�������¼�*/
int usbd_event_handle (struct usbd  *p_usbd,
                       uint32_t      event,
                       void         *arg);
/** ע��USB�������˵�*/
int usbd_ep_register (struct usbd    *p_usbd,
                      struct usbd_ep *p_ep,
                      uint8_t         ep_addr,
                      uint8_t         type_support,
                      uint16_t        mps_limt);
/**
 * �˵�����
 *
 * @param p_usbd USB�豸������
 * @param addr   �˵��ַ
 * @param type   �˵�����
 * @param mps    �˵���������
 *
 * @return �ɹ����� USB�豸�˵�ṹ��
 */
 struct usbd_ep *usbd_ep_alloc (struct usbd  *p_usbd,
                                uint8_t       addr,
                                uint8_t       type,
                                uint16_t      mps);
/**
 * �ͷ�USB�豸�������˵�
 *
 * @param p_usbd USB�豸������
 * @param p_ep   Ҫ�ͷŵĶ˵�
 *
 * @return �ɹ�����USB_OK*/
int usbd_ep_free (struct usbd    *p_usbd,
                  struct usbd_ep *p_ep);
/**
 * ʹ���豸�˵�
 *
 * @param p_usbd USB�豸������
 * @param p_ep   Ҫ���ܵĶ˵�
 *
 * @return �ɹ�����USB_OK
 */
int usbd_ep_enable (struct usbd    *p_usbd,
                    struct usbd_ep *p_ep);
/**
 * �����豸�˵�
 *
 * @param p_usbd USB�豸������
 * @param p_ep   Ҫ���ܵĶ˵�
 *
 * @return �ɹ�����USB_OK
 */
int usbd_ep_disable (struct usbd    *p_usbd,
                     struct usbd_ep *p_ep);
/** ��λUSB�������˵�*/
int usbd_ep_reset (struct usbd    *p_usbd,
                   struct usbd_ep *p_ep);
/** ���ö˵�����*/
int usbd_ep_feature (struct usbd    *p_usbd,
                     struct usbd_ep *p_ep,
                     usb_bool_t      set);
/**
 * ͨ������Ѱ�Ҷ�Ӧ��USB�豸������
 *
 * @param name ����
 *
 * @return �ҵ�����USB�豸�������ṹ�壬�Ҳ�������NULL
 */
struct usbd *usbd_find_dev_controller (const char *name);
/**
 * ����USB�豸����
 *
 * @param p_ubd USB�豸������
 * @param p_obj USB�豸����
 * @param p_cb  USB�豸����������������
 *
 * @return �ɹ�����USB_OK
 */
int usbd_start (struct usbd          *p_usbd,
                struct usbd_obj      *p_obj,
                const struct usbd_cb *p_cb);
/** ֹͣUSB�豸������*/
int usbd_stop (struct usbd  *p_usbd);
/**
 * USB�豸����������
 *
 * @param p_usbd  USB�豸������
 * @param p_trans USB�豸����
 *
 * @return �ɹ�����USB_OK
 */
int usbd_trans_req (struct usbd        *p_usbd,
                    struct usbd_trans  *p_trans);
/** USB�豸����ȡ��*/
int usbd_trans_cancel (struct usbd        *p_usbd,
                       struct usbd_trans  *p_trans);
/** ��ȡUSB�豸����������*/
int usbd_attribute_get (struct usbd  *p_usbd,
                        uint32_t     *attr);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBD_H */

