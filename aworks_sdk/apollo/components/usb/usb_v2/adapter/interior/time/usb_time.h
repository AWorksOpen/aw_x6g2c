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
#ifndef __USB_TIME_H__
#define __USB_TIME_H__
#include "usb_errno.h"
#include <stdio.h>

/* ʱ����ṹ��*/
struct usb_timespec {
        long ts_sec;         /* ��*/
        long ts_nsec;        /* ����*/
};

/** ��ȡ��ǰʱ���*/
usb_err_t usb_get_timespec(struct usb_timespec *ts);
/** ͨ�������ȡʱ���*/
struct usb_timespec usb_ns_to_timespec(const int64_t nsec);
#endif
