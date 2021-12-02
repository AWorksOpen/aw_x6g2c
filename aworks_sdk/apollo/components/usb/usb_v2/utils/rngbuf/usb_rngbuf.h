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

/* 环形缓冲区管理结构*/
struct usb_rngbuf {
    uint32_t    in;         /**< \brief 缓冲区数据写入位置 */
    uint32_t    out;        /**< \brief 缓冲区数据读出位置 */
    int         size;       /**< \brief 缓冲区大小 */
    char       *buf;        /**< \brief 缓冲区 */
};

void usb_rngbuf_init (struct usb_rngbuf *p_rb, char *p_buf, uint32_t size);
uint32_t usb_rngbuf_put (struct usb_rngbuf *rb, const char *p_buf, uint32_t nbytes);
uint32_t usb_rngbuf_get (struct usb_rngbuf *rb, char *p_buf, uint32_t nbytes);
#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AW_RNGBUF_H */
