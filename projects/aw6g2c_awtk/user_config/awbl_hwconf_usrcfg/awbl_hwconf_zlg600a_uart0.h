/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_ZLG600A_UART0_H
#define __AWBL_HWCONF_ZLG600A_UART0_H

#ifdef  AW_DEV_ZLG600A_UART_0

#include "imx6ul_gpio_pin.h"
#include "driver/rfid/awbl_zlg600a_uart.h"

#define __ZLG600A_UART_TX_BUF_SIZE   282

aw_local uint8_t __zlg600a_uart_tx_buf[__ZLG600A_UART_TX_BUF_SIZE];

/**
 * \addtogroup grp_aw_plfm_ext_zlg600a_uart_usrcfg
 * @{
 */

/* 平台相关初始化 */
aw_local void __zlg600a_uart_0_plfm_init (void)
{
#if 0
    /* 拉低复位引脚最多2ms，复位后等待模块10ms */
    aw_gpio_pin_cfg(GPIO3_4, AW_GPIO_OUTPUT);
    aw_gpio_set(GPIO3_4, 0);
    aw_mdelay(2);
    aw_gpio_set(GPIO3_4, 1);
    aw_mdelay(10);
#endif
}

/* ZLG600A UART 设备信息 */
aw_local aw_const awbl_zlg600a_uart_devinfo_t __g_zlg600a_uart_0_devinfo = {
    {
        0,
    },
    AWBL_ZLG600A_FRAME_FMT_OLD,
    AWBL_ZLG600A_BAUDRATE_19200,
    0xB2 >> 1,
    AWBL_ZLG600A_MODE_AUTO_CHECK,
    200,
    __zlg600a_uart_tx_buf,
    __ZLG600A_UART_TX_BUF_SIZE,
    __zlg600a_uart_0_plfm_init
};

/* ZLG600A UART 设备实例内存静态分配 */
aw_local struct awbl_zlg600a_uart_dev __g_zlg600a_uart_0_dev;

#define AWBL_HWCONF_ZLG600A_UART_0                \
    {                                             \
        AWBL_ZLG600A_UART_NAME,                   \
        COM1,                                     \
        AWBL_BUSID_PLB,                           \
        0,                                        \
        &__g_zlg600a_uart_0_dev.dev,              \
        &__g_zlg600a_uart_0_devinfo               \
    },

/** @} */

#else
#define AWBL_HWCONF_ZLG600A_UART_0

#endif  /* AW_DEV_IMX28_ZLG600A_UART_0 */

#endif /* __AWBL_HWCONF_ZLG600A_UART0_H */

/* end of file */
