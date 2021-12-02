/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_GPIO_SPI0_H
#define __AWBL_HWCONF_GPIO_SPI0_H

#ifdef AW_DEV_GPIO_SPI

#include "aw_gpio.h"
#include "driver/busctlr/awbl_gpio_spi.h"
#include "imx6ul_gpio_pin.h"

/* 初始化 */
aw_local void __pfunc_plfm_init (void) {
    int gpio_spi0_gpios[] = {
            GPIO4_25,                     /* sck引脚号 */
            GPIO4_27,                     /* mosi引脚号 */
            GPIO4_28,                     /* miso引脚号 */
    };

    aw_gpio_pin_request("gpio_spi0_gpios",
                        gpio_spi0_gpios,
                        AW_NELEMENTS(gpio_spi0_gpios));
}

/* 设备信息 */
aw_local aw_const struct awbl_gpio_spi_devinfo __g_gpio_spi0_devinfo = {
    {
          IMX6UL_GPIO_SPI0_BUSID,        /* 总线编号 */
    },

    GPIO4_25,                     /* sck引脚号 */
    GPIO4_27,                     /* mosi引脚号 */
    GPIO4_28,                     /* miso引脚号 */

    0xFF,                       /* 接收数据时，MOSI输出值 */
    NULL,                       /* 提供GPIO输出函数，不提供(NULL)则使用系统默认接口 */
//    10,                         /* 驱动传输任务优先级 */
    __pfunc_plfm_init,          /* 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_gpio_spi_dev __g_gpio_spi0_dev;

#define AWBL_HWCONF_GPIO_SPI0                   \
    {                                           \
        AWBL_GPIO_SPI_NAME,                     \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        &__g_gpio_spi0_dev.super.super.super,   \
        &__g_gpio_spi0_devinfo                  \
    },

#else
#define AWBL_HWCONF_GPIO_SPI0

#endif  /* AW_DEV_GPIO_SPI_0 */

#endif  /* __AWBL_HWCONF_GPIO_SPI0_H */

/* end of file */
