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
#ifndef __AWBL_USBH_UVCVIDEO_H
#define __AWBL_USBH_UVCVIDEO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_camera.h"

/* UVC´òÓ¡º¯Êý*/
#define __AUVC_TRACE(info, ...) \
            do { \
                AW_INFOF(("AUSBH-UVC:"info, ##__VA_ARGS__)); \
            } while (0)

#define USB_UVC_DEVICE_MATCH_DEVICE \
            (AWBL_USBH_FUNDRV_MATCH_VENDOR | \
             AWBL_USBH_FUNDRV_MATCH_PRODUCT)

#define USB_UVC_DEVICE_MATCH_INT_INFO \
            (AWBL_USBH_FUNDRV_MATCH_FUN_CLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL)

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
