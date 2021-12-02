/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  IMX6UL ����UART  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_UART1_H_
#define AWBL_HWCONF_IMX6UL_UART1_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART1

/*
 * ע��: ��imx6ul�е�Ӳ�����غͱ�׼��Ӳ���������ŵ������������, �������оƬ����imx6ul���ձ�׼�Ľӷ�û������
 *      �����imx6ul��������׼��оƬ��Ӳ�����صĽӷ��ο�|imx6ul-��׼|
 *
 *      |�� ׼-��׼|           |imx6ul-imx6ul|                 |imx6ul-��׼|
 *
 *  RXD  \ /  TXD (OUTPUT)  RXD  \ /  TXD (OUTPUT)         RXD  \ / TXD (OUTPUT)
 *        /                       /                              /
 *  TXD  / \  RXD (INPUT)   TXD  / \  RXD (INPUT)          TXD  / \ RXD (INPUT)
 *
 *  RTS  \ /  RTS (OUTPUT)  RTS  \ /  RTS (INPUT)   (INPUT)RTS  --- RTS (OUTPUT)
 *        /                       /
 *  CTS  / \  CTS (INPUT)   CTS  / \  CTS (OUTPUT)  (OUTPUT)CTS --- CTS (INPUT)
 */

aw_local void __imx6ul_uart1_plat_init (void);

aw_local void __imx6ul_uart1_pinmux_init (void)
{

    int uart1_gpios[] = {GPIO16, GPIO17};
    if (aw_gpio_pin_request("uart1_gpios",
                             uart1_gpios,
                             AW_NELEMENTS(uart1_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO16, GPIO1_16_UART1_TX);
        aw_gpio_pin_cfg(GPIO17, GPIO1_17_UART1_RX);
    }
}

aw_local void __imx6uL_uart1_rs485_dir (uint8_t dir)
{
    aw_gpio_set(GPIO17, dir);
}


aw_local char recv_buf1[128];
/* imx6ul UART1 �豸��Ϣ */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart1_devinfo = {
    INUM_UART1_IRQ,             // irq
    IMX6UL_UART1_BASE_ADDR,     // �Ĵ�������ַ
    IMX6UL_CLK_UART1_SERIAL,    // ����ʱ��id
    recv_buf1,
    128,
    115200,
    NULL,   // RS485������ƺ���
    __imx6ul_uart1_plat_init,   // ƽ̨��ʼ������

};

aw_local void __imx6ul_uart1_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART1_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART1_IPG);
    __imx6ul_uart1_pinmux_init();
}


/* imx6ul�ڲ�UART�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_uart_dev __g_imx6ul_uart1_dev;

#define AWBL_HWCONF_IMX6UL_UART1    \
    {                               \
        AWBL_IMX6UL_UART_NAME,      \
        IMX6UL_UART1_COMID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_uart1_dev.dev,  \
        &__g_imx6ul_uart1_devinfo   \
    },
#else
#define AWBL_HWCONF_IMX6UL_UART1
#endif

#endif /* AWBL_HWCONF_IMX6UL_OCOTP_H_ */
