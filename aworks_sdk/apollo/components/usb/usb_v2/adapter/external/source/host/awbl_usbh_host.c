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

/* USB主机链表*/
aw_local struct awbl_usbh *__g_hs_list = NULL;

#define __AWBL_USBH_LOCK_TIMEOUT    5000
/* 获取USB主机信息*/
#define __USBH_TO_INFO(p_hc) \
    (struct awbl_usbh_info *)AWBL_DEVINFO_GET(&p_hc->super.super)

/** 初始化USB主机链表*/
aw_err_t awbl_usbh_host_list_init (void)
{
    __g_hs_list = NULL;
    return AW_OK;
}

/** 寻找USB主机*/
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
        /* 检查总线索引是否相同*/
        if (p_exist->bus_index == p_new->bus_index) {
            break;
        }

        p_tmp = p_tmp->p_next;
    }
    AW_INT_CPU_UNLOCK(key);
    return p_tmp;
}

/** 添加USB主机*/
aw_local void __hs_add (struct awbl_usbh *p_hc)
{
    AW_INT_CPU_LOCK_DECL( key);

    AW_INT_CPU_LOCK(key);
    p_hc->p_next = __g_hs_list;
    __g_hs_list  = p_hc;
    AW_INT_CPU_UNLOCK(key);
}

/** 根集线器延时工作*/
aw_local void __rh_work (void *p_arg)
{
    struct awbl_usbh *p_hc = (struct awbl_usbh *)p_arg;
    uint16_t          change;
    aw_err_t          ret = AW_OK;

    AW_MUTEX_LOCK(p_hc->mutex, __AWBL_USBH_LOCK_TIMEOUT);
    if (ret == AW_OK) {
        /* 根集线器发送变化*/
        if ((usbh_rh_check(p_hc->hc, &change) == AW_OK) &&
                (change != 0)) {
            *(uint16_t *)p_hc->hc->rh.trp.p_data = change;
            /* 调用根集线器传输请求完成回调函数*/
            if (p_hc->hc->rh.trp.pfn_done) {
                p_hc->hc->rh.trp.pfn_done(p_hc->hc->rh.trp.p_arg);
            }
        }
        AW_MUTEX_UNLOCK(p_hc->mutex);
    }
    /* 再次启动延时工作*/
    aw_delayed_work_start(&p_hc->rh_work, 500);
}

/**
 * 创建一个USB主机
 *
 * @param p_hc   USB主机结构体
 *
 * @return 成功返回AW_OK
 */
aw_err_t awbl_usbh_host_create (struct awbl_usbh *p_hc){
    aw_err_t ret;
    struct awbl_usbh_info *p_info;

    /* 通过设备实例获取设备信息*/
    p_info = (struct awbl_usbh_info *)AWBL_DEVINFO_GET(
                                    &p_hc->super.super);

    /* 合法性检查*/
    if ((p_hc == NULL) || (p_info == NULL)) {
        return -AW_EINVAL;
    }
    /* 检查USB主机是否已经存在*/
    if (__hs_find(p_hc)) {
        return -AW_EEXIST;
    }
    /* 初始化互斥锁*/
    if(AW_MUTEX_INIT(p_hc->mutex, AW_SEM_Q_PRIORITY) == NULL){
        return -AW_ENOMEM;
    }

    /* 向AWorks声明总线 */
    ret = awbl_bus_announce((struct awbl_busctlr *)p_hc, AWBL_BUSID_USBH);
    if (ret != AW_OK) {
        return ret;
    }
    /* 初始化一个延时工作*/
    aw_delayed_work_init(&p_hc->rh_work, __rh_work, p_hc);
    /* 创建一个USB主机*/
    p_hc->hc = usbh_create(p_info->bus_index);
    if(p_hc->hc == NULL){
        return -AW_ENOMEM;
    }
    /* 设置USB主机私有数据*/
    ret = usbh_usrdata_set(p_hc->hc, p_hc);
    if (ret != AW_OK) {
        return ret;
    }
    /* 添加USB主机*/
    __hs_add(p_hc);
    /* 启动一个延时工作*/
    aw_delayed_work_start(&p_hc->rh_work, 500);
    return AW_OK;
}
