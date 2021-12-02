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
#ifndef __USB_JOB_H
#define __USB_JOB_H

/** 创建一个USB工作*/
void* usb_job_create(void (*pfunc_job)(void *p_arg), void *p_arg);
/** 销毁USB工作*/
void usb_job_destory(void *job);
/** 启动USB工作*/
void usb_job_start(void *job);
#endif

