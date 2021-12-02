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
/**
 * \file
 * \brief imx6ul nandflash driver source file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 20-04-14 vih, refactoring
 * \endinternal
 */

#include "apollo.h"
#include "awbl_nand_ecc.h"

#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_cache.h"
#include "driver/busctlr/awbl_imx6ul_gpmi.h"
#include "driver/nand/awbl_imx6ul_nand.h"
#include "awbl_nand.h"

#include "awbl_plb.h"
#include "mtd/aw_mtd.h"
#include "aw_bitops.h"

#include "aw_mem.h"
#include "string.h"
#include "stdio.h"

#include "aw_assert.h"

/******************************************************************************/
#define __ASSERT(x, ...)  aw_assert(x)
#define __ASSERT_D(a, cond, b)  aw_assert((a cond b))

/******************************************************************************/
aw_local void __oob_free_get (awbl_nand_info_t  *p_info,
                              uint8_t           *oob_buf_src,
                              uint16_t           free_ofs,
                              uint8_t           *oob_free_buf,
                              uint16_t           free_buf_size)
{
    const struct aw_mtd_oob_space *free = p_info->p_nand_ooblayout->ptr_freepos;
    uint32_t                boffs = 0;
    size_t                  bytes = 0;
    int                     free_cnt = p_info->p_nand_ooblayout->cnt_freepos;
    uint16_t                len   = free_buf_size;

    memset(oob_free_buf, 0xFF, free_buf_size);
    /* 提取free区的数据 */
    for (; free->length && len && free_cnt; free++, len -= bytes, free_cnt--) {
        /* Read request not from offset 0? */
        if (free_ofs) {
            if (free_ofs >= free->length) {
                free_ofs -= free->length;
                continue;
            }
            boffs = free->offset + free_ofs;
            bytes = min(len, (free->length - free_ofs));
            free_ofs = 0;
        } else {
            bytes = min(len, free->length);
            boffs = free->offset;
        }
        memcpy(oob_free_buf, oob_buf_src + boffs, bytes);
        oob_free_buf += bytes;
    }
    __ASSERT_D(len, ==, 0);
}

aw_local void __oob_free_put (awbl_nand_info_t  *p_info,
                              uint8_t           *oob_free_src,
                              uint16_t           free_size,
                              uint16_t           free_ofs,
                              uint8_t           *oob_buf)
{
    const struct aw_mtd_oob_space *free = p_info->p_nand_ooblayout->ptr_freepos;
    uint32_t boffs = 0;
    uint32_t woffs = free_ofs;  /* oob_buf 的存放的起始偏移 */
    size_t   bytes = 0;
    int      free_cnt = p_info->p_nand_ooblayout->cnt_freepos;
    uint16_t len = free_size;

    for (; free->length && len && free_cnt; free++, len -= bytes, free_cnt--) {
        /* Write request not from offset 0? */
        if (unlikely(woffs)) {
            if (woffs >= free->length) {
                woffs -= free->length;
                continue;
            }
            boffs = free->offset + woffs;
            bytes = min(len, (free->length - woffs));
            woffs = 0;
        } else {
            bytes = min(len, free->length);
            boffs = free->offset;
        }
        memcpy(oob_buf + boffs, oob_free_src, bytes);
        oob_free_src += bytes;
    }
    __ASSERT_D(len, ==, 0);
}

/******************************************************************************/
aw_local int __hw_page_write (awbl_nand_ecc_t           *p_this,
                              uint8_t                   *buf,
                              uint8_t                   *oob_free,
                              uint16_t                   oob_free_size,
                              aw_bool_t                  oob_required,
                              uint32_t                   page)
{
    aw_err_t ret;

    /* 默认全为 0xFF，保护 坏块标记和未使用区域 */
    memset(p_this->buffers->oob_buf, 0xff, p_this->p_info->attr.spare_size);

    if (oob_required) {
        __oob_free_put(p_this->p_info,
                       oob_free,
                       oob_free_size,
                       0,
                       p_this->buffers->oob_buf);
    }
    ret = imx6ul_gpmi_ecc_write_page(p_this->p_pf_serv,
                                     p_this->p_pf_serv->p_bus,
                                     page,
                                     buf,
                                     p_this->p_info->attr.page_size,
                                     p_this->buffers->oob_buf,
                                     p_this->p_info->attr.spare_size,
                                     oob_required);
    if (ret != AW_OK) {
        AW_INFOF(("[%s:%d] ret:%d\n", __func__, __LINE__, ret));
        return ret;
    }

    return AW_OK;
}

aw_local int __hw_page_read (awbl_nand_ecc_t            *p_this,
                             uint8_t                    *buf,
                             uint8_t                    *oob_free,
                             uint16_t                    oob_free_size,
                             aw_bool_t                   oob_required,
                             uint32_t                    page)
{
    aw_err_t ret;
    ret =  imx6ul_gpmi_ecc_read_page(p_this->p_pf_serv,
                                     p_this->p_pf_serv->p_bus,
                                     page,
                                     buf,
                                     p_this->p_info->attr.page_size,
                                     p_this->buffers->oob_buf,
                                     p_this->p_info->attr.spare_size,
                                     oob_required);
    if (ret != AW_OK) {
        /* AW_INFOF(("[%s:%d] ret:%d\n", __func__, __LINE__, ret)); */
        return ret;
    }
    if (oob_required) {
        __oob_free_get(p_this->p_info,
                       p_this->buffers->oob_buf,
                       0,
                       oob_free,
                       oob_free_size);
    }

    return AW_OK;
}

/******************************************************************************/
/* tag init */
aw_local awbl_nand_ecc_t *__imx6ul_platform_hw_ecc_create (
        awbl_nand_ecc_init_t *p_init)
{
    awbl_nand_ecc_t             *p_ecc;
    awbl_nand_platform_serv_t   *p_pf_serv  = p_init->p_pf_serv;
    awbl_nand_info_t            *p_info     = p_init->p_info;

    aw_assert(p_init && p_init->p_info && p_init->p_pf_serv );

    p_ecc = malloc(sizeof(*p_ecc));
    aw_assert(p_ecc);
    memset(p_ecc, 0, sizeof(*p_ecc));

    p_ecc->p_info    = p_init->p_info;
    p_ecc->p_pf_serv = p_init->p_pf_serv;
    /* 初始化 buffer */
    {
        awbl_nand_ecc_buffers_t    *nbuf;

        nbuf = (awbl_nand_ecc_buffers_t *)malloc(
                    sizeof(*nbuf) + p_info->attr.spare_size * 2);
        aw_assert(nbuf);

        nbuf->oob_buf   = (uint8_t *)(nbuf + 1);
        nbuf->calc_ecc  = nbuf->oob_buf  + p_info->attr.spare_size;

        p_ecc->buffers = nbuf;

        /* Set the internal oob buffer location */
//        p_nand->p_oob_poi        = p_nand->buffers->oob_buf;
    }

    aw_assert(p_pf_serv->p_bus);

    /* 设置 bch 参数 */
    {
        struct awbl_imx6ul_gpmi *p_gpmi =
               AW_CONTAINER_OF(p_pf_serv->p_bus,
                               struct awbl_imx6ul_gpmi,
                               nandbus);
        struct bch_geometry   *bch_geo = &p_gpmi->bch_geo;

        awbl_imx6ul_bch_geometry_set(p_gpmi, p_pf_serv->p_info);

        p_ecc->size          = bch_geo->ecc_chunk_size;
        p_ecc->steps         = bch_geo->ecc_chunk_count;
#if 0
        p_ecc->total_bytes   = AW_ROUND_UP((bch_geo->ecc_strength *
                                                 bch_geo->gf_len) / 8 ,
                                                 4);
#else
        p_ecc->total_bytes   = (bch_geo->ecc_strength * bch_geo->gf_len
                                        * bch_geo->ecc_chunk_count + 7)/8 +
                                          bch_geo->metadata_size - 1 ;
#endif
        p_ecc->bytes         = p_ecc->total_bytes / p_ecc->steps;

    }

    /* 设置硬件 ecc 读写回调 */
    {
        awbl_nand_ecc_itf_def_set(p_ecc);
        p_ecc->itf.read_page      = __hw_page_read;
        p_ecc->itf.write_page     = __hw_page_write;
    }

    {
        int                          i;
        uint16_t                     ecc_space = 0;

        __ASSERT(p_ecc->total_bytes != 0, "");
        /* 检测总的 ecc bytes 区够不够 */
        if (p_info->ecc_mode != AWBL_NAND_ECC_MODE_NONE) {
            for (i = 0; i < p_info->p_nand_ooblayout->cnt_eccpos; i++) {
                ecc_space += p_info->p_nand_ooblayout->ptr_eccpos[i].length;
            }

            __ASSERT(ecc_space >= p_ecc->total_bytes,
                     "ecc_space = %d, p_nand->ecc.total_bytes = %d",
                     ecc_space,
                     p_ecc->total_bytes);
        }

        if (ecc_space) {
            p_ecc->ecc_buf = (uint8_t *)malloc(ecc_space);
            __ASSERT(p_ecc->ecc_buf, "malloc() = NULL");
            memset(p_ecc->ecc_buf, 0, ecc_space);
        }
    }

    return p_ecc;
}

aw_local awbl_nand_platform_serv_t *__platform_serv_create (
        awbl_nand_info_t    *p_info,
        struct awbl_nandbus *p_bus)
{
    awbl_nand_platform_serv_t *p_pf_serv;
    aw_assert(p_bus);

    p_pf_serv = malloc(sizeof(*p_pf_serv));
    aw_assert(p_pf_serv);
    memset(p_pf_serv, 0, sizeof(*p_pf_serv));

    awbl_nand_serv_default_set(p_pf_serv);
    p_pf_serv->p_info = p_info;
    p_pf_serv->p_bus  = p_bus;
    return p_pf_serv;
}

aw_local void __platform_serv_destroy (awbl_nand_platform_serv_t *p_this)
{
    aw_assert(p_this);
    free(p_this);
}

void awbl_imx6ul_nand_init (void)
{
    awbl_nand_ecc_register(AWBL_NAND_ECC_MODE_HW,
                           __imx6ul_platform_hw_ecc_create);
    awbl_nand_ecc_register(AWBL_NAND_ECC_MODE_HW_OOBFIRST,
                           __imx6ul_platform_hw_ecc_create);

    awbl_nand_platform_serv_register(__platform_serv_create,
                                     __platform_serv_destroy);
}
