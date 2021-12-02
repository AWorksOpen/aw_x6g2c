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
#include "interior/time/usb_time.h"
#include "usb_config.h"
#include "usb_common.h"
#include "aworks.h"
#include "aw_time.h"

#ifdef USB_TIME_SUPPORT

/** 获取当前时间戳*/
usb_err_t usb_get_timespec(struct usb_timespec *ts){
    aw_timespec_t p_timespec;
    if(ts == NULL){
        return -USB_EINVAL;
    }
    aw_timespec_get(&p_timespec);

    ts->ts_sec = p_timespec.tv_sec;
    ts->ts_nsec = p_timespec.tv_nsec;

    return USB_OK;
}

/** 通过纳秒获取时间戳*/
struct usb_timespec usb_ns_to_timespec(const int64_t nsec)
{
    struct usb_timespec ts;
    int32_t             rem;

    if (nsec == 0){
        return (struct usb_timespec) {0, 0};
    }
    /* 获取秒*/
    ts.ts_sec = usb_div_s64_rem(nsec, NSEC_PER_SEC, &rem);
    if (rem < 0) {
        ts.ts_sec--;
        rem += NSEC_PER_SEC;
    }
    ts.ts_nsec = rem;

    return ts;
}

#else
/** 获取当前时间*/
void usb_get_time(struct usb_timespec *ts){
    return;
}

#endif
