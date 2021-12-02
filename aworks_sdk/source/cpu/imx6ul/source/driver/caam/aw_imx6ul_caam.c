/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2014 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul caam标准接口
 *
 * 本模块提供CAAM 的操作
 *
 * 使用本模块需要包含头文件 aw_imx6ul_caam.h
 *
 * \internal
 * \par modification history
 * - 1.00 2016-07-21  jac, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_common.h"

#include "aw_imx6ul_caam.h"
#include "aw_caam_internal.h"

AWBL_METHOD_IMPORT(aw_imx6ul_caam_helper_get);

static struct aw_imx6ul_caam_helper __g_caam_helper = {0};

aw_local aw_err_t __aw_caam_enum_dev_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t       pfunc_caam_helper_get  = NULL;

    /* find handler */
    pfunc_caam_helper_get = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(aw_imx6ul_caam_helper_get));

    /* call method handler to allocate GPIO service */
    if (pfunc_caam_helper_get != NULL) {
        if ( NULL == __g_caam_helper.p_caam_dev ) {
             pfunc_caam_helper_get(p_dev, &__g_caam_helper);
        }

    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief 初始化aw_imx6ul_caam功能
 */
void aw_imx6ul_caam_init(void)
{
    awbl_dev_iterate(__aw_caam_enum_dev_helper, NULL, AWBL_ITERATE_INSTANCES);
}

int aw_imx6ul_caam_encap_blob(
        aw_const uint8_t *p_key_mod,
        uint8_t *p_blob,
        aw_const uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    int         err = -AW_ENODEV;

    if (NULL != __g_caam_helper.p_caam_dev) {
        err = __g_caam_helper.caam_func->fn_caam_encap_blob(
                __g_caam_helper.p_caam_dev,
                p_key_mod,p_blob,
                p_data,data_len,
                key_color,auth_way);
    }

    return err;
}

int aw_imx6ul_caam_decap_blob(
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_blob,
        uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    int         err = -AW_ENODEV;

    if (NULL != __g_caam_helper.p_caam_dev) {
        err = __g_caam_helper.caam_func->fn_caam_decap_blob(
                __g_caam_helper.p_caam_dev,
                p_key_mod,p_blob,
                p_data,data_len,
                key_color,auth_way);
    }

    return err;
}

uint32_t aw_imx6ul_caam_get_blob_len(
        uint32_t data_len
        )
{
    uint32_t        err = -AW_ENODEV;

    if (NULL != __g_caam_helper.p_caam_dev) {
        err = __g_caam_helper.caam_func->fn_caam_get_blob_len(
                __g_caam_helper.p_caam_dev,
                data_len );
    }

    return err;
}

/**
 * \brief 执行AES加密或解密
 *
 *
 * \param   p_key       [in]AES 密钥
 * \param   key_len     [in]AES 密钥的长度，有效值为16,24,192分别对应128,192,256bit
 * \param   p_data_in   [in]想要加密的原始数据或是要解密的数据
 * \param   data_len    [in]原始数据大小
 * \param   p_data_out  [out]输出加密的结果或原始数据，要求至少有data_len字节
 * \param   b_encrypt   [in] FALSE表示执行解密操作，否则为加密
 * \return  失败返回负的错误码，否则返回AW_OK
 */
int aw_imx6ul_caam_aes(
        aw_const uint8_t   *p_key,
        uint32_t            key_len,
        aw_const uint8_t   *p_data_in,
        uint32_t            data_len,
        uint8_t            *p_data_out,
        aw_bool_t              b_encrypt)
{
    int         err = -AW_ENODEV;

    if (NULL != __g_caam_helper.p_caam_dev) {
        err = __g_caam_helper.caam_func->fn_caam_aes(
                __g_caam_helper.p_caam_dev,
                p_key,key_len,
                p_data_in,data_len,
                p_data_out,b_encrypt);
    }

    return err;
}


int aw_imx6ul_caam_aes_with_black_blob(
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_black_blob,
        uint8_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        KEY_AUTH_WAY auth_way,
        aw_bool_t  is_encrypt
        )
{
    int         err = -AW_ENODEV;

    if (NULL != __g_caam_helper.p_caam_dev) {
        err = __g_caam_helper.caam_func->fn_caam_aes_with_black_blob(
                __g_caam_helper.p_caam_dev,
                p_key_mod,p_black_blob,key_len,
                p_data_in,data_len,
                p_data_out,
                auth_way,
                is_encrypt);
    }

    return err;
}




/* end of file */
