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
  IMX6UL ����I2C  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_I2C3_H_
#define AWBL_HWCONF_IMX6UL_I2C3_H_

#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "imx6ul_gpio_pin.h"
#include "aw_gpio.h"
#include "driver/i2c/awbl_imx6ul_i2c.h"
#ifdef AW_DEV_IMX6UL_I2C_3


aw_local void __imx6ul_i2c3_plfm_init (void);

/* �豸��Ϣ */
aw_local struct awbl_imx6ul_i2c_devinfo  __g_imx6ul_i2c3_devinfo = {
    {
        IMX6UL_I2C3_BUSID,           /* ����������Ӧ�����߱�� */
        AW_CFG_IMX6UL_I2C_BUS_SPEED, /* �����������ٶ� */
        100                          /* ��ʱ (ms)*/
    },
    IMX6UL_I2C3_BASE_ADDR,
    INUM_I2C3_IRQ,
    IMX6UL_CLK_I2C3,
    __imx6ul_i2c3_plfm_init,
};

/* �������� */
#define I2C_PAD_CTRL  (PUS_1_47K_Ohm_Pull_Up  | PKE_1_Pull_Keeper_Enabled | \
        PUE_1_Pull | DSE_5_R0_5   | SRE_0_Fast_Slew_Rate | Open_Drain_Enabled)

aw_local void __imx6ul_i2c3_plfm_init (void)
{
    int i2c3_gpios[] = {GPIO2_8, GPIO2_9};

    /* ʱ������ */
    aw_clk_enable(IMX6UL_CLK_I2C3);

    if (aw_gpio_pin_request("i2c3_gpios",
                             i2c3_gpios,
                             AW_NELEMENTS(i2c3_gpios)) == AW_OK) {

        /*  IO���� */
        aw_gpio_pin_cfg(GPIO2_8, GPIO2_8_I2C3_SCL |
                                  IMX6UL_PAD_CTL(I2C_PAD_CTRL));
        aw_gpio_pin_cfg(GPIO2_9, GPIO2_9_I2C3_SDA |
                                  IMX6UL_PAD_CTL(I2C_PAD_CTRL));
    }
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_i2c_dev __g_imx6ul_i2c3_dev;

#define AWBL_HWCONF_IMX6UL_I2C3                 \
    {                                           \
        AWBL_IMX6UL_I2C_NAME,                   \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        (struct awbl_dev *)&__g_imx6ul_i2c3_dev,\
        &(__g_imx6ul_i2c3_devinfo)              \
    },
#else   /* AW_DEV_IMX6UL_I2C */

#define AWBL_HWCONF_IMX6UL_I2C3

#endif  /* AW_DEV_IMX6UL_I2C */



#endif /* AWBL_HWCONF_IMX6UL_I2C3_H_ */
