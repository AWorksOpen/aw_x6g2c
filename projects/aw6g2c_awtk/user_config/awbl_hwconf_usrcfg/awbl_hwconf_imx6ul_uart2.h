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

#ifndef AWBL_HWCONF_IMX6UL_UART2_H_
#define AWBL_HWCONF_IMX6UL_UART2_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART2

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

aw_local void __imx6ul_uart2_plat_init (void);

aw_local void __imx6ul_uart2_pinmux_init (void)
{

    int uart2_gpios[] = {GPIO20, GPIO21, GPIO22, GPIO23};
    if (aw_gpio_pin_request("uart2_gpios",
                             uart2_gpios,
                             AW_NELEMENTS(uart2_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(GPIO20, GPIO1_20_UART2_TX);
        aw_gpio_pin_cfg(GPIO21, GPIO1_21_UART2_RX);
        aw_gpio_pin_cfg(GPIO22, GPIO1_22_UART2_CTS_B);
        aw_gpio_pin_cfg(GPIO23, GPIO1_23_UART2_RTS_B);
//        aw_gpio_pin_cfg(GPIO5_6, AW_GPIO_OUTPUT_INIT_LOW);
    }
}

aw_local void __imx6uL_uart2_rs485_dir (uint8_t dir)
{
    aw_gpio_set(GPIO5_6, dir);
}


aw_local char recv_buf2[128];

/* imx6ul UART2 �豸��Ϣ */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart2_devinfo = {
    INUM_UART2_IRQ,             // irq
    IMX6UL_UART2_BASE_ADDR,     // �Ĵ�������ַ
    IMX6UL_CLK_UART2_SERIAL,    // ����ʱ��id
    recv_buf2,
    128,
    115200,
    __imx6uL_uart2_rs485_dir,   // RS485������ƺ���
    __imx6ul_uart2_plat_init,   // ƽ̨��ʼ������
};

aw_local void __imx6ul_uart2_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART2_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART2_IPG);
    __imx6ul_uart2_pinmux_init();
}

/* imx6ul �ڲ� UART �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_uart_dev __g_imx6ul_uart2_dev;

#define AWBL_HWCONF_IMX6UL_UART2    \
    {                               \
        AWBL_IMX6UL_UART_NAME,      \
        IMX6UL_UART2_COMID,         \
        AWBL_BUSID_PLB,             \
        0,                          \
        &__g_imx6ul_uart2_dev.dev,  \
        &__g_imx6ul_uart2_devinfo   \
    },
#else
#define AWBL_HWCONF_IMX6UL_UART2
#endif

#endif /* AWBL_HWCONF_IMX6UL_OCOTP_H_ */
