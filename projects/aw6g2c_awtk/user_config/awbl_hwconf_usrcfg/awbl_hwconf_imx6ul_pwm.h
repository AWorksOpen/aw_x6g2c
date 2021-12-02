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
/**
 * \file
 * \brief Freescale i.MX283 PWM外设驱动配置信息
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-06  chenshuyi, first implemetation
 * \endinternal
 */

/*******************************************************************************
  PWM配置信息
*******************************************************************************/

#include "driver/pwm/awbl_imx6ul_pwm.h"
#include "imx6ul_pwmid.h"
#include "imx6ul_regs_base.h"
#include "imx6ul_inum.h"
#include "imx6ul_gpio_pin.h"
#include "aw_gpio.h"

#ifdef AW_DEV_IMX6UL_PWM

/*
 *                                    IMX6UL Pin Definitions
 * ---------------------------------------------------------------------------------------------------------------------------------------------------
 * |   PAD NAME       |      ALT0     |      ALT1     |      ALT2     |      ALT3     |      ALT4     |      ALT5     |      ALT6     |      ALT8     |
 * ---------------------------------------------------------------------------------------------------------------------------------------------------
 * |   ENET1_RX_DATA0 | ENET1_RDATA00 |  UART4_RTS_B  |   PWM1_OUT    |  CSI_DATA16   |  FLEXCAN1_TX  |   GPIO2_IO00  |   KPP_ROW00   |  USDHC1_LCTL  |
 * |   GPIO1_IO08     |   PWM1_OUT    |  WDOG1_WDOG_B |   SPDIF_OUT   |  CSI_VSYNC    | USDHC2_VSELECT|   GPIO1_IO08  |  CCM_PMIC_RDY |  UART5_RTS_B  |
 * |   LCD_DATA00     | LCDIF_DATA00  |    PWM1_OUT   |      -        | ENET1_EVT2_IN |   I2C3_SDA    |   GPIO3_IO05  | SRC_BT_CFG00  |   SAI1_MCLK   |
 * |   ENET1_RX_DATA1 | ENET1_RDATA01 |  UART4_CTS_B  |   PWM2_OUT    |  CSI_DATA17   |  FLEXCAN1_RX  |   GPIO2_IO01  |   KPP_COL00   |  USDHC2_LCTL  |
 * |   GPIO1_IO09     |   PWM2_OUT    |  WDOG1_ANY    |   SPDIF_IN    |  CSI_HSYNC    | USDHC2_RESET_B|   GPIO1_IO09  | USDHC1_RESET_B|  UART5_CTS_B  |
 * |   LCD_DATA01     | LCDIF_DATA01  |    PWM2_OUT   |      -        | ENET1_EVT2_OUT|   I2C3_SCL    |   GPIO3_IO06  | SRC_BT_CFG01  | SAI1_TX_SYNC  |
 * |   GPIO1_IO04     | ENET1_REF_CLK1|    PWM3_OUT   |  USB_OTG1_PWR |      -        | USDHC1_RESET_B|   GPIO1_IO04  | ENET2_EVT0_IN |   UART5_TX    |               |
 * |   LCD_DATA02     | LCDIF_DATA02  |    PWM3_OUT   |      -        | ENET1_EVT3_IN |   I2C4_SDA    |   GPIO3_IO07  | SRC_BT_CFG02  | SAI1_TX_BCLK  |
 * |   NAND_ALE       |  RAWNAND_ALE  | USDHC2_RESET_B|  QSPI_A_DQS   |    PWM3_OUT   |  EIM_ADDR17   |   GPIO4_IO10  |       -       |   ECSPI3_SS1  |
 * |   GPIO1_IO05     | ENET2_REF_CLK2|    PWM4_OUT   | ANATOP_OTG2_ID|   CSI_FIELD   | USDHC1_VSELECT|   GPIO1_IO05  | ENET2_EVT0_OUT|    UART5_RX   |
 * |   LCD_DATA03     | LCDIF_DATA03  |    PWM4_OUT   |      -        | ENET1_EVT3_OUT|   I2C4_SCL    |   GPIO3_IO08  | SRC_BT_CFG03  | SAI1_RX_BCLK  |
 * |   NAND_WP_B      | RAWNAND_WP_B  | USDHC1_RESET_B|  QSPI_A_SCLK  |    PWM4_OUT   |  EIM_BCLK     |   GPIO4_IO11  |       -       |   ECSPI3_RDY  |
 * |   ENET1_TX_DATA1 | ENET1_TDATA01 |  UART6_CTS_B  |   PWM5_OUT    |  CSI_DATA20   |  ENET2_MDIO   |   GPIO2_IO04  |   KPP_ROW02   |WDOG1_RST_B_DEB|
 * |   LCD_DATA18     | LCDIF_DATA18  |    PWM5_OUT   |CA7_MX6UL_EVTO |   CSI_DATA10  |   EIM_DATA10  |   GPIO3_IO23  |  SRC_BT_CFG26 |  USDHC2_CMD   |
 * |   NAND_DQS       | RAWNAND_DQS   |  CSI_FIELD    |  QSPI_A_SS0_B |    PWM5_OUT   |   EIM_WAIT    |   GPIO4_IO16  | SDMA_EXT_EVT01| SPDIF_EXT_CLK |
 * |   ENET1_TX_EN    | ENET1_TX_EN   |  UART6_RTS_B  |   PWM6_OUT    |  CSI_DATA21   |  ENET2_MDC    |   GPIO2_IO05  |   KPP_COL02   |WDOG2_RST_B_DEB|
 * |   JTAG_TDI       |   SJC_TDI     | GPT2_COMPARE1 | SAI2_TX_BCLK  |      -        |  PWM6_OUT     |   GPIO1_IO13  |   MQS_LEFT    |SIM1_POWER_FAIL|
 * |   LCD_DATA19     | LCDIF_DATA19  |    PWM6_OUT   |WDOG1_WDOG_ANY |   CSI_DATA11  |   EIM_DATA11  |   GPIO3_IO24  |  SRC_BT_CFG27 |  USDHC2_CLK   |
 * |   CSI_VSYNC      |   CSI_VSYNC   |  USDHC2_CLK   |SIM1_PORT1_CLK |   I2C2_SDA    |   EIM_RW      |   GPIO4_IO19  |   PWM7_OUT    |  UART6_RTS_B  |
 * |   ENET1_TX_CLK   | ENET1_TX_CLK  |  UART7_CTS_B  |   PWM7_OUT    |  CSI_DATA22   | ENET1_REF_CLK1|   GPIO2_IO06  |   KPP_ROW03   |   GPT1_CLK    |
 * |   JTAG_TCK       |   SJC_TCK     | GPT2_COMPARE2 | SAI2_TX_DATA  |      -        |  PWM7_OUT     |   GPIO1_IO14  |      -        |SIM2_POWER_FAIL|
 * |   CSI_HSYNC      |   CSI_HSYNC   |  USDHC2_CMD   | SIM1_PORT1_PD |   I2C2_SCL    |   EIM_LBA_B   |   GPIO4_IO20  |   PWM8_OUT    |  UART6_CTS_B  |
 * |   ENET1_RX_ER    | ENET1_RX_ER   |  UART8_RTS_B  |SIM2_PORT0_SVEN|  ECSPI4_SS0   |   EIM_ADDR25  |   GPIO2_IO15  |   KPP_COL07   | WDOG1_WDOG_ANY|
 * |   JTAG_TRST_B    |   SJC_TRSTB   | GPT2_COMPARE3 | SAI2_TX_DATA  |      -        |  PWM8_OUT     |   GPIO1_IO15  |      -        |CAAM_RNG_OSC_OBS|
 *
 */


/** \brief 引脚用于PWM功能的GPIO配置 */
#define __PWM_PIN_FUNC_CFG(x)  (IMX6UL_PIN_MUX(x))

/** \brief PWM禁能时GPIO的配置 */
#define __PWM_PIN_DFUNC_CFG   (IMX6UL_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

/* imx6ul中PWM对应的GPIO */
#define IMX6UL_PWM1_GPIO          GPIO1_8
#define IMX6UL_PWM2_GPIO          GPIO1_9
#define IMX6UL_PWM3_GPIO          GPIO1_4
#define IMX6UL_PWM4_GPIO          GPIO1_5
#define IMX6UL_PWM5_GPIO          GPIO3_23
#define IMX6UL_PWM6_GPIO          GPIO3_24
#define IMX6UL_PWM7_GPIO          GPIO4_20
#define IMX6UL_PWM8_GPIO          GPIO2_15

/* imx6ul中PWM对应的GPIO的PWM复用配置  */
#define IMX6UL_GPIO1_8_MUX_PWM1   __PWM_PIN_FUNC_CFG(PIN_MUX_ALT0)
#define IMX6UL_GPIO1_9_MUX_PWM2   __PWM_PIN_FUNC_CFG(PIN_MUX_ALT0)
#define IMX6UL_GPIO1_4_MUX_PWM3   __PWM_PIN_FUNC_CFG(PIN_MUX_ALT1)
#define IMX6UL_GPIO1_5_MUX_PWM4   __PWM_PIN_FUNC_CFG(PIN_MUX_ALT1)
#define IMX6UL_GPIO3_23_MUX_PWM5   __PWM_PIN_FUNC_CFG(PIN_MUX_ALT1)
#define IMX6UL_GPIO3_24_MUX_PWM6  __PWM_PIN_FUNC_CFG(PIN_MUX_ALT1)
#define IMX6UL_GPIO4_20_MUX_PWM7  __PWM_PIN_FUNC_CFG(PIN_MUX_ALT6)
#define IMX6UL_GPIO2_15_MUX_PWM8  __PWM_PIN_FUNC_CFG(PIN_MUX_ALT4)

aw_local awbl_imx6ul_pwm_devcfg_t __g_imx6ul_pwm_devcfg_list[] = {
    /* PWM ID , GPIO, PWM引脚功能, 默认引脚功能, PWM通道特性 */
    {PWM1,IMX6UL_PWM1_GPIO, IMX6UL_GPIO1_8_MUX_PWM1,  __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM2,IMX6UL_PWM2_GPIO, IMX6UL_GPIO1_9_MUX_PWM2,  __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM3,IMX6UL_PWM3_GPIO, IMX6UL_GPIO1_4_MUX_PWM3,  __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM4,IMX6UL_PWM4_GPIO, IMX6UL_GPIO1_5_MUX_PWM4,  __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM5,IMX6UL_PWM5_GPIO, IMX6UL_GPIO3_23_MUX_PWM5, __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM6,IMX6UL_PWM6_GPIO, IMX6UL_GPIO3_24_MUX_PWM6, __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM7,IMX6UL_PWM7_GPIO, IMX6UL_GPIO4_20_MUX_PWM7, __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT},
    {PWM8,IMX6UL_PWM8_GPIO, IMX6UL_GPIO2_15_MUX_PWM8, __PWM_PIN_DFUNC_CFG, AWBL_IMX6UL_PWM_CFG_DEFAULT}
};

aw_local awbl_imx6ul_pwm_devinfo_t __g_imx6ul_pwm_devinfo = {
    {
            PWM1,
            PWM8
    },
    &__g_imx6ul_pwm_devcfg_list[0],
    8,
};

/* 设备实例内存静态分配 */
aw_local awbl_imx6ul_pwm_dev_t __g_imx6ul_pwm;

#define AWBL_HWCONF_IMX6UL_PWM      \
    {                               \
        AWBL_IMX6UL_PWM_NAME,       \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_pwm.super),    \
        &(__g_imx6ul_pwm_devinfo)   \
    },

#else

#define AWBL_HWCONF_IMX6UL_PWM

#endif

