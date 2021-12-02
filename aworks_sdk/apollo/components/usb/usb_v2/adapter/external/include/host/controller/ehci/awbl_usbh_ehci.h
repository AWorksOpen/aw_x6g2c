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
#ifndef __AWBL_USBH_EHCI_H
#define __AWBL_USBH_EHCI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_timer.h"
#include "aw_spinlock.h"
#include "host/controller/ehci/usbh_ehci.h"

/** EHCI控制器结构体*/
struct awbl_usbh_ehci {
    struct usbh_ehci          p_echi;    /* USB内部EHCI结构体*/
    aw_spinlock_isr_t         spinlock;
    aw_timer_t                guarder;   /* 守护定时器*/
};

aw_err_t awbl_usbh_ehci_init (struct usbh           *p_hc,
                              struct awbl_usbh_ehci *p_aw_ehci,
                              uint32_t               reg_base,
                              int                    task_prio,
                              uint8_t                ps_bit,
                              int                    nqhs,
                              int                    nqtds,
                              int                    nitds,
                              int                    nsitds);
aw_err_t awbl_usbh_ehci_rh_change_check (struct awbl_usbh_ehci *aw_ehci,
                                         uint16_t              *p_change);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_EHCI_H */
