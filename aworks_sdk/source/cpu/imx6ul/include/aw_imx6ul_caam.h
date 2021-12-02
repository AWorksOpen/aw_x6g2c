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

#ifndef __AW_IMX6UL_CAAM_H
#define __AW_IMX6UL_CAAM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_imx6ul_caam
 * \copydoc aw_imx6ul_caam.h
 * @{
 */

#include "aw_common.h"
/** \brief ����KEY����ɫ������μ�red��black blob */
typedef enum {
    RED_KEY = 0,
    BLACK_KEY = 1,
}KEY_COLOR;

/** \brief ����KEY�ӽ���/cover�ķ�ʽ */
typedef enum {
    KEY_COVER_ECB = 0,      /**< \brief cover key in AES-ECB */
    KEY_COVER_CCM = 1,      /**< \brief cover key with AES-CCM */
}KEY_AUTH_WAY;

/**
 * \brief ��ʼ��aw_imx6ul_caam����
 */
void aw_imx6ul_caam_init(void);

/**
 * \brief ����һ��blob
 *
 * ��������������red blob��black blob,����red blob��Ҫ���ڱ���ԭʼ���ݣ�
 * black blob���ڱ�����Կ��black blob�⿪���ܿ���ԭʼ��key����red blob
 * �⿪������ԭʼ����
 *
 * \param   p_key_mod   [in]key modifer,8�ֽڴ�С�����飬������master key����BKEK
 * \param   p_blob      [out]������ɵ�blob������Ҫ����data_len+48�ֽڿռ�
 * \param   p_data      [in]��Ҫ��blob������ԭʼ���ݻ�ԭʼkey
 * \param   data_len    [in]��Ҫ��blob���������ݵĳ���
 * \param   key_color   [in] ��ЧֵΪRED_KEY��BLACK_KEY,�ֱ��Ӧ������red��black blob.
 * \param   keyauth     [in]��֤��ʽ�����ѡ��ECMģʽ����ֵΪKEY_COVER_ECB��
 *                          ����ΪKEY_COVER_CCM
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
int aw_imx6ul_caam_encap_blob(
        aw_const uint8_t *p_key_mod,
        uint8_t *p_blob,
        aw_const uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        );


/**
 * \brief �⿪һ��blob
 *
 * ���������Խ⿪red blob��black blob,
 * ����black blob�⿪����Ȼ���������б�����ԭʼ����
 *
 * \param   p_key_mod   [in]key modifer,8�ֽڴ�С�����飬������master key����BKEK
 * \param   p_blob      [in]��Ҫ�⿪��blob������Ҫ����data_len+48�ֽڿռ�
 * \param   p_data      [out]����⿪blob�õ������ݣ�������data_len�ֽ�
 * \param   data_len    [in]blob�б��������ݴ�С
 * \param   key_color   [in] ��ЧֵΪRED_KEY��BLACK_KEY,�ֱ��Ӧ��red��black blob.
 * \param   keyauth     [in]��֤��ʽ�����ѡ��ECMģʽ����ֵΪKEY_COVER_ECB��
 *                          ����ΪKEY_COVER_CCM
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
int aw_imx6ul_caam_decap_blob(
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_blob,
        uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        );

/**
 * \brief �����ݳ��ȷ������ɵ�blob����
 *
 * \param   data_len    [in]blob�б��������ݴ�С
 * \return  ����blob���ȣ�ʵ��Ϊdata_len+48
 */
uint32_t aw_imx6ul_caam_get_blob_len(
        uint32_t data_len
        );

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
int
aw_imx6ul_caam_aes (
        aw_const uint8_t *p_key,
        uint32_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        aw_bool_t b_encrypt
        );

/**
 * \brief ͨ��black blobִ��AES���ܻ����
 *
 * ����black blob���Ա�����Կ��ʹ��ԭʼ��Կ���ɼ�����black blob����AES�ӽ���
 * �����߰�ȫ��
 * \param   p_key_mod   [in]key modifer,8�ֽڴ�С�����飬������master key����BKEK
 * \param   p_black_blob [in]����AES��Կ��black blob������Ҫ����key_len+48�ֽڿռ�
 * \param   key_len     [in]AES ��Կ�ĳ��ȣ���ЧֵΪ16,24,192�ֱ��Ӧ128,192,256bit
 * \param   p_data_in   [in]��Ҫ���ܵ�ԭʼ���ݻ���Ҫ���ܵ�����
 * \param   data_len    [in]ԭʼ���ݴ�С
 * \param   p_data_out  [out]������ܵĽ����ԭʼ���ݣ�Ҫ��������data_len�ֽ�
 * \param   auth_way    [in]��֤��ʽ ���ѡ��ECMģʽ����ֵΪKEY_COVER_ECB��
 *                          ����ΪKEY_COVER_CCM
 * \param   b_encrypt   [in] FALSE��ʾִ�н��ܲ���������Ϊ����
 * \return  ʧ�ܷ��ظ��Ĵ����룬���򷵻�AW_OK
 */
int aw_imx6ul_caam_aes_with_black_blob(
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_black_blob,
        uint8_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        KEY_AUTH_WAY auth_way,
        aw_bool_t  is_encrypt
        );

/** @} grp_aw_if_imx6ul_caam */

#ifdef __cplusplus
}
#endif

#endif /* __AW_IMX6UL_CAAM_H */

/* end of file */
