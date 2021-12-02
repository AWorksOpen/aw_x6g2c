#ifndef CAAM_SM_H
#define CAAM_SM_H

#include "caam_func.h"

#define SM_MAX_PAGES                16
#define SM_SLOT_COUNT_PER_PAGE      4

/* Storage access permissions */
#define SM_PERM_READ                0x01
#define SM_PERM_WRITE               0x02
#define SM_PERM_BLOB                0x03

/* Define treatment of secure memory vs. general memory blobs */
#define SM_SECMEM                   0
#define SM_GENMEM                   1

/*
 * Round a key size up to an AES blocksize boundary so to allow for
 * padding out to a full block
 */
#define AES_BLOCK_PAD(x) ((x % 16) ? ((x >> 4) + 1) << 4 : x)

/* Define space required for BKEK + MAC tag storage in any blob */
#define BLOB_OVERHEAD (32 + 16)


/* Data structure to hold per-slot information */
struct keystore_data_slot_info {
    u8                      allocated;              /* Track slot assignments */
    u32                     key_length;             /* Size of the key */
};

/* Data structure to hold keystore information */
struct keystore_data {
    void                           *base_address;       /* Virtual base of secure memory pages */
    dma_addr_t                      phys_address;       /* Physical base of secure memory pages */
    struct keystore_data_slot_info  slot[SM_SLOT_COUNT_PER_PAGE];/* Per-slot information */
};

struct caam_drv_private;
/* store the detected attributes of a secure memory page */
struct sm_page_descriptor {
    struct keystore_data        ksdata;
    int                         ref_count;              /* 有多少个想要使用这个page的任务 */
};

struct caam_drv_private_sm {
    /* SM Register offset from JR base address */
    u32                         sm_reg_offset;

    u32                         page_size;              /* page size */
    u32                         slot_size;              /* selected size of each storage block */

    /* Partition/Page Allocation Map */
    u32                         localpages;             /* Number of pages we can access */
    struct sm_page_descriptor   pagedesc[SM_MAX_PAGES]; /* Allocated per-page */
};


/* Keystore maintenance functions */
u32 sm_detect_keystore_units(struct caam_drv_private *ctrlpriv);
int sm_establish_keystore(struct caam_drv_private *ctrlpriv, u32 unit);
void sm_release_keystore(struct caam_drv_private *ctrlpriv, u32 unit);
void caam_sm_shutdown(struct caam_drv_private *ctrlpriv);



/* Keystore accessor functions */
int sm_keystore_slot_alloc(
        struct caam_drv_private *ctrlpriv,
        u32 unit,
        u32 size,
        u32 *slot
        );
int sm_keystore_slot_dealloc(
        struct caam_drv_private *ctrlpriv,
        u32 unit,
        u32 slot
        );
int sm_keystore_slot_load(
        struct caam_drv_private *ctrlpriv,
        u32 unit, u32 slot,
        const u8 *key_data,
        u32 key_length
        );
int sm_keystore_slot_read(
        struct caam_drv_private *ctrlpriv,
        u32 unit,
        u32 slot,
        u32 key_length,
        u8 *key_data);
int sm_keystore_cover_key(
        struct caam_drv_private   *ctrlpriv,
        u32 unit,
        u32 slot,
        u32 key_length,
        KEY_AUTH_WAY keyauth);
int sm_keystore_slot_export(
        struct caam_drv_private *ctrlpriv,
        u32 unit,
        u32 slot,
        KEY_COLOR keycolor,
        KEY_AUTH_WAY keyauth,
        u8 *outbuf,
        u16 keylen,
        aw_const u8 *keymod
        );
int sm_keystore_slot_import(
        struct caam_drv_private *ctrlpriv,
        u32 unit,
        u32 slot,
        KEY_COLOR keycolor,
        KEY_AUTH_WAY keyauth,
        aw_const u8 *inbuf,
        u16 keylen,
        aw_const u8 *keymod
        );

int sm_aes_encrypt(
        struct caam_drv_private   *ctrlpriv,
        u32 unit,
        u32 slot,
        u32 key_len,
        aw_const uint8_t *p_data_in,
        uint32_t data_len,
        uint8_t *p_data_out,
        aw_bool_t b_encrypt,
        aw_bool_t is_blacken_key
        );
#endif /* CAAM_SM_H */
