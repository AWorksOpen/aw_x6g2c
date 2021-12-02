#include <apollo.h>
#include <string.h>
#include <aw_vdebug.h>
#include "imx6ul_caam_regs.h"
#include "caam_internal.h"
#include "imx6ul_regs_base.h"
#include "aw_bitops.h"
#include "desc_constr.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "caam_sm.h"


#define SECMEM_KEYMOD_LEN   8
#define GENMEM_KEYMOD_LEN   16
#define INITIAL_DESCSZ      16

static int blob_decap_jobdesc(u32 *desc,
                              dma_addr_t keymod,
                              dma_addr_t blobbuf,
                              dma_addr_t outbuf,
                              u16 secretsz,
                              u8 keycolor,
                              u8 blobtype,
                              u8 auth)
{
    u32     tmpdesc[INITIAL_DESCSZ];
    u16     dsize, idx;

    memset(tmpdesc, 0, INITIAL_DESCSZ * sizeof(u32));
    idx = 1;

    /* Load key modifier */
    if (blobtype == SM_SECMEM)
        tmpdesc[idx++] = CMD_LOAD | LDST_CLASS_2_CCB |
                 LDST_SRCDST_BYTE_KEY |
                 ((12 << LDST_OFFSET_SHIFT) & LDST_OFFSET_MASK)
                 | (8 & LDST_LEN_MASK);
    else /* is general memory blob */
        tmpdesc[idx++] = CMD_LOAD | LDST_CLASS_2_CCB |
                 LDST_SRCDST_BYTE_KEY | (16 & LDST_LEN_MASK);

    tmpdesc[idx++] = (u32)keymod;

    /* Compensate BKEK + MAC tag over size of encapsulated secret */
    tmpdesc[idx++] = CMD_SEQ_IN_PTR | (secretsz + BLOB_OVERHEAD);
    tmpdesc[idx++] = (u32)blobbuf;
    tmpdesc[idx++] = CMD_SEQ_OUT_PTR | secretsz;
    tmpdesc[idx++] = (u32)outbuf;

    /* Decapsulate from secure memory partition to black blob */
    tmpdesc[idx] = CMD_OPERATION | OP_TYPE_DECAP_PROTOCOL | OP_PCLID_BLOB;

    if (blobtype == SM_SECMEM)
        tmpdesc[idx] |= OP_PCL_BLOB_PTXT_SECMEM;

    if (auth == KEY_COVER_CCM)
        tmpdesc[idx] |= OP_PCL_BLOB_EKT;

    if (keycolor == BLACK_KEY)
        tmpdesc[idx] |= OP_PCL_BLOB_BLACK;

    idx++;
    tmpdesc[0] = CMD_DESC_HDR | HDR_ONE | (idx & HDR_DESCLEN_MASK);
    dsize = idx * sizeof(u32);


    memcpy(desc, tmpdesc, dsize);

    return dsize;
}

static int blob_encap_jobdesc(u32 *desc, dma_addr_t keymod,
                  dma_addr_t secretbuf, dma_addr_t outbuf,
                  u16 secretsz, u8 keycolor, u8 blobtype, u8 auth)
{
    u32  tmpdesc[INITIAL_DESCSZ];
    u16 dsize, idx;

    memset(tmpdesc, 0, INITIAL_DESCSZ * sizeof(u32));
    idx = 1;

    /*
     * Key modifier works differently for secure/general memory blobs
     * This accounts for the permission/protection data encapsulated
     * within the blob if a secure memory blob is requested
     */
    if (blobtype == SM_SECMEM)
        tmpdesc[idx++] = CMD_LOAD | LDST_CLASS_2_CCB |
                 LDST_SRCDST_BYTE_KEY |
                 ((12 << LDST_OFFSET_SHIFT) & LDST_OFFSET_MASK)
                 | (8 & LDST_LEN_MASK);
    else /* is general memory blob */
        tmpdesc[idx++] = CMD_LOAD | LDST_CLASS_2_CCB |
                 LDST_SRCDST_BYTE_KEY | (16 & LDST_LEN_MASK);

    tmpdesc[idx++] = (u32)keymod;

    /*
     * Encapsulation output must include space for blob key encryption
     * key and MAC tag
     */
    tmpdesc[idx++] = CMD_SEQ_OUT_PTR | (secretsz + BLOB_OVERHEAD);
    tmpdesc[idx++] = (u32)outbuf;

    /* Input data, should be somewhere in secure memory */
    tmpdesc[idx++] = CMD_SEQ_IN_PTR | secretsz;
    tmpdesc[idx++] = (u32)secretbuf;

    /* Set blob encap, then color */
    tmpdesc[idx] = CMD_OPERATION | OP_TYPE_ENCAP_PROTOCOL | OP_PCLID_BLOB;

    if (blobtype == SM_SECMEM)
        tmpdesc[idx] |= OP_PCL_BLOB_PTXT_SECMEM;

    if (auth == KEY_COVER_CCM)
        tmpdesc[idx] |= OP_PCL_BLOB_EKT;

    if (keycolor == BLACK_KEY)
        tmpdesc[idx] |= OP_PCL_BLOB_BLACK;

    idx++;
    tmpdesc[0] = CMD_DESC_HDR | HDR_ONE | (idx & HDR_DESCLEN_MASK);
    dsize = idx * sizeof(u32);



    memcpy(desc, tmpdesc, dsize);


    return dsize;
}

int caam_jr_enqueue(struct caam_drv_jr_info *jrp, u32 *desc);
int sm_key_job(struct caam_drv_private *ctrlpriv, u32 *jobdesc)
{

    int rtn = 0;

    rtn = caam_jr_enqueue(
            &ctrlpriv->jr_info[0],
            jobdesc);

    return rtn;
}

static int sm_set_cmd_reg(
        struct caam_drv_private_sm *smpriv,
        struct caam_drv_jr_info *jrpriv,
        u32 val)
{
    if (smpriv->sm_reg_offset == SM_V1_OFFSET) {
        struct caam_secure_mem_v1 *sm_regs_v1;
        sm_regs_v1 = (struct caam_secure_mem_v1 *)
            ((void *)jrpriv->rregs + SM_V1_OFFSET);
        wr_reg32(&sm_regs_v1->sm_cmd, val);

    } else if (smpriv->sm_reg_offset == SM_V2_OFFSET) {
        struct caam_secure_mem_v2 *sm_regs_v2;
        sm_regs_v2 = (struct caam_secure_mem_v2 *)
            ((void *)jrpriv->rregs + SM_V2_OFFSET);
        wr_reg32(&sm_regs_v2->sm_cmd, val);
    } else {
        return -AW_EINVAL;
    }

    return 0;
}

static u32 sm_get_status_reg(
        struct caam_drv_private_sm *smpriv,
        struct caam_drv_jr_info *jrpriv,
        u32 *val)
{

    if (smpriv->sm_reg_offset == SM_V1_OFFSET) {
        struct caam_secure_mem_v1 *sm_regs_v1;
        sm_regs_v1 = (struct caam_secure_mem_v1 *)
            ((void *)jrpriv->rregs + SM_V1_OFFSET);
        *val = rd_reg32(&sm_regs_v1->sm_status);
    } else if (smpriv->sm_reg_offset == SM_V2_OFFSET) {
        struct caam_secure_mem_v2 *sm_regs_v2;
        sm_regs_v2 = (struct caam_secure_mem_v2 *)
            ((void *)jrpriv->rregs + SM_V2_OFFSET);
        *val = rd_reg32(&sm_regs_v2->sm_status);
    } else {
        return -AW_EINVAL;
    }

    return 0;
}

static int kso_init_data(struct caam_drv_private *ctrlpriv, u32 unit)
{
    int     retval = -AW_EINVAL;


    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    if(unit >= SM_MAX_PAGES) {
        goto out;
    }

    smpriv->pagedesc[unit].ref_count ++;
    retval = 0;
out:
    return retval;
}

static int kso_cleanup_data(struct caam_drv_private *ctrlpriv, u32 unit)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    int     retval = -AW_EINVAL;

    if (unit >= SM_MAX_PAGES) {
        goto out;
    }

    if(0>smpriv->pagedesc[unit].ref_count) {
        smpriv->pagedesc[unit].ref_count --;
    }

out:
    return retval;
}

static void *slot_get_address(struct caam_drv_private *ctrlpriv,
                       u32 unit, u32 slot);
static int slot_alloc(struct caam_drv_private *ctrlpriv,
               u32 unit, u32 size, u32 *slot)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    struct keystore_data *ksdata = &smpriv->pagedesc[unit].ksdata;
    u32     i;
    char    *p;

    if (size > smpriv->slot_size || (0 == smpriv->pagedesc[unit].ref_count)) {
        return -AW_ENOMEM;
    }

    for (i = 0; i < SM_SLOT_COUNT_PER_PAGE; i++) {
        if (ksdata->slot[i].allocated == 0) {
            ksdata->slot[i].allocated = 1;
            (*slot) = i;
            p = slot_get_address(ctrlpriv,unit,i);
            memset(p,0,ctrlpriv->sec_mem.slot_size);
            return 0;
        }
    }

    return -AW_ENOSPC;
}

static int slot_dealloc(struct caam_drv_private *ctrlpriv, u32 unit, u32 slot)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    struct keystore_data *ksdata = &smpriv->pagedesc[unit].ksdata;
    u8  *slotdata;

    if (slot >= SM_SLOT_COUNT_PER_PAGE || (0 == smpriv->pagedesc[unit].ref_count)) {
        return -AW_EINVAL;
    }
    slotdata = ksdata->base_address + slot * smpriv->slot_size;

    if (ksdata->slot[slot].allocated == 1) {
        /* Forcibly overwrite the data from the keystore */
        memset(slotdata, 0,
               smpriv->slot_size);

        ksdata->slot[slot].allocated = 0;
        return 0;
    }

    return -AW_EINVAL;
}

static void *slot_get_address(struct caam_drv_private *ctrlpriv,
                       u32 unit, u32 slot)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    struct keystore_data *ksdata = &smpriv->pagedesc[unit].ksdata;

    if (slot >= SM_SLOT_COUNT_PER_PAGE || (0 == smpriv->pagedesc[unit].ref_count)) {
        return NULL;
    }

    return ksdata->base_address + slot * smpriv->slot_size;
}

static dma_addr_t slot_get_physical(struct caam_drv_private *ctrlpriv
                        , u32 unit, u32 slot)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    struct keystore_data *ksdata = &smpriv->pagedesc[unit].ksdata;

    if (slot >= SM_SLOT_COUNT_PER_PAGE || (0 == smpriv->pagedesc[unit].ref_count)) {
        return 0;
    }


    return ksdata->phys_address + slot * smpriv->slot_size;
}

static u32 slot_get_slot_size(struct caam_drv_private *ctrlpriv,
                       u32 unit, u32 slot)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;

    return smpriv->slot_size;
}


static void sm_init_keystore(struct caam_drv_private *ctrlpriv)
{
    //struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;

}

static int caam_sm_startup( struct caam_drv_private *ctrlpriv )
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;
    struct caam_drv_jr_info    *jrpriv; /* need this for reg page */
    u32                         page, pgstat, lpagect, smvid;
    int                         err = AW_ERROR;
    uint32_t                    max_pages;


    memset(smpriv->pagedesc,0,sizeof(smpriv->pagedesc));
    /* Set the Secure Memory Register Map Version */
    smvid = rd_reg32(&ctrlpriv->ctrl->perfmon.smvid);
    if (smvid < SMVID_V2)
        smpriv->sm_reg_offset = SM_V1_OFFSET;
    else
        smpriv->sm_reg_offset = SM_V2_OFFSET;

    /*
     * Collect configuration limit data for reference
     * This batch comes from the partition data/vid registers in perfmon
     */
    max_pages = ((rd_reg32(&ctrlpriv->ctrl->perfmon.smpart)
                & SMPART_MAX_NUMPG_MASK) >>
                SMPART_MAX_NUMPG_SHIFT) + 1;
    if ( SM_MAX_PAGES != max_pages) {
        aw_kprintf("%s@%d::error config error\r\n",__func__,
                __LINE__);
        while(1);
    }
    smpriv->page_size = 1024 << ((rd_reg32(&ctrlpriv->ctrl->perfmon.smvid)
                & SMVID_PG_SIZE_MASK) >> SMVID_PG_SIZE_SHIFT);
    if ( 0 != smpriv->page_size% SM_SLOT_COUNT_PER_PAGE) {
        aw_kprintf("%s@%d::error config error\r\n",__func__,
                __LINE__);
        while(1);
    }
    smpriv->slot_size = smpriv->page_size / SM_SLOT_COUNT_PER_PAGE;

    /*
     * Now probe for partitions/pages to which we have access. Note that
     * these have likely been set up by a bootloader or platform
     * provisioning application, so we have to assume that we "inherit"
     * a configuration and work within the constraints of what it might be.
     *
     * Assume use of the zeroth ring in the present iteration (until
     * we can divorce the controller and ring drivers, and then assign
     * an SM instance to any ring instance).
     */
    jrpriv = &ctrlpriv->jr_info[0];
    lpagect = 0;
    pgstat = 0;

    for (page = 0; page < SM_MAX_PAGES; page++) {
        if (sm_set_cmd_reg(smpriv, jrpriv,
                   ((page << SMC_PAGE_SHIFT) & SMC_PAGE_MASK) |
                   (SMC_CMD_PAGE_INQUIRY & SMC_CMD_MASK))) {
            goto cleanup;
        }

        if (sm_get_status_reg(smpriv, jrpriv, &pgstat)) {
            goto cleanup;
        }

        if (((pgstat & SMCS_PGWON_MASK) >> SMCS_PGOWN_SHIFT)
            == SMCS_PGOWN_OWNED) { /* our page? */

            smpriv->pagedesc[lpagect].ksdata.base_address = ctrlpriv->sm_base +
                ((smpriv->page_size * page) / sizeof(u32));
            smpriv->pagedesc[lpagect].ksdata.phys_address = virt_to_phys(ctrlpriv->sm_base) +
                (smpriv->page_size * page);

            lpagect++;
        }
    }

    smpriv->localpages = lpagect;
    sm_init_keystore(ctrlpriv);
    err = AW_OK;
cleanup:
    return err;
}

int caam_sm_init(struct caam_drv_private *ctrlpriv)
{
    caam_sm_startup(ctrlpriv);
    return 0;
}

int caam_sm_deinit(struct caam_drv_private *ctrlpriv)
{
    return 0;
}

u32 sm_detect_keystore_units(struct caam_drv_private *ctrlpriv)
{
    struct caam_drv_private_sm *smpriv = &ctrlpriv->sec_mem;

    return smpriv->localpages;
}

int sm_establish_keystore(struct caam_drv_private *ctrlpriv, u32 unit)
{
    /* Call the data_init function for any user setup */
    return kso_init_data(ctrlpriv, unit);
}


void sm_release_keystore(struct caam_drv_private *ctrlpriv, u32 unit)
{

    kso_cleanup_data(ctrlpriv, unit);

    return;
}

/*
 * Subsequent interfacce (sm_keystore_*) forms the accessor interfacce to
 * the keystore
 */
int sm_keystore_slot_alloc(struct caam_drv_private *ctrlpriv, u32 unit, u32 size, u32 *slot)
{

    return slot_alloc(ctrlpriv, unit, size, slot);;
}


int sm_keystore_slot_dealloc(struct caam_drv_private *ctrlpriv, u32 unit, u32 slot)
{
    return slot_dealloc(ctrlpriv, unit, slot);;
}

int sm_keystore_slot_load(struct caam_drv_private *ctrlpriv, u32 unit, u32 slot,
              const u8 *key_data, u32 key_length)
{
    int retval = -AW_EINVAL;
    u32 slot_size;
    u32 i;
    u8 __iomem *slot_location;


    slot_size = slot_get_slot_size(ctrlpriv, unit, slot);

    if (key_length > slot_size) {
        retval = -AW_EFBIG;
        goto out;
    }

    slot_location = slot_get_address(ctrlpriv, unit, slot);

    for (i = 0; i < key_length; i++)
        slot_location[i] = key_data[i];

    retval = 0;

out:
    return retval;
}

int sm_keystore_slot_read(struct caam_drv_private *ctrlpriv, u32 unit, u32 slot,
              u32 key_length, u8 *key_data)
{
    int retval = -AW_EINVAL;
    u8 __iomem *slot_addr;
    u32 slot_size;


    slot_addr = slot_get_address(ctrlpriv, unit, slot);
    slot_size = slot_get_slot_size(ctrlpriv, unit, slot);

    if (key_length > slot_size) {
        retval = -AW_EINVAL;
        goto out;
    }

    memcpy(key_data, slot_addr, key_length);
    retval = 0;

out:

    return retval;
}

/* Import a black/red key from a blob residing in external memory */
int sm_keystore_slot_import(struct caam_drv_private   *ctrlpriv,
                            u32 unit, u32 slot, u8 keycolor,
                u8 keyauth, aw_const u8 *inbuf, u16 keylen, aw_const u8 *keymod)
{

    int retval = 0;
    u8 __iomem *slotaddr, *lkeymod = NULL,*p_data_in=NULL;
    dma_addr_t slotphys;
    dma_addr_t keymod_dma, inbuf_dma;
    u32 dsize, jstat;
    u32 __iomem *decapdesc = NULL;

    /* Get the base address(es) of the specified slot */
    slotaddr = (u8 *)slot_get_address(ctrlpriv, unit, slot);
    slotaddr = slotaddr;
    slotphys = slot_get_physical(ctrlpriv, unit, slot);

    /* Build/map/flush the key modifier */
    lkeymod = aw_cache_dma_align(SECMEM_KEYMOD_LEN,128);
    memcpy(lkeymod, keymod, SECMEM_KEYMOD_LEN);
    keymod_dma = virt_to_phys(lkeymod);

    p_data_in = aw_cache_dma_align( keylen + BLOB_OVERHEAD,128);
    memcpy(p_data_in, inbuf, keylen + BLOB_OVERHEAD);
    inbuf_dma = virt_to_phys(p_data_in);

    decapdesc = aw_cache_dma_align( INITIAL_DESCSZ*2,128);
    memset(decapdesc,0,INITIAL_DESCSZ*2);

    /* Build the encapsulation job descriptor */
    dsize = blob_decap_jobdesc(decapdesc,
                               keymod_dma,
                               inbuf_dma, slotphys,
                               keylen,
                               keycolor,
                               SM_SECMEM,
                               keyauth);
    if (!dsize) {
        retval = -AW_ENOMEM;
        goto out;
    }

    jstat = sm_key_job(ctrlpriv, decapdesc);

    /*
     * May want to expand upon error meanings a bit. Any CAAM status
     * is reported as AW_EIO, but we might want to look for something more
     * meaningful for something like an ICV error on restore, otherwise
     * the caller is left guessing.
     */
    if (jstat) {
        retval = -AW_EIO;
        goto out;
    }
    retval = AW_OK;
out:
    if(NULL != lkeymod) {
        aw_cache_dma_free(lkeymod);
        lkeymod  = NULL;
    }
    if(NULL != p_data_in) {
        aw_cache_dma_free(p_data_in);
        p_data_in = NULL;
    }
    if(NULL != decapdesc) {
        aw_cache_dma_free(decapdesc);
        decapdesc = NULL;
    }

    return retval;
}

/* Export a black/red key to a blob in external memory */
int sm_keystore_slot_export(struct caam_drv_private *ctrlpriv,
                            u32 unit,
                            u32 slot,
                            u8 keycolor,
                            u8 keyauth,
                            u8 *outbuf,
                            u16 keylen,
                            aw_const u8 *keymod)
{

    int retval = 0;
    u8 __iomem *slotaddr, *lkeymod = NULL;
    dma_addr_t slotphys;
    dma_addr_t keymod_dma, outbuf_dma;
    u32 dsize, jstat;
    u32 __iomem *encapdesc = NULL;
    u8          *p_data_out = NULL;

    /* Get the base address(es) of the specified slot */
    slotaddr = (u8 *)slot_get_address(ctrlpriv, unit, slot);
    slotaddr =slotaddr;
    slotphys = slot_get_physical(ctrlpriv, unit, slot);

    /* Build/map/flush the key modifier */
    lkeymod = aw_cache_dma_align(SECMEM_KEYMOD_LEN,128);
    memcpy(lkeymod, keymod, SECMEM_KEYMOD_LEN);
    keymod_dma = virt_to_phys(lkeymod);

    p_data_out = aw_cache_dma_align( keylen + BLOB_OVERHEAD,128);
    outbuf_dma = virt_to_phys(p_data_out);

    encapdesc = aw_cache_dma_align( INITIAL_DESCSZ*2,128);
    memset(encapdesc,0,INITIAL_DESCSZ*2);


    /* Build the encapsulation job descriptor */
    dsize = blob_encap_jobdesc(encapdesc,
                               keymod_dma,
                               slotphys,
                               outbuf_dma,
                               keylen,
                               keycolor,
                               SM_SECMEM,
                               keyauth);
    if (!dsize) {
        retval = -AW_ENOMEM;
        goto out;
    }
    jstat = sm_key_job(ctrlpriv, encapdesc);
    if(jstat) {
        retval = -AW_EIO;
        goto out;
    }
    memcpy(outbuf, p_data_out, keylen + BLOB_OVERHEAD);
out:
    if(NULL != lkeymod) {
        aw_cache_dma_free(lkeymod);
        lkeymod  = NULL;
    }
    if(NULL != p_data_out) {
        aw_cache_dma_free(p_data_out);
        p_data_out = NULL;
    }
    if(NULL != encapdesc) {
        aw_cache_dma_free(encapdesc);
        encapdesc = NULL;
    }

    return retval;
}

/****************************************
 *----- Data encryption descriptor -----*
 ****************************************/
/* 1. Header
 *
 * 1011 0000 1000 0000 0000 0000 0000 1000
 * |||| | | ||||
 * ++++-+-- Header +-++++-- 8 words in descriptor
 */
#define ENCRYPT_DESC1 0xB0800008

/* 2. Load AES-128 key from secure memory
 *
 * 0000 0010 0010 0000 0000 0000 0001 0000
 * |||| |||    |
 * |||| |||    ++++ no write back
 * |||| |++--class 1 key
 * ++++-+-- key command
 */
#define ENCRYPT_DESC2 0x02200010

/* 3. Pointer to key data in secure memory */
/* Address is provided during run time */
#define ENCRYPT_DESC3 0x00000000

/* 4. Algorith Operation - Decrypt with ECB mode
 *
 * 1000 0010 0001 0000 0000 0010 0000 1101
 * |||| |||| |||| |||| |||| |||| |||| ||||
 * |||| |||| |||| |||| |||| |||| |||| |||+-- Encrypt
 * |||| |||| |||| |||| |||| |||| |||| ++-- Initialize/Finalize
 * |||| |||| |||| |||| ---+-++++-++++-- ECB mode
 * |||| |||| ++++-++++-- AES
 * |||| |+++-- Optype: Class 1 algorithm
 * ++++-+-- ALGORITHM OP.
 */
#define ENCRYPT_DESC4 0x8210020D

/* 5. Load 16 bytes of message data
 *
 * 0010 0010 0001 0010 0000 0000 0000 0000
 * |||| |||| |||| |||| |||| |||| |||| ||||
 * |||| |||| |||| |||| |||| |||| |||| ||||
 * |||| |||| |||| |||| |||| |||| |||| ||||
 * |||| |||| |||| |||| ++++-++++-++++-++++-- Msg Length
 * |||| |||| ||++-++++-- Input data type: Msg data LC1=1
 * |||| |||| |+-- EXT: No extended length
 * |||| |||| +-- IMM: data begins at the location pointed to by the next word
 * |||| |||++-SGT/VLF: FIFO Load-Pointer points to actual data
 * |||| |++-- Load FIFO with data for Class 1 CHA
 * ++++-+-- FIFO Load
 */
#define ENCRYPT_DESC5 0x22120000

/* 6. Pointer to plain text test vector message */
/* Address is provided during run time */
#define ENCRYPT_DESC6 0x00000000

/* 7. FIFO STORE - encrypted result.
 * 0110 0000 0011 0000 0000 0000 0000 0000
 * |||| |||| |||| |||| |||| |||| |||| ||||
 * |||| |||| |||| |||| ++++-++++-++++-++++-- Length
 * |||| |||| ||++-++++-- Output data type: Msg Data
 * |||| |||| |+-- EXT: No extended length
 * |||| |||| +-- CONT: No continue
 * |||| |||+-- SGT/VLF: Pointer points to actual data
 * |||| |++-- AUX: None
 * ++++-+-- FIFO Store
 */
#define ENCRYPT_DESC7 0x60300000

/* 8. Pointer to ciphered text buffer */
/* Address is provided during run time */
#define ENCRYPT_DESC8 0x00000000



static void build_aes_job_desc(
        u32 *desc,
        dma_addr_t key_slot_phys,
        uint32_t key_len,
        dma_addr_t data_in_phys,
        uint32_t data_len,
        dma_addr_t data_out_phys,
        aw_bool_t b_encrypt,
        aw_bool_t is_blacken_key
        )
{
    u32     tmpdesc[INITIAL_DESCSZ];
    u16     dsize, idx;

    memset(tmpdesc, 0, INITIAL_DESCSZ * sizeof(u32));
    idx = 1;

    /* 加载key */
    tmpdesc[idx] = CMD_KEY|CLASS_1|KEY_NWB | key_len;
    if(is_blacken_key) {
        tmpdesc[idx]|=KEY_ENC;
    }
    idx++;
    tmpdesc[idx++] = (u32)key_slot_phys;

    tmpdesc[idx] = CMD_OPERATION | OP_TYPE_CLASS1_ALG|OP_PCLID_AES
                    |OP_ALG_AAI_ECB|OP_ALG_AS_INITFINAL;

    if(b_encrypt ) {
         tmpdesc[idx] |= OP_ALG_ENCRYPT;
    }

    idx++;
    tmpdesc[idx++] = ENCRYPT_DESC5| data_len;
    tmpdesc[idx++] = (u32)data_in_phys;
    tmpdesc[idx++] = ENCRYPT_DESC7 | data_len;
    tmpdesc[idx++] = (u32)data_out_phys;

    tmpdesc[0] = CMD_DESC_HDR | HDR_ONE | (idx & HDR_DESCLEN_MASK);
    dsize = idx * sizeof(u32);
    memcpy(desc, tmpdesc, dsize);
}
/* Import a black/red key from a blob residing in external memory */
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
        )
{
    u8              *p_slot_addr;
    dma_addr_t      slot_phys;

    uint8_t         *p_data_in_align = NULL;
    uint8_t         *p_data_out_align = NULL;
    dma_addr_t      data_in_phys;
    dma_addr_t      data_out_phys;
    int             err = -AW_EINVAL;
    uint32_t        *p_job_desc = NULL;
    int             job_state = 0;

    /* 获取指定槽的地址 */
    p_slot_addr = (u8 *)slot_get_address(ctrlpriv, unit, slot);
    slot_phys = slot_get_physical(ctrlpriv, unit, slot);
    if(NULL == p_slot_addr) {
        goto cleanup;
    }

    /* 分配用于输入数据的DMA缓冲 */
    p_data_in_align = aw_cache_dma_align(data_len,128);
    if(NULL == p_data_in_align) {
        err = -AW_ENOMEM;
        goto cleanup;
    }
    memcpy(p_data_in_align, p_data_in, data_len);
    data_in_phys = virt_to_phys(p_data_in_align);

    /* 分配用于输出数据的DMA缓冲 */
    p_data_out_align = aw_cache_dma_align( data_len,128);
    if(NULL == p_data_out_align) {
        err = -AW_ENOMEM;
        goto cleanup;
    }
    data_out_phys = virt_to_phys(p_data_out_align);
    /* 分配描述符内存 */
    p_job_desc = aw_cache_dma_align( INITIAL_DESCSZ*2,128);
    if(NULL == p_job_desc) {
        err = -AW_ENOMEM;
        goto cleanup;
    }

    build_aes_job_desc(p_job_desc,
                       slot_phys,
                       key_len,
                       data_in_phys,
                       data_len,
                       data_out_phys,
                       b_encrypt,
                       is_blacken_key
                       );

    job_state = sm_key_job(ctrlpriv, p_job_desc);

    if (job_state) {
        err = -AW_EIO;
        goto cleanup;
    }
    memcpy(p_data_out,p_data_out_align,data_len);
    err = AW_OK;
cleanup:
    if(NULL != p_job_desc) {
        aw_cache_dma_free(p_job_desc);
        p_job_desc  = NULL;
    }
    if(NULL != p_data_out_align) {
        aw_cache_dma_free(p_data_out_align);
        p_data_out_align = NULL;
    }
    if(NULL != p_data_in_align) {
        aw_cache_dma_free(p_data_in_align);
        p_data_in_align = NULL;
    }

    return err;
}

static int blacken_key_jobdesc(
        u32 *desc,
        dma_addr_t key_addr_phys,
        uint32_t keysz,
        uint32_t auth
        )
{
    u32     tmpdesc[INITIAL_DESCSZ];
    u32     dsize, idx;

    memset(tmpdesc, 0, INITIAL_DESCSZ * sizeof(u32));
    idx = 1;

    /* Load key to class 1 key register */
    tmpdesc[idx++] = CMD_KEY | CLASS_1 | (keysz & KEY_LENGTH_MASK);
    tmpdesc[idx++] = (u32)key_addr_phys;

    /* ...and write back out via FIFO store*/
    tmpdesc[idx] = CMD_FIFO_STORE | CLASS_1 | (keysz & KEY_LENGTH_MASK);

    /* plus account for ECB/CCM option in FIFO_STORE */
    if (auth == KEY_COVER_ECB)
        tmpdesc[idx] |= FIFOST_TYPE_KEY_KEK;
    else
        tmpdesc[idx] |= FIFOST_TYPE_KEY_CCM_JKEK;

    idx++;
    tmpdesc[idx++] = (u32)key_addr_phys;

    /* finish off the job header */
    tmpdesc[0] = CMD_DESC_HDR | HDR_ONE | (idx & HDR_DESCLEN_MASK);
    dsize = idx * sizeof(u32);

    memcpy(desc, tmpdesc, dsize);
    return dsize;
}


/**
 * \brief 将一个key加密为blacken key
 *
 * 目前仅支持class 1 keys
 * \param   ctrlpriv    CAAM驱动上下文
 * \param   unit        要处理的key位于安全内存的page index
 * \param   slot        要处理的key所在安全内存page的slot index
 * \param   key_length  key的长度，不能超过32字节，因为密钥最大长度为256 bit
 * \param   keyauth     key的认证方式，如果选用ECM模式，则值为KEY_COVER_ECB，
 *                          否则为KEY_COVER_CCM
 * \return  失败返回负的错误码，否则返回AW_OK
 */
int sm_keystore_cover_key(
        struct caam_drv_private   *ctrlpriv,
        u32 unit,
        u32 slot,
        u32 key_length,
        KEY_AUTH_WAY keyauth)
{
    u8              *p_slot_addr;
    dma_addr_t      slot_phys;
    int             err = -AW_EINVAL;
    uint32_t        *p_job_desc = NULL;
    int             job_state = 0;

    /* 获取指定槽的地址 */
    p_slot_addr = (u8 *)slot_get_address(ctrlpriv, unit, slot);
    slot_phys = slot_get_physical(ctrlpriv, unit, slot);
    if(NULL == p_slot_addr) {
        goto cleanup;
    }

    /* 分配描述符内存 */
    p_job_desc = aw_cache_dma_align( INITIAL_DESCSZ*2,128);
    if(NULL == p_job_desc) {
        err = -AW_ENOMEM;
        goto cleanup;
    }

    blacken_key_jobdesc(p_job_desc, slot_phys, key_length, keyauth);
    job_state = sm_key_job(ctrlpriv, p_job_desc);
    if (job_state) {
        err = -AW_EIO;
        goto cleanup;
    }
    err = AW_OK;
cleanup:
    if(NULL != p_job_desc) {
        aw_cache_dma_free(p_job_desc);
        p_job_desc  = NULL;
    }
    return err;
}
