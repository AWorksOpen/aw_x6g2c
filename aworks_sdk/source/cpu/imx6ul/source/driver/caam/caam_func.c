#include <apollo.h>
#include <string.h>
#include "aw_mem.h"
#include "imx6ul_caam_regs.h"
#include "caam_internal.h"
#include "aw_bitops.h"
#include "desc_constr.h"
#include "caam_func.h"
#include "aw_clk.h"



int caam_decap_blob(
        void *pDev,
        aw_const uint8_t *p_key_mod,
        aw_const uint8_t *p_blob,
        uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    uint32_t        unit_count,unit= -1;
    aw_err_t        err = -AW_EINVAL;

    struct caam_drv_private *ctrlpriv = (struct caam_drv_private *)pDev;

    u32         keyslot32 = -1;

    if(data_len % 16 != 0) {
        goto cleanup;
    }
    /* 确定安全存储有几个单元 */
    unit_count = sm_detect_keystore_units(ctrlpriv);
    if (0 == unit_count) {
        err = -AW_ENODEV;
        goto cleanup;
    }
    /* 我们选择最后一个单元 */
    /* 初始化并使能我们选择的单元 */
    err = sm_establish_keystore(ctrlpriv, unit_count -1);
    if(err <0 ) {
        goto  cleanup;
    }
    unit = unit_count -1;

    /* 从我们选定的单元存储中分配一个槽 */
    err = sm_keystore_slot_alloc(ctrlpriv, unit, data_len, &keyslot32);
    if ( err<0) {
        goto cleanup;
    }
    /* 导入blob至安全内存 */
    err = sm_keystore_slot_import(
            ctrlpriv,
            unit,
            keyslot32,
            key_color,
            auth_way,
            p_blob,
            data_len,
            p_key_mod
            );
    if(err < 0) {
        goto cleanup;
    }

    /* 读取槽的内容 */
    err = sm_keystore_slot_read(ctrlpriv, unit, keyslot32, data_len,
            p_data);
    if(err < 0) {
        goto cleanup;
    }


cleanup:
    /* Remove keys from keystore */
    if (-1 != keyslot32) {
        sm_keystore_slot_dealloc(ctrlpriv, unit, keyslot32);
    }
    if(-1 != unit) {
        sm_release_keystore(ctrlpriv, unit);
    }

    return err;
}

uint32_t caam_get_blob_len(
        void *pDev,
        uint32_t data_len
        )
{
    return data_len+BLOB_OVERHEAD;
}

int caam_encap_blob(
        void *pDev,
        aw_const uint8_t *p_key_mod,
        uint8_t *p_blob,
        aw_const uint8_t *p_data,
        uint32_t data_len,
        KEY_COLOR key_color,
        KEY_AUTH_WAY auth_way
        )
{
    uint32_t        unit_count,unit= -1;
    aw_err_t        err = -AW_EINVAL;

    struct caam_drv_private *ctrlpriv = (struct caam_drv_private *)pDev;

    u32         key_slot = -1;

    if(data_len % 16 != 0) {
        goto cleanup;
    }
    /* 确定安全存储有几个单元 */
    unit_count = sm_detect_keystore_units(ctrlpriv);
    if (0 == unit_count) {
        err = -AW_ENODEV;
        goto cleanup;
    }
    /* 我们选择最后一个单元 */
    /* 初始化并使能我们选择的单元 */
    err = sm_establish_keystore(ctrlpriv, unit_count -1);
    if(err <0 ) {
        goto  cleanup;
    }
    unit = unit_count -1;

    /* 从我们选定的单元存储中分配一个槽 */
    err = sm_keystore_slot_alloc(ctrlpriv, unit, data_len, &key_slot);
    if ( err<0) {
        goto cleanup;
    }

    /* 将数据放入安全内存 */
    err = sm_keystore_slot_load(ctrlpriv, unit, key_slot,
            p_data,data_len);
    if(err < 0) {
        goto cleanup;
    }

    //如果是black blob，则cover the key
    if(BLACK_KEY == key_color) {
        err = sm_keystore_cover_key(
                ctrlpriv,
                unit,
                key_slot,
                data_len,
                auth_way);
        if(err < 0) {
            goto cleanup;
        }
    }
    /* 导出blob */
    err = sm_keystore_slot_export(
            ctrlpriv,
            unit,
            key_slot,
            key_color,
            auth_way,
            p_blob,
            data_len,
            p_key_mod
            );
    if(err < 0) {
        goto cleanup;
    }


cleanup:
    /* Remove keys from keystore */
    if (-1 != key_slot) {
        sm_keystore_slot_dealloc(ctrlpriv, unit, key_slot);
    }
    if(-1 != unit) {
        sm_release_keystore(ctrlpriv, unit);
    }

    return err;
}

int
caam_aes (
        void *pDev,
        aw_const uint8_t *p_key,
        uint32_t key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        aw_bool_t b_encrypt
        )
{
    uint32_t        unit_count,unit= -1;
    aw_err_t        err = -AW_EINVAL;
    struct caam_drv_private *ctrlpriv = (struct caam_drv_private *)pDev;

    u32             key_slot = -1;

    if(key_len !=16 && key_len!= 24 && key_len != 32 ) {
        goto cleanup;
    }
    if(data_len % 16 != 0 && data_len> 256) {
        goto cleanup;
    }

    /* 确定安全存储有几个单元 */
    unit_count = sm_detect_keystore_units(ctrlpriv);
    if (0 == unit_count) {
        err = -AW_ENODEV;
        goto cleanup;
    }
    /* 我们选择最后一个单元 */
    /* 初始化并使能我们选择的单元 */
    err = sm_establish_keystore(ctrlpriv, unit_count -1);
    if(err <0 ) {
        goto  cleanup;
    }
    unit = unit_count -1;

    /* 从我们选定的单元存储中分配一个槽 */
    err = sm_keystore_slot_alloc(ctrlpriv, unit, data_len, &key_slot);
    if ( err<0) {
        goto cleanup;
    }
    /* 将key放入安全内存 */
    err = sm_keystore_slot_load(ctrlpriv, unit, key_slot, p_key,
            key_len);
    if(err < 0) {
        goto cleanup;
    }

    err = sm_aes_encrypt(ctrlpriv,
                         unit,
                         key_slot,
                         key_len
                         ,p_data_in,
                         data_len,
                         p_data_out,
                         b_encrypt,
                         AW_FALSE);
    if(err < 0) {
        goto cleanup;
    }

cleanup:
    /* Remove keys from keystore */
    if (-1 != key_slot) {
        sm_keystore_slot_dealloc(ctrlpriv, unit, key_slot);
    }
    if(-1 != unit) {
        sm_release_keystore(ctrlpriv, unit);
    }

    return err;
}


int caam_aes_with_black_blob(
        void    *h_caam,
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
    uint32_t        unit_count,unit= -1;
    aw_err_t        err = -AW_EINVAL;
    struct caam_drv_private *ctrlpriv = (struct caam_drv_private *)h_caam;


    u32             key_slot = -1;

    if(key_len !=16 && key_len!= 24 && key_len != 32 ) {
        goto cleanup;
    }
    if(data_len % 16 != 0 && data_len> 1024) {
        goto cleanup;
    }

    /* 确定安全存储有几个单元 */
    unit_count = sm_detect_keystore_units(ctrlpriv);
    if (0 == unit_count) {
        err = -AW_ENODEV;
        goto cleanup;
    }
    /* 我们选择最后一个单元 */
    /* 初始化并使能我们选择的单元 */
    err = sm_establish_keystore(ctrlpriv, unit_count -1);
    if(err <0 ) {
        goto  cleanup;
    }
    unit = unit_count -1;

    /* 从我们选定的单元存储中分配一个槽 */
    err = sm_keystore_slot_alloc(ctrlpriv, unit, key_len, &key_slot);
    if ( err<0) {
        goto cleanup;
    }

    /* 导入blob至安全内存 */
    err = sm_keystore_slot_import(
            ctrlpriv,
            unit,
            key_slot,
            BLACK_KEY,
            auth_way,
            p_black_blob,
            key_len,
            p_key_mod
            );
    if(err < 0) {
        goto cleanup;
    }


    /* 加密或解密 */
    err = sm_aes_encrypt(ctrlpriv,
                         unit,
                         key_slot,
                         key_len,
                         p_data_in,
                         data_len,
                         p_data_out,
                         is_encrypt,
                         AW_TRUE);
    if(err < 0) {
        goto cleanup;
    }

cleanup:
    /* Remove keys from keystore */
    if (-1 != key_slot) {
        sm_keystore_slot_dealloc(ctrlpriv, unit, key_slot);
    }
    if(-1 != unit) {
        sm_release_keystore(ctrlpriv, unit);
    }

    return  err;
}

void caam_deinit(void *p_caam)
{
     struct caam_drv_private       *p = NULL;

     if ( NULL == p_caam) {
         return ;
     }
     p = (struct caam_drv_private *)p_caam;
     caam_deinit_obj(p);
     aw_mem_free(p);
}

void *caam_init()
{
    struct caam_drv_private        *p;
    int                             ret = 0;

    p = (void *)aw_mem_alloc(sizeof(struct caam_drv_private) );
    if (NULL == p) {
        goto cleanup;
    }
    ret = caam_init_obj(p);
    if (0 != ret) {
        goto cleanup;
    }
cleanup:
    if (0 != ret) {
        aw_mem_free(p);
        p = NULL;
    }
    return p;
}



int caam_init_obj(struct caam_drv_private *ctrlpriv)
{
    aw_clk_enable(IMX6UL_CLK_CAAM_MEM);
    aw_clk_enable(IMX6UL_CLK_CAAM_ACLK);
    aw_clk_enable(IMX6UL_CLK_CAAM_IPG);
    aw_clk_enable(IMX6UL_CLK_SNVS_HP);
    aw_clk_enable(IMX6UL_CLK_SNVS_LP);
    aw_clk_enable(IMX6UL_CLK_SNVS_DRYICE);

    caam_probe(ctrlpriv);
    caam_jr_init(&ctrlpriv->jr_info[0] );
    caam_sm_init(ctrlpriv);

    return 0;
}

void caam_deinit_obj(struct caam_drv_private *ctrlpriv)
{
    caam_sm_deinit(ctrlpriv);
    caam_jr_deinit(&ctrlpriv->jr_info[0] );
    caam_remove(ctrlpriv);

    aw_clk_disable(IMX6UL_CLK_SNVS_DRYICE);
    aw_clk_disable(IMX6UL_CLK_SNVS_LP);
    aw_clk_disable(IMX6UL_CLK_SNVS_HP);
    aw_clk_disable(IMX6UL_CLK_CAAM_IPG);
    aw_clk_disable(IMX6UL_CLK_CAAM_ACLK);
    aw_clk_disable(IMX6UL_CLK_CAAM_MEM);
}

#if 0
/* Fixed known pattern for a key modifier */
static uint8_t skeymod[] = {
    0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08
};

//由linux系统生成的包含32字节数据的red blob
static uint8_t test_ret_blob_linux_32[] = {
        0xba,0xac,0xfd,0x3a,0x0f,0x3e,0xff,0x80,
        0xa3,0x57,0x88,0x38,0x0d,0x52,0x24,0x15,
        0xca,0xe3,0x05,0xb3,0x61,0x38,0xe4,0x7b,
        0xb8,0xb0,0x2a,0xb4,0xfb,0x0d,0x0b,0x43,
        0x2d,0xa6,0x12,0xde,0x6d,0xa9,0x13,0xd5,
        0xb8,0x35,0x02,0xe3,0xd4,0xca,0x95,0xad,
        0x9d,0x71,0xed,0xc1,0xe0,0xc4,0xb5,0x4d,
        0xd3,0xe3,0xc1,0xce,0xcb,0x38,0xa1,0xfc,
        0x22,0xfa,0x0a,0x36,0xab,0x51,0xea,0x2a,
        0x8a,0xf3,0xa2,0x0f,0xcf,0x31,0x19,0x12,
};
void send_str(char *str);

uint8_t         g_dst_buf1[256];
uint8_t         g_dst_buf2[256];
uint8_t         g_src_buf1[256];


/* Known test vector from NIST specific publication 800-38A (section F.1.1)*/
/* AES ECB 128-bit encryption mode:
 * Encryption key = 2b7e151628aed2a6abf7158809cf4f3c
 * Initialization vector = not used in ECB mode
 * Test vector = 6bc1bee22e409f96e93d7e117393172a
 * Cipher text = 3ad77bb40d7a3660a89ecaf32466ef97
 */
/* Data Encryption Key - i.e. the key will be wrapped in the blob */
/* Data Encryption Key - i.e. the key will be wrapped in the blob */
uint8_t dek[16] =
{
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};
/* Test vector to encrypt */
uint8_t test_vect[16] =
{
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
};
/* Cipher text */
uint8_t cipher_text[16] =
{
    0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A, 0x36, 0x60,
    0xA8, 0x9E, 0xCA, 0xF3, 0x24, 0x66, 0xEF, 0x97
};


void test_caam(void *h_caam)
{
    uint32_t        unit_count,unit= -1;
    aw_err_t        err = -AW_EINVAL;
    struct caam_drv_private *ctrlpriv = (struct caam_drv_private *)h_caam;
    uint32_t        key_len = 16;
    uint32_t        data_len = 16;

    u32             key_slot = -1;

    if(key_len !=16 && key_len!= 24 && key_len != 32 ) {
        goto cleanup;
    }
    if(data_len % 16 != 0 && data_len> 256) {
        goto cleanup;
    }

    /* 确定安全存储有几个单元 */
    unit_count = sm_detect_keystore_units(ctrlpriv);
    if (0 == unit_count) {
        err = -AW_ENODEV;
        goto cleanup;
    }
    /* 我们选择最后一个单元 */
    /* 初始化并使能我们选择的单元 */
    err = sm_establish_keystore(ctrlpriv, unit_count -1);
    if(err <0 ) {
        goto  cleanup;
    }
    unit = unit_count -1;

    /* 从我们选定的单元存储中分配一个槽 */
    err = sm_keystore_slot_alloc(ctrlpriv, unit, data_len, &key_slot);
    if ( err<0) {
        goto cleanup;
    }
    /* 将key放入安全内存 */
    err = sm_keystore_slot_load(ctrlpriv, unit, key_slot, dek,
            key_len);
    if(err < 0) {
        goto cleanup;
    }

   err = sm_keystore_cover_key(
            ctrlpriv,
            unit,
            key_slot,
            key_len,
            KEY_COVER_ECB);
    err = sm_aes_encrypt(ctrlpriv,
                         unit,
                         key_slot,
                         key_len,
                         test_vect,
                         data_len,
                         g_dst_buf1,
                         AW_TRUE,
                         AW_TRUE);
    if(err < 0) {
        goto cleanup;
    }

cleanup:
    /* Remove keys from keystore */
    if (-1 != key_slot) {
        sm_keystore_slot_dealloc(ctrlpriv, unit, key_slot);
    }
    if(-1 != unit) {
        sm_release_keystore(ctrlpriv, unit);
    }

}
#endif
