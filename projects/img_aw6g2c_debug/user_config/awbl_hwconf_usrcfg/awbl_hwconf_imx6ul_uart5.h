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
  IMX6UL UART  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_UART5_H_
#define AWBL_HWCONF_IMX6UL_UART5_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART5

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

aw_local void __imx6ul_uart5_plat_init (void);

aw_local void __imx6ul_uart5_pinmux_init (void)
{
    int uart5_gpios[] = {GPIO30, GPIO31};
    if (aw_gpio_pin_request("uart5_gpios",
                             uart5_gpios,
                             AW_NELEMENTS(uart5_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO30, GPIO1_30_UART5_TX);
        aw_gpio_pin_cfg(GPIO31, GPIO1_31_UART5_RX);
    }
}

aw_local void __imx6uL_uart5_rs485_dir (uint8_t dir)
{
    aw_gpio_set(GPIO17, dir);
}

aw_local char recv_buf5[128];


/* imx6ul UART5 �豸��Ϣ */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart5_devinfo = {
    INUM_UART5_IRQ,             // irq
    IMX6UL_UART5_BASE_ADDR,     // �Ĵ�������ַ
    IMX6UL_CLK_UART5_SERIAL,    // ����ʱ��id
    recv_buf5,
    128,
    115200,
    __imx6uL_uart5_rs485_dir,   // RS485������ƺ���
    __imx6ul_uart5_plat_init,   // ƽ̨��ʼ������
};

aw_local void __imx6ul_uart5_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART5_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART5_IPG);
    __imx6ul_uart5_pinmux_init();
}


/* imx6ul �ڲ� UART �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_uart_dev __g_imx6ul_uart5_dev;

#define AWBL_HWCONF_IMX6UL_UART5    \
    {                               \
        AWBL_IMX6UL_UART_NAME,      \
        IMX6UL_UART5_COMID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_uart5_dev.dev,  \
        &__g_imx6ul_uart5_devinfo   \
    },
#else
#define AWBL_HWCONF_IMX6UL_UART5
#endif

#endif /* AWBL_HWCONF_IMX6UL_UART5_H_ */
