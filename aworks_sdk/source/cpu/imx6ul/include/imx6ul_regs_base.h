/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL各模块寄存器基址
 *
 * 提供各模块的寄存器组的物理基地址
 *
 * \par 1.兼容设备
 *
 *  - 飞思卡尔IMX6ULMCU
 *
 * \internal
 * \par modification history
 * - 1.00 2015.09.30  jac, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_REGS_BASE_H
#define __IMX6UL_REGS_BASE_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_imx6ul_bsp IMX6UL BSP支持
 * @{
 */
/**
 * \addtogroup grp_imx6ul_bsp_modules_base 各模块基地址
 * @{
 */

/** \brief IMX6UL On-Chip RAM base address */
#define IMX6UL_OCRAM_BASE_ADDR              0x00900000

/** \brief IMX6UL iomuxc register base address */
#define IMX6UL_IOMUXC_REGS_BASE_ADDR        0x020E0000
/** \brief IMX6UL iomuxc gpr register base address */
#define IMX6UL_IOMUXC_GPR_REGS_BASE_ADDR    0x020E4000

/** \brief IMX6UL UART1 register base address */
#define IMX6UL_UART1_BASE_ADDR              0x02020000
/** \brief IMX6UL UART2 register base address */
#define IMX6UL_UART2_BASE_ADDR              0x021E8000
/** \brief IMX6UL UART3 register base address */
#define IMX6UL_UART3_BASE_ADDR              0x021EC000
/** \brief IMX6UL UART4 register base address */
#define IMX6UL_UART4_BASE_ADDR              0x021F0000
/** \brief IMX6UL UART5 register base address */
#define IMX6UL_UART5_BASE_ADDR              0x021F4000
/** \brief IMX6UL UART6 register base address */
#define IMX6UL_UART6_BASE_ADDR              0x021FC000
/** \brief IMX6UL UART7 register base address */
#define IMX6UL_UART7_BASE_ADDR              0x02018000
/** \brief IMX6UL UART8 register base address */
#define IMX6UL_UART8_BASE_ADDR              0x02024000

/** \brief IMX6UL OCOTP controller register base address */
#define IMX6UL_OCOTP_BASE_ADDR              0x021BC000

/** \brief IMX6UL GPIO1 register base address */
#define IMX6UL_GPIO1_BASE_ADDR              0x0209C000

/** \brief IMX6UL CAAM register base address */
#define IMX6UL_CAAM_BASE_ADDR               0x02140000
/** \brief IMX6UL CAAM register base address */
#define IMX6UL_CAAM_SECURE_MEM_BASE_ADDR    0x00100000
#define IMX6UL_CAAM_SECURE_MEM_LENGTH       0x8000
#define IMX6UL_CAAM_JR0_ADDR                (IMX6UL_CAAM_BASE_ADDR + 0x1000)
#define IMX6UL_CAAM_JR1_ADDR                (IMX6UL_CAAM_BASE_ADDR + 0x2000)
#define IMX6UL_CAAM_JR2_ADDR                (IMX6UL_CAAM_BASE_ADDR + 0x3000)

/** \brief IMX6UL USDHC1 register base address */
#define IMX6UL_USDHC1_BASE_ADDR             (0x02190000)
/** \brief IMX6UL USDHC2 register base address */
#define IMX6UL_USDHC2_BASE_ADDR             (0x02194000)
/** \brief IMX6UL ENET2 register base address */
#define IMX6UL_ENET1_BASE_ADDR              0x02188000
/** \brief IMX6UL ENET2 register base address */
#define IMX6UL_ENET2_BASE_ADDR              0x020B4000

/** \brief IMX6UL I2C1 register base address */
#define IMX6UL_I2C1_BASE_ADDR               0x021A0000

/** \brief IMX6UL I2C2 register base address */
#define IMX6UL_I2C2_BASE_ADDR               0x021A4000

/** \brief IMX6UL I2C3 register base address */
#define IMX6UL_I2C3_BASE_ADDR               0x021A8000

/** \brief IMX6UL I2C4 register base address */
#define IMX6UL_I2C4_BASE_ADDR               0x021F8000

/** \brief IMX6UL GPT1 register base address */
#define IMX6UL_GPT1_BASE_ADDR               0x02098000

/** \brief IMX6UL GPT2 register base address */
#define IMX6UL_GPT2_BASE_ADDR               0x020E8000
/** \brief IMX6UL SDMA register base address */
#define IMX6UL_SDMA_BASE_ADDR               0x020EC000

/** \brief IMX6UL ADC1 register base address */
#define IMX6UL_ADC1_BASE_ADDR               0x02198000

/** \brief IMX6UL ADC2 register base address */
#define IMX6UL_ADC2_BASE_ADDR               0x0219C000

/** \brief IMX6UL CAN1 register base address */
#define IMX6UL_CAN1_BASE_ADDR               0x02090000

/** \brief IMX6UL CAN2 register base address */
#define IMX6UL_CAN2_BASE_ADDR               0x02094000

/** \brief IMX6UL eLCDIF register base address */
#define IMX6UL_LCDIF_BASE_ADDR              0x021C8000

/** \brief IMX6UL Touch Screen Controler register base address */
#define IMX6UL_TS_BASE_ADDR                 0x02040000

/** \brief IMX6UL Temperature Monitor register base address */
#define IMX6UL_TEMPMON_BASE_ADDR            0x020C8180

/** \brief IMX6UL WDT1 register base address */
#define IMX6UL_WDT1_BASE_ADDR               0x020BC000

/** \brief IMX6UL WDT2 register base address */
#define IMX6UL_WDT2_BASE_ADDR               0x020C0000

/** \brief IMX6UL WDT3 register base address */
#define IMX6UL_WDT3_BASE_ADDR               0x021E4000

/** \brief IMX6UL ECSPI1 register base address */
#define IMX6UL_ECSPI1_BASE_ADDR             0x02008000

/** \brief IMX6UL ECSPI2 register base address */
#define IMX6UL_ECSPI2_BASE_ADDR             0x0200C000

/** \brief IMX6UL ECSPI2 register base address */
#define IMX6UL_ECSPI3_BASE_ADDR             0x02010000

/** \brief IMX6UL ECSPI3 register base address */
#define IMX6UL_ECSPI4_BASE_ADDR             0x02014000

/** \brief IMX6UL SAI1 register base address */
#define IMX6UL_SAI1_BASE_ADDR               0x02028000

/** \brief IMX6UL SAI2 register base address */
#define IMX6UL_SAI2_BASE_ADDR               0x0202C000

/** \brief IMX6UL SAI3 register base address */
#define IMX6UL_SAI3_BASE_ADDR               0x02030000

/** \brief IMX6UL GPMI register base address */
#define IMX6UL_GPMI_BASE_ADDR               0x01806000

/** \brief IMX6UL BCH register base address */
#define IMX6UL_BCH_BASE_ADDR                0x01808000

/** \brief IMX6UL APBH DMA register base address */
#define IMX6UL_APBHDMA_BASE_ADDR            0x01804000

/** \brief IMX6UL USB_OTG1 register base address */
#define IMX6UL_USBCTRL1_BASE_ADDR           0x02184000

/** \brief IMX6UL USB_OTG2 register base address */
#define IMX6UL_USBCTRL2_BASE_ADDR           0x02184200

/** \brief IMX6UL ANALOG register base address */
#define IMX6UL_ANALOG_DIG_BASE_ADDR         0x020C8000

/** \brief IMX6UL USB_OTG1 register base address */
#define IMX6UL_USBPHY1_BASE_ADDR            0x020C9000

/** \brief IMX6UL USB_OTG2 register base address */
#define IMX6UL_USBPHY2_BASE_ADDR            0x020CA000

/** \brief PMU_REG_3P0's address */
#define IMX6UL_PMU_REG_3P0                  0x20C8120

/** \brief IMX6UL SIM1 register base address */
#define IMX6UL_SIM1_BASE_ADDR               0x0218C000

/** \brief IMX6UL SIM2 register base address */
#define IMX6UL_SIM2_BASE_ADDR               0x021B4000

/** \brief IMX6UL QSPI register base address */
#define IMX6UL_QSPI_BASE_ADDR               0x021E0000

/** \brief IMX6UL CSI register base address */
#define IMX6UL_CSI_BASE_ADDR                0x021C4000

/** \brief IMX6UL EIM register base address */
#define IMX6UL_EIM_BASE_ADDR                0x021B8000

/** @} end of grp_imx6ul_bsp_modules_base */
/** @} end of grp_imx6ul_bsp */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __IMX6UL_CLOCK_CFG_H */

/* end of file */
