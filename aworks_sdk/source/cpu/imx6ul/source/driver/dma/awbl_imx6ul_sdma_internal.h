#ifndef __AWBL_IMX6UL_SDMA_INTERNAL_H__
#define __AWBL_IMX6UL_SDMA_INTERNAL_H__

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


struct sdma_firmware_header {
    uint32_t magic;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t script_addrs_start;
    uint32_t num_script_addrs;
    uint32_t ram_code_start;
    uint32_t ram_code_size;
};

/**
 * struct sdma_script_start_addrs - SDMA script start pointers
 *
 * start addresses of the different functions in the physical
 * address space of the SDMA engine.
 */
struct sdma_script_start_addrs {
    int32_t ap_2_ap_addr;
    int32_t ap_2_bp_addr;
    int32_t ap_2_ap_fixed_addr;
    int32_t bp_2_ap_addr;
    int32_t loopback_on_dsp_side_addr;
    int32_t mcu_interrupt_only_addr;
    int32_t firi_2_per_addr;
    int32_t firi_2_mcu_addr;
    int32_t per_2_firi_addr;
    int32_t mcu_2_firi_addr;
    int32_t uart_2_per_addr;
    int32_t uart_2_mcu_addr;
    int32_t per_2_app_addr;
    int32_t mcu_2_app_addr;
    int32_t per_2_per_addr;
    int32_t uartsh_2_per_addr;
    int32_t uartsh_2_mcu_addr;
    int32_t per_2_shp_addr;
    int32_t mcu_2_shp_addr;
    int32_t ata_2_mcu_addr;
    int32_t mcu_2_ata_addr;
    int32_t app_2_per_addr;
    int32_t app_2_mcu_addr;
    int32_t shp_2_per_addr;
    int32_t shp_2_mcu_addr;
    int32_t mshc_2_mcu_addr;
    int32_t mcu_2_mshc_addr;
    int32_t spdif_2_mcu_addr;
    int32_t mcu_2_spdif_addr;
    int32_t asrc_2_mcu_addr;
    int32_t ext_mem_2_ipu_addr;
    int32_t descrambler_addr;
    int32_t dptc_dvfs_addr;
    int32_t utra_addr;
    int32_t ram_code_start_addr;
    /* End of v1 array */
    int32_t mcu_2_ssish_addr;
    int32_t ssish_2_mcu_addr;
    int32_t hdmi_dma_addr;
    /* End of v2 array */
    int32_t zcanfd_2_mcu_addr;
    int32_t zqspi_2_mcu_addr;
    int32_t mcu_2_ecspi_addr;
    /* End of v3 array */
    int32_t mcu_2_zqspi_addr;
    /* End of v4 array */
};


/*
 * Mode/Count of data node descriptors - IPCv2
 */
struct sdma_mode_count {
    uint32_t count   : 16; /* size of the buffer pointed by this BD */
    uint32_t status  :  8; /* E,R,I,C,W,D status bits stored here */
    uint32_t command :  8; /* command mostlky used for channel 0 */
};


/*
 * Buffer descriptor
 */
struct sdma_buffer_descriptor {
    struct sdma_mode_count  mode;
    uint32_t buffer_addr;    /* address of the buffer described */
    uint32_t ext_buffer_addr;    /* extended buffer address */
} __attribute__ ((packed));


/**
 * struct sdma_channel_control - Channel control Block
 *
 * @current_bd_ptr  current buffer descriptor processed
 * @base_bd_ptr     first element of buffer descriptor array
 * @unused      padding. The SDMA engine expects an array of 128 byte
 *          control blocks
 */
struct sdma_channel_control {
    uint32_t current_bd_ptr;
    uint32_t base_bd_ptr;
    uint32_t unused[2];
} __attribute__ ((packed));


/**
 * struct sdma_state_registers - SDMA context for a channel
 *
 * @pc:     program counter
 * @t:      test bit: status of arithmetic & test instruction
 * @rpc:    return program counter
 * @sf:     source fault while loading data
 * @spc:    loop start program counter
 * @df:     destination fault while storing data
 * @epc:    loop end program counter
 * @lm:     loop mode
 */
struct sdma_state_registers {
    uint32_t pc     :14;
    uint32_t unused1: 1;
    uint32_t t      : 1;
    uint32_t rpc    :14;
    uint32_t unused0: 1;
    uint32_t sf     : 1;
    uint32_t spc    :14;
    uint32_t unused2: 1;
    uint32_t df     : 1;
    uint32_t epc    :14;
    uint32_t lm     : 2;
} __attribute__ ((packed));


/**
 * struct sdma_context_data - sdma context specific to a channel
 *
 * @channel_state:  channel state bits
 * @gReg:       general registers
 * @mda:        burst dma destination address register
 * @msa:        burst dma source address register
 * @ms:         burst dma status register
 * @md:         burst dma data register
 * @pda:        peripheral dma destination address register
 * @psa:        peripheral dma source address register
 * @ps:         peripheral dma status register
 * @pd:         peripheral dma data register
 * @ca:         CRC polynomial register
 * @cs:         CRC accumulator register
 * @dda:        dedicated core destination address register
 * @dsa:        dedicated core source address register
 * @ds:         dedicated core status register
 * @dd:         dedicated core data register
 */
struct sdma_context_data {
    struct sdma_state_registers  channel_state;
    uint32_t  gReg[8];
    uint32_t  mda;
    uint32_t  msa;
    uint32_t  ms;
    uint32_t  md;
    uint32_t  pda;
    uint32_t  psa;
    uint32_t  ps;
    uint32_t  pd;
    uint32_t  ca;
    uint32_t  cs;
    uint32_t  dda;
    uint32_t  dsa;
    uint32_t  ds;
    uint32_t  dd;
    uint32_t  scratch0;
    uint32_t  scratch1;
    uint32_t  scratch2;
    uint32_t  scratch3;
    uint32_t  scratch4;
    uint32_t  scratch5;
    uint32_t  scratch6;
    uint32_t  scratch7;
} __attribute__ ((packed));


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


