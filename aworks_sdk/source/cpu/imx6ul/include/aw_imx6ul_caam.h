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
/** \brief 定义KEY的颜色，具体参见red和black blob */
typedef enum {
    RED_KEY = 0,
    BLACK_KEY = 1,
}KEY_COLOR;

/** \brief 定义KEY加解密/cover的方式 */
typedef enum {
    KEY_COVER_ECB = 0,      /**< \brief cover key in AES-ECB */
    KEY_COVER_CCM = 1,      /**< \brief cover key with AES-CCM */
}KEY_AUTH_WAY;

/**
 * \brief 初始化aw_imx6ul_caam功能
 */
void aw_imx6ul_caam_init(void);

/**
 * \brief 生成一个blob
 *
 * 本函数可以生成red blob和black blob,其中red blob主要用于保护原始数据，
 * black blob用于保护密钥，black blob解开后不能看到原始的key，而red blob
 * 解开后会输出原始数据
 *
 * \param   p_key_mod   [in]key modifer,8字节大小的数组，控制由master key生成BKEK
 * \param   p_blob      [out]输出生成的blob，至少要求有data_len+48字节空间
 * \param   p_data      [in]想要用blob保护的原始数据或原始key
 * \param   data_len    [in]想要用blob保护的数据的长度
 * \param   key_color   [in] 有效值为RED_KEY和BLACK_KEY,分别对应于生成red和black blob.
 * \param   keyauth     [in]认证方式，如果选用ECM模式，则值为KEY_COVER_ECB，
 *                          否则为KEY_COVER_CCM
 * \return  失败返回负的错误码，否则返回AW_OK
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
 * \brief 解开一个blob
 *
 * 本函数可以解开red blob和black blob,
 * 但是black blob解开后依然看不到其中保护的原始数据
 *
 * \param   p_key_mod   [in]key modifer,8字节大小的数组，控制由master key生成BKEK
 * \param   p_blob      [in]想要解开的blob，至少要求有data_len+48字节空间
 * \param   p_data      [out]输入解开blob得到的数据，至少有data_len字节
 * \param   data_len    [in]blob中保护的数据大小
 * \param   key_color   [in] 有效值为RED_KEY和BLACK_KEY,分别对应于red和black blob.
 * \param   keyauth     [in]认证方式，如果选用ECM模式，则值为KEY_COVER_ECB，
 *                          否则为KEY_COVER_CCM
 * \return  失败返回负的错误码，否则返回AW_OK
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
 * \brief 由数据长度返回生成的blob长度
 *
 * \param   data_len    [in]blob中保护的数据大小
 * \return  返回blob长度，实际为data_len+48
 */
uint32_t aw_imx6ul_caam_get_blob_len(
        uint32_t data_len
        );

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
 * \brief 通过black blob执行AES加密或解密
 *
 * 由于black blob可以保护密钥，使得原始密钥不可见，将black blob用于AES加解密
 * 将更具安全性
 * \param   p_key_mod   [in]key modifer,8字节大小的数组，控制由master key生成BKEK
 * \param   p_black_blob [in]包含AES密钥的black blob，至少要求有key_len+48字节空间
 * \param   key_len     [in]AES 密钥的长度，有效值为16,24,192分别对应128,192,256bit
 * \param   p_data_in   [in]想要加密的原始数据或是要解密的数据
 * \param   data_len    [in]原始数据大小
 * \param   p_data_out  [out]输出加密的结果或原始数据，要求至少有data_len字节
 * \param   auth_way    [in]认证方式 如果选用ECM模式，则值为KEY_COVER_ECB，
 *                          否则为KEY_COVER_CCM
 * \param   b_encrypt   [in] FALSE表示执行解密操作，否则为加密
 * \return  失败返回负的错误码，否则返回AW_OK
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
