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

#ifndef __AWBL_HWCONF_GPIO_I2C0_H
#define __AWBL_HWCONF_GPIO_I2C0_H

#ifdef AW_DEV_GPIO_I2C_0

#include "aw_gpio.h"
#include "driver/busctlr/awbl_gpio_i2c.h"

/* 平台相关初始化 */
aw_local void __awbl_gpio_i2c0_plfm_init (void);


/** \brief 提供给驱动内部进行引脚输入输出的配置(可以用户自己设置SDA具体是开漏或者上拉)  */
aw_local void __awbl_sda_set_input(struct awbl_gpio_i2c_devinfo *p_dev_info)
{
    aw_gpio_pin_cfg(p_dev_info->pin_sda, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
}

aw_local void __awbl_sda_set_output(struct awbl_gpio_i2c_devinfo *p_dev_info)
{
    aw_gpio_pin_cfg(p_dev_info->pin_sda, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
}

/* 设备信息 */
aw_local aw_const struct awbl_gpio_i2c_devinfo __g_gpio_i2c0_devinfo = {
    {
        IMX6UL_GPIO_I2C0_BUSID,
        AW_CFG_IMX6UL_I2C_BUS_SPEED,
        AWBL_I2C_WAITFOREVER,
    },
    GPIO5_7,
    GPIO5_8,
//    GPIO1_28,  /* 对应硬件I2C的SCL */
//    GPIO1_29,  /* 对应硬件I2C的SDA */
    500000,
    __awbl_sda_set_input,
    __awbl_sda_set_output,
    __awbl_gpio_i2c0_plfm_init       /* 平台相关初始化 */
};

/* 平台相关初始化 */
aw_local void __awbl_gpio_i2c0_plfm_init (void)
{
    int gpio_i2c0_gpios[] = {__g_gpio_i2c0_devinfo.pin_sda, __g_gpio_i2c0_devinfo.pin_scl};
    if (aw_gpio_pin_request("gpio_i2c0_gpios",
                             gpio_i2c0_gpios,
                             AW_NELEMENTS(gpio_i2c0_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(__g_gpio_i2c0_devinfo.pin_sda, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
        aw_gpio_pin_cfg(__g_gpio_i2c0_devinfo.pin_scl, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
    }
}

/* 设备实例内存静态分配 */
aw_local struct awbl_gpio_i2c_dev __g_gpio_i2c0_dev;

#define AWBL_HWCONF_GPIO_I2C0                   \
    {                                           \
        AWBL_GPIO_I2C_NAME,                     \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        &__g_gpio_i2c0_dev.super.super.super,   \
        &__g_gpio_i2c0_devinfo                  \
    },

#else
#define AWBL_HWCONF_GPIO_I2C0

#endif /* AW_DEV_GPIO_I2C_0 */

#endif /* __AWBL_HWCONF_GPIO_I2C0_H */

/* end of file */
