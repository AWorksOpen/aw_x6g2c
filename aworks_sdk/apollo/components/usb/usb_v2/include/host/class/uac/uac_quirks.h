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
#ifndef __UAC_QUIRKS_H_
#define __UAC_QUIRKS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "host/core/usbh.h"
#include "host/class/uac/uac_driver.h"
#include "host/class/uac/uac_format.h"
//#include "host/class/uac/uac_pcm.h"
//#include "host/class/uac/uac_mixer.h"
//#include "usb_config.h"

/** 检查UAC设备启动兼容*/
usb_err_t uac_apply_boot_quirk(struct usbh_function  *p_fun,
                               struct usbh_interface *p_intf);
/** 检查UAC设备接口兼容*/
usb_err_t uac_apply_interface_quirk(struct usbh_audio *p_uaudio,
                                    int                iface,
                                    int                altno);
usb_err_t uac_select_mode_quirk(struct uac_substream *subs,
                                struct audioformat   *fmt);
/** 设置接口兼容*/
void uac_set_interface_quirk(struct usbh_device *p_dev);
/** 设置格式兼容*/
void uac_set_format_quirk(struct uac_substream *subs,
                          struct audioformat   *fmt);
/** 端点启动兼容*/
void uac_endpoint_start_quirk(struct uac_endpoint *ep);
/** 检查设备是否使用大端采样*/
usb_bool_t uac_is_big_endian_format(struct usbh_audio *p_uaudio, struct audioformat *fp);
/**
 * 调用以扩充DSD的PCM格式位字段类型，UAC标准没有指定位字段来表示支持的DSD接口。
 * 因此，所有已知支持此格式的硬件都必须在此列出。
 */
uint64_t uac_interface_dsd_format_quirks(struct usbh_audio  *p_uaudio,
                                         struct audioformat *fp,
                                         uint32_t            sample_bytes);
/** 控制传输兼容*/
void uac_ctl_msg_quirk(struct usbh_function *p_fun,
                       uint8_t               request,
                       uint8_t               requesttype,
                       uint16_t              value,
                       uint16_t              index,
                       void                 *data,
                       uint16_t              size);
/** 设备获取采样率兼容*/
usb_bool_t uac_get_sample_rate_quirk(struct usbh_audio *p_uaudio);
/** 设置同步端点兼容*/
usb_err_t set_sync_ep_implicit_fb_quirk(struct uac_substream *subs,
                                        uint32_t              attr);
///** 混音接口创建兼容*/
//usb_err_t usbh_uac_mixer_apply_create_quirk(struct usb_mixer_interface *mixer);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

