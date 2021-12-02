/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
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
 * \brief imx6ul uart driver
 *
 * this driver is the implementation of aw_serial.h for imx6ul
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-12-21, jac, first implementation.
 * - 1.01 2016-9-9,   sni, alloc dma buffer for tx
 * \endinternal
 */


#include "apollo.h"
#include "awbus_lite.h"
#include <string.h>
#include "aw_common.h"
#include "aw_int.h"
#include "awbl_sio.h"
#include "aw_bitops.h"
#include "aw_clk.h"
#include "driver/serial/awbl_imx6ul_uart.h"
#include "aw_serial.h"


/**
 * \addtogroup grp_imx6ul_bsp IMX6UL BSP֧��
 * @{
 */

/**
 * \addtogroup grp_awbus_lite_driver AWbus lite����
 * @{
 */

/**
 * \addtogroup grp_awbl_drv_imx6ul_uart IMX6UL UART����
 * \copydetails awbl_imx6ul_uart.h
 * @{
 */

 
/* min/max baud rate */
#define __IMX6UL_UART_MIN_RATE              300
#define __IMX6UL_UART_MAX_RATE              5000000

#define __IMX6UL_RS485_TX_MODE              0x01
#define __IMX6UL_RS485_RX_MODE              0x00

#define CONFIG_UART_TXTL                    16
#define CONFIG_UART_RXTL                    16


#define __UART_DEVINFO_GET(p_dev) \
        ((awbl_imx6ul_uart_chan_param_t *)AWBL_DEVINFO_GET(p_dev))

/* declare mm32f103 uart device instance */
#define __UART_DEV_DECL(p_this, p_dev) \
            struct awbl_imx6ul_uart_dev *p_this = \
                   (struct awbl_imx6ul_uart_dev *)(p_dev)

#define __UART_DEVINFO_DECL(p_param, p_dev) \
            awbl_imx6ul_uart_chan_param_t *p_param = \
                   ((awbl_imx6ul_uart_chan_param_t *)AWBL_DEVINFO_GET(p_dev))

/** \brief ���Ͳ����� */
static const uint32_t __g_typical_baudrate[] = {
    110,
    300,
    600,
    1200,
    2400,
    4800,
    9600,
    14400,
    19200,
    38400,
    43000,
    56000,
    57600,
    76800,
    115200,
    128000,
    230400,
    256000,
    460800,
    921600,
    1382400
};

static const int __g_num_typical_baudrate = AW_NELEMENTS(__g_typical_baudrate);

aw_local void __imx6ul_uart_disable(void *p_dev);
aw_local void __imx6ul_uart_enable(void *p_dev);


/**
 * \addtogroup grp_awbl_drv_imx6ul_uart_internal_function uart�ڲ�ʵ��
 * @{
 */
/**
 * \brief �������Լ��
 *
 * \param m Ҫ�������Լ��������֮һ
 * \param m Ҫ�������Լ��������֮��
 *
 */
static uint32_t gcd(uint32_t m, uint32_t n)
{
    uint32_t        temp;

    if(m < n) {
        temp = n;
        n    = m;
        m    = temp;
    }
    while(0 != n) {
        temp = m % n;
        m    = n;
        n    = temp;
    }

    return m;
}


/**
 * \brief change baud rate for channel
 *
 * This routine sets the baud rate for the UART. The interrupts are disabled
 * during chip access.
 *
 * \return AW_OK
 */
aw_local aw_err_t __imx6ul_uart_baud_set (void      *p_dev,
                                          uint32_t   baud)
{
    uint32_t                         ref_freq, t;
    uint32_t                         ubmr, ubir;
    imx6ul_uart_regs_t              *p_uart_reg;
    __UART_DEVINFO_DECL(p_param, p_dev);

    p_uart_reg = (imx6ul_uart_regs_t *)p_param->base_addr;
    ref_freq   = aw_clk_rate_get(p_param->ref_clk_id);

    t = ref_freq / 16;

    if (t < baud) {
        /* Ƶ��С��Ҫ���ò�����  */
        return -AW_EINVAL;
    }

    if ((t == 0) || (baud == 0)) {
        return -AW_EINVAL;
    }

    ubir = gcd(t, baud);
    ubmr = t / ubir - 1;
    ubir = baud / ubir - 1;

    if ((ubmr > 0xffff) || (ubir > 0xffff)) {
        return -AW_EINVAL;
    }

    /* disable interrupts during chip access */
    AW_REG_WRITE32(&p_uart_reg->UBIR, ubir);
    AW_REG_WRITE32(&p_uart_reg->UBMR, ubmr);

    return (AW_OK);
}


/**
 * \brief deferred handle a receiver interrupt
 *
 * This routine handles read interrupts from the UART.
 */
aw_local void __imx6ul_uart_int_rd (void *p_dev)
{
    char                             inchar;
    uint32_t                         data;
    imx6ul_uart_regs_t              *p_uart_reg;
    __UART_DEV_DECL(p_this, p_dev);
    __UART_DEVINFO_DECL(p_param, p_dev);


    p_uart_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    while (!AW_REG_BIT_ISSET32(&p_uart_reg->UTS, 5) ) {
        data = AW_REG_READ32(&p_uart_reg->URXD);

        /* ���Լ��  */
        if (data & (1 << 10)) {

            /* У��ʧ�ܶ������� */
            return;
        }
        inchar = (char)data;
        awbl_serial_rx(&p_this->serial, inchar);
    }
}

/**
 * \brief interrupt level processing
 *
 * This routine handles four sources of interrupts from the UART. They are
 * prioritized in the following order by the Interrupt Identification Register:
 * Receiver Line Status, Received Data Ready, Transmit Holding Register Empty
 * and Modem Status.
 *
 * When a modem status interrupt occurs, the transmit interrupt is enabled if
 * the CTS signal is AW_TRUE.
 */
aw_local void __imx6ul_uart_int (void *p_dev)
{
    uint32_t                         int_status;
    imx6ul_uart_regs_t              *p_uart_reg;
    __UART_DEV_DECL(p_this, p_dev);
    __UART_DEVINFO_DECL(p_param, p_dev);

    p_uart_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    /* read the Interrupt Status Register (Int. Ident.) */
    int_status = AW_REG_READ32(&p_uart_reg->USR1);

    if (AW_BIT_ISSET(int_status, 9) ||
            AW_BIT_ISSET(int_status, 8) ) {     /* transmitter holding register ready */
        __imx6ul_uart_int_rd(p_dev);
        awbl_serial_int_idle (&p_this->serial);
    }

    if (AW_BIT_ISSET(int_status, 13)) {     /* transmitter holding register ready */
        /* �ر��ж�  */
        AW_REG_BIT_CLR32(&p_uart_reg->UCR1, 13);
        AW_SEMB_GIVE(p_this->w_semb);
    }

    /* �������,��FIFOΪ��,������λ�Ĵ���Ϊ��  */
    if (AW_REG_BIT_ISSET32(&p_uart_reg->UCR4, 3) &&
        AW_REG_BIT_ISSET32(&p_uart_reg->USR2, 3)) {

        AW_SEMB_GIVE(p_this->w_485semb);

        /* disable the interrupt */
        AW_REG_BIT_CLR32(&p_uart_reg->UCR4, 3);
    }

    AW_REG_WRITE32(&(p_uart_reg->USR1), int_status);

}


/**
 * \brief transmitter routine
 *
 *
 * Call interrupt level character output routine and enable interrupt if it is
 * in interrupt mode with no hardware flow control.
 * If the option for hardware flow control is enabled and CTS is set AW_TRUE,
 * then the Tx interrupt is enabled.
 *
 * \return AW_OK, or -AW_ENOSYS if in polled mode
 */
aw_local aw_err_t __imx6ul_uart_tx (struct aw_sio_chan *p_siochan,
                                    const char *p_buffer,
                                    size_t nbytes)
{
    struct awbl_serial *p_serial;
    awbl_imx6ul_uart_chan_param_t        *p_param;
    imx6ul_uart_regs_t                   *p_uart_reg;
    struct awbl_imx6ul_uart_dev          *p_this = NULL;
    uint32_t                              index  = 0;
    size_t                                len    = nbytes;

    p_serial = AW_CONTAINER_OF(p_siochan,
                               struct awbl_serial,
                                sio_chan);
    p_this   = AW_CONTAINER_OF(p_serial,
                struct awbl_imx6ul_uart_dev,
                serial);

    /* get the channel parameter */
    p_param    = __UART_DEVINFO_GET(p_this);

    p_uart_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    /* rs485 ����  */
    if (p_param->pfn_rs485_dir) {
        p_param->pfn_rs485_dir(__IMX6UL_RS485_TX_MODE);
    }

    while (len) {

        if (!AW_REG_BIT_ISSET32(&(p_uart_reg->UTS), 4)) {

            AW_REG_WRITE32(&p_uart_reg->UTXD, p_buffer[index++]);
            len--;

        } else {

            /* �򿪷����ж� */
            AW_REG_BIT_SET32(&p_uart_reg->UCR1, 13);

            /* ���ź���֤������FIFO���пռ�  */
            AW_SEMB_TAKE(p_this->w_semb, AW_WAIT_FOREVER);
        }
    }

    /* ����485  */
    if (p_param->pfn_rs485_dir) {

        /* ���ڲ��ǹرյ�nvic���ж�,���Կ��ܳ��ֶഥ��һ���жϵ����,�����ڴ�����ж�֮ǰ�Ƚ��ź��������� */
        AW_SEMB_RESET(p_this->w_485semb, 0);

        /* �򿪷�������ж�, ֻҪ�򿪷�������ж�,��ʹFIFO��û������Ҳ�ᴥ���ж�  */
        AW_REG_BIT_SET32(&p_uart_reg->UCR4, 3);

        /* �ȴ�485 �������   */
        AW_SEMB_TAKE(p_this->w_485semb, AW_WAIT_FOREVER);
        p_param->pfn_rs485_dir(__IMX6UL_RS485_RX_MODE);
    }

    return nbytes;
}


/**
 * \brief output a character in polled mode
 *
 * This routine transmits a character in polled mode.
 *
 * \return AW_OK if a character sent, -EAGAIN if the output buffer is full
 */
aw_local int __imx6ul_uart_poll_output (struct aw_sio_chan *p_siochan,
                                        char outchar)
{
    awbl_imx6ul_uart_chan_param_t   *p_param;
    imx6ul_uart_regs_t              *p_uart_reg;

    struct awbl_serial *p_serial;
    struct awbl_imx6ul_uart_dev *p_this;

    p_serial = AW_CONTAINER_OF(p_siochan,
                               struct awbl_serial,
                                sio_chan);
    p_this   = AW_CONTAINER_OF(p_serial,
                struct awbl_imx6ul_uart_dev,
                serial);

    /* get the channel parameter */
    p_param = __UART_DEVINFO_GET(p_this);

    p_uart_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    /* is the transmitter ready to accept a character? */
    while(AW_REG_BIT_ISSET32(&p_uart_reg->UTS, 4));

    /* transmit */
    AW_REG_WRITE32(&(p_uart_reg->UTXD), outchar);

    return (AW_OK);
}


aw_local int __dcd_param_check(const struct aw_serial_dcb *p_dcb)
{
    int      i;

    /* ������ò���, ֻ֧��7,8λ��*/
    if ((p_dcb->byte_size < 7) || (p_dcb->byte_size > 8)) {
        return -AW_EINVAL;          /* ��Ч���� */
    }

    /* ��鲨����  */
    if ((p_dcb->baud_rate >= __g_typical_baudrate[0]) &&
        (p_dcb->baud_rate <= __g_typical_baudrate[__g_num_typical_baudrate - 1])) {
        for (i = 0; i < __g_num_typical_baudrate; i++) {
            if (p_dcb->baud_rate == __g_typical_baudrate[i]) {
                break;
            }
        }

        /* �ڱ�׼��Χ, �Ǳ�׼������  */
        if (i >= __g_num_typical_baudrate) {

            /* ������׼��Χ�Ĳ�����  */
            return -AW_EINVAL;      /* ��Ч���� */
        }
    } else if (p_dcb->baud_rate < __g_typical_baudrate[0]) {
        /* ���ڱ�׼��Χ  */
        return -AW_EINVAL;          /* ��Ч���� */
    } else {
        /* ���ڱ�׼�����ʿ��������ķ�Ƶ */
    }

    /* �ж��Ƿ��д����ֹͣλ  */
    if (p_dcb->stop_bits % 4 == AW_SERIAL_STOP_INVALID) {
        return -AW_EINVAL;          /* ��Ч���� */
    }

    /* ��֧��1.5 λֹͣλ   */
    if (p_dcb->stop_bits == AW_SERIAL_ONE5STOPTS) {
        return -AW_ENOTSUP;
    }

    /* ���RTS �Ƿ�����Чλ  */
    if (p_dcb->f_rtsctrl % 4 == AW_SERIAL_RTS_INVALID) {
        return -AW_EINVAL;          /* ��Ч���� */
    }

    /* ���DTR */
    if (p_dcb->f_dtrctrl % 4 == AW_SERIAL_DTR_INVALID) {
        return -AW_EINVAL;          /* ��Ч���� */
    }

    /* ���������� */
    if ((p_dcb->f_outx) || (p_dcb->f_inx)) {
        return -AW_ENOTSUP;    /* ��֧��������� */
    }

    if (p_dcb->f_dsrsensitivity) {
        return -AW_ENOTSUP;    /* ��֧��dsrsensitivity */
    }

    if (p_dcb->f_dsrflow) {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

aw_local void __imx6ul_uart_reset(void *p_dev)
{
    __UART_DEVINFO_DECL(p_param, p_dev);

    imx6ul_uart_regs_t  *p_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    AW_REG_BIT_CLR32(&p_reg->UCR2, 0);

    while(AW_REG_BIT_ISSET32(&p_reg->UTS, 0)) {

    }
}


aw_local int __imx6ul_uart_dcb_set (struct aw_sio_chan         *p_sio_chan,
                                    const struct aw_serial_dcb *p_dcb)
{
    aw_err_t err = AW_OK;
    struct awbl_imx6ul_uart_chan_param  *p_this_dvif;
    imx6ul_uart_regs_t                  *p_reg;
    uint32_t                             lcr;
    uint32_t                             ref_freq = 0;

    struct awbl_serial *p_serial;
    struct awbl_imx6ul_uart_dev *p_this;

    p_serial = AW_CONTAINER_OF(p_sio_chan,
                               struct awbl_serial,
                               sio_chan);
    p_this   = AW_CONTAINER_OF(p_serial,
                struct awbl_imx6ul_uart_dev,
                serial);


    p_this_dvif = __UART_DEVINFO_GET(p_this);
    p_reg       = (imx6ul_uart_regs_t *)p_this_dvif->base_addr;

    /* dcb�������  */
    err = __dcd_param_check(p_dcb);

    if (err) {
        return err;
    }

    __imx6ul_uart_disable(p_this);

    __imx6ul_uart_reset(p_this);

    err = __imx6ul_uart_baud_set(p_this, p_dcb->baud_rate);

    if (err) {
        return err;
    }

    lcr = AW_REG_READ32(&p_reg->UCR2);

    /* ����λ���� */
    if (p_dcb->byte_size == 7) {
        AW_BIT_CLR(lcr, 5);
    } else {
        AW_BIT_SET(lcr, 5);
    }

    /* ��ż�������� */
    if (p_dcb->f_parity) {  /* ʹ����żУ�� */

        AW_BIT_SET(lcr, 8);
        if (p_dcb->parity == AW_SERIAL_EVENPARITY) { /* żУ�� */
            AW_BIT_CLR(lcr, 7);
        } else {
            AW_BIT_SET(lcr, 7);
        }
    } else {                /* �޼��� */
        AW_BIT_CLR(lcr, 8);
    }

    /* ֹͣλ���� */
    if (p_dcb->stop_bits == AW_SERIAL_ONESTOPBIT) {
        AW_BIT_CLR(lcr, 6);
    } else {
        AW_BIT_SET(lcr, 6);
    }


    /* ����CTS */
    if (p_dcb->f_ctsflow == AW_TRUE) {
        AW_BIT_SET(lcr, 13);
    } else {
        AW_BIT_CLR(lcr, 13);
    }

    /* ����RTS,��׼��RTS�����CTS������,������Imx6ul��ȴ���෴�� */
    if (p_dcb->f_rtsctrl == AW_SERIAL_RTS_HANDSHAKE) {
        AW_BIT_CLR(lcr, 14);
    } else {
        /* ����RTS */
        AW_BIT_SET(lcr, 14);

        /* �������CTS��������  */
        if (p_dcb->f_rtsctrl == AW_SERIAL_RTS_ENABLE) {
            /* ʹ��CTS�� */
            AW_BIT_SET(lcr, 12);
        } else {
            /* ��ЧCTS�� */
            AW_BIT_CLR(lcr, 12);
        }
    }

    AW_REG_WRITE32(&p_reg->UCR2, lcr);

    /* Reference Frequency DividerΪ1 */
    AW_REG_BITS_SET32(&p_reg->UFCR, 7, 3, 5);

    /* ����FIFO�жϴ����� */
    AW_REG_BITS_SET32(&p_reg->UFCR, 10, 6, CONFIG_UART_TXTL);

    if (p_dcb->f_fast_reaction) {
        /* ������Ӧ����, FIFO�жϴ����� ����Ϊ1, һ��Ӧ����modbus*/
        AW_REG_BITS_SET32(&p_reg->UFCR, 0, 6, 1);
    } else {
        /* ����FIFO�жϴ����� */
        AW_REG_BITS_SET32(&p_reg->UFCR, 0, 6, CONFIG_UART_RXTL);
    }


    /* ����1ms���� */
    ref_freq = aw_clk_rate_get(p_this_dvif->ref_clk_id);
    AW_REG_BITS_SET32(&p_reg->ONEMS, 0, 24, ref_freq / 1000);

    /* �ֲ���˵��һλ�������� */
    AW_REG_BIT_SET32(&p_reg->UCR3, 2);

    /* ���ô���Ӳ������������FIFO��СΪ28�ֽ� */
    AW_REG_BITS_SET32(&p_reg->UCR4, 10, 6, 28);

    /* tx��rx enable */
    AW_REG_BIT_SET32(&p_reg->UCR2, 1);
    AW_REG_BIT_SET32(&p_reg->UCR2, 2);

     /* ���ܷ����ж�  */
    AW_REG_BIT_CLR32(&p_reg->UCR1, 13);

    /* clear RTS and __DTR bits */
    AW_REG_WRITE32(&p_reg->USR1, 1 << 12);
    AW_REG_WRITE32(&p_reg->USR2, 1 << 13);

    /* �򿪽����ж�  */
    AW_REG_BIT_SET32(&p_reg->UCR1, 9);
    /* aging, �����aging��ֻ�н��յ��� FIFO level ���ж� */
    AW_REG_BIT_SET32(&p_reg->UCR2, 3);

    memcpy(&p_serial->dcb, p_dcb, sizeof(*p_dcb));
    __imx6ul_uart_enable(p_this);

    return err;
}


aw_local int __imx6ul_uart_dcb_get (struct aw_sio_chan   *p_sio_chan,
                                    struct aw_serial_dcb *p_dcb)
{
    struct awbl_serial *p_serial = AW_CONTAINER_OF(p_sio_chan,
                                                   struct awbl_serial,
                                                   sio_chan);

    memcpy(p_dcb, &p_serial->dcb, sizeof(p_serial->dcb));

    return AW_OK;
}


/*
 * �������������������
 */
aw_local int __imx6ul_recv_decrease (struct aw_sio_chan *p_siochan, const int freebytes)
{
    return AW_OK;
}

/** \brief sio driver functions */
aw_local aw_const struct aw_sio_drv_funcs __g_imx6ul_uart_sio_drv_funcs = {
    __imx6ul_uart_tx,
    __imx6ul_uart_poll_output,
    __imx6ul_uart_dcb_set,
    __imx6ul_uart_dcb_get,
    __imx6ul_recv_decrease,
};

/** @} grp_awbl_drv_imx6ul_uart_awbus_interface */

/**
 * \addtogroup grp_awbl_drv_imx6ul_uart_awbus_interface Awbus�ӿ�
 * @{
 */


/**
 * \brief ���ܴ���
 *
 */
aw_local void __imx6ul_uart_disable(void *p_dev)
{
    imx6ul_uart_regs_t                     *p_reg;
    __UART_DEVINFO_DECL(p_param, p_dev);

    p_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    /* �ȴ�Ӳ���������  */
    while(!AW_REG_BIT_ISSET32(&p_reg->USR2, 3));

    /* uart disable  */
    do {
        AW_REG_BIT_CLR32(&p_reg->UCR1, 0);
    } while (AW_REG_BIT_ISSET32(&p_reg->UCR1, 0));
}


/**
 * \brief ʹ�ܴ���
 */
aw_local void __imx6ul_uart_enable(void *p_dev)
{
    imx6ul_uart_regs_t                     *p_reg;
    __UART_DEVINFO_DECL(p_param, p_dev);

    p_reg = (imx6ul_uart_regs_t *)p_param->base_addr;

    /* uart enable  */
    do {
        AW_REG_BIT_SET32(&p_reg->UCR1, 0);
    } while (!AW_REG_BIT_ISSET32(&p_reg->UCR1, 0));

}


aw_local const struct aw_serial_dcb __init_dcb = {

        .byte_size         = 8,
        .f_parity          = AW_FALSE,
        .parity            = AW_SERIAL_EVENPARITY,
        .stop_bits         = AW_SERIAL_ONESTOPBIT,
        .f_ctsflow         = AW_FALSE,
        .f_rtsctrl         = AW_SERIAL_RTS_DISABLE,
        .f_dsrsensitivity  = AW_FALSE,
        .f_dsrflow         = AW_FALSE,
        .f_dtrctrl         = AW_SERIAL_DTR_DISABLE,
        .f_outx            = AW_FALSE,
        .f_inx             = AW_FALSE,
        .baud_rate         = 115200,
        .xon_lim           = 0,
        .xoff_lim          = 0,
        .xon_char          = 0x11,
        .xoff_char         = 0x13,
        .f_fast_reaction   = AW_FALSE,
};



/**
 * \brief uart������ʼ������1
 *
 * \param   p_dev   AWbus������������ǰ�豸
 * \return  ��
 */
aw_local void __imx6ul_uart_inst_init (awbl_dev_t *p_dev)
{
    struct aw_serial_dcb             dcb;
    int                              ret = AW_OK;
    struct aw_sio_chan    sio_chan;

    __UART_DEV_DECL(p_this, p_dev);
    __UART_DEVINFO_DECL(p_param, p_dev);

    /** �����ж� */
    if ((p_param->recv_buf == NULL)) {
        while (1);
    }

    /* Call platform initialize function first */
    if (NULL != p_param->pfunc_plat_init) {
        p_param->pfunc_plat_init();
    }

    /* initialize the driver function pointers in the channel */
    sio_chan.p_drv_funcs = &__g_imx6ul_uart_sio_drv_funcs;   /** to up user */

    AW_SEMB_INIT(p_this->w_semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
    AW_SEMB_INIT(p_this->w_485semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* ���������豸 */
    ret = awbl_serial_create(&p_this->serial,
                            p_param->recv_buf,
                            p_param->recv_size,
                            p_dev->p_devhcf->unit,
                            &sio_chan);

    if (ret != AW_OK) { /* �豸����ʧ��  */
        while (1);
    }

    if (p_param->pfn_rs485_dir != NULL) {
        /* set to receive mode */
        p_param->pfn_rs485_dir(__IMX6UL_RS485_RX_MODE);
    }

    dcb = __init_dcb;
    dcb.baud_rate = p_param->init_baudrate;

    ret = __imx6ul_uart_dcb_set(&p_this->serial.sio_chan, &dcb);
    if (ret != AW_OK) {
        while (1);
    }

    /* connect and enable interrupts */
    aw_int_connect(p_param->vector, (void (*)(void*))__imx6ul_uart_int, p_dev);
    aw_int_enable(p_param->vector);
}


/**
 * \brief initialize uart device
 *
 * This routine initializes the uart device.  We do not assume a single RS232
 * device for this physical device.  Instead, we assume or more devices.  Each
 * device uses a separate reg_base.  For each non-NULL reg_base, we allocate a
 * p_chan structure and allocate a unit number with that structure.  In
 * __imx28x_duart_inst_connect(), we attach the channel to the I/O system.
 * This bypasses other BSP-based configuration mechanisms.
 *
 * Instead of waiting for __imx28x_duart_inst_connect(), as would be done for typical
 * devices, we initialize the serial ports and connect them to the I/O system
 * here. This allows early console output.
 *
 * This routine is called later during system initialization.  OS features
 * such as memory allocation are available at this time.
 */
aw_local void __imx6ul_uart_inst_init2 (awbl_dev_t *p_dev)
{

}

/**
 * \brief connect uart device to system
 *
 * Nothing to do here.  We want serial channels available for output reasonably
 * early during the boot process.  Because this is a serial channel, we connect
 * to the I/O system in __imx6ul_uart_inst_init2() instead of here.
 */
aw_local void __imx6ul_uart_inst_connect (awbl_dev_t *p_dev)
{
}


aw_const struct awbl_drvfuncs __g_imx6ul_uart_awbl_drvfuncs = {
    __imx6ul_uart_inst_init,            /* devInstanceInit */
    __imx6ul_uart_inst_init2,           /* devInstanceInit2 */
    __imx6ul_uart_inst_connect          /* devConnect */
};


aw_local aw_const struct awbl_drvinfo __g_imx6ul_uart_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX6UL_UART_NAME,              /* p_drvname */
    &__g_imx6ul_uart_awbl_drvfuncs,     /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

void awbl_imx6ul_uart_drv_register (void)
{
    awbl_drv_register(&__g_imx6ul_uart_drv_registration);
}
/** @} grp_awbl_drv_imx6ul_uart_awbus_interface */

/* end of file */
