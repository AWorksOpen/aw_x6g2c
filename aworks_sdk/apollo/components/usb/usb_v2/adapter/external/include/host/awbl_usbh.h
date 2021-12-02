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
#ifndef __AWBL_USBH_H
#define __AWBL_USBH_H
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"

#include "host/core/usbh.h"
#include "usb_common_def.h"
#include "list/usb_list.h"
#include "usb_adapter_inter.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* ͨ�������ȡUSB���ܽṹ��*/
#define __AWBL_USBH_FUN_DECL(p_fun, p_awdev) \
    struct awbl_usbh_function *p_fun = \
        (struct awbl_usbh_function *)AW_CONTAINER_OF( \
            p_awdev, struct awbl_usbh_function, awdev)
/* ͨ��������Ϣ��ȡUSB ����������Ϣ*/
#define __AWBL_USBH_FUN_DRV(p_drv, p_awdrv) \
    struct awbl_usbh_fundrv_info *p_drv = \
        (struct awbl_usbh_fundrv_info *)AW_CONTAINER_OF( \
            p_awdrv, struct awbl_usbh_fundrv_info, awdrv)

struct awbl_usbh_trp;

/** USB �����ṹ��*/
struct awbl_usbh {
    struct awbl_busctlr         super;    /* ���߿�����*/
    AW_MUTEX_DECL(mutex);                 /* ����һ��������*/
    struct usbh                *hc;
    struct aw_delayed_work      rh_work;
    struct awbl_usbh           *p_next;
};

/** USB ������Ϣ*/
struct awbl_usbh_info {
    uint8_t     bus_index;
};

/* �豸�ض���������Ϣ*/
struct awbl_usbh_dev_spec_config_info{
    uint16_t pid;
    uint16_t vid;
    uint16_t config_num;   /* ���ñ��*/
};

#define AWBL_USBH_FUNDRV_INFO_END  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/** USB����������Ϣƥ���*/
struct awbl_usbh_fundrv_info_tab {
    uint16_t                    flags;                 /* ƥ���־*/
#define AWBL_USBH_FUNDRV_MATCH_VENDOR          0x0001  /* ƥ��VID*/
#define AWBL_USBH_FUNDRV_MATCH_PRODUCT         0x0002  /* ƥ��PID*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_CLASS       0x0010  /* ƥ���豸�����*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS    0x0020  /* ƥ���豸�������*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL    0x0040  /* ƥ���豸Э�����*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_CLASS       0x0080  /* ƥ��ӿ������*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS    0x0100  /* ƥ��ӿ��������*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL    0x0200  /* ƥ��ӿ�Э�����*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER      0x0400  /* ƥ���豸�ӿ�����*/
    uint16_t                    vid;
    uint16_t                    pid;

    uint8_t                     dclss;
    uint8_t                     dsclss;
    uint8_t                     dproto;

    uint8_t                     iclss;
    uint8_t                     isclss;
    uint8_t                     iproto;
    uint8_t                     inum;
};

/** USB �豸������Ϣ*/
struct awbl_usbh_fundrv_info {
    struct awbl_drvinfo                     awdrv;
    const struct awbl_usbh_fundrv_info_tab *p_tab;
};

/** USB���ܽṹ��*/
struct awbl_usbh_function {
    struct awbl_dev       awdev;       /* �����豸��*/
    struct awbl_devhcf    devhcf;      /* �豸Ӳ����Ϣ*/
    struct usbh_function *usb_func;    /* USB�ڲ����ܽṹ��*/
};

aw_err_t awbl_usbh_lib_init (int stk_size, int stk_prio);
aw_err_t awbl_usbh_host_list_init (void);
/**
 * ����һ��USB����
 *
 * @param p_hc   USB�����ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_err_t awbl_usbh_host_create (struct awbl_usbh *p_hc);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_H */
