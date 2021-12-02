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
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "host/core/usbh.h"
#include "string.h"
#include "aw_delayed_work.h"

/** ����һ��USB����
 *  �����֧�֣��뷵��NULL*/
void* usb_job_create(void (*pfunc_job)(void *p_arg), void *p_arg){
    struct aw_delayed_work *p_work = NULL;

    if(pfunc_job == NULL){
        return NULL;
    }

    p_work = aw_mem_alloc(sizeof(struct aw_delayed_work));
    if(p_work == NULL){
        return NULL;
    }
    memset(p_work, 0, sizeof(struct aw_delayed_work));

    aw_delayed_work_init(p_work, pfunc_job, p_arg);

    return p_work;
}

/** ����USB����*/
void usb_job_destory(void *job){

}

/** ����USB����*/
void usb_job_start(void *job){
    struct aw_delayed_work *p_work = (struct aw_delayed_work *)job;
    if(job == NULL){
        return;
    }
    aw_delayed_work_start(p_work, 500);
}

