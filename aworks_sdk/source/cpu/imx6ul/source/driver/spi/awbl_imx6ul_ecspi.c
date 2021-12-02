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
#include "aw_int.h"
#include "aw_gpio.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_spibus.h"
#include "aw_clk.h"
#include "aw_io.h"
#include "aw_spi.h"
#include "driver/spi/awbl_imx6ul_ecspi.h"

/*******************************************************************************
  macro operate
*******************************************************************************/
/* write register */
#define __ECSPI_REG_WRITE(p_devinfo, offset, data) \
    AW_REG_WRITE32((p_devinfo)->regbase + (offset), (data))

/* read register */
#define __ECSPI_REG_READ(p_devinfo, offset) \
    AW_REG_READ32((p_devinfo)->regbase + (offset))

/* bit is set ? */
#define __ECSPI_REG_BIT_ISSET(p_devinfo, offset, bit) \
    AW_REG_BIT_ISSET32((p_devinfo)->regbase + (offset), bit)

/* set bit */
#define __ECSPI_REG_BIT_SET(p_devinfo, offset, bit) \
    AW_REG_BIT_SET32((p_devinfo)->regbase + (offset), bit)

/* clear bit */
#define __ECSPI_REG_BIT_CLR(p_devinfo, offset, bit) \
    AW_REG_BIT_CLR32((p_devinfo)->regbase + (offset), bit)

/* get register bits */
#define __ECSPI_REG_BITS_GET(p_devinfo, offset, start, len) \
    AW_REG_BITS_GET32((p_devinfo)->regbase + (offset), start, len)

/* set register bits */
#define __ECSPI_REG_BITS_SET(p_devinfo, offset, start, len, value) \
    AW_REG_BITS_SET32((p_devinfo)->regbase + (offset), start, len, value)

/* declare lpc11xx spi device instance */
#define __ECSPI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_ecspi_dev *p_this = \
        (struct awbl_imx6ul_ecspi_dev *)(p_dev)

/* declare lpc11xx spi device infomation */
#define __ECSPI_DEVINFO_DECL(p_this_dvif, p_dev) \
    struct awbl_imx6ul_ecspi_devinfo *p_this_dvif = \
        (struct awbl_imx6ul_ecspi_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get lpc11xx spi device infomation */
#define __ECSPI_DEVINFO_GET(p_dev) \
    ((struct awbl_imx6ul_ecspi_devinfo *)AWBL_DEVINFO_GET(p_dev))

/* get spi master instance */
#define __ECSPI_MASTER_DEV_DECL(p_master, p_dev) \
    struct awbl_spi_master *p_master = (struct awbl_spi_master *)(p_dev)

/* get spi master infomation */
#define __ECSPI_MASTER_DEVINFO_DECL(p_master_dvif, p_dev) \
    struct awbl_spi_master_devinfo *p_master_dvif = \
        (struct awbl_spi_master_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* write 8-bit width repeatly */
#define __ECSPI_DATA_WRITE_DUMP_REP(p_devinfo, len) {\
        size_t i = len; \
        while (i--) { \
            __raw_writel(0, (void*)((p_devinfo)->regbase + __ECSPI_REG_TXDATA)); \
        } \
    }


/* write 8-bit width repeatly */
#define __ECSPI_DATA_READ_DUMP_REP(p_this_dvif, len) {\
        size_t i = len; \
        while (i--) { \
            __raw_readl((void*)((p_devinfo)->regbase + __ECSPI_REG_RXDATA)); \
        } \
    }

#define __ECSPI_DATA_READ_REP(p_devinfo, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf); \
        while (i--) { \
            *ptr++ = __raw_readl((void*)((p_devinfo)->regbase + __ECSPI_REG_RXDATA));\
        } \
}


#define __ECSPI_DATA_READ(p_devinfo, p_buf, offset, len, bpw)    { \
             if (p_buf == NULL) {                                  \
                 __ECSPI_DATA_READ_DUMP_REP(p_devinfo, len);       \
             } else {                                              \
                 __ECSPI_DATA_READ_REP(p_devinfo,                  \
                         ((uint##bpw##_t *)p_buf) + offset,        \
                         len,                                      \
                         bpw)                                      \
             }                                                     \
}



#define __ECSPI_DATA_WRITE_REP(p_devinfo, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf); \
        while (i--) { \
            __raw_writel(*ptr++, (void*)((p_devinfo)->regbase + __ECSPI_REG_TXDATA));\
        } \
}

#define __ECSPI_DATA_WRITE(p_devinfo, p_buf, offset, len, bpw)    {\
             if (p_buf == NULL) {                                  \
                 __ECSPI_DATA_WRITE_DUMP_REP(p_devinfo, len);      \
             } else {                                              \
                 __ECSPI_DATA_WRITE_REP(p_devinfo,                 \
                                        ((uint##bpw##_t*)p_buf) + offset,  \
                                        len ,   \
                                        bpw) \
             }                                                     \
}



/*******************************************************************************
  register defination
*******************************************************************************/

/* RX FIFO */
#define __ECSPI_REG_RXDATA                0x00

/* TX FIFO */
#define __ECSPI_REG_TXDATA                0x04

/** ECSPI control register */
#define __ECSPI_REG_CTRL                  0x08
#define __ECSPI_REG_CTRL_BURST_LEN_SHIFT  20
#define __ECSPI_REG_CTRL_BURST_LEN_MASK  (0xFFF << __ECSPI_REG_CTRL_BURST_LEN_SHIFT)
#define __ECSPI_REG_CTRL_CHAN_SEL_SHIFT   18
#define __ECSPI_REG_CTRL_CHAN_SEL_MASK   (0x3 << __ECSPI_REG_CTRL_CHAN_SEL_SHIFT)
#define __ECSPI_REG_CTRL_DRCTL_SHIFT      16
#define __ECSPI_REG_CTRL_DRCTL_MASK      (0x3 << __ECSPI_REG_CTRL_DRCTL_SHIFT)
#define __ECSPI_REG_CTRL_PRE_DIV_SHIFT    12
#define __ECSPI_REG_CTRL_PRE_DIV_MASK    (0xF << __ECSPI_REG_CTRL_PRE_DIV_SHIFT)
#define __ECSPI_REG_CTRL_POST_DIV_SHIFT   8
#define __ECSPI_REG_CTRL_POST_DIV_MASK   (0xF << __ECSPI_REG_CTRL_POST_DIV_SHIFT)
#define __ECSPI_REG_CTRL_CHAN_MODE_SHIFT  4
#define __ECSPI_REG_CTRL_CHAN_MODE_MASK  (0xF << __ECSPI_REG_CTRL_CHAN_MODE_SHIFT)
#define __ECSPI_REG_CTRL_SMC_SHIFT        3
#define __ECSPI_REG_CTRL_XCH_SHIFT        2
#define __ECSPI_REG_CTRL_HT_SHIFT         1
#define __ECSPI_REG_CTRL_EN_SHIFT         0

/* ECSPI config register */
#define __ECSPI_REG_CFG                   0x0C
#define __ECSPI_REG_CFG_SCLK_CTL_SHIFT    20
#define __ECSPI_REG_CFG_SCLK_CTL_MASK    (0xF << __ECSPI_REG_CFG_SCLK_CTL_SHIFT)
#define __ECSPI_REG_CFG_DATA_CTL_SHIFT    16
#define __ECSPI_REG_CFG_DATA_CTL_MASK    (0xF << __ECSPI_REG_CFG_DATA_CTL_SHIFT)
#define __ECSPI_REG_CFG_SS_POL_SHIFT      12
#define __ECSPI_REG_CFG_SS_POL_MASK      (0xF << __ECSPI_REG_CFG_SS_POL_SHIFT)
#define __ECSPI_REG_CFG_SS_CTL_SHIFT      8
#define __ECSPI_REG_CFG_SS_CTL_MASK      (0xF << __ECSPI_REG_CFG_SS_CTL_SHIFT)
#define __ECSPI_REG_CFG_SCLK_POL_SHIFT    4
#define __ECSPI_REG_CFG_SCLK_POL_MASK    (0xF << __ECSPI_REG_CFG_SCLK_POL_SHIFT)
#define __ECSPI_REG_CFG_SCLK_PHA_SHIFT    0
#define __ECSPI_REG_CFG_SCLK_PHA_MASK    (0xF << __ECSPI_REG_CFG_SCLK_PHA_SHIFT)

/* ECSPI interrupt enable register */
#define __ECSPI_REG_INT                   0x10
#define __ECSPI_REG_INT_TC_SHIFT          7
#define __ECSPI_REG_INT_RO_SHIFT          6
#define __ECSPI_REG_INT_RF_SHIFT          5
#define __ECSPI_REG_INT_RDR_SHIFT         4
#define __ECSPI_REG_INT_RR_SHIFT          3
#define __ECSPI_REG_INT_TF_SHIFT          2
#define __ECSPI_REG_INT_TDR_SHIFT         1
#define __ECSPI_REG_INT_TE_SHIFT          0

/* ECSPI DMA control register */
#define __ECSPI_REG_DMA                   0x14
#define __ECSPI_REG_DMA_RX_WML_SHIFT      16
#define __ECSPI_REG_DMA_RX_WML_MASK      (0x3F << __ECSPI_REG_DMA_RX_WML_SHIFT)

/* ECSPI interrupt status register */
#define __ECSPI_REG_STAT                  0x18
#define __ECSPI_REG_STAT_TC_SHIFT         7
#define __ECSPI_REG_STAT_RO_SHIFT         6
#define __ECSPI_REG_STAT_RF_SHIFT         5
#define __ECSPI_REG_STAT_RDR_SHIFT        4
#define __ECSPI_REG_STAT_RR_SHIFT         3
#define __ECSPI_REG_STAT_TF_SHIFT         2
#define __ECSPI_REG_STAT_TDR_SHIFT        1
#define __ECSPI_REG_STAT_TE_SHIFT         0

/* ECSPI Sample Period Control Register */
#define __ECSPI_REG_PERIOD                0x1C
#define __ECSPI_REG_PERIOD_CSD_MASK       (0x3F << __ECSPI_REG_PERIOD_CSD_SHIFT)
#define __ECSPI_REG_PERIOD_CSD_SHIFT      16
#define __ECSPI_REG_PERIOD_CSRC_MASK      (0x1 << __ECSPI_REG_PERIOD_CSRC_SHIFT)
#define __ECSPI_REG_PERIOD_CSRC_SHIFT     15
#define __ECSPI_REG_PERIOD_SAMPLE_MASK    (0x7FFF << __ECSPI_REG_PERIOD_SAMPLE_SHIFT)
#define __ECSPI_REG_PERIOD_SAMPLE_SHIFT   0

/* ECSPI test register */
#define __ECSPI_REG_TEST                  0x20
#define __ECSPI_REG_TEST_LBC_SHIFT        31

#define __ECSPI_TX_FIFO_SIZE              64
#define __ECSPI_RX_FIFO_SIZE              64

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __ecspi_inst_init1 (awbl_dev_t *p_dev);
aw_local void __ecspi_inst_init2 (awbl_dev_t *p_dev);
aw_local void __ecspi_inst_connect (awbl_dev_t *p_dev);

/** \brief ecspi set up */
aw_local aw_err_t __ecspi_setup (struct awbl_spi_master *p_master,
                                 aw_spi_device_t         *p_dev);

/** \brief interrupt service */
aw_local void __ecspi_isr (void *p_arg);

aw_local void __bwp8_isr (void *p_arg);

aw_local void __bwp16_isr (void *p_arg);

aw_local void __bwp32_isr (void *p_arg);

/** \brief hardware initial */
aw_local void __ecspi_hard_init (struct awbl_imx6ul_ecspi_dev *p_this);


/** \brief ecspi clock set up  */
aw_local uint32_t __ecspi_clk_config (uint32_t  in_clk,
                                      uint32_t  speed,
                                      uint32_t *p_out_clk);

aw_err_t __ecspi_write_and_read(struct awbl_spi_master   *p_master,
                                const void               *tx_buf,
                                void                     *rx_buf,
                                uint32_t                 nbytes);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_ecspi_drvfuncs = {
    __ecspi_inst_init1,
    __ecspi_inst_init2,
    __ecspi_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_ecspi_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_ECSPI_NAME,                 /* p_drvname */
        &__g_ecspi_drvfuncs,                    /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};


/* 返回默认配置  */
aw_local aw_err_t __ecspi_get_default_config(struct awbl_spi_config *p_cfg)
{
    p_cfg->bpw = 8;
    p_cfg->mode = AW_SPI_MODE_0;
    p_cfg->speed_hz = 1000000;

    return AW_OK;
}

aw_err_t __ecspi_config(struct awbl_spi_master *p_master,
                        struct awbl_spi_config *config)
{
    __ECSPI_DEV_DECL(p_this, p_master);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_this);

    uint32_t ctrl, cfg = 0, test = 0;
    uint32_t clk_in, clk_out;
    uint32_t max_speed = 0, min_speed = 0;

    ctrl = (1 << __ECSPI_REG_CTRL_EN_SHIFT);

    ctrl |= __ECSPI_REG_CTRL_CHAN_MODE_MASK;

    clk_in = aw_clk_rate_get(p_devinfo->ref_clk_id);  // 获取输入时钟

    max_speed = clk_in / (1 * 1);
    min_speed = clk_in / (16 * 32768);

    if (config->speed_hz > max_speed) {
        config->speed_hz = max_speed;
    } else if (config->speed_hz < min_speed) {

        return -AW_ENOTSUP;
    }

    /* set clock speed */
    ctrl |= __ecspi_clk_config(clk_in, config->speed_hz, &clk_out);

    /* set chip select to use */
    ctrl |= (p_devinfo->cs_channel << __ECSPI_REG_CTRL_CHAN_SEL_SHIFT);

    /* set bpw */
    if ((config->bpw < 1) || (config->bpw > 32)) {
        return -AW_ENOTSUP;
    }

    ctrl |= ((config->bpw - 1) << __ECSPI_REG_CTRL_BURST_LEN_SHIFT);

    /* set spi Polarity and Phase */
    if (config->mode & AW_SPI_CPHA) {
        cfg |= (1 << (__ECSPI_REG_CFG_SCLK_PHA_SHIFT + p_devinfo->cs_channel));
    }

    if (config->mode & AW_SPI_CPOL) {
        cfg |= (1 << (__ECSPI_REG_CFG_SCLK_POL_SHIFT + p_devinfo->cs_channel));
        cfg |= (1 << (__ECSPI_REG_CFG_SCLK_CTL_SHIFT + p_devinfo->cs_channel));
    }

    if (config->mode & AW_SPI_CS_HIGH) {
        cfg |= (1 << (__ECSPI_REG_CFG_SS_POL_SHIFT + p_devinfo->cs_channel));
    }

    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_CTRL, ctrl);
    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_CFG, cfg);

    if (config->mode & AW_SPI_LOOP) {
        test = (1 << __ECSPI_REG_TEST_LBC_SHIFT);
        __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_TEST, test);
    }

    return AW_OK;
}


/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_spi_master_devinfo2 __g_ecspi_devinfo2 = {
    AWBL_FEATURE_SPI_CPOL_H  |
    AWBL_FEATURE_SPI_CPOL_L  |
    AWBL_FEATURE_SPI_CPHA_U  |
    AWBL_FEATURE_SPI_CPHA_D  |
    AWBL_FEATURE_SPI_LOOP,                  /* features */
    0,                                      /* flags */
    __ecspi_setup,                          /* pfunc_setup */
    __ecspi_write_and_read,
    NULL,
    __ecspi_config,
    __ecspi_get_default_config,
};


/**
 * \brief find last (most-significant) bit set
 * \param[in] x : the word to search
 *
 * \note This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */

aw_static_inline int __ecspi_fls(int x)
{
    int r = 32;

    if (!x) {
        return 0;
    }

    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }

    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }

    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }

    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }

    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }

    return r;
}


/**
 * \brief ECSPI时钟配置
 */
aw_local uint32_t __ecspi_clk_config (uint32_t  in_clk,
                                      uint32_t  speed,
                                      uint32_t *p_out_clk)
{
    /*
     * there are two 4-bit dividers, the pre-divider divides by
     * $pre, the post-divider by 2^$post
     */
    unsigned int pre, post;

    if (speed > in_clk) {
        return 0;
    }

    post = __ecspi_fls(in_clk) - __ecspi_fls(speed);
    if (in_clk > speed << post) {
        post++;
    }

    /* now we have: (in_clk <= speed << post) with post being minimal */
    post = max(4U, post) - 4;
    if (post > 0xf) {
        return 0xff;
    }

    pre = AW_DIV_ROUND_UP(in_clk, speed << post) - 1;

    /* Resulting frequency for the SCLK line. */
    *p_out_clk = (in_clk / (pre + 1)) >> post;

    return ((pre << __ECSPI_REG_CTRL_PRE_DIV_SHIFT) |
            (post << __ECSPI_REG_CTRL_POST_DIV_SHIFT));
}


/**
 * \brief SPI controller hardware initialize
 */
aw_local void __ecspi_hard_init (struct awbl_imx6ul_ecspi_dev *p_this)
{
    __ECSPI_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t ctrl, period;

    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_CTRL, 0);

    /* 配置ECSPI */
    ctrl = (1 << __ECSPI_REG_CTRL_EN_SHIFT);

    ctrl |= __ECSPI_REG_CTRL_CHAN_MODE_MASK;

    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_CTRL, ctrl);

    /* disable all interrupt */
    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_INT, 0U);

    /* delay config */
    period = ((p_devinfo->cs_delay     << __ECSPI_REG_PERIOD_CSD_SHIFT)     &
                                          __ECSPI_REG_PERIOD_CSD_MASK)      |
             ((p_devinfo->delay_clksrc << __ECSPI_REG_PERIOD_CSRC_SHIFT)    &
                                          __ECSPI_REG_PERIOD_CSRC_MASK)     |
             ((p_devinfo->trans_delay   << __ECSPI_REG_PERIOD_SAMPLE_SHIFT) &
                                          __ECSPI_REG_PERIOD_SAMPLE_MASK);
    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_PERIOD, period);


    __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_DMA, (64 / 2 - 1));
}


/* 写NULL,读NULL的还没有判断  */
aw_err_t __ecspi_write_and_read(struct awbl_spi_master   *p_master,
                                const void               *tx_buf,
                                void                     *rx_buf,
                                uint32_t                 nbytes)
{
    __ECSPI_DEV_DECL(p_this, p_master);
    __ECSPI_DEVINFO_DECL(p_this_dvif, p_this);

    uint8_t  bits_per_word = p_master->cur_config.bpw;
    uint32_t len = 0;

    p_this->p_write_buf = tx_buf;
    p_this->p_read_buf  = rx_buf;
    p_this->write_index = 0;
    p_this->read_nbytes  = 0;
    p_this->nbytes_to_recv  = nbytes;

    if (bits_per_word < 9) {
        len = nbytes  > __ECSPI_TX_FIFO_SIZE ? __ECSPI_TX_FIFO_SIZE : nbytes;
        __ECSPI_DATA_WRITE(p_this_dvif, tx_buf, 0, len, 8);
        p_this->write_index = len;

        p_this->bpw_func = __bwp8_isr;
    } else if (bits_per_word < 17) {

        len = (nbytes / 2)  > __ECSPI_TX_FIFO_SIZE ? __ECSPI_TX_FIFO_SIZE : (nbytes / 2 );
        __ECSPI_DATA_WRITE(p_this_dvif, tx_buf, 0, len, 16);
        p_this->write_index = len;
        p_this->bpw_func = __bwp16_isr;
    } else {
        len = (nbytes / 4)  > __ECSPI_TX_FIFO_SIZE ? __ECSPI_TX_FIFO_SIZE : (nbytes / 4);
        __ECSPI_DATA_WRITE(p_this_dvif, tx_buf, 0, len, 32);
        p_this->write_index = len;
        p_this->bpw_func = __bwp32_isr;
    }

    /* 启动传输 */
    __ECSPI_REG_BIT_SET(p_this_dvif,
                        __ECSPI_REG_CTRL,
                        __ECSPI_REG_CTRL_SMC_SHIFT);

    /* 设置FIFO的域值 */
    if (len >  __ECSPI_TX_FIFO_SIZE / 2) {

        /* 设置达到域值中断  */
        __ECSPI_REG_WRITE(p_this_dvif,
                          __ECSPI_REG_INT,
                         (1 << __ECSPI_REG_INT_TDR_SHIFT) |
                         (1 << __ECSPI_REG_INT_TE_SHIFT));
    } else {

        /* 打开发送FIFO空中断  */
        __ECSPI_REG_WRITE(p_this_dvif,
                          __ECSPI_REG_INT,
                         (1 << __ECSPI_REG_INT_TE_SHIFT));
    }


    /* 等待信号量  */
    AW_SEMB_TAKE(p_this->ecspi_sem_sync, AW_SEM_WAIT_FOREVER);

    return AW_OK;
}


aw_local void __bwp8_isr(void *p_arg)
{
    __ECSPI_DEV_DECL(p_this, p_arg);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_arg);

    int len = (__ECSPI_REG_READ(p_devinfo, __ECSPI_REG_TEST) >> 8) & 0x7F;


    /* 读空recieve buffer  */

    __ECSPI_DATA_READ(p_devinfo, p_this->p_read_buf, p_this->read_nbytes, len, 8);

    p_this->read_nbytes += len;

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {
         /* 关闭中断  */
         __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_INT, 0UL);

         p_this->write_index = 0;
         p_this->read_nbytes = 0;
         AW_SEMB_GIVE(p_this->ecspi_sem_sync);
         return;
    }


    if (p_this->nbytes_to_recv - p_this->write_index > len) {

      __ECSPI_DATA_WRITE(p_devinfo,
                         p_this->p_write_buf,
                         p_this->write_index,
                         len,
                         8);
      p_this->write_index += len;
    } else {

        __ECSPI_DATA_WRITE(p_devinfo,
                           p_this->p_write_buf,
                           p_this->write_index,
                           p_this->nbytes_to_recv - p_this->write_index,
                           8);

        p_this->write_index += p_this->nbytes_to_recv - p_this->write_index;
        /* 清除发域值中断，剩下发送空中断 */
        __ECSPI_REG_WRITE(p_devinfo,
                          __ECSPI_REG_INT,
                         (1 << __ECSPI_REG_INT_TE_SHIFT));
    }
}


aw_local void __bwp16_isr(void *p_arg)
{
    __ECSPI_DEV_DECL(p_this, p_arg);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_arg);

    int len = (__ECSPI_REG_READ(p_devinfo, __ECSPI_REG_TEST) >> 8) & 0x7F;
    /* 读空recieve buffer  */

    __ECSPI_DATA_READ(p_devinfo, p_this->p_read_buf, p_this->read_nbytes / 2, len, 16);

    p_this->read_nbytes += (len * 2);

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {
         /* 关闭中断  */
         __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_INT, 0UL);

         p_this->write_index = 0;
         p_this->read_nbytes = 0;
         AW_SEMB_GIVE(p_this->ecspi_sem_sync);
         return;
    }

    if ((p_this->nbytes_to_recv / 2) - p_this->write_index > len) {

        __ECSPI_DATA_WRITE(p_devinfo,
                           p_this->p_write_buf,
                           p_this->write_index,
                           len,
                           16);
        p_this->write_index += len;
    } else {

        __ECSPI_DATA_WRITE(p_devinfo,
                           p_this->p_write_buf,
                           p_this->write_index,
                           (p_this->nbytes_to_recv / 2) - p_this->write_index,
                           16);

        p_this->write_index += (p_this->nbytes_to_recv / 2) - p_this->write_index;

        /* 清除发域值中断，剩下发送空中断 */
        __ECSPI_REG_WRITE(p_devinfo,
                          __ECSPI_REG_INT,
                         (1 << __ECSPI_REG_INT_TE_SHIFT));
    }
}


aw_local void __bwp32_isr(void *p_arg)
{
    __ECSPI_DEV_DECL(p_this, p_arg);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_arg);

    int len = (__ECSPI_REG_READ(p_devinfo, __ECSPI_REG_TEST) >> 8) & 0x7F;
    /* 读空recieve buffer  */

    __ECSPI_DATA_READ(p_devinfo, p_this->p_read_buf, p_this->read_nbytes / 4, len, 32);
    p_this->read_nbytes += len * 4;

    if (p_this->read_nbytes == p_this->nbytes_to_recv) {
       /* 关闭中断  */
       __ECSPI_REG_WRITE(p_devinfo, __ECSPI_REG_INT, 0UL);

       p_this->write_index = 0;
       p_this->read_nbytes = 0;
       AW_SEMB_GIVE(p_this->ecspi_sem_sync);
       return ;
    }

    if ((p_this->nbytes_to_recv / 4) - p_this->write_index > len) {

        __ECSPI_DATA_WRITE(p_devinfo,
                       p_this->p_write_buf,
                       p_this->write_index,
                       len,
                       32);
        p_this->write_index += len;
    } else {
        __ECSPI_DATA_WRITE(p_devinfo,
                           p_this->p_write_buf,
                           p_this->write_index,
                           (p_this->nbytes_to_recv / 4) - p_this->write_index,
                           32);

        p_this->write_index += (p_this->nbytes_to_recv / 4) - p_this->write_index;

        /* 清除发域值中断，剩下发送空中断 */
        __ECSPI_REG_WRITE(p_devinfo,
                      __ECSPI_REG_INT,
                     (1 << __ECSPI_REG_INT_TE_SHIFT));
    }
}

/**
 * \brief SPI interrupt service routine
 *
 * \param[in]   p_arg
 */
aw_local void __ecspi_isr (void *p_arg)
{
    __ECSPI_DEV_DECL(p_this, p_arg);

    p_this->bpw_func(p_arg);
}


/**
 * \brief set up spi controller
 */
aw_local aw_err_t __ecspi_setup (struct awbl_spi_master *p_master,
                                 aw_spi_device_t        *p_spi_dev)
{
    __ECSPI_DEV_DECL(p_this, p_master);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_this);

    /*
     * check bits_per_word
     * keystone spi suppurt max 32bit
     */

    if ((p_spi_dev->bits_per_word < 1) ||
        (p_spi_dev->bits_per_word > 32)) {

        return -AW_ENOTSUP;
    }

    /*
     * check if we can provide the requested rate
     */
    {
        uint32_t max_speed, min_speed;
        uint32_t clk_in;

        clk_in = aw_clk_rate_get(p_devinfo->ref_clk_id);
        max_speed = clk_in / (1 * 1);
        min_speed = clk_in / (16 * 32768);

        if (p_spi_dev->max_speed_hz > max_speed) {
            p_spi_dev->max_speed_hz = max_speed;
        } else if (p_spi_dev->max_speed_hz < min_speed) {

            return -AW_ENOTSUP;
        }
    }
    return AW_OK;
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ecspi_inst_init1(awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ecspi_inst_init2(awbl_dev_t *p_dev)
{
    __ECSPI_MASTER_DEV_DECL(p_master, p_dev);
    __ECSPI_DEV_DECL(p_this, p_dev);
    __ECSPI_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* AWBus spi master general init2 */
    p_master->p_devinfo2 = &__g_ecspi_devinfo2;

    /* init controller hardware */
    __ecspi_hard_init(p_this);

    /* connect and enable isr */
    aw_int_connect(p_devinfo->inum, (aw_pfuncvoid_t)__ecspi_isr, (void *)p_dev);
    aw_int_enable(p_devinfo->inum);

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
aw_local void __ecspi_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/
void awbl_imx6ul_ecspi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_ecspi_drv_registration);
}

/* end of file */
