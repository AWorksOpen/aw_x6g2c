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
#include "aw_list.h"
#include "aw_vdebug.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "host/controller/ehci/awbl_usbh_ehci.h"
#include "host/controller/ehci/usbh_ehci.h"
#include <string.h>


/* 说明，为了防止USB中断丢失，开启了定时器去轮询EHCI的异步调度的QH是否有数据*/
/**
 * EHCI守护定时器回调函数
 *
 * @param p_arg 回调函数参数(这里传入EHCI结构体)
 */
aw_local void __guarder_handle (void *p_arg)
{
    struct awbl_usbh_ehci *p_aw_ehci = (struct awbl_usbh_ehci *)p_arg;
    /* 释放EHCI守护事件信号量*/
    usbh_ehci_guard_evt_start(&p_aw_ehci->p_echi);
    /* 重启EHCI定时器(100ms定时)*/
    aw_timer_start(&p_aw_ehci->guarder, 100);
}

/**
 * USB EHCI初始化函数
 *
 * @param p_ehci        EHCI结构体
 * @param reg_base      USB性能寄存器基地址
 * @param task_prio     EHCI任务优先级
 * @param pfn_rh_change 根集线器变化回调函数
 * @param ps_bit        端口速度位
 * @param nqhs          QH数量
 * @param nqtds         qTD数量
 * @param nitds         iTD数量
 * @param nsitds        siTD数量
 *
 * @return 成功返回AW_OK
 */
aw_err_t awbl_usbh_ehci_init (struct usbh           *p_hc,
                              struct awbl_usbh_ehci *p_aw_ehci,
                              uint32_t               reg_base,
                              int                    task_prio,
                              uint8_t                ps_bit,
                              int                    nqhs,
                              int                    nqtds,
                              int                    nitds,
                              int                    nsitds){
    aw_err_t ret;
    /* 初始化一个守护定时器*/
    aw_timer_init(&p_aw_ehci->guarder,
                  __guarder_handle,
                  (void *)p_aw_ehci);
    /* 初始化自旋锁*/
    aw_spinlock_isr_init(&p_aw_ehci->spinlock, 0);
    /* 初始化EHCI*/
    ret = usbh_ehci_init(p_hc, &p_aw_ehci->p_echi, reg_base, task_prio,
            ps_bit, nqhs, nqtds, nitds, nsitds);
    if(ret == AW_OK){
        /* 启动EHCI守护定时器(500ms定时)*/
        aw_timer_start(&p_aw_ehci->guarder, 500);
    }
    return ret;
}
