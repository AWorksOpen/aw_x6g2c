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
#ifndef __AWBL_USBD_H
#define __AWBL_USBD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "device/core/usbd.h"
#include "aw_spinlock.h"
#include "aw_list.h"

/** AWorks USB �豸�������ṹ��*/
struct awbl_usbd {
    struct awbl_dev awdev;  /* �����豸��*/
    struct usbd    *p_usbd;
};

/** AWorks USB �豸����������������*/
struct awbl_usbd_drv {
    /* AWorks driver */
    struct awbl_drvinfo awdrv;
    struct usbd_drv    *p_usbd_drv;
};

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBD_H */
