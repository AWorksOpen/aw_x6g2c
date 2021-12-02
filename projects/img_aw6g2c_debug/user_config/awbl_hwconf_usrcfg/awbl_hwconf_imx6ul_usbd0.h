/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/
#ifndef __AWBL_HWCONF_IMX6UL_USBD0_H
#define __AWBL_HWCONF_IMX6UL_USBD0_H

#ifdef AW_DEV_IMX6UL_USB_DEVICE0

#include "aw_gpio.h"
#include "aw_io.h"

#include "driver/usb/awbl_imx6ul_usbd.h"
//#include "awbl_imx6ul_usbd.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"
#include "imx6ul_regs_base.h"
#include "imx6ul_gpio_pin.h"
#include "aw_gpio.h"
#include "imx6ul_inum.h"

#define OTG_PAD_CTRL    (SRE_0_Fast_Slew_Rate | DSE_3_R0_3 |      \
                         SPEED_1_medium_100MHz|                   \
                         PKE_1_Pull_Keeper_Enabled | PUE_1_Pull | \
                         PUS_1_47K_Ohm_Pull_Up| HYS_1_Hysteresis_Enabled)


#define __IMX6UL_USBD0_CHARGE_DETECT      (0x1B0)
#define __IMX6UL_USBD0_CHARGE_DETECT_SET  (__IMX6UL_USBD0_CHARGE_DETECT+4)
#define __IMX6UL_USBD0_CHARGE_DETECT_CLR  (__IMX6UL_USBD0_CHARGE_DETECT+8)
#define __IMX6UL_USBD0_CHARGE_DETECT_TOG  (__IMX6UL_USBD0_CHARGE_DETECT+12)

#define __IMX6UL_USBD0_CHARGE_DETECT_DISABLE  AW_BIT(20)


aw_local void __imx6ul_usbd0_plfm_init (void)
{
    uint32_t       temp;
    void           *p = (void *)IMX6UL_PMU_REG_3P0;

    // power on the 3P0 regulator
    temp = readl(p);
    if( !( temp & PMU_REG_3P0_OK_VDD3P0 ) ) {
        temp = PMU_REG_3P0_ENABLE| PMU_REG_3P0_ENABLE_BO;
        //select otg2 vbus as the input voltage
        temp |= PMU_REG_3P0_VBUS_SEL_OTG2;
        //the voltage is 3.0V
        temp |= PMU_REG_3P0_OUTPUT_TRIG(0xF);
        temp |= PMU_REG_3P0_BO_OFFSET(0);

        writel(temp,p);

        do {
            temp = readl(p);
        }while( !( temp & PMU_REG_3P0_OK_VDD3P0 ) ) ;
    }

    //disable the charge detect
    p = (void *)(IMX6UL_ANALOG_DIG_BASE_ADDR +
            __IMX6UL_USBD0_CHARGE_DETECT_SET);
    writel(__IMX6UL_USBD0_CHARGE_DETECT_DISABLE,p);

    /* OTG1 ID */
    aw_gpio_pin_cfg(GPIO1_0, GPIO1_0_ANATOP_OTG1_ID|IMX6UL_PAD_CTL(OTG_PAD_CTRL));

    aw_clk_enable(IMX6UL_CLK_USBOH3);
    aw_clk_enable(IMX6UL_CLK_USBPHY1_GATE);

};


/* 设备信息 */
aw_local aw_const struct awbl_imx6ul_usbd_info  __g_imx6ul_usbd0_info = {
    {
        "/dev/usbd0",
        USBD_CFG_SELF_POWERED
    },
    IMX6UL_USBCTRL1_BASE_ADDR,  /**< 寄存器基地址 */
    IMX6UL_USBPHY1_BASE_ADDR,
    INUM_OTG1_IRQ,
    5,                        /**< 处理usb中断业务的任务优先级 */
    32,
    __imx6ul_usbd0_plfm_init   /**< 平台相关初始化：初始化引脚 */

};

/* 设备实例内存静态分配 */
static struct awbl_imx6ul_usbd __g_imx6ul_usbd0_dev;

#define AWBL_HWCONF_IMX6UL_USBD0                    \
    {                                               \
        AWBL_IMX6UL_USBD_DRV_NAME,                  \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_usbd0_dev,   \
        &(__g_imx6ul_usbd0_info)                    \
    },

#else
#define AWBL_HWCONF_IMX6UL_USBD0

#endif  /* AW_DEV_IMX6UL_USB_DEVICE0 */


#endif  /* __AWBL_HWCONF_IMX6UL_USBD0_H */

/* end of file */
