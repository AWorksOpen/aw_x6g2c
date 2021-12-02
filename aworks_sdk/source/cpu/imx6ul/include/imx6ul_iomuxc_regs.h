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
 * \brief 定义操作时钟的接口
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-18  randy, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_IOMUXC_REGS_H
#define __IMX6UL_IOMUXC_REGS_H

#include "apollo.h"
#include "aw_spinlock.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


//-------------------------------------------------------------------------------------------
// hw_ccm_analog_t - module struct
//-------------------------------------------------------------------------------------------
/*!
 * @brief All CCM_ANALOG module registers.
 */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
    volatile uint32_t IOMUXC_GPR_GPR0;
    volatile uint32_t IOMUXC_GPR_GPR1;
    volatile uint32_t IOMUXC_GPR_GPR2;
    volatile uint32_t IOMUXC_GPR_GPR3;
    volatile uint32_t IOMUXC_GPR_GPR4;
    volatile uint32_t IOMUXC_GPR_GPR5;
    volatile uint32_t IOMUXC_GPR_GPR6;
    volatile uint32_t IOMUXC_GPR_GPR7;
    volatile uint32_t IOMUXC_GPR_GPR8;
    volatile uint32_t IOMUXC_GPR_GPR9;
    volatile uint32_t IOMUXC_GPR_GPR10;
} hw_iomuxc_gpr_regs_t;

typedef struct
{
    uint32_t reserved[5];
    uint32_t SW_MUX_CTL_PAD_BOOT_MODE0;         /* offset 0014 */
    uint32_t SW_MUX_CTL_PAD_BOOT_MODE1;         /* offset 0018 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER0;       /* offset 001C */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER1;       /* offset 0020 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER2;       /* offset 0024 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER3;       /* offset 0028 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER4;       /* offset 002C */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER5;       /* offset 0030 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER6;       /* offset 0034 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER7;       /* offset 0038 */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER8;       /* offset 003C */
    uint32_t SW_MUX_CTL_PAD_SNVS_TAMPER9;       /* offset 0040 */
    uint32_t SW_MUX_CTL_PAD_JTAG_MOD;           /* offset 0044 */
    uint32_t SW_MUX_CTL_PAD_JTAG_TMS;           /* offset 0048 */
    uint32_t SW_MUX_CTL_PAD_JTAG_TDO;           /* offset 004C */
    uint32_t SW_MUX_CTL_PAD_JTAG_TDI;           /* offset 0050 */
    uint32_t SW_MUX_CTL_PAD_JTAG_TCK;           /* offset 0054 */
    uint32_t SW_MUX_CTL_PAD_JTAG_TRST_B;        /* offset 0058 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO00;         /* offset 005C */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO01;         /* offset 0060 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO02;         /* offset 0064 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO03;         /* offset 0068 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO04;         /* offset 006C */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO05;         /* offset 0070 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO06;         /* offset 0074 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO07;         /* offset 0078 */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO08;         /* offset 007C */
    uint32_t SW_MUX_CTL_PAD_GPIO1_IO09;         /* offset 0080 */
    uint32_t SW_MUX_CTL_PAD_UART1_TX_DATA;      /* offset 0084 */
    uint32_t SW_MUX_CTL_PAD_UART1_RX_DATA;      /* offset 0088 */
    uint32_t SW_MUX_CTL_PAD_UART1_CTS_B;        /* offset 008C */
    uint32_t SW_MUX_CTL_PAD_UART1_RTS_B;        /* offset 0090 */
    uint32_t SW_MUX_CTL_PAD_UART2_TX_DATA;      /* offset 0094 */
    uint32_t SW_MUX_CTL_PAD_UART2_RX_DATA;      /* offset 0098 */
    uint32_t SW_MUX_CTL_PAD_UART2_CTS_B;        /* offset 009C */
    uint32_t SW_MUX_CTL_PAD_UART2_RTS_B;        /* offset 00A0 */
    uint32_t SW_MUX_CTL_PAD_UART3_TX_DATA;      /* offset 00A4 */
    uint32_t SW_MUX_CTL_PAD_UART3_RX_DATA;      /* offset 00A8 */
    uint32_t SW_MUX_CTL_PAD_UART3_CTS_B;        /* offset 00AC */
    uint32_t SW_MUX_CTL_PAD_UART3_RTS_B;        /* offset 00B0 */
    uint32_t SW_MUX_CTL_PAD_UART4_TX_DATA;      /* offset 00B4 */
    uint32_t SW_MUX_CTL_PAD_UART4_RX_DATA;      /* offset 00B8 */
    uint32_t SW_MUX_CTL_PAD_UART5_TX_DATA;      /* offset 00BC */
    uint32_t SW_MUX_CTL_PAD_UART5_RX_DATA;      /* offset 00C0 */
    uint32_t SW_MUX_CTL_PAD_ENET1_RX_DATA0;     /* offset 00C4 */
    uint32_t SW_MUX_CTL_PAD_ENET1_RX_DATA1;     /* offset 00C8 */
    uint32_t SW_MUX_CTL_PAD_ENET1_RX_EN;        /* offset 00CC */
    uint32_t SW_MUX_CTL_PAD_ENET1_TX_DATA0;     /* offset 00D0 */
    uint32_t SW_MUX_CTL_PAD_ENET1_TX_DATA1;     /* offset 00D4 */
    uint32_t SW_MUX_CTL_PAD_ENET1_TX_EN;        /* offset 00D8 */
    uint32_t SW_MUX_CTL_PAD_ENET1_TX_CLK;       /* offset 00DC */
    uint32_t SW_MUX_CTL_PAD_ENET1_RX_ER;        /* offset 00E0 */
    uint32_t SW_MUX_CTL_PAD_ENET2_RX_DATA0;     /* offset 00E4 */
    uint32_t SW_MUX_CTL_PAD_ENET2_RX_DATA1;     /* offset 00E8 */
    uint32_t SW_MUX_CTL_PAD_ENET2_RX_EN;        /* offset 00EC */
    uint32_t SW_MUX_CTL_PAD_ENET2_TX_DATA0;     /* offset 00F0 */
    uint32_t SW_MUX_CTL_PAD_ENET2_TX_DATA1;     /* offset 00F4 */
    uint32_t SW_MUX_CTL_PAD_ENET2_TX_EN;        /* offset 00F8 */
    uint32_t SW_MUX_CTL_PAD_ENET2_TX_CLK;       /* offset 00FC */

    uint32_t SW_MUX_CTL_PAD_ENET2_RX_ER;        /* offset 0100 */
    uint32_t SW_MUX_CTL_PAD_LCD_CLK;            /* offset 0104 */
    uint32_t SW_MUX_CTL_PAD_LCD_ENABLE;         /* offset 0108 */
    uint32_t SW_MUX_CTL_PAD_LCD_HSYNC;          /* offset 010C */
    uint32_t SW_MUX_CTL_PAD_LCD_VSYNC;          /* offset 0110 */
    uint32_t SW_MUX_CTL_PAD_LCD_RESET;          /* offset 0114 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA00;         /* offset 0118 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA01;         /* offset 011C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA02;         /* offset 0120 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA03;         /* offset 0124 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA04;         /* offset 0128 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA05;         /* offset 012C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA06;         /* offset 0130 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA07;         /* offset 0134 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA08;         /* offset 0138 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA09;         /* offset 013C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA10;         /* offset 0140 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA11;         /* offset 0144 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA12;         /* offset 0148 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA13;         /* offset 014C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA14;         /* offset 0150 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA15;         /* offset 0154 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA16;         /* offset 0158 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA17;         /* offset 015C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA18;         /* offset 0160 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA19;         /* offset 0164 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA20;         /* offset 0168 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA21;         /* offset 016C */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA22;         /* offset 0170 */
    uint32_t SW_MUX_CTL_PAD_LCD_DATA23;         /* offset 0174 */
    uint32_t SW_MUX_CTL_PAD_NAND_RE_B;          /* offset 0178 */
    uint32_t SW_MUX_CTL_PAD_NAND_WE_B;          /* offset 017C */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA00;        /* offset 0180 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA01;        /* offset 0184 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA02;        /* offset 0188 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA03;        /* offset 018C */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA04;        /* offset 0190 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA05;        /* offset 0194 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA06;        /* offset 0198 */
    uint32_t SW_MUX_CTL_PAD_NAND_DATA07;        /* offset 019C */
    uint32_t SW_MUX_CTL_PAD_NAND_ALE;           /* offset 01A0 */
    uint32_t SW_MUX_CTL_PAD_NAND_WP_B;          /* offset 01A4 */
    uint32_t SW_MUX_CTL_PAD_NAND_READY_B;       /* offset 01A8 */
    uint32_t SW_MUX_CTL_PAD_NAND_CE0_B;         /* offset 01AC */
    uint32_t SW_MUX_CTL_PAD_NAND_CE1_B;         /* offset 01B0 */
    uint32_t SW_MUX_CTL_PAD_NAND_CLE;           /* offset 01B4 */
    uint32_t SW_MUX_CTL_PAD_NAND_DQS;           /* offset 01B8 */
    uint32_t SW_MUX_CTL_PAD_SD1_CMD;            /* offset 01BC */
    uint32_t SW_MUX_CTL_PAD_SD1_CLK;            /* offset 01C0 */
    uint32_t SW_MUX_CTL_PAD_SD1_DATA0;          /* offset 01C4 */
    uint32_t SW_MUX_CTL_PAD_SD1_DATA1;          /* offset 01C8 */
    uint32_t SW_MUX_CTL_PAD_SD1_DATA2;          /* offset 01CC */
    uint32_t SW_MUX_CTL_PAD_SD1_DATA3;          /* offset 01D0 */
    uint32_t SW_MUX_CTL_PAD_CSI_MCLK;           /* offset 01D4 */
    uint32_t SW_MUX_CTL_PAD_CSI_PIXCLK;         /* offset 01D8 */
    uint32_t SW_MUX_CTL_PAD_CSI_VSYNC;          /* offset 01DC */
    uint32_t SW_MUX_CTL_PAD_CSI_HSYNC;          /* offset 01E0 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA00;         /* offset 01E4 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA01;         /* offset 01E8 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA02;         /* offset 01EC */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA03;         /* offset 01F0 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA04;         /* offset 01F4 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA05;         /* offset 01F8 */
    uint32_t SW_MUX_CTL_PAD_CSI_DATA06;         /* offset 01FC */

    uint32_t SW_MUX_CTL_PAD_CSI_DATA07;         /* offset 0200 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR00;        /* offset 0204 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR01;        /* offset 0208 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR02;        /* offset 020C */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR03;        /* offset 0210 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR04;        /* offset 0214 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR05;        /* offset 0218 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR06;        /* offset 021C */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR07;        /* offset 0220 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR08;        /* offset 0224 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR09;        /* offset 0228 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR10;        /* offset 022C */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR11;        /* offset 0230 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR12;        /* offset 0234 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR13;        /* offset 0238 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR14;        /* offset 023C */
    uint32_t SW_PAD_CTL_PAD_DRAM_ADDR15;        /* offset 0240 */
    uint32_t SW_PAD_CTL_PAD_DRAM_DQM0;          /* offset 0244 */
    uint32_t SW_PAD_CTL_PAD_DRAM_DQM1;          /* offset 0248 */
    uint32_t SW_PAD_CTL_PAD_DRAM_RAS_B;         /* offset 024C */
    uint32_t SW_PAD_CTL_PAD_DRAM_CAS_B;         /* offset 0250 */
    uint32_t SW_PAD_CTL_PAD_DRAM_CS0_B;         /* offset 0254 */
    uint32_t SW_PAD_CTL_PAD_DRAM_CS1_B;         /* offset 0258 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDWE_B;        /* offset 025C */
    uint32_t SW_PAD_CTL_PAD_DRAM_ODT0;          /* offset 0260 */
    uint32_t SW_PAD_CTL_PAD_DRAM_ODT1;          /* offset 0264 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDBA0;         /* offset 0268 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDBA1;         /* offset 026C */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDBA2;         /* offset 0270 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDCKE0;        /* offset 0274 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDCKE1;        /* offset 0278 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDCLK0_P;      /* offset 027C */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDQS0_P;       /* offset 0280 */
    uint32_t SW_PAD_CTL_PAD_DRAM_SDQS1_P;       /* offset 0284 */
    uint32_t SW_PAD_CTL_PAD_DRAM_RESET;         /* offset 0288 */
    uint32_t SW_PAD_CTL_PAD_TEST_MODE;          /* offset 028C */
    uint32_t SW_PAD_CTL_PAD_POR_B;              /* offset 0290 */
    uint32_t SW_PAD_CTL_PAD_ONOFF;              /* offset 0294 */
    uint32_t SW_PAD_CTL_PAD_SNVS_PMIC_ON_REQ;   /* offset 0298 */
    uint32_t SW_PAD_CTL_PAD_CCM_PMIC_STBY_REQ;  /* offset 029C */
    uint32_t SW_PAD_CTL_PAD_BOOT_MODE0;         /* offset 02A0 */
    uint32_t SW_PAD_CTL_PAD_BOOT_MODE1;         /* offset 02A4 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER0;       /* offset 02A8 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER1;       /* offset 02AC */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER2;       /* offset 02B0 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER3;       /* offset 02B4 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER4;       /* offset 02B8 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER5;       /* offset 02BC */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER6;       /* offset 02C0 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER7;       /* offset 02C4 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER8;       /* offset 02C8 */
    uint32_t SW_PAD_CTL_PAD_SNVS_TAMPER9;       /* offset 02CC */
    uint32_t SW_PAD_CTL_PAD_JTAG_MOD;           /* offset 02D0 */
    uint32_t SW_PAD_CTL_PAD_JTAG_TMS;           /* offset 02D4 */
    uint32_t SW_PAD_CTL_PAD_JTAG_TDO;           /* offset 02D8 */
    uint32_t SW_PAD_CTL_PAD_JTAG_TDI;           /* offset 02DC */
    uint32_t SW_PAD_CTL_PAD_JTAG_TCK;           /* offset 02E0 */
    uint32_t SW_PAD_CTL_PAD_JTAG_TRST_B;        /* offset 02E4 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO00;         /* offset 02E8 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO01;         /* offset 02EC */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO02;         /* offset 02F0 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO03;         /* offset 02F4 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO04;         /* offset 02F8 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO05;         /* offset 02FC */


    uint32_t SW_PAD_CTL_PAD_GPIO1_IO06;         /* offset 0300 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO07;         /* offset 0304 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO08;         /* offset 0308 */
    uint32_t SW_PAD_CTL_PAD_GPIO1_IO09;         /* offset 030C */
    uint32_t SW_PAD_CTL_PAD_UART1_TX_DATA;      /* offset 0310 */
    uint32_t SW_PAD_CTL_PAD_UART1_RX_DATA;      /* offset 0314 */
    uint32_t SW_PAD_CTL_PAD_UART1_CTS_B;        /* offset 0318 */
    uint32_t SW_PAD_CTL_PAD_UART1_RTS_B;        /* offset 031C */
    uint32_t SW_PAD_CTL_PAD_UART2_TX_DATA;      /* offset 0320 */
    uint32_t SW_PAD_CTL_PAD_UART2_RX_DATA;      /* offset 0324 */
    uint32_t SW_PAD_CTL_PAD_UART2_CTS_B;        /* offset 0328 */
    uint32_t SW_PAD_CTL_PAD_UART2_RTS_B;        /* offset 032C */
    uint32_t SW_PAD_CTL_PAD_UART3_TX_DATA;      /* offset 0330 */
    uint32_t SW_PAD_CTL_PAD_UART3_RX_DATA;      /* offset 0334 */
    uint32_t SW_PAD_CTL_PAD_UART3_CTS_B;        /* offset 0338 */
    uint32_t SW_PAD_CTL_PAD_UART3_RTS_B;        /* offset 033C */
    uint32_t SW_PAD_CTL_PAD_UART4_TX_DATA;      /* offset 0340 */
    uint32_t SW_PAD_CTL_PAD_UART4_RX_DATA;      /* offset 0344 */
    uint32_t SW_PAD_CTL_PAD_UART5_TX_DATA;      /* offset 0348 */
    uint32_t SW_PAD_CTL_PAD_UART5_RX_DATA;      /* offset 034C */
    uint32_t SW_PAD_CTL_PAD_ENET1_RX_DATA0;     /* offset 0350 */
    uint32_t SW_PAD_CTL_PAD_ENET1_RX_DATA1;     /* offset 0354 */
    uint32_t SW_PAD_CTL_PAD_ENET1_RX_EN;        /* offset 0358 */
    uint32_t SW_PAD_CTL_PAD_ENET1_TX_DATA0;     /* offset 035C */
    uint32_t SW_PAD_CTL_PAD_ENET1_TX_DATA1;     /* offset 0360 */
    uint32_t SW_PAD_CTL_PAD_ENET1_TX_EN;        /* offset 0364 */
    uint32_t SW_PAD_CTL_PAD_ENET1_TX_CLK;       /* offset 0368 */
    uint32_t SW_PAD_CTL_PAD_ENET1_RX_ER;        /* offset 036C */
    uint32_t SW_PAD_CTL_PAD_ENET2_RX_DATA0;     /* offset 0370 */
    uint32_t SW_PAD_CTL_PAD_ENET2_RX_DATA1;     /* offset 0374 */
    uint32_t SW_PAD_CTL_PAD_ENET2_RX_EN;        /* offset 0378 */
    uint32_t SW_PAD_CTL_PAD_ENET2_TX_DATA0;     /* offset 037C */
    uint32_t SW_PAD_CTL_PAD_ENET2_TX_DATA1;     /* offset 0380 */
    uint32_t SW_PAD_CTL_PAD_ENET2_TX_EN;        /* offset 0384 */
    uint32_t SW_PAD_CTL_PAD_ENET2_TX_CLK;       /* offset 0388 */
    uint32_t SW_PAD_CTL_PAD_ENET2_RX_ER;        /* offset 038C */
    uint32_t SW_PAD_CTL_PAD_LCD_CLK;            /* offset 0390 */
    uint32_t SW_PAD_CTL_PAD_LCD_ENABLE;         /* offset 0394 */
    uint32_t SW_PAD_CTL_PAD_LCD_HSYNC;          /* offset 0398 */
    uint32_t SW_PAD_CTL_PAD_LCD_VSYNC;          /* offset 039C */
    uint32_t SW_PAD_CTL_PAD_LCD_RESET;          /* offset 03A0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA00;         /* offset 03A4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA01;         /* offset 03A8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA02;         /* offset 03AC */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA03;         /* offset 03B0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA04;         /* offset 03B4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA05;         /* offset 03B8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA06;         /* offset 03BC */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA07;         /* offset 03C0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA08;         /* offset 03C4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA09;         /* offset 03C8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA10;         /* offset 03CC */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA11;         /* offset 03D0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA12;         /* offset 03D4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA13;         /* offset 03D8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA14;         /* offset 03DC */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA15;         /* offset 03E0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA16;         /* offset 03E4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA17;         /* offset 03E8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA18;         /* offset 03EC */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA19;         /* offset 03F0 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA20;         /* offset 03F4 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA21;         /* offset 03F8 */
    uint32_t SW_PAD_CTL_PAD_LCD_DATA22;         /* offset 03FC */


    uint32_t SW_PAD_CTL_PAD_LCD_DATA23;         /* offset 0400 */
    uint32_t SW_PAD_CTL_PAD_NAND_RE_B;          /* offset 0404 */
    uint32_t SW_PAD_CTL_PAD_NAND_WE_B;          /* offset 0408 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA00;        /* offset 040C */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA01;        /* offset 0410 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA02;        /* offset 0414 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA03;        /* offset 0418 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA04;        /* offset 041C */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA05;        /* offset 0420 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA06;        /* offset 0424 */
    uint32_t SW_PAD_CTL_PAD_NAND_DATA07;        /* offset 0428 */
    uint32_t SW_PAD_CTL_PAD_NAND_ALE;           /* offset 042C */
    uint32_t SW_PAD_CTL_PAD_NAND_WP_B;          /* offset 0430 */
    uint32_t SW_PAD_CTL_PAD_NAND_READY_B;       /* offset 0434 */
    uint32_t SW_PAD_CTL_PAD_NAND_CE0_B;         /* offset 0438 */
    uint32_t SW_PAD_CTL_PAD_NAND_CE1_B;         /* offset 043C */
    uint32_t SW_PAD_CTL_PAD_NAND_CLE;           /* offset 0440 */
    uint32_t SW_PAD_CTL_PAD_NAND_DQS;           /* offset 0444 */
    uint32_t SW_PAD_CTL_PAD_SD1_CMD;            /* offset 0448 */
    uint32_t SW_PAD_CTL_PAD_SD1_CLK;            /* offset 044C */
    uint32_t SW_PAD_CTL_PAD_SD1_DATA0;          /* offset 0450 */
    uint32_t SW_PAD_CTL_PAD_SD1_DATA1;          /* offset 0454 */
    uint32_t SW_PAD_CTL_PAD_SD1_DATA2;          /* offset 0458 */
    uint32_t SW_PAD_CTL_PAD_SD1_DATA3;          /* offset 045C */
    uint32_t SW_PAD_CTL_PAD_CSI_MCLK;           /* offset 0460 */
    uint32_t SW_PAD_CTL_PAD_CSI_PIXCLK;         /* offset 0464 */
    uint32_t SW_PAD_CTL_PAD_CSI_VSYNC;          /* offset 0468 */
    uint32_t SW_PAD_CTL_PAD_CSI_HSYNC;          /* offset 046C */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA00;         /* offset 0470 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA01;         /* offset 0474 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA02;         /* offset 0478 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA03;         /* offset 047C */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA04;         /* offset 0480 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA05;         /* offset 0484 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA06;         /* offset 0488 */
    uint32_t SW_PAD_CTL_PAD_CSI_DATA07;         /* offset 048C */
    uint32_t SW_PAD_CTL_GRP_ADDDS;              /* offset 0490 */
    uint32_t SW_PAD_CTL_GRP_DDRMODE_CTL;        /* offset 0494 */
    uint32_t SW_PAD_CTL_GRP_B0DS;               /* offset 0498 */
    uint32_t SW_PAD_CTL_GRP_DDRPK;              /* offset 049C */
    uint32_t SW_PAD_CTL_GRP_CTLDS;              /* offset 04A0 */
    uint32_t SW_PAD_CTL_GRP_B1DS;               /* offset 04A4 */
    uint32_t SW_PAD_CTL_GRP_DDRHYS;             /* offset 04A8 */
    uint32_t SW_PAD_CTL_GRP_DDRPKE;             /* offset 04AC */
    uint32_t SW_PAD_CTL_GRP_DDRMODE;            /* offset 04B0 */
    uint32_t SW_PAD_CTL_GRP_DDR_TYPE;           /* offset 04B4 */
    uint32_t USB_OTG1_ID_SELECT_INPUT;          /* offset 04B8 */
    uint32_t USB_OTG2_ID_SELECT_INPUT;          /* offset 04BC */
    uint32_t CCM_PMIC_READY_SELECT_INPUT;       /* offset 04C0 */
    uint32_t CSI_DATA02_SELECT_INPUT;           /* offset 04C4 */
    uint32_t CSI_DATA03_SELECT_INPUT;           /* offset 04C8 */
    uint32_t CSI_DATA05_SELECT_INPUT;           /* offset 04CC */
    uint32_t CSI_DATA00_SELECT_INPUT;           /* offset 04D0 */
    uint32_t CSI_DATA01_SELECT_INPUT;           /* offset 04D4 */
    uint32_t CSI_DATA04_SELECT_INPUT;           /* offset 04D8 */
    uint32_t CSI_DATA06_SELECT_INPUT;           /* offset 04DC */
    uint32_t CSI_DATA07_SELECT_INPUT;           /* offset 04E0 */
    uint32_t CSI_DATA08_SELECT_INPUT;           /* offset 04E4 */
    uint32_t CSI_DATA09_SELECT_INPUT;           /* offset 04E8 */
    uint32_t CSI_DATA10_SELECT_INPUT;           /* offset 04EC */
    uint32_t CSI_DATA11_SELECT_INPUT;           /* offset 04F0 */
    uint32_t CSI_DATA12_SELECT_INPUT;           /* offset 04F4 */
    uint32_t CSI_DATA13_SELECT_INPUT;           /* offset 04F8 */
    uint32_t CSI_DATA14_SELECT_INPUT;           /* offset 04FC */


    uint32_t CSI_DATA15_SELECT_INPUT;           /* offset 0500 */
    uint32_t CSI_DATA16_SELECT_INPUT;           /* offset 0504 */
    uint32_t CSI_DATA17_SELECT_INPUT;           /* offset 0508 */
    uint32_t CSI_DATA18_SELECT_INPUT;           /* offset 050C */
    uint32_t CSI_DATA19_SELECT_INPUT;           /* offset 0510 */
    uint32_t CSI_DATA20_SELECT_INPUT;           /* offset 0514 */
    uint32_t CSI_DATA21_SELECT_INPUT;           /* offset 0518 */
    uint32_t CSI_DATA22_SELECT_INPUT;           /* offset 051C */
    uint32_t CSI_DATA23_SELECT_INPUT;           /* offset 0520 */
    uint32_t CSI_HSYNC_SELECT_INPUT;            /* offset 0524 */
    uint32_t CSI_PIXCLK_SELECT_INPUT;           /* offset 0528 */
    uint32_t CSI_VSYNC_SELECT_INPUT;            /* offset 052C */
    uint32_t CSI_FIELD_SELECT_INPUT;            /* offset 0530 */
    uint32_t ECSPI1_SCLK_SELECT_INPUT;          /* offset 0534 */
    uint32_t ECSPI1_MISO_SELECT_INPUT;          /* offset 0538 */
    uint32_t ECSPI1_MOSI_SELECT_INPUT;          /* offset 053C */
    uint32_t ECSPI1_SS0_B_SELECT_INPUT;         /* offset 0540 */
    uint32_t ECSPI2_SCLK_SELECT_INPUT;          /* offset 0544 */
    uint32_t ECSPI2_MISO_SELECT_INPUT;          /* offset 0548 */
    uint32_t ECSPI2_MOSI_SELECT_INPUT;          /* offset 054C */
    uint32_t ECSPI2_SS0_B_SELECT_INPUT;         /* offset 0550 */
    uint32_t ECSPI3_SCLK_SELECT_INPUT;          /* offset 0554 */
    uint32_t ECSPI3_MISO_SELECT_INPUT;          /* offset 0558 */
    uint32_t ECSPI3_MOSI_SELECT_INPUT;          /* offset 055C */
    uint32_t ECSPI3_SS0_B_SELECT_INPUT;         /* offset 0560 */
    uint32_t ECSPI4_SCLK_SELECT_INPUT;          /* offset 0564 */
    uint32_t ECSPI4_MISO_SELECT_INPUT;          /* offset 0568 */
    uint32_t ECSPI4_MOSI_SELECT_INPUT;          /* offset 056C */
    uint32_t ECSPI4_SS0_B_SELECT_INPUT;         /* offset 0570 */
    uint32_t ENET1_REF_CLK1_SELECT_INPUT;       /* offset 0574 */
    uint32_t ENET1_MAC0_MDIO_SELECT_INPUT;      /* offset 0578 */
    uint32_t ENET2_REF_CLK2_SELECT_INPUT;       /* offset 057C */
    uint32_t ENET2_MAC0_MDIO_SELECT_INPUT;      /* offset 0580 */
    uint32_t FLEXCAN1_RX_SELECT_INPUT;          /* offset 0584 */
    uint32_t FLEXCAN2_RX_SELECT_INPUT;          /* offset 0588 */
    uint32_t GPT1_CAPTURE1_SELECT_INPUT;        /* offset 058C */
    uint32_t GPT1_CAPTURE2_SELECT_INPUT;        /* offset 0590 */
    uint32_t GPT1_CLK_SELECT_INPUT;             /* offset 0594 */
    uint32_t GPT2_CAPTURE1_SELECT_INPUT;        /* offset 0598 */
    uint32_t GPT2_CAPTURE2_SELECT_INPUT;        /* offset 059C */
    uint32_t GPT2_CLK_SELECT_INPUT;             /* offset 05A0 */
    uint32_t I2C1_SCL_SELECT_INPUT;             /* offset 05A4 */
    uint32_t I2C1_SDA_SELECT_INPUT;             /* offset 05A8 */
    uint32_t I2C2_SCL_SELECT_INPUT;             /* offset 05AC */
    uint32_t I2C2_SDA_SELECT_INPUT;             /* offset 05B0 */
    uint32_t I2C3_SCL_SELECT_INPUT;             /* offset 05B4 */
    uint32_t I2C3_SDA_SELECT_INPUT;             /* offset 05B8 */
    uint32_t I2C4_SCL_SELECT_INPUT;             /* offset 05BC */
    uint32_t I2C4_SDA_SELECT_INPUT;             /* offset 05C0 */
    uint32_t KPP_COL0_SELECT_INPUT;             /* offset 05C4 */
    uint32_t KPP_COL1_SELECT_INPUT;             /* offset 05C8 */
    uint32_t KPP_COL2_SELECT_INPUT;             /* offset 05CC */
    uint32_t KPP_ROW0_SELECT_INPUT;             /* offset 05D0 */
    uint32_t KPP_ROW1_SELECT_INPUT;             /* offset 05D4 */
    uint32_t KPP_ROW2_SELECT_INPUT;             /* offset 05D8 */
    uint32_t LCD_BUSY_SELECT_INPUT;             /* offset 05DC */
    uint32_t SAI1_MCLK_SELECT_INPUT;            /* offset 05E0 */
    uint32_t SAI1_RX_DATA_SELECT_INPUT;         /* offset 05E4 */
    uint32_t SAI1_TX_BCLK_SELECT_INPUT;         /* offset 05E8 */
    uint32_t SAI1_TX_SYNC_SELECT_INPUT;         /* offset 05EC */
    uint32_t SAI2_MCLK_SELECT_INPUT;            /* offset 05F0 */
    uint32_t SAI2_RX_DATA_SELECT_INPUT;         /* offset 05F4 */
    uint32_t SAI2_TX_BCLK_SELECT_INPUT;         /* offset 05F8 */
    uint32_t SAI2_TX_SYNC_SELECT_INPUT;         /* offset 05FC */


    uint32_t SAI3_MCLK_SELECT_INPUT;            /* offset 0600 */
    uint32_t SAI3_RX_DATA_SELECT_INPUT;         /* offset 0604 */
    uint32_t SAI3_TX_BCLK_SELECT_INPUT;         /* offset 0608 */
    uint32_t SAI3_TX_SYNC_SELECT_INPUT;         /* offset 060C */
    uint32_t SDMA_EVENTS0_SELECT_INPUT;         /* offset 0610 */
    uint32_t SDMA_EVENTS1_SELECT_INPUT;         /* offset 0614 */
    uint32_t SPDIF_IN_SELECT_INPUT;             /* offset 0618 */
    uint32_t SPDIF_EXT_CLK_SELECT_INPUT;        /* offset 061C */
    uint32_t UART1_RTS_B_SELECT_INPUT;          /* offset 0620 */
    uint32_t UART1_RX_DATA_SELECT_INPUT;        /* offset 0624 */
    uint32_t UART2_RTS_B_SELECT_INPUT;          /* offset 0628 */
    uint32_t UART2_RX_DATA_SELECT_INPUT;        /* offset 062C */
    uint32_t UART3_RTS_B_SELECT_INPUT;          /* offset 0630 */
    uint32_t UART3_RX_DATA_SELECT_INPUT;        /* offset 0634 */
    uint32_t UART4_RTS_B_SELECT_INPUT;          /* offset 0638 */
    uint32_t UART4_RX_DATA_SELECT_INPUT;        /* offset 063C */
    uint32_t UART5_RTS_B_SELECT_INPUT;          /* offset 0640 */
    uint32_t UART5_RX_DATA_SELECT_INPUT;        /* offset 0644 */
    uint32_t UART6_RTS_B_SELECT_INPUT;          /* offset 0648 */
    uint32_t UART6_RX_DATA_SELECT_INPUT;        /* offset 064C */
    uint32_t UART7_RTS_B_SELECT_INPUT;          /* offset 0650 */
    uint32_t UART7_RX_DATA_SELECT_INPUT;        /* offset 0654 */
    uint32_t UART8_RTS_B_SELECT_INPUT;          /* offset 0658 */
    uint32_t UART8_RX_DATA_SELECT_INPUT;        /* offset 065C */
    uint32_t USB_OTG2_OC_SELECT_INPUT;          /* offset 0660 */
    uint32_t USB_OTG_OC_SELECT_INPUT;           /* offset 0664 */
    uint32_t USDHC1_CD_B_SELECT_INPUT;          /* offset 0668 */
    uint32_t USDHC1_WP_SELECT_INPUT;            /* offset 066C */
    uint32_t USDHC2_CLK_SELECT_INPUT;           /* offset 0670 */
    uint32_t USDHC2_CD_B_SELECT_INPUT;          /* offset 0674 */
    uint32_t USDHC2_CMD_SELECT_INPUT;           /* offset 0678 */
    uint32_t USDHC2_DATA0_SELECT_INPUT;         /* offset 067C */
    uint32_t USDHC2_DATA1_SELECT_INPUT;         /* offset 0680 */
    uint32_t USDHC2_DATA2_SELECT_INPUT;         /* offset 0684 */
    uint32_t USDHC2_DATA3_SELECT_INPUT;         /* offset 0688 */
    uint32_t USDHC2_DATA4_SELECT_INPUT;         /* offset 068C */
    uint32_t USDHC2_DATA5_SELECT_INPUT;         /* offset 0690 */
    uint32_t USDHC2_DATA6_SELECT_INPUT;         /* offset 0694 */
    uint32_t USDHC2_DATA7_SELECT_INPUT;         /* offset 0698 */
    uint32_t USDHC2_WP_SELECT_INPUT;            /* offset 069C */

} hw_iomuxc_regs_t;

#pragma pack(pop)


extern aw_spinlock_isr_t __spinlock_isr_iomuxc_gpr;

uint32_t iomuxc_reg_bits_get(uint32_t offset,uint32_t start,uint32_t len);
void iomuxc_reg_bits_set(uint32_t offset,uint32_t start,uint32_t len,uint32_t val);

#define IOMUX_GPR_REG_GET(reg,start,len) \
        iomuxc_reg_bits_get(AW_OFFSET(hw_iomuxc_gpr_regs_t,reg),start,len )

#define IOMUX_GPR_REG_MOD(reg,start,len,val) \
        iomuxc_reg_bits_set(AW_OFFSET(hw_iomuxc_gpr_regs_t,reg),start,len,val);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_IOMUXC_REGS_H */

/* end of file */
