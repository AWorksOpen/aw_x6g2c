/*******************************************************************************
 *                                 AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn/
 *******************************************************************************/

#include "apollo.h"
#include "imx6ul_pin_cfg_map.h"
#include "imx6ul_iomuxc_regs.h"

// GPIO0
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_0 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO00),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO00),
    {
        //I2C2_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SCL_SELECT_INPUT),0x1},
        //GPT1_CAPTURE1
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CAPTURE1_SELECT_INPUT),0x0},
        //ANATOP_OTG1_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG1_ID_SELECT_INPUT),0x0},
        //ENET1_REF_CLK1
        {AW_OFFSET(hw_iomuxc_regs_t,ENET1_REF_CLK1_SELECT_INPUT),0x0},
        //MQS_RIGHT
        {0x0,0x0},
        //GPIO1_0
        {0x0,0x0},
        //ENET1_1588_EVENT0_IN
        {0x0,0x0},
        //SRC_SYSTEM_RESET
        {0x0,0x0},
        //WDOG3_WDOG_B
        {0x0,0x0},
    }
};
// GPIO1
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_1 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO01),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO01),
    {
        //I2C2_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SDA_SELECT_INPUT),0x1},
        //GPT1_COMPARE1
        {0x0,0x0},
        //USB_OTG1_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG_OC_SELECT_INPUT),0x0},
        //ENET2_REF_CLK2
        {AW_OFFSET(hw_iomuxc_regs_t,ENET2_REF_CLK2_SELECT_INPUT),0x0},
        //MQS_LEFT
        {0x0,0x0},
        //GPIO1_1
        {0x0,0x0},
        //ENET1_1588_EVENT0_OUT
        {0x0,0x0},
        //SRC_EARLY_RESET
        {0x0,0x0},
        //WDOG1_WDOG_B
        {0x0,0x0},
    }
};
// GPIO2
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_2 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO02),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO02),
    {
        //I2C1_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SCL_SELECT_INPUT),0x0},
        //GPT1_COMPARE2
        {0x0,0x0},
        //USB_OTG2_PWR
        {0x0,0x0},
        //ENET1_REF_CLK_25M
        {0x0,0x0},
        //USDHC1_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_WP_SELECT_INPUT),0x0},
        //GPIO1_2
        {0x0,0x0},
        //SDMA_EXT_EVENT00
        {AW_OFFSET(hw_iomuxc_regs_t,SDMA_EVENTS0_SELECT_INPUT),0x1},
        //SRC_ANY_PU_RESET
        {0x0,0x0},
        //UART1_TX
        {0x0,0x0},
    }
};
// GPIO3
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_3 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO03),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO03),
    {
        //I2C1_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SDA_SELECT_INPUT),0x1},
        //GPT1_COMPARE3
        {0x0,0x0},
        //USB_OTG2_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_OC_SELECT_INPUT),0x0},
        //OSC32K_32K_OUT
        {0x0,0x0},
        //USDHC1_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_CD_B_SELECT_INPUT),0x0},
        //GPIO1_3
        {0x0,0x0},
        //CCM_DI0_EXT_CLK
        {0x0,0x0},
        //SRC_TESTER_ACK
        {0x0,0x0},
        //UART1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART1_RX_DATA_SELECT_INPUT),0x0},
    }
};
// GPIO4
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_4 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO04),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO04),
    {
        //ENET1_REF_CLK1
        {AW_OFFSET(hw_iomuxc_regs_t,ENET1_REF_CLK1_SELECT_INPUT),0x1},
        //PWM3_OUT
        {0x0,0x0},
        //USB_OTG1_PWR
        {0x0,0x0},
        //ANATOP_24M_OUT
        {0x0,0x0},
        //USDHC1_RESET_B
        {0x0,0x0},
        //GPIO1_4
        {0x0,0x0},
        //ENET2_1588_EVENT0_IN
        {0x0,0x0},
        {0x0,0x0},
        //UART5_TX
        {0x0,0x0},
    }
};
// GPIO5
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_5 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO05),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO05),
    {
        //ENET2_REF_CLK2
        {AW_OFFSET(hw_iomuxc_regs_t,ENET2_REF_CLK2_SELECT_INPUT),0x1},
        //PWM4_OUT
        {0x0,0x0},
        //ANATOP_OTG2_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_ID_SELECT_INPUT),0x0},
        //CSI_FIELD
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_FIELD_SELECT_INPUT),0x0},
        //USDHC1_VSELECT
        {0x0,0x0},
        //GPIO1_5
        {0x0,0x0},
        //ENET2_1588_EVENT0_OUT
        {0x0,0x0},
        {0x0,0x0},
        //UART5_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RX_DATA_SELECT_INPUT),0x3},
    }
};
// GPIO6
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_6 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO06),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO06),
    {
        //ENET1_MDIO
        {AW_OFFSET(hw_iomuxc_regs_t,ENET1_MAC0_MDIO_SELECT_INPUT),0x0},
        //ENET2_MDIO
        {AW_OFFSET(hw_iomuxc_regs_t,ENET2_MAC0_MDIO_SELECT_INPUT),0x0},
        //USB_OTG_PWR_WAKE
        {0x0,0x0},
        //CSI_MCLK
        {0x0,0x0},
        //USDHC2_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_WP_SELECT_INPUT),0x0},
        //GPIO1_6
        {0x0,0x0},
        //CCM_WAIT
        {0x0,0x0},
        //CCM_REF_EN_B
        {0x0,0x0},
        //UART1_CTS_B
        {0x0,0x0},
    }
};
// GPIO7
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_7 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO07),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO07),
    {
        //ENET1_MDC
        {0x0,0x0},
        //ENET2_MDC
        {0x0,0x0},
        //USB_OTG_HOST_MODE
        {0x0,0x0},
        //CSI_PIXCLK
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_PIXCLK_SELECT_INPUT),0x0},
        //USDHC2_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CD_B_SELECT_INPUT),0x1},
        //GPIO1_7
        {0x0,0x0},
        //CCM_STOP
        {0x0,0x0},
        {0x0,0x0},
        //UART1_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART1_RTS_B_SELECT_INPUT),0x1},
    }
};
// GPIO8
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_8 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO08),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO08),
    {
        //PWM1_OUT
        {0x0,0x0},
        //WDOG1_WDOG_B
        {0x0,0x0},
        //SPDIF_OUT
        {0x0,0x0},
        //CSI_VSYNC
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_VSYNC_SELECT_INPUT),0x1},
        //USDHC2_VSELECT
        {0x0,0x0},
        //GPIO1_8
        {0x0,0x0},
        //CCM_PMIC_RDY
        {AW_OFFSET(hw_iomuxc_regs_t,CCM_PMIC_READY_SELECT_INPUT),0x1},
        {0x0,0x0},
        //UART5_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RTS_B_SELECT_INPUT),0x1},
    }
};
// GPIO9
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_9 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO1_IO09),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO1_IO09),
    {
        //PWM2_OUT
        {0x0,0x0},
        //WDOG1_WDOG_ANY
        {0x0,0x0},
        //SPDIF_IN
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_IN_SELECT_INPUT),0x0},
        //CSI_HSYNC
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_HSYNC_SELECT_INPUT),0x1},
        //USDHC2_RESET_B
        {0x0,0x0},
        //GPIO1_9
        {0x0,0x0},
        //USDHC1_RESET_B
        {0x0,0x0},
        {0x0,0x0},
        //UART5_CTS_B
        {0x0,0x0},
    }
};
// GPIO10
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_10 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_MOD),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_MOD),
    {
        //SJC_MOD
        {0x0,0x0},
        //GPT2_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CLK_SELECT_INPUT),0x0},
        //SPDIF_OUT
        {0x0,0x0},
        //ENET1_REF_CLK_25M
        {0x0,0x0},
        //CCM_PMIC_RDY
        {AW_OFFSET(hw_iomuxc_regs_t,CCM_PMIC_READY_SELECT_INPUT),0x0},
        //GPIO1_10
        {0x0,0x0},
        //SDMA_EXT_EVENT00
        {AW_OFFSET(hw_iomuxc_regs_t,SDMA_EVENTS0_SELECT_INPUT),0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO11
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_11 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_TMS),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_TMS),
    {
        //SJC_TMS
        {0x0,0x0},
        //GPT2_CAPTURE1
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CAPTURE1_SELECT_INPUT),0x0},
        //SAI2_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_MCLK_SELECT_INPUT),0x0},
        //CCM_CLKO1
        {0x0,0x0},
        //CCM_WAIT
        {0x0,0x0},
        //GPIO1_11
        {0x0,0x0},
        //SDMA_EXT_EVENT01
        {AW_OFFSET(hw_iomuxc_regs_t,SDMA_EVENTS1_SELECT_INPUT),0x0},
        {0x0,0x0},
        //EPIT1_OUT
        {0x0,0x0},
    }
};
// GPIO12
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_12 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_TDO),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_TDO),
    {
        //SJC_TDO
        {0x0,0x0},
        //GPT2_CAPTURE2
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CAPTURE2_SELECT_INPUT),0x0},
        //SAI2_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_TX_SYNC_SELECT_INPUT),0x0},
        //CCM_CLKO2
        {0x0,0x0},
        //CCM_STOP
        {0x0,0x0},
        //GPIO1_12
        {0x0,0x0},
        //MQS_RIGHT
        {0x0,0x0},
        {0x0,0x0},
        //EPIT2_OUT
        {0x0,0x0},
    }
};
// GPIO13
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_13 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_TDI),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_TDI),
    {
        //SJC_TDI
        {0x0,0x0},
        //GPT2_COMPARE1
        {0x0,0x0},
        //SAI2_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_TX_BCLK_SELECT_INPUT),0x0},
        //PWM6_OUT
        {0x0,0x0},
        //CCM_STOP
        {0x0,0x0},
        //GPIO1_13
        {0x0,0x0},
        //MQS_LEFT
        {0x0,0x0},
        {0x0,0x0},
        //SIM1_POWER_FAIL
        {0x0,0x0},
    }
};
// GPIO14
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_14 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_TCK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_TCK),
    {
        //SJC_TCK
        {0x0,0x0},
        //GPT2_COMPARE2
        {0x0,0x0},
        //SAI2_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_RX_DATA_SELECT_INPUT),0x0},
        //PWM7_OUT
        {0x0,0x0},
        //CCM_STOP
        {0x0,0x0},
        //GPIO1_14
        {0x0,0x0},
        //OSC32K_32K_OUT
        {0x0,0x0},
        {0x0,0x0},
        //SIM2_POWER_FAIL
        {0x0,0x0},
    }
};
// GPIO15
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_15 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_JTAG_TRST_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_JTAG_TRST_B),
    {
        //SJC_TRSTB
        {0x0,0x0},
        //GPT2_COMPARE3
        {0x0,0x0},
        //SAI2_TX_DATA
        {0x0,0x0},
        {0x0,0x0},
        //PWM8_OUT
        {0x0,0x0},
        //GPIO1_15
        {0x0,0x0},
        //ANATOP_24M_OUT
        {0x0,0x0},
        {0x0,0x0},
        //CAAM_RNG_OSC_OBS
        {0x0,0x0},
    }
};
// GPIO16
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_16 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART1_TX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART1_TX_DATA),
    {
        //UART1_TX
        {0x0,0x0},
        //ENET1_RDATA02
        {0x0,0x0},
        //I2C3_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SCL_SELECT_INPUT),0x0},
        //CSI_DATA02
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA02_SELECT_INPUT),0x1},
        //GPT1_COMPARE1
        {0x0,0x0},
        //GPIO1_16
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //SPDIF_OUT
        {0x0,0x0},
    }
};
// GPIO17
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_17 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART1_RX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART1_RX_DATA),
    {
        //UART1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART1_RX_DATA_SELECT_INPUT),0x3},
        //ENET1_RDATA03
        {0x0,0x0},
        //I2C3_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SDA_SELECT_INPUT),0x0},
        //CSI_DATA03
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA03_SELECT_INPUT),0x1},
        //GPT1_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CLK_SELECT_INPUT),0x0},
        //GPIO1_17
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //SPDIF_IN
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_IN_SELECT_INPUT),0x1},
    }
};
// GPIO18
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_18 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART1_CTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART1_CTS_B),
    {
        //UART1_CTS_B
        {0x0,0x0},
        //ENET1_RX_CLK
        {0x0,0x0},
        //USDHC1_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_WP_SELECT_INPUT),0x1},
        //CSI_DATA04
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA04_SELECT_INPUT),0x0},
        //ENET2_1588_EVENT1_IN
        {0x0,0x0},
        //GPIO1_18
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_WP_SELECT_INPUT),0x1},
    }
};
// GPIO19
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_19 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART1_RTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART1_RTS_B),
    {
        //UART1_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART1_RTS_B_SELECT_INPUT),0x3},
        //ENET1_TX_ER
        {0x0,0x0},
        //USDHC1_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_CD_B_SELECT_INPUT),0x1},
        //CSI_DATA05
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA05_SELECT_INPUT),0x1},
        //ENET2_1588_EVENT1_OUT
        {0x0,0x0},
        //GPIO1_19
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CD_B_SELECT_INPUT),0x2},
    }
};
// GPIO20
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_20 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART2_TX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART2_TX_DATA),
    {
        //UART2_TX
        {0x0,0x0},
        //ENET1_TDATA02
        {0x0,0x0},
        //I2C4_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SCL_SELECT_INPUT),0x0},
        //CSI_DATA06
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA06_SELECT_INPUT),0x0},
        //GPT1_CAPTURE1
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CAPTURE1_SELECT_INPUT),0x1},
        //GPIO1_20
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI3_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_SS0_B_SELECT_INPUT),0x0},
    }
};
// GPIO21
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_21 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART2_RX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART2_RX_DATA),
    {
        //UART2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART2_RX_DATA_SELECT_INPUT),0x1},
        //ENET1_TDATA03
        {0x0,0x0},
        //I2C4_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SDA_SELECT_INPUT),0x0},
        //CSI_DATA07
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA07_SELECT_INPUT),0x0},
        //GPT1_CAPTURE2
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CAPTURE2_SELECT_INPUT),0x0},
        //GPIO1_21
        {0x0,0x0},
        {0x0,0x0},
        //SJC_DONE
        {0x0,0x0},
        //ECSPI3_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_SCLK_SELECT_INPUT),0x0},
    }
};
// GPIO22
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_22 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART2_CTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART2_CTS_B),
    {
        //UART2_CTS_B
        {0x0,0x0},
        //ENET1_CRS
        {0x0,0x0},
        //FLEXCAN2_TX
        {0x0,0x0},
        //CSI_DATA08
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA08_SELECT_INPUT),0x0},
        //GPT1_COMPARE2
        {0x0,0x0},
        //GPIO1_22
        {0x0,0x0},
        {0x0,0x0},
        //SJC_DE_B
        {0x0,0x0},
        //ECSPI3_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_MOSI_SELECT_INPUT),0x0},
    }
};
// GPIO23
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_23 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART2_RTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART2_RTS_B),
    {
        //UART2_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART2_RTS_B_SELECT_INPUT),0x1},
        //ENET1_COL
        {0x0,0x0},
        //FLEXCAN2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN2_RX_SELECT_INPUT),0x0},
        //CSI_DATA09
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA09_SELECT_INPUT),0x0},
        //GPT1_COMPARE3
        {0x0,0x0},
        //GPIO1_23
        {0x0,0x0},
        {0x0,0x0},
        //SJC_FAIL
        {0x0,0x0},
        //ECSPI3_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_MISO_SELECT_INPUT),0x0},
    }
};
// GPIO24
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_24 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART3_TX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART3_TX_DATA),
    {
        //UART3_TX
        {0x0,0x0},
        //ENET2_RDATA02
        {0x0,0x0},
        //SIM1_PORT0_PD
        {0x0,0x0},
        //CSI_DATA01
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA01_SELECT_INPUT),0x0},
        //UART2_CTS_B
        {0x0,0x0},
        //GPIO1_24
        {0x0,0x0},
        {0x0,0x0},
        //SJC_JTAG_ACT
        {0x0,0x0},
        //ANATOP_OTG1_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG1_ID_SELECT_INPUT),0x1},
    }
};
// GPIO25
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_25 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART3_RX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART3_RX_DATA),
    {
        //UART3_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART3_RX_DATA_SELECT_INPUT),0x1},
        //ENET2_RDATA03
        {0x0,0x0},
        //SIM2_PORT0_PD
        {0x0,0x0},
        //CSI_DATA00
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA00_SELECT_INPUT),0x0},
        //UART2_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART2_RTS_B_SELECT_INPUT),0x3},
        //GPIO1_25
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //EPIT1_OUT
        {0x0,0x0},
    }
};
// GPIO26
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_26 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART3_CTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART3_CTS_B),
    {
        //UART3_CTS_B
        {0x0,0x0},
        //ENET2_RX_CLK
        {0x0,0x0},
        //FLEXCAN1_TX
        {0x0,0x0},
        //CSI_DATA10
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA10_SELECT_INPUT),0x0},
        //ENET1_1588_EVENT1_IN
        {0x0,0x0},
        //GPIO1_26
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //EPIT2_OUT
        {0x0,0x0},
    }
};
// GPIO27
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_27 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART3_RTS_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART3_RTS_B),
    {
        //UART3_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART3_RTS_B_SELECT_INPUT),0x1},
        //ENET2_TX_ER
        {0x0,0x0},
        //FLEXCAN1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN1_RX_SELECT_INPUT),0x0},
        //CSI_DATA11
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA11_SELECT_INPUT),0x0},
        //ENET1_1588_EVENT1_OUT
        {0x0,0x0},
        //GPIO1_27
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //WDOG1_WDOG_B
        {0x0,0x0},
    }
};
// GPIO28
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_28 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART4_TX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART4_TX_DATA),
    {
        //UART4_TX
        {0x0,0x0},
        //ENET2_TDATA02
        {0x0,0x0},
        //I2C1_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SCL_SELECT_INPUT),0x1},
        //CSI_DATA12
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA12_SELECT_INPUT),0x0},
        //CSU_CSU_ALARM_AUT02
        {0x0,0x0},
        //GPIO1_28
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_SCLK_SELECT_INPUT),0x1},
    }
};
// GPIO29
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_29 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART4_RX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART4_RX_DATA),
    {
        //UART4_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART4_RX_DATA_SELECT_INPUT),0x1},
        //ENET2_TDATA03
        {0x0,0x0},
        //I2C1_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SDA_SELECT_INPUT),0x2},
        //CSI_DATA13
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA13_SELECT_INPUT),0x0},
        //CSU_CSU_ALARM_AUT01
        {0x0,0x0},
        //GPIO1_29
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_SS0_B_SELECT_INPUT),0x1},
    }
};
// GPIO30
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_30 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART5_TX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART5_TX_DATA),
    {
        //UART5_TX
        {0x0,0x0},
        //ENET2_CRS
        {0x0,0x0},
        //I2C2_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SCL_SELECT_INPUT),0x2},
        //CSI_DATA14
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA14_SELECT_INPUT),0x0},
        //CSU_CSU_ALARM_AUT00
        {0x0,0x0},
        //GPIO1_30
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_MOSI_SELECT_INPUT),0x0},
    }
};
// GPIO31
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_31 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_UART5_RX_DATA),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_UART5_RX_DATA),
    {
        //UART5_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RX_DATA_SELECT_INPUT),0x5},
        //ENET2_COL
        {0x0,0x0},
        //I2C2_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SDA_SELECT_INPUT),0x2},
        //CSI_DATA15
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA15_SELECT_INPUT),0x0},
        //CSU_CSU_INT_DEB
        {0x0,0x0},
        //GPIO1_31
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_MISO_SELECT_INPUT),0x1},
    }
};
// GPIO32
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_32 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_RX_DATA0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_RX_DATA0),
    {
        //ENET1_RDATA00
        {0x0,0x0},
        //UART4_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART4_RTS_B_SELECT_INPUT),0x0},
        //PWM1_OUT
        {0x0,0x0},
        //CSI_DATA16
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA16_SELECT_INPUT),0x0},
        //FLEXCAN1_TX
        {0x0,0x0},
        //GPIO2_0
        {0x0,0x0},
        //KPP_ROW00
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW0_SELECT_INPUT),0x0},
        {0x0,0x0},
        //USDHC1_LCTL
        {0x0,0x0},
    }
};
// GPIO33
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_33 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_RX_DATA1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_RX_DATA1),
    {
        //ENET1_RDATA01
        {0x0,0x0},
        //UART4_CTS_B
        {0x0,0x0},
        //PWM2_OUT
        {0x0,0x0},
        //CSI_DATA17
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA17_SELECT_INPUT),0x0},
        //FLEXCAN1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN1_RX_SELECT_INPUT),0x1},
        //GPIO2_1
        {0x0,0x0},
        //KPP_COL00
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL0_SELECT_INPUT),0x0},
        {0x0,0x0},
        //USDHC2_LCTL
        {0x0,0x0},
    }
};
// GPIO34
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_34 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_RX_EN),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_RX_EN),
    {
        //ENET1_RX_EN
        {0x0,0x0},
        //UART5_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RTS_B_SELECT_INPUT),0x3},
        //OSC32K_32K_OUT
        {0x0,0x0},
        //CSI_DATA18
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA18_SELECT_INPUT),0x0},
        //FLEXCAN2_TX
        {0x0,0x0},
        //GPIO2_2
        {0x0,0x0},
        //KPP_ROW01
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW1_SELECT_INPUT),0x0},
        {0x0,0x0},
        //USDHC1_VSELECT
        {0x0,0x0},
    }
};
// GPIO35
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_35 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_TX_DATA0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_TX_DATA0),
    {
        //ENET1_TDATA00
        {0x0,0x0},
        //UART5_CTS_B
        {0x0,0x0},
        //ANATOP_24M_OUT
        {0x0,0x0},
        //CSI_DATA19
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA19_SELECT_INPUT),0x0},
        //FLEXCAN2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN2_RX_SELECT_INPUT),0x1},
        //GPIO2_3
        {0x0,0x0},
        //KPP_COL01
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL1_SELECT_INPUT),0x0},
        {0x0,0x0},
        //USDHC2_VSELECT
        {0x0,0x0},
    }
};
// GPIO36
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_36 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_TX_DATA1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_TX_DATA1),
    {
        //ENET1_TDATA01
        {0x0,0x0},
        //UART6_CTS_B
        {0x0,0x0},
        //PWM5_OUT
        {0x0,0x0},
        //CSI_DATA20
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA20_SELECT_INPUT),0x0},
        //ENET2_MDIO
        {AW_OFFSET(hw_iomuxc_regs_t,ENET2_MAC0_MDIO_SELECT_INPUT),0x1},
        //GPIO2_4
        {0x0,0x0},
        //KPP_ROW02
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW2_SELECT_INPUT),0x0},
        {0x0,0x0},
        //WDOG1_WDOG_RST_B_DEB
        {0x0,0x0},
    }
};
// GPIO37
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_37 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_TX_EN),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_TX_EN),
    {
        //ENET1_TX_EN
        {0x0,0x0},
        //UART6_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART6_RTS_B_SELECT_INPUT),0x3},
        //PWM6_OUT
        {0x0,0x0},
        //CSI_DATA21
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA21_SELECT_INPUT),0x0},
        //ENET2_MDC
        {0x0,0x0},
        //GPIO2_5
        {0x0,0x0},
        //KPP_COL02
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL2_SELECT_INPUT),0x0},
        {0x0,0x0},
        //WDOG2_WDOG_RST_B_DEB
        {0x0,0x0},
    }
};
// GPIO38
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_38 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_TX_CLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_TX_CLK),
    {
        //ENET1_TX_CLK
        {0x0,0x0},
        //UART7_CTS_B
        {0x0,0x0},
        //PWM7_OUT
        {0x0,0x0},
        //CSI_DATA22
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA22_SELECT_INPUT),0x0},
        //ENET1_REF_CLK1
        {AW_OFFSET(hw_iomuxc_regs_t,ENET1_REF_CLK1_SELECT_INPUT),0x2},
        //GPIO2_6
        {0x0,0x0},
        //KPP_ROW03
        {0x0,0x0},
        {0x0,0x0},
        //GPT1_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CLK_SELECT_INPUT),0x1},
    }
};
// GPIO39
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_39 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET1_RX_ER),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET1_RX_ER),
    {
        //ENET1_RX_ER
        {0x0,0x0},
        //UART7_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART7_RTS_B_SELECT_INPUT),0x1},
        //PWM8_OUT
        {0x0,0x0},
        //CSI_DATA23
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA23_SELECT_INPUT),0x0},
        //EIM_CRE
        {0x0,0x0},
        //GPIO2_7
        {0x0,0x0},
        //KPP_COL03
        {0x0,0x0},
        {0x0,0x0},
        //GPT1_CAPTURE2
        {AW_OFFSET(hw_iomuxc_regs_t,GPT1_CAPTURE2_SELECT_INPUT),0x1},
    }
};
// GPIO40
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_40 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_RX_DATA0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_RX_DATA0),
    {
        //ENET2_RDATA00
        {0x0,0x0},
        //UART6_TX
        {0x0,0x0},
        //SIM1_PORT0_TRXD
        {0x0,0x0},
        //I2C3_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SCL_SELECT_INPUT),0x1},
        //ENET1_MDIO
        {0x0,0x0},
        //GPIO2_8
        {0x0,0x0},
        //KPP_ROW04
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG1_PWR
        {0x0,0x0},
    }
};
// GPIO41
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_41 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_RX_DATA1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_RX_DATA1),
    {
        //ENET2_RDATA01
        {0x0,0x0},
        //UART6_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART6_RX_DATA_SELECT_INPUT),0x2},
        //SIM1_PORT0_CLK
        {0x0,0x0},
        //I2C3_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SDA_SELECT_INPUT),0x1},
        //ENET1_MDC
        {0x0,0x0},
        //GPIO2_9
        {0x0,0x0},
        //KPP_COL04
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG1_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG_OC_SELECT_INPUT),0x1},
    }
};
// GPIO42
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_42 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_RX_EN),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_RX_EN),
    {
        //ENET2_RX_EN
        {0x0,0x0},
        //UART7_TX
        {0x0,0x0},
        //SIM1_PORT0_RST_B
        {0x0,0x0},
        //I2C4_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SCL_SELECT_INPUT),0x1},
        //EIM_ADDR26
        {0x0,0x0},
        //GPIO2_10
        {0x0,0x0},
        //KPP_ROW05
        {0x0,0x0},
        {0x0,0x0},
        //ENET1_REF_CLK_25M
        {0x0,0x0},
    }
};
// GPIO43
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_43 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_TX_DATA0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_TX_DATA0),
    {
        //ENET2_TDATA00
        {0x0,0x0},
        //UART7_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART7_RX_DATA_SELECT_INPUT),0x1},
        //SIM1_PORT0_SVEN
        {0x0,0x0},
        //I2C4_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SDA_SELECT_INPUT),0x1},
        //EIM_EB_B02
        {0x0,0x0},
        //GPIO2_11
        {0x0,0x0},
        //KPP_COL05
        {0x0,0x0},
        {0x0,0x0},
        //ANATOP_24M_OUT
        {0x0,0x0},
    }
};
// GPIO44
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_44 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_TX_DATA1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_TX_DATA1),
    {
        //ENET2_TDATA01
        {0x0,0x0},
        //UART8_TX
        {0x0,0x0},
        //SIM2_PORT0_TRXD
        {0x0,0x0},
        //ECSPI4_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_SCLK_SELECT_INPUT),0x0},
        //EIM_EB_B03
        {0x0,0x0},
        //GPIO2_12
        {0x0,0x0},
        //KPP_ROW06
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG2_PWR
        {0x0,0x0},
    }
};
// GPIO45
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_45 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_TX_EN),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_TX_EN),
    {
        //ENET2_TX_EN
        {0x0,0x0},
        //UART8_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART8_RX_DATA_SELECT_INPUT),0x1},
        //SIM2_PORT0_CLK
        {0x0,0x0},
        //ECSPI4_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_MOSI_SELECT_INPUT),0x0},
        //EIM_ACLK_FREERUN
        {0x0,0x0},
        //GPIO2_13
        {0x0,0x0},
        //KPP_COL06
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG2_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_OC_SELECT_INPUT),0x1},
    }
};
// GPIO46
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_46 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_TX_CLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_TX_CLK),
    {
        //ENET2_TX_CLK
        {0x0,0x0},
        //UART8_CTS_B
        {0x0,0x0},
        //SIM2_PORT0_RST_B
        {0x0,0x0},
        //ECSPI4_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_MISO_SELECT_INPUT),0x0},
        //ENET2_REF_CLK2
        {AW_OFFSET(hw_iomuxc_regs_t,ENET2_REF_CLK2_SELECT_INPUT),0x2},
        //GPIO2_14
        {0x0,0x0},
        //KPP_ROW07
        {0x0,0x0},
        {0x0,0x0},
        //ANATOP_OTG2_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_ID_SELECT_INPUT),0x1},
    }
};
// GPIO47
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_47 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_ENET2_RX_ER),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_ENET2_RX_ER),
    {
        //ENET2_RX_ER
        {0x0,0x0},
        //UART8_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART8_RTS_B_SELECT_INPUT),0x1},
        //SIM2_PORT0_SVEN
        {0x0,0x0},
        //ECSPI4_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_SS0_B_SELECT_INPUT),0x0},
        //EIM_ADDR25
        {0x0,0x0},
        //GPIO2_15
        {0x0,0x0},
        //KPP_COL07
        {0x0,0x0},
        {0x0,0x0},
        //WDOG1_WDOG_ANY
        {0x0,0x0},
    }
};
// GPIO48
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_48 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_CMD),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_CMD),
    {
        //USDHC1_CMD
        {0x0,0x0},
        //GPT2_COMPARE1
        {0x0,0x0},
        //SAI2_RX_SYNC
        {0x0,0x0},
        //SPDIF_OUT
        {0x0,0x0},
        //EIM_ADDR19
        {0x0,0x0},
        //GPIO2_16
        {0x0,0x0},
        //SDMA_EXT_EVENT00
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG1_PWR
        {0x0,0x0},
    }
};
// GPIO49
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_49 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_CLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_CLK),
    {
        //USDHC1_CLK
        {0x0,0x0},
        //GPT2_COMPARE2
        {0x0,0x0},
        //SAI2_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_MCLK_SELECT_INPUT),0x1},
        //SPDIF_IN
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_IN_SELECT_INPUT),0x3},
        //EIM_ADDR20
        {0x0,0x0},
        //GPIO2_17
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG1_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_OC_SELECT_INPUT),0x2},
    }
};
// GPIO50
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_50 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_DATA0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_DATA0),
    {
        //USDHC1_DATA0
        {0x0,0x0},
        //GPT2_COMPARE3
        {0x0,0x0},
        //SAI2_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_TX_SYNC_SELECT_INPUT),0x1},
        //FLEXCAN1_TX
        {0x0,0x0},
        //EIM_ADDR21
        {0x0,0x0},
        //GPIO2_18
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ANATOP_OTG1_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG1_ID_SELECT_INPUT),0x2},
    }
};
// GPIO51
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_51 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_DATA1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_DATA1),
    {
        //USDHC1_DATA1
        {0x0,0x0},
        //GPT2_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CLK_SELECT_INPUT),0x1},
        //SAI2_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_TX_BCLK_SELECT_INPUT),0x1},
        //FLEXCAN1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN1_RX_SELECT_INPUT),0x3},
        //EIM_ADDR22
        {0x0,0x0},
        //GPIO2_19
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG2_PWR
        {0x0,0x0},
    }
};
// GPIO52
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_52 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_DATA2),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_DATA2),
    {
        //USDHC1_DATA2
        {0x0,0x0},
        //GPT2_CAPTURE1
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CAPTURE1_SELECT_INPUT),0x1},
        //SAI2_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI2_RX_DATA_SELECT_INPUT),0x1},
        //FLEXCAN2_TX
        {0x0,0x0},
        //EIM_ADDR23
        {0x0,0x0},
        //GPIO2_20
        {0x0,0x0},
        //CCM_CLKO1
        {0x0,0x0},
        {0x0,0x0},
        //USB_OTG2_OC
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_OC_SELECT_INPUT),0x2},
    }
};
// GPIO53
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_53 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SD1_DATA3),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SD1_DATA3),
    {
        //USDHC1_DATA3
        {0x0,0x0},
        //GPT2_CAPTURE2
        {AW_OFFSET(hw_iomuxc_regs_t,GPT2_CAPTURE2_SELECT_INPUT),0x1},
        //SAI2_TX_DATA
        {0x0,0x0},
        //FLEXCAN2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN2_RX_SELECT_INPUT),0x3},
        //EIM_ADDR24
        {0x0,0x0},
        //GPIO2_21
        {0x0,0x0},
        //CCM_CLKO2
        {0x0,0x0},
        {0x0,0x0},
        //ANATOP_OTG2_ID
        {AW_OFFSET(hw_iomuxc_regs_t,USB_OTG2_ID_SELECT_INPUT),0x2},
    }
};
// GPIO54
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_54 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO55
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_55 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO56
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_56 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO57
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_57 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO58
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_58 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO59
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_59 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO60
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_60 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO61
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_61 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO62
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_62 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO63
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_63 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO64
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_64 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_CLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_CLK),
    {
        //LCDIF_CLK
        {0x0,0x0},
        //LCDIF_WR_RWN
        {0x0,0x0},
        //UART4_TX
        {0x0,0x0},
        //SAI3_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_MCLK_SELECT_INPUT),0x0},
        //EIM_CS2_B
        {0x0,0x0},
        //GPIO3_0
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //WDOG1_WDOG_RST_B_DEB
        {0x0,0x0},
    }
};
// GPIO65
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_65 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_ENABLE),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_ENABLE),
    {
        //LCDIF_ENABLE
        {0x0,0x0},
        //LCDIF_RD_E
        {0x0,0x0},
        //UART4_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART4_RX_DATA_SELECT_INPUT),0x3},
        //SAI3_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_TX_SYNC_SELECT_INPUT),0x0},
        //EIM_CS3_B
        {0x0,0x0},
        //GPIO3_1
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_RDY
        {0x0,0x0},
    }
};
// GPIO66
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_66 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_HSYNC),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_HSYNC),
    {
        //LCDIF_HSYNC
        {0x0,0x0},
        //LCDIF_RS
        {0x0,0x0},
        //UART4_CTS_B
        {0x0,0x0},
        //SAI3_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_TX_BCLK_SELECT_INPUT),0x0},
        //WDOG3_WDOG_RST_B_DEB
        {0x0,0x0},
        //GPIO3_2
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_SS1
        {0x0,0x0},
    }
};
// GPIO67
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_67 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_VSYNC),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_VSYNC),
    {
        //LCDIF_VSYNC
        {0x0,0x0},
        //LCDIF_BUSY
        {AW_OFFSET(hw_iomuxc_regs_t,LCD_BUSY_SELECT_INPUT),0x1},
        //UART4_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART4_RTS_B_SELECT_INPUT),0x3},
        //SAI3_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_RX_DATA_SELECT_INPUT),0x0},
        //WDOG2_WDOG_B
        {0x0,0x0},
        //GPIO3_3
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_SS2
        {0x0,0x0},
    }
};
// GPIO68
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_68 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_RESET),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_RESET),
    {
        //LCDIF_RESET
        {0x0,0x0},
        //LCDIF_CS
        {0x0,0x0},
        //CA7_MX6UL_EVENTI
        {0x0,0x0},
        //SAI3_TX_DATA
        {0x0,0x0},
        //WDOG1_WDOG_ANY
        {0x0,0x0},
        //GPIO3_4
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI2_SS3
        {0x0,0x0},
    }
};
// GPIO69
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_69 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA00),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA00),
    {
        //LCDIF_DATA00
        {0x0,0x0},
        //PWM1_OUT
        {0x0,0x0},
        {0x0,0x0},
        //ENET1_1588_EVENT2_IN
        {0x0,0x0},
        //I2C3_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SDA_SELECT_INPUT),0x2},
        //GPIO3_5
        {0x0,0x0},
        //SRC_BT_CFG00
        {0x0,0x0},
        {0x0,0x0},
        //SAI1_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_MCLK_SELECT_INPUT),0x1},
    }
};
// GPIO70
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_70 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA01),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA01),
    {
        //LCDIF_DATA01
        {0x0,0x0},
        //PWM2_OUT
        {0x0,0x0},
        {0x0,0x0},
        //ENET1_1588_EVENT2_OUT
        {0x0,0x0},
        //I2C3_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C3_SCL_SELECT_INPUT),0x2},
        //GPIO3_6
        {0x0,0x0},
        //SRC_BT_CFG01
        {0x0,0x0},
        {0x0,0x0},
        //SAI1_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_TX_SYNC_SELECT_INPUT),0x0},
    }
};
// GPIO71
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_71 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA02),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA02),
    {
        //LCDIF_DATA02
        {0x0,0x0},
        //PWM3_OUT
        {0x0,0x0},
        {0x0,0x0},
        //ENET1_1588_EVENT3_IN
        {0x0,0x0},
        //I2C4_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SDA_SELECT_INPUT),0x2},
        //GPIO3_7
        {0x0,0x0},
        //SRC_BT_CFG02
        {0x0,0x0},
        {0x0,0x0},
        //SAI1_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_TX_BCLK_SELECT_INPUT),0x1},
    }
};
// GPIO72
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_72 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA03),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA03),
    {
        //LCDIF_DATA03
        {0x0,0x0},
        //PWM4_OUT
        {0x0,0x0},
        {0x0,0x0},
        //ENET1_1588_EVENT3_OUT
        {0x0,0x0},
        //I2C4_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C4_SCL_SELECT_INPUT),0x2},
        //GPIO3_8
        {0x0,0x0},
        //SRC_BT_CFG03
        {0x0,0x0},
        {0x0,0x0},
        //SAI1_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_RX_DATA_SELECT_INPUT),0x0},
    }
};
// GPIO73
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_73 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA04),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA04),
    {
        //LCDIF_DATA04
        {0x0,0x0},
        //UART8_CTS_B
        {0x0,0x0},
        {0x0,0x0},
        //ENET2_1588_EVENT2_IN
        {0x0,0x0},
        //SPDIF_SR_CLK
        {0x0,0x0},
        //GPIO3_9
        {0x0,0x0},
        //SRC_BT_CFG04
        {0x0,0x0},
        {0x0,0x0},
        //SAI1_TX_DATA
        {0x0,0x0},
    }
};
// GPIO74
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_74 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA05),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA05),
    {
        //LCDIF_DATA05
        {0x0,0x0},
        //UART8_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART8_RTS_B_SELECT_INPUT),0x3},
        {0x0,0x0},
        //ENET2_1588_EVENT2_OUT
        {0x0,0x0},
        //SPDIF_OUT
        {0x0,0x0},
        //GPIO3_10
        {0x0,0x0},
        //SRC_BT_CFG05
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI1_SS1
        {0x0,0x0},
    }
};
// GPIO75
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_75 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA06),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA06),
    {
        //LCDIF_DATA06
        {0x0,0x0},
        //UART7_CTS_B
        {0x0,0x0},
        {0x0,0x0},
        //ENET2_1588_EVENT3_IN
        {0x0,0x0},
        //SPDIF_LOCK
        {0x0,0x0},
        //GPIO3_11
        {0x0,0x0},
        //SRC_BT_CFG06
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI1_SS2
        {0x0,0x0},
    }
};
// GPIO76
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_76 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA07),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA07),
    {
        //LCDIF_DATA07
        {0x0,0x0},
        //UART7_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART7_RTS_B_SELECT_INPUT),0x3},
        {0x0,0x0},
        //ENET2_1588_EVENT3_OUT
        {0x0,0x0},
        //SPDIF_EXT_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_EXT_CLK_SELECT_INPUT),0x0},
        //GPIO3_12
        {0x0,0x0},
        //SRC_BT_CFG07
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI1_SS3
        {0x0,0x0},
    }
};
// GPIO77
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_77 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA08),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA08),
    {
        //LCDIF_DATA08
        {0x0,0x0},
        //SPDIF_IN
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_IN_SELECT_INPUT),0x2},
        {0x0,0x0},
        //CSI_DATA16
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA16_SELECT_INPUT),0x1},
        //EIM_DATA00
        {0x0,0x0},
        //GPIO3_13
        {0x0,0x0},
        //SRC_BT_CFG08
        {0x0,0x0},
        {0x0,0x0},
        //FLEXCAN1_TX
        {0x0,0x0},
    }
};
// GPIO78
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_78 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA09),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA09),
    {
        //LCDIF_DATA09
        {0x0,0x0},
        //SAI3_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_MCLK_SELECT_INPUT),0x1},
        {0x0,0x0},
        //CSI_DATA17
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA17_SELECT_INPUT),0x1},
        //EIM_DATA01
        {0x0,0x0},
        //GPIO3_14
        {0x0,0x0},
        //SRC_BT_CFG09
        {0x0,0x0},
        {0x0,0x0},
        //FLEXCAN1_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN1_RX_SELECT_INPUT),0x2},
    }
};
// GPIO79
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_79 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA10),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA10),
    {
        //LCDIF_DATA10
        {0x0,0x0},
        //SAI3_RX_SYNC
        {0x0,0x0},
        {0x0,0x0},
        //CSI_DATA18
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA18_SELECT_INPUT),0x1},
        //EIM_DATA02
        {0x0,0x0},
        //GPIO3_15
        {0x0,0x0},
        //SRC_BT_CFG10
        {0x0,0x0},
        {0x0,0x0},
        //FLEXCAN2_TX
        {0x0,0x0},
    }
};
// GPIO80
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_80 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA11),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA11),
    {
        //LCDIF_DATA11
        {0x0,0x0},
        //SAI3_RX_BCLK
        {0x0,0x0},
        {0x0,0x0},
        //CSI_DATA19
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA19_SELECT_INPUT),0x1},
        //EIM_DATA03
        {0x0,0x0},
        //GPIO3_16
        {0x0,0x0},
        //SRC_BT_CFG11
        {0x0,0x0},
        {0x0,0x0},
        //FLEXCAN2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,FLEXCAN2_RX_SELECT_INPUT),0x2},
    }
};
// GPIO81
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_81 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA12),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA12),
    {
        //LCDIF_DATA12
        {0x0,0x0},
        //SAI3_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_TX_SYNC_SELECT_INPUT),0x1},
        {0x0,0x0},
        //CSI_DATA20
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA20_SELECT_INPUT),0x1},
        //EIM_DATA04
        {0x0,0x0},
        //GPIO3_17
        {0x0,0x0},
        //SRC_BT_CFG12
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI1_RDY
        {0x0,0x0},
    }
};
// GPIO82
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_82 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA13),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA13),
    {
        //LCDIF_DATA13
        {0x0,0x0},
        //SAI3_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_TX_BCLK_SELECT_INPUT),0x1},
        {0x0,0x0},
        //CSI_DATA21
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA21_SELECT_INPUT),0x1},
        //EIM_DATA05
        {0x0,0x0},
        //GPIO3_18
        {0x0,0x0},
        //SRC_BT_CFG13
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_RESET_B
        {0x0,0x0},
    }
};
// GPIO83
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_83 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA14),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA14),
    {
        //LCDIF_DATA14
        {0x0,0x0},
        //SAI3_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI3_RX_DATA_SELECT_INPUT),0x1},
        {0x0,0x0},
        //CSI_DATA22
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA22_SELECT_INPUT),0x1},
        //EIM_DATA06
        {0x0,0x0},
        //GPIO3_19
        {0x0,0x0},
        //SRC_BT_CFG14
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA4
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA4_SELECT_INPUT),0x0},
    }
};
// GPIO84
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_84 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA15),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA15),
    {
        //LCDIF_DATA15
        {0x0,0x0},
        //SAI3_TX_DATA
        {0x0,0x0},
        {0x0,0x0},
        //CSI_DATA23
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA23_SELECT_INPUT),0x1},
        //EIM_DATA07
        {0x0,0x0},
        //GPIO3_20
        {0x0,0x0},
        //SRC_BT_CFG15
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA5
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA5_SELECT_INPUT),0x0},
    }
};
// GPIO85
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_85 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA16),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA16),
    {
        //LCDIF_DATA16
        {0x0,0x0},
        //UART7_TX
        {0x0,0x0},
        {0x0,0x0},
        //CSI_DATA01
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA01_SELECT_INPUT),0x1},
        //EIM_DATA08
        {0x0,0x0},
        //GPIO3_21
        {0x0,0x0},
        //SRC_BT_CFG24
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA6
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA6_SELECT_INPUT),0x0},
    }
};
// GPIO86
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_86 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA17),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA17),
    {
        //LCDIF_DATA17
        {0x0,0x0},
        //UART7_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART7_RX_DATA_SELECT_INPUT),0x3},
        {0x0,0x0},
        //CSI_DATA00
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA00_SELECT_INPUT),0x1},
        //EIM_DATA09
        {0x0,0x0},
        //GPIO3_22
        {0x0,0x0},
        //SRC_BT_CFG25
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA7
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA7_SELECT_INPUT),0x0},
    }
};
// GPIO87
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_87 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA18),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA18),
    {
        //LCDIF_DATA18
        {0x0,0x0},
        //PWM5_OUT
        {0x0,0x0},
        //CA7_MX6UL_EVENTO
        {0x0,0x0},
        //CSI_DATA10
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA10_SELECT_INPUT),0x1},
        //EIM_DATA10
        {0x0,0x0},
        //GPIO3_23
        {0x0,0x0},
        //SRC_BT_CFG26
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_CMD
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CMD_SELECT_INPUT),0x1},
    }
};
// GPIO88
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_88 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA19),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA19),
    {
        //LCDIF_DATA19
        {0x0,0x0},
        //PWM6_OUT
        {0x0,0x0},
        //WDOG1_WDOG_ANY
        {0x0,0x0},
        //CSI_DATA11
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA11_SELECT_INPUT),0x1},
        //EIM_DATA11
        {0x0,0x0},
        //GPIO3_24
        {0x0,0x0},
        //SRC_BT_CFG27
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CLK_SELECT_INPUT),0x1},
    }
};
// GPIO89
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_89 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA20),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA20),
    {
        //LCDIF_DATA20
        {0x0,0x0},
        //UART8_TX
        {0x0,0x0},
        //ECSPI1_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_SCLK_SELECT_INPUT),0x0},
        //CSI_DATA12
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA12_SELECT_INPUT),0x1},
        //EIM_DATA12
        {0x0,0x0},
        //GPIO3_25
        {0x0,0x0},
        //SRC_BT_CFG28
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA0
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA0_SELECT_INPUT),0x1},
    }
};
// GPIO90
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_90 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA21),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA21),
    {
        //LCDIF_DATA21
        {0x0,0x0},
        //UART8_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART8_RX_DATA_SELECT_INPUT),0x3},
        //ECSPI1_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_SS0_B_SELECT_INPUT),0x0},
        //CSI_DATA13
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA13_SELECT_INPUT),0x1},
        //EIM_DATA13
        {0x0,0x0},
        //GPIO3_26
        {0x0,0x0},
        //SRC_BT_CFG29
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA1
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA1_SELECT_INPUT),0x1},
    }
};
// GPIO91
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_91 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA22),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA22),
    {
        //LCDIF_DATA22
        {0x0,0x0},
        //MQS_RIGHT
        {0x0,0x0},
        //ECSPI1_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_MOSI_SELECT_INPUT),0x0},
        //CSI_DATA14
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA14_SELECT_INPUT),0x1},
        //EIM_DATA14
        {0x0,0x0},
        //GPIO3_27
        {0x0,0x0},
        //SRC_BT_CFG30
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA2
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA2_SELECT_INPUT),0x1},
    }
};
// GPIO92
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_92 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_LCD_DATA23),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_LCD_DATA23),
    {
        //LCDIF_DATA23
        {0x0,0x0},
        //MQS_LEFT
        {0x0,0x0},
        //ECSPI1_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_MISO_SELECT_INPUT),0x0},
        //CSI_DATA15
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA15_SELECT_INPUT),0x1},
        //EIM_DATA15
        {0x0,0x0},
        //GPIO3_28
        {0x0,0x0},
        //SRC_BT_CFG31
        {0x0,0x0},
        {0x0,0x0},
        //USDHC2_DATA3
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA3_SELECT_INPUT),0x1},
    }
};
// GPIO93
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_93 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO94
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_94 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO95
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_95 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO96
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_96 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_RE_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_RE_B),
    {
        //RAWNAND_RE_B
        {0x0,0x0},
        //USDHC2_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CLK_SELECT_INPUT),0x2},
        //QSPI_B_SCLK
        {0x0,0x0},
        //KPP_ROW00
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW0_SELECT_INPUT),0x1},
        //EIM_EB_B00
        {0x0,0x0},
        //GPIO4_0
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI3_SS2
        {0x0,0x0},
    }
};
// GPIO97
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_97 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_WE_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_WE_B),
    {
        //RAWNAND_WE_B
        {0x0,0x0},
        //USDHC2_CMD
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CMD_SELECT_INPUT),0x2},
        //QSPI_B_SS0_B
        {0x0,0x0},
        //KPP_COL00
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL0_SELECT_INPUT),0x1},
        //EIM_EB_B01
        {0x0,0x0},
        //GPIO4_1
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI3_SS3
        {0x0,0x0},
    }
};
// GPIO98
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_98 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA00),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA00),
    {
        //RAWNAND_DATA00
        {0x0,0x0},
        //USDHC2_DATA0
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA0_SELECT_INPUT),0x2},
        //QSPI_B_SS1_B
        {0x0,0x0},
        //KPP_ROW01
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW1_SELECT_INPUT),0x1},
        //EIM_AD08
        {0x0,0x0},
        //GPIO4_2
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI4_RDY
        {0x0,0x0},
    }
};
// GPIO99
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_99 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA01),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA01),
    {
        //RAWNAND_DATA01
        {0x0,0x0},
        //USDHC2_DATA1
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA1_SELECT_INPUT),0x2},
        //QSPI_B_DQS
        {0x0,0x0},
        //KPP_COL01
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL1_SELECT_INPUT),0x1},
        //EIM_AD09
        {0x0,0x0},
        //GPIO4_3
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI4_SS1
        {0x0,0x0},
    }
};
// GPIO100
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_100 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA02),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA02),
    {
        //RAWNAND_DATA02
        {0x0,0x0},
        //USDHC2_DATA2
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA2_SELECT_INPUT),0x1},
        //QSPI_B_DATA00
        {0x0,0x0},
        //KPP_ROW02
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_ROW2_SELECT_INPUT),0x1},
        //EIM_AD10
        {0x0,0x0},
        //GPIO4_4
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI4_SS2
        {0x0,0x0},
    }
};
// GPIO101
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_101 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA03),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA03),
    {
        //RAWNAND_DATA03
        {0x0,0x0},
        //USDHC2_DATA3
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA3_SELECT_INPUT),0x2},
        //QSPI_B_DATA01
        {0x0,0x0},
        //KPP_COL02
        {AW_OFFSET(hw_iomuxc_regs_t,KPP_COL2_SELECT_INPUT),0x1},
        //EIM_AD11
        {0x0,0x0},
        //GPIO4_5
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI4_SS3
        {0x0,0x0},
    }
};
// GPIO102
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_102 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA04),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA04),
    {
        //RAWNAND_DATA04
        {0x0,0x0},
        //USDHC2_DATA4
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA4_SELECT_INPUT),0x1},
        //QSPI_B_DATA02
        {0x0,0x0},
        //ECSPI4_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_SCLK_SELECT_INPUT),0x1},
        //EIM_AD12
        {0x0,0x0},
        //GPIO4_6
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART2_TX
        {0x0,0x0},
    }
};
// GPIO103
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_103 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA05),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA05),
    {
        //RAWNAND_DATA05
        {0x0,0x0},
        //USDHC2_DATA5
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA5_SELECT_INPUT),0x1},
        //QSPI_B_DATA03
        {0x0,0x0},
        //ECSPI4_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_MOSI_SELECT_INPUT),0x1},
        //EIM_AD13
        {0x0,0x0},
        //GPIO4_7
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART2_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART2_RX_DATA_SELECT_INPUT),0x3},
    }
};
// GPIO104
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_104 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA06),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA06),
    {
        //RAWNAND_DATA06
        {0x0,0x0},
        //USDHC2_DATA6
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA6_SELECT_INPUT),0x1},
        //SAI2_RX_BCLK
        {0x0,0x0},
        //ECSPI4_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_MISO_SELECT_INPUT),0x1},
        //EIM_AD14
        {0x0,0x0},
        //GPIO4_8
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART2_CTS_B
        {0x0,0x0},
    }
};
// GPIO105
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_105 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DATA07),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DATA07),
    {
        //RAWNAND_DATA07
        {0x0,0x0},
        //USDHC2_DATA7
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA7_SELECT_INPUT),0x1},
        //QSPI_A_SS1_B
        {0x0,0x0},
        //ECSPI4_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI4_SS0_B_SELECT_INPUT),0x1},
        //EIM_AD15
        {0x0,0x0},
        //GPIO4_9
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART2_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART2_RTS_B_SELECT_INPUT),0x5},
    }
};
// GPIO106
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_106 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_ALE),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_ALE),
    {
        //RAWNAND_ALE
        {0x0,0x0},
        //USDHC2_RESET_B
        {0x0,0x0},
        //QSPI_A_DQS
        {0x0,0x0},
        //PWM3_OUT
        {0x0,0x0},
        //EIM_ADDR17
        {0x0,0x0},
        //GPIO4_10
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI3_SS1
        {0x0,0x0},
    }
};
// GPIO107
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_107 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_WP_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_WP_B),
    {
        //RAWNAND_WP_B
        {0x0,0x0},
        //USDHC1_RESET_B
        {0x0,0x0},
        //QSPI_A_SCLK
        {0x0,0x0},
        //PWM4_OUT
        {0x0,0x0},
        //EIM_BCLK
        {0x0,0x0},
        //GPIO4_11
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //ECSPI3_RDY
        {0x0,0x0},
    }
};
// GPIO108
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_108 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_READY_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_READY_B),
    {
        //RAWNAND_READY_B
        {0x0,0x0},
        //USDHC1_DATA4
        {0x0,0x0},
        //QSPI_A_DATA00
        {0x0,0x0},
        //ECSPI3_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_SS0_B_SELECT_INPUT),0x1},
        //EIM_CS1_B
        {0x0,0x0},
        //GPIO4_12
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART3_TX
        {0x0,0x0},
    }
};
// GPIO109
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_109 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_CE0_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_CE0_B),
    {
        //RAWNAND_CE0_B
        {0x0,0x0},
        //USDHC1_DATA5
        {0x0,0x0},
        //QSPI_A_DATA01
        {0x0,0x0},
        //ECSPI3_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_SCLK_SELECT_INPUT),0x1},
        //EIM_DTACK_B
        {0x0,0x0},
        //GPIO4_13
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART3_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART3_RX_DATA_SELECT_INPUT),0x3},
    }
};
// GPIO110
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_110 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_CE1_B),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_CE1_B),
    {
        //RAWNAND_CE1_B
        {0x0,0x0},
        //USDHC1_DATA6
        {0x0,0x0},
        //QSPI_A_DATA02
        {0x0,0x0},
        //ECSPI3_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_MOSI_SELECT_INPUT),0x1},
        //EIM_ADDR18
        {0x0,0x0},
        //GPIO4_14
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART3_CTS_B
        {0x0,0x0},
    }
};
// GPIO111
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_111 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_CLE),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_CLE),
    {
        //RAWNAND_CLE
        {0x0,0x0},
        //USDHC1_DATA7
        {0x0,0x0},
        //QSPI_A_DATA03
        {0x0,0x0},
        //ECSPI3_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI3_MISO_SELECT_INPUT),0x1},
        //EIM_ADDR16
        {0x0,0x0},
        //GPIO4_15
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //UART3_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART3_RTS_B_SELECT_INPUT),0x3},
    }
};
// GPIO112
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_112 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_NAND_DQS),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_NAND_DQS),
    {
        //RAWNAND_DQS
        {0x0,0x0},
        //CSI_FIELD
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_FIELD_SELECT_INPUT),0x1},
        //QSPI_A_SS0_B
        {0x0,0x0},
        //PWM5_OUT
        {0x0,0x0},
        //EIM_WAIT
        {0x0,0x0},
        //GPIO4_16
        {0x0,0x0},
        //SDMA_EXT_EVENT01
        {0x0,0x0},
        {0x0,0x0},
        //SPDIF_EXT_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,SPDIF_EXT_CLK_SELECT_INPUT),0x1},
    }
};
// GPIO113
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_113 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_MCLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_MCLK),
    {
        //CSI_MCLK
        {0x0,0x0},
        //USDHC2_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CD_B_SELECT_INPUT),0x0},
        //RAWNAND_CE2_B
        {0x0,0x0},
        //I2C1_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SDA_SELECT_INPUT),0x0},
        //EIM_CS0_B
        {0x0,0x0},
        //GPIO4_17
        {0x0,0x0},
        //SNVS_HP_VIO_5_CTL
        {0x0,0x0},
        {0x0,0x0},
        //UART6_TX
        {0x0,0x0},
    }
};
// GPIO114
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_114 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_PIXCLK),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_PIXCLK),
    {
        //CSI_PIXCLK
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_PIXCLK_SELECT_INPUT),0x1},
        //USDHC2_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_WP_SELECT_INPUT),0x2},
        //RAWNAND_CE3_B
        {0x0,0x0},
        //I2C1_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C1_SCL_SELECT_INPUT),0x2},
        //EIM_OE
        {0x0,0x0},
        //GPIO4_18
        {0x0,0x0},
        //SNVS_HP_VIO_5
        {0x0,0x0},
        {0x0,0x0},
        //UART6_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART6_RX_DATA_SELECT_INPUT),0x3},
    }
};
// GPIO115
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_115 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_VSYNC),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_VSYNC),
    {
        //CSI_VSYNC
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_VSYNC_SELECT_INPUT),0x0},
        //USDHC2_CLK
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CLK_SELECT_INPUT),0x0},
        //SIM1_PORT1_CLK
        {0x0,0x0},
        //I2C2_SDA
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SDA_SELECT_INPUT),0x0},
        //EIM_RW
        {0x0,0x0},
        //GPIO4_19
        {0x0,0x0},
        //PWM7_OUT
        {0x0,0x0},
        {0x0,0x0},
        //UART6_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART6_RTS_B_SELECT_INPUT),0x0},
    }
};
// GPIO116
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_116 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_HSYNC),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_HSYNC),
    {
        //CSI_HSYNC
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_HSYNC_SELECT_INPUT),0x0},
        //USDHC2_CMD
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_CMD_SELECT_INPUT),0x0},
        //SIM1_PORT1_PD
        {0x0,0x0},
        //I2C2_SCL
        {AW_OFFSET(hw_iomuxc_regs_t,I2C2_SCL_SELECT_INPUT),0x0},
        //EIM_LBA_B
        {0x0,0x0},
        //GPIO4_20
        {0x0,0x0},
        //PWM8_OUT
        {0x0,0x0},
        {0x0,0x0},
        //UART6_CTS_B
        {0x0,0x0},
    }
};
// GPIO117
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_117 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA00),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA00),
    {
        //CSI_DATA02
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA02_SELECT_INPUT),0x0},
        //USDHC2_DATA0
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA0_SELECT_INPUT),0x0},
        //SIM1_PORT1_RST_B
        {0x0,0x0},
        //ECSPI2_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_SCLK_SELECT_INPUT),0x0},
        //EIM_AD00
        {0x0,0x0},
        //GPIO4_21
        {0x0,0x0},
        //SRC_INT_BOOT
        {0x0,0x0},
        {0x0,0x0},
        //UART5_TX
        {0x0,0x0},
    }
};
// GPIO118
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_118 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA01),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA01),
    {
        //CSI_DATA03
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA03_SELECT_INPUT),0x0},
        //USDHC2_DATA1
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA1_SELECT_INPUT),0x0},
        //SIM1_PORT1_SVEN
        {0x0,0x0},
        //ECSPI2_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_SS0_B_SELECT_INPUT),0x0},
        //EIM_AD01
        {0x0,0x0},
        //GPIO4_22
        {0x0,0x0},
        //SAI1_MCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_MCLK_SELECT_INPUT),0x0},
        {0x0,0x0},
        //UART5_RX
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RX_DATA_SELECT_INPUT),0x1},
    }
};
// GPIO119
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_119 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA02),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA02),
    {
        //CSI_DATA04
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA04_SELECT_INPUT),0x1},
        //USDHC2_DATA2
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA2_SELECT_INPUT),0x2},
        //SIM1_PORT1_TRXD
        {0x0,0x0},
        //ECSPI2_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_MOSI_SELECT_INPUT),0x1},
        //EIM_AD02
        {0x0,0x0},
        //GPIO4_23
        {0x0,0x0},
        //SAI1_RX_SYNC
        {0x0,0x0},
        {0x0,0x0},
        //UART5_RTS_B
        {AW_OFFSET(hw_iomuxc_regs_t,UART5_RTS_B_SELECT_INPUT),0x5},
    }
};
// GPIO120
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_120 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA03),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA03),
    {
        //CSI_DATA05
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA05_SELECT_INPUT),0x0},
        //USDHC2_DATA3
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA3_SELECT_INPUT),0x0},
        //SIM2_PORT1_PD
        {0x0,0x0},
        //ECSPI2_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI2_MOSI_SELECT_INPUT),0x1},
        //EIM_AD03
        {0x0,0x0},
        //GPIO4_24
        {0x0,0x0},
        //SAI1_RX_BCLK
        {0x0,0x0},
        {0x0,0x0},
        //UART5_CTS_B
        {0x0,0x0},
    }
};
// GPIO121
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_121 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA04),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA04),
    {
        //CSI_DATA06
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA06_SELECT_INPUT),0x1},
        //USDHC2_DATA4
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA4_SELECT_INPUT),0x2},
        //SIM2_PORT1_CLK
        {0x0,0x0},
        //ECSPI1_SCLK
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_SCLK_SELECT_INPUT),0x1},
        //EIM_AD04
        {0x0,0x0},
        //GPIO4_25
        {0x0,0x0},
        //SAI1_TX_SYNC
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_TX_SYNC_SELECT_INPUT),0x1},
        {0x0,0x0},
        //USDHC1_WP
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_WP_SELECT_INPUT),0x2},
    }
};
// GPIO122
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_122 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA05),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA05),
    {
        //CSI_DATA07
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA07_SELECT_INPUT),0x1},
        //USDHC2_DATA5
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA5_SELECT_INPUT),0x2},
        //SIM2_PORT1_RST_B
        {0x0,0x0},
        //ECSPI1_SS0
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_SS0_B_SELECT_INPUT),0x1},
        //EIM_AD05
        {0x0,0x0},
        //GPIO4_26
        {0x0,0x0},
        //SAI1_TX_BCLK
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_TX_BCLK_SELECT_INPUT),0x1},
        {0x0,0x0},
        //USDHC1_CD_B
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC1_CD_B_SELECT_INPUT),0x2},
    }
};
// GPIO123
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_123 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA06),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA06),
    {
        //CSI_DATA08
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA08_SELECT_INPUT),0x1},
        //USDHC2_DATA6
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA6_SELECT_INPUT),0x2},
        //SIM2_PORT1_SVEN
        {0x0,0x0},
        //ECSPI1_MOSI
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_MOSI_SELECT_INPUT),0x1},
        //EIM_AD06
        {0x0,0x0},
        //GPIO4_27
        {0x0,0x0},
        //SAI1_RX_DATA
        {AW_OFFSET(hw_iomuxc_regs_t,SAI1_RX_DATA_SELECT_INPUT),0x1},
        {0x0,0x0},
        //USDHC1_RESET_B
        {0x0,0x0},
    }
};
// GPIO124
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_124 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_CSI_DATA07),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_CSI_DATA07),
    {
        //CSI_DATA09
        {AW_OFFSET(hw_iomuxc_regs_t,CSI_DATA09_SELECT_INPUT),0x1},
        //USDHC2_DATA7
        {AW_OFFSET(hw_iomuxc_regs_t,USDHC2_DATA7_SELECT_INPUT),0x2},
        //SIM2_PORT1_TRXD
        {0x0,0x0},
        //ECSPI1_MISO
        {AW_OFFSET(hw_iomuxc_regs_t,ECSPI1_MISO_SELECT_INPUT),0x1},
        //EIM_AD07
        {0x0,0x0},
        //GPIO4_28
        {0x0,0x0},
        //SAI1_TX_DATA
        {0x0,0x0},
        {0x0,0x0},
        //USDHC1_VSELECT
        {0x0,0x0},
    }
};
// GPIO125
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_125 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO126
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_126 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO127
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_127 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO128
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_128 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER0),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER0),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_0
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO129
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_129 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER1),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER1),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_1
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO130
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_130 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER2),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER2),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_2
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO131
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_131 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER3),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER3),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_3
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO132
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_132 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER4),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER4),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_4
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO133
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_133 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER5),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER5),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_5
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO134
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_134 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER6),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER6),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_6
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO135
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_135 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER7),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER7),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_7
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO136
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_136 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER8),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER8),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_8
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO137
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_137 = {
    AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_SNVS_TAMPER9),
    AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_SNVS_TAMPER9),
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        //GPIO5_9
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO138
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_138 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO139
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_139 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO140
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_140 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO141
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_141 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO142
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_142 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO143
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_143 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO144
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_144 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO145
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_145 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO146
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_146 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO147
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_147 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO148
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_148 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO149
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_149 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO150
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_150 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO151
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_151 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO152
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_152 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO153
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_153 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO154
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_154 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO155
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_155 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO156
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_156 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO157
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_157 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO158
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_158 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};
// GPIO159
static imx6ul_pin_cfg_map_t aw_const _pin_cfg_map_159 = {
    0,
    0,
    {
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
        {0x0,0x0},
    }
};

#define     ELEM_PIN_MAP0 \
    &_pin_cfg_map_0, \
    &_pin_cfg_map_1, \
    &_pin_cfg_map_2, \
    &_pin_cfg_map_3, \
    &_pin_cfg_map_4, \
    &_pin_cfg_map_5, \
    &_pin_cfg_map_6, \
    &_pin_cfg_map_7, \
    &_pin_cfg_map_8, \
    &_pin_cfg_map_9 \

#define     ELEM_PIN_MAP_TENS(x) \
    &_pin_cfg_map_##x##0, \
    &_pin_cfg_map_##x##1, \
    &_pin_cfg_map_##x##2, \
    &_pin_cfg_map_##x##3, \
    &_pin_cfg_map_##x##4, \
    &_pin_cfg_map_##x##5, \
    &_pin_cfg_map_##x##6, \
    &_pin_cfg_map_##x##7, \
    &_pin_cfg_map_##x##8, \
    &_pin_cfg_map_##x##9 \

static aw_const imx6ul_pin_cfg_map_t * aw_const __pin_cfg_maps[140] = {

        ELEM_PIN_MAP0,
        ELEM_PIN_MAP_TENS(1),
        ELEM_PIN_MAP_TENS(2),
        ELEM_PIN_MAP_TENS(3),
        ELEM_PIN_MAP_TENS(4),
        ELEM_PIN_MAP_TENS(5),
        ELEM_PIN_MAP_TENS(6),
        ELEM_PIN_MAP_TENS(7),
        ELEM_PIN_MAP_TENS(8),
        ELEM_PIN_MAP_TENS(9),
        ELEM_PIN_MAP_TENS(10),
        ELEM_PIN_MAP_TENS(11),
        ELEM_PIN_MAP_TENS(12),
        ELEM_PIN_MAP_TENS(13),


};

aw_const imx6ul_pin_cfg_map_t *_imx6ul_get_pin_cfg_map(int pin)
{
    if(pin <0 || pin >= AW_NELEMENTS(__pin_cfg_maps)) {
        return &_pin_cfg_map_159;
    }
    if (NULL == __pin_cfg_maps[pin]) {
        return &_pin_cfg_map_159;
    }
    return __pin_cfg_maps[pin];
}
