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

/* 时间戳结构体*/
struct usb_timespec {
        long ts_sec;         /* 秒*/
        long ts_nsec;        /* 纳秒*/
};

/** 获取当前时间戳*/
usb_err_t usb_get_timespec(struct usb_timespec *ts);
/** 通过纳秒获取时间戳*/
struct usb_timespec usb_ns_to_timespec(const int64_t nsec);
#endif
