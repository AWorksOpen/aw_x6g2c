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

#ifndef AWBL_HWCONF_IMX6UL_UART3_H_
#define AWBL_HWCONF_IMX6UL_UART3_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART3

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

aw_local void __imx6ul_uart3_plat_init (void);

aw_local void __imx6ul_uart3_pinmux_init (void)
{
    int uart3_gpios[] = {GPIO24, GPIO25, GPIO3_21};
    if (aw_gpio_pin_request("uart3_gpios",
                             uart3_gpios,
                             AW_NELEMENTS(uart3_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO24, GPIO1_24_UART3_TX);
        aw_gpio_pin_cfg(GPIO25, GPIO1_25_UART3_RX);
        aw_gpio_pin_cfg(GPIO3_21, AW_GPIO_OUTPUT);
    }
}

aw_local void __imx6uL_uart3_rs485_dir (uint8_t dir)
{
    aw_gpio_set(GPIO3_21, dir);
}

aw_local char recv_buf3[128];

/* imx6ul UART3 �豸��Ϣ */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart3_devinfo = {
    INUM_UART3_IRQ,             // irq
    IMX6UL_UART3_BASE_ADDR,     // �Ĵ�������ַ
    IMX6UL_CLK_UART3_SERIAL,    // ����ʱ��id
    recv_buf3,
    128,
    115200,
    __imx6uL_uart3_rs485_dir,   // RS485������ƺ���
    __imx6ul_uart3_plat_init,   // ƽ̨��ʼ������
};

aw_local void __imx6ul_uart3_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART3_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART3_IPG);
    __imx6ul_uart3_pinmux_init();
}

/* imx6ul �ڲ� UART �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_uart_dev __g_imx6ul_uart3_dev;

#define AWBL_HWCONF_IMX6UL_UART3    \
    {                               \
        AWBL_IMX6UL_UART_NAME,      \
        IMX6UL_UART3_COMID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_uart3_dev.dev,  \
        &__g_imx6ul_uart3_devinfo   \
    },
#else
#define AWBL_HWCONF_IMX6UL_UART3
#endif

#endif /* AWBL_HWCONF_IMX6UL_OCOTP_H_ */
