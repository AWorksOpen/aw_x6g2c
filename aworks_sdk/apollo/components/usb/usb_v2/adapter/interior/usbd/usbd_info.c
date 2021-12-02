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
#include "awbus_lite.h"
#include "aw_refcnt.h"
#include <string.h>
#include "device/core/usbd.h"
#include "device/awbl_usbd.h"

/** AWorks 获取USB设备控制器信息*/
void *usbd_get_info(void *data){
    struct usbd *p_usbd = (struct usbd *)data;

    if((p_usbd == NULL) || (p_usbd->usr_priv == NULL)){
        return NULL;
    }
    struct usbd_info *p_info = (struct usbd_info *)AWBL_DEVINFO_GET(p_usbd->usr_priv);
    return p_info;
}

/** AWorks 获取USB设备驱动函数集*/
void *usbd_get_drv(void *data){
    struct usbd *p_usbd = (struct usbd *)data;
    if((p_usbd == NULL) || (p_usbd->usr_priv == NULL)){
        return NULL;
    }
    struct awbl_usbd_drv *p_drv =
            (struct awbl_usbd_drv *)AW_CONTAINER_OF(
                    ((struct awbl_dev *)p_usbd->usr_priv)->p_driver,
                    struct awbl_usbd_drv,
                    awdrv);
    if(p_drv->p_usbd_drv != NULL){
        return p_drv->p_usbd_drv;
    }
    return NULL;
}



