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
#include "aworks.h"
#include "aw_delay.h"

/** AWorks ������ʱms(����)����*/
void usb_mdelay_block(uint32_t ms){
    aw_udelay(ms * 1000);
}

/** AWorks ������ʱus(����)����*/
void usb_udelay_block(uint32_t us){
    aw_udelay(us);
}

/** AWorks ������ʱms(������)����*/
void usb_mdelay(uint32_t ms){
    aw_mdelay(ms);
}
