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
#ifndef __UAC_PITCH_H_
#define __UAC_PITCH_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uac/uac_driver.h"


/** 初始化音量控制*/
usb_err_t uac_init_pitch(struct usbh_audio     *p_uaudio,
                         int                    iface,
                         struct usbh_interface *alts,
                         struct audioformat    *fmt);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

