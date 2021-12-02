/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
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
 * \brief IMX6UL GPMI head file
 *
 * define IMX6UL GPMI data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-11  deo, first implementation
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_GPMI_NAND_H
#define __AWBL_IMX6UL_GPMI_NAND_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "apollo.h"
#include "aw_sem.h"
//#include "awbl_nand_bus.h"
#include "driver/nand/awbl_imx6ul_nand.h"
#include "driver/dma/awbl_imx6ul_dma.h"
#include "aw_cpu_clk.h"

/** \brief The Driver name define */
#define AWBL_IMX6UL_GPMI_NAME   "awbl_imx6ul_gpmi"

/** \brief CE ID define, The GPMI supports up to four NAND chip selects */
#define AWBL_IMX6UL_GPMI_CE0        0
#define AWBL_IMX6UL_GPMI_CE1        1
#define AWBL_IMX6UL_GPMI_CE2        2
#define AWBL_IMX6UL_GPMI_CE3        3

/** \brief IMX28 GPMI device info */
struct awbl_imx6ul_gpmi_devinfo {

	/** \brief  The nand controller information */
//    struct awbl_nandflash_controller_devinfo controller_info;

    uint8_t      nandbus_index;

    /** \brief The gpmi regbase */
    uint32_t     gpmi_regbase;

    /** \brief The bch regbase */
    uint32_t     bch_regbase;

    /** \brief get the gpmi io_clock frequency */
    aw_clk_id_t  clk_io;

    /** \brief DMA channel */
    uint32_t        dma_channel;

    /** \brief The platform */
    void (*pfn_plfm_init)(void);
};

/**
 * struct bch_geometry - BCH geometry description.
 * @gf_len:                   The length of Galois Field. (e.g., 13 or 14)
 * @ecc_strength:             A number that describes the strength of the ECC
 *                            algorithm.
 * @page_total_size:          The size, in bytes, of a physical page, including
 *                            both data and OOB.
 * @metadata_size:            The size, in bytes, of the metadata.
 * @ecc_chunk_size:           The size, in bytes, of a single ECC chunk. Note
 *                            the first chunk in the page includes both data and
 *                            metadata, so it's a bit larger than this value.
 * @ecc_chunk_count:          The number of ECC chunks in the page,
 * @payload_size:             The size, in bytes, of the payload buffer.
 * @auxiliary_size:           The size, in bytes, of the auxiliary buffer.
 * @auxiliary_status_offset:  The offset into the auxiliary buffer at which
 *                            the ECC status appears.
 * @block_mark_byte_offset:   The byte offset in the ECC-based page view at
 *                            which the underlying physical block mark appears.
 * @block_mark_bit_offset:    The bit offset into the ECC-based page view at
 *                            which the underlying physical block mark appears.
 */
struct bch_geometry {
    unsigned int  gf_len;
    unsigned int  ecc_strength;
    unsigned int  page_total_size;
    unsigned int  metadata_size;
    unsigned int  ecc_chunk_size;
    unsigned int  ecc_chunk_count;
    unsigned int  payload_size;
    unsigned int  auxiliary_size;
    unsigned int  auxiliary_status_offset;
    unsigned int  block_mark_byte_offset;
    unsigned int  block_mark_bit_offset;
    int           oob_free_len;
    unsigned int  bch_max_ecc_strength;
};

struct __imx6ul_gpmi_regs;
struct __imx6ul_bch_regs;

/** \brief IMX6UL GPMI device */
struct awbl_imx6ul_gpmi {
    struct awbl_nandbus                nandbus;
    void                              *p_cur_chip;

    struct __imx6ul_gpmi_regs          *p_gpmi_regs;
    struct __imx6ul_bch_regs           *p_bch_regs;
    awbl_imx6ul_dma_desc_t             *p_dma_desc;
    uint8_t                            *p_dma_buf;
    int                                 dma_desc_count;
    struct bch_geometry                 bch_geo;
    AW_SEMB_DECL(sem_bch_finished);

    uint8_t                            *cmd_buffer;
    int                                 command_length;

    uint8_t                            *p_mux_buf;      //多种用途的缓冲区,cache对齐
    int                                 mux_buf_len;    //多种用途缓冲区长度
    int                                 cache_line_size;
};

int imx6ul_gpmi_ecc_write_page (
        awbl_nand_platform_serv_t        *p_pf_serv,
        struct awbl_nandbus              *p_bus,
        uint32_t                          page_addr,
        uint8_t                          *p_buf,
        uint32_t                          len,
        uint8_t                          *oob_buf,
        uint32_t                          oob_size,
        aw_bool_t                            oob_required);

int imx6ul_gpmi_ecc_read_page(
        awbl_nand_platform_serv_t        *p_pf_serv,
        struct awbl_nandbus              *p_bus,
        uint32_t                          page_addr,
        uint8_t                          *p_buf,
        uint32_t                          len,
        uint8_t                          *oob_buf,
        uint32_t                          oob_size,
        aw_bool_t                            oob_required);

void awbl_imx6ul_gpmi_drv_register (void);

void awbl_imx6ul_bch_geometry_set (struct awbl_imx6ul_gpmi *p_dev,
                                   struct awbl_nand_info   *p_info);


#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif /* __AWBL_IMX28_GPMI_NAND_H */
