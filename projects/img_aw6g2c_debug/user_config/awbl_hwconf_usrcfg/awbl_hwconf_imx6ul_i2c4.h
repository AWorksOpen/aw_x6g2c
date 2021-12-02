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
  IMX6UL 内置I2C  配置信息
*******************************************************************************/
#ifndef AWBL_HWCONF_IMX6UL_I2C4_H_
#define AWBL_HWCONF_IMX6UL_I2C4_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "imx6ul_gpio_pin.h"
#include "aw_gpio.h"
#include "driver/i2c/awbl_imx6ul_i2c.h"
#ifdef AW_DEV_IMX6UL_I2C_4


aw_local void __imx6ul_i2c4_plfm_init (void);

/* 设备信息 */
aw_local struct awbl_imx6ul_i2c_devinfo  __g_imx6ul_i2c4_devinfo = {
    {
        IMX6UL_I2C4_BUSID,           /* 控制器所对应的总线编号 */
        AW_CFG_IMX6UL_I2C_BUS_SPEED, /* 控制器总线速度 */
        100                          /* 超时 (ms)*/
    },
    IMX6UL_I2C4_BASE_ADDR,
    INUM_I2C4_IRQ,
    IMX6UL_CLK_I2C4,
    __imx6ul_i2c4_plfm_init,
};

/* 引脚配置 */
#define I2C_PAD_CTRL  (PUS_1_47K_Ohm_Pull_Up  | PKE_1_Pull_Keeper_Enabled | \
        PUE_1_Pull | DSE_5_R0_5   | SRE_0_Fast_Slew_Rate | Open_Drain_Enabled)

aw_local void __imx6ul_i2c4_plfm_init (void)
{
    int i2c4_gpios[] = {GPIO2_10, GPIO2_11};

    /* 时钟配置 */
    aw_clk_enable(IMX6UL_CLK_I2C4);

    if (aw_gpio_pin_request("i2c4_gpios",
                             i2c4_gpios,
                             AW_NELEMENTS(i2c4_gpios)) == AW_OK) {

        /*  IO配置 */
        aw_gpio_pin_cfg(GPIO2_10, GPIO2_10_I2C4_SCL |
                                  IMX6UL_PAD_CTL(I2C_PAD_CTRL));
        aw_gpio_pin_cfg(GPIO2_11, GPIO2_11_I2C4_SDA |
                                  IMX6UL_PAD_CTL(I2C_PAD_CTRL));
    }
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_i2c_dev __g_imx6ul_i2c4_dev;

#define AWBL_HWCONF_IMX6UL_I2C4                 \
    {                                           \
        AWBL_IMX6UL_I2C_NAME,                   \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        (struct awbl_dev *)&__g_imx6ul_i2c4_dev,\
        &(__g_imx6ul_i2c4_devinfo)              \
    },
#else   /* AW_DEV_IMX6UL_I2C */

#define AWBL_HWCONF_IMX6UL_I2C4

#endif  /* AW_DEV_IMX6UL_I2C */



#endif /* AWBL_HWCONF_IMX6UL_I2C4_H_ */
