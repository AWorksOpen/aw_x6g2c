#ifndef     __AW_CAAM_INTERNAL_H__
#define     __AW_CAAM_INTERNAL_H__


typedef int (*pfn_caam_encap_blob_t)(
        void *p_dev,
        aw_const uint8_t *p_key_mod,
        uint8_t *p_blob,
        aw_const uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        );
typedef int (*pfn_caam_decap_blob_t)(
        void *p_dev,
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_blob,
        uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        );

typedef int (*pfn_caam_aes_t)(void *p_dev,
        aw_const uint8_t *p_key,
        uint32_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        aw_bool_t b_encrypt
        );

typedef uint32_t (*pfn_caam_get_blob_len_t)(
        void *p_dev,
        uint32_t data_len
        );

typedef int (*pfn_caam_aes_with_black_blob_t)(
        void *p_dev,
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_black_blob,
        uint8_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        KEY_AUTH_WAY auth_way,
        aw_bool_t  is_encrypt
        );

struct imx6ul_caam_func{

    pfn_caam_encap_blob_t           fn_caam_encap_blob;
    pfn_caam_decap_blob_t           fn_caam_decap_blob;
    pfn_caam_get_blob_len_t         fn_caam_get_blob_len;
    pfn_caam_aes_t                  fn_caam_aes;
    pfn_caam_aes_with_black_blob_t  fn_caam_aes_with_black_blob;
};

struct aw_imx6ul_caam_helper {
    void                           *p_caam_dev;
    const struct imx6ul_caam_func  *caam_func;
};

#endif


