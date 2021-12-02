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
 * ��ʼ�����ü���
 *
 * @param  p_ref ���ü�������ָ��
 */
void usb_refcnt_init (int *p_ref);
/**
 * ������ü����Ƿ���Ч��
 *
 * @param p_ref ���ü�������ָ��
 */
usb_bool_t usb_refcnt_valid (int  *p_ref);
/**
 * ���ü����ӡ�
 *
 * @param p_ref ���ü�������ָ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usb_refcnt_get (int *p_ref);
/**
 * ���ü�����
 *
 * @param  p_ref       ���ü�������ָ��
 * @param  pfn_release �ص����������ü����ͷź�ִ��
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t usb_refcnt_put (int   *p_ref,
                          void (*pfn_release) (int *p_ref));






#endif

