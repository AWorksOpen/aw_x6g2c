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
 * \brief AWBus IMX6UL SIM driver
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_sim.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-12  win, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_int.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

#include "aw_clk.h"
#include "driver/sim/awbl_imx6ul_sim.h"

#include <string.h>

/*******************************************************************************
  define
*******************************************************************************/

/* Transmit and receive buffer sizes */
#define SIM_XMT_BUFFER_SIZE (300)
#define SIM_RCV_BUFFER_SIZE (400)

#define SIM_TX_FIFO_DEPTH   (16)
#define SIM_RX_FIFO_DEPTH   (285)

#define __SIM_TX_FIFO_THRESHOLD   (0x04)
#define RX_FIFO_THRESHOLD   (250)

/* Interface character references */
#define SIM_IFC_TXI(letter, number) (letter + number * 4)
#define SIM_IFC_TA1   SIM_IFC_TXI(0, 0)
#define SIM_IFC_TB1   SIM_IFC_TXI(0, 1)
#define SIM_IFC_TC1   SIM_IFC_TXI(0, 2)
#define SIM_IFC_TD1   SIM_IFC_TXI(0, 3)
#define SIM_IFC_TA2   SIM_IFC_TXI(1, 0)
#define SIM_IFC_TB2   SIM_IFC_TXI(1, 1)
#define SIM_IFC_TC2   SIM_IFC_TXI(1, 2)
#define SIM_IFC_TD2   SIM_IFC_TXI(1, 3)
#define SIM_IFC_TA3   SIM_IFC_TXI(2, 0)
#define SIM_IFC_TB3   SIM_IFC_TXI(2, 1)
#define SIM_IFC_TC3   SIM_IFC_TXI(2, 2)
#define SIM_IFC_TD3   SIM_IFC_TXI(2, 3)
#define SIM_IFC_TA4   SIM_IFC_TXI(3, 0)
#define SIM_IFC_TB4   SIM_IFC_TXI(3, 1)
#define SIM_IFC_TC4   SIM_IFC_TXI(3, 2)
#define SIM_IFC_TD4   SIM_IFC_TXI(3, 3)

/* ATR and OPS states */
#define __SIM_STATE_REMOVED           (0)
#define __SIM_STATE_DETECTED          (1)
#define __SIM_STATE_ATR_RECEIVING     (2)
#define __SIM_STATE_ATR_RECEIVED      (3)
#define __SIM_STATE_XMTING            (4)
#define __SIM_STATE_XMT_DONE          (5)
#define __SIM_STATE_XMT_ERROR         (6)
#define __SIM_STATE_RECEIVING         (7)
#define __SIM_STATE_RECEIVE_DONE      (8)
#define __SIM_STATE_RECEIVE_ERROR     (9)
#define __SIM_STATE_RESET_SEQUENCY    (10)

/******************************************************************************/

/* sim port [0|1] control register */
#define __SIM_REG_PORT1_CNTL               (0x00)
#define __SIM_REG_PORT0_CNTL               (0x14)
#define __SIM_REG_PORT_CNTL_SFPD           (1 << 7)
#define __SIM_REG_PORT_CNTL_3VOLT          (1 << 6)
#define __SIM_REG_PORT_CNTL_SCSP           (1 << 5)
#define __SIM_REG_PORT_CNTL_SCEN           (1 << 4)
#define __SIM_REG_PORT_CNTL_SRST           (1 << 3)
#define __SIM_REG_PORT_CNTL_STEN           (1 << 2)
#define __SIM_REG_PORT_CNTL_SVEN           (1 << 1)
#define __SIM_REG_PORT_CNTL_SAPD           (1 << 0)

/* sim setup register */
#define __SIM_REG_SETUP                    (0x04)
#define __SIM_REG_SETUP_SPS_PORT0          (0 << 1)
#define __SIM_REG_SETUP_SPS_PORT1          (1 << 1)

/* sim port [0|1] detect Register */
#define __SIM_REG_PORT1_DETECT             (0x08)
#define __SIM_REG_PORT0_DETECT             (0x3C)
#define __SIM_REG_PORT_DETECT_SPDS         (1 << 3)
#define __SIM_REG_PORT_DETECT_SPDP         (1 << 2)
#define __SIM_REG_PORT_DETECT_SDI          (1 << 1)
#define __SIM_REG_PORT_DETECT_SDIM         (1 << 0)

/* sim transmit buffer register */
#define __SIM_REG_PORT_XMT_BUF             (0x0C)

/* sim receive buffer register */
#define __SIM_REG_PORT_RCV_BUF             (0x10)

/* sim control register */
#define __SIM_REG_CNTL                     (0x18)
#define __SIM_REG_CNTL_BWTEN               (1 << 15)
#define __SIM_REG_CNTL_XMT_CRC_LRC         (1 << 14)
#define __SIM_REG_CNTL_CRCEN               (1 << 13)
#define __SIM_REG_CNTL_LRCEN               (1 << 12)
#define __SIM_REG_CNTL_CWTEN               (1 << 11)
#define __SIM_REG_CNTL_SAMPLE12            (1 << 4)
#define __SIM_REG_CNTL_ONACK               (1 << 3)
#define __SIM_REG_CNTL_ANACK               (1 << 2)
#define __SIM_REG_CNTL_ICM                 (1 << 1)
#define __SIM_REG_CNTL_GPCNT_CLK_SEL(x)    ((x&0x03) << 9)
#define __SIM_REG_CNTL_GPCNT_CLK_SEL_MASK  (0x03 << 9)
#define __SIM_REG_CNTL_BAUD_SEL(x)         ((x&0x07) << 6)
#define __SIM_REG_CNTL_BAUD_SEL_MASK       (0x07 << 6)
#define __SIM_REG_CNTL_GPCNT_CARD_CLK       1
#define __SIM_REG_CNTL_GPCNT_RCV_CLK        2
#define __SIM_REG_CNTL_GPCNT_ETU_CLK        3

/* sim clock prescaler register */
#define __SIM_REG_CLK_PRESCALER             (0x1C)

/* sim receive threshold register*/
#define __SIM_REG_RCV_THRESHOLD             (0x20)
#define __SIM_REG_RCV_THRESHOLD_RTH(x)      ((x&0x0f) << 9)
#define __SIM_REG_RCV_THRESHOLD_RTH_MASK    (0x0f << 9)
#define __SIM_REG_RCV_THRESHOLD_RDT(x)      ((x&0x1ff) << 0)
#define __SIM_REG_RCV_THRESHOLD_RDT_MASK    (0x1ff << 0)

/* sim enable register */
#define __SIM_REG_ENABLE                    (0x24)
#define __SIM_REG_ENABLE_XMTEN              (1 << 1)
#define __SIM_REG_ENABLE_RCVEN              (1 << 0)
#define __SIM_REG_ENABLE_ESTOP_EN           (1 << 5)
#define __SIM_REG_ENABLE_ESTOP_EXE          (1 << 6)

/* sim transmit status register*/
#define __SIM_REG_XMT_STATUS                (0x28)
#define __SIM_REG_XMT_STATUS_GPCNT          (1 << 8)
#define __SIM_REG_XMT_STATUS_TDTF           (1 << 7)
#define __SIM_REG_XMT_STATUS_TFO            (1 << 6)
#define __SIM_REG_XMT_STATUS_TC             (1 << 5)
#define __SIM_REG_XMT_STATUS_ETC            (1 << 4)
#define __SIM_REG_XMT_STATUS_TFE            (1 << 3)
#define __SIM_REG_XMT_STATUS_XTE            (1 << 0)

/* sim receive status register */
#define __SIM_REG_RCV_STATUS                (0x2C)
#define __SIM_REG_RCV_STATUS_BGT            (1 << 11)
#define __SIM_REG_RCV_STATUS_BWT            (1 << 10)
#define __SIM_REG_RCV_STATUS_RTE            (1 << 9)
#define __SIM_REG_RCV_STATUS_CWT            (1 << 8)
#define __SIM_REG_RCV_STATUS_CRCOK          (1 << 7)
#define __SIM_REG_RCV_STATUS_LRCOK          (1 << 6)
#define __SIM_REG_RCV_STATUS_RDRF           (1 << 5)
#define __SIM_REG_RCV_STATUS_RFD            (1 << 4)
#define __SIM_REG_RCV_STATUS_RFE            (1 << 1)
#define __SIM_REG_RCV_STATUS_OEF            (1 << 0)

/* sim interrupt mask register */
#define __SIM_REG_INT_MASK                  (0x30)
#define __SIM_REG_INT_MASK_RFEM             (1 << 13)
#define __SIM_REG_INT_MASK_BGTM             (1 << 12)
#define __SIM_REG_INT_MASK_BWTM             (1 << 11)
#define __SIM_REG_INT_MASK_RTM              (1 << 10)
#define __SIM_REG_INT_MASK_CWTM             (1 << 9)
#define __SIM_REG_INT_MASK_GPCM             (1 << 8)
#define __SIM_REG_INT_MASK_TDTFM            (1 << 7)
#define __SIM_REG_INT_MASK_TFOM             (1 << 6)
#define __SIM_REG_INT_MASK_XTM              (1 << 5)
#define __SIM_REG_INT_MASK_TFEIM            (1 << 4)
#define __SIM_REG_INT_MASK_ETCIM            (1 << 3)
#define __SIM_REG_INT_MASK_OIM              (1 << 2)
#define __SIM_REG_INT_MASK_TCIM             (1 << 1)
#define __SIM_REG_INT_MASK_RIM              (1 << 0)

//#define __SIM_REG_PORTO_XMT_BUF             (0x34)

/* sim receive buffer register */
//#define __SIM_REG_PORT0_RCV_BUF             (0x38)

#define __SIM_REG_DATA_FORMAT               (0x40)

/* sim transmit threshold register */
#define __SIM_REG_XMT_THRESHOLD             (0x44)
#define __SIM_REG_XMT_THRESHOLD_XTH(x)      ((x&0x0f) << 4)
#define __SIM_REG_XMT_THRESHOLD_XTH_MASK    (0x0f << 4)
#define __SIM_REG_XMT_THRESHOLD_TDT(x)      ((x&0x0f) << 0)
#define __SIM_REG_XMT_THRESHOLD_TDT_MASK    (0x0f << 0)

/* sim transmit guard control register */
#define __SIM_REG_GUARD_CNTL                (0x48)
#define __SIM_REG_GUARD_CNTL_RCVR11         (1 << 8)
#define __SIM_REG_GIARD_CNTL_GETU(x)        (x&0xff)
#define __SIM_REG_GIARD_CNTL_GETU_MASK      (0xff)

/* sim open drain configuration control register */
#define __SIM_REG_OD_CONFIG                 (0x4C)
#define __SIM_REG_OD_CONFIG_OD_P1           (1 << 1)
#define __SIM_REG_OD_CONFIG_OD_P0           (1 << 0)

/* SIM reset cntl register */
#define __SIM_REG_RESET_CNTL                (0x50)
#define __SIM_REG_RESET_CNTL_FLUSH_RCV      (1 << 0)
#define __SIM_REG_RESET_CNTL_FLUSH_XMT      (1 << 1)
#define __SIM_REG_RESET_CNTL_SOFT_RESET     (1 << 2)
#define __SIM_REG_RESET_CNTL_KILL_CLOCK     (1 << 3)
#define __SIM_REG_RESET_CNTL_DOZE           (1 << 4)
#define __SIM_REG_RESET_CNTL_STOP           (1 << 5)
#define __SIM_REG_RESET_CNTL_DEBUG          (1 << 6)

/* sim character wait time Register */
#define __SIM_REG_CHAR_WAIT           (0x54)

/* sim general purpose counter register */
#define __SIM_REG_GPCNT               (0x58)

#define __SIM_REG_DIVISOR             (0x5C)

/* sim block wait time register */
#define __SIM_REG_BWT                 (0x60)

/* sim block guard time register */
#define __SIM_REG_BGT                 (0x64)

/* sim block wait time register high */
#define __SIM_REG_BWT_H               (0x68)

/* sim transmit FIFO status register */
#define __SIM_REG_XMT_FIFO_STAT       (0x6C)

/* simi receive FIFO counter register */
#define __SIM_REG_RCV_FIFO_CNT        (0x70)

#define __SIM_REG_RCV_FIFO_WPTR       (0x74)
#define __SIM_REG_RCV_FIFO_RPTR       (0x78)


/*SIM receive buffer register error status*/
#define __SIM_REC_CWT_ERROR           (1 << 10)
#define __SIM_REC_FRAME_ERROR         (1 << 9)
#define __SIM_REC_PARITY_ERROR        (1 << 8)

#define __SIM_EMV_NACK_THRESHOLD      (5)
#define __SIM_EMV_T0_BGT              (16)
#define __SIM_EMV_T1_BGT              (22)
#define __SIM_ATR_THRESHOLD_MAX       (100)
#define __SIM_ATR_MAX_CWT             (10080)
#define __SIM_ATR_MAX_DURATION        (20160)
#define __SIM_FCLK_FREQ               (4000000)

#define __SIM_ATR_TIMEOUT             (5)
#define __SIM_TX_TIMEOUT              (5)
#define __SIM_RX_TIMEOUT              (5)
#define __SIM_RESET_RETRY_TIMES       (5)
#define __SIM_QUIRK                   (1 << 0)
#define __SIM_EMV_RESET_LOW_CYCLES     40000
#define __SIM_ATR_MAX_DELAY_CLK        46400



/* write register */
#define __SIM_REG_WRITE(p_devinfo, offset, data) \
    AW_REG_WRITE32((p_devinfo)->regbase + (offset), (data))

/* read register */
#define __SIM_REG_READ(p_devinfo, offset) \
    AW_REG_READ32((p_devinfo)->regbase + (offset))

/* bit is set ? */
#define __SIM_REG_BIT_ISSET(p_devinfo, offset, bit) \
    AW_REG_BIT_ISSET32((p_devinfo)->regbase + (offset), bit)

/* set bit */
#define __SIM_REG_BIT_SET(p_devinfo, offset, bit) \
    AW_REG_BIT_SET32((p_devinfo)->regbase + (offset), bit)

/* clear bit */
#define __SIM_REG_BIT_CLR(p_devinfo, offset, bit) \
    AW_REG_BIT_CLR32((p_devinfo)->regbase + (offset), bit)

/* get register bits */
#define __SIM_REG_BITS_GET(p_devinfo, offset, start, len) \
    AW_REG_BITS_GET32((p_devinfo)->regbase + (offset), start, len)

/* set register bits */
#define __SIM_REG_BITS_SET(p_devinfo, offset, start, len, value) \
    AW_REG_BITS_SET32((p_devinfo)->regbase + (offset), start, len, value)

/* declare sim device instance */
#define __SIM_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_sim_dev *p_this = \
        (struct awbl_imx6ul_sim_dev *)(p_dev)

/* declare sim device infomation */
#define __SIM_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx6ul_sim_devinfo *p_devinfo = \
        (struct awbl_imx6ul_sim_devinfo *)AWBL_DEVINFO_GET(p_dev)



/*******************************************************************************
  forward declarations
*******************************************************************************/

/** \brief sim ioctl function */
aw_local aw_err_t __sim_ioctl (void *p_cookie, int cmd, void *p_arg);

/** \brief first level initialization routine */
aw_local void __sim_inst_init1 (awbl_dev_t *p_dev);

/** \brief second level initialization routine */
aw_local void __sim_inst_init2 (awbl_dev_t *p_dev);

/** \brief third level initialization routine */
aw_local void __sim_inst_connect (awbl_dev_t *p_dev);

/** \brief method "awbl_zlg600a_uartserv_get" handler */
aw_local aw_err_t __sim_simserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/**
 * \brief 时钟率转换因子
 */
aw_local aw_const uint16_t __g_sim_f[16] = {
    372,   372,  558,  744,
    1116, 1488, 1860,    0,
       0,  512,  768, 1024,
    1536, 2048,    0,    0
};

/**
 * \brief 波特率校正参数
 */
aw_local aw_const uint16_t __g_sim_d[16] = {
    0,   1,  2,  4,
    8,  16, 32,  0,
    12, 20,  0,  0,
    0,   0,  0,  0
};

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sim_drvfuncs = {
    __sim_inst_init1,
    __sim_inst_init2,
    __sim_inst_connect
};

/* pcf8563 service functions (must defined as aw_const) */
aw_local aw_const struct awbl_sim_servopts __g_sim_servopts = {
    __sim_ioctl,
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_simserv_get);

aw_local aw_const struct awbl_dev_method __g_sim_dev_methods[] = {
    AWBL_METHOD(awbl_simserv_get, __sim_simserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_sim_drv_registration = {
    AWBL_VER_1,                       /* awb_ver */
    AWBL_BUSID_PLB,                   /* bus_id */
    AWBL_IMX6UL_SIM_NAME,             /* p_drvname */
    &__g_sim_drvfuncs,                /* p_busfuncs */
    &__g_sim_dev_methods[0],          /* p_methods */
    NULL                              /* pfunc_drv_probe */
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief 服务模块
 */
aw_local aw_err_t __sim_reset_module (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;
    uint8_t  timeout = __SIM_RESET_RETRY_TIMES;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_RESET_CNTL);
    reg_val |= (__SIM_REG_RESET_CNTL_SOFT_RESET);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RESET_CNTL, reg_val);

    while (__SIM_REG_READ(p_devinfo, __SIM_REG_RESET_CNTL) & __SIM_REG_RESET_CNTL_SOFT_RESET) {
        aw_udelay(50);
        if (timeout-- <= 0) {
            AW_DBGF("SIM module reset timeout\n");
            return -AW_EINVAL;
        }
    }

    return AW_OK;
}

/**
 * \brief 数据复位
 */
aw_local void __sim_data_reset (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);

    p_this->error = AWBL_SIM_OK;
    p_this->serv.protocol_type  = 0;
    p_this->timeout        = 0;
    p_this->nack_threshold = __SIM_EMV_NACK_THRESHOLD;
    p_this->nack_enable    = AW_FALSE;

    memset(&p_this->timing_data, 0, sizeof(p_this->timing_data));
    //memset(&p_this->baud_rate, 0, sizeof(p_this->baud_rate));

    p_this->xmt_remaining = 0;
    p_this->xmt_pos       = 0;
    p_this->rcv_count     = 0;
    p_this->rcv_head      = 0;
    //p_this->last_is_tx    = AW_FALSE;

    memset(p_devinfo->p_rcv_buffer, 0, p_devinfo->rcv_buffer_size);
    memset(p_devinfo->p_xmt_buffer, 0, p_devinfo->xmt_buffer_size);
};

/**
 * \brief sim 启动
 */
aw_local void __sim_start (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);

    uint32_t reg_data, clk_rate, clk_div = 0;
    AW_DBGF(("%s entering.\n", __func__));

    if (p_devinfo->port_index == 1) {
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_SETUP, __SIM_REG_SETUP_SPS_PORT1);
    } else {
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_SETUP, __SIM_REG_SETUP_SPS_PORT0);
    }

    /*1 ~ 5 MHz */
    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);
    clk_div = (clk_rate + __SIM_FCLK_FREQ - 1) / __SIM_FCLK_FREQ;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CLK_PRESCALER, clk_div);

    /* Set the port pin to be open drained */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_OD_CONFIG);
    if (p_devinfo->port_index == 1) {
        reg_data |= __SIM_REG_OD_CONFIG_OD_P1;
    } else {
        reg_data |= __SIM_REG_OD_CONFIG_OD_P0;
    }
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_OD_CONFIG, reg_data);

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);

    /* One pin mode */
    reg_data |= __SIM_REG_PORT_CNTL_3VOLT | __SIM_REG_PORT_CNTL_STEN;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

    /* presense detect */
    AW_DBGF(("%s sim%d port1 detect is 0x%x \n",
            __func__,
            p_devinfo->servinfo.dev_id / 2 + 1,
            __SIM_REG_READ(p_devinfo, p_this->port_detect_reg)));

    if (__SIM_REG_READ(p_devinfo, p_this->port_detect_reg)
        & __SIM_REG_PORT_DETECT_SPDP) {
        reg_data = __SIM_REG_READ(p_devinfo, p_this->port_detect_reg);
        reg_data &= ~__SIM_REG_PORT_DETECT_SPDS;
        __SIM_REG_WRITE(p_devinfo, p_this->port_detect_reg, reg_data);
        p_this->present = AWBL_SIM_PRESENT_REMOVED;
        p_this->state   = __SIM_STATE_REMOVED;
    } else {
        reg_data = __SIM_REG_READ(p_devinfo, p_this->port_detect_reg);
        reg_data |= __SIM_REG_PORT_DETECT_SPDS;
        __SIM_REG_WRITE(p_devinfo, p_this->port_detect_reg, reg_data);
        p_this->present = AWBL_SIM_PRESENT_DETECTED;
        p_this->state   = __SIM_STATE_DETECTED;
    }

    /* enable card interrupt. clear interrupt status */
    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_detect_reg);
    reg_data |= __SIM_REG_PORT_DETECT_SDI;
    reg_data |= __SIM_REG_PORT_DETECT_SDIM;
    __SIM_REG_WRITE(p_devinfo, p_this->port_detect_reg, reg_data);
}

/**
 * \brief sim activate function
 */
aw_local void __sim_activate (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    AW_DBGF(("%s Activate on the sim%d port%d.\n",
            __func__,
            p_devinfo->servinfo.dev_id / 2 + 1,
            p_devinfo->servinfo.dev_id % 2));

    /* activate on sequence */
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {

        /* Disable Reset pin */
        reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
        reg_data &= ~__SIM_REG_PORT_CNTL_SRST;

        /* If sven is low active, we need to set sevn to be high */
        if (p_devinfo->sven_low_active) {
            reg_data |= __SIM_REG_PORT_CNTL_SVEN;
        }

        __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

        /* Enable VCC pin */
        reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);

        if (p_devinfo->sven_low_active) {
            reg_data &= ~__SIM_REG_PORT_CNTL_SVEN;
        } else {
            reg_data |= __SIM_REG_PORT_CNTL_SVEN;
        }

        __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

        aw_mdelay(10);

        /* Enable clock pin */
        reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
        reg_data |= __SIM_REG_PORT_CNTL_SCEN;
        __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

        aw_mdelay(10);
    } else {
        AW_DBGF(("No card%s\n", __func__));
    }
}

/**
 * \brief sim reset gpc timer function
 */
aw_local void __sim_timer_reset (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);
}

/**
 * \brief sim timer start function
 */
aw_local void __sim_timer_start(struct awbl_imx6ul_sim_dev *p_this,
                                uint8_t                     clk_source)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK;
    reg_data |= __SIM_REG_CNTL_GPCNT_CLK_SEL(clk_source);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);
}

/**
 * \brief sim set gpc_timer function
 */
aw_local void __sim_gpc_timer_set (struct awbl_imx6ul_sim_dev *p_this,
                                   uint32_t                    val)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    /* clear the interrupt status */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_STATUS);
    reg_data |= __SIM_REG_XMT_STATUS_GPCNT;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, reg_data);

    /* set the timer counter */
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_GPCNT, val);

    /* First reset the counter */
    __sim_timer_reset(p_this);

    /* Enable GPC timer interrupt */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_data &= ~__SIM_REG_INT_MASK_GPCM;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

    /* set the GPCNT clock source to be Fclk */
    __sim_timer_start(p_this, __SIM_REG_CNTL_GPCNT_CARD_CLK);
}

/**
 * \brief sim reset low timing
 */
aw_local aw_err_t __sim_low_timing_reset (struct awbl_imx6ul_sim_dev *p_this,
                                          uint32_t                    clock_cycle)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    //int      errval = 0;
    uint32_t timeout, reg_data;
    aw_err_t ret;

    timeout = __SIM_EMV_RESET_LOW_CYCLES * 1000 / __SIM_FCLK_FREQ;

    __sim_gpc_timer_set(p_this, clock_cycle);

    ret = AW_SEMB_TAKE(p_this->semb, aw_ms_to_ticks(timeout * 2));
    if (ret == -AW_ETIME) {
        AW_DBGF("Reset low GPC timout\n");
    }

    __sim_timer_reset(p_this);

    /* disable GPC timer interrupt */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_data |= __SIM_REG_INT_MASK_GPCM;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

    return ret;
}

/**
 * \brief sim cwt set function
 */
aw_local void __sim_cwt_set (struct awbl_imx6ul_sim_dev *p_this,
                             aw_bool_t                      enable)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    if (enable && p_this->timing_data.cwt) {
        reg_val |= __SIM_REG_CNTL_CWTEN;
    } else {
        reg_val &= ~__SIM_REG_CNTL_CWTEN;
    }
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_val);
}

/**
 * \brief sim bwt set function
 */
aw_local void __sim_bwt_set (struct awbl_imx6ul_sim_dev *p_this,
                             aw_bool_t                      enable)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    if (enable && (p_this->timing_data.bwt || p_this->timing_data.bgt)) {
        reg_val |= __SIM_REG_CNTL_BWTEN;
    } else {
        reg_val &= ~__SIM_REG_CNTL_BWTEN;
    }
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_val);
}

/**
 * \brief sim cold reset sequency function
 */
aw_local void __sim_cold_reset_sequency (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    p_this->state = __SIM_STATE_RESET_SEQUENCY;
    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data &= ~__SIM_REG_PORT_CNTL_SRST;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);

    if (p_devinfo->sven_low_active) {
        reg_data &= ~__SIM_REG_PORT_CNTL_SVEN;
    } else {
        reg_data |= __SIM_REG_PORT_CNTL_SVEN;
    }

    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
    aw_mdelay(9);

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data |= __SIM_REG_PORT_CNTL_SCEN;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

    __sim_low_timing_reset(p_this, __SIM_EMV_RESET_LOW_CYCLES);

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data |= __SIM_REG_PORT_CNTL_SRST;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
    __sim_gpc_timer_set(p_this, __SIM_ATR_MAX_DELAY_CLK);
}

/**
 * \brief sim deactivate function
 */
aw_local void __sim_deactivate (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    AW_DBGF(("%s entering.\n", __func__));

    /* Auto powdown to implement the deactivate sequence */
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {
        if (p_devinfo->sven_low_active) {

            /* Set the RESET to be low */
            reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
            reg_data &= ~__SIM_REG_PORT_CNTL_SRST;
            __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
            aw_udelay(5);

            /* Set the clock to be low */
            reg_data &= ~__SIM_REG_PORT_CNTL_SCEN;
            __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
            aw_udelay(5);

            /* Set the sven to be high */
            reg_data |= __SIM_REG_PORT_CNTL_SVEN;
            __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

        } else {
            reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);;
            reg_data |= __SIM_REG_PORT_CNTL_SAPD;
            __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
            reg_data |= __SIM_REG_PORT_CNTL_SFPD;
            __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
        }
    } else {
        AW_DBGF(("No card%s\n", __func__));
    }
}

/**
 * \brief sim NACK set function
 */
aw_local void __sim_nack_set (struct awbl_imx6ul_sim_dev *p_this,
                              aw_bool_t                      enable)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);

    /* Disable overrun NACK setting for now */
    reg_val &= ~(__SIM_REG_CNTL_ONACK);

    if (enable) {
        reg_val |= __SIM_REG_CNTL_ANACK;
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_val);
        reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_THRESHOLD);
        reg_val &= ~(__SIM_REG_XMT_THRESHOLD_XTH_MASK);
        reg_val |= __SIM_REG_XMT_THRESHOLD_XTH(p_this->nack_threshold);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_THRESHOLD, reg_val);
    } else {
        reg_val &= ~__SIM_REG_CNTL_ANACK;
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_val);
    }

    p_this->nack_enable = enable;
}

/**
 * \brief sim transmit set function
 */
aw_local void __sim_tx_set (struct awbl_imx6ul_sim_dev *p_this,
                            aw_bool_t                      enable)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_ENABLE);
    if (enable) {
        reg_data |= __SIM_REG_ENABLE_XMTEN | __SIM_REG_ENABLE_RCVEN;
        if (p_this->quirks & __SIM_QUIRK) {
            reg_data &= ~(__SIM_REG_ENABLE_ESTOP_EN | __SIM_REG_ENABLE_ESTOP_EXE);
        }
    } else {
        reg_data &= ~__SIM_REG_ENABLE_XMTEN;
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_ENABLE, reg_data);
}

/**
 * \brief sim receive set function
 */
aw_local void __sim_rx_set (struct awbl_imx6ul_sim_dev *p_this,
                            aw_bool_t                      enable)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_ENABLE);
    if (enable) {
        reg_data |= __SIM_REG_ENABLE_RCVEN;
        reg_data &= ~__SIM_REG_ENABLE_XMTEN;
        if (p_this->quirks & __SIM_QUIRK) {
            reg_data |= (__SIM_REG_ENABLE_ESTOP_EN | __SIM_REG_ENABLE_ESTOP_EXE);
        }
    } else {
        reg_data &= ~__SIM_REG_ENABLE_RCVEN;
        if (p_this->quirks & __SIM_QUIRK) {
            reg_data &= ~(__SIM_REG_ENABLE_ESTOP_EN | __SIM_REG_ENABLE_ESTOP_EXE);
        }
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_ENABLE, reg_data);
}

/**
 * \brief sim receive atr set function
 */
aw_local void __sim_receive_atr_set (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    /* Enable RX */
    __sim_rx_set(p_this, AW_TRUE);

    /* Receive fifo threshold = 1 */
    reg_data = __SIM_REG_RCV_THRESHOLD_RTH(0) | __SIM_REG_RCV_THRESHOLD_RDT(1);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_THRESHOLD, reg_data);

    /* Clear the interrupt status */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_RCV_STATUS);
    reg_data |= (__SIM_REG_RCV_STATUS_CWT | __SIM_REG_RCV_STATUS_RDRF);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_STATUS, reg_data);

    /* Set the cwt timer.Refer the setting of ATR on EMV4.3 book */
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CHAR_WAIT, __SIM_ATR_MAX_CWT);

    /*
     * Set the baud rate to be 1/372. Refer the setting of ATR on EMV4.3 book
     * Enable the CWT timer during receiving ATR process.
     */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~__SIM_REG_CNTL_BAUD_SEL_MASK;
    reg_data |= __SIM_REG_CNTL_BAUD_SEL(0) | __SIM_REG_CNTL_CWTEN;

    /* Enable ICM mode */
    reg_data |= __SIM_REG_CNTL_ICM;

    /* Enable Sample12 */
    reg_data |= __SIM_REG_CNTL_SAMPLE12;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

    /* Disable NACK */
    __sim_nack_set(p_this, AW_FALSE);

    /* Set 12 ETUS */
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_GUARD_CNTL, 0);

    p_this->error = AWBL_SIM_OK;
    p_this->rcv_count = 0;
    p_this->checking_ts_timing = 1;
    p_this->state = __SIM_STATE_ATR_RECEIVING;

    /* Enable the RIM and GPC interrupt, disalbe the CWT interrupt */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_data |= __SIM_REG_INT_MASK_CWTM;
    reg_data &= ~(__SIM_REG_INT_MASK_RIM | __SIM_REG_INT_MASK_GPCM);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);
}

/**
 * \brief sim receive data check
 */
aw_local int __sim_rec_data_check (uint32_t *reg_data)
{
    int err = 0;

    if (*reg_data & __SIM_REC_CWT_ERROR) {
        err |= AWBL_SIM_ERROR_CWT;
    }

    if (*reg_data & __SIM_REC_FRAME_ERROR) {
        err |= AWBL_SIM_ERROR_FRAME;
    }

    if (*reg_data & __SIM_REC_PARITY_ERROR) {
        err |= AWBL_SIM_ERROR_PARITY;
    }

    return err;
}

/**
 * \brief sim transmit fifo fill function
 */
aw_local void __sim_xmt_fifo_fill (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;
    uint32_t bytesleft, i;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_FIFO_STAT);
    bytesleft = SIM_TX_FIFO_DEPTH - ((reg_data >> 8) & 0x0F);

    if (bytesleft > p_this->xmt_remaining) {
        bytesleft = p_this->xmt_remaining;
    }

    for (i = 0; i < bytesleft; i++) {
        __SIM_REG_WRITE(p_devinfo,
                        __SIM_REG_PORT_XMT_BUF,
                        p_devinfo->p_xmt_buffer[p_this->xmt_pos]);
        p_this->xmt_pos++;
    }
    p_this->xmt_remaining -= bytesleft;
}

/**
 * \brief sim receive fifo read function
 */
static void __sim_rcv_fifo_read (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint16_t i, count;
    uint32_t reg_data;

    count  = __SIM_REG_READ(p_devinfo, __SIM_REG_RCV_FIFO_CNT);

    aw_spinlock_isr_take(&p_this->dev_lock);
    for (i = 0; i < count; i++) {
        reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_PORT_RCV_BUF);
        p_this->error |= __sim_rec_data_check(&reg_data);

        /*
         * T1 mode and t0 mode no parity error, T1 mode SIM module will not produce NACK be
         * NACK is disabled. T0 mode to ensure there is no parity error for the current byte
         */
        if (!(p_this->nack_enable && (reg_data & __SIM_REC_PARITY_ERROR))) {
            p_devinfo->p_rcv_buffer[p_this->rcv_head + p_this->rcv_count] = (uint8_t)reg_data;
            p_this->rcv_count++;
        }
        if (p_this->rcv_head + p_this->rcv_count >= p_devinfo->rcv_buffer_size) {
            //pr_err("The software fifo is full,head %d, cnt%d\n", sim->rcv_head, sim->rcv_count);
            break;
        }
    }
    aw_spinlock_isr_give(&p_this->dev_lock);
}

/**
 * \brief sim transmit interrupt enable
 */
aw_local void __sim_tx_int_enable (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    /* Clear the status and enable the related interrupt */
    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_STATUS);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, reg_val);
    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_RCV_STATUS);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_STATUS, reg_val);

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);

    /*
     * Disable CWT , BWT interrupt when transmitting, it would
     * be enabled when rx is enabled just after tx completes
     * The timer will be enabled.
     */
    reg_val |= __SIM_REG_INT_MASK_CWTM | __SIM_REG_INT_MASK_BWTM;
    reg_val |= __SIM_REG_INT_MASK_RIM | __SIM_REG_INT_MASK_RTM;

    if (p_this->xmt_remaining != 0) {
        reg_val &= ~__SIM_REG_INT_MASK_TDTFM;
    } else {
        reg_val &= ~__SIM_REG_INT_MASK_TCIM;

        /* Enable transmit early complete interrupt. */
        reg_val &= ~__SIM_REG_INT_MASK_ETCIM;
    }

    /* NACK interrupt is enabled only when T0 mode */
    if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0 || p_this->nack_enable) {
        reg_val &= ~__SIM_REG_INT_MASK_XTM;
    } else {
        reg_val |= __SIM_REG_INT_MASK_XTM;
    }
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_val);
}

/**
 * \brief sim transmit interrupt disable
 */
aw_local void __sim_tx_int_disable (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    /* Disable the NACK interruptand TX related interrupt */
    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_val |= (__SIM_REG_INT_MASK_TDTFM |
                __SIM_REG_INT_MASK_TCIM  |
                __SIM_REG_INT_MASK_XTM   |
                __SIM_REG_INT_MASK_ETCIM);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_val);
}

/**
 * \brief sim receive interrput enable
 */
aw_local void __sim_rx_int_enable (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    /*
     * Ensure the CWT timer is enabled.
     */
    __sim_cwt_set(p_this, AW_TRUE);
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_data |= (__SIM_REG_INT_MASK_TCIM | __SIM_REG_INT_MASK_TDTFM | __SIM_REG_INT_MASK_XTM);
    reg_data &= ~(__SIM_REG_INT_MASK_RIM | __SIM_REG_INT_MASK_CWTM | __SIM_REG_INT_MASK_BWTM);

    if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0 || p_this->nack_enable) {
        reg_data &= ~__SIM_REG_INT_MASK_RTM;
    } else {
        reg_data |= __SIM_REG_INT_MASK_RTM;
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);
}

/**
 * \brief sim transmit interrupt enable
 */
aw_local void __sim_rx_int_disable (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_val |= (__SIM_REG_INT_MASK_RIM  |
                __SIM_REG_INT_MASK_CWTM |
                __SIM_REG_INT_MASK_BWTM |
                __SIM_REG_INT_MASK_RTM);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_val);
}

/**
 * \brief sim cold reset function
 */
aw_local void __sim_cold_reset (struct awbl_imx6ul_sim_dev *p_this)
{
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {
        p_this->state   = __SIM_STATE_DETECTED;
        p_this->present = AWBL_SIM_PRESENT_DETECTED;

        __sim_cold_reset_sequency(p_this);
        __sim_receive_atr_set(p_this);
    } else {
        AW_DBGF(("No card%s\n", __func__));
    }
}

/**
 * \brief sim warm reset sequency function
 */
aw_local void __sim_warm_reset_sequency (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    p_this->state = __SIM_STATE_RESET_SEQUENCY;

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data |= (__SIM_REG_PORT_CNTL_SRST | __SIM_REG_PORT_CNTL_SCEN);
    if (p_devinfo->sven_low_active) {
        reg_data &= ~__SIM_REG_PORT_CNTL_SVEN;
    } else {
        reg_data |= __SIM_REG_PORT_CNTL_SVEN;
    }
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

    aw_udelay(25);

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data &= ~__SIM_REG_PORT_CNTL_SRST;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);

    __sim_low_timing_reset(p_this, __SIM_EMV_RESET_LOW_CYCLES);

    reg_data = __SIM_REG_READ(p_devinfo, p_this->port_ctrl_reg);
    reg_data |= __SIM_REG_PORT_CNTL_SRST;
    __SIM_REG_WRITE(p_devinfo, p_this->port_ctrl_reg, reg_data);
    __sim_gpc_timer_set(p_this, __SIM_ATR_MAX_DELAY_CLK);
}

/**
 * \brief sim warm reset function
 */
aw_local void __sim_warm_reset (struct awbl_imx6ul_sim_dev *p_this)
{
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {
        __sim_data_reset(p_this);
        __sim_warm_reset_sequency(p_this);
        __sim_receive_atr_set(p_this);
    } else {
        AW_DBGF(("No card%s\n", __func__));
    }
}

/**
 * \brief sim lock card
 */
aw_local aw_err_t __sim_card_lock (struct awbl_imx6ul_sim_dev *p_this)
{
    aw_err_t ret;

    /* place holder for true physcial locking */
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {
        ret = AW_OK;
    } else {
        ret = -AW_EAGAIN;
    }
    return ret;
}

/**
 * \brief sim eject card
 */
aw_local aw_err_t __sim_card_eject (struct awbl_imx6ul_sim_dev *p_this)
{
    aw_err_t ret;

    AW_DBGF(("%s entering.\n", __func__));

    /* place holder for true physcial ejecting */
    if (p_this->present != AWBL_SIM_PRESENT_REMOVED) {
        ret = AW_OK;
    } else {
        ret = -AW_EAGAIN;
    }

    return ret;
};

/**
 * \brief sim baudrate set function
 */
aw_local aw_err_t __sim_baud_rate_set (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~(__SIM_REG_CNTL_BAUD_SEL_MASK);

    switch (__g_sim_d[p_this->baud_rate.di]) {

    case 1 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(0);
        break;

    case 2 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(1);
        break;

    case 4 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(2);
        break;

    case 8 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(3);
        break;

    case 16 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(4);
        break;

    case 32 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(5);
        break;

    case 64 :
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(6);
        break;

    default:

        if (__g_sim_f[p_this->baud_rate.fi] == 0 ||
            __g_sim_d[p_this->baud_rate.di] == 0) {
            break;
        }
        reg_data |= __SIM_REG_CNTL_BAUD_SEL(7);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);
        __SIM_REG_WRITE(p_devinfo,
                        __SIM_REG_DIVISOR,
                        __g_sim_f[p_this->baud_rate.fi] / __g_sim_d[p_this->baud_rate.di]);
//        reg_data |= __SIM_REG_CNTL_BAUD_SEL(0);
        break;
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

    return AW_OK;
}

/**
 * \brief sim timing data check function
 */
aw_local aw_err_t __sim_timing_data_check (awbl_sim_timing_t *p_timing_data)
{
    if (p_timing_data->wwt > 0xFFFF ||
        p_timing_data->cwt > 0xFFFF ||
        p_timing_data->bgt > 0xFFFF ||
        p_timing_data->cgt > 0xFF) {

        /*Check whether the counter is out of the scope of SIM IP*/
        AW_DBGF("The timing value is out of scope of IP\n");

        return -AW_EINVAL;
    }

    return AW_OK;
}

/**
 * \brief sim timer counter set function
 */
aw_local void __sim_timer_counter_set (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);

    if (p_this->timing_data.wwt != 0 &&
        p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0) {
        p_this->timing_data.cwt = p_this->timing_data.wwt;
        p_this->timing_data.bwt = p_this->timing_data.wwt;
    }


    if (p_this->timing_data.bgt != 0) {
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_BGT, p_this->timing_data.bgt);
    }

    if (p_this->timing_data.cwt != 0) {
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CHAR_WAIT, p_this->timing_data.cwt);
    }

    if (p_this->timing_data.bwt != 0) {

        __SIM_REG_WRITE(p_devinfo, __SIM_REG_BWT, p_this->timing_data.bwt & 0x0000FFFF);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_BWT_H, (p_this->timing_data.bwt >> 16) & 0x0000FFFF);
    }

    if (p_this->timing_data.cgt == 0xFF && p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0) {

        /*
         * From EMV4.3 , CGT =0xFF in T0 mode means 12 ETU.
         * Set register to be 12 ETU for transmitting and receiving.
         */
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_GUARD_CNTL, 0);
    } else if (p_this->timing_data.cgt == 0xFF && p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T1) {

        /*
         * From EMV4.3 , CGT =0xFF in T1 mode means 11 ETU.
         * Set register to be 12 ETU for transmitting and receiving.
         */
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_GUARD_CNTL, 0x1FF);
    } else if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T1) {

        /* For the T1 mode, use 11ETU to receive. */
        __SIM_REG_WRITE(p_devinfo,
                        __SIM_REG_GUARD_CNTL,
                       (p_this->timing_data.cgt | __SIM_REG_GUARD_CNTL_RCVR11));
    } else {

        /* sim->protocol_type == AWBL_SIM_PROTOCOL_T0 */
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_GUARD_CNTL, p_this->timing_data.cgt);
    }
}

/**
 * \brief sim xmt start function
 */
aw_local void __sim_xmt_start (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    /* Set TX threshold if there are remaing data */
    if (p_this->xmt_remaining != 0) {
        reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_THRESHOLD);
        reg_val &= ~__SIM_REG_XMT_THRESHOLD_TDT_MASK;
        reg_val |= __SIM_REG_XMT_THRESHOLD_TDT(__SIM_TX_FIFO_THRESHOLD);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_THRESHOLD, reg_val);
    }

    __sim_tx_int_enable(p_this);

    /* Enable  BWT and disalbe CWT timers when tx */
    __sim_bwt_set(p_this, AW_TRUE);
    __sim_cwt_set(p_this, AW_FALSE);

    /* Disalbe RX */
    __sim_rx_set(p_this, AW_FALSE);

    /* Enable TX */
    __sim_tx_set(p_this, AW_TRUE);
}

/**
 * \brief sim fifo flush function
 */
aw_local void __sim_fifo_flush (struct awbl_imx6ul_sim_dev *p_this,
                                aw_bool_t                      flush_tx,
                                aw_bool_t                      flush_rx)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_val;

    reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_RESET_CNTL);

    if (flush_tx) {
        reg_val |= __SIM_REG_RESET_CNTL_FLUSH_XMT;
    }

    if (flush_rx) {
        reg_val |= __SIM_REG_RESET_CNTL_FLUSH_RCV;
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RESET_CNTL, reg_val);

    aw_udelay(3);

    if (flush_tx) {
        reg_val &= ~(__SIM_REG_RESET_CNTL_FLUSH_XMT);
    }

    if (flush_rx) {
        reg_val &= ~(__SIM_REG_RESET_CNTL_FLUSH_RCV);
    }

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RESET_CNTL, reg_val);
}

/**
 * \brief sim receive threshold change function
 */
aw_local void __sim_rcv_threshold_change (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t rx_threshold = 0;
    uint32_t reg_val = 0;

    if (p_this->is_fixed_len_rec) {
        rx_threshold = p_this->expected_rcv_cnt - p_this->rcv_count;
        reg_val = __SIM_REG_READ(p_devinfo, __SIM_REG_RCV_THRESHOLD);
        reg_val &= ~(__SIM_REG_RCV_THRESHOLD_RDT_MASK);
        reg_val |= __SIM_REG_RCV_THRESHOLD_RDT(rx_threshold);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_THRESHOLD, reg_val);
    }
}

/**
 * \brief sim receive start function
 */
aw_local void __sim_rcv_start (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);

    __sim_baud_rate_set(p_this);
    if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0) {
        __sim_nack_set(p_this, AW_TRUE);
    } else if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T1) {
        __sim_nack_set(p_this, AW_FALSE);
    }

    /* Set RX threshold */
    if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0) {
        __SIM_REG_WRITE(p_devinfo,
                        __SIM_REG_RCV_THRESHOLD,
                        __SIM_REG_RCV_THRESHOLD_RTH(p_this->nack_threshold) |
                        __SIM_REG_RCV_THRESHOLD_RDT(RX_FIFO_THRESHOLD));
    } else {
        __SIM_REG_WRITE(p_devinfo,
                        __SIM_REG_RCV_THRESHOLD,
                        __SIM_REG_RCV_THRESHOLD_RDT(RX_FIFO_THRESHOLD));
    }

    /* Clear status and enable interrupt */
    __sim_rx_int_enable(p_this);

    /* Disalbe TX and Enable Rx */
    __sim_rx_set(p_this, AW_TRUE);
    __sim_tx_set(p_this, AW_FALSE);
}

/**
 * \brief sim poll delay
 */
aw_local void __sim_polling_delay (struct awbl_imx6ul_sim_dev *p_this,
                                   uint32_t                    delay)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data;

    /* Reset the timer */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK;
    reg_data |= __SIM_REG_CNTL_GPCNT_CLK_SEL(0);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

    /* Clear the interrupt status */
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, __SIM_REG_XMT_STATUS_GPCNT);

    /* Disable timer interrupt */
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
    reg_data |= __SIM_REG_INT_MASK_GPCM;
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

    __SIM_REG_WRITE(p_devinfo, __SIM_REG_GPCNT, delay);

    /*Set the ETU as clock source and start timer*/
    reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
    reg_data &= ~__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK;
    reg_data |= __SIM_REG_CNTL_GPCNT_CLK_SEL(3);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

    /* Loop for timeout */
    while (!(__SIM_REG_READ(p_devinfo, __SIM_REG_XMT_STATUS) & __SIM_REG_XMT_STATUS_GPCNT)) {
        aw_udelay(20);
    }
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, __SIM_REG_XMT_STATUS_GPCNT);
}

/**
 * \brief sim receive fifo clear
 */
aw_local void __sim_rx_buf_clear (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t i;

    for (i = 0; i < p_devinfo->rcv_buffer_size; i++) {
        p_devinfo->p_rcv_buffer[i] = 0;
    }

    p_this->rcv_count = 0;
    p_this->rcv_head = 0;
}

/**
 * \brief sim ioctl function
 */
aw_local aw_err_t __sim_ioctl (void *p_cookie, int cmd, void *p_arg)
{
    __SIM_DEV_DECL(p_this, p_cookie);
    __SIM_DEVINFO_DECL(p_devinfo, p_cookie);
    uint32_t reg_data;
    uint32_t copy_cnt;

    aw_err_t ret = AW_OK;

    AW_DBGF(("%s entering.\n", __func__));

    switch (cmd) {

    case AWBL_SIM_IOCTL_GET_PRESENSE :
        *(int *)p_arg = p_this->state;
        break;

    case AWBL_SIM_IOCTL_GET_ATR :
        if (p_this->present != AWBL_SIM_PRESENT_DETECTED) {
            AW_DBGF(("NO card ...\n"));
            ret = -AW_ENODEV;
            break;
        }
        p_this->timeout = __SIM_ATR_TIMEOUT * 1000;

        ret = AW_SEMB_TAKE(p_this->semb, aw_ms_to_ticks(p_this->timeout));

        /* Disable the GPCNT timer and CWT timer right now */
        reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
        reg_data &= ~(__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK | __SIM_REG_CNTL_CWTEN);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

        reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
        reg_data |= (__SIM_REG_INT_MASK_GPCM | __SIM_REG_INT_MASK_CWTM);
        __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

        if (ret != AW_OK) {
            AW_DBGF("ATR timeout\n");
            break;
        }

        if (p_arg == NULL) {
            AW_DBGF(("ATR access rcv_count error\n"));
            ret = -AW_EINVAL;
            break;
        }
        ((awbl_sim_atr_t *)p_arg)->size = p_this->rcv_count;

        if (((awbl_sim_atr_t *)p_arg)->p_atr_buf == NULL) {
            AW_DBGF(("ATR access buffer error\n"))
            ret = -AW_EINVAL;
            break;
        }
        memcpy(((awbl_sim_atr_t *)p_arg)->p_atr_buf,
               p_devinfo->p_rcv_buffer,
               p_this->rcv_count);

        ((awbl_sim_atr_t *)p_arg)->error = p_this->error;

        p_this->rcv_count = 0;
        p_this->rcv_head  = 0;
        p_this->error     = 0;
        break;

    case AWBL_SIM_IOCTL_XMT :
        if (p_arg == NULL) {
            AW_DBGF(("XMT access xmt_length error.\n"));
            ret = -AW_EINVAL;
            break;
        }

        p_this->xmt_remaining = ((awbl_sim_xmt_t *)p_arg)->xmt_length;
        if (p_this->xmt_remaining > p_devinfo->xmt_buffer_size) {
            AW_DBGF("XMT no enough buffer.\n");
            ret = -AW_EINVAL;
            break;
        }

        if (((awbl_sim_xmt_t *)p_arg)->p_xmt_buf == NULL) {
            AW_DBGF(("XMT access xmt buffer error.\n"));
            ret = -AW_EINVAL;
            break;
        }

        memcpy(p_devinfo->p_xmt_buffer,
              ((awbl_sim_xmt_t *)p_arg)->p_xmt_buf,
              ((awbl_sim_xmt_t *)p_arg)->xmt_length);

        __sim_rx_buf_clear(p_this);
        __sim_cwt_set(p_this, AW_FALSE);
        __sim_bwt_set(p_this, AW_FALSE);

        /* Flush the tx rx fifo */
        __sim_fifo_flush(p_this, AW_TRUE, AW_TRUE);
        p_this->xmt_pos = 0;
        p_this->error   = 0;

        __sim_xmt_fifo_fill(p_this);
        __sim_baud_rate_set(p_this);

        if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T0) {
            __sim_nack_set(p_this, AW_TRUE);
        } else if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T1) {
            __sim_nack_set(p_this, AW_FALSE);
        } else {
            AW_DBGF("Invalid protocol not T0 or T1\n");
            ret = -AW_EINVAL;
            break;
        }

        __sim_timer_counter_set(p_this);
        __sim_xmt_start(p_this);
        p_this->state = __SIM_STATE_XMTING;

        p_this->timeout = __SIM_TX_TIMEOUT * 1000;
        ret = AW_SEMB_TAKE(p_this->semb, aw_ms_to_ticks(p_this->timeout));
        if (ret != AW_OK) {

            /* Disable the NACK interruptand TX related interrupt */
            __sim_tx_int_disable(p_this);
            AW_DBGF("tx timeout\n");
        }

        if (ret != AW_OK || p_this->state == __SIM_STATE_XMT_ERROR) {

            AW_DBGF("TX error\n");

            /* Disable timers */
            __sim_cwt_set(p_this, AW_FALSE);
            __sim_bwt_set(p_this, AW_FALSE);

            /* Disable TX */
            __sim_tx_set(p_this, 0);

            /* Flush the tx fifos */
            __sim_fifo_flush(p_this, AW_TRUE, AW_FALSE);


            ((awbl_sim_xmt_t *)p_arg)->error = p_this->error;
            p_this->error = 0;
            break;
        }

        ((awbl_sim_xmt_t *)p_arg)->error = p_this->error;

        //p_this->last_is_tx = AW_TRUE;

        /* Start RX */
        p_this->error = 0;
        p_this->state = __SIM_STATE_RECEIVING;
        __sim_rcv_start(p_this);
        break;

    case AWBL_SIM_IOCTL_RCV :
        if (p_this->present != AWBL_SIM_PRESENT_DETECTED) {
            ret = -AW_EAGAIN;
            break;
        }
        p_this->is_fixed_len_rec = 0;

        if (p_arg == NULL) {
            AW_DBGF("RCV access rcv_length error.\n");
            ret = -AW_EINVAL;
            break;
        }
        p_this->expected_rcv_cnt = ((awbl_sim_rcv_t *)p_arg)->rcv_length;

        /* Set the length to be 0 at first */
        ((awbl_sim_rcv_t *)p_arg)->rcv_length = 0;

        if (p_this->expected_rcv_cnt != 0) {
            p_this->is_fixed_len_rec = 1;
        }

        if (p_this->is_fixed_len_rec &&
            p_this->rcv_count >= p_this->expected_rcv_cnt) {
            goto copy_data;
        }

        if (p_this->state != __SIM_STATE_RECEIVING) {

            __sim_timer_counter_set(p_this);

            /* Enable CWT BWT */
            __sim_cwt_set(p_this, AW_TRUE);
            __sim_bwt_set(p_this, AW_TRUE);
            p_this->state = __SIM_STATE_RECEIVING;
            __sim_rcv_start(p_this);
        }

        aw_spinlock_isr_take(&p_this->dev_lock);
        if (p_this->is_fixed_len_rec && p_this->rcv_count < p_this->expected_rcv_cnt) {
            __sim_rcv_threshold_change(p_this);
        }
        aw_spinlock_isr_give(&p_this->dev_lock);
        p_this->timeout = __SIM_RX_TIMEOUT * 1000;
        ret = AW_SEMB_TAKE(p_this->semb, aw_ms_to_ticks(p_this->timeout));

        if (ret != AW_OK) {
            AW_DBGF("Receiving timeout\n");

            __sim_cwt_set(p_this, AW_FALSE);
            __sim_bwt_set(p_this, AW_FALSE);
            __sim_rx_int_disable(p_this);
            break;
        }

copy_data:
        if (p_this->is_fixed_len_rec) {
            copy_cnt = p_this->rcv_count >= p_this->expected_rcv_cnt ? p_this->expected_rcv_cnt : p_this->rcv_count;
        } else {
            copy_cnt = p_this->rcv_count;
        }

        ((awbl_sim_rcv_t *)p_arg)->rcv_length = copy_cnt;

        if (((awbl_sim_rcv_t *)p_arg)->p_rcv_buf == NULL) {
            AW_DBGF("RCV access rcv buffer error.\n");
            ret = -AW_EINVAL;
            break;
        }

        memcpy(((awbl_sim_rcv_t *)p_arg)->p_rcv_buf,
               &p_devinfo->p_rcv_buffer[p_this->rcv_head],
               copy_cnt);

        ((awbl_sim_rcv_t *)p_arg)->error = p_this->error;

        /* Reset the receiving count and errval */
        aw_spinlock_isr_take(&p_this->dev_lock);
        p_this->rcv_head  += copy_cnt;
        p_this->rcv_count -= copy_cnt;
        p_this->error      = 0;
        aw_spinlock_isr_give(&p_this->dev_lock);

        //p_this->last_is_tx = AW_FALSE;
        break;

    case AWBL_SIM_IOCTL_ACTIVATE :
        __sim_activate(p_this);
        break;

    case AWBL_SIM_IOCTL_DEACTIVATE :
        __sim_deactivate(p_this);
        break;

    case AWBL_SIM_IOCTL_WARM_RESET :
        __sim_warm_reset(p_this);
        break;

    case AWBL_SIM_IOCTL_COLD_RESET :
        p_this->present = AWBL_SIM_PRESENT_REMOVED;
        p_this->state   = __SIM_STATE_REMOVED;

        __sim_reset_module(p_this);
        __sim_data_reset(p_this);
        __sim_start(p_this);
        __sim_activate(p_this);
        __sim_cold_reset(p_this);

        break;

    case AWBL_SIM_IOCTL_CARD_LOCK :
        ret = __sim_card_lock(p_this);
        break;

    case AWBL_SIM_IOCTL_CARD_EJECT :
        ret = __sim_card_eject(p_this);
        break;

    case AWBL_SIM_IOCTL_SET_PROTOCOL :
        p_this->serv.protocol_type = (uint32_t)p_arg;
        ret = AW_OK;
        break;

    case AWBL_SIM_IOCTL_SET_TIMING :
        memcpy(&p_this->timing_data, p_arg, sizeof(awbl_sim_timing_t));
        ret = __sim_timing_data_check(p_arg);
        break;

    case AWBL_SIM_IOCTL_SET_BAUD :
        memcpy(&p_this->baud_rate, p_arg, sizeof(awbl_sim_baud_t));
        ret = __sim_baud_rate_set(p_this);
        break;

    case AWBL_SIM_IOCTL_WAIT :
        __sim_polling_delay(p_this, (uint32_t)p_arg);
        break;

    case AWBL_SIM_IOCTL_GET_PROTOCOL :
        *(uint32_t *)p_arg = p_this->serv.protocol_type;
        ret = AW_OK;
        break;

    case AWBL_SIM_IOCTL_GET_TIMING :
        memcpy(p_arg, &p_this->timing_data, sizeof(awbl_sim_timing_t));
        ret = AW_OK;
        break;

    case AWBL_SIM_IOCTL_GET_BAUD :
        memcpy(p_arg, &p_this->baud_rate, sizeof(awbl_sim_baud_t));
        ret = AW_OK;
        break;

    default :
        break;
    }

    return ret;
}



/**
 * \brief sim interrupt service routine
 *
 * \param[in]   p_arg
 */
aw_local aw_err_t __sim_isr (void *p_arg)
{
    __SIM_DEV_DECL(p_this, p_arg);
    __SIM_DEVINFO_DECL(p_devinfo, p_arg);

    p_this->tx_status  = __SIM_REG_READ(p_devinfo, __SIM_REG_XMT_STATUS);
    p_this->rx_status  = __SIM_REG_READ(p_devinfo, __SIM_REG_RCV_STATUS);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, p_this->tx_status);
    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_STATUS, p_this->rx_status);

    if ((p_this->state == __SIM_STATE_RECEIVING      ||
         p_this->state == __SIM_STATE_ATR_RECEIVING) &&
        (p_this->rx_status & __SIM_REG_RCV_STATUS_RFE)) {
        return AW_OK;
    }

    AW_SEMB_GIVE(p_this->sim_sem_sync);

    __sim_tx_int_disable(p_this);
    __sim_rx_int_disable(p_this);

    return AW_OK;
}

/**
 * \brief sim status machine
 */
aw_local void __sim_sm_send_event (struct awbl_imx6ul_sim_dev *p_this)
{
    __SIM_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t reg_data, tx_status, rx_status;

    tx_status = p_this->tx_status;
    rx_status = p_this->rx_status;

    switch (p_this->state) {

    case __SIM_STATE_ATR_RECEIVING :
        if (p_this->checking_ts_timing == 1) {
            if ((tx_status & __SIM_REG_XMT_STATUS_GPCNT) &&
                !(rx_status & __SIM_REG_RCV_STATUS_RDRF)) {

                /* Disable the GPCNT timer and CWT timer right now */
                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
                reg_data &= ~(__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK |
                              __SIM_REG_CNTL_CWTEN);
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
                reg_data |= (__SIM_REG_INT_MASK_GPCM |
                             __SIM_REG_INT_MASK_CWTM |
                             __SIM_REG_INT_MASK_RIM);
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);
                p_this->error = AWBL_SIM_ERROR_ATR_DELAY;
                AW_SEMB_GIVE(p_this->semb);
                p_this->checking_ts_timing = 0;
            } else if (rx_status & __SIM_REG_RCV_STATUS_RDRF) {
                /*
                 * Reset/stop the GPCNT timer first.
                 */
                __sim_timer_reset(p_this);

                /*
                 * Enable GPC, CWT interrupt and disable the rx full interrupt
                 */
                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
                reg_data &= ~(__SIM_REG_INT_MASK_GPCM | __SIM_REG_INT_MASK_CWTM);
                reg_data |= __SIM_REG_INT_MASK_RIM;
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

                __sim_rcv_fifo_read(p_this);

                /* Clear the GPCNT expiring status */
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_XMT_STATUS, __SIM_REG_XMT_STATUS_GPCNT);

                /* ATR each recieved byte will cost 12 ETU, so get the remaining etus */
                reg_data = __SIM_ATR_MAX_DURATION - p_this->rcv_count * 12;
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_GPCNT, reg_data);

                __sim_timer_start(p_this, __SIM_REG_CNTL_GPCNT_ETU_CLK);

                /* receive fifo threshold set to max value */
//                reg_data = __SIM_REG_RCV_THRESHOLD_RTH(0) |
//                           __SIM_REG_RCV_THRESHOLD_RDT(__SIM_ATR_THRESHOLD_MAX);
//                __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_THRESHOLD, reg_data);
                p_this->checking_ts_timing = 0;

                p_this->td_also = AW_TRUE;
                p_this->td_x    = 0xFF;

                __sim_rx_int_enable(p_this);
            } else {
                AW_DBGF(("Unexpected irq when delay checking\n"));
            }
        } else {
            if ((rx_status & __SIM_REG_RCV_STATUS_CWT)    ||
                ((tx_status & __SIM_REG_XMT_STATUS_GPCNT) &&
                 (p_this->rcv_count != 0))) {

                /* disable the GPCNT timer and CWT timer right now */
                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
                reg_data &= ~(__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK | __SIM_REG_CNTL_CWTEN);
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
                reg_data |= (__SIM_REG_INT_MASK_GPCM | __SIM_REG_INT_MASK_CWTM);
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

                if (tx_status & __SIM_REG_XMT_STATUS_GPCNT) {
                    p_this->error |= AWBL_SIM_ERROR_ATR_TIMEROUT;
                }

                if (rx_status & __SIM_REG_RCV_STATUS_CWT) {
                    p_this->error |= AWBL_SIM_ERROR_CWT;
                }

                __sim_rcv_fifo_read(p_this);
                p_this->state = __SIM_STATE_ATR_RECEIVED;
                AW_SEMB_GIVE(p_this->semb);
            } else if (rx_status & __SIM_REG_RCV_STATUS_RDRF) {

                p_this->th = 0;
                __sim_rcv_fifo_read(p_this);

                if (p_this->td_also == AW_TRUE) {
                    p_this->td = p_devinfo->p_rcv_buffer[p_this->rcv_count - 1];
                    if (p_this->rcv_count == 2) {
                        /* T0 字节 */
                        p_this->history = p_this->td & 0x0F;
                    }
                    p_this->td & 0x10 ? p_this->th++ : (void)0;
                    p_this->td & 0x20 ? p_this->th++ : (void)0;
                    p_this->td & 0x40 ? p_this->th++ : (void)0;
//                    p_this->td & 0x80 ? p_this->th++ : (void)0;
//                    if (p_this->rcv_count == 1) {
//                        p_this->td_x = p_this->th + 1;
//                    }
//                    p_this->td_also = p_this->td & 0x80 ? AW_TRUE : AW_FALSE;
                    if (p_this->td & 0x80) {
                        p_this->th++;
                        if (p_this->rcv_count == 2) {
                            /* TD1 定义协议类型 */
                            p_this->td_x = p_this->th + 1;
                        }
                    } else {
                        p_this->td_also = AW_FALSE;
                    }

                    if (p_this->rcv_count == p_this->td_x + 1) {
                        p_this->serv.protocol_type = p_devinfo->p_rcv_buffer[p_this->td_x] & 0x0F;
                        if (p_this->serv.protocol_type == AWBL_SIM_PROTOCOL_T1 &&
                            (p_devinfo->p_rcv_buffer[1] & 0x80)) {
                            p_this->history++;
                        }
                    }

                    /* receive fifo threshold reset  */
                    reg_data = __SIM_REG_RCV_THRESHOLD_RTH(0) |
                               __SIM_REG_RCV_THRESHOLD_RDT(p_this->th);
                    __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_THRESHOLD, reg_data);
                } else {
                    if (p_this->history > 0) {
                        /* receive fifo threshold reset  */
                        reg_data = __SIM_REG_RCV_THRESHOLD_RTH(0) |
                                   __SIM_REG_RCV_THRESHOLD_RDT(p_this->history);
                        __SIM_REG_WRITE(p_devinfo, __SIM_REG_RCV_THRESHOLD, reg_data);
                        p_this->history = 0;
                    } else {

                        /* disable the GPCNT timer and CWT timer right now */
                        reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_CNTL);
                        reg_data &= ~(__SIM_REG_CNTL_GPCNT_CLK_SEL_MASK | __SIM_REG_CNTL_CWTEN);
                        __SIM_REG_WRITE(p_devinfo, __SIM_REG_CNTL, reg_data);

                        reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
                        reg_data |= (__SIM_REG_INT_MASK_GPCM | __SIM_REG_INT_MASK_CWTM | __SIM_REG_INT_MASK_RIM);
                        __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);

                        p_this->state = __SIM_STATE_ATR_RECEIVED;
                        AW_SEMB_GIVE(p_this->semb);
                    }
                }
                __sim_rx_int_enable(p_this);
            }
        }
        break;

    case __SIM_STATE_XMTING :

        /* The CWT BWT expire should not happen when in the transmitting state */
        if (tx_status & __SIM_REG_XMT_STATUS_ETC) {
            /* Once the transmit frame is completed, need to enable CWT timer */
            __sim_cwt_set(p_this, AW_TRUE);
        }

        if (tx_status & __SIM_REG_XMT_STATUS_XTE) {

            /* Disable TX */
            __sim_tx_set(p_this, AW_FALSE);

            /* Disalbe the timers */
            __sim_cwt_set(p_this, AW_FALSE);
            __sim_bwt_set(p_this, AW_FALSE);

            /* Disable the NACK interruptand TX related interrupt */
            //__sim_tx_int_disable(p_this);

            /* Update the state and status */
            p_this->error |= AWBL_SIM_ERROR_NACK_THRESHOLD;
            p_this->state = __SIM_STATE_XMT_ERROR;

            AW_SEMB_GIVE(p_this->semb);
        } else if ((tx_status & __SIM_REG_XMT_STATUS_TDTF) && p_this->xmt_remaining != 0) {

            __sim_xmt_fifo_fill(p_this);
            if (p_this->xmt_remaining == 0) {

                /* Disable TX threshold interrupt and enable tx complete interrupt */
                reg_data = __SIM_REG_READ(p_devinfo, __SIM_REG_INT_MASK);
                reg_data |= __SIM_REG_INT_MASK_TDTFM;

                /* Enable transmit complete and early transmit complete interrupt */
                reg_data &= ~(__SIM_REG_INT_MASK_TCIM | __SIM_REG_INT_MASK_ETCIM);
                __SIM_REG_WRITE(p_devinfo, __SIM_REG_INT_MASK, reg_data);
            } else {
                __sim_tx_int_enable(p_this);
            }
        } else if ((tx_status & __SIM_REG_XMT_STATUS_TC) && p_this->xmt_remaining == 0) {

            /* Disable the NACK interruptand TX related interrupt */
            //__sim_tx_int_disable(p_this);
            __sim_rx_set(p_this, AW_TRUE);

            /* Update the state and status */
            p_this->state = __SIM_STATE_XMT_DONE;
            AW_SEMB_GIVE(p_this->semb);
        }
        break;

    case __SIM_STATE_RECEIVING :

        /*
         * It takes some time to change from SIM_STATE_XMT_DONE to SIM_STATE_RECEIVING
         * RX would only be enabled after state becomes SIM_STATE_RECEIVING
         */
        if (rx_status & __SIM_REG_RCV_STATUS_RTE) {

            /* Disable RX */
            __sim_rx_set(p_this, AW_FALSE);

            /* Disable the BWT timer and CWT timer right now */
            __sim_cwt_set(p_this, AW_FALSE);
            __sim_bwt_set(p_this, AW_FALSE);

            /* Disable the interrupt right now */
            //__sim_rx_int_disable(p_this);

            /* Should we read the fifo or just flush the fifo? */
            __sim_rcv_fifo_read(p_this);
            p_this->error = AWBL_SIM_ERROR_NACK_THRESHOLD;
            p_this->state = __SIM_STATE_RECEIVE_ERROR;
            AW_SEMB_GIVE(p_this->semb);
            break;
        }

        if (rx_status & __SIM_REG_RCV_STATUS_RDRF) {
            __sim_rcv_fifo_read(p_this);
            if (p_this->is_fixed_len_rec &&
                p_this->rcv_count >= p_this->expected_rcv_cnt) {

                /* Disable the BWT timer and CWT timer right now */
                //__sim_rx_int_disable(p_this);

                /*
                 * Add the state judgement to ensure the maybe complete
                 * has been impletment in the above "if" case
                 */
                if (p_this->state == __SIM_STATE_RECEIVING) {
                    p_this->state = __SIM_STATE_RECEIVE_DONE;
                    AW_SEMB_GIVE(p_this->semb);
                    break;
                }
            }
        }

        if ((rx_status & __SIM_REG_RCV_STATUS_CWT) ||
            (rx_status & __SIM_REG_RCV_STATUS_BWT) ||
            (rx_status & __SIM_REG_RCV_STATUS_BGT)) {

            /* Disable the BWT timer and CWT timer right now */
            __sim_cwt_set(p_this, AW_FALSE);
            __sim_bwt_set(p_this, AW_FALSE);
            __sim_rx_int_disable(p_this);

            if (rx_status & __SIM_REG_RCV_STATUS_BWT) {
                p_this->error |= AWBL_SIM_ERROR_BWT;
            }

            if (rx_status & __SIM_REG_RCV_STATUS_CWT) {
                p_this->error |= AWBL_SIM_ERROR_CWT;
            }

            if (rx_status & __SIM_REG_RCV_STATUS_BGT) {
                p_this->error |= AWBL_SIM_ERROR_BGT;
            }

            __sim_rcv_fifo_read(p_this);

            /*
             * Add the state judgement to ensure the maybe
             * complete has been impletment in the above "if" case
             */
            if (p_this->state == __SIM_STATE_RECEIVING) {
                p_this->state = __SIM_STATE_RECEIVE_DONE;
                AW_SEMB_GIVE(p_this->semb);
            }
        }
        break;

    case __SIM_STATE_RESET_SEQUENCY :
        if (tx_status & __SIM_REG_XMT_STATUS_GPCNT) {
            AW_SEMB_GIVE(p_this->semb);
        }
        break;

    default :
        if (p_this->rx_status & __SIM_REG_RCV_STATUS_RDRF) {
            AW_DBGF(("unexpected  status %d\n", p_this->state));
            __sim_rcv_fifo_read(p_this);
        }
        break;
    }
}

/**
 * \brief sim transfer task
 */
aw_local void __sim_trans_task_entry (void *p_arg)
{
    __SIM_DEV_DECL(p_this, p_arg);

    AW_FOREVER {
        AW_SEMB_TAKE(p_this->sim_sem_sync, AW_SEM_WAIT_FOREVER);
        __sim_sm_send_event(p_this);
    }
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sim_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sim_inst_init2 (awbl_dev_t *p_dev)
{
    __SIM_DEV_DECL(p_this, p_dev);
    __SIM_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    //__SIM_DEV_LOCK_INIT(p_this);
    aw_spinlock_isr_init(&p_this->dev_lock, 0);
    AW_SEMB_INIT(p_this->semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);


    AW_DBGF(("sim%d port%d clock:%lu\n",
            p_devinfo->servinfo.dev_id / 2 + 1,
            p_devinfo->servinfo.dev_id % 2
            imx6ul_get_clk_freq(p_devinfo->ref_clk_id)));

    if (p_devinfo->port_index == 1) {
        p_this->port_ctrl_reg   = __SIM_REG_PORT1_CNTL;
        p_this->port_detect_reg = __SIM_REG_PORT1_DETECT;
    } else {
        p_this->port_ctrl_reg   = __SIM_REG_PORT0_CNTL;
        p_this->port_detect_reg = __SIM_REG_PORT0_DETECT;
    }

    p_this->quirks = __SIM_QUIRK;

    /* connect and enable isr */
    aw_int_connect(p_devinfo->inum, (aw_pfuncvoid_t)__sim_isr, (void *)p_dev);
    aw_int_enable(p_devinfo->inum);

    /* initialize the stack binary semaphore */
    AW_SEMB_INIT(p_this->sim_sem_sync, 0, AW_SEM_Q_FIFO);

    /* 初始化任务sim_trans_task */
    AW_TASK_INIT(p_this->sim_trans_task_decl,       /* 任务实体 */
                 "sim_trans_task",                  /* 任务名字 */
                 p_devinfo->task_trans_prio,        /* 任务优先级 */
                 __IMX6UL_SIM_TASK_STACK_SIZE,      /* 任务堆栈大小 */
                 __sim_trans_task_entry,            /* 任务入口函数 */
                 (void *)p_this);                   /* 任务入口参数 */

    /* 启动任务sim_trans_task */
    AW_TASK_STARTUP(p_this->sim_trans_task_decl);

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sim_inst_connect (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_zlg600a_uartserv_get" handler
 */
aw_local aw_err_t __sim_simserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __SIM_DEV_DECL(p_this, p_dev);
    __SIM_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_sim_service *p_serv = NULL;

    /* get intcltr service instance */
    p_serv = &p_this->serv;

    /* initialize the sim service instance */
    p_serv->p_next     = NULL;
    p_serv->p_servinfo = &p_devinfo->servinfo;
    p_serv->p_servopts = &__g_sim_servopts;
    p_serv->p_cookie   = (void *)p_dev;

    AW_MUTEX_INIT(p_serv->devlock, AW_SEM_Q_PRIORITY);

    /* send back the service instance */
    *(struct awbl_sim_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief imx6ul sim driver register
 */
void awbl_imx6ul_sim_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_sim_drv_registration);
}

/* end of file */
