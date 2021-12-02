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
#ifndef __AWBL_IMX6UL_USB_HC_H
#define __AWBL_IMX6UL_USB_HC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "host/awbl_usbh.h"
#include "host/controller/ehci/awbl_usbh_ehci.h"

/* 驱动名字*/
#define AWBL_IMX6UL_USBH_DRV_NAME   "awbl_imx6ul_usbh"

/** imx6ul USB主机控制器结构体*/
struct awbl_imx6ul_usbh {
    struct awbl_usbh      aw_usbh;
    struct awbl_usbh_ehci aw_ehci;
};

/** imx6ul USB主机控制器信息*/
struct awbl_imx6ul_usbh_info {
    struct awbl_usbh_info   info;          /* USB主机信息*/
    uint32_t                prio;

    uint32_t                usb_reg;       /* USB寄存器基地址*/
    uint32_t                phy_reg;       /* USB PHY寄存器基地址*/

    int                     inum;          /* 中断引脚号*/

    int                     nqhs;          /* QH数量*/
    int                     nqtds;         /* qTDs数量*/
    int                     nitds;         /* iTDs数量*/
    int                     nsitds;        /* siTDs数量*/

    void (*pfunc_pwr_ctrl) (aw_bool_t on); /* 电源控制回调函数*/
    void (*pfunc_plfm_init)(void);         /* 平台初始化回调函数*/
};

/** USB主机驱动注册*/
void awbl_imx6ul_usbh_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_USB_HC_H */
