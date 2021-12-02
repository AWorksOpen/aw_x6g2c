/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul caam driver
 *
 * this driver is the implementation of aw_serial.h for imx6ul
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-7-21, jac, first implementation.
 * \endinternal
 */


#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_int.h"
#include "aw_assert.h"
#include "driver/caam/awbl_imx6ul_caam.h"
#include "imx6ul_caam_regs.h"
#include "caam_internal.h"
#include "caam_func.h"
#include "aw_caam_internal.h"


/**
 * \addtogroup grp_imx6ul_bsp IMX6UL BSP支持
 * @{
 */

/**
 * \addtogroup grp_awbus_lite_driver AWbus lite驱动
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam IMX6UL CAAM驱动
 * \copydetails awbl_imx6ul_caam.h
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam_internal_function caam内部实现
 * @{
 */

/** @} grp_awbl_drv_imx6ul_caam_internal_function */

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam_sio_interface sio(串口IO)接口
 * @{
 */

/** @} grp_awbl_drv_imx6ul_caam_awbus_interface */

/**
 * \addtogroup grp_awbl_drv_imx6ul_caam_awbus_interface Awbus接口
 * @{
 */

/**
 * \brief connect caam device to system
 */
aw_local void __imx6ul_caam_inst_connect (awbl_dev_t *p_dev)
{
    awbl_imx6ul_caam_dev_t     *p_caam_dev = (awbl_imx6ul_caam_dev_t *)p_dev;
    AW_MUTEX_INIT(p_caam_dev->mutex_dev,AW_SEM_Q_FIFO);
    if (sizeof(struct caam_drv_private) > sizeof(p_caam_dev->caam_priv) ) {
        AW_FOREVER {

        }
    }
    caam_init_obj((struct caam_drv_private*)&p_caam_dev->caam_priv);
}

aw_local aw_const struct awbl_drvfuncs __g_imx6ul_caam_awbl_drvfuncs = {
    NULL,                               /* devInstanceInit */
    NULL,                               /* devInstanceInit2 */
    __imx6ul_caam_inst_connect          /* devConnect */
};

static int __imx6ul_caam_encap_blob (
        void *pDev,
        aw_const uint8_t *p_key_mod,
        uint8_t *p_blob,
        aw_const uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)pDev;
    int                             err;

    AW_MUTEX_LOCK(p_caam_dev->mutex_dev,AW_SEM_WAIT_FOREVER);
    err = caam_encap_blob(&p_caam_dev->caam_priv,
                    p_key_mod,p_blob,
                    p_data,data_len,
                    key_color,
                    auth_way);
    AW_MUTEX_UNLOCK(p_caam_dev->mutex_dev);

    return err;
}

static int __imx6ul_caam_decap_blob (
        void *pDev,
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_blob,
        uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)pDev;
    int                             err;

    AW_MUTEX_LOCK(p_caam_dev->mutex_dev,AW_SEM_WAIT_FOREVER);
    err = caam_decap_blob(&p_caam_dev->caam_priv,
                    p_key_mod,p_blob,
                    p_data,data_len,
                    key_color,
                    auth_way);
    AW_MUTEX_UNLOCK(p_caam_dev->mutex_dev);

    return err;
}

static uint32_t __imx6ul_caam_get_blob_len (
        void *pDev,
        uint32_t data_len
        )
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)pDev;
    uint32_t                        err;

    AW_MUTEX_LOCK(p_caam_dev->mutex_dev,AW_SEM_WAIT_FOREVER);
    err = caam_get_blob_len(&p_caam_dev->caam_priv,
                    data_len);
    AW_MUTEX_UNLOCK(p_caam_dev->mutex_dev);

    return err;
}

static int __imx6ul_caam_aes (
        void *pDev,
        aw_const uint8_t *p_key,
        uint32_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        aw_bool_t b_encrypt
        )
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)pDev;
    int                             err;

    AW_MUTEX_LOCK(p_caam_dev->mutex_dev,AW_SEM_WAIT_FOREVER);
    err = caam_aes(&p_caam_dev->caam_priv,
                    p_key,key_len,
                    p_data_in,data_len,
                    p_data_out,
                    b_encrypt);
    AW_MUTEX_UNLOCK(p_caam_dev->mutex_dev);

    return err;
}

static int  __imx6ul_caam_aes_with_black_blob (
        void *p_dev,
        aw_const uint8_t *p_key_mod,
       aw_const  uint8_t *p_black_blob,
        uint8_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        KEY_AUTH_WAY auth_way,
        aw_bool_t  is_encrypt
        )
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)p_dev;
    int                             err;

    AW_MUTEX_LOCK(p_caam_dev->mutex_dev,AW_SEM_WAIT_FOREVER);
    err = caam_aes_with_black_blob(&p_caam_dev->caam_priv,
                    p_key_mod,p_black_blob,key_len,
                    p_data_in,data_len,
                    p_data_out,
                    auth_way,
                    is_encrypt);
    AW_MUTEX_UNLOCK(p_caam_dev->mutex_dev);

    return err;
}

aw_local aw_const struct imx6ul_caam_func __g_imx6ul_caam_func = {
    __imx6ul_caam_encap_blob,
    __imx6ul_caam_decap_blob,
    __imx6ul_caam_get_blob_len,
    __imx6ul_caam_aes,
    __imx6ul_caam_aes_with_black_blob,
};

AWBL_METHOD_DEF(aw_imx6ul_caam_helper_get, "awbl_imx6ul_caam_helper_get");
void __imx6ul_caam_helper_get (awbl_dev_t *p_dev, void *p_arg)
{
    awbl_imx6ul_caam_dev_t         *p_caam_dev = (awbl_imx6ul_caam_dev_t *)p_dev;
    struct aw_imx6ul_caam_helper   *p_helper;

    p_helper = (struct aw_imx6ul_caam_helper *)p_arg;
    p_helper->p_caam_dev = p_caam_dev;
    p_helper->caam_func = &__g_imx6ul_caam_func;
}

aw_local aw_const struct awbl_dev_method __g_imx6ul_caam_plb_awbl_methods[] = {
    AWBL_METHOD(aw_imx6ul_caam_helper_get,__imx6ul_caam_helper_get),
    AWBL_METHOD_END
};


aw_local aw_const struct awbl_drvinfo __g_imx6ul_caam_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX6UL_CAAM_NAME,              /* p_drvname */
    &__g_imx6ul_caam_awbl_drvfuncs,     /* p_busfuncs */
    __g_imx6ul_caam_plb_awbl_methods,   /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

void awbl_imx6ul_caam_drv_register (void)
{
    awbl_drv_register(&__g_imx6ul_caam_drv_registration);
}
/** @} grp_awbl_drv_imx6ul_caam_awbus_interface */

/* end of file */
