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
#ifndef __USB_REFCNT_H
#define __USB_REFCNT_H

#include "usb_common.h"
#include "usb_errno.h"
#include "usb_adapter_inter.h"

/**
 * 初始化引用计数
 *
 * @param  p_ref 引用计数对象指针
 */
void usb_refcnt_init (int *p_ref);
/**
 * 检查引用计数是否有效。
 *
 * @param p_ref 引用计数对象指针
 */
usb_bool_t usb_refcnt_valid (int  *p_ref);
/**
 * 引用计数加。
 *
 * @param p_ref 引用计数对象指针
 *
 * @return 成功返回USB_OK
 */
usb_err_t usb_refcnt_get (int *p_ref);
/**
 * 引用计数减
 *
 * @param  p_ref       引用计数对象指针
 * @param  pfn_release 回调函数，引用计数释放后执行
 *
 * @return 成功返回USB_OK
 */
usb_err_t usb_refcnt_put (int   *p_ref,
                          void (*pfn_release) (int *p_ref));






#endif

