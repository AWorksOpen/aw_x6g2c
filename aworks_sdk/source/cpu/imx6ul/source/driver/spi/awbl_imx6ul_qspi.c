/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Ti KeyStone Architecture SPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-02-24  sss, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_list.h"

#include "aw_assert.h"

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_spibus.h"

#include "driver/spi/awbl_imx6ul_qspi.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_cache.h"

#include <string.h>

/*******************************************************************************
  SPI State and Event define
*******************************************************************************/
/*
 * SPI Controller State
 */

#define __QSPI_ST_IDLE               0       /* idle state */
#define __QSPI_ST_MSG_START          1       /* message start */
#define __QSPI_ST_TRANS_START        2       /* transfer start */
#define __QSPI_ST_M_SEND_DATA        3       /* master send data */
#define __QSPI_ST_M_RECV_DATA        4       /* master receive data */

/*
 * SPI Controller Event
 *
 * Event has 32-bits, bit[15:0] event number, bit[31:16] event parameter
 */

#define __QSPI_EVT_NUM_GET(event)    ((event) & 0xFFFF)
#define __QSPI_EVT_PAR_GET(event)    ((event >> 16) & 0xFFFF)

#define __QSPI_EVT(evt_num, evt_par) (((evt_num) & 0xFFFF) | ((evt_par) << 16))

/* no event */
#define __QSPI_EVT_NONE               __QSPI_EVT(0, 0)

/* launch message transfer */
#define __QSPI_EVT_MSG_LAUNCH         __QSPI_EVT(1, 0)

/* launch transfers */
#define __QSPI_EVT_TRANS_LAUNCH       __QSPI_EVT(2, 0)

/* receive data */
#define __QSPI_EVT_M_SEND_DATA        __QSPI_EVT(3, 0)

/* receive data */
#define __QSPI_EVT_M_RECV_DATA        __QSPI_EVT(4, 0)

/* udelay */
#define __QSPI_EVT_UDELAY_DONE        __QSPI_EVT(5, 0)



/*******************************************************************************
  macro operate
*******************************************************************************/

/* write register */
#define __QSPI_REG_WRITE(p_devinfo, offset, data) \
    AW_REG_WRITE32((p_devinfo)->regbase + (offset), (data))

/* read register */
#define __QSPI_REG_READ(p_devinfo, offset) \
    AW_REG_READ32((p_devinfo)->regbase + (offset))

/* bit is set ? */
#define __QSPI_REG_BIT_ISSET(p_devinfo, offset, bit) \
    AW_REG_BIT_ISSET32((p_devinfo)->regbase + (offset), bit)

/* set bit */
#define __QSPI_REG_BIT_SET(p_devinfo, offset, bit) \
    AW_REG_BIT_SET32((p_devinfo)->regbase + (offset), bit)

/* clear bit */
#define __QSPI_REG_BIT_CLR(p_devinfo, offset, bit) \
    AW_REG_BIT_CLR32((p_devinfo)->regbase + (offset), bit)

/* get register bits */
#define __QSPI_REG_BITS_GET(p_devinfo, offset, start, len) \
    AW_REG_BITS_GET32((p_devinfo)->regbase + (offset), start, len)

/* set register bits */
#define __QSPI_REG_BITS_SET(p_devinfo, offset, start, len, value) \
    AW_REG_BITS_SET32((p_devinfo)->regbase + (offset), start, len, value)

/* declare lpc11xx spi device instance */
#define __QSPI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_qspi_dev *p_this = \
        (struct awbl_imx6ul_qspi_dev *)(p_dev)

/* declare lpc11xx spi device infomation */
#define __QSPI_DEVINFO_DECL(p_this_dvif, p_dev) \
    struct awbl_imx6ul_qspi_devinfo *p_this_dvif = \
        (struct awbl_imx6ul_qspi_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get lpc11xx spi device infomation */
#define __QSPI_DEVINFO_GET(p_dev) \
    ((struct awbl_imx6ul_qspi_devinfo *)AWBL_DEVINFO_GET(p_dev))

/* get spi master instance */
#define __QSPI_MASTER_DEV_DECL(p_master, p_dev) \
    struct awbl_spi_master *p_master = (struct awbl_spi_master *)(p_dev)

/* get spi master infomation */
#define __QSPI_MASTER_DEVINFO_DECL(p_master_dvif, p_dev) \
    struct awbl_spi_master_devinfo *p_master_dvif = \
        (struct awbl_spi_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get current message */
#define __qspi_cur_msg(p_this) \
    ((p_this)->p_cur_msg)

/* check if transfers empty */
#define __qspi_trans_empty(p_this) \
    ((p_this)->p_cur_trans == NULL)

/* get current transfer */
#define __qspi_cur_trans(p_this) \
    ((p_this)->p_cur_trans)

/* check if data pointer beyond the max value */
#define __qspi_data_ptr_ov(p_this) \
    ((p_this)->data_ptr >= (p_this)->p_cur_trans->nbytes)

/* check if data pointer point to last position */
#define __qspi_data_ptr_last(p_this) \
    ((p_this)->data_ptr == (p_this)->p_cur_trans->nbytes - 1)

/* current data */
#define __qspi_cur_data(p_this) \
    ((p_this)->p_cur_trans->p_buf[(p_this)->data_ptr])

/*************************************************************************
  Registers Definition
**************************************************************************/
#define __QSPI_AHBMAP_BANK_MAXSIZE      0x04000000

/* The registers */
#define __QSPI_MCR                      0x00
#define __QSPI_MCR_DQS_PHASE_EN_SHIFT   30
#define __QSPI_MCR_DQS_PHASE_EN_MASK   (1 << __QSPI_MCR_DQS_PHASE_EN_SHIFT)
#define __QSPI_MCR_RESERVED_SHIFT       16
#define __QSPI_MCR_RESERVED_MASK       (0xF << __QSPI_MCR_RESERVED_SHIFT)
#define __QSPI_MCR_MDIS_SHIFT           14
#define __QSPI_MCR_MDIS_MASK           (1 << __QSPI_MCR_MDIS_SHIFT)
#define __QSPI_MCR_CLR_TXF_SHIFT        11
#define __QSPI_MCR_CLR_TXF_MASK        (1 << __QSPI_MCR_CLR_TXF_SHIFT)
#define __QSPI_MCR_CLR_RXF_SHIFT        10
#define __QSPI_MCR_CLR_RXF_MASK        (1 << __QSPI_MCR_CLR_RXF_SHIFT)
#define __QSPI_MCR_DDR_EN_SHIFT         7
#define __QSPI_MCR_DDR_EN_MASK         (1 << __QSPI_MCR_DDR_EN_SHIFT)
#define __QSPI_MCR_DQS_EN_SHIFT         6
#define __QSPI_MCR_DQS_EN_MASK         (1 << __QSPI_MCR_DQS_EN_SHIFT)
#define __QSPI_MCR_END_CFG_SHIFT        2
#define __QSPI_MCR_END_CFG_MASK        (3 << __QSPI_MCR_END_CFG_SHIFT)
#define __QSPI_MCR_SWRSTHD_SHIFT        1
#define __QSPI_MCR_SWRSTHD_MASK        (1 << __QSPI_MCR_SWRSTHD_SHIFT)
#define __QSPI_MCR_SWRSTSD_SHIFT        0
#define __QSPI_MCR_SWRSTSD_MASK        (1 << __QSPI_MCR_SWRSTSD_SHIFT)

#define __QSPI_IPCR                     0x08
#define __QSPI_IPCR_SEQID_SHIFT         24
#define __QSPI_IPCR_SEQID_MASK         (0xF << __QSPI_IPCR_SEQID_SHIFT)

#define __QSPI_FLSHCR                   0x0c
#define __QSPI_FLSHCR_TDH_SHIFT         16
#define __QSPI_FLSHCR_TDH_MASK         (3 << __QSPI_FLSHCR_TDH_SHIFT)
#define __QSPI_FLSHCR_TDH_DDR_EN       (1 << __QSPI_FLSHCR_TDH_SHIFT)

#define __QSPI_BUF0CR                   0x10
#define __QSPI_BUF1CR                   0x14
#define __QSPI_BUF2CR                   0x18
#define __QSPI_BUFXCR_INVALID_MSTRID    0xe

#define __QSPI_BUF3CR                   0x1c
#define __QSPI_BUF3CR_ALLMST_SHIFT      31
#define __QSPI_BUF3CR_ALLMST_MASK      (1 << __QSPI_BUF3CR_ALLMST_SHIFT)
#define __QSPI_BUF3CR_ADATSZ_SHIFT      8
#define __QSPI_BUF3CR_ADATSZ_MASK      (0xFF << __QSPI_BUF3CR_ADATSZ_SHIFT)

#define __QSPI_BFGENCR                  0x20
#define __QSPI_BFGENCR_PAR_EN_SHIFT     16
#define __QSPI_BFGENCR_PAR_EN_MASK     (1 << (__QSPI_BFGENCR_PAR_EN_SHIFT))
#define __QSPI_BFGENCR_SEQID_SHIFT      12
#define __QSPI_BFGENCR_SEQID_MASK      (0xF << __QSPI_BFGENCR_SEQID_SHIFT)

#define __QSPI_BUF0IND                  0x30
#define __QSPI_BUF1IND                  0x34
#define __QSPI_BUF2IND                  0x38
#define __QSPI_SFAR                     0x100

#define __QSPI_SMPR                     0x108
#define __QSPI_SMPR_DDRSMP_SHIFT        16
#define __QSPI_SMPR_DDRSMP_MASK        (7 << __QSPI_SMPR_DDRSMP_SHIFT)
#define __QSPI_SMPR_FSDLY_SHIFT         6
#define __QSPI_SMPR_FSDLY_MASK         (1 << __QSPI_SMPR_FSDLY_SHIFT)
#define __QSPI_SMPR_FSPHS_SHIFT         5
#define __QSPI_SMPR_FSPHS_MASK         (1 << __QSPI_SMPR_FSPHS_SHIFT)
#define __QSPI_SMPR_HSENA_SHIFT         0
#define __QSPI_SMPR_HSENA_MASK         (1 << __QSPI_SMPR_HSENA_SHIFT)
//#define __QSPI_SMPR_SDRSMP_SHIFT        5
//#define __QSPI_SMPR_SDRSMP_MASK        (3 << __QSPI_SMPR_SDRSMP_SHIFT)

#define __QSPI_RBSR                     0x10c
#define __QSPI_RBSR_RDBFL_SHIFT         8
#define __QSPI_RBSR_RDBFL_MASK         (0x3F << __QSPI_RBSR_RDBFL_SHIFT)

#define __QSPI_RBCT                     0x110
#define __QSPI_RBCT_WMRK_MASK           0x1F
#define __QSPI_RBCT_RXBRD_SHIFT         8
#define __QSPI_RBCT_RXBRD_USEIPS       (0x1 << __QSPI_RBCT_RXBRD_SHIFT)

#define __QSPI_TBSR                     0x150
#define __QSPI_TBDR                     0x154
#define __QSPI_SR                       0x15c
#define __QSPI_SR_IP_ACC_SHIFT          1
#define __QSPI_SR_IP_ACC_MASK          (0x1 << __QSPI_SR_IP_ACC_SHIFT)
#define __QSPI_SR_AHB_ACC_SHIFT         2
#define __QSPI_SR_AHB_ACC_MASK         (0x1 << __QSPI_SR_AHB_ACC_SHIFT)


#define __QSPI_FR                       0x160
#define __QSPI_FR_TFF_MASK              0x1

#define __QSPI_SPTRCLR                  0x16C
#define __QSPI_SPTRCLR_BFPTRC_SHIFT     1
#define __QSPI_SPTRCLR_BFPTRC_MASK     (0x1 << __QSPI_SPTRCLR_BFPTRC_SHIFT)

#define __QSPI_SFA1AD                   0x180
#define __QSPI_SFA2AD                   0x184
#define __QSPI_SFB1AD                   0x188
#define __QSPI_SFB2AD                   0x18c
#define __QSPI_RBDR                     0x200

#define __QSPI_LUTKEY                   0x300
#define __QSPI_LUTKEY_VALUE             0x5AF05AF0

#define __QSPI_LCKCR                    0x304
#define __QSPI_LCKER_LOCK               0x1
#define __QSPI_LCKER_UNLOCK             0x2

#define __QSPI_RSER                     0x164
#define __QSPI_RSER_TFIE               (0x1 << 0)

#define __QSPI_LUT_BASE                 0x310

/*
 * The definition of the LUT register shows below:
 *
 *  ---------------------------------------------------
 *  | INSTR1 | PAD1 | OPRND1 | INSTR0 | PAD0 | OPRND0 |
 *  ---------------------------------------------------
 */
#define __QSPI_OPRND0_SHIFT        0
#define __QSPI_PAD0_SHIFT          8
#define __QSPI_INSTR0_SHIFT        10
#define __QSPI_OPRND1_SHIFT        16

/* Instruction set for the LUT register. */
#define __QSPI_LUT_STOP            0
#define __QSPI_LUT_CMD             1
#define __QSPI_LUT_ADDR            2
#define __QSPI_LUT_DUMMY           3
#define __QSPI_LUT_MODE            4
#define __QSPI_LUT_MODE2           5
#define __QSPI_LUT_MODE4           6
#define __QSPI_LUT_READ            7
#define __QSPI_LUT_WRITE           8
#define __QSPI_LUT_JMP_ON_CS       9
#define __QSPI_LUT_ADDR_DDR        10
#define __QSPI_LUT_MODE_DDR        11
#define __QSPI_LUT_MODE2_DDR       12
#define __QSPI_LUT_MODE4_DDR       13
#define __QSPI_LUT_READ_DDR        14
#define __QSPI_LUT_WRITE_DDR       15
#define __QSPI_LUT_DATA_LEARN      16

/*
 * The PAD definitions for LUT register.
 *
 * The pad stands for the lines number of IO[0:3].
 * For example, the Quad read need four IO lines, so you should
 * set LUT_PAD4 which means we use four IO lines.
 */
#define __QSPI_LUT_PAD1            0
#define __QSPI_LUT_PAD2            1
#define __QSPI_LUT_PAD4            2

/* Oprands for the LUT register. */
#define __QSPI_ADDR24BIT           0x18
#define __QSPI_ADDR32BIT           0x20

/* Macros for constructing the LUT register. */
#define __QSPI_LUT0(ins, pad, opr)                     \
        (((opr) << __QSPI_OPRND0_SHIFT) | ((__QSPI_LUT_##pad) << __QSPI_PAD0_SHIFT) | \
        ((__QSPI_LUT_##ins) << __QSPI_INSTR0_SHIFT))

#define __QSPI_LUT1(ins, pad, opr) (__QSPI_LUT0(ins, pad, opr) << __QSPI_OPRND1_SHIFT)

/* other macros for LUT register. */
#define __QSPI_LUT(x)         (__QSPI_LUT_BASE + (x) * 4)
#define __QSPI_LUT_NUM         64

/* SEQID -- we can have 16 seqids at most. */
#define __QSPI_SEQID_QUAD_READ         0
#define __QSPI_SEQID_WREN              1
#define __QSPI_SEQID_WRDI              2
#define __QSPI_SEQID_RDSR              3
#define __QSPI_SEQID_SE                4
#define __QSPI_SEQID_CHIP_ERASE        5
#define __QSPI_SEQID_PP                6
#define __QSPI_SEQID_RDID              7
#define __QSPI_SEQID_WRSR              8
#define __QSPI_SEQID_RDCR              9
#define __QSPI_SEQID_EN4B              10
#define __QSPI_SEQID_BRWR              11

/* Endianess Configuration */
#define __QSPI_BE_64       0x00
#define __QSPI_LE_32       0x01
#define __QSPI_BE_32       0x02
#define __QSPI_LE_64       0x03

#define __QSPI_XFER_BEGIN  0x01
#define __QSPI_XFER_END    0x02

/* Oprands for the LUT register. */
#define __QSPI_ADDR_24BIT  0x18
#define __QSPI_ADDR_32BIT  0x20

#define __QSPI_TXFIFO_SIZE   512
#define __QSPI_RXFIFO_SIZE   128
#define __QSPI_AHB_BUF_SIZE  1024

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __qspi_inst_init1 (awbl_dev_t *p_dev);
aw_local void __qspi_inst_init2 (awbl_dev_t *p_dev);
aw_local void __qspi_inst_connect (awbl_dev_t *p_dev);

/** \brief set up spi controller */
aw_local aw_err_t __qspi_setup (struct awbl_spi_master *p_master,
                                aw_spi_device_t        *p_dev);

/** \brief QSPI controller hardware initialize */
aw_local void __qspi_hard_init (struct awbl_imx6ul_qspi_dev  *p_this);

aw_local aw_err_t __qspi_isr (void *p_arg);

aw_err_t __qspi_config(struct awbl_spi_master *p_master,
                        struct awbl_spi_config *config);

aw_local struct awbl_spi_config __qspi_get_default_config(void);

aw_local aw_err_t __qspi_setup (struct awbl_spi_master *p_master,
                                aw_spi_device_t        *p_spi_dev);

aw_err_t __qspi_write_and_read(struct awbl_spi_master   *p_master,
                               const void               *tx_buf,
                               void                     *rx_buf,
                               uint32_t                 nbytes);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_qspi_drvfuncs = {
    __qspi_inst_init1,
    __qspi_inst_init2,
    __qspi_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_qspi_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_QSPI_NAME,                  /* p_drvname */
        &__g_qspi_drvfuncs,                     /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_spi_master_devinfo2 __g_qspi_devinfo2 = {
    AW_SPI_CPHA  |
    AW_SPI_CPOL  |
    AW_SPI_NO_CS |
    AW_SPI_LOOP  |
    AW_SPI_CS_HIGH,                         /* features */
    0,                                      /* flags */
    __qspi_setup,                           /* pfunc_setup */
    __qspi_write_and_read,
    NULL,
    __qspi_config,
    __qspi_get_default_config,
};



/*******************************************************************************
  implementation
*******************************************************************************/


/**
 * \brief QSPI lut table lock
 */
aw_static_inline void __qspi_lut_lock (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);

    __QSPI_REG_WRITE(p_this_dvif, __QSPI_LUTKEY, __QSPI_LUTKEY_VALUE);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_LCKCR, __QSPI_LCKER_UNLOCK);
}

/**
 * \brief QSPI lut table unlock
 */
aw_static_inline void __qspi_lut_unlock (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);

    __QSPI_REG_WRITE(p_this_dvif, __QSPI_LUTKEY, __QSPI_LUTKEY_VALUE);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_LCKCR, __QSPI_LCKER_UNLOCK);
}

/*
 * If we have changed the content of the flash by writing or erasing,
 * we need to invalidate the AHB buffer. If we do not do so, we may read out
 * the wrong data. The spec tells us reset the AHB domain and Serial Flash
 * domain at the same time.
 */
aw_static_inline void __qspi_invalid (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t reg;

    reg = __QSPI_REG_READ(p_this_dvif, __QSPI_MCR);
    reg |= __QSPI_MCR_SWRSTHD_MASK | __QSPI_MCR_SWRSTSD_MASK;
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, reg);

    /*
     * The minimum delay : 1 AHB + 2 SFCK clocks.
     * Delay 1 us is enough.
     */
    aw_udelay(1);

    reg &= ~(__QSPI_MCR_SWRSTHD_MASK | __QSPI_MCR_SWRSTSD_MASK);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, reg);
}

/**
 * \brief Get the SEQID for the command
 */
aw_local int __qspi_seqid_get (struct awbl_imx6ul_qspi_dev *p_this,
                               uint8_t                      cmd)
{
    switch (cmd) {

    case AWBL_QSPI_OP_READ:
    case AWBL_QSPI_OP_READ_FAST:
    case AWBL_QSPI_OP_READ4:
    case AWBL_QSPI_OP_READ4_FAST:
    case AWBL_QSPI_OP_READ4_1_1_2:
    case AWBL_QSPI_OP_READ_1_1_4_D:
    case AWBL_QSPI_OP_READ_1_4_4_D:
    case AWBL_QSPI_OP_READ4_1_4_4_D:
    case AWBL_QSPI_OP_READ4_1_1_4:
    case AWBL_QSPI_OP_READ_1_1_4:
    case AWBL_QSPI_OP_READ_1_1_2:
        return __QSPI_SEQID_QUAD_READ;

    case AWBL_QSPI_OP_WREN:
        return __QSPI_SEQID_WREN;

    case AWBL_QSPI_OP_WRDI:
        return __QSPI_SEQID_WRDI;

    case AWBL_QSPI_OP_RDSR:
        return __QSPI_SEQID_RDSR;

    case AWBL_QSPI_OP_BE_4K:
    case AWBL_QSPI_OP_SE:
        return __QSPI_SEQID_SE;

    case AWBL_QSPI_OP_CHIP_ERASE:
        return __QSPI_SEQID_CHIP_ERASE;

    case AWBL_QSPI_OP_PP:
        return __QSPI_SEQID_PP;

    case AWBL_QSPI_OP_RDID:
        return __QSPI_SEQID_RDID;

    case AWBL_QSPI_OP_WRSR:
        return __QSPI_SEQID_WRSR;

    case AWBL_QSPI_OP_RDCR:
        return __QSPI_SEQID_RDCR;

    case AWBL_QSPI_OP_EN4B:
        return __QSPI_SEQID_EN4B;

    case AWBL_QSPI_OP_BRWR:
        return __QSPI_SEQID_BRWR;

    default:
        AW_DBGF(("Unsupported cmd 0x%.2x\n", cmd));
        break;
    }

    return -AW_EINVAL;
}

/**
 * \brief QSPI initialize ABH READ
 * \note  There are two different ways to read out the data from the flash:
 *        the "IP Command Read" and the "AHB Command Read".
 *
 *        The IC guy suggests we use the "AHB Command Read" which is faster
 *        then the "IP Command Read". (What's more is that there is a bug in
 *        the "IP Command Read" in the Vybrid.)
 *
 *        After we set up the registers for the "AHB Command Read", we can use
 *        the memcpy to read the data directly. A "missed" access to the buffer
 *        causes the controller to clear the buffer, and use the sequence pointed
 *        by the __QSPI_BFGENCR[__QSPI_SEQID] to initiate a read from the flash.
 */
aw_local void __qspi_abh_read_init (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t reg, reg2;
    int      seqid;

    /* AHB configuration for access buffer 0/1/2 . */
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF0CR, __QSPI_BUFXCR_INVALID_MSTRID);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF1CR, __QSPI_BUFXCR_INVALID_MSTRID);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF2CR, __QSPI_BUFXCR_INVALID_MSTRID);

    /*
     * Set ADATSZ with the maximum AHB buffer size to improve the
     * read performance.
     */
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_BUF3CR,
                     __QSPI_BUF3CR_ALLMST_MASK  |
                     ((__QSPI_AHB_BUF_SIZE / 8) << __QSPI_BUF3CR_ADATSZ_SHIFT));

    /* We only use the buffer3 */
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF0IND, 0);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF1IND, 0);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BUF2IND, 0);

    /* Set the default lut sequence for AHB Read. */
    seqid = __qspi_seqid_get(p_this, p_this_dvif->read_opcode);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_BFGENCR, seqid << __QSPI_BFGENCR_SEQID_SHIFT);

    /* enable the DDR quad read */
    if (p_this_dvif->read_mode == AWBL_QSPI_NOR_DDR_QUAD) {
        reg = __QSPI_REG_READ(p_this_dvif, __QSPI_MCR);

        /* Firstly, disable the module */
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, __QSPI_MCR_MDIS_MASK);

        /* Set the Sampling Register for DDR */
        reg2 = __QSPI_REG_READ(p_this_dvif, __QSPI_SMPR);
        reg2 &= ~__QSPI_SMPR_DDRSMP_MASK;
        reg2 |= ((p_this_dvif->ddr_smp << __QSPI_SMPR_DDRSMP_SHIFT) &
                  __QSPI_SMPR_DDRSMP_MASK);
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_SMPR, reg2);

        /* Enable the module again (enable the DDR too) */
        reg |= __QSPI_MCR_DDR_EN_MASK;
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, reg);

        reg  = __QSPI_REG_READ(p_this_dvif, __QSPI_FLSHCR);
        reg &= ~__QSPI_FLSHCR_TDH_MASK;
        reg |= __QSPI_FLSHCR_TDH_DDR_EN;
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_FLSHCR, reg);
    }
}

/**
 * \brief QSPi memory map address set
 */
aw_local void __qspi_map_addr_set (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);

    __QSPI_REG_WRITE(p_this_dvif, __QSPI_SFA1AD, p_this_dvif->bank_memmap_phy[1]);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_SFA2AD, p_this_dvif->bank_memmap_phy[2]);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_SFB1AD, p_this_dvif->bank_memmap_phy[3]);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_SFB2AD, p_this_dvif->bank_memmap_phy[4]);

}

/**
 * \brief QSPI initialize lut table
 */
aw_local void __qspi_lut_init (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);

    int      i;
    uint32_t lut_base;
    uint8_t  op, dm;
    uint8_t  addr_len;
    int      rxfifo = __QSPI_RXFIFO_SIZE;

    __qspi_lut_unlock(p_this);

    /* Clear all the LUT table */
    for (i = 0; i < __QSPI_LUT_NUM; i++) {
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_LUT_BASE + i * 4, 0);
    }

    /* Quad Read and DDR Quad Read*/
    lut_base = __QSPI_SEQID_QUAD_READ * 4;
    op       = p_this_dvif->read_opcode;
    dm       = p_this_dvif->read_dummy;
    addr_len = p_this_dvif->addr_width == 3 ?
               __QSPI_ADDR_24BIT : __QSPI_ADDR_32BIT;

    switch (p_this_dvif->read_mode) {

    case AWBL_QSPI_NOR_QUAD :
        if (op == AWBL_QSPI_OP_READ_1_1_4 ||
            op == AWBL_QSPI_OP_READ4_1_1_4) {

            /* read mode : 1-1-4 */
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR, PAD1, addr_len));
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(DUMMY, PAD1, dm) | __QSPI_LUT1(READ, PAD4, rxfifo));
                             
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT0(JMP_ON_CS, PAD1, 0));
        } else {
            AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
        }
        break;

    case AWBL_QSPI_NOR_DDR_QUAD :
        if (op == AWBL_QSPI_OP_READ_1_4_4_D ||
            op == AWBL_QSPI_OP_READ4_1_4_4_D) {

            /* read mode : 1-4-4, such as Spansion s25fl128s. */
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR_DDR, PAD4, addr_len));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(MODE_DDR, PAD4, 0xff) | __QSPI_LUT1(DUMMY, PAD1, dm));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT0(READ_DDR, PAD4, rxfifo) | __QSPI_LUT1(JMP_ON_CS, PAD1, 0));
        } else if (op == AWBL_QSPI_OP_READ_1_1_4_D) {

            /* read mode : 1-1-4, such as Micron N25Q256A. */
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR_DDR, PAD1, addr_len));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(DUMMY, PAD1, dm) | __QSPI_LUT1(READ_DDR, PAD4, rxfifo));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT1(JMP_ON_CS, PAD1, 0));
        } else {
            AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
        }
        break;

    case AWBL_QSPI_NOR_DUAL :
        if (op == AWBL_QSPI_OP_READ_1_1_2 ||
            op == AWBL_QSPI_OP_READ4_1_1_2) {

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR, PAD1, addr_len));

#if 0
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(MODE4, PAD2, 0xFF) | __QSPI_LUT1(DUMMY, PAD1, dm));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT0(READ, PAD2, rxfifo) | __QSPI_LUT1(JMP_ON_CS, PAD1, 0));
#else
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(DUMMY, PAD1, dm) | __QSPI_LUT1(READ, PAD2, rxfifo));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT1(JMP_ON_CS, PAD1, 0));
#endif
        } else {
            AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
        }
        break;

    case AWBL_QSPI_NOR_FAST :
        if (op == AWBL_QSPI_OP_READ_FAST ||
            op == AWBL_QSPI_OP_READ4_FAST) {

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR, PAD1, addr_len));


            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(DUMMY, PAD1, dm) | __QSPI_LUT1(READ, PAD1, rxfifo));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 2),
                             __QSPI_LUT0(JMP_ON_CS, PAD1, 0));
        } else {
            AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
        }
        break;

    case AWBL_QSPI_NOR_NORMAL :
    default :
        if (op == AWBL_QSPI_OP_READ ||
            op == AWBL_QSPI_OP_READ4) {
            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base),
                             __QSPI_LUT0(CMD, PAD1, op) | __QSPI_LUT1(ADDR, PAD1, addr_len));

            __QSPI_REG_WRITE(p_this_dvif,
                             __QSPI_LUT(lut_base + 1),
                             __QSPI_LUT0(READ, PAD1, rxfifo) | __QSPI_LUT1(JMP_ON_CS, PAD1, 0));
        } else {
            AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
        }
        break;
    }

    /* Write enable */
    lut_base = __QSPI_SEQID_WREN * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_WREN));

    /* Page Program */
    lut_base = __QSPI_SEQID_PP * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, p_this_dvif->program_opcode) |
                     __QSPI_LUT1(ADDR, PAD1, addr_len));
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base + 1),
                     __QSPI_LUT0(WRITE, PAD1, 0));

    /* Read Status */
    lut_base = __QSPI_SEQID_RDSR * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_RDSR) |
                     __QSPI_LUT1(READ, PAD1, 0x1));

    /* Erase a sector */
    lut_base = __QSPI_SEQID_SE * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, p_this_dvif->erase_opcode) |
                     __QSPI_LUT1(ADDR, PAD1, addr_len));

    /* Erase the whole chip */
    lut_base = __QSPI_SEQID_CHIP_ERASE * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_CHIP_ERASE));

    /* READ ID */
    lut_base = __QSPI_SEQID_RDID * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_RDID) |
                     __QSPI_LUT1(READ, PAD1, 0x8));

    /* Write Register */
    lut_base = __QSPI_SEQID_WRSR * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_WRSR) |
                     __QSPI_LUT1(WRITE, PAD1, 0x2));

    /* Read Configuration Register */
    lut_base = __QSPI_SEQID_RDCR * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_RDCR) |
                     __QSPI_LUT1(READ, PAD1, 0x1));

    /* Write disable */
    lut_base = __QSPI_SEQID_WRDI * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_WRDI));

    /* Enter 4 Byte Mode (Micron) */
    lut_base = __QSPI_SEQID_EN4B * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_EN4B));

    /* Enter 4 Byte Mode (Spansion) */
    lut_base = __QSPI_SEQID_BRWR * 4;
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_LUT(lut_base),
                     __QSPI_LUT0(CMD, PAD1, AWBL_QSPI_OP_BRWR));

    __qspi_lut_lock(p_this);
}

/**
 * \brief check flash cmd weather need wait follow-up data
 */
aw_local aw_bool_t __qspi_cmd_need_wait_data (struct awbl_imx6ul_qspi_dev *p_this,
                                           uint8_t                      op)
{
    if (op == AWBL_QSPI_OP_WREN       ||
        op == AWBL_QSPI_OP_BE_4K      ||
        op == AWBL_QSPI_OP_SE         ||
        op == AWBL_QSPI_OP_CHIP_ERASE ||
        op == AWBL_QSPI_OP_WRDI       ||
        op == AWBL_QSPI_OP_EN4B       ||
        op == AWBL_QSPI_OP_BRWR) {
        return AW_FALSE;
    }

    return AW_TRUE;
}

/**
 * \brief QSPI flash cmd execute
 */
aw_local int __qspi_cmd_run (struct awbl_imx6ul_qspi_dev *p_this,
                             uint8_t                      cmd,
                             uint32_t                     addr,
                             int                          len)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    int      seqid;
    uint32_t reg, reg2;
    aw_err_t err;

    /* save the reg */
    reg = __QSPI_REG_READ(p_this_dvif, __QSPI_MCR);

    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_SFAR,
                     p_this_dvif->bank_memmap_phy[p_this_dvif->bank_id] + addr);
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_RBCT,
                     __QSPI_RBCT_WMRK_MASK | __QSPI_RBCT_RXBRD_USEIPS);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, reg | __QSPI_MCR_CLR_RXF_MASK);

    /* check the SR first, wait previous cmd completed */
    do {
        reg2 = __QSPI_REG_READ(p_this_dvif, __QSPI_SR);
        if (reg2 & (__QSPI_SR_IP_ACC_MASK | __QSPI_SR_AHB_ACC_MASK)) {
            aw_udelay(1);
            AW_DBGF(("The controller is busy, 0x%x\n", reg2));
            continue;
        }
        break;
    } while (1);

    /* trigger the LUT now */
    seqid = __qspi_seqid_get(p_this, cmd);
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_IPCR,
                     (seqid << __QSPI_IPCR_SEQID_SHIFT) | len);

    /* Wait until completed */
    err = AW_SEMB_TAKE(p_this->ecspi_sem_sync, aw_ms_to_ticks(1000));
    if (err != AW_OK) {
        AW_DBGF(("cmd 0x%.2x timeout, addr@%.8x, FR:0x%.8x, SR:0x%.8x\n",
                cmd,
                addr,
                __QSPI_REG_READ(p_this_dvif, __QSPI_FR),
                __QSPI_REG_READ(p_this_dvif, __QSPI_SR)));
    }

    /* restore the MCR */
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, reg);

    /* After switch BANK, AHB buffer should also be invalid. */
    if ((AWBL_QSPI_OP_SE == cmd) ||
        (AWBL_QSPI_OP_PP == cmd) ||
        (AWBL_QSPI_OP_BE_4K == cmd)) {
        __qspi_invalid(p_this);
    }

    return err;
}

/**
 * \brief Write data to the QUADSPI_TBDR buffer registers.
 */
aw_local void __qspi_nor_write (struct awbl_imx6ul_qspi_dev *p_this,
                                int                          len,
                                const uint8_t               *p_txbuf)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t tmp;
    int      i, j;

    AW_DBGF(("QSPI write to 0x%.8x:0x%.8x, len : %d\n",
             p_this_dvif->bank_memmap_phy[p_this_dvif->bank_id], p_this->addr, len));

    /* clear the TX FIFO. */
    tmp = __QSPI_REG_READ(p_this_dvif, __QSPI_MCR);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, tmp | __QSPI_MCR_CLR_TXF_MASK);

    i = len / 4;

    for (j = 0; j < i * 4; j += 4) {
        tmp =  p_txbuf[j]            |
              (p_txbuf[j + 1] << 8)  |
              (p_txbuf[j + 2] << 16) |
              (p_txbuf[j + 3] << 24);
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_TBDR, tmp);
    }

    switch (len % 4) {

    case 1 :
        tmp = p_txbuf[j] | 0xFFFFFF00;
        i++;
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_TBDR, tmp);
        break;

    case 2 :
        tmp = p_txbuf[j] | (p_txbuf[j + 1] << 8) | 0xFFFF0000;
        i++;
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_TBDR, tmp);
        break;

    case 3 :
        tmp = p_txbuf[j] | (p_txbuf[j + 1] << 8) | (p_txbuf[j + 2] << 16) | 0xFF000000;
        i++;
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_TBDR, tmp);
        break;

    default :
        break;
    }

    for (; i < 4; i++) {
        /* fill the TXFIFO upto 16 bytes */
        __QSPI_REG_WRITE(p_this_dvif, __QSPI_TBDR, 0xFFFFFFFF);
    }

    /* Trigger it */
    __qspi_cmd_run(p_this, p_this->op, p_this->addr, len);

    /* next transfer is cmd */
    p_this->is_cmd = AW_TRUE;
}

/**
 * \brief QSPI send data.
 * \note no effective way to distinguish flash cmd and tx data.
 */
aw_local aw_err_t __qspi_data_send (struct awbl_imx6ul_qspi_dev *p_this,
                                    uint32_t                     len,
                                    const uint8_t               *p_txbuf)
{
    aw_err_t ret    = AW_OK;
    uint32_t opcode = 0;

    if (p_this->is_cmd == AW_TRUE) {
        p_this->is_cmd = AW_FALSE;
        opcode = p_txbuf[0];
        if (len > 1) {
            p_this->addr = p_txbuf[1] << 16 | p_txbuf[2] << 8 | p_txbuf[3];
        } else {
            p_this->addr = 0;
        }

        p_this->op = opcode;

        if (!__qspi_cmd_need_wait_data(p_this, opcode)) {
            ret = __qspi_cmd_run(p_this, opcode, p_this->addr, len);
            p_this->is_cmd = AW_TRUE;
        }
    } else { /* send data */
        __qspi_nor_write(p_this, len, p_txbuf);
    }

    return ret;
}

/**
 * \brief
 */
aw_local void __qspi_ahb_buffer_read (struct awbl_imx6ul_qspi_dev *p_this,
                                      uint32_t                     len,
                                      uint8_t                     *p_rxbuf)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t reg;

    AW_DBGF(("cmd [%x],read from 0x%p, len:%d\n",
             p_this->op,
             p_this_dvif->bank_memmap_phy[p_this_dvif->bank_id] + p_this->addr,
             len));

    reg = __QSPI_REG_READ(p_this_dvif, __QSPI_SPTRCLR);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_SPTRCLR, reg | __QSPI_SPTRCLR_BFPTRC_MASK);

    /* check the SR first, wait previous cmd completed */
    do {
        reg = __QSPI_REG_READ(p_this_dvif, __QSPI_SR);
        if (reg & (__QSPI_SR_IP_ACC_MASK | __QSPI_SR_AHB_ACC_MASK)) {
            aw_udelay(1);
            AW_DBGF(("The controller is busy, 0x%x\n", reg));
            continue;
        }
        break;
    } while (1);

    /* Read out the data directly from the AHB buffer.*/
    memcpy(p_rxbuf,
           (void *)(p_this_dvif->bank_memmap_phy[p_this_dvif->bank_id] + p_this->addr),
           len);

    /* check the SR first, wait previous cmd completed */
    do {
        reg = __QSPI_REG_READ(p_this_dvif, __QSPI_SR);
        if (reg & (__QSPI_SR_IP_ACC_MASK | __QSPI_SR_AHB_ACC_MASK)) {
            aw_udelay(1);
            AW_DBGF(("The controller is busy, 0x%x\n", reg));
            continue;
        }
        break;
    } while (1);

    /* next transfer is cmd */
    p_this->is_cmd = AW_TRUE;
}

/**
 * \brief Read out the data from the QUADSPI_RBDR buffer registers.
 */
aw_local void __qspi_ip_buffer_read (struct awbl_imx6ul_qspi_dev *p_this,
                                     uint32_t                     len,
                                     uint8_t                     *p_rxbuf)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t tmp;
    int      i = 0;

    while (len > 0) {
        tmp = __QSPI_REG_READ(p_this_dvif, __QSPI_RBDR + i * 4);
        AW_DBGF(("chip addr:0x%.8x, rcv:0x%.8x\n", p_this->addr, tmp));

        if (len >= 4) {
            *((uint32_t *)p_rxbuf) = tmp;
            p_rxbuf += 4;
        } else {
            memcpy(p_rxbuf, &tmp, len);
            break;
        }

        len -= 4;
        i++;
    }
}

/**
 * \brief QSPI receive data.
 */
aw_local aw_err_t __qspi_data_recv (struct awbl_imx6ul_qspi_dev *p_this,
                                    uint32_t                     len,
                                    uint8_t                     *p_rxbuf)
{
    aw_err_t ret = AW_OK;
    int      seqid;

    seqid = __qspi_seqid_get(p_this, p_this->op);
    if (seqid == __QSPI_SEQID_QUAD_READ) {
        __qspi_ahb_buffer_read(p_this, len, p_rxbuf);
    } else {
        ret = __qspi_cmd_run(p_this, p_this->op, 0, len);
        p_this->is_cmd = AW_TRUE;
        if (ret != AW_OK) {
            return ret;
        }
        __qspi_ip_buffer_read(p_this, len, p_rxbuf);
    }

    return ret;
}

/* 写NULL,读NULL的还没有判断  */
aw_err_t __qspi_write_and_read(struct awbl_spi_master   *p_master,
                               const void               *tx_buf,
                               void                     *rx_buf,
                               uint32_t                 nbytes)
{
    __QSPI_DEV_DECL(p_this, p_master);

    if (tx_buf && rx_buf) {
        return -AW_EINVAL;
    }

    uint32_t len = 0;

    p_this->p_write_buf = tx_buf;
    p_this->p_read_buf  = rx_buf;
    p_this->write_index = 0;
    p_this->read_nbytes  = 0;
    p_this->nbytes_to_recv  = nbytes;

    /* make sure the length won't beyond TX FIFO */
    len = nbytes > __QSPI_TXFIFO_SIZE ?
          __QSPI_TXFIFO_SIZE : nbytes;

    /* valid data to send */
    if (tx_buf != NULL) {
        __qspi_data_send(p_this,
                         len,
                         tx_buf);
    }
    /* the received data is valid */
    if (rx_buf != NULL) {
        __qspi_data_recv(p_this,
                         len,
                         rx_buf);
    }

    return AW_OK;
}

/**
 * \brief SPI interrupt service routine
 *
 * \param[in]   p_arg
 */
aw_local aw_err_t __qspi_isr (void *p_arg)
{
    __QSPI_DEV_DECL(p_this, p_arg);
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t reg;

    /* clear interrupt */
    reg = __QSPI_REG_READ(p_this_dvif, __QSPI_FR);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_FR, reg);

    if (reg & __QSPI_FR_TFF_MASK) {
        AW_SEMB_GIVE(p_this->ecspi_sem_sync);
    }

    return AW_OK;
}

aw_err_t __qspi_config(struct awbl_spi_master *p_master,
                        struct awbl_spi_config *config)
{
    return AW_OK;
}

/* 返回默认配置  */
aw_local struct awbl_spi_config __qspi_get_default_config(void)
{

}

/**
 * \brief set up spi controller
 */
aw_local aw_err_t __qspi_setup (struct awbl_spi_master *p_master,
                                aw_spi_device_t        *p_spi_dev)
{
    /*
     * check bits_per_word
     * keystone spi suppurt 2bit-16bit
     */

    if ((p_spi_dev->bits_per_word < 2) ||
        (p_spi_dev->bits_per_word > 16)) {

        return -AW_ENOTSUP;
    }


    return AW_OK;
}


/**
 * \brief QSPI controller hardware initialize
 */
aw_local void __qspi_hard_init (struct awbl_imx6ul_qspi_dev *p_this)
{
    __QSPI_DEVINFO_DECL(p_this_dvif, p_this);
    uint32_t reg_data;

    /* clear the DDR_EN bit */
    reg_data = __QSPI_REG_READ(p_this_dvif, __QSPI_MCR);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_MCR, ~(__QSPI_MCR_DDR_EN_MASK) & reg_data);
    aw_udelay(1);

    /* Reset the module */
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_MCR,
                     __QSPI_MCR_SWRSTSD_MASK | __QSPI_MCR_SWRSTHD_MASK);
    aw_udelay(1);

    /* Init the LUT table. */
    __qspi_lut_init(p_this);

    /* Disable the module */
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_MCR,
                     __QSPI_MCR_MDIS_MASK | __QSPI_MCR_RESERVED_MASK);

    /* Set the Sampling Register for SDR */
    reg_data = __QSPI_REG_READ(p_this_dvif, __QSPI_SMPR);
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_SMPR,
                     reg_data & ~(__QSPI_SMPR_FSDLY_MASK |
                                  __QSPI_SMPR_FSPHS_MASK |
                                  __QSPI_SMPR_HSENA_MASK |
                                  __QSPI_SMPR_DDRSMP_MASK));

    /* Enable the module */
    __QSPI_REG_WRITE(p_this_dvif,
                     __QSPI_MCR,
                     __QSPI_MCR_RESERVED_MASK |
                     __QSPI_MCR_END_CFG_MASK);

    /* enable interrupt */
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_FR, 0xffffffff);
    __QSPI_REG_WRITE(p_this_dvif, __QSPI_RSER, __QSPI_RSER_TFIE);

    /* set address map */
    __qspi_map_addr_set(p_this);

    /* init for AHB read */
    __qspi_abh_read_init(p_this);
}



/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __qspi_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __qspi_inst_init2 (awbl_dev_t *p_dev)
{
    __QSPI_MASTER_DEV_DECL(p_master, p_dev);
    __QSPI_DEV_DECL(p_this, p_dev);
    __QSPI_DEVINFO_DECL(p_this_dvif, p_dev);
	
	/* platform initialization */
    if (p_this_dvif->pfunc_plfm_init != NULL) {
        p_this_dvif->pfunc_plfm_init();
    }
	
    /* AWBus spi master general init2 */
    p_master->p_devinfo2 = &__g_qspi_devinfo2;

    /* first tranfser should be cmd  */
    p_this->is_cmd = AW_TRUE;

	/* init controller hardware */
    __qspi_hard_init(p_this);

    /* connect and enable isr */
    aw_int_connect(p_this_dvif->inum, (aw_pfuncvoid_t)__qspi_isr, (void *)p_dev);
    aw_int_enable(p_this_dvif->inum);

    /* create spi bus */
    awbl_spibus_create(p_master);
	
	/* initialize the stack binary semaphore */
    AW_SEMB_INIT(p_this->ecspi_sem_sync, 0, AW_SEM_Q_FIFO);
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __qspi_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/
void awbl_imx6ul_qspi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_qspi_drv_registration);
}

/* end of file */
