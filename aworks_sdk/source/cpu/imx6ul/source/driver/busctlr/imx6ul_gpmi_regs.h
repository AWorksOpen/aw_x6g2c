#ifndef             __IMX6UL_GPMI_REGS_H__
#define             __IMX6UL_GPMI_REGS_H__

/*******************************************************************************
    register define
*******************************************************************************/
struct __imx6ul_gpmi_reg1 {
    volatile uint32_t value;
    volatile uint32_t value_set;
    volatile uint32_t value_clr;
    volatile uint32_t value_tog;
};

struct __imx6ul_gpmi_reg2 {
    volatile uint32_t value;
    volatile uint32_t reserved[3];
};

struct __imx6ul_gpmi_regs {

    /* GPMI Control Register 0 */
    struct __imx6ul_gpmi_reg1 ctrl0;
#define __GPMI_CTRL0_SFTRST                 (1u << 31)
#define __GPMI_CTRL0_CLKGATE                (1u << 30)
#define __GPMI_CTRL0_RUN                    (1u << 29)
#define __GPMI_CTRL0_LOCK_CS                (1u << 27)
#define __GPMI_CTRL0_COMMAND_MODE           (3u << 24)
#define __GPMI_CTRL0_WORD_LENGTH            (1u << 23)
#define __GPMI_CTRL0_CS                     (7u << 20)
#define __GPMI_CTRL0_ADDRESS                (7u << 17)
#define __GPMI_CTRL0_ADDRESS_INCREMENT      (1u << 16)
#define __GPMI_CTRL0_XFER_COUNT             (0xFFFFu << 0)

#define __GPMI_SET_CTRL0_LOCK_CS(v)    (((v) << 27) & __GPMI_CTRL0_LOCK_CS)

#define __GPMI_CTRL0_CMD_WRITE         0x00
#define __GPMI_CTRL0_CMD_READ          0x01
#define __GPMI_CTRL0_CMD_READ_COMPARE  0x02
#define __GPMI_CTRL0_CMD_WAIT_READY    0x03
#define __GPMI_SET_CTRL0_CMD_MODE(v)   (((v) << 24) & __GPMI_CTRL0_COMMAND_MODE)

#define __GPMI_SET_CTRL0_CS(v) (((v) << 20) & __GPMI_CTRL0_CS)

#define __GPMI_CTRL0_ADDRESS_DATA     0x00  /* read and write data bytes */
#define __GPMI_CTRL0_ADDRESS_CMD      0x01  /* write command */
#define __GPMI_CTRL0_ADDRESS_ADDR     0x02  /* write address */
#define __GPMI_SET_CTRL0_ADDRESS(v)   (((v) << 17) & __GPMI_CTRL0_ADDRESS)
#define __GPMI_SET_CTRL0_XFER_COUNT(v) (((v) << 0)  & __GPMI_CTRL0_XFER_COUNT)


    /* GPMI Compare Register */
    struct __imx6ul_gpmi_reg2 compare;
#define __GPMI_COMPARE_MASK             (0xFFFFu << 16)
#define __GPMI_COMPARE_REFERENCE        (0xFFFFu << 0)

    /* GPMI Integrated ECC Control Register */
    struct __imx6ul_gpmi_reg1 eccctrl;
#define __GPMI_ECCCTRL_HANDLE            (0xFFFFu << 16)
#define __GPMI_ECCCTRL_ECC_CMD           (3u << 13)
#define __GPMI_ECCCTRL_ENABLE_ECC        (1u << 12)
#define __GPMI_ECCCTRL_ENABLE_RANDOMIZER (1u << 11)
#define __GPMI_ECCCTRL_BUFFER_MASK       (0x1FFu << 0)

    /* GPMI Integrated ECC Transfer Count Register */
    struct __imx6ul_gpmi_reg2 ecccount;
#define __GPMI_ECCCOUNT_RANDOMIZER_PAGE  (0xFF    << 16)
#define __GPMI_ECCCOUNT_COUNT            (0xFFFFu << 0)

    /* GPMI Payload Address Register,the least 2bit must be zero */
    struct __imx6ul_gpmi_reg2 payload;

    /*
     * GPMI Auxiliary Address Register
     * Pointer to ECC control structure and meta-data storage
     * also,the least 2bit must be zero
     */
    struct __imx6ul_gpmi_reg2 auxiliary;

    /* GPMI Control Register 1  */
    struct __imx6ul_gpmi_reg1 ctrl1;
#define __GPMI_CTRL1_GPMI_MODE                    (0x01u << 0)
#define __GPMI_CTRL1_CAMERA_MODE                  (0x01u << 1)
#define __GPMI_CTRL1_ATA_IRQRDY_POLARITY          (0x01u << 2)
#define __GPMI_CTRL1_DEV_RESET                    (0x01u << 3)
#define __GPMI_CTRL1_ABORT_WAIT_FOR_READY_CHANNEL (0x07u << 4)
#define __GPMI_CTRL1_ABORT_WAIT_REQUEST           (0x01u << 7)
#define __GPMI_CTRL1_BURST_EN                     (0x01u << 8)
#define __GPMI_CTRL1_TIMEOUT_REQ                  (0x01u << 9)
#define __GPMI_CTRL1_DEV_REQ                      (0x01u << 10)
#define __GPMI_CTRL1_DMA2ECC_MODE                 (0x01u << 11)
#define __GPMI_CTRL1_RDN_DELAY                    (0x0Fu << 12)
#define __GPMI_CTRL1_HALF_PERIOD                  (0x01u << 16)
#define __GPMI_CTRL1_DLL_ENABLE                   (0x01u << 17)
#define __GPMI_CTRL1_BCH_MODE                     (0x01u << 18)
#define __GPMI_CTRL1_GANGEN_RDYBUSY               (0x01u << 19)
#define __GPMI_CTRL1_TIMEOUT_IRQ_EN               (0x01u << 20)
#define __GPMI_CTRL1_TIMEOUT_TEST_TRIGGER         (0x01u << 21)
#define __GPMI_CTRL1_WRN_DLY_SEL                  (0x03u << 22)
#define __GPMI_CTRL1_DECOUPLE_CS                  (0x01u << 24)
#define __GPMI_CTRL1_SSYNCMODE                    (0x01u << 25)
#define __GPMI_CTRL1_UPDATE_CS                    (0x01u << 26)
#define __GPMI_CTRL1_GPMI_CLK_DIV2_EN             (0x01u << 27)
#define __GPMI_CTRL1_TOGGLE_MODE                  (0x01u << 28)
#define __GPMI_CTRL1_WRITE_CLK_STOP               (0x01u << 29)
#define __GPMI_CTRL1_SSYNC_CLK_STOP               (0x01u << 30)
#define __GPMI_CTRL1_DEV_CLK_STOP                 (0x01u << 31)

    /* GPMI Timing Register 0 */
    struct __imx6ul_gpmi_reg2 timing0;
#define __GPMI_TIMING0_ADDRESS_SETUP       (0xFFu << 16)
#define __GPMI_TIMING0_DATA_HOLD           (0xFFu << 8)
#define __GPMI_TIMING0_DATA_SETUP          (0xFFu << 0)

    /* GPMI Timing Register 1 */
    struct __imx6ul_gpmi_reg2 timing1;
#define __GPMI_TIMING1_BUSY_TIMEOUT        (0xFFFFu << 16)

    /* GPMI Timing Register 2 */
    struct __imx6ul_gpmi_reg2 timing2;
#define __GPMI_TIMING2_DATA_PAUSE        (0x0Fu << 0)
#define __GPMI_TIMING2_CMDADD_PAUSE      (0x0Fu << 4)
#define __GPMI_TIMING2_POSTAMBLE_DELAY   (0x0Fu << 8)
#define __GPMI_TIMING2_PREAMBLE_DELAY    (0x0Fu << 12)
#define __GPMI_TIMING2_CE_DELAY          (0x1Fu << 16)
#define __GPMI_TIMING2_READ_LATENCY      (0x07u << 24)
#define __GPMI_TIMING2_TCR               (0x03u << 27)
#define __GPMI_TIMING2_TRPSTH            (0x07u << 29)

    /* GPMI DMA Data Transfer Register */
    struct __imx6ul_gpmi_reg2 data;

    /* GPMI Status Register */
    struct __imx6ul_gpmi_reg2 status;
#define __GPMI_STATUS_PRESENT              (0x01u << 0)
#define __GPMI_STATUS_FIFO_FULL            (0x01u << 1)
#define __GPMI_STATUS_FIFO_EMPTY           (0x01u << 2)
#define __GPMI_STATUS_INVALID_BUFFER_MASK  (0x01u << 3)
#define __GPMI_STATUS_ATA_IRQ              (0x01u << 4)
#define __GPMI_STATUS_DEV0_ERROR           (0x01u << 8)
#define __GPMI_STATUS_DEV1_ERROR           (0x01u << 9)
#define __GPMI_STATUS_DEV2_ERROR           (0x01u << 10)
#define __GPMI_STATUS_DEV3_ERROR           (0x01u << 11)
#define __GPMI_STATUS_DEV4_ERROR           (0x01u << 12)
#define __GPMI_STATUS_DEV5_ERROR           (0x01u << 13)
#define __GPMI_STATUS_DEV6_ERROR           (0x01u << 14)
#define __GPMI_STATUS_DEV7_ERROR           (0x01u << 15)
#define __GPMI_STATUS_FIFO_RDY_TIMEOUT     (0xFFu << 16)
#define __GPMI_STATUS_FIFO_RDY_READY_BUSY  (0xFFu << 24)

    /* GPMI debug information register */
    struct __imx6ul_gpmi_reg2 debug;

    /* GPMI Version Register */
    struct __imx6ul_gpmi_reg2 vesion;

    /* GPMI Debug2 Information Register */
    struct __imx6ul_gpmi_reg2 debug2;

    /* GPMI Debug3 Information Register */
    struct __imx6ul_gpmi_reg2 debug3;

    /* GPMI Double Rate Read DLL Control Register */
    struct __imx6ul_gpmi_reg2 read_ddr_dll_ctrl;

    /* GPMI Double Rate Write DLL Control Register */
    struct __imx6ul_gpmi_reg2 write_ddr_dll_ctrl;

    /* GPMI Double Rate Read DLL Status Control Register */
    struct __imx6ul_gpmi_reg2 read_ddr_dll_sts;

    /* GPMI Double Rate Write DLL Status Control Register */
    struct __imx6ul_gpmi_reg2 write_ddr_dll_sts;
};


struct __imx6ul_bch_regs {
    struct __imx6ul_gpmi_reg1 ctrl;
#define __BCH_CTRL_SFTRST                   (1u<<31)
#define __BCH_CTRL_CLKGATE                  (1u<<30)
#define __BCH_CTRL_M2M_LAYOUT               (3u<<18)
#define __BCH_CTRL_M2M_ENCODE               (1u<<17)
#define __BCH_CTRL_M2M_ENABLE               (1u<<16)
#define __BCH_CTRL_COMPLETE_IRQ_EN          (1u<<8)
#define __BCH_CTRL_BM_ERROR_IRQ             (1u<<3)
#define __BCH_CTRL_COMPLETE_IRQ             (1u<<0)

    struct __imx6ul_gpmi_reg2 status;
    struct __imx6ul_gpmi_reg2 mode;
    struct __imx6ul_gpmi_reg2 encodeptr;
    struct __imx6ul_gpmi_reg2 dataptr;
    struct __imx6ul_gpmi_reg2 metaptr;
    struct __imx6ul_gpmi_reg2 reserved;
    struct __imx6ul_gpmi_reg2 layoutselect;
    struct __imx6ul_gpmi_reg2 flash0layout0;
#define __BCH_FLASH0LAYOUT0_NBLOCKS(v)      ((v) << 24)
#define __BCH_FLASH0LAYOUT0_META_SIZE(v)    ((v) << 16)
#define __BCH_FLASH0LAYOUT0_ECC0(v)         ((v) << 11)
#define __BCH_FLASH0LAYOUT0_GF(v)           ((v)==14? (1<<10):0 )
#define __BCH_FLASH0LAYOUT0_DATA0_SIZE(v)   ((v) >> 2)

    struct __imx6ul_gpmi_reg2 flash0layout1;
#define __BCH_FLASH0LAYOUT1_PAGE_SIZE(v)    ((v) << 16)
#define __BCH_FLASH0LAYOUT1_ECCN(v)         ((v) << 11)
#define __BCH_FLASH0LAYOUT1_GF(v)           ((v)==14? (1<<10):0 )
#define __BCH_FLASH0LAYOUT1_DATAN_SIZE(v)   ((v) >> 2)

    struct __imx6ul_gpmi_reg2 flash1layout0;
    struct __imx6ul_gpmi_reg2 flash1layout1;
    struct __imx6ul_gpmi_reg2 flash2layout0;
    struct __imx6ul_gpmi_reg2 flash2layout1;
    struct __imx6ul_gpmi_reg2 flash3layout0;
    struct __imx6ul_gpmi_reg2 flash3layout1;
    struct __imx6ul_gpmi_reg2 debug0;
    struct __imx6ul_gpmi_reg2 dbgkesread;
    struct __imx6ul_gpmi_reg2 dbgcsferead;
    struct __imx6ul_gpmi_reg2 dbgsyndgenread;
    struct __imx6ul_gpmi_reg2 dbgahbmread;
    struct __imx6ul_gpmi_reg2 blockname;
    struct __imx6ul_gpmi_reg2 version;
    struct __imx6ul_gpmi_reg2 debug1;
};

#define __GPMI_BCH_REG_CLKGATE  (1 << 30)
#define __GPMI_BCH_REG_SFTRST   (1 << 31)
#define __GPMI_BCH_REG_SET_ADDR 0x4
#define __GPMI_BCH_REG_CLR_ADDR 0x8
#endif
