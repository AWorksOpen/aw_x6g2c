/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX6UL GPIO 驱动
 *
 * iMX6UL GPIO 为 iMX6UL 系列MCU的片上GPIO，最多可以有160个GPIO引脚可用，有如下
 * 特性:
 *  - 引脚可以由软件配置为输入/输出
 *  - 每个单独引脚可被用作外部中断输入引脚
 *  - 每个中断可配置为下降沿、上升沿或高低电平产生中断
 *  - 所有GPIO 引脚默认为输入
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "imx6ul_gpio"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_imx6ul_gpio_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-10-24, jac, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_GPIO_PRIVATE_H
#define __AWBL_IMX6UL_GPIO_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio IMX6UL GPIO驱动
 * \copydetails awbl_imx6ul_gpio.h
 * @{
 */

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_pin_cfg IMX6UL GPIO引脚配置
 * @{
 */

#define IMX6UL_PIN_MUX_BITS_START       6
#define IMX6UL_PIN_MUX_BITS_LEN         6

typedef enum __tag_imx6ul_pinmux_sel {
    PIN_MUX_ALT0 = 0x0,
    PIN_MUX_ALT1 = 0x1,
    PIN_MUX_ALT2 = 0x2,
    PIN_MUX_ALT3 = 0x3,
    PIN_MUX_ALT4 = 0x4,
    PIN_MUX_ALT5 = 0x5,
    PIN_MUX_ALT6 = 0x6,
    PIN_MUX_ALT7 = 0x7,
    PIN_MUX_ALT8 = 0x8,
    PIN_MUX_SI_ON = 0x10,
    PIN_MUX_SET_ENABLE = 0x20,
}imx6ul_pin_mux_sel_t;

#define IMX6UL_PIN_MUX(x)               ( ((x)|PIN_MUX_SET_ENABLE) << IMX6UL_PIN_MUX_BITS_START)
#define IMX6UL_PIN_MUX_GET(data)        AW_BITS_GET((data), \
                                                 IMX6UL_PIN_MUX_BITS_START, \
                                                 IMX6UL_PIN_MUX_BITS_LEN-1 )
#define IS_PIN_MUX_SET_ENABLE(flag)     AW_BIT_ISSET(flag, \
                                            IMX6UL_PIN_MUX_BITS_START+IMX6UL_PIN_MUX_BITS_LEN -1)

#define IMX6UL_PAD_CTL_BITS_START       12
#define IMX6UL_PAD_CTL_BITS_LEN         18


typedef enum __tag_imx6ul_pad_ctl_sel {
    SRE_0_Slow_Slew_Rate = 0x0 << 0,
    SRE_0_Fast_Slew_Rate = 0x1 << 0,

    DSE_0_output_driver_disabled = 0x0 << 3,
    DSE_1_R0 = 0x1 <<3,
    DSE_2_R0_2 = 0x2 << 3,
    DSE_3_R0_3 = 0x3 << 3,
    DSE_4_R0_4 = 0x4 << 3,
    DSE_5_R0_5 = 0x5 << 3,
    DSE_6_R0_6 = 0x6 << 3,
    DSE_7_R0_7 = 0x7 << 3,

    SPEED_0_low_50MHz = 0x0 << 6,
    SPEED_1_medium_100MHz = 0x1 << 6,
    SPEED_2_medium_100MHz = 0x2 << 6,
    SPEED_3_max_200MHz = 0x3 << 6,

    Open_Drain_Disabled  = 0x0 << 11,
    Open_Drain_Enabled = 0x1 << 11,

    PKE_0_Pull_Keeper_Disabled  = 0x0 << 12,
    PKE_1_Pull_Keeper_Enabled = 0x1 << 12,

    PUE_0_Keeper  = 0x0 << 13,
    PUE_1_Pull = 0x1 << 13,

    PUS_0_100K_Ohm_Pull_Down = 0x0 <<14,
    PUS_1_47K_Ohm_Pull_Up = 0x1 <<14,
    PUS_2_100K_Ohm_Pull_Up = 0x2 <<14,
    PUS_3_22K_Ohm_Pull_Up = 0x3 <<14,

    HYS_0_Hysteresis_Disabled  = 0x0 << 16,
    HYS_1_Hysteresis_Enabled = 0x1 << 16,

    PAD_CTL_SET_ENABLE = 0x1 << 17,
}imx6ul_pad_ctl_sel_t;

#define IMX6UL_PAD_CTL(x)               ( ((x)|PAD_CTL_SET_ENABLE) << IMX6UL_PAD_CTL_BITS_START)
#define IMX6UL_PAD_CTL_GET(data)        AW_BITS_GET((data), \
                                                 IMX6UL_PAD_CTL_BITS_START, \
                                                 IMX6UL_PAD_CTL_BITS_LEN-1 )

#define IS_PAD_CTL_SET_ENABLE(flag)     AW_BIT_ISSET(flag, \
                                            IMX6UL_PAD_CTL_BITS_START+IMX6UL_PAD_CTL_BITS_LEN -1)

/** @} grp_awbl_drv_imx6ul_gpio_usr_register */
/** @} grp_awbl_drv_imx6ul_gpio*/




/******************************************************************************/
/** \brief imx6ul GPIO 管脚的功能编号 */

#define  GPIO1_0_I2C2_SCL                    IMX6UL_PIN_MUX(PIN_MUX_ALT0|PIN_MUX_SI_ON)
#define  GPIO1_0_GPT1_CAPTURE1               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_0_ANATOP_OTG1_ID              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_0_ENET1_REF_CLK1              IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_0_MQS_RIGHT                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_0_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_0_ENET1_1588_EVENT0_IN        IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_0_SRC_SYSTEM_RESET            IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_0_WDOG3_WDOG_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_1_I2C2_SDA                    IMX6UL_PIN_MUX(PIN_MUX_ALT0|PIN_MUX_SI_ON)
#define  GPIO1_1_GPT1_COMPARE1               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_1_USB_OTG1_OC                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_1_ENET2_REF_CLK2              IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_1_MQS_LEFT                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_1_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_1_ENET1_1588_EVENT0_OUT       IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_1_SRC_EARLY_RESET             IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_1_WDOG1_WDOG_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_2_I2C1_SCL                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_2_GPT1_COMPARE2               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_2_USB_OTG2_PWR                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_2_ENET1_REF_CLK_25M           IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_2_USDHC1_WP                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_2_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_2_SDMA_EXT_EVENT00            IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_2_SRC_ANY_PU_RESET            IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_2_UART1_TX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_3_I2C1_SDA                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_3_GPT1_COMPARE3               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_3_USB_OTG2_OC                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_3_OSC32K_32K_OUT              IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_3_USDHC1_CD_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_3_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_3_CCM_DI0_EXT_CLK             IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_3_SRC_TESTER_ACK              IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_3_UART1_RX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_4_ENET1_REF_CLK1              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_4_PWM3_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_4_USB_OTG1_PWR                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_4_ANATOP_24M_OUT              IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_4_USDHC1_RESET_B              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_4_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_4_ENET2_1588_EVENT0_IN        IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_4_UART5_TX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_5_ENET2_REF_CLK2              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_5_PWM4_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_5_ANATOP_OTG2_ID              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_5_CSI_FIELD                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_5_USDHC1_VSELECT              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_5_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_5_ENET2_1588_EVENT0_OUT       IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_5_UART5_RX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_6_ENET1_MDIO                  IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_6_ENET2_MDIO                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_6_USB_OTG_PWR_WAKE            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_6_CSI_MCLK                    IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_6_USDHC2_WP                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_6_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_6_CCM_WAIT                    IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_6_CCM_REF_EN_B                IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_6_UART1_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_7_ENET1_MDC                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_7_ENET2_MDC                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_7_USB_OTG_HOST_MODE           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_7_CSI_PIXCLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_7_USDHC2_CD_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_7_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_7_CCM_STOP                    IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_7_UART1_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_8_PWM1_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_8_WDOG1_WDOG_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_8_SPDIF_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_8_CSI_VSYNC                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_8_USDHC2_VSELECT              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_8_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_8_CCM_PMIC_RDY                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_8_UART5_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_9_PWM2_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_9_WDOG1_WDOG_ANY              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_9_SPDIF_IN                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_9_CSI_HSYNC                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_9_USDHC2_RESET_B              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_9_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_9_USDHC1_RESET_B              IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_9_UART5_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_10_SJC_MOD                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_10_GPT2_CLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_10_SPDIF_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_10_ENET1_REF_CLK_25M          IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_10_CCM_PMIC_RDY               IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_10_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_10_SDMA_EXT_EVENT00           IMX6UL_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO1_11_SJC_TMS                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_11_GPT2_CAPTURE1              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_11_SAI2_MCLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_11_CCM_CLKO1                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_11_CCM_WAIT                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_11_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_11_SDMA_EXT_EVENT01           IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_11_EPIT1_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_12_SJC_TDO                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_12_GPT2_CAPTURE2              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_12_SAI2_TX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_12_CCM_CLKO2                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_12_CCM_STOP                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_12_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_12_MQS_RIGHT                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_12_EPIT2_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_13_SJC_TDI                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_13_GPT2_COMPARE1              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_13_SAI2_TX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_13_PWM6_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_13_CCM_STOP                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_13_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_13_MQS_LEFT                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_13_SIM1_POWER_FAIL            IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_14_SJC_TCK                    IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_14_GPT2_COMPARE2              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_14_SAI2_RX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_14_PWM7_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_14_CCM_STOP                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_14_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_14_OSC32K_32K_OUT             IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_14_SIM2_POWER_FAIL            IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_15_SJC_TRSTB                  IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_15_GPT2_COMPARE3              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_15_SAI2_TX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_15_PWM8_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_15_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_15_ANATOP_24M_OUT             IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_15_CAAM_RNG_OSC_OBS           IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_16_UART1_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_16_ENET1_RDATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_16_I2C3_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_16_CSI_DATA02                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_16_GPT1_COMPARE1              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_16_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_16_SPDIF_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_17_UART1_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_17_ENET1_RDATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_17_I2C3_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_17_CSI_DATA03                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_17_GPT1_CLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_17_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_17_SPDIF_IN                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_18_UART1_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_18_ENET1_RX_CLK               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_18_USDHC1_WP                  IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_18_CSI_DATA04                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_18_ENET2_1588_EVENT1_IN       IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_18_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_18_USDHC2_WP                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_19_UART1_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_19_ENET1_TX_ER                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_19_USDHC1_CD_B                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_19_CSI_DATA05                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_19_ENET2_1588_EVENT1_OUT      IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_19_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_19_USDHC2_CD_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_20_UART2_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_20_ENET1_TDATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_20_I2C4_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_20_CSI_DATA06                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_20_GPT1_CAPTURE1              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_20_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_20_ECSPI3_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_21_UART2_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_21_ENET1_TDATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_21_I2C4_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_21_CSI_DATA07                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_21_GPT1_CAPTURE2              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_21_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_21_SJC_DONE                   IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_21_ECSPI3_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_22_UART2_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_22_ENET1_CRS                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_22_FLEXCAN2_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_22_CSI_DATA08                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_22_GPT1_COMPARE2              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_22_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_22_SJC_DE_B                   IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_22_ECSPI3_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_23_UART2_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_23_ENET1_COL                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_23_FLEXCAN2_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_23_CSI_DATA09                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_23_GPT1_COMPARE3              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_23_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_23_SJC_FAIL                   IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_23_ECSPI3_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_24_UART3_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_24_ENET2_RDATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_24_SIM1_PORT0_PD              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_24_CSI_DATA01                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_24_UART2_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_24_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_24_SJC_JTAG_ACT               IMX6UL_PIN_MUX(PIN_MUX_ALT7)
#define  GPIO1_24_ANATOP_OTG1_ID             IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_25_UART3_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_25_ENET2_RDATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_25_SIM2_PORT0_PD              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_25_CSI_DATA00                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_25_UART2_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_25_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_25_EPIT1_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_26_UART3_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_26_ENET2_RX_CLK               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_26_FLEXCAN1_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_26_CSI_DATA10                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_26_ENET1_1588_EVENT1_IN       IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_26_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_26_EPIT2_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_27_UART3_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_27_ENET2_TX_ER                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_27_FLEXCAN1_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_27_CSI_DATA11                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_27_ENET1_1588_EVENT1_OUT      IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_27_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_27_WDOG1_WDOG_B               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_28_UART4_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_28_ENET2_TDATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_28_I2C1_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT2|PIN_MUX_SI_ON)
#define  GPIO1_28_CSI_DATA12                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_28_CSU_CSU_ALARM_AUT02        IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_28_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_28_ECSPI2_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_29_UART4_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_29_ENET2_TDATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_29_I2C1_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT2|PIN_MUX_SI_ON)
#define  GPIO1_29_CSI_DATA13                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_29_CSU_CSU_ALARM_AUT01        IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_29_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_29_ECSPI2_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_30_UART5_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_30_ENET2_CRS                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_30_I2C2_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_30_CSI_DATA14                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_30_CSU_CSU_ALARM_AUT00        IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_30_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_30_ECSPI2_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO1_31_UART5_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_31_ENET2_COL                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_31_I2C2_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_31_CSI_DATA15                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_31_CSU_CSU_INT_DEB            IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_31_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_31_ECSPI2_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT8)




#define  GPIO2_0_ENET1_RDATA00               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_0_UART4_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_0_PWM1_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_0_CSI_DATA16                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_0_FLEXCAN1_TX                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_0_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_0_KPP_ROW00                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_0_USDHC1_LCTL                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_1_ENET1_RDATA01               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_1_UART4_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_1_PWM2_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_1_CSI_DATA17                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_1_FLEXCAN1_RX                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_1_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_1_KPP_COL00                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_1_USDHC2_LCTL                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_2_ENET1_RX_EN                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_2_UART5_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_2_OSC32K_32K_OUT              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_2_CSI_DATA18                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_2_FLEXCAN2_TX                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_2_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_2_KPP_ROW01                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_2_USDHC1_VSELECT              IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_3_ENET1_TDATA00               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_3_UART5_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_3_ANATOP_24M_OUT              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_3_CSI_DATA19                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_3_FLEXCAN2_RX                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_3_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_3_KPP_COL01                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_3_USDHC2_VSELECT              IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_4_ENET1_TDATA01               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_4_UART6_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_4_PWM5_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_4_CSI_DATA20                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_4_ENET2_MDIO                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_4_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_4_KPP_ROW02                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_4_WDOG1_WDOG_RST_B_DEB        IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_5_ENET1_TX_EN                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_5_UART6_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_5_PWM6_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_5_CSI_DATA21                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_5_ENET2_MDC                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_5_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_5_KPP_COL02                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_5_WDOG2_WDOG_RST_B_DEB        IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_6_ENET1_TX_CLK                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_6_UART7_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_6_PWM7_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_6_CSI_DATA22                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_6_ENET1_REF_CLK1              IMX6UL_PIN_MUX(PIN_MUX_ALT4|PIN_MUX_SI_ON)
#define  GPIO2_6_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_6_KPP_ROW03                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_6_GPT1_CLK                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_7_ENET1_RX_ER                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_7_UART7_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_7_PWM8_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_7_CSI_DATA23                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_7_EIM_CRE                     IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_7_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_7_KPP_COL03                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_7_GPT1_CAPTURE2               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_8_ENET2_RDATA00               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_8_UART6_TX                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_8_SIM1_PORT0_TRXD             IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_8_I2C3_SCL                    IMX6UL_PIN_MUX(PIN_MUX_ALT3|PIN_MUX_SI_ON)
#define  GPIO2_8_ENET1_MDIO                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_8_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_8_KPP_ROW04                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_8_USB_OTG1_PWR                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_9_ENET2_RDATA01               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_9_UART6_RX                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_9_SIM1_PORT0_CLK              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_9_I2C3_SDA                    IMX6UL_PIN_MUX(PIN_MUX_ALT3|PIN_MUX_SI_ON)
#define  GPIO2_9_ENET1_MDC                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_9_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_9_KPP_COL04                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_9_USB_OTG1_OC                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_10_ENET2_RX_EN                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_10_UART7_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_10_SIM1_PORT0_RST_B           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_10_I2C4_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT3|PIN_MUX_SI_ON)
#define  GPIO2_10_EIM_ADDR26                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_10_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_10_KPP_ROW05                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_10_ENET1_REF_CLK_25M          IMX6UL_PIN_MUX(PIN_MUX_ALT8)


#define  GPIO2_11_ENET2_TDATA00              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_11_UART7_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_11_SIM1_PORT0_SVEN            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_11_I2C4_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT3|PIN_MUX_SI_ON)
#define  GPIO2_11_EIM_EB_B02                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_11_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_11_KPP_COL05                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_11_ANATOP_24M_OUT             IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_12_ENET2_TDATA01              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_12_UART8_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_12_SIM2_PORT0_TRXD            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_12_ECSPI4_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_12_EIM_EB_B03                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_12_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_12_KPP_ROW06                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_12_USB_OTG2_PWR               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_13_ENET2_TX_EN                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_13_UART8_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_13_SIM2_PORT0_CLK             IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_13_ECSPI4_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_13_EIM_ACLK_FREERUN           IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_13_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_13_KPP_COL06                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_13_USB_OTG2_OC                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_14_ENET2_TX_CLK               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_14_UART8_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_14_SIM2_PORT0_RST_B           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_14_ECSPI4_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_14_ENET2_REF_CLK2             IMX6UL_PIN_MUX(PIN_MUX_ALT4|PIN_MUX_SI_ON)
#define  GPIO2_14_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_14_KPP_ROW07                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_14_ANATOP_OTG2_ID             IMX6UL_PIN_MUX(PIN_MUX_ALT8)


#define  GPIO2_15_ENET2_RX_ER                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_15_UART8_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_15_SIM2_PORT0_SVEN            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_15_ECSPI4_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_15_EIM_ADDR25                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_15_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_15_KPP_COL07                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_15_WDOG1_WDOG_ANY             IMX6UL_PIN_MUX(PIN_MUX_ALT8)


#define  GPIO2_16_USDHC1_CMD                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_16_GPT2_COMPARE1              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_16_SAI2_RX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_16_SPDIF_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_16_EIM_ADDR19                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_16_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_16_SDMA_EXT_EVENT00           IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_16_USB_OTG1_PWR               IMX6UL_PIN_MUX(PIN_MUX_ALT8)


#define  GPIO2_17_USDHC1_CLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_17_GPT2_COMPARE2              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_17_SAI2_MCLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_17_SPDIF_IN                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_17_EIM_ADDR20                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_17_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_17_USB_OTG1_OC                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_18_USDHC1_DATA0               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_18_GPT2_COMPARE3              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_18_SAI2_TX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_18_FLEXCAN1_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_18_EIM_ADDR21                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_18_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_18_ANATOP_OTG1_ID             IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_19_USDHC1_DATA1               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_19_GPT2_CLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_19_SAI2_TX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_19_FLEXCAN1_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_19_EIM_ADDR22                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_19_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_19_USB_OTG2_PWR               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_20_USDHC1_DATA2               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_20_GPT2_CAPTURE1              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_20_SAI2_RX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_20_FLEXCAN2_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_20_EIM_ADDR23                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_20_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_20_CCM_CLKO1                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_20_USB_OTG2_OC                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO2_21_USDHC1_DATA3               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_21_GPT2_CAPTURE2              IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_21_SAI2_TX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_21_FLEXCAN2_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_21_EIM_ADDR24                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_21_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_21_CCM_CLKO2                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO2_21_ANATOP_OTG2_ID             IMX6UL_PIN_MUX(PIN_MUX_ALT8)



#define  GPIO3_0_LCDIF_CLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_0_LCDIF_WR_RWN                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_0_UART4_TX                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_0_SAI3_MCLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_0_EIM_CS2_B                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_0_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_0_WDOG1_WDOG_RST_B_DEB        IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_1_LCDIF_ENABLE                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_1_LCDIF_RD_E                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_1_UART4_RX                    IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_1_SAI3_TX_SYNC                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_1_EIM_CS3_B                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_1_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_1_ECSPI2_RDY                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_2_LCDIF_HSYNC                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_2_LCDIF_RS                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_2_UART4_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_2_SAI3_TX_BCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_2_WDOG3_WDOG_RST_B_DEB        IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_2_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_2_ECSPI2_SS1                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_3_LCDIF_VSYNC                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_3_LCDIF_BUSY                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_3_UART4_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_3_SAI3_RX_DATA                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_3_WDOG2_WDOG_B                IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_3_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_3_ECSPI2_SS2                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_4_LCDIF_RESET                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_4_LCDIF_CS                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_4_CA7_MX6UL_EVENTI            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_4_SAI3_TX_DATA                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_4_WDOG1_WDOG_ANY              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_4_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_4_ECSPI2_SS3                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_5_LCDIF_DATA00                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_5_PWM1_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_5_ENET1_1588_EVENT2_IN        IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_5_I2C3_SDA                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_5_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_5_SRC_BT_CFG00                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_5_SAI1_MCLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_6_LCDIF_DATA01                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_6_PWM2_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_6_ENET1_1588_EVENT2_OUT       IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_6_I2C3_SCL                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_6_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_6_SRC_BT_CFG01                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_6_SAI1_TX_SYNC                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_7_LCDIF_DATA02                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_7_PWM3_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_7_ENET1_1588_EVENT3_IN        IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_7_I2C4_SDA                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_7_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_7_SRC_BT_CFG02                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_7_SAI1_TX_BCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_8_LCDIF_DATA03                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_8_PWM4_OUT                    IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_8_ENET1_1588_EVENT3_OUT       IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_8_I2C4_SCL                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_8_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_8_SRC_BT_CFG03                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_8_SAI1_RX_DATA                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_9_LCDIF_DATA04                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_9_UART8_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_9_ENET2_1588_EVENT2_IN        IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_9_SPDIF_SR_CLK                IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_9_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_9_SRC_BT_CFG04                IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_9_SAI1_TX_DATA                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_10_LCDIF_DATA05               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_10_UART8_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_10_ENET2_1588_EVENT2_OUT      IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_10_SPDIF_OUT                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_10_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_10_SRC_BT_CFG05               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_10_ECSPI1_SS1                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_11_LCDIF_DATA06               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_11_UART7_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_11_ENET2_1588_EVENT3_IN       IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_11_SPDIF_LOCK                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_11_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_11_SRC_BT_CFG06               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_11_ECSPI1_SS2                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_12_LCDIF_DATA07               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_12_UART7_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_12_ENET2_1588_EVENT3_OUT      IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_12_SPDIF_EXT_CLK              IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_12_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_12_SRC_BT_CFG07               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_12_ECSPI1_SS3                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_13_LCDIF_DATA08               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_13_SPDIF_IN                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_13_CSI_DATA16                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_13_EIM_DATA00                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_13_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_13_SRC_BT_CFG08               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_13_FLEXCAN1_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_14_LCDIF_DATA09               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_14_SAI3_MCLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_14_CSI_DATA17                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_14_EIM_DATA01                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_14_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_14_SRC_BT_CFG09               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_14_FLEXCAN1_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_15_LCDIF_DATA10               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_15_SAI3_RX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_15_CSI_DATA18                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_15_EIM_DATA02                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_15_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_15_SRC_BT_CFG10               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_15_FLEXCAN2_TX                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_16_LCDIF_DATA11               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_16_SAI3_RX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_16_CSI_DATA19                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_16_EIM_DATA03                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_16_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_16_SRC_BT_CFG11               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_16_FLEXCAN2_RX                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_17_LCDIF_DATA12               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_17_SAI3_TX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_17_CSI_DATA20                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_17_EIM_DATA04                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_17_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_17_SRC_BT_CFG12               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_17_ECSPI1_RDY                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_18_LCDIF_DATA13               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_18_SAI3_TX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_18_CSI_DATA21                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_18_EIM_DATA05                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_18_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_18_SRC_BT_CFG13               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_18_USDHC2_RESET_B             IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_19_LCDIF_DATA14               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_19_SAI3_RX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_19_CSI_DATA22                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_19_EIM_DATA06                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_19_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_19_SRC_BT_CFG14               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_19_USDHC2_DATA4               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_20_LCDIF_DATA15               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_20_SAI3_TX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_20_CSI_DATA23                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_20_EIM_DATA07                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_20_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_20_SRC_BT_CFG15               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_20_USDHC2_DATA5               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_21_LCDIF_DATA16               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_21_UART7_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_21_CSI_DATA01                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_21_EIM_DATA08                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_21_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_21_SRC_BT_CFG24               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_21_USDHC2_DATA6               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_22_LCDIF_DATA17               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_22_UART7_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_22_CSI_DATA00                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_22_EIM_DATA09                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_22_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_22_SRC_BT_CFG25               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_22_USDHC2_DATA7               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_23_LCDIF_DATA18               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_23_PWM5_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_23_CA7_MX6UL_EVENTO           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_23_CSI_DATA10                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_23_EIM_DATA10                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_23_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_23_SRC_BT_CFG26               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_23_USDHC2_CMD                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_24_LCDIF_DATA19               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_24_PWM6_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_24_WDOG1_WDOG_ANY             IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_24_CSI_DATA11                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_24_EIM_DATA11                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_24_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_24_SRC_BT_CFG27               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_24_USDHC2_CLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_25_LCDIF_DATA20               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_25_UART8_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_25_ECSPI1_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_25_CSI_DATA12                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_25_EIM_DATA12                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_25_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_25_SRC_BT_CFG28               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_25_USDHC2_DATA0               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_26_LCDIF_DATA21               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_26_UART8_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_26_ECSPI1_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_26_CSI_DATA13                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_26_EIM_DATA13                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_26_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_26_SRC_BT_CFG29               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_26_USDHC2_DATA1               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_27_LCDIF_DATA22               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_27_MQS_RIGHT                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_27_ECSPI1_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_27_CSI_DATA14                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_27_EIM_DATA14                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_27_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_27_SRC_BT_CFG30               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_27_USDHC2_DATA2               IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO3_28_LCDIF_DATA23               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_28_MQS_LEFT                   IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_28_ECSPI1_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_28_CSI_DATA15                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_28_EIM_DATA15                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_28_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_28_SRC_BT_CFG31               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO3_28_USDHC2_DATA3               IMX6UL_PIN_MUX(PIN_MUX_ALT8)



#define  GPIO4_0_RAWNAND_RE_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_0_USDHC2_CLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_0_QSPI_B_SCLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_0_KPP_ROW00                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_0_EIM_EB_B00                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_0_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_0_ECSPI3_SS2                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_1_RAWNAND_WE_B                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_1_USDHC2_CMD                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_1_QSPI_B_SS0_B                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_1_KPP_COL00                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_1_EIM_EB_B01                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_1_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_1_ECSPI3_SS3                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_2_RAWNAND_DATA00              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_2_USDHC2_DATA0                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_2_QSPI_B_SS1_B                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_2_KPP_ROW01                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_2_EIM_AD08                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_2_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_2_ECSPI4_RDY                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_3_RAWNAND_DATA01              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_3_USDHC2_DATA1                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_3_QSPI_B_DQS                  IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_3_KPP_COL01                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_3_EIM_AD09                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_3_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_3_ECSPI4_SS1                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_4_RAWNAND_DATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_4_USDHC2_DATA2                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_4_QSPI_B_DATA00               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_4_KPP_ROW02                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_4_EIM_AD10                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_4_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_4_ECSPI4_SS2                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_5_RAWNAND_DATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_5_USDHC2_DATA3                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_5_QSPI_B_DATA01               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_5_KPP_COL02                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_5_EIM_AD11                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_5_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_5_ECSPI4_SS3                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_6_RAWNAND_DATA04              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_6_USDHC2_DATA4                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_6_QSPI_B_DATA02               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_6_ECSPI4_SCLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_6_EIM_AD12                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_6_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_6_UART2_TX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_7_RAWNAND_DATA05              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_7_USDHC2_DATA5                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_7_QSPI_B_DATA03               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_7_ECSPI4_MOSI                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_7_EIM_AD13                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_7_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_7_UART2_RX                    IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_8_RAWNAND_DATA06              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_8_USDHC2_DATA6                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_8_SAI2_RX_BCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_8_ECSPI4_MISO                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_8_EIM_AD14                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_8_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_8_UART2_CTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_9_RAWNAND_DATA07              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_9_USDHC2_DATA7                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_9_QSPI_A_SS1_B                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_9_ECSPI4_SS0                  IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_9_EIM_AD15                    IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_9_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_9_UART2_RTS_B                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_10_RAWNAND_ALE                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_10_USDHC2_RESET_B             IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_10_QSPI_A_DQS                 IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_10_PWM3_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_10_EIM_ADDR17                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_10_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_10_ECSPI3_SS1                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_11_RAWNAND_WP_B               IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_11_USDHC1_RESET_B             IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_11_QSPI_A_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_11_PWM4_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_11_EIM_BCLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_11_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_11_ECSPI3_RDY                 IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_12_RAWNAND_READY_B            IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_12_USDHC1_DATA4               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_12_QSPI_A_DATA00              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_12_ECSPI3_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_12_EIM_CS1_B                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_12_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_12_UART3_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_13_RAWNAND_CE0_B              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_13_USDHC1_DATA5               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_13_QSPI_A_DATA01              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_13_ECSPI3_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_13_EIM_DTACK_B                IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_13_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_13_UART3_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_14_RAWNAND_CE1_B              IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_14_USDHC1_DATA6               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_14_QSPI_A_DATA02              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_14_ECSPI3_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_14_EIM_ADDR18                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_14_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_14_UART3_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_15_RAWNAND_CLE                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_15_USDHC1_DATA7               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_15_QSPI_A_DATA03              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_15_ECSPI3_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_15_EIM_ADDR16                 IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_15_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_15_UART3_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_16_RAWNAND_DQS                IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_16_CSI_FIELD                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_16_QSPI_A_SS0_B               IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_16_PWM5_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_16_EIM_WAIT                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_16_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_16_SDMA_EXT_EVENT01           IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_16_SPDIF_EXT_CLK              IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_17_CSI_MCLK                   IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_17_USDHC2_CD_B                IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_17_RAWNAND_CE2_B              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_17_I2C1_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_17_EIM_CS0_B                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_17_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_17_SNVS_HP_VIO_5_CTL          IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_17_UART6_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_18_CSI_PIXCLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_18_USDHC2_WP                  IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_18_RAWNAND_CE3_B              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_18_I2C1_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_18_EIM_OE                     IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_18_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_18_SNVS_HP_VIO_5              IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_18_UART6_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_19_CSI_VSYNC                  IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_19_USDHC2_CLK                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_19_SIM1_PORT1_CLK             IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_19_I2C2_SDA                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_19_EIM_RW                     IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_19_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_19_PWM7_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_19_UART6_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_20_CSI_HSYNC                  IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_20_USDHC2_CMD                 IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_20_SIM1_PORT1_PD              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_20_I2C2_SCL                   IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_20_EIM_LBA_B                  IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_20_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_20_PWM8_OUT                   IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_20_UART6_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_21_CSI_DATA02                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_21_USDHC2_DATA0               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_21_SIM1_PORT1_RST_B           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_21_ECSPI2_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_21_EIM_AD00                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_21_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_21_SRC_INT_BOOT               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_21_UART5_TX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_22_CSI_DATA03                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_22_USDHC2_DATA1               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_22_SIM1_PORT1_SVEN            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_22_ECSPI2_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_22_EIM_AD01                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_22_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_22_SAI1_MCLK                  IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_22_UART5_RX                   IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_23_CSI_DATA04                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_23_USDHC2_DATA2               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_23_SIM1_PORT1_TRXD            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_23_ECSPI2_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_23_EIM_AD02                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_23_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_23_SAI1_RX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_23_UART5_RTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_24_CSI_DATA05                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_24_USDHC2_DATA3               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_24_SIM2_PORT1_PD              IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_24_ECSPI2_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_24_EIM_AD03                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_24_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_24_SAI1_RX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_24_UART5_CTS_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_25_CSI_DATA06                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_25_USDHC2_DATA4               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_25_SIM2_PORT1_CLK             IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_25_ECSPI1_SCLK                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_25_EIM_AD04                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_25_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_25_SAI1_TX_SYNC               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_25_USDHC1_WP                  IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_26_CSI_DATA07                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_26_USDHC2_DATA5               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_26_SIM2_PORT1_RST_B           IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_26_ECSPI1_SS0                 IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_26_EIM_AD05                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_26_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_26_SAI1_TX_BCLK               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_26_USDHC1_CD_B                IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_27_CSI_DATA08                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_27_USDHC2_DATA6               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_27_SIM2_PORT1_SVEN            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_27_ECSPI1_MOSI                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_27_EIM_AD06                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_27_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_27_SAI1_RX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_27_USDHC1_RESET_B             IMX6UL_PIN_MUX(PIN_MUX_ALT8)

#define  GPIO4_28_CSI_DATA09                 IMX6UL_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_28_USDHC2_DATA7               IMX6UL_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_28_SIM2_PORT1_TRXD            IMX6UL_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_28_ECSPI1_MISO                IMX6UL_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_28_EIM_AD07                   IMX6UL_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_28_GPIO                       IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_28_SAI1_TX_DATA               IMX6UL_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO4_28_USDHC1_VSELECT             IMX6UL_PIN_MUX(PIN_MUX_ALT8)



#define  GPIO5_0_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_1_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_2_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_3_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_4_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_5_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_6_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_7_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_8_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_9_GPIO                        IMX6UL_PIN_MUX(PIN_MUX_ALT5)




#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
