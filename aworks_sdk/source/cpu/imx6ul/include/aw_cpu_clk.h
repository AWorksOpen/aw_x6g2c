#ifndef         __AW_CPU_CLK_H__
#define         __AW_CPU_CLK_H__

typedef enum {
    IMX6UL_CLK_INVALID = -1,
    IMX6UL_CLK_DUMMY = 0,
    IMX6UL_CLK_CKIL = 1,
    IMX6UL_CLK_CKIH = 2,
    IMX6UL_CLK_OSC = 3,

    IMX6UL_PLL1_BYPASS_SRC = 4,
    IMX6UL_PLL2_BYPASS_SRC = 5,
    IMX6UL_PLL3_BYPASS_SRC = 6,
    IMX6UL_PLL4_BYPASS_SRC = 7,
    IMX6UL_PLL5_BYPASS_SRC = 8,
    IMX6UL_PLL6_BYPASS_SRC = 9,

    IMX6UL_PLL7_BYPASS_SRC = 10,
    IMX6UL_CLK_PLL1 = 11,
    IMX6UL_CLK_PLL2 = 12,
    IMX6UL_CLK_PLL3 = 13,
    IMX6UL_CLK_PLL4 = 14,
    IMX6UL_CLK_PLL5 = 15,
    IMX6UL_CLK_PLL6 = 16,
    IMX6UL_CLK_PLL7 = 17,
    IMX6UL_PLL1_BYPASS = 18,
    IMX6UL_PLL2_BYPASS = 19,

    IMX6UL_PLL3_BYPASS = 20,
    IMX6UL_PLL4_BYPASS = 21,
    IMX6UL_PLL5_BYPASS = 22,
    IMX6UL_PLL6_BYPASS = 23,
    IMX6UL_PLL7_BYPASS = 24,
    IMX6UL_CLK_PLL1_SYS = 25,
    IMX6UL_CLK_PLL2_BUS = 26,
    IMX6UL_CLK_PLL3_USB_OTG  = 27,
    IMX6UL_CLK_PLL4_AUDIO  = 28,
    IMX6UL_CLK_PLL5_VIDEO  = 29,

    IMX6UL_CLK_PLL6_ENET = 30,
    IMX6UL_CLK_PLL7_USB_HOST = 31,
    IMX6UL_CLK_USBPHY1 = 32,
    IMX6UL_CLK_USBPHY2 = 33,
    IMX6UL_CLK_USBPHY1_GATE = 34,
    IMX6UL_CLK_USBPHY2_GATE = 35,
    IMX6UL_CLK_PLL2_PFD0 = 36,
    IMX6UL_CLK_PLL2_PFD1 = 37,
    IMX6UL_CLK_PLL2_PFD2 = 38,
    IMX6UL_CLK_PLL2_PFD3 = 39,

    IMX6UL_CLK_PLL3_PFD0 = 40,
    IMX6UL_CLK_PLL3_PFD1 = 41,
    IMX6UL_CLK_PLL3_PFD2 = 42,
    IMX6UL_CLK_PLL3_PFD3 = 43,
    IMX6UL_CLK_ENET_REF = 44,
    IMX6UL_CLK_ENET2_REF = 45,
    IMX6UL_CLK_ENET2_REF_125M = 46,
    IMX6UL_CLK_ENET_PTP_REF = 47,
    IMX6UL_CLK_ENET_PTP = 48,
    IMX6UL_CLK_PLL4_POST_DIV = 49,

    IMX6UL_CLK_PLL4_AUDIO_DIV = 50,
    IMX6UL_CLK_PLL5_POST_DIV = 51,
    IMX6UL_CLK_PLL5_VIDEO_DIV = 52,
    IMX6UL_CLK_PLL2_198M = 53,
    IMX6UL_CLK_PLL3_80M = 54,
    IMX6UL_CLK_PLL3_60M = 55,
    IMX6UL_CLK_STEP = 56,
    IMX6UL_CLK_PLL1_SW = 57,
    IMX6UL_CLK_AXI_ALT_SEL = 58,
    IMX6UL_CLK_AXI_SEL = 59,

    IMX6UL_CLK_PERIPH_PRE= 60,
    IMX6UL_CLK_PERIPH2_PRE = 61,
    IMX6UL_CLK_PERIPH_CLK2_SEL = 62,
    IMX6UL_CLK_PERIPH2_CLK2_SEL = 63,
    IMX6UL_CLK_USDHC1_SEL= 64,
    IMX6UL_CLK_USDHC2_SEL = 65,
    IMX6UL_CLK_BCH_SEL = 66,
    IMX6UL_CLK_GPMI_SEL = 67,
    IMX6UL_CLK_EIM_SLOW_SEL = 68,
    IMX6UL_CLK_SPDIF_SEL = 69,

    IMX6UL_CLK_SAI1_SEL = 70,
    IMX6UL_CLK_SAI2_SEL = 71,
    IMX6UL_CLK_SAI3_SEL = 72,
    IMX6UL_CLK_LCDIF_PRE_SEL = 73,
    IMX6UL_CLK_SIM_PRE_SEL = 74,
    IMX6UL_CLK_LDB_DI0_SEL =75,
    IMX6UL_CLK_LDB_DI1_SEL = 76,
    IMX6UL_CLK_ENFC_SEL = 77,
    IMX6UL_CLK_CAN_SEL = 78,
    IMX6UL_CLK_ECSPI_SEL = 79,

    IMX6UL_CLK_UART_SEL = 80,
    IMX6UL_CLK_QSPI1_SEL = 81,
    IMX6UL_CLK_PERCLK_SEL = 82,
    IMX6UL_CLK_LCDIF_SEL = 83,
    IMX6UL_CLK_SIM_SEL = 84,
    IMX6UL_CLK_PERIPH  = 85,
    IMX6UL_CLK_PERIPH2  = 86,
    IMX6UL_CLK_LDB_DI0_DIV_3_5 = 87,
    IMX6UL_CLK_LDB_DI0_DIV_7 = 88,
    IMX6UL_CLK_LDB_DI1_DIV_3_5 = 89,

    IMX6UL_CLK_LDB_DI1_DIV_7 = 90,
    IMX6UL_CLK_LDB_DI0_DIV_SEL = 91,
    IMX6UL_CLK_LDB_DI1_DIV_SEL = 92,
    IMX6UL_CLK_ARM  = 93,
    IMX6UL_CLK_PERIPH_CLK2 = 94,
    IMX6UL_CLK_PERIPH2_CLK2 = 95,
    IMX6UL_CLK_AHB = 96,
    IMX6UL_CLK_MMDC_PODF = 97,
    IMX6UL_CLK_AXI_PODF = 98,
    IMX6UL_CLK_PERCLK = 99,

    IMX6UL_CLK_IPG  = 100,
    IMX6UL_CLK_USDHC1_PODF = 101,
    IMX6UL_CLK_USDHC2_PODF = 102,
    IMX6UL_CLK_BCH_PODF = 103,
    IMX6UL_CLK_GPMI_PODF = 104,
    IMX6UL_CLK_EIM_SLOW_PODF = 105,
    IMX6UL_CLK_SPDIF_PRED = 106,
    IMX6UL_CLK_SPDIF_PODF = 107,
    IMX6UL_CLK_SAI1_PRED = 108,
    IMX6UL_CLK_SAI1_PODF = 109,

    IMX6UL_CLK_SAI2_PRED = 110,
    IMX6UL_CLK_SAI2_PODF = 111,
    IMX6UL_CLK_SAI3_PRED  = 112,
    IMX6UL_CLK_SAI3_PODF = 113,
    IMX6UL_CLK_LCDIF_PRED = 114,
    IMX6UL_CLK_LCDIF_PODF = 115,
    IMX6UL_CLK_SIM_PODF = 116,
    IMX6UL_CLK_QSPI1_PDOF = 117,
    IMX6UL_CLK_ENFC_PRED = 118,
    IMX6UL_CLK_ENFC_PODF = 119,

    IMX6UL_CLK_CAN_PODF = 120,
    IMX6UL_CLK_ECSPI_PODF = 121,
    IMX6UL_CLK_UART_PODF = 122,
    IMX6UL_CLK_ADC1 = 123,
    IMX6UL_CLK_ADC2 = 124,
    IMX6UL_CLK_AIPSTZ1 = 125,
    IMX6UL_CLK_AIPSTZ2 = 126,
    IMX6UL_CLK_AIPSTZ3 = 127,
    IMX6UL_CLK_APBHDMA = 128,
    IMX6UL_CLK_ASRC_IPG = 129,

    IMX6UL_CLK_ASRC_MEM = 130,
    IMX6UL_CLK_GPMI_BCH_APB = 131,
    IMX6UL_CLK_GPMI_BCH = 132,
    IMX6UL_CLK_GPMI_IO = 133,
    IMX6UL_CLK_GPMI_APB = 134,
    IMX6UL_CLK_CAAM_MEM = 135,
    IMX6UL_CLK_CAAM_ACLK = 136,
    IMX6UL_CLK_CAAM_IPG = 137,
    IMX6UL_CLK_CSI = 138,
    IMX6UL_CLK_ECSPI1 = 139,

    IMX6UL_CLK_ECSPI2 = 140,
    IMX6UL_CLK_ECSPI3 = 141,
    IMX6UL_CLK_ECSPI4 = 142,
    IMX6UL_CLK_EIM = 143,
    IMX6UL_CLK_ENET = 144,
    IMX6UL_CLK_ENET_AHB = 145,
    IMX6UL_CLK_EPIT1 = 146,
    IMX6UL_CLK_EPIT2 = 147,
    IMX6UL_CLK_CAN1_IPG = 148,
    IMX6UL_CLK_CAN1_SERIAL = 149,

    IMX6UL_CLK_CAN2_IPG = 150,
    IMX6UL_CLK_CAN2_SERIAL = 151,
    IMX6UL_CLK_GPT1_BUS = 152,
    IMX6UL_CLK_GPT1_SERIAL = 153,
    IMX6UL_CLK_GPT2_BUS = 154,
    IMX6UL_CLK_GPT2_SERIAL = 155,
    IMX6UL_CLK_I2C1 = 156,
    IMX6UL_CLK_I2C2 = 157,
    IMX6UL_CLK_I2C3 = 158,
    IMX6UL_CLK_I2C4 = 159,

    IMX6UL_CLK_IOMUXC_IPT_CLK_IO = 160,
    IMX6UL_CLK_LCDIF_APB = 161,
    IMX6UL_CLK_LCDIF_PIX = 162,
    IMX6UL_CLK_MMDC_P0_FAST = 163,
    IMX6UL_CLK_MMDC_P0_IPG = 164,
    IMX6UL_CLK_OCOTP = 165,
    IMX6UL_CLK_OCRAM = 166,
    IMX6UL_CLK_PWM1 = 167,
    IMX6UL_CLK_PWM2 = 168,
    IMX6UL_CLK_PWM3 = 169,

    IMX6UL_CLK_PWM4 = 170,
    IMX6UL_CLK_PWM5 = 171,
    IMX6UL_CLK_PWM6 = 172,
    IMX6UL_CLK_PWM7 = 173,
    IMX6UL_CLK_PWM8 = 174,
    IMX6UL_CLK_PXP = 175,
    IMX6UL_CLK_QSPI = 176,
    IMX6UL_CLK_ROM = 177,
    IMX6UL_CLK_SAI1 = 178,
    IMX6UL_CLK_SAI1_IPG = 179,

    IMX6UL_CLK_SAI2 = 180,
    IMX6UL_CLK_SAI2_IPG = 181,
    IMX6UL_CLK_SAI3 = 182,
    IMX6UL_CLK_SAI3_IPG = 183,
    IMX6UL_CLK_SDMA = 184,
    IMX6UL_CLK_SIM = 185,
    IMX6UL_CLK_SIM_S = 186,
    IMX6UL_CLK_SPBA = 187,
    IMX6UL_CLK_SPDIF = 188,
    IMX6UL_CLK_UART1_IPG = 189,

    IMX6UL_CLK_UART1_SERIAL = 190,
    IMX6UL_CLK_UART2_IPG = 191,
    IMX6UL_CLK_UART2_SERIAL = 192,
    IMX6UL_CLK_UART3_IPG = 193,
    IMX6UL_CLK_UART3_SERIAL = 194,
    IMX6UL_CLK_UART4_IPG = 195,
    IMX6UL_CLK_UART4_SERIAL = 196,
    IMX6UL_CLK_UART5_IPG = 197,
    IMX6UL_CLK_UART5_SERIAL = 198,
    IMX6UL_CLK_UART6_IPG = 199,

    IMX6UL_CLK_UART6_SERIAL = 200,
    IMX6UL_CLK_UART7_IPG = 201,
    IMX6UL_CLK_UART7_SERIAL = 202,
    IMX6UL_CLK_UART8_IPG  = 203,
    IMX6UL_CLK_UART8_SERIAL = 204,
    IMX6UL_CLK_USBOH3 = 205,
    IMX6UL_CLK_USDHC1 = 206,
    IMX6UL_CLK_USDHC2 = 207,
    IMX6UL_CLK_WDOG1 = 208,
    IMX6UL_CLK_WDOG2 = 209,

    IMX6UL_CLK_WDOG3  = 210,
    IMX6UL_CLK_LDB_DI0 = 211,
    IMX6UL_CLK_AXI = 212,
    IMX6UL_CLK_SPDIF_GCLK = 213,
    IMX6UL_CLK_GPT_3M  = 214,
    IMX6UL_CLK_SIM2 = 215,
    IMX6UL_CLK_SIM1 = 216,
    IMX6UL_CLK_IPP_DI0 = 217,
    IMX6UL_CLK_IPP_DI1 = 218,
    IMX6UL_CA7_SECONDARY_SEL = 219,

    IMX6UL_CLK_PER_BCH = 220,
    IMX6UL_CLK_CSI_SEL = 221,
    IMX6UL_CLK_CSI_PODF = 222,
    IMX6UL_CLK_PLL3_120M = 223,
    IMX6UL_CLK_ENET1_REF_125M = 224,
    IMX6UL_CLK_ARM_DBG = 225,
    IMX6UL_CLK_CSU = 226,
    IMX6UL_CLK_GPIO1 = 227,
    IMX6UL_CLK_GPIO2 = 228,
    IMX6UL_CLK_GPIO3 = 229,

    IMX6UL_CLK_GPIO4 = 230,
    IMX6UL_CLK_GPIO5 = 231,
    IMX6UL_CLK_IPMUX1 = 232,
    IMX6UL_CLK_IPMUX2 = 233,
    IMX6UL_CLK_IPMUX3 = 234,
    IMX6UL_CLK_TZASC_ACLK = 235,
    IMX6UL_CLK_CA7_CCM_DAP = 236,
    IMX6UL_CLK_CA7_CLKDIV_PATCH_IPG = 237,
    IMX6UL_CLK_MMDC_CORE_IPG_CLK_P1 = 238,
    IMX6UL_CLK_PCIE_ROOT = 239,
    IMX6UL_CLK_IOMUXC = 240,
    IMX6UL_CLK_IOMUXC_GPR = 241,
    IMX6UL_CLK_SIM_CPU = 242,
    IMX6UL_CLK_CXAPBSYNCBRIDGE_SLAVE = 243,
    IMX6UL_CLK_TSC_DIG = 244,
    IMX6UL_CLK_PL301_MX6QPER2_MAINCLK = 245,
    IMX6UL_CLK_STCR = 246,
    IMX6UL_CLK_SNVS_DRYICE = 247,
    IMX6UL_CLK_KPP = 248,
    IMX6UL_CLK_SIM_MAIN = 249,
    IMX6UL_CLK_SNVS_HP = 250,
    IMX6UL_CLK_SNVS_LP = 251,
    IMX6UL_CLK_ANADIG = 252,
    IMX6UL_CLK_ARM_A7_AHB = 253,
    IMX6UL_CLK_ARM_A7_ATB = 254,
    IMX6UL_CLK_ARM_A7_APB_DBG = 255,
    IMX6UL_CLK_ENET1_TX_CLK_INPUT = 256,
    IMX6UL_CLK_ENET2_TX_CLK_INPUT = 257,
    IMX6UL_CLK_ENET1_TX_REF_CLK = 258,
    IMX6UL_CLK_ENET2_TX_REF_CLK = 259,
    imx6ul_clk_max_count,
}aw_clk_id_t;

typedef uint32_t aw_clk_rate_t;
typedef uint64_t aw_clk_rate_tt;

typedef uint32_t aw_clk_reg_t;

aw_local aw_inline aw_clk_reg_t clk_readl(void *p_reg)
{
    return readl(p_reg);
}

aw_local aw_inline void clk_writel(aw_clk_reg_t val, void *reg)
{
    writel(val, reg);
}

#endif
