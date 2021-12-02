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


/** �������洢�豸�߼���Ԫ�ṹ�� */
struct awbl_usbh_ms_lun {
    struct aw_blk_dev       bd;        /* ���豸*/
    struct aw_delayed_work  work;      /* ��ʱ����*/
    struct usbh_ms_lun     *p_ms_lun;
};


/** ��ȡ�������豸����*/
char *awbl_usbh_mass_storage_lun_blkname_get(struct usbh_ms_lun *p_lun);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

