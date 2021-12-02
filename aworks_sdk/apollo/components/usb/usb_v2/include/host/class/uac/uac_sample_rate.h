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
 * ת��������ΪSNDRV_PCM_RATE_xxxλ
 *
 * @param rate Ҫת���Ĳ�����
 *
 * @return �����Ĳ�����ת���ɵ�SNDRV_PCM_RATE_xxx��־������δ֪������ΪSNDRV_PCM_RATE_KNOT
 */
uint32_t uac_pcm_rate_to_rate_bit(uint32_t rate);
/** ��ʼ��������*/
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
