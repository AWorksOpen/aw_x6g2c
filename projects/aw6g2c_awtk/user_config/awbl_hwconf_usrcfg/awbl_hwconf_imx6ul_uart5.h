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
  IMX6UL UART  配置信息
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_UART5_H_
#define AWBL_HWCONF_IMX6UL_UART5_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "driver/serial/awbl_imx6ul_uart.h"

#ifdef AW_DEV_IMX6UL_UART5

/*
 * 注意: 在imx6ul中的硬件流控和标准的硬件流控引脚的输入输出急性, 如果两个芯片都是imx6ul则按照标准的接法没有问题
 *      如果是imx6ul和其他标准的芯片的硬件流控的接法参考|imx6ul-标准|
 *
 *      |标 准-标准|           |imx6ul-imx6ul|                 |imx6ul-标准|
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


/* imx6ul UART5 设备信息 */
aw_local aw_const awbl_imx6ul_uart_chan_param_t __g_imx6ul_uart5_devinfo = {
    INUM_UART5_IRQ,             // irq
    IMX6UL_UART5_BASE_ADDR,     // 寄存器基地址
    IMX6UL_CLK_UART5_SERIAL,    // 输入时钟id
    recv_buf5,
    128,
    115200,
    __imx6uL_uart5_rs485_dir,   // RS485方向控制函数
    __imx6ul_uart5_plat_init,   // 平台初始化函数
};

aw_local void __imx6ul_uart5_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_UART5_SERIAL);
    aw_clk_enable(IMX6UL_CLK_UART5_IPG);
    __imx6ul_uart5_pinmux_init();
}


/* imx6ul 内部 UART 设备实例内存静态分配 */
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
