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
#ifndef __USB_DELAY_H
#define __USB_DELAY_H

void usb_mdelay_block(uint32_t ms);
void usb_udelay_block(uint32_t us);
void usb_mdelay(uint32_t ms);

#endif
