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

/* ��������*/
#define AWBL_IMX6UL_USBH_DRV_NAME   "awbl_imx6ul_usbh"

/** imx6ul USB�����������ṹ��*/
struct awbl_imx6ul_usbh {
    struct awbl_usbh      aw_usbh;
    struct awbl_usbh_ehci aw_ehci;
};

/** imx6ul USB������������Ϣ*/
struct awbl_imx6ul_usbh_info {
    struct awbl_usbh_info   info;          /* USB������Ϣ*/
    uint32_t                prio;

    uint32_t                usb_reg;       /* USB�Ĵ�������ַ*/
    uint32_t                phy_reg;       /* USB PHY�Ĵ�������ַ*/

    int                     inum;          /* �ж����ź�*/

    int                     nqhs;          /* QH����*/
    int                     nqtds;         /* qTDs����*/
    int                     nitds;         /* iTDs����*/
    int                     nsitds;        /* siTDs����*/

    void (*pfunc_pwr_ctrl) (aw_bool_t on); /* ��Դ���ƻص�����*/
    void (*pfunc_plfm_init)(void);         /* ƽ̨��ʼ���ص�����*/
};

/** USB��������ע��*/
void awbl_imx6ul_usbh_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_USB_HC_H */
