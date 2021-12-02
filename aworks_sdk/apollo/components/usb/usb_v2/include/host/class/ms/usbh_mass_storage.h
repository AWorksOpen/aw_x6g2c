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
#ifndef __USBH_MASS_STORAGE_H
#define __USBH_MASS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "usb_adapter_inter.h"
#include "refcnt/usb_refcnt.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

/* �������洢�豸�ӿ�Э�����*/
#define USBH_MS_PR_CBI               0x00    /* ����/����/�ж�*/
#define USBH_MS_PR_CB                0x01    /* ����/����*/
#define USBH_MS_PR_BULK              0x50    /* ����*/
#define USBH_MS_PR_ANY               0xFF    /* ������Э��*/

/* �������洢�豸�豸����*/
#define USBH_MS_SC_RBC               0x01    /* flash �豸*/
#define USBH_MS_SC_8020              0x02    /* CD's DVD's*/
#define USBH_MS_SC_QIC               0x03    /* �Ŵ�*/
#define USBH_MS_SC_UFI               0x04    /* Floppy*/
#define USBH_MS_SC_8070              0x05    /* Floppies(mostly)*/
#define USBH_MS_SC_SCSI              0x06    /* SCSI �豸*/
#define USBH_MS_SC_RAMD              0x13    /* Ram �����豸*/


/* �������洢�豸��ṹ��*/
struct ums_lib{
    struct usb_list_head    ums_lib_dev_list;       /* ���������豸����*/
    usb_bool_t              is_lib_init;            /* �Ƿ��ʼ����*/
    usb_bool_t              is_lib_uniniting;       /* �Ƿ��Ƴ���*/
    usb_mutex_handle_t      ums_lib_lock;           /* ������*/
    uint8_t                 ums_lib_dev_num;        /* ��ǰ�����豸������*/
    int                     refcnt;                 /* ���ü���*/
    struct usbh_mem_record  ums_mem_record;         /* �ڴ��¼*/
};

/** �������洢�豸�߼���Ԫ�ṹ�� */
struct usbh_ms_lun {
    struct usbh_mass_storage   *p_ms;      /* ��صĴ������洢�豸*/
    char                        name[32];  /* �߼���Ԫ����*/
    uint8_t                     lun;       /* �߼���Ԫ��*/
    uint32_t                    nblks;     /* ������*/
    uint32_t                    bsize;     /* ���С*/
    usb_bool_t                  wp;        /* �Ƿ���д����*/
    void                       *p_buf;     /* ���ݻ���*/
    usb_bool_t                  is_init;   /* �Ƿ��ʼ���ɹ�*/
    usb_bool_t                  is_used;   /* �Ƿ�ʹ��*/
    void                       *user_priv; /* �û�˽������*/
};

struct usbh_ms_sclass {
    uint8_t    id;


    usb_err_t (*pfn_init) (struct usbh_ms_lun *p_lun);

    usb_err_t (*pfn_read) (struct usbh_ms_lun *p_lun,
                           uint32_t            blk,
                           uint32_t            nblks,
                           void               *p_buf);

    usb_err_t (*pfn_write) (struct usbh_ms_lun *p_lun,
                            uint32_t            blk,
                            uint32_t            nblks,
                            void               *p_data);

};

/** USB�����������洢�豸*/
struct usbh_mass_storage {
    struct usbh_function   *p_fun;            /* ��صĹ��ܽӿ�*/
    usb_mutex_handle_t      lock;
    struct usbh_endpoint   *p_in;
    struct usbh_endpoint   *p_out;
    struct usbh_endpoint   *p_intr;           /* �ж϶˵�*/

    struct usbh_ms_sclass  *p_sclass;         /* �������洢�豸���*/

    int                     id;               /* �������洢�豸ID*/
    uint8_t                 nluns;            /* �߼���������*/
    int                     ref;              /* ���ü���*/
    uint8_t                 pro;              /* �ӿ�Э��*/
    int                     tag;
    struct usb_list_head    node;             /* �������洢�豸�ڵ�*/
    struct usbh_ms_lun     *luns;             /* �߼�����*/
    usb_bool_t              removed;          /* �Ƴ���־*/
    void                   *p_cbw;            /* ������*/
    void                   *p_csw;            /* ����״̬��*/
};

/*******************************************��������**********************************************/
/** ��ʼ���������洢�豸��*/
usb_err_t usbh_ms_lib_init(void);
/** ��ȡ��ǰ���ڵĴ������洢�豸����*/
int usbh_ms_dev_num_get(void);
/**
 * ����PID��VID��ȡ�������洢�豸
 *
 * @param pid ָ��PID,��ΪUSBDEV_IGONRE_PID�����ƥ��pid
 * @param vid ָ��VID,��ΪUSBDEV_IGONRE_VID�����ƥ��vid
 * @param idx ������ָ��Ҫ��ȡ�ڼ����������洢�豸(��СΪ1)
 *
 * @return �ɹ����ش������洢�豸�ṹ��
 */
struct usbh_mass_storage *usbh_ms_dev_open(uint16_t pid, uint16_t vid, uint8_t idx);
/** �ͷ�USB�������洢�豸*/
usb_err_t usbh_ms_dev_close(struct usbh_mass_storage *p_ms);
/** ���߼�������ȡ�������洢�豸*/
struct usbh_mass_storage *usbh_ms_dev_get_from_lun(struct usbh_ms_lun *p_lun);

/** ���������洢�豸�Ƿ��Ѿ��Ƴ�*/
usb_bool_t usbh_ms_dev_removed_check(struct usbh_mass_storage *p_ms);
/**
 * ��ȡ���õ��߼���Ԫ
 *
 * @param p_ms     �������洢�豸
 * @param luns_num �߼���Ԫ��
 *
 * @return �ɹ������߼���Ԫ�ṹ��
 */
struct usbh_ms_lun *usbh_ms_valid_lun_get(struct usbh_mass_storage *p_ms, uint8_t luns_num);
/**
 * �ͷſ��õ��߼���Ԫ
 *
 * @param p_lun Ҫ�ͷŵ��߼���Ԫ
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usbh_ms_valid_lun_put(struct usbh_ms_lun *p_lun);
/** ��ȡ����߼���Ԫ����*/
usb_err_t usbh_ms_max_lun_get(struct usbh_mass_storage *p_ms, uint8_t *nluns);
/** ��ȡ�豸��ǰ���߼���Ԫ����*/
int usbh_ms_lun_num_get(struct usbh_mass_storage *p_ms);

/** ��ȡ�������С*/
uint32_t usbh_ms_lun_bsize_get(struct usbh_ms_lun *p_lun);
/** ��ȡ����������*/
uint32_t usbh_ms_lun_nblks_get(struct usbh_ms_lun *p_lun);

/** ���÷�������*/
usb_err_t usbh_ms_lun_name_set(struct usbh_ms_lun *p_lun, char *name_new);
/** ��ȡ��������*/
char *usbh_ms_lun_name_get(struct usbh_ms_lun *p_lun);

/** �������Ƿ�д����*/
usb_bool_t usbh_ms_lun_is_wp(struct usbh_ms_lun *p_lun);

/** ��ȡ������������*/
usb_err_t usbh_ms_lun_databuf_get(struct usbh_ms_lun *p_lun, void *dest_buf, uint32_t size);
/** ��������д����*/
usb_err_t usbh_ms_lun_databuf_set(struct usbh_ms_lun *p_lun, void *src_buf, uint32_t size);

/** �����û�˽������*/
usb_err_t usbh_ms_lun_usrdata_set(struct usbh_ms_lun *p_lun, void *usr_priv);
/** ��ȡ�û�˽������*/
void* usbh_ms_lun_usrdata_get(struct usbh_ms_lun *p_lun);


/**
 * ����һ���������洢�豸
 *
 * @param p_ufun       ��ص�USB���ܽӿ�
 * @param lun_buf_size U�����ݽ������棬Ӱ��U�̶�д���ܣ��Ƽ�16k
 *
 * @return �ɹ����ش������洢�豸�ṹ��
 */
struct usbh_mass_storage *usbh_ms_dev_create(struct usbh_function *p_ufun,
                                             uint32_t              lun_buf_size);
/** �������洢�豸����*/
usb_err_t usbh_ms_dev_destory(struct usbh_mass_storage *p_ms);

/**
 * �������洢�豸�������
 *
 * @param p_lun   �߼������ṹ��
 * @param blk_no  ��ʼ����
 * @param nblks   ������
 * @param p_buf   ���ݻ���(ΪNULL�����߼������Ļ���)
 *
 * @return �ɹ�����ʵ�ʶ�д�ɹ����ֽ���
 */
int usbh_ms_blk_read(struct usbh_ms_lun *p_lun,
                     uint32_t            blk_no,
                     uint32_t            nblks,
                     void               *p_buf);
/**
 * �������洢�豸��д����
 *
 * @param p_lun   �߼������ṹ��
 * @param blk_no  ��ʼ����
 * @param nblks   ������
 * @param p_buf   ���ݻ���(ΪNULL�����߼������Ļ���)
 *
 * @return �ɹ�����ʵ�ʶ�д�ɹ����ֽ���
 */
int usbh_ms_blk_write(struct usbh_ms_lun *p_lun,
                      uint32_t            blk_no,
                      uint32_t            nblks,
                      void               *p_buf);

/** ��ȡUSB�������洢�����ڴ��¼*/
usb_err_t ums_lib_mem_record_get(struct usbh_mem_record *mem_record);
/**************************************************************************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

