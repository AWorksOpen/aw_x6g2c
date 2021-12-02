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
  IMX6UL UART4  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_UART4_H_
#define AWBL_HWCONF_IMX6UL_UART4_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART4

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

aw_local void __imx6ul_uart4_plat_init (void);

aw_local void __imx6ul_uart4_pinmux_init (void)
{
    int uart4_gpios[] = {GPIO28, GPIO29, GPIO5_6};
    if (aw_gpio_pin_request("uart4_gpios",
                             uart4_gpios,
                             AW_NELEMENTS(uart4_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO28, GPIO1_28_UART4_TX);
        aw_gpio_pin_cfg(GPIO29, GPIO1_29_UART4_RX);
        aw_gpio_pin_cfg(GPIO5_6, AW_GPIO_OUTPUT_INIT_LOW);
    }
}

aw_local void __imx6uL_uart4_rs485_dir (uint8_t dir)
{
    aw_gpio_set(GPIO5_6, dir);
}


aw_local char recv_buf4[128];


/* imx6ul UART4 �豸��Ϣ */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart4_devinfo = {
    INUM_UART4_IRQ,             // irq
    IMX6UL_UART4_BASE_ADDR,     // �Ĵ�������ַ
    IMX6UL_CLK_UART4_SERIAL,    // ����ʱ��id
    recv_buf4,
    128,
    115200,
    __imx6uL_uart4_rs485_dir,   // RS485������ƺ���
    __imx6ul_uart4_plat_init,   // ƽ̨��ʼ������
};

aw_local void __imx6ul_uart4_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART4_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART4_IPG);
    __imx6ul_uart4_pinmux_init();
}

/* imx6ul �ڲ� UART�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_uart_dev __g_imx6ul_uart4_dev;

#define AWBL_HWCONF_IMX6UL_UART4    \
    {                               \
        AWBL_IMX6UL_UART_NAME,      \
        IMX6UL_UART4_COMID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_uart4_dev.dev,  \
        &__g_imx6ul_uart4_devinfo   \
    },
#else
#define AWBL_HWCONF_IMX6UL_UART4
#endif

#endif /* AWBL_HWCONF_IMX6UL_UART4_H_ */
