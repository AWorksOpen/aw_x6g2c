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


#ifndef __USBD_OBJ_H
#define __USBD_OBJ_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "usb_adapter_inter.h"
#include "device/core/usbd.h"
#include "usb_errno.h"
#include "list/usb_list.h"

/** \brief �ܵ������� */
#define USB_PIPE_DIR_IN      USB_DIR_IN       /**< \brief ������*/
#define USB_PIPE_DIR_OUT     USB_DIR_OUT      /**< \brief ���豸*/

/** \brief �ܵ����Ͷ��� */
#define USB_PIPE_TYPE_CTRL   USB_EP_TYPE_CTRL     /**< \brief ���ƹܵ� */
#define USB_PIPE_TYPE_ISO    USB_EP_TYPE_ISO      /**< \brief ��ʱ�ܵ� */
#define USB_PIPE_TYPE_BULK   USB_EP_TYPE_BULK     /**< \brief �����ܵ� */
#define USB_PIPE_TYPE_INT    USB_EP_TYPE_INT      /**< \brief �жϹܵ� */

/** \brief USBD����ṹ���� */
struct usbd_obj_trans {
    struct usbd_obj_pipe *p_pipe;    /**< \brief �ܵ�ָ�� */
    struct usbd_trans     tr;        /**< \brief ����ṹ */
};

/** \brief USBD����ܵ���Ϣ�ṹ���� */
struct usbd_obj_pipe_info {
    uint8_t  num;       /**< \brief �˵��ַ    0Ϊ�Զ����� */
    uint8_t  dir;       /**< \brief �˵㷽��    1:IN 0:OUT */
    uint8_t  attr;      /**< \brief �������ͺͲ�����Ϣ */
    uint8_t  inter;     /**< \brief �жϺ͵�ʱ�˵�ķ���ʱ�� */
    uint16_t mps;       /**< \brief �˵�ÿ������������������ֽ��� */
};

/** \brief USBD����ܵ����Ͷ��� */
struct usbd_obj_pipe {
    struct usbd_ep                  *p_hw;      /**< \brief �˵�ָ�� */
    const struct usbd_obj_pipe_info *p_info;    /**< \brief �ܵ���Ϣָ�� */
    struct usbd_obj_fun             *p_fun;     /**< \brief �ӿ�ָ�� */
    uint8_t                          addr;      /**< \brief �ܵ���ַ */
    struct usb_list_head             node;      /**< \brief �ڵ� */
    usb_sem_handle_t                 sync;
};

/** \brief �ӿ�����������*/
struct usbd_obj_iad {
    uint8_t    first_if;      /* ��һ���ӿڱ��*/
    uint8_t    if_count;      /* �ӿ�����*/
    uint8_t    fun_class;     /* �ӿ���*/
    uint8_t    fun_subclass;  /* �ӿ�����*/
    uint8_t    fun_protocol;  /* �ӿ�Э��*/
    uint8_t    ifunc;
    usb_bool_t used;         /* �Ƿ�ʹ��*/
};

/** \brief USBD����ӿ���Ϣ */
struct usbd_obj_fun_info {
    uint8_t       clss;         /**< \brief ����� */
    uint8_t       sub_clss;     /**< \brief ������� */
    uint8_t       protocol;     /**< \brief Э����� */

    /** \brief ������ûص� */
    int (*pfn_alt_set) (struct usbd_obj_fun *p_fun,
                        usb_bool_t           set);

    /** \brief setup���ƴ���ص� */
    int (*pfn_ctrl_setup) (struct usbd_obj_fun *p_fun,
                           struct usb_ctrlreq  *p_setup,
                           void                *p_buf,
                           int                  buf_len);
};

/** \brief USBD����ӿڽṹ���� */
struct usbd_obj_fun {
    struct usbd_obj                *p_obj;      /**< \brief USBD�豸ָ�� */
    const char                     *name;       /**< \brief �ӿ����� */
    const struct usbd_obj_fun_info *p_info;     /**< \brief �ӿ� ��Ϣ */
    uint8_t                         num;        /**< \brief �ӿڱ�� */
    uint8_t                         alt;        /**< \brief ������ñ�� */
    uint8_t                         num_alt;    /**< \brief ֻ������0��Ч*/
    struct usbd_obj_fun            *p_next_alt; /**< \brief ���һ��ΪNULL*/
    usb_bool_t                      need_iad;
    uint8_t                        *p_spec_des; /**< \brief ���Զ���������*/
    uint8_t                         spec_size;  /**< \brief ���Զ�����������С*/
    usb_bool_t                      set;        /**< \brief �Ƿ����� */
    uint8_t                         neps;       /**< \brief �˵����� */
    struct usb_list_head            pipes;      /**< \brief �ܵ����� */
    struct usb_list_head            node;       /**< \brief �ڵ� */
    uint8_t                         strid;      /**< \brief �ַ������� */
};

/* �ӿ������������������*/
#define USBD_MAX_IAD_NUM     10

/** \brief USBD�������ýṹ���� */
struct usbd_obj_cfg {
    struct usbd_obj       *p_obj;                     /**< \brief USBD�豸ָ�� */
    struct usbd_obj_iad    iads[USBD_MAX_IAD_NUM];    /**< \brief �ӿ�����������*/
    struct usb_list_head   itfs;                      /**< \brief �ӿ����� */
    uint8_t                nitfs;                     /**< \brief �ӿ����� */
    uint8_t                value;                     /**< \brief ���ñ�� */
    struct usb_list_head   node;                      /**< \brief �ڵ� */
};

/** \brief USBD������Ϣ�ṹ���� */
struct usbd_obj_info {
    uint8_t          clss;          /**< \brief ����� */
    uint8_t          sub_clss;      /**< \brief ������� */
    uint8_t          protocol;      /**< \brief Э����� */
    uint8_t          mps0;          /**< \brief �˵�0�����ߴ� */
    uint16_t         bcd;           /**< \brief �豸�汾�� */
    uint16_t         vid;           /**< \brief ����ID */
    uint16_t         pid;           /**< \brief ��ƷID */
    const char      *manufacturer;  /**< \brief �������ַ������� */
    const char      *product;       /**< \brief ��Ʒ�ַ������� */
    const char      *serial;        /**< \brief ���к��ַ������� */
};

/** \brief USBD����ṹ���� */
struct usbd_obj {
    struct usbd                *p_usbd;     /**< \brief USBD�豸ָ�� */
    const struct usbd_obj_info *p_info;     /**< \brief �豸��Ϣ */
    struct usbd_obj_cfg        *p_cur_cfg;  /**< \brief ��ǰ����ָ�� */
    struct usbd_obj_cfg         def_cfg;    /**< \brief Ĭ������ */
    struct usb_list_head        cfgs;       /**< \brief �������� */
    uint8_t                     ncfgs;      /**< \brief �������� */
    const struct usbd_obj_cb   *p_cb;       /**< \brief �豸��ص������б� */
    usb_mutex_handle_t          lock;
    uint8_t                     status;     /**< \brief �豸״̬ */
    uint8_t                     strid_mf;   /**< \brief �����ַ������� */
    uint8_t                     strid_pd;   /**< \brief ��Ʒ�ַ������� */
    uint8_t                     strid_sn;   /**< \brief ���к��ַ������� */
};

/** \brief USB�豸��ص������б� */
struct usbd_obj_cb {
    /** \brief ���������ӵ��豸 */
    int (*pfn_link) (struct usbd_obj *p_obj);
    /** \brief ���������豸�Ͽ����� */
    int (*pfn_unlink) (struct usbd_obj *p_obj);
    /** \brief ���յ�SETUP�� */
    int (*pfn_ctrl_setup) (struct usbd_obj    *p_obj,
                           struct usb_ctrlreq *p_setup,
                           void               *p_buf,
                           int                 buf_len);
    /** \brief ���û�������� */
    int (*pfn_config_set) (struct usbd_obj      *p_obj,
                           struct usbd_obj_cfg  *p_cfg,
                           usb_bool_t        set);
    /** \brief ���߹��� */
    void (*pfn_suspend) (struct usbd_obj *p_obj);
    /** \brief ���߻ָ� */
    void (*pfn_resume) (struct usbd_obj  *p_obj);
    /** \brief ���߶Ͽ����� */
    void (*pfn_disconnect) (struct usbd_obj *p_obj);
};

/***************************************��������**************************************************/
/**
 * \brief USB device�����ʼ������
 *
 * @param p_obj      USBD����ָ��
 * @param p_info     USBD�豸��Ϣ�ṹָ��
 * @param p_cb       USBD�ص������б�ָ��
 * @param controller USB device����������
 *
 * @return USB_OK    ��ʼ���ɹ�
 */
int usbd_obj_init (struct usbd_obj            *p_obj,
                   const struct usbd_obj_info *p_info,
                   const struct usbd_obj_cb   *p_cb,
                   const char                 *controller);
/**
 * ��ʼ���ӿڹ���
 *
 * @param p_fun  USB�ӿڽṹ
 * @param name   �ӿ�����
 * @param p_info USB�ӿ���Ϣ
 *
 * @return �ɹ�����USB_OK
 */
int usbd_obj_fun_init (struct usbd_obj_fun            *p_fun,
                       const char                     *name,
                       const struct usbd_obj_fun_info *p_info);
/**
 * ��ӹ��ܽӿ��ض�������
 *
 * @param p_fun  USB���ܽӿڽṹ��
 * @param p_desc �ض�������
 * @param size   �ض���������С
 *
 * @return �ɹ�����USB_OK
 */
int usbd_fun_spec_desc_add (struct usbd_obj_fun *p_fun,
                            uint8_t             *p_desc,
                            uint8_t              size);
/**
 * ����һ���ӿ�����������
 *
 * @param p_fun     USB�ӿڹ��ܽṹ��
 * @param cfg_value ���ñ��(0��ʾĬ�ϵ�һ������)
 * @param count     �����Ľӿ�����
 *
 * @return �ɹ�����USB_OK
 */
int usbd_fun_iad_create(struct usbd_obj_fun *p_fun,
                        uint8_t              cfg_value,
                        uint8_t              count);
/** ɾ��USB�豸�ӿڹ���*/
int usbd_fun_del (struct usbd_obj     *p_obj,
                  uint8_t              value,
                  struct usbd_obj_fun *p_fun);
/**
 * ����USB�ܵ�
 *
 * @param p_fun  USB�豸�ӿڽṹ��
 * @param p_pipe USB�豸�ܵ�����
 * @param p_info USB�ܵ���Ϣ�ṹ
 *
 * @return �ɹ�����USB_OK
 */
int usbd_obj_pipe_create (struct usbd_obj_fun              *p_fun,
                          struct usbd_obj_pipe             *p_pipe,
                          const struct usbd_obj_pipe_info  *p_info);
/**
 * ��USB�ӿ���ӵ�USB�豸����
 *
 * @param p_obj USB�豸����
 * @param value ���ñ��(0��ʾʹ��USB�����Ĭ�����ã�ÿ��USB������һ��Ĭ������)
 * @param p_fun USB�ӿڽṹ��
 *
 * @return �ɹ�����USB_OK
 */
int usbd_obj_fun_add (struct usbd_obj     *p_obj,
                      uint8_t              value,
                      struct usbd_obj_fun *p_fun);
/**
 * ����USB�ܵ�
 *
 * @param p_fun  USB�豸�ӿڽṹ��
 * @param p_pipe USB�豸�ܵ�����
 *
 * @return �ɹ�����USB_OK
 */
int usbd_obj_pipe_destory (struct usbd_obj_fun *p_fun,
                           struct usbd_obj_pipe *p_pipe);
/**
 * ����USB�豸����
 *
 * @param p_obj USB�豸����
 *
 * @return �ɹ�����USB_OK
 */
int usbd_obj_start (struct usbd_obj *p_obj);
/**
 * ��䴫������
 *
 * @param p_trans  ��������ṹ��
 * @param p_pipe   ���д���Ĺܵ�
 * @param p_buf    ���ݻ���
 * @param len      ���ݳ���
 * @param flag     ��־
 * @param complete ������ɻص�����
 * @param p_arg    ������ɻص���������
 */
void usbd_fill_trans (struct usbd_obj_trans *p_trans,
                      struct usbd_obj_pipe  *p_pipe,
                      void                  *p_buf,
                      size_t                 len,
                      int                    flag,
                      void                 (*complete) (void *p_arg),
                      void                  *p_arg);
/**
 * USB�豸�첽���亯��
 *
 * @param p_obj   USB�豸����
 * @param p_trans USB��������ṹ��
 *
 * @return �ɹ�����USB_OK
 */
int usbd_trans_async (struct usbd_obj        *p_obj,
                      struct usbd_obj_trans  *p_trans);
/**
 * USB�豸ͬ�����亯��
 *
 * @param p_obj   USB�豸����
 * @param p_pipe  ʹ�õ�USB�ܵ�
 * @param p_buf   ���ݻ���
 * @param len     ���ݳ���
 * @param flag    ��־
 * @param timeout ��ʱʱ��
 *
 * @return ����ɹ����ش�������ݳ��ȣ�����ʧ�ܷ��ش������
 */
int usbd_trans_sync (struct usbd_obj      *p_obj,
                     struct usbd_obj_pipe *p_pipe,
                     void                 *p_buf,
                     size_t                len,
                     int                   flag,
                     int                   timeout);
/**
 * ���USB�豸���ܽӿ��Ƿ����
 *
 * @param p_fun Ҫ����USB�豸���ܽӿ�
 *
 * @return ���÷���USB_TRUE�������÷���USB_FALSE
 */
usb_bool_t usbd_obj_fun_valid (struct usbd_obj_fun *p_fun);
/** ֹͣUSB�豸����ܵ�*/
int usbd_obj_pipe_stall (struct usbd_obj       *p_obj,
                         struct usbd_obj_pipe  *p_pipe,
                         usb_bool_t             set);
/** ��λUSB�豸����ܵ�*/
int usbd_obj_pipe_reset (struct usbd_obj       *p_obj,
                         struct usbd_obj_pipe  *p_pipe);
/**********************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __USBD_OBJ_H */
