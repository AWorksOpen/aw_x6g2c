/*
 * CAAM hardware register-level view
 *
 * Copyright (C) 2008-2015 Freescale Semiconductor, Inc.
 */

#ifndef     __IMX6UL_CAAM_REG_H__
#define     __IMX6UL_CAAM_REG_H__

/*
 * Architecture-specific register access methods
 *
 * CAAM's bus-addressable registers are 64 bits internally.
 * They have been wired to be safely accessible on 32-bit
 * architectures, however. Registers were organized such
 * that (a) they can be contained in 32 bits, (b) if not, then they
 * can be treated as two 32-bit entities, or finally (c) if they
 * must be treated as a single 64-bit value, then this can safely
 * be done with two 32-bit cycles.
 *
 * For 32-bit operations on 64-bit values, CAAM follows the same
 * 64-bit register access conventions as it's predecessors, in that
 * writes are "triggered" by a write to the register at the numerically
 * higher address, thus, a full 64-bit write cycle requires a write
 * to the lower address, followed by a write to the higher address,
 * which will latch/execute the write cycle.
 *
 * For example, let's assume a SW reset of CAAM through the master
 * configuration register.
 * - SWRST is in bit 31 of MCFG.
 * - MCFG begins at base+0x0000.
 * - Bits 63-32 are a 32-bit word at base+0x0000 (numerically-lower)
 * - Bits 31-0 are a 32-bit word at base+0x0004 (numerically-higher)
 *
 * (and on Power, the convention is 0-31, 32-63, I know...)
 *
 * Assuming a 64-bit write to this MCFG to perform a software reset
 * would then require a write of 0 to base+0x0000, followed by a
 * write of 0x80000000 to base+0x0004, which would "execute" the
 * reset.
 *
 * Of course, since MCFG 63-32 is all zero, we could cheat and simply
 * write 0x8000000 to base+0x0004, and the reset would work fine.
 * However, since CAAM does contain some write-and-read-intended
 * 64-bit registers, this code defines 64-bit access methods for
 * the sake of internal consistency and simplicity, and so that a
 * clean transition to 64-bit is possible when it becomes necessary.
 *
 * There are limitations to this that the developer must recognize.
 * 32-bit architectures cannot enforce an atomic-64 operation,
 * Therefore:
 *
 * - On writes, since the HW is assumed to latch the cycle on the
 *   write of the higher-numeric-address word, then ordered
 *   writes work OK.
 *
 * - For reads, where a register contains a relevant value of more
 *   that 32 bits, the hardware employs logic to latch the other
 *   "half" of the data until read, ensuring an accurate value.
 *   This is of particular relevance when dealing with CAAM's
 *   performance counters.
 *
 */

typedef uint32_t    dma_addr_t;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint64_t    u64;
typedef uint8_t     u8;

#define OUTREG32(addr,b)    writel((b), (addr))
#define INREG32(addr)       readl( (addr) )
#define rd_reg32            INREG32
#define wr_reg32            OUTREG32
#define clrbits32           AW_REG_BIT_CLR_MASK32
#define setbits32           AW_REG_BIT_SET_MASK32

static inline u32 _upper_32_bits(u64 data)
{
    return (u32)( data >>32);
}

static inline u32 _lower_32_bits(u64 data)
{
    return (u32)( data & 0xFFFFFFFF);
}

static inline dma_addr_t virt_to_phys(void *p)
{
    return (dma_addr_t)p;
}

static inline void wr_reg64(u64 *addr, u64 data)
{
    u32     ms,ls,*p;
    ms = _upper_32_bits(data);
    ls = _lower_32_bits(data);

    p = (u32 *)addr;
    wr_reg32(p,ms);
    p++;
    wr_reg32(p,ls);
}


#define cpu_relax() aw_mb()


/*
 * CHA version ID / instantiation bitfields
 * Defined for use within cha_id in perfmon
 * Note that the same shift/mask selectors can be used to pull out number
 * of instantiated blocks within cha_num in perfmon, the locations are
 * the same.
 */

/* Job Ring */
#define CHA_ID_JR_SHIFT         60
#define CHA_ID_JR_MASK          (0xfull << CHA_ID_JR_SHIFT)

/* DEscriptor COntroller */
#define CHA_ID_DECO_SHIFT       56
#define CHA_ID_DECO_MASK        (0xfull << CHA_ID_DECO_SHIFT)
#define CHA_NUM_DECONUM_SHIFT   56 /* legacy definition */
#define CHA_NUM_DECONUM_MASK    (0xfull << CHA_NUM_DECONUM_SHIFT)

/* ZUC-Authentication */
#define CHA_ID_ZA_SHIFT         44
#define CHA_ID_ZA_MASK          (0xfull << CHA_ID_ZA_SHIFT)

/* ZUC-Encryption */
#define CHA_ID_ZE_SHIFT         40
#define CHA_ID_ZE_MASK          (0xfull << CHA_ID_ZE_SHIFT)

/* SNOW f9 */
#define CHA_ID_SNW9_SHIFT       36
#define CHA_ID_SNW9_MASK        (0xfull << CHA_ID_SNW9_SHIFT)

/* CRC */
#define CHA_ID_CRC_SHIFT        32
#define CHA_ID_CRC_MASK         (0xfull << CHA_ID_CRC_SHIFT)

/* Public Key */
#define CHA_ID_PK_SHIFT         28
#define CHA_ID_PK_MASK          (0xfull << CHA_ID_PK_SHIFT)

/* Kasumi */
#define CHA_ID_KAS_SHIFT        24
#define CHA_ID_KAS_MASK         (0xfull << CHA_ID_KAS_SHIFT)

/* SNOW f8 */
#define CHA_ID_SNW8_SHIFT       20
#define CHA_ID_SNW8_MASK        (0xfull << CHA_ID_SNW8_SHIFT)

/*
 * Random Generator
 * RNG4 = FIPS-verification-compliant, requires init kickstart for use
 */
#define CHA_ID_RNG_SHIFT        16
#define CHA_ID_RNG_MASK         (0xfull << CHA_ID_RNG_SHIFT)
#define CHA_ID_RNG_A            (0x1ull << CHA_ID_RNG_SHIFT)
#define CHA_ID_RNG_B            (0x2ull << CHA_ID_RNG_SHIFT)
#define CHA_ID_RNG_C            (0x3ull << CHA_ID_RNG_SHIFT)
#define CHA_ID_RNG_4            (0x4ull << CHA_ID_RNG_SHIFT)

/*
 * Message Digest
 * LP256 = Low Power (MD5/SHA1/SHA224/SHA256 + HMAC)
 * LP512 = Low Power (LP256 + SHA384/SHA512)
 * HP    = High Power (LP512 + SMAC)
 */
#define CHA_ID_MD_SHIFT         12
#define CHA_ID_MD_MASK          (0xfull << CHA_ID_MD_SHIFT)
#define CHA_ID_MD_LP256         (0x0ull << CHA_ID_MD_SHIFT)
#define CHA_ID_MD_LP512         (0x1ull << CHA_ID_MD_SHIFT)
#define CHA_ID_MD_HP            (0x2ull << CHA_ID_MD_SHIFT)

/* ARC4 Streamcipher */
#define CHA_ID_ARC4_SHIFT       8
#define CHA_ID_ARC4_MASK        (0xfull << CHA_ID_ARC4_SHIFT)
#define CHA_ID_ARC4_LP          (0x0ull << CHA_ID_ARC4_SHIFT)
#define CHA_ID_ARC4_HP          (0x1ull << CHA_ID_ARC4_SHIFT)

/* DES Blockcipher Accelerator */
#define CHA_ID_DES_SHIFT        4
#define CHA_ID_DES_MASK         (0xfull << CHA_ID_DES_SHIFT)

/*
 * AES Blockcipher + Combo Mode Accelerator
 * LP = Low Power (includes ECB/CBC/CFB128/OFB/CTR/CCM/CMAC/XCBC-MAC)
 * HP = High Power (LP + CBCXCBC/CTRXCBC/XTS/GCM)
 * DIFFPWR = ORed in if differential-power-analysis resistance implemented
 */
#define CHA_ID_AES_SHIFT        0
#define CHA_ID_AES_MASK         (0xfull << CHA_ID_AES_SHIFT)
#define CHA_ID_AES_LP           (0x3ull << CHA_ID_AES_SHIFT)
#define CHA_ID_AES_HP           (0x4ull << CHA_ID_AES_SHIFT)
#define CHA_ID_AES_DIFFPWR      (0x1ull << CHA_ID_AES_SHIFT)


/*
 * caam_perfmon - Performance Monitor/Secure Memory Status/
 *                CAAM Global Status/Component Version IDs
 *
 * Spans f00-fff wherever instantiated
 */

/* Number of DECOs */
#define CHA_NUM_DECONUM_SHIFT       56
#define CHA_NUM_DECONUM_MASK        (0xfull << CHA_NUM_DECONUM_SHIFT)

struct sec_vid {
    uint16_t                ip_id;
    uint8_t                 maj_rev;
    uint8_t                 min_rev;
};

#define SEC_VID_IPID_SHIFT          16
#define SEC_VID_MAJ_SHIFT           8
#define SEC_VID_MAJ_MASK            0xFF00

struct caam_perfmon {
    /* Performance Monitor Registers                f00-f9f */
    uint64_t                req_dequeued;           /* PC_REQ_DEQ - Dequeued Requests	     */
    uint64_t                ob_enc_req;             /* PC_OB_ENC_REQ - Outbound Encrypt Requests */
    uint64_t                ib_dec_req;             /* PC_IB_DEC_REQ - Inbound Decrypt Requests  */
    uint64_t                ob_enc_bytes;           /* PC_OB_ENCRYPT - Outbound Bytes Encrypted  */
    uint64_t                ob_prot_bytes;          /* PC_OB_PROTECT - Outbound Bytes Protected  */
    uint64_t                ib_dec_bytes;           /* PC_IB_DECRYPT - Inbound Bytes Decrypted   */
    uint64_t                ib_valid_bytes;         /* PC_IB_VALIDATED Inbound Bytes Validated   */
    uint64_t                rsvd[13];

    /* CAAM Hardware Instantiation Parameters       fa0-fbf */
    uint64_t                cha_rev;                /* CRNR - CHA Revision Number		*/

#define CTPR_MS_QI_MASK             (0x1 << 25)
    uint32_t                ctpr_ms;                /* CTPR - Compile Parameters Register	*/
    uint32_t                ctpr_ls;

    /* Secure Memory State Visibility */
    uint32_t                rsvd1;
    uint32_t                smstatus;               /* Secure memory status */
    uint32_t                rsvd2;
    uint32_t                smpartown;              /* Secure memory partition owner */

    /* CAAM Global Status                           fc0-fdf */
    uint64_t                faultaddr;              /* FAR  - Fault Address		*/
    uint32_t                faultliodn;             /* FALR - Fault Address LIODN	*/
    uint32_t                faultdetail;            /* FADR - Fault Addr Detail	*/
    uint32_t                rsvd3;
    uint32_t                status;                 /* CSTA - CAAM Status */
    uint32_t                smpart;                 /* Secure Memory Partition Parameters */
    uint32_t                smvid;                  /* Secure Memory Version ID */

    /* Component Instantiation Parameters           fe0-fff */
    uint32_t                rtic_id;                /* RVID - RTIC Version ID	*/
    uint32_t                ccb_id;                 /* CCBVID - CCB Version ID	*/
    uint32_t                cha_id_ms;              /* CHAVID - CHA Version ID	*/
    uint32_t                cha_id_ls;
    uint64_t                cha_num;                /* CHANUM - CHA Number		*/
    uint64_t                caam_id;                /* CAAMVID - CAAM Version ID	*/
};

#define SMSTATUS_PART_SHIFT         28
#define SMSTATUS_PART_MASK          (0xf << SMSTATUS_PART_SHIFT)
#define SMSTATUS_PAGE_SHIFT         16
#define SMSTATUS_PAGE_MASK          (0x7ff << SMSTATUS_PAGE_SHIFT)
#define SMSTATUS_MID_SHIFT          8
#define SMSTATUS_MID_MASK           (0x3f << SMSTATUS_MID_SHIFT)
#define SMSTATUS_ACCERR_SHIFT       4
#define SMSTATUS_ACCERR_MASK        (0xf << SMSTATUS_ACCERR_SHIFT)
#define SMSTATUS_ACCERR_NONE        0
#define SMSTATUS_ACCERR_ALLOC       1               /* Page not allocated */
#define SMSTATUS_ACCESS_ID          2               /* Not granted by ID */
#define SMSTATUS_ACCESS_WRITE       3               /* Writes not allowed */
#define SMSTATUS_ACCESS_READ        4               /* Reads not allowed */
#define SMSTATUS_ACCESS_NONKEY      6               /* Non-key reads not allowed */
#define SMSTATUS_ACCESS_BLOB        9               /* Blob access not allowed */
#define SMSTATUS_ACCESS_DESCB       10              /* Descriptor Blob access spans pages */
#define SMSTATUS_ACCESS_NON_SM      11              /* Outside Secure Memory range */
#define SMSTATUS_ACCESS_XPAGE       12              /* Access crosses pages */
#define SMSTATUS_ACCESS_INITPG      13              /* Page still initializing */
#define SMSTATUS_STATE_SHIFT        0
#define SMSTATUS_STATE_MASK         (0xf << SMSTATUS_STATE_SHIFT)
#define SMSTATUS_STATE_RESET        0
#define SMSTATUS_STATE_INIT         1
#define SMSTATUS_STATE_NORMAL       2
#define SMSTATUS_STATE_FAIL         3

/* up to 15 rings, 2 bits shifted by ring number */
#define SMPARTOWN_RING_SHIFT        2
#define SMPARTOWN_RING_MASK         3
#define SMPARTOWN_AVAILABLE         0
#define SMPARTOWN_NOEXIST           1
#define SMPARTOWN_UNAVAILABLE       2
#define SMPARTOWN_OURS              3

/* Maximum number of pages possible */
#define SMPART_MAX_NUMPG_SHIFT      16
#define SMPART_MAX_NUMPG_MASK       (0x3f << SMPART_MAX_NUMPG_SHIFT)

/* Maximum partition number */
#define SMPART_MAX_PNUM_SHIFT       12
#define SMPART_MAX_PNUM_MASK        (0xf << SMPART_MAX_PNUM_SHIFT)

/* Highest possible page number */
#define SMPART_MAX_PG_SHIFT         0
#define SMPART_MAX_PG_MASK          (0x3f << SMPART_MAX_PG_SHIFT)

/* Max size of a page */
#define SMVID_PG_SIZE_SHIFT         16
#define SMVID_PG_SIZE_MASK          (0x7 << SMVID_PG_SIZE_SHIFT)

/* Major/Minor Version ID */
#define SMVID_MAJ_VERS_SHIFT        8
#define SMVID_MAJ_VERS              (0xf << SMVID_MAJ_VERS_SHIFT)
#define SMVID_MIN_VERS_SHIFT        0
#define SMVID_MIN_VERS              (0xf << SMVID_MIN_VERS_SHIFT)

/* LIODN programming for DMA configuration */
#define MSTRID_LOCK_LIODN           0x80000000
#define MSTRID_LOCK_MAKETRUSTED     0x00010000      /* only for JR masterid */

#define MSTRID_LIODN_MASK           0x0fff
struct masterid {
    uint32_t                liodn_ms;               /* lock and make-trusted control bits */
    uint32_t                liodn_ls;               /* LIODN for non-sequence and seq access */
};

/* Partition ID for DMA configuration */
struct partid {
    uint32_t                rsvd1;
    uint32_t                pidr;                   /* partition ID, DECO */
};

/* RNGB test mode (replicated twice in some configurations) */
/* Padded out to 0x100 */
struct rngtst {
    uint32_t                mode;                   /* RTSTMODEx - Test mode */
    uint32_t                rsvd1[3];
    uint32_t                reset;                  /* RTSTRESETx - Test reset control */
    uint32_t                rsvd2[3];
    uint32_t                status;                 /* RTSTSSTATUSx - Test status */
    uint32_t                rsvd3;
    uint32_t                errstat;                /* RTSTERRSTATx - Test error status */
    uint32_t                rsvd4;
    uint32_t                errctl;                 /* RTSTERRCTLx - Test error control */
    uint32_t                rsvd5;
    uint32_t                entropy;                /* RTSTENTROPYx - Test entropy */
    uint32_t                rsvd6[15];
    uint32_t                verifctl;               /* RTSTVERIFCTLx - Test verification control */
    uint32_t                rsvd7;
    uint32_t                verifstat;              /* RTSTVERIFSTATx - Test verification status */
    uint32_t                rsvd8;
    uint32_t                verifdata;              /* RTSTVERIFDx - Test verification data */
    uint32_t                rsvd9;
    uint32_t                xkey;                   /* RTSTXKEYx - Test XKEY */
    uint32_t                rsvd10;
    uint32_t                oscctctl;               /* RTSTOSCCTCTLx - Test osc. counter control */
    uint32_t                rsvd11;
    uint32_t                oscct;                  /* RTSTOSCCTx - Test oscillator counter */
    uint32_t                rsvd12;
    uint32_t                oscctstat;              /* RTSTODCCTSTATx - Test osc counter status */
    uint32_t                rsvd13[2];
    uint32_t                ofifo[4];               /* RTSTOFIFOx - Test output FIFO */
    uint32_t                rsvd14[15];
};

/* RNG4 TRNG test registers */
struct rng4tst {
#define RTMCTL_PRGM                 0x00010000      /* 1 -> program mode, 0 -> run mode */
    uint32_t                rtmctl;	                /* misc. control register */
    uint32_t                rtscmisc;               /* statistical check misc. register */
    uint32_t                rtpkrrng;               /* poker range register */
    union {
        uint32_t            rtpkrmax;               /* PRGM=1: poker max. limit register */
        uint32_t            rtpkrsq;                /* PRGM=0: poker square calc. result register */
    };
#define RTSDCTL_ENT_DLY_SHIFT       16
#define RTSDCTL_ENT_DLY_MASK        (0xffff << RTSDCTL_ENT_DLY_SHIFT)
#define RTSDCTL_ENT_DLY_MIN         3200
#define RTSDCTL_ENT_DLY_MAX         12800
    uint32_t                rtsdctl;                /* seed control register */
    union {
        uint32_t            rtsblim;                /* PRGM=1: sparse bit limit register */
        uint32_t            rttotsam;               /* PRGM=0: total samples register */
    };
    uint32_t                rtfrqmin;               /* frequency count min. limit register */
    union {
        uint32_t            rtfrqmax;               /* PRGM=1: freq. count max. limit register */
        uint32_t            rtfrqcnt;               /* PRGM=0: freq. count register */
    };
    uint32_t                rsvd1[40];
#define RDSTA_SKVT                  0x80000000
#define RDSTA_SKVN                  0x40000000
#define RDSTA_IF0                   0x00000001
#define RDSTA_IF1                   0x00000002
#define RDSTA_IFMASK                (RDSTA_IF1 | RDSTA_IF0)
    uint32_t                rdsta;
    uint32_t                rsvd2[15];
};

/*
 * caam_ctrl - basic core configuration
 * starts base + 0x0000 padded out to 0x1000
 */

#define KEK_KEY_SIZE                8
#define TKEK_KEY_SIZE               8
#define TDSK_KEY_SIZE               8

#define DECO_RESET                  1               /* Use with DECO reset/availability regs */
#define DECO_RESET_0                (DECO_RESET << 0)
#define DECO_RESET_1                (DECO_RESET << 1)
#define DECO_RESET_2                (DECO_RESET << 2)
#define DECO_RESET_3                (DECO_RESET << 3)
#define DECO_RESET_4                (DECO_RESET << 4)

struct caam_ctrl {
    /* Basic Configuration Section          000-01f */
    /* Read/Writable                        */
    uint32_t                rsvd1;
    uint32_t                mcr;                    /* MCFG      Master Config Register  */
    uint32_t                rsvd2;
    uint32_t                scfgr;                  /* SCFGR, Security Config Register */

    /* Bus Access Configuration Section     010-11f */
    /* Read/Writable                                                */
    struct masterid         jr_mid[4];              /* JRxLIODNR - JobR LIODN setup */
    uint32_t                rsvd3[12];
    struct masterid         rtic_mid[4];            /* RTICxLIODNR - RTIC LIODN setup */
    uint32_t                rsvd4[7];
    uint32_t                deco_rq;                /* DECORR - DECO Request */
    struct partid           deco_mid[5];            /* DECOxLIODNR - 1 per DECO */
    uint32_t                rsvd5[22];

    /* DECO Availability/Reset Section      20-3ff */
    uint32_t                deco_avail;             /* DAR - DECO availability */
    uint32_t                deco_reset;             /* DRR - DECO reset */
    uint32_t                rsvd6[182];

    /* Key Encryption/Decryption Configuration              400-5ff */
    /* Read/Writable only while in Non-secure mode                  */
    uint32_t                 kek[KEK_KEY_SIZE];     /* JDKEKR - Key Encryption Key */
    uint32_t                tkek[TKEK_KEY_SIZE];    /* TDKEKR - Trusted Desc KEK */
    uint32_t                tdsk[TDSK_KEY_SIZE];    /* TDSKR - Trusted Desc Signing Key */
    uint32_t                rsvd7[32];
    uint64_t                sknonce;                /* SKNR - Secure Key Nonce */
    uint32_t                rsvd8[70];

    /* RNG Test/Verification/Debug Access                   600-7ff */
    /* (Useful in Test/Debug modes only...)                         */
    union {
        struct rngtst       rtst[2];
        struct rng4tst      r4tst[2];
    };

    uint32_t                rsvd9[448];

    /* Performance Monitor f00-fff */
    struct caam_perfmon     perfmon;
};

/*
 * Controller master config register defs
 */
#define MCFGR_SWRESET               0x80000000      /* software reset */
#define MCFGR_WDENABLE              0x40000000      /* DECO watchdog enable */
#define MCFGR_WDFAIL                0x20000000      /* DECO watchdog force-fail */
#define MCFGR_DMA_RESET             0x10000000
#define MCFGR_LONG_PTR              0x00010000      /* Use >32-bit desc addressing */
#define SCFGR_RDBENABLE             0x00000400
#define DECORR_RQD0ENABLE           0x00000001      /* Enable DECO0 for direct access */
#define DECORR_DEN0                 0x00010000      /* DECO0 available for access*/

/* AXI read cache control */
#define MCFGR_ARCACHE_SHIFT         12
#define MCFGR_ARCACHE_MASK          (0xf << MCFGR_ARCACHE_SHIFT)

/* AXI write cache control */
#define MCFGR_AWCACHE_SHIFT         8
#define MCFGR_AWCACHE_MASK          (0xf << MCFGR_AWCACHE_SHIFT)

/* AXI pipeline depth */
#define MCFGR_AXIPIPE_SHIFT         4
#define MCFGR_AXIPIPE_MASK          (0xf << MCFGR_AXIPIPE_SHIFT)

#define MCFGR_AXIPRI                0x00000008      /* Assert AXI priority sideband */
#define MCFGR_BURST_64              0x00000001      /* Max burst size */

/* Secure Memory Configuration - if you have it */
/* Secure Memory Register Offset from JR Base Reg*/
#define SM_V1_OFFSET                0x0f4
#define SM_V2_OFFSET                0xa00

/* Minimum SM Version ID requiring v2 SM register mapping */
#define SMVID_V2                    0x20105

struct caam_secure_mem_v1 {
    uint32_t                    sm_cmd;             /* SMCJRx - Secure memory command */
    uint32_t                    rsvd1;
    uint32_t                    sm_status;          /* SMCSJRx - Secure memory status */

    uint32_t                    sm_perm;            /* SMAPJRx - Secure memory access perms */
    uint32_t                    sm_group2;          /* SMAP2JRx - Secure memory access group 2 */
    uint32_t                    sm_group1;          /* SMAP1JRx - Secure memory access group 1 */
};

struct caam_secure_mem_v2 {
    uint32_t                    sm_perm;            /* SMAPJRx - Secure memory access perms */
    uint32_t                    sm_group2;          /* SMAP2JRx - Secure memory access group 2 */
    uint32_t                    sm_group1;          /* SMAP1JRx - Secure memory access group 1 */
    uint32_t                    rsvd1[118];
    uint32_t                    sm_cmd;             /* SMCJRx - Secure memory command */
    uint32_t                    rsvd2;
    uint32_t                    sm_status;          /* SMCSJRx - Secure memory status */
};

/*
 * caam_job_ring - direct job ring setup
 * 1-4 possible per instantiation, base + 1000/2000/3000/4000
 * Padded out to 0x1000
 */
struct caam_job_ring {
    /* Input ring */
    uint64_t                    inpring_base;       /* IRBAx -  Input desc ring baseaddr */
    uint32_t                    rsvd1;
    uint32_t                    inpring_size;       /* IRSx - Input ring size */
    uint32_t                    rsvd2;
    uint32_t                    inpring_avail;      /* IRSAx - Input ring room remaining */
    uint32_t                    rsvd3;
    uint32_t                    inpring_jobadd;     /* IRJAx - Input ring jobs added */

    /* Output Ring */
    uint64_t                    outring_base;       /* ORBAx - Output status ring base addr */
    uint32_t                    rsvd4;
    uint32_t                    outring_size;       /* ORSx - Output ring size */
    uint32_t                    rsvd5;
    uint32_t                    outring_rmvd;       /* ORJRx - Output ring jobs removed */
    uint32_t                    rsvd6;
    uint32_t                    outring_used;       /* ORSFx - Output ring slots full */

    /* Status/Configuration */
    uint32_t                    rsvd7;
    uint32_t                    jroutstatus;        /* JRSTAx - JobR output status */
    uint32_t                    rsvd8;
    uint32_t                    jrintstatus;        /* JRINTx - JobR interrupt status */
    uint32_t                    rconfig_hi;         /* JRxCFG - Ring configuration */
    uint32_t                    rconfig_lo;

    /* Indices. CAAM maintains as "heads" of each queue */
    uint32_t                    rsvd9;
    uint32_t                    inp_rdidx;          /* IRRIx - Input ring read index */
    uint32_t                    rsvd10;
    uint32_t                    out_wtidx;          /* ORWIx - Output ring write index */

    /* Command/control */
    uint32_t                    rsvd11;
    uint32_t                    jrcommand;          /* JRCRx - JobR command */
    uint32_t                    rsvd12[931];

    /* Performance Monitor f00-fff */
    struct caam_perfmon         perfmon;
};

#define JR_RINGSIZE_MASK            0x03ff
/*
 * jrstatus - Job Ring Output Status
 * All values in lo word
 * Also note, same values written out as status through QI
 * in the command/status field of a frame descriptor
 */
#define JRSTA_SSRC_SHIFT            28
#define JRSTA_SSRC_MASK             0xf0000000

#define JRSTA_SSRC_NONE             0x00000000
#define JRSTA_SSRC_CCB_ERROR        0x20000000
#define JRSTA_SSRC_JUMP_HALT_USER   0x30000000
#define JRSTA_SSRC_DECO             0x40000000
#define JRSTA_SSRC_JRERROR          0x60000000
#define JRSTA_SSRC_JUMP_HALT_CC     0x70000000

#define JRSTA_DECOERR_JUMP          0x08000000
#define JRSTA_DECOERR_INDEX_SHIFT   8
#define JRSTA_DECOERR_INDEX_MASK    0xff00
#define JRSTA_DECOERR_ERROR_MASK    0x00ff

#define JRSTA_DECOERR_NONE          0x00
#define JRSTA_DECOERR_LINKLEN       0x01
#define JRSTA_DECOERR_LINKPTR       0x02
#define JRSTA_DECOERR_JRCTRL        0x03
#define JRSTA_DECOERR_DESCCMD       0x04
#define JRSTA_DECOERR_ORDER         0x05
#define JRSTA_DECOERR_KEYCMD        0x06
#define JRSTA_DECOERR_LOADCMD       0x07
#define JRSTA_DECOERR_STORECMD      0x08
#define JRSTA_DECOERR_OPCMD         0x09
#define JRSTA_DECOERR_FIFOLDCMD     0x0a
#define JRSTA_DECOERR_FIFOSTCMD     0x0b
#define JRSTA_DECOERR_MOVECMD       0x0c
#define JRSTA_DECOERR_JUMPCMD       0x0d
#define JRSTA_DECOERR_MATHCMD       0x0e
#define JRSTA_DECOERR_SHASHCMD      0x0f
#define JRSTA_DECOERR_SEQCMD        0x10
#define JRSTA_DECOERR_DECOINTERNAL  0x11
#define JRSTA_DECOERR_SHDESCHDR     0x12
#define JRSTA_DECOERR_HDRLEN        0x13
#define JRSTA_DECOERR_BURSTER       0x14
#define JRSTA_DECOERR_DESCSIGNATURE 0x15
#define JRSTA_DECOERR_DMA           0x16
#define JRSTA_DECOERR_BURSTFIFO     0x17
#define JRSTA_DECOERR_JRRESET       0x1a
#define JRSTA_DECOERR_JOBFAIL       0x1b
#define JRSTA_DECOERR_DNRERR        0x80
#define JRSTA_DECOERR_UNDEFPCL      0x81
#define JRSTA_DECOERR_PDBERR        0x82
#define JRSTA_DECOERR_ANRPLY_LATE   0x83
#define JRSTA_DECOERR_ANRPLY_REPLAY 0x84
#define JRSTA_DECOERR_SEQOVF        0x85
#define JRSTA_DECOERR_INVSIGN       0x86
#define JRSTA_DECOERR_DSASIGN       0x87

#define JRSTA_CCBERR_JUMP           0x08000000
#define JRSTA_CCBERR_INDEX_MASK     0xff00
#define JRSTA_CCBERR_INDEX_SHIFT    8
#define JRSTA_CCBERR_CHAID_MASK     0x00f0
#define JRSTA_CCBERR_CHAID_SHIFT    4
#define JRSTA_CCBERR_ERRID_MASK     0x000f

#define JRSTA_CCBERR_CHAID_AES      (0x01 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_DES      (0x02 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_ARC4     (0x03 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_MD       (0x04 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_RNG      (0x05 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_SNOW     (0x06 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_KASUMI   (0x07 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_PK       (0x08 << JRSTA_CCBERR_CHAID_SHIFT)
#define JRSTA_CCBERR_CHAID_CRC      (0x09 << JRSTA_CCBERR_CHAID_SHIFT)

#define JRSTA_CCBERR_ERRID_NONE     0x00
#define JRSTA_CCBERR_ERRID_MODE     0x01
#define JRSTA_CCBERR_ERRID_DATASIZ  0x02
#define JRSTA_CCBERR_ERRID_KEYSIZ   0x03
#define JRSTA_CCBERR_ERRID_PKAMEMSZ 0x04
#define JRSTA_CCBERR_ERRID_PKBMEMSZ 0x05
#define JRSTA_CCBERR_ERRID_SEQUENCE 0x06
#define JRSTA_CCBERR_ERRID_PKDIVZRO 0x07
#define JRSTA_CCBERR_ERRID_PKMODEVN 0x08
#define JRSTA_CCBERR_ERRID_KEYPARIT 0x09
#define JRSTA_CCBERR_ERRID_ICVCHK   0x0a
#define JRSTA_CCBERR_ERRID_HARDWARE 0x0b
#define JRSTA_CCBERR_ERRID_CCMAAD   0x0c
#define JRSTA_CCBERR_ERRID_INVCHA   0x0f

#define JRINT_ERR_INDEX_MASK        0x3fff0000
#define JRINT_ERR_INDEX_SHIFT       16
#define JRINT_ERR_TYPE_MASK         0xf00
#define JRINT_ERR_TYPE_SHIFT        8
#define JRINT_ERR_HALT_MASK         0xc
#define JRINT_ERR_HALT_SHIFT        2
#define JRINT_ERR_HALT_INPROGRESS   0x4
#define JRINT_ERR_HALT_COMPLETE     0x8
#define JRINT_JR_ERROR              0x02
#define JRINT_JR_INT                0x01

#define JRINT_ERR_TYPE_WRITE        1
#define JRINT_ERR_TYPE_BAD_INPADDR  3
#define JRINT_ERR_TYPE_BAD_OUTADDR  4
#define JRINT_ERR_TYPE_INV_INPWRT   5
#define JRINT_ERR_TYPE_INV_OUTWRT   6
#define JRINT_ERR_TYPE_RESET        7
#define JRINT_ERR_TYPE_REMOVE_OFL   8
#define JRINT_ERR_TYPE_ADD_OFL      9

#define JRCFG_SOE                   0x04
#define JRCFG_ICEN                  0x02
#define JRCFG_IMSK                  0x01
#define JRCFG_ICDCT_SHIFT           8
#define JRCFG_ICTT_SHIFT            16

#define JRCR_RESET                  0x01

/* secure memory command */
#define SMC_PAGE_SHIFT              16
#define SMC_PAGE_MASK               (0xffff << SMC_PAGE_SHIFT)
#define SMC_PART_SHIFT              8
#define SMC_PART_MASK               (0x0f << SMC_PART_SHIFT)
#define SMC_CMD_SHIFT               0
#define SMC_CMD_MASK                (0x0f << SMC_CMD_SHIFT)

#define SMC_CMD_ALLOC_PAGE          0x01    /* allocate page to this partition */
#define SMC_CMD_DEALLOC_PAGE        0x02    /* deallocate page from partition */
#define SMC_CMD_DEALLOC_PART        0x03    /* deallocate partition */
#define SMC_CMD_PAGE_INQUIRY        0x05    /* find partition associate with page */

/* secure memory (command) status */
#define SMCS_PAGE_SHIFT             16
#define SMCS_PAGE_MASK              (0x0fff << SMCS_PAGE_SHIFT)
#define SMCS_CMDERR_SHIFT           14
#define SMCS_CMDERR_MASK            (3 << SMCS_CMDERR_SHIFT)
#define SMCS_ALCERR_SHIFT           12
#define SMCS_ALCERR_MASK            (3 << SMCS_ALCERR_SHIFT)
#define SMCS_PGOWN_SHIFT            6
#define SMCS_PGWON_MASK             (3 << SMCS_PGOWN_SHIFT)
#define SMCS_PART_SHIFT             0
#define SMCS_PART_MASK              (0xf << SMCS_PART_SHIFT)

#define SMCS_CMDERR_NONE            0
#define SMCS_CMDERR_INCOMP          1       /* Command not yet complete */
#define SMCS_CMDERR_SECFAIL         2       /* Security failure occurred */
#define SMCS_CMDERR_OVERFLOW        3       /* Command overflow */

#define SMCS_ALCERR_NONE            0
#define SMCS_ALCERR_PSPERR          1       /* Partion marked PSP (dealloc only) */
#define SMCS_ALCERR_PAGEAVAIL       2       /* Page not available */
#define SMCS_ALCERR_PARTOWN         3       /* Partition ownership error */

#define SMCS_PGOWN_AVAIL            0       /* Page is available */
#define SMCS_PGOWN_NOEXIST          1       /* Page initializing or nonexistent */
#define SMCS_PGOWN_NOOWN            2       /* Page owned by another processor */
#define SMCS_PGOWN_OWNED            3       /* Page belongs to this processor */

/* secure memory access permissions */
#define SMCS_PERM_KEYMOD_SHIFT      16
#define SMCA_PERM_KEYMOD_MASK       (0xff << SMCS_PERM_KEYMOD_SHIFT)
#define SMCA_PERM_CSP_ZERO          0x8000  /* Zero when deallocated or released */
#define SMCA_PERM_PSP_LOCK          0x4000  /* Part./pages can't be deallocated */
#define SMCA_PERM_PERM_LOCK         0x2000  /* Lock permissions */
#define SMCA_PERM_GRP_LOCK          0x1000  /* Lock access groups */
#define SMCA_PERM_RINGID_SHIFT      10
#define SMCA_PERM_RINGID_MASK       (3 << SMCA_PERM_RINGID_SHIFT)
#define SMCA_PERM_G2_BLOB           0x0080  /* Group 2 blob import/export */
#define SMCA_PERM_G2_WRITE          0x0020  /* Group 2 write */
#define SMCA_PERM_G2_READ           0x0010  /* Group 2 read */
#define SMCA_PERM_G1_BLOB           0x0008  /* Group 1... */
#define SMCA_PERM_G1_WRITE          0x0002
#define SMCA_PERM_G1_READ           0x0001

/*
 * caam_assurance - Assurance Controller View
 * base + 0x6000 padded out to 0x1000
 */

struct rtic_element {
    uint64_t                    address;
    uint32_t                    rsvd;
    uint32_t                    length;
};

struct rtic_block {
    struct rtic_element         element[2];
};

struct rtic_memhash {
    uint32_t                    memhash_be[32];
    uint32_t                    memhash_le[32];
};

struct caam_assurance {
    /* Status/Command/Watchdog */
    uint32_t                    rsvd1;
    uint32_t                    status;             /* RSTA - Status */
    uint32_t                    rsvd2;
    uint32_t                    cmd;                /* RCMD - Command */
    uint32_t                    rsvd3;
    uint32_t                    ctrl;               /* RCTL - Control */
    uint32_t                    rsvd4;
    uint32_t                    throttle;           /* RTHR - Throttle */
    uint32_t                    rsvd5[2];
    uint64_t                    watchdog;           /* RWDOG - Watchdog Timer */
    uint32_t                    rsvd6;
    uint32_t                    rend;               /* REND - Endian corrections */
    uint32_t                    rsvd7[50];

    /* Block access/configuration @ 100/110/120/130 */
    struct rtic_block           memblk[4];          /* Memory Blocks A-D */
    uint32_t                    rsvd8[32];

    /* Block hashes @ 200/300/400/500 */
    struct rtic_memhash         hash[4];            /* Block hash values A-D */
    uint32_t                    rsvd_3[640];
};

/*
 * caam_queue_if - QI configuration and control
 * starts base + 0x7000, padded out to 0x1000 long
 */

struct caam_queue_if {
    uint32_t                    qi_control_hi;      /* QICTL  - QI Control */
    uint32_t                    qi_control_lo;
    uint32_t                    rsvd1;
    uint32_t                    qi_status;          /* QISTA  - QI Status */
    uint32_t                    qi_deq_cfg_hi;      /* QIDQC  - QI Dequeue Configuration */
    uint32_t                    qi_deq_cfg_lo;
    uint32_t                    qi_enq_cfg_hi;      /* QISEQC - QI Enqueue Command     */
    uint32_t                    qi_enq_cfg_lo;
    uint32_t                    rsvd2[1016];
};

/* QI control bits - low word */
#define QICTL_DQEN              0x01                /* Enable frame pop          */
#define QICTL_STOP              0x02                /* Stop dequeue/enqueue      */
#define QICTL_SOE               0x04                /* Stop on error             */

/* QI control bits - high word */
#define QICTL_MBSI              0x01
#define QICTL_MHWSI             0x02
#define QICTL_MWSI              0x04
#define QICTL_MDWSI             0x08
#define QICTL_CBSI              0x10                /* CtrlDataByteSwapInput     */
#define QICTL_CHWSI             0x20                /* CtrlDataHalfSwapInput     */
#define QICTL_CWSI              0x40                /* CtrlDataWordSwapInput     */
#define QICTL_CDWSI             0x80                /* CtrlDataDWordSwapInput    */
#define QICTL_MBSO              0x0100
#define QICTL_MHWSO             0x0200
#define QICTL_MWSO              0x0400
#define QICTL_MDWSO             0x0800
#define QICTL_CBSO              0x1000              /* CtrlDataByteSwapOutput    */
#define QICTL_CHWSO             0x2000              /* CtrlDataHalfSwapOutput    */
#define QICTL_CWSO              0x4000              /* CtrlDataWordSwapOutput    */
#define QICTL_CDWSO             0x8000              /* CtrlDataDWordSwapOutput   */
#define QICTL_DMBS              0x010000
#define QICTL_EPO               0x020000

/* QI status bits */
#define QISTA_PHRDERR           0x01                /* PreHeader Read Error      */
#define QISTA_CFRDERR           0x02                /* Compound Frame Read Error */
#define QISTA_OFWRERR           0x04                /* Output Frame Read Error   */
#define QISTA_BPDERR            0x08                /* Buffer Pool Depleted      */
#define QISTA_BTSERR            0x10                /* Buffer Undersize          */
#define QISTA_CFWRERR           0x20                /* Compound Frame Write Err  */
#define QISTA_STOPD             0x80000000          /* QI Stopped (see QICTL)    */

/* deco_sg_table - DECO view of scatter/gather table */
struct deco_sg_table {
    uint64_t                    addr;               /* Segment Address */
    uint32_t                    elen;               /* E, F bits + 30-bit length */
    uint32_t                    bpid_offset;        /* Buffer Pool ID + 16-bit length */
};

/*
 * caam_deco - descriptor controller - CHA cluster block
 *
 * Only accessible when direct DECO access is turned on
 * (done in DECORR, via MID programmed in DECOxMID
 *
 * 5 typical, base + 0x8000/9000/a000/b000
 * Padded out to 0x1000 long
 */
struct caam_deco {
    uint32_t                    rsvd1;
    uint32_t                    cls1_mode;          /* CxC1MR -  Class 1 Mode */
    uint32_t                    rsvd2;
    uint32_t                    cls1_keysize;       /* CxC1KSR - Class 1 Key Size */
    uint32_t                    cls1_datasize_hi;   /* CxC1DSR - Class 1 Data Size */
    uint32_t                    cls1_datasize_lo;
    uint32_t                    rsvd3;
    uint32_t                    cls1_icvsize;       /* CxC1ICVSR - Class 1 ICV size */
    uint32_t                    rsvd4[5];
    uint32_t                    cha_ctrl;           /* CCTLR - CHA control */
    uint32_t                    rsvd5;
    uint32_t                    irq_crtl;           /* CxCIRQ - CCB interrupt done/error/clear */
    uint32_t                    rsvd6;
    uint32_t                    clr_written;        /* CxCWR - Clear-Written */
    uint32_t                    ccb_status_hi;      /* CxCSTA - CCB Status/Error */
    uint32_t                    ccb_status_lo;
    uint32_t                    rsvd7[3];
    uint32_t                    aad_size;           /* CxAADSZR - Current AAD Size */
    uint32_t                    rsvd8;
    uint32_t                    cls1_iv_size;       /* CxC1IVSZR - Current Class 1 IV Size */
    uint32_t                    rsvd9[7];
    uint32_t                    pkha_a_size;        /* PKASZRx - Size of PKHA A */
    uint32_t                    rsvd10;
    uint32_t                    pkha_b_size;        /* PKBSZRx - Size of PKHA B */
    uint32_t                    rsvd11;
    uint32_t                    pkha_n_size;        /* PKNSZRx - Size of PKHA N */
    uint32_t                    rsvd12;
    uint32_t                    pkha_e_size;        /* PKESZRx - Size of PKHA E */
    uint32_t                    rsvd13[24];
    uint32_t                    cls1_ctx[16];       /* CxC1CTXR - Class 1 Context @100 */
    uint32_t                    rsvd14[48];
    uint32_t                    cls1_key[8];        /* CxC1KEYR - Class 1 Key @200 */
    uint32_t                    rsvd15[121];
    uint32_t                    cls2_mode;          /* CxC2MR - Class 2 Mode */
    uint32_t                    rsvd16;
    uint32_t                    cls2_keysize;       /* CxX2KSR - Class 2 Key Size */
    uint32_t                    cls2_datasize_hi;   /* CxC2DSR - Class 2 Data Size */
    uint32_t                    cls2_datasize_lo;
    uint32_t                    rsvd17;
    uint32_t                    cls2_icvsize;       /* CxC2ICVSZR - Class 2 ICV Size */
    uint32_t                    rsvd18[56];
    uint32_t                    cls2_ctx[18];       /* CxC2CTXR - Class 2 Context @500 */
    uint32_t                    rsvd19[46];
    uint32_t                    cls2_key[32];       /* CxC2KEYR - Class2 Key @600 */
    uint32_t                    rsvd20[84];
    uint32_t                    inp_infofifo_hi;    /* CxIFIFO - Input Info FIFO @7d0 */
    uint32_t                    inp_infofifo_lo;
    uint32_t                    rsvd21[2];
    uint64_t                    inp_datafifo;       /* CxDFIFO - Input Data FIFO */
    uint32_t                    rsvd22[2];
    uint64_t                    out_datafifo;       /* CxOFIFO - Output Data FIFO */
    uint32_t                    rsvd23[2];
    uint32_t                    jr_ctl_hi;          /* CxJRR - JobR Control Register @800 */
    uint32_t                    jr_ctl_lo;
    uint64_t                    jr_descaddr;        /* CxDADR - JobR Descriptor Address */
#define DECO_OP_STATUS_HI_ERR_MASK  0xF00000FF
    uint32_t                    op_status_hi;       /* DxOPSTA - DECO Operation Status */
    uint32_t                    op_status_lo;
    uint32_t                    rsvd24[2];
    uint32_t                    liodn;              /* DxLSR - DECO LIODN Status - non-seq */
    uint32_t                    td_liodn;           /* DxLSR - DECO LIODN Status - trustdesc */
    uint32_t                    rsvd26[6];
    uint64_t                    math[4];            /* DxMTH - Math register */
    uint32_t                    rsvd27[8];
    struct deco_sg_table        gthr_tbl[4];        /* DxGTR - Gather Tables */
    uint32_t                    rsvd28[16];
    struct deco_sg_table        sctr_tbl[4];        /* DxSTR - Scatter Tables */
    uint32_t                    rsvd29[48];
    uint32_t                    descbuf[64];        /* DxDESB - Descriptor buffer */
    uint32_t                    rscvd30[193];
#define DESC_DBG_DECO_STAT_HOST_ERR 0x00D00000
#define DESC_DBG_DECO_STAT_VALID    0x80000000
#define DESC_DBG_DECO_STAT_MASK     0x00F00000
    uint32_t                    desc_dbg;           /* DxDDR - DECO Debug Register */
    uint32_t                    rsvd31[126];
};

#define DECO_JQCR_WHL               0x20000000
#define DECO_JQCR_FOUR              0x10000000

/*
 * Current top-level view of memory map is:
 *
 * 0x0000 - 0x0fff - CAAM Top-Level Control
 * 0x1000 - 0x1fff - Job Ring 0
 * 0x2000 - 0x2fff - Job Ring 1
 * 0x3000 - 0x3fff - Job Ring 2
 * 0x4000 - 0x4fff - Job Ring 3
 * 0x5000 - 0x5fff - (unused)
 * 0x6000 - 0x6fff - Assurance Controller
 * 0x7000 - 0x7fff - Queue Interface
 * 0x8000 - 0x8fff - DECO-CCB 0
 * 0x9000 - 0x9fff - DECO-CCB 1
 * 0xa000 - 0xafff - DECO-CCB 2
 * 0xb000 - 0xbfff - DECO-CCB 3
 * 0xc000 - 0xcfff - DECO-CCB 4
 *
 * caam_full describes the full register view of CAAM if useful,
 * although many configurations may choose to implement parts of
 * the register map separately, in differing privilege regions
 */
struct caam_full {
    struct caam_ctrl volatile   ctrl;
    struct caam_job_ring        jr[4];
    uint64_t                    rsvd[512];
    struct caam_assurance       assure;
    struct caam_queue_if        qi;
    struct caam_deco            deco;
};

#endif /* REGS_H */
