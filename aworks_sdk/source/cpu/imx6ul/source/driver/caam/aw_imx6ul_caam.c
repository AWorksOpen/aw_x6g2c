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
 * \brief imx6ul caam��׼�ӿ�
 *
 * ��ģ���ṩCAAM �Ĳ���
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_imx6ul_caam.h
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
 * \brief ��ʼ��aw_imx6ul_caam����
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
 * \brief ִ��AES���ܻ����
 *
 *
 * \param   p_key       [in]AES ��Կ
 * \param   key_len     [in]AES ��Կ�ĳ��ȣ���ЧֵΪ16,24,192�ֱ��Ӧ128,192,256bit
 * \param   p_data_in   [in]��Ҫ���ܵ�ԭʼ���ݻ���Ҫ���ܵ�����
 * \param   data_len    [in]ԭʼ���ݴ�С
 * \param   p_data_out  [out]������ܵĽ����ԭʼ���ݣ�Ҫ��������data_len�ֽ�
 * \param   b_encrypt   [in] FALSE��ʾִ�н��ܲ���������Ϊ����
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
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
