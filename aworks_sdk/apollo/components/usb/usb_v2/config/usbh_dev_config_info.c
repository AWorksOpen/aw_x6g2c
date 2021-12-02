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
#include "host/core/usbh.h"

#define DEV_SPEC_END  {                      \
                            .vid = 0,        \
                            .pid = 0,        \
                            .config_num = 0, \
                       }
#define USB_ECM


/* 设置设备初始化配置*/
const struct usbh_dev_spec_config_info __g_dev_config_tab[] = {
#ifdef USB_ECM
/* 华为4G模块，设置4G模块模式(PPP或ECM)*/
{
        .vid = 0x12D1,
        .pid = 0x15c1,
        .config_num = 2,
},
#endif
    DEV_SPEC_END,
};
