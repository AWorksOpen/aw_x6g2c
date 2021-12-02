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
#ifndef __USB_RNGBUF_H
#define __USB_RNGBUF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ���λ���������ṹ*/
struct usb_rngbuf {
    uint32_t    in;         /**< \brief ����������д��λ�� */
    uint32_t    out;        /**< \brief ���������ݶ���λ�� */
    int         size;       /**< \brief ��������С */
    char       *buf;        /**< \brief ������ */
};

void usb_rngbuf_init (struct usb_rngbuf *p_rb, char *p_buf, uint32_t size);
uint32_t usb_rngbuf_put (struct usb_rngbuf *rb, const char *p_buf, uint32_t nbytes);
uint32_t usb_rngbuf_get (struct usb_rngbuf *rb, char *p_buf, uint32_t nbytes);
#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AW_RNGBUF_H */
