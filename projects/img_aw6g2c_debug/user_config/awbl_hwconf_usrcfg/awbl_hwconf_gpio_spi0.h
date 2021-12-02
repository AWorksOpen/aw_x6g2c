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

/* ��ʼ�� */
aw_local void __pfunc_plfm_init (void) {
    int gpio_spi0_gpios[] = {
            GPIO4_25,                     /* sck���ź� */
            GPIO4_27,                     /* mosi���ź� */
            GPIO4_28,                     /* miso���ź� */
    };

    aw_gpio_pin_request("gpio_spi0_gpios",
                        gpio_spi0_gpios,
                        AW_NELEMENTS(gpio_spi0_gpios));
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_gpio_spi_devinfo __g_gpio_spi0_devinfo = {
    {
          IMX6UL_GPIO_SPI0_BUSID,        /* ���߱�� */
    },

    GPIO4_25,                     /* sck���ź� */
    GPIO4_27,                     /* mosi���ź� */
    GPIO4_28,                     /* miso���ź� */

    0xFF,                       /* ��������ʱ��MOSI���ֵ */
    NULL,                       /* �ṩGPIO������������ṩ(NULL)��ʹ��ϵͳĬ�Ͻӿ� */
//    10,                         /* ���������������ȼ� */
    __pfunc_plfm_init,          /* ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
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
