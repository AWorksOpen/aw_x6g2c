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
#include "usb_refcnt.h"

/* 引用计数互斥锁*/
static usb_mutex_handle_t   __g_usb_refcnt_lock   = NULL;
/* 是否初始化完成互斥锁标志*/
static usb_bool_t           __g_is_lock_init      = USB_FALSE;

/**
 * 初始化引用计数
 *
 * @param  p_ref 引用计数对象指针
 */
void usb_refcnt_init (int *p_ref){

    if(__g_is_lock_init == USB_FALSE){
        __g_usb_refcnt_lock = usb_mutex_create();
        if(__g_usb_refcnt_lock == NULL){
            return;
        }
        __g_is_lock_init = USB_TRUE;
    }

    if(p_ref == NULL){
        return;
    }

    *p_ref = 1;
}

/**
 * 检查引用计数是否有效。
 *
 * @param p_ref 引用计数对象指针
 */
usb_bool_t usb_refcnt_valid (int  *p_ref){
    if(p_ref == NULL){
        return USB_FALSE;
    }

    return (*p_ref != 0);
}

/**
 * 引用计数加。
 *
 * @param p_ref 引用计数对象指针
 *
 * @return 成功返回USB_OK
 */
usb_err_t usb_refcnt_get (int *p_ref)
{
    if((p_ref == NULL) || (__g_is_lock_init == USB_FALSE)){
        return -USB_EPERM;
    }

    usb_mutex_lock(__g_usb_refcnt_lock, USB_WAIT_FOREVER);
    if (*p_ref == 0) {
        usb_mutex_unlock(__g_usb_refcnt_lock);
        return -USB_EPERM;
    }
    *p_ref += 1;
    usb_mutex_unlock(__g_usb_refcnt_lock);

    return USB_OK;
}

/**
 * 引用计数减
 *
 * @param  p_ref       引用计数对象指针
 * @param  pfn_release 回调函数，引用计数释放后执行
 *
 * @return 成功返回USB_OK
 */
usb_err_t usb_refcnt_put (int   *p_ref,
                          void (*pfn_release) (int *p_ref))
{
    if((p_ref == NULL) || (__g_is_lock_init == USB_FALSE)){
        return -USB_EPERM;
    }

    usb_mutex_lock(__g_usb_refcnt_lock, USB_WAIT_FOREVER);
    if (*p_ref == 0) {
        usb_mutex_unlock(__g_usb_refcnt_lock);
        return -USB_EPERM;
    }
    *p_ref -= 1;
    if ((*p_ref == 0) && (pfn_release)) {
        usb_mutex_unlock(__g_usb_refcnt_lock);
        pfn_release(p_ref);
        return USB_OK;
    }

    usb_mutex_unlock(__g_usb_refcnt_lock);
    return USB_OK;
}


