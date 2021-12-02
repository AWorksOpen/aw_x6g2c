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
    LED≈‰÷√–≈œ¢
*******************************************************************************/

#include "apollo.h"
#include "awbl_gpio.h"
#include "imx6ul_regs_base.h"
#include "driver/gpio/awbl_imx6ul_gpio.h"
#include "imx6ul_gpio_pin.h"

/* “˝Ω≈≈‰÷√◊¥Ã¨±Ì */
aw_local uint8_t __g_gpio_cfg_state[(GPIO_MAX + 7) / 8];

aw_local struct awbl_imx6ul_gpio_info __g_imx6ul_gpio_devinfo = {
    NULL,
    IMX6UL_GPIO1_BASE_ADDR,
    IMX6UL_IOMUXC_REGS_BASE_ADDR,
    {
            GPIO0,
            GPIO159
    },
    {
            INUM_GPIO_PIN0,
            INUM_GPIO_PIN159,
    },
    __g_gpio_cfg_state
};

struct awbl_imx6ul_gpio_dev __g_imx6ul_gpio_dev;

#define AWBL_HWCONF_IMX6UL_GPIO         \
    {                                   \
        AWBL_IMX6UL_GPIO_NAME,          \
        0,                              \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &(__g_imx6ul_gpio_dev.awdev),   \
        &(__g_imx6ul_gpio_devinfo)      \
    },

/* end of file */

