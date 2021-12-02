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
#ifndef __AWBL_USBH_MASS_STORAGE_H
#define __AWBL_USBH_MASS_STORAGE_H
#include "aw_delayed_work.h"
#include "fs/aw_blk_dev.h"
#include "host/class/ms/usbh_mass_storage.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/** 大容量存储设备逻辑单元结构体 */
struct awbl_usbh_ms_lun {
    struct aw_blk_dev       bd;        /* 块设备*/
    struct aw_delayed_work  work;      /* 延时工作*/
    struct usbh_ms_lun     *p_ms_lun;
};


/** 获取分区块设备名字*/
char *awbl_usbh_mass_storage_lun_blkname_get(struct usbh_ms_lun *p_lun);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

