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
#ifndef __UAC_SAMPLE_RATE_H_
#define __UAC_SAMPLE_RATE_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */




/**
 * 转换采样率为SNDRV_PCM_RATE_xxx位
 *
 * @param rate 要转换的采样率
 *
 * @return 给定的采样率转换成的SNDRV_PCM_RATE_xxx标志，或者未知采样率为SNDRV_PCM_RATE_KNOT
 */
uint32_t uac_pcm_rate_to_rate_bit(uint32_t rate);
/** 初始化采样率*/
usb_err_t uac_init_sample_rate(struct usbh_audio     *p_uaudio,
                               struct uac_substream  *subs,
                               int                    iface,
                               struct usbh_interface *alts,
                               struct audioformat    *fmt,
                               int                    rate);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
