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

/** ��ȡ���ṹ��*/
#define __DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_u9300c_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                           struct awbl_gprs_u9300c_dev, \
                                                           dev)
/** ��GPRS�豸�л�ȡ�豸�ṹ��*/
#define __DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_u9300c_dev *p_this = \
                     (struct awbl_gprs_u9300c_dev *)p_gprs->p_gprs_dev

/** ��ӵ�GPRS����*/
aw_local aw_err_t __dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __DEV_DECL(p_this, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* �Ϸ��Լ��*/
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* ��ȡGPRS�ṹ��*/
    p_gprs = &p_this->gprs;
    /* ��ʼ��������*/
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* ��ʼ��GPRS�ṹ���Ա*/
    p_gprs->p_gprs_dev  = (void *)p_this;

    /* �ѳ�ʼ�����GPRS�ṹ�巵�ظ�GPRS����*/
    *(struct awbl_gprs_dev **)p_arg = p_gprs;
    AW_INFOF(("%s driver register.\n", p_gprs->info.devname));
    return AW_OK;
}

aw_local void __drv_connect(struct awbl_dev *p_dev)
{
}

/** ������ʼ������*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    __drv_connect,
};

/** GPRS����METHOD*/
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __dev_get),
    AWBL_METHOD_END
};

/** ������Ϣ*/
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

/** U9300C 4G����ע�ắ��*/
void awbl_gprs_u9300c_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}
