/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_gprs.h"

#include <stdio.h>
#include <string.h>

#include "driver/gprs/awbl_gprs_u9300c.h"

/** 获取备结构体*/
#define __DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_u9300c_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                           struct awbl_gprs_u9300c_dev, \
                                                           dev)
/** 从GPRS设备中获取设备结构体*/
#define __DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_u9300c_dev *p_this = \
                     (struct awbl_gprs_u9300c_dev *)p_gprs->p_gprs_dev

/** 添加到GPRS服务*/
aw_local aw_err_t __dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __DEV_DECL(p_this, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* 合法性检查*/
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* 获取GPRS结构体*/
    p_gprs = &p_this->gprs;
    /* 初始化互斥锁*/
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* 初始化GPRS结构体成员*/
    p_gprs->p_gprs_dev  = (void *)p_this;

    /* 把初始化完的GPRS结构体返回给GPRS服务*/
    *(struct awbl_gprs_dev **)p_arg = p_gprs;
    AW_INFOF(("%s driver register.\n", p_gprs->info.devname));
    return AW_OK;
}

aw_local void __drv_connect(struct awbl_dev *p_dev)
{
}

/** 驱动初始化函数*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    __drv_connect,
};

/** GPRS服务METHOD*/
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __dev_get),
    AWBL_METHOD_END
};

/** 驱动信息*/
aw_local aw_const awbl_plb_drvinfo_t __g_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_U9300C_NAME,                /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_methods[0],          /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

/** U9300C 4G驱动注册函数*/
void awbl_gprs_u9300c_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}
