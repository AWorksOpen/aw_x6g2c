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
#include "apollo.h"
#include "aw_int.h"
#include "aw_sem.h"
#include "host/awbl_usbh.h"
#include "host/core/usbh.h"
#include "string.h"

/* USB��������*/
aw_local struct awbl_usbh *__g_hs_list = NULL;

#define __AWBL_USBH_LOCK_TIMEOUT    5000
/* ��ȡUSB������Ϣ*/
#define __USBH_TO_INFO(p_hc) \
    (struct awbl_usbh_info *)AWBL_DEVINFO_GET(&p_hc->super.super)

/** ��ʼ��USB��������*/
aw_err_t awbl_usbh_host_list_init (void)
{
    __g_hs_list = NULL;
    return AW_OK;
}

/** Ѱ��USB����*/
aw_local struct awbl_usbh *__hs_find (struct awbl_usbh *p_hc)
{
    struct awbl_usbh      *p_tmp;
    struct awbl_usbh_info *p_new, *p_exist;

    p_tmp = __g_hs_list;
    p_new = __USBH_TO_INFO(p_hc);

    AW_INT_CPU_LOCK_DECL( key);

    AW_INT_CPU_LOCK(key);
    while (p_tmp) {
        if (p_tmp == p_hc) {
            break;
        }

        p_exist = __USBH_TO_INFO(p_tmp);
        /* ������������Ƿ���ͬ*/
        if (p_exist->bus_index == p_new->bus_index) {
            break;
        }

        p_tmp = p_tmp->p_next;
    }
    AW_INT_CPU_UNLOCK(key);
    return p_tmp;
}

/** ���USB����*/
aw_local void __hs_add (struct awbl_usbh *p_hc)
{
    AW_INT_CPU_LOCK_DECL( key);

    AW_INT_CPU_LOCK(key);
    p_hc->p_next = __g_hs_list;
    __g_hs_list  = p_hc;
    AW_INT_CPU_UNLOCK(key);
}

/** ����������ʱ����*/
aw_local void __rh_work (void *p_arg)
{
    struct awbl_usbh *p_hc = (struct awbl_usbh *)p_arg;
    uint16_t          change;
    aw_err_t          ret = AW_OK;

    AW_MUTEX_LOCK(p_hc->mutex, __AWBL_USBH_LOCK_TIMEOUT);
    if (ret == AW_OK) {
        /* �����������ͱ仯*/
        if ((usbh_rh_check(p_hc->hc, &change) == AW_OK) &&
                (change != 0)) {
            *(uint16_t *)p_hc->hc->rh.trp.p_data = change;
            /* ���ø�����������������ɻص�����*/
            if (p_hc->hc->rh.trp.pfn_done) {
                p_hc->hc->rh.trp.pfn_done(p_hc->hc->rh.trp.p_arg);
            }
        }
        AW_MUTEX_UNLOCK(p_hc->mutex);
    }
    /* �ٴ�������ʱ����*/
    aw_delayed_work_start(&p_hc->rh_work, 500);
}

/**
 * ����һ��USB����
 *
 * @param p_hc   USB�����ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_err_t awbl_usbh_host_create (struct awbl_usbh *p_hc){
    aw_err_t ret;
    struct awbl_usbh_info *p_info;

    /* ͨ���豸ʵ����ȡ�豸��Ϣ*/
    p_info = (struct awbl_usbh_info *)AWBL_DEVINFO_GET(
                                    &p_hc->super.super);

    /* �Ϸ��Լ��*/
    if ((p_hc == NULL) || (p_info == NULL)) {
        return -AW_EINVAL;
    }
    /* ���USB�����Ƿ��Ѿ�����*/
    if (__hs_find(p_hc)) {
        return -AW_EEXIST;
    }
    /* ��ʼ��������*/
    if(AW_MUTEX_INIT(p_hc->mutex, AW_SEM_Q_PRIORITY) == NULL){
        return -AW_ENOMEM;
    }

    /* ��AWorks�������� */
    ret = awbl_bus_announce((struct awbl_busctlr *)p_hc, AWBL_BUSID_USBH);
    if (ret != AW_OK) {
        return ret;
    }
    /* ��ʼ��һ����ʱ����*/
    aw_delayed_work_init(&p_hc->rh_work, __rh_work, p_hc);
    /* ����һ��USB����*/
    p_hc->hc = usbh_create(p_info->bus_index);
    if(p_hc->hc == NULL){
        return -AW_ENOMEM;
    }
    /* ����USB����˽������*/
    ret = usbh_usrdata_set(p_hc->hc, p_hc);
    if (ret != AW_OK) {
        return ret;
    }
    /* ���USB����*/
    __hs_add(p_hc);
    /* ����һ����ʱ����*/
    aw_delayed_work_start(&p_hc->rh_work, 500);
    return AW_OK;
}
