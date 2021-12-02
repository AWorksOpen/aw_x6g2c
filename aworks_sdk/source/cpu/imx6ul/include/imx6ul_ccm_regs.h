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

#ifndef __IMX6UL_CCM_REGS_H
#define __IMX6UL_CCM_REGS_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*!
 * @brief HW_CCM_CCR - CCM Control Register (RW)
 *
 * Reset value: 0x040116ff
 *
 * The figure below represents the CCM Control Register (CCR), which contains bits to control
 * general operation of CCM. The table below provides its field descriptions.
 */
typedef union hw_ccm_cc
{
    uint32_t value;
    struct _hw_ccm_ccr_bitfields
    {
        unsigned OSCNT : 7;         //!< [6:0] Oscillator ready counter value.
        unsigned RESERVED0 : 5;     //!< [11:7] Reserved
        unsigned COSC_EN : 1;       //!< [12] On chip oscillator enable bit - this bit value is reflected on the output cosc_en.
        unsigned RESERVED1 : 3;     //!< [15:13] Reserved
        unsigned WB_COUNT : 3;      //!< [18:16] Well Bias counter.
        unsigned RESERVED2 : 2;     //!< [20:19] Reserved
        unsigned REG_BYPASS_COUNT : 6; //!< [26:21] Counter for analog_reg_bypass signal assertion after standby voltage request by pmic_vstby_req.
        unsigned RBC_EN : 1;        //!< [27] Enable for REG_BYPASS_COUNTER.
        unsigned RESERVED3 : 4;     //!< [31:28] Reserved
    }bits;
} hw_ccm_ccr_t;

/*!
 * @brief HW_CCM_CCDR - CCM Control Divider Register (RW)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Control Divider Register (CCDR), which contains bits that
 * control the loading of the dividers that need handshake with the modules they affect. The table
 * below provides its field descriptions.
 */
typedef union _hw_ccm_ccdr
{
    uint32_t value;
    struct _hw_ccm_ccdr_bitfields
    {
        unsigned RESERVED0 : 16;        //!< [15:0] Reserved
        unsigned MMDC_MASK : 1;         //!< [16] During divider ratio mmdc_ch1_axi_podf change or sync mux periph2_clk_sel change (but not jtag) or SRC request during warm reset, mask handshake with mmdc_ch1 module.
        unsigned RESERVED1 : 15;        //!< [31:17] Reserved
    }bits;
} hw_ccm_ccdr_t;

/*!
 * @brief HW_CCM_CSR - CCM Status Register (RO)
 *
 * Reset value: 0x00000010
 *
 * The figure below represents the CCM status Register (CSR). The status bits are read only bits.
 * The table below provides its field descriptions.
 */
typedef union _hw_ccm_csr
{
    uint32_t value;
    struct _hw_ccm_csr_bitfields
    {
        unsigned REF_EN_B : 1;      //!< [0] Status of the value of CCM_REF_EN_B output of ccm
        unsigned RESERVED0 : 4;     //!< [4:1] Reserved.
        unsigned COSC_READY : 1;    //!< [5] Status indication of on board oscillator.
        unsigned RESERVED1 : 26;    //!< [31:6] Reserved
    } bits;
} hw_ccm_csr_t;

/*!
 * @brief HW_CCM_CCSR - CCM Clock Swither Register (RW)
 *
 * Reset value: 0x00000100
 *
 * The figure below represents the CCM Clock Switcher register (CCSR). The CCSR register contains
 * bits to control the switcher sub module dividers and multiplexers. The table below provides its
 * field descriptions.
 */
typedef union _hw_ccm_ccsr
{
    uint32_t value;
    struct _hw_ccm_ccsr_bitfields
    {
        unsigned PLL3_SW_CLK_SEL : 1;   //!< [0] Selects source to generate pll3_sw_clk.
        unsigned PLL2_SW_CLK_SEL : 1;   //!< [1] Selects source to generate pll2_sw_clk.
        unsigned PLL1_SW_CLK_SEL : 1;   //!< [2] Selects source to generate pll1_sw_clk.
        unsigned CA7_SECONDARY_CLK_SEL : 1; //!< [3] CA7 secondary clock
        unsigned RESERVED0 : 4;         //!< [7:4] Reserved
        unsigned STEP_SEL : 1;          //!< [8] Selects the option to be chosen for the step frequency when shifting ARM frequency.
        unsigned RESERVED1 : 23;        //!< [31:9] Reserved
    } bits;
} hw_ccm_ccsr_t;

/*!
 * @brief HW_CCM_CACRR - CCM Arm Clock Root Register (RW)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Arm Clock Root register (CACRR). The CACRR register contains
 * bits to control the ARM clock root generation. The table below provides its field descriptions.
 */
typedef union _hw_ccm_cacrr
{
    uint32_t value;
    struct _hw_ccm_cacrr_bitfields
    {
        unsigned ARM_PODF : 3;      //!< [2:0] Divider for ARM clock root.
        unsigned RESERVED0 : 29;    //!< [31:3] Reserved
    } bits;
} hw_ccm_cacrr_t;

/*!
 * @brief HW_CCM_CBCDR - CCM Bus Clock Divider Register (RW)
 *
 * Reset value: 0x00018d40
 *
 * The figure below represents the CCM Bus Clock Divider Register (CBCDR). The CBCDR register
 * contains bits to control the clock generation sub module dividers. The table below provides its
 * field descriptions.
 */
typedef union _hw_ccm_cbcdr
{
    uint32_t value;
    struct _hw_ccm_cbcdr_bitfields
    {
        unsigned PERIPH2_CLK2_PODF : 3; //!< [2:0] Divider for periph2_clk2 podf.
        unsigned FABRIC_MMDC_PODF : 3;  //!< [5:3] Post divider for fabric / mmdc clock.
        unsigned OCRAM_CLK_SEL : 1;     //!< [6] OCRAM clock source select
        unsigned OCRAM_ALT_CLK_SEL : 1; //!< [7] OCRAM alternative clock select
        unsigned IPG_PODF : 2;          //!< [9:8] Divider for ipg podf.
        unsigned AHB_PODF : 3;          //!< [12:10] Divider for ahb podf.
        unsigned RESERVED0 : 3;         //!< [15:13] Reserved
        unsigned OCRAM_PODF : 3;        //!< [18:16] Post divider for ocram clock.
        unsigned RESERVED1 : 6;         //!< [24:19] Reserved
        unsigned PERIPH_CLK_SEL : 1;    //!< [25] Selector for peripheral main clock (source of mmdc_ch0_axi_clk_root).
        unsigned PERIPH2_CLK_SEL : 1;   //!< [26] Selector for peripheral2 main clock (source of mmdc_ch1_axi_clk_root ).
        unsigned PERIPH_CLK2_PODF : 3;  //!< [29:27] Divider for periph2 clock podf.
        unsigned RESERVED2 : 2;         //!< [31:30] Reserved
    } bits;
} hw_ccm_cbcdr_t;

/*!
 * @brief HW_CCM_CBCMR - CCM Bus Clock Multiplexer Register (RW)
 *
 * Reset value: 0x00022324
 *
 * The figure below represents the CCM Bus Clock Multiplexer Register (CBCMR). The CBCMR register
 * contains bits to control the multiplexers that generate the bus clocks. The table below provides
 * its field descriptions. Note: Any change on the above multiplexer will have to be done while the
 * module that its clock is affected is not functional and the respective clock is gated in LPCG. If
 * the change will be done during operation of the module, then it is not guaranteed that the
 * modules operation will not be harmed. The change for arm_axi_clk_sel should be done through sdma
 * so that ARM will not use this clock during the change and the clock will be gated in LPCG.
 */
typedef union _hw_ccm_cbcmr
{
    uint32_t value;
    struct _hw_ccm_cbcmr_bitfields
    {
        unsigned RESERVED0 : 12;            //!< [11:0] Reserved
        unsigned PERIPH_CLK2_SEL : 2;       //!< [13:12] Selector for peripheral clk2 clock multiplexer
        unsigned RESERVED1 : 4;             //!< [17:14] Reserved
        unsigned PRE_PERIPH_CLK_SEL : 2;    //!< [19:18] Selector for pre_periph clock multiplexer
        unsigned PERIPH2_CLK2_SEL : 1;      //!< [20] Selector for periph2_clk2 clock multiplexer
        unsigned PRE_PERIPH2_CLK_SEL : 2;   //!< [22:21] Selector for pre_periph2 clock multiplexer
        unsigned LCDIF1_PODF : 3;           //!< [25:23] Post-divider for lcdif1 clock.
        unsigned RESERVED2 : 3;             //!< [28:26] Reserved
        unsigned RESERVED3 : 3;             //!< [31:29] Reserved
    } bits;
} hw_ccm_cbcmr_t;

/*!
 * @brief HW_CCM_CSCMR1 - CCM Serial Clock Multiplexer Register 1 (RW)
 *
 * Reset value: 0x00f00000
 *
 * The figure below represents the CCM Serial Clock Multiplexer Register 1 (CSCMR1). The CSCMR1
 * register contains bits to control the multiplexers that generate the serial clocks. The table
 * below provides its field descriptions. Note: Any change on the above multiplexer will have to be
 * done while the module that its clock is affected is not functional and the clock is gated. If the
 * change will be done during operation of the module, then it is not guaranteed that the modules
 * operation will not be harmed.
 */
typedef union _hw_ccm_cscmr1
{
    uint32_t value;
    struct _hw_ccm_cscmr1_bitfields
    {
        unsigned PERCLK_PODF : 6;       //!< [5:0] Divider for perclk podf.
        unsigned PERCLK_CLK_SEL : 1;    //!< [6] Selector for the perclk clock multiplexor
        unsigned QSPI1_SEL : 3;         //!< [9:7] QSPI1 clock select
        unsigned SAI1_CLK_SEL : 2;      //!< [11:10] Selector for sai1 clock multiplexer
        unsigned SAI2_CLK_SEL : 2;      //!< [13:12] Selector for sai2 clock multiplexer
        unsigned SAI3_CLK_SEL : 2;      //!< [15:14] Selector for sai3 clock multiplexer
        unsigned USDHC1_CLK_SEL : 1;    //!< [16] Selector for usdhc1 clock multiplexer
        unsigned USDHC2_CLK_SEL : 1;    //!< [17] Selector for usdhc2 clock multiplexer
        unsigned BCH_CLK_SEL : 1;       //!< [18] Selector for bch clock multiplexer
        unsigned GPMI_CLK_SEL : 1;      //!< [19] Selector for gpmi clock multiplexer
        unsigned RESERVED0 : 3;         //!< [22:20] Reserved
        unsigned ACLK_EIM_SLOW_PODF : 3; //!< [25:23] Divider for aclk_eim_slow clock root.
        unsigned QSPI1_PODF : 3;        //!< [28:26] Selector for aclk root clock multiplexer
        unsigned ACLK_EIM_SLOW_SEL : 2; //!< [30:29] Selector for aclk_eim_slow root clock multiplexer
        unsigned RESERVED2 : 1;         //!< [31] Reserved.
    } bits;
} hw_ccm_cscmr1_t;

/*!
 * @brief HW_CCM_CSCMR2 - CCM Serial Clock Multiplexer Register 2 (RW)
 *
 * Reset value: 0x02b92f06
 *
 * The figure below represents the CCM Serial Clock Multiplexer Register 2 (CSCMR2). The CSCMR2
 * register contains bits to control the multiplexers that generate the serial clocks. The table
 * below provides its field descriptions. Note: Any change on the above multiplexer will have to be
 * done while the module that its clock is affected is not functional and the clock is gated. If the
 * change will be done during operation of the module, then it is not guaranteed that the modules
 * operation will not be harmed.
 */
typedef union _hw_ccm_cscmr2
{
    uint32_t value;
    struct _hw_ccm_cscmr2_bitfields
    {
        unsigned RESERVED0 :    2;      //!< [1:0] Reserved
        unsigned CAN_CLK_PODF : 6;      //!< [7:2] Divider for can clock podf.
        unsigned CAN_CLK_SEL : 2;       //!< [9:8] Selector for FlexCAN clock multiplexer
        unsigned LDB_DI0_IPU_DIV : 1;   //!< [10] Control for divider of ldb clock for IPU di0
        unsigned LDB_DI1_IPU_DIV : 1;   //!< [11] Control for divider of ldb clock for IPU di1
        unsigned RESERVED2 : 9;         //!< [20:12] Reserved
        unsigned VID_CLK_SEL : 3;       //!< [23:21] Selector for vid clock multiplexer
        unsigned VID_CLK_PRE_PODF : 2;  //!< [25:24] Post-divider for vid clock root
        unsigned VID_CLK_PODF : 3;      //!< [28:26] Post-divider for vid clock root
        unsigned RESERVED3 : 3;         //!< [31:29] Reserved
    } bits;
} hw_ccm_cscmr2_t;

/*!
 * @brief HW_CCM_CSCDR1 - CCM Serial Clock Divider Register 1 (RW)
 *
 * Reset value: 0x00490b00
 *
 * The figure below represents the CCM Serial Clock Divider Register 1 (CSCDR1). The CSCDR1 register
 * contains bits to control the clock generation sub module dividers. The table below provides its
 * field descriptions. Note: Any change on the above dividers will have to be done while the module
 * that its clock is affected is not functional and the affected clock is gated. If the change will
 * be done during operation of the module, then it is not guaranteed that the modules operation will
 * not be harmed.
 */
typedef union _hw_ccm_cscdr1
{
    uint32_t value;
    struct _hw_ccm_cscdr1_bitfields
    {
        unsigned UART_CLK_PODF : 6;     //!< [5:0] Divider for uart clock podf.
        unsigned UART_CLK_SEL:1;        //!< [6] Selector for the UART clock multiplexor.
        unsigned RESERVED0 : 4;         //!< [10:7] Reserved.
        unsigned USDHC1_PODF : 3;       //!< [13:11] Divider for usdhc1 clock podf.
        unsigned RESERVED1 : 2;         //!< [15:14] Reserved
        unsigned USDHC2_PODF : 3;       //!< [18:16] Divider for usdhc2 clock.
        unsigned BCH_PODF : 3;          //!< [21:19] Divider for bch clock podf.
        unsigned GPMI_PODF : 3;         //!< [24:22] Divider for gpmi clock pred.
        unsigned RESERVED2 : 3;         //!< [27:25] Reserved
        unsigned RESERVED3 : 4;         //!< [31:28] Reserved
    } bits;
} hw_ccm_cscdr1_t;

/*!
 * @brief HW_CCM_CS1CDR - CCM SSI1 Clock Divider Register (RW)
 *
 * Reset value: 0x0ec102c1
 *
 * The figure below represents the CCM SSI1, SSI3, ESAI Clock Divider Register (CS1CDR). The CS1CDR
 * register contains bits to control the ssi1 clock generation dividers. The table below provides
 * its field descriptions.
 */
typedef union _hw_ccm_cs1cdr
{
    uint32_t value;
    struct _hw_ccm_cs1cdr_bitfields
    {
        unsigned SAI1_CLK_PODF : 6; //!< [5:0] Divider for sai1 clock podf.
        unsigned SAI1_CLK_PRED : 3; //!< [8:6] Divider for sai1 clock pred.
        unsigned RESERVED0 : 7;     //!< [15:9] Reserved
        unsigned SAI3_CLK_PODF : 6; //!< [21:16] Divider for sai3 clock podf.
        unsigned SAI3_CLK_PRED : 3; //!< [24:22] Divider for sai3 clock pred.
        unsigned RESERVED1 : 7;     //!< [31:25] Reserved
    } bits;
} hw_ccm_cs1cdr_t;
/*!
 * @brief HW_CCM_CS2CDR - CCM SSI2 Clock Divider Register (RW)
 *
 * Reset value: 0x000736c1
 *
 * The figure below represents the CCM SSI2, LDB Clock Divider Register (CS2CDR). The CS2CDR
 * register contains bits to control the ssi2 clock generation dividers, and ldb serial clocks
 * select. The table below provides its field descriptions.
 */
typedef union _hw_ccm_cs2cdr
{
    uint32_t value;
    struct _hw_ccm_cs2cdr_bitfields
    {
        unsigned SAI2_CLK_PODF : 6;     //!< [5:0] Divider for sai2 clock podf.
        unsigned SAI2_CLK_PRED : 3;     //!< [8:6] Divider for sai2 clock pred.
        unsigned LDB_DI0_CLK_SEL : 3;   //!< [11:9] Selector for ldb_di1 clock multiplexer
        unsigned LDB_DI1_CLK_SEL : 3;   //!< [14:12] Selector for ldb_di1 clock multiplexer
        unsigned ENFC_CLK_SEL : 3;      //!< [17:15] Selector for enfc clock multiplexer
        unsigned ENFC_CLK_PRED : 3;     //!< [20:18] Divider for enfc clock pred divider.
        unsigned ENFC_CLK_PODF : 6;     //!< [26:21] Divider for enfc clock divider.
        unsigned RESERVED1 : 5;         //!< [31:27] Reserved
    } bits;
} hw_ccm_cs2cdr_t;

/*!
 * @brief HW_CCM_CDCDR - CCM D1 Clock Divider Register (RW)
 *
 * Reset value: 0x33f71f92
 *
 * The figure below represents the CCM DI Clock Divider Register (CDCDR). The table below provides
 * its field descriptions.
 */
typedef union _hw_ccm_cdcdr
{
    uint32_t value;
    struct _hw_ccm_cdcdr_bitfields
    {
        unsigned RESERVED0 : 20;        //!< [19:0] Reserved
        unsigned SPDIF0_CLK_SEL : 2;    //!< [21:20] Selector for spdif0 clock multiplexer
        unsigned SPDIF0_CLK_PODF : 3;   //!< [24:22] Divider for spdif0 clock podf.
        unsigned SPDIF0_CLK_PRED : 3;   //!< [27:25] Divider for spdif0 clock pred.
        unsigned RESERVED1 : 4;         //!< [31:28] Reserved
    } bits;
} hw_ccm_cdcdr_t;

/*!
 * @brief HW_CCM_CHSCCDR - CCM HSC Clock Divider Register (RW)
 *
 * Reset value: 0x0002a150
 *
 * The figure below represents the CCM IPU1 DI Clock Divider Register (CHSCCDR). The CHSCCDR
 * register contains bits to control the ipu di clock generation dividers. The table below provides
 * its field descriptions.
 */
typedef union _hw_ccm_chsccdr
{
    uint32_t value;
    struct _hw_ccm_chsccdr_bitfields
    {
        unsigned RESERVED0 : 9;         //!< [8:0] Reserved
        unsigned SIM_CLK_SEL : 3;       //!< [11:9] Selector for SIM root clock multiplexer
        unsigned SIM_PODF : 3;          //!< [14:12] Divider for SIM clock divider.
        unsigned SIM_PRE_CLK_SEL : 3;   //!< [17:15] Selector for SIM root clock pre-multiplexer
        unsigned RESERVED1 : 14;        //!< [31:18] Reserved
    } bits;
} hw_ccm_chsccdr_t;

/*!
 * @brief HW_CCM_CSCDR2 - CCM Serial Clock Divider Register 2 (RW)
 *
 * Reset value: 0x0002a150
 *
 * The figure below represents the CCM Serial Clock Divider Register 2(CSCDR2). The CSCDR2 register
 * contains bits to control the clock generation sub module dividers. The table below provides its
 * field descriptions.
 */
typedef union _hw_ccm_cscdr2
{
    uint32_t value;
    struct _hw_ccm_cscdr2_bitfields
    {
        unsigned RESERVED0 : 9;         //!< [8:0] Reserved
        unsigned LCDIF1_CLK_SEL : 3;    //!< [11:9] Selector for lcdif1 root clock multiplexer
        unsigned LCDIF1_PRED : 3;       //!< [14:12] Pre-divider for lcdif1 clock.
        unsigned ILCDIF1_PRE_CLK_SEL:3; //!< [17:15] Selector for lcdif1 root clock pre-multiplexer
        unsigned ECSPI_CLK_SEL : 1;     //!< [18] Selector for the ECSPI clock multiplexor
        unsigned ECSPI_CLK_PODF : 6;    //!< [24:19] Divider for ecspi clock podf.
        unsigned RESERVED1 : 7;         //!< [31:25] Reserved.
    } bits;
} hw_ccm_cscdr2_t;

/*!
 * @brief HW_CCM_CSCDR3 - CCM Serial Clock Divider Register 3 (RW)
 *
 * Reset value: 0x00014841
 *
 * The figure below represents the CCM Serial Clock Divider Register 3(CSCDR3). The CSCDR3 register
 * contains bits to control the clock generation sub module dividers. The table below provides its
 * field descriptions.
 */
typedef union _hw_ccm_cscdr3
{
    uint32_t value;
    struct _hw_ccm_cscdr3_bitfields
    {
        unsigned RESERVED0 : 9;         //!< [8:0] Reserved
        unsigned CSI_CLK_SEL : 2;       //!< [10:9] Selector for csi clock multiplexer
        unsigned CSI_PODF : 3;          //!< [13:11] Post divider for csi_core clock.
        unsigned RESERVED1 : 18;        //!< [31:14] Reserved
    } bits;
} hw_ccm_cscdr3_t;

/*!
 * @brief HW_CCM_CWDR - CCM Wakeup Detector Register (RW)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Wakeup Detector Register (CWDR). The CWDR register contains
 * reserved, read-only bits.The table below provides its field descriptions.
 */
typedef union _hw_ccm_cwdr
{
    uint32_t value;
    struct _hw_ccm_cwdr_bitfields
    {
        unsigned RESERVED0 : 32;    //!< [31:0] Reserved
    } bits;
} hw_ccm_cwdr_t;

/*!
 * @brief HW_CCM_CDHIPR - CCM Divider Handshake In-Process Register (RO)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Divider Handshake In-Process Register (CDHIPR). The CDHIPR
 * register contains read only bits that indicate that CCM is in the process of updating dividers or
 * muxes that might need handshake with modules.
 */
typedef union _hw_ccm_cdhipr
{
    uint32_t value;
    struct _hw_ccm_cdhipr_bitfields
    {
        unsigned AXI_PODF_BUSY : 1;         //!< [0] Busy indicator for axi_podf.
        unsigned AHB_PODF_BUSY : 1;         //!< [1] Busy indicator for ahb_podf.
        unsigned MMDC_PODF_BUSY : 1;        //!< [2] Busy indicator for mmdc_axi_podf..
        unsigned PERIPH2_CLK_SEL_BUSY : 1;  //!< [3] Busy indicator for periph2_clk_sel mux control.
        unsigned MMDC_CH0_PODF_BUSY : 1;    //!< [4] Busy indicator for mmdc_ch0_axi_podf.
        unsigned PERIPH_CLK_SEL_BUSY : 1;   //!< [5] Busy indicator for periph_clk_sel mux control.
        unsigned RESERVED0 : 10;            //!< [15:6] Reserved
        unsigned ARM_PODF_BUSY : 1;         //!< [16] Busy indicator for arm_podf.
        unsigned RESERVED1 : 15;            //!< [31:17] Reserved
    } bits;
} hw_ccm_cdhipr_t;

/*!
 * @brief HW_CCM_CTOR - CCM Testing Observability Register (RW)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Testing Observability Register (CTOR). CCM includes three
 * muxes to mux between different critical signals for testing observability. The output of the
 * three muxes is generated on the three output signals obs_output_0, obs_output_1 and obs_output_2.
 * Those three output signals can be generated on the IC pads by configuring the IOMUXC. The CTOR
 * register contains bits to control the data generated for observability on the three output
 * signals above. The table below provides its field descriptions.
 */
typedef union _hw_ccm_ctor
{
    uint32_t value;
    struct _hw_ccm_ctor_bitfields
    {
        unsigned OBS_SPARE_OUTPUT_2_SEL : 4; //!< [3:0] Selection of the signal to be generated on obs_output_2 (output of CCM) for observability on the pads.
        unsigned OBS_SPARE_OUTPUT_1_SEL : 4; //!< [7:4] Selection of the signal to be generated on obs_output_1 (output of CCM) for observability on the pads.
        unsigned OBS_SPARE_OUTPUT_0_SEL : 5; //!< [12:8] Selection of the signal to be generated on obs_output_0 (output of CCM) for observability on the pads.
        unsigned OBS_EN : 1; //!< [13] observability enable bit.
        unsigned RESERVED0 : 18; //!< [31:14] Reserved
    } bits;
} hw_ccm_ctor_t;

/*!
 * @brief HW_CCM_CLPCR - CCM Low Power Control Register (RW)
 *
 * Reset value: 0x00000079
 *
 * The figure below represents the CCM Low Power Control Register (CLPCR). The CLPCR register
 * contains bits to control the low power modes operation.The table below provides its field
 * descriptions.
 */
typedef union _hw_ccm_clpcr
{
    uint32_t value;
    struct _hw_ccm_clpcr_bitfields
    {
        unsigned LPM : 2;               //!< [1:0] Setting the low power mode that system will enter on next assertion of dsm_request signal.
        unsigned RESERVED0 : 3;         //!< [4:2] Reserved
        unsigned ARM_CLK_DIS_ON_LPM : 1;//!< [5] Define if ARM clocks (arm_clk, soc_mxclk, soc_pclk, soc_dbg_pclk, vl_wrck) will be disabled on wait mode.
        unsigned SBYOS : 1;             //!< [6] Standby clock oscillator bit.
        unsigned DIS_REF_OSC : 1;       //!< [7] dis_ref_osc - in run mode, software can manually control closing of external reference oscillator clock, i.e.
        unsigned VSTBY : 1;             //!< [8] Voltage standby request bit.
        unsigned STBY_COUNT : 2;        //!< [10:9] Standby counter definition.
        unsigned COSC_PWRDOWN : 1;      //!< [11] In run mode, software can manually control powering down of on chip oscillator, i.e.
        unsigned RESERVED1 : 9;         //!< [20:12] Reserved
        unsigned BYPASS_MMDC__LPM_HS:1; //!< [21] Bypass handshake with mmdc on next entrance to low power mode (wait or stop mode).
        unsigned MASK_CORE0_WFI : 1;    //!< [22] Mask WFI of core0 for entering low power mode
        unsigned MASK_CORE1_WFI : 1;    //!< [23] Mask WFI of core1 for entering low power mode
        unsigned MASK_CORE2_WFI : 1;    //!< [24] Mask WFI of core2 for entering low power mode
        unsigned MASK_CORE3_WFI : 1;    //!< [25] Mask WFI of core3 for entering low power mode
        unsigned MASK_SCU_IDLE : 1;     //!< [26] Mask SCU IDLE for entering low power mode
        unsigned MASK_L2CC_IDLE : 1;    //!< [27] Mask L2CC IDLE for entering low power mode.
        unsigned RESERVED4 : 4;         //!< [31:28] Reserved
    } bits;
} hw_ccm_clpcr_t;

/*!
 * @brief HW_CCM_CISR - CCM Interrupt Status Register (W1C)
 *
 * Reset value: 0x00000000
 *
 * The figure below represents the CCM Interrupt Status Register (CISR). This is a write one to
 * clear register. Once a interrupt is generated, software should write one to clear it. The table
 * below provides its field descriptions. * Note: ipi_int_1 can be masked by ipi_int_1_mask bit.
 * ipi_int_2 can be masked by ipi_int_2_mask bit.
 */
typedef union _hw_ccm_cisr
{
    uint32_t value;
    struct _hw_ccm_cisr_bitfields
    {
        unsigned LRF_PLL : 1;               //!< [0] Interrupt ipi_int_2 generated due to lock of all enabled and not bypaseed pll's
        unsigned RESERVED0 : 5;             //!< [5:1] Reserved
        unsigned COSC_READY : 1;            //!< [6] Interrupt ipi_int_2 generated due to on board oscillator ready, i.e.
        unsigned RESERVED1 : 10;            //!< [16:7] Reserved
        unsigned AXI_PODF_LOADED : 1;       //!< [17] Interrupt ipi_int_1 generated due to frequency change of axi_a_podf
        unsigned RESERVED2 : 1;             //!< [18] Reserved
        unsigned PERIPH2_CLK_SEL_LOADED : 1;//!< [19] Interrupt ipi_int_1 generated due to frequency change of periph2_clk_sel
        unsigned AHB_PODF_LOADED : 1;       //!< [20] Interrupt ipi_int_1 generated due to frequency change of ahb_podf
        unsigned MMDC_PODF_LOADED : 1;      //!< [21] Interrupt ipi_int_1 generated due to frequency change of mmdc_ch0_podf_ loaded
        unsigned PERIPH_CLK_SEL_LOADED : 1; //!< [22] Interrupt ipi_int_1 generated due to update of periph_clk_sel.
        unsigned RESERVED3 : 3;             //!< [25:23] Reserved
        unsigned ARM_PODF_LOADED : 1;       //!< [26] Interrupt ipi_int_1 generated due to frequency change of arm_podf.
        unsigned RESERVED4 : 5;             //!< [31:27] Reserved
    } bits;
} hw_ccm_cisr_t;

/*!
 * @brief HW_CCM_CIMR - CCM Interrupt Mask Register (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Interrupt Mask Register (CIMR). The table below provides its
 * field descriptions.
 */
typedef union _hw_ccm_cimr
{
    uint32_t value;
    struct _hw_ccm_cimr_bitfields
    {
        unsigned MASK_LRF_PLL : 1;          //!< [0] mask interrupt generation due to lrf of pll's
        unsigned RESERVED0 : 5;             //!< [5:1] Reserved
        unsigned MASK_COSC_READY : 1;       //!< [6] mask interrupt generation due to on board oscillator ready
        unsigned RESERVED1 : 10;            //!< [16:7] Reserved
        unsigned MASK_AXI_PODF_LOADED:1;    //!< [17] mask interrupt generation due to frequency change of axi_podf
        unsigned RESERVED2 : 1;             //!< [18] Reserved
        unsigned MASK_PERIPH2_CLK_SEL_LOADED:1;//!< [19] mask interrupt generation due to update of periph2_clk_sel.
        unsigned MASK_AHB_PODF_LOADED : 1; //!< [20] mask interrupt generation due to frequency change of ahb_podf
        unsigned MASK_MMDC_PODF_LOADED : 1;//!< [21] mask interrupt generation due to update of mask_mmdc_ch1_podf
        unsigned MASK_PERIPH_CLK_SEL_LOADED:1;//!< [22] mask interrupt generation due to update of periph_clk_sel.
        unsigned MASK_MMDC_CH0_PODF_LOADED : 1; //!< [23] mask interrupt generation due to update of mask_mmdc_ch0_podf
        unsigned RESERVED3 : 2;             //!< [25:24] Reserved
        unsigned ARM_PODF_LOADED : 1;       //!< [26] mask interrupt generation due to frequency change of arm_podf
        unsigned RESERVED4 : 5;             //!< [31:27] Reserved
    } bits;
} hw_ccm_cimr_t;

/*!
 * @brief HW_CCM_CCOSR - CCM Clock Output Source Register (RW)
 *
 * Reset value: 0x000a0001
 *
 * The figure below represents the CCM Clock Output Source Register (CCOSR). The CCOSR register
 * contains bits to control the clock s that will be generated on the output ipp_do_clko1
 * (CCM_CLKO1) and ipp_do_clko2 (CCM_CLKO2) .The table below provides its field descriptions.
 */
typedef union _hw_ccm_ccosr
{
    uint32_t value;
    struct _hw_ccm_ccosr_bitfields
    {
        unsigned CLKO1_SEL : 4;     //!< [3:0] Selection of the clock to be generated on CCM_CLKO1
        unsigned CLKO1_DIV : 3;     //!< [6:4] Setting the divider of CCM_CLKO1
        unsigned CLKO1_EN : 1;      //!< [7] Enable of CCM_CLKO1 clock
        unsigned CLKO_SEL : 1;      //!< [8] CCM_CLKO1 output to reflect CCM_CLKO1 or CCM_CLKO2 clocks
        unsigned RESERVED0 : 7;     //!< [15:9] Reserved
        unsigned CLKO2_SEL : 5;     //!< [20:16] Selection of the clock to be generated on CCM_CLKO2
        unsigned CLKO2_DIV : 3;     //!< [23:21] Setting the divider of CCM_CLKO2
        unsigned CLKO2_EN : 1;      //!< [24] Enable of CCM_CLKO2 clock
        unsigned RESERVED1 : 7;     //!< [31:25] Reserved
    } bits;
} hw_ccm_ccosr_t;

/*!
 * @brief HW_CCM_CGPR - CCM General Purpose Register (RW)
 *
 * Reset value: 0x0000fe62
 *
 * Fast PLL enable. Can be used to engage PLL faster after STOP mode, if 24MHz OSC was active
 */
typedef union _hw_ccm_cgpr
{
    uint32_t value;
    struct _hw_ccm_cgpr_bitfields
    {
        unsigned PMIC_DELAY_SCALER : 1;     //!< [0] Defines clock dividion of clock for stby_count (pmic delay counter)
        unsigned RESERVED0 : 1;             //!< [1] Reserved
        unsigned MMDC_EXT_CLK_DIS : 1;      //!< [2] Disable external clock driver of MMDC during STOP mode
        unsigned RESERVED1 : 1;             //!< [3] Reserved
        unsigned EFUSE_PROG_SUPPLY_GATE : 1;//!< [4] Defines the value of the output signal cgpr_dout[4].
        unsigned RESERVED3 : 9;             //!< [13:5] Reserved.
        unsigned SYS_MEM_DS_CTRL:2;         //!< [15:14] System memory DS control
        unsigned FPL : 1;                   //!< [16] Fast PLL enable.
        unsigned INT_MEM_CLK_LPM : 1;       //!< [17] Control for the Deep Sleep signal to the ARM Platform memories.
        unsigned RESERVED4 : 14;            //!< [31:18] Reserved
    } bits;
} hw_ccm_cgpr_t;

typedef enum
{
    CGR_CLK_OFF_RUN = 0,
    CGR_CLK_OFF_WAIT = 1,
    CGR_CLK_OFF_STOP = 3,

}CCM_CGR_VALUE;
/*!
 * @brief HW_CCM_CCGR0 - CCM Clock Gating Register 0 (RW)
 *
 * Reset value: 0xffffffff
 *
 * CG(i) bits CCGR 0- 6 These bits are used to turn on/off the clock to each module
 * independently.The following table details the possible clock activity conditions for each module
 * CGR value Clock Activity Description 00 clock is off during all modes. stop enter hardware
 * handshake is disabled. 01 clock is on in run mode, but off in wait and stop modes 10 Not
 * applicable (Reserved). 11 clock is on during all modes, except stop mode. Module should be
 * stopped, before set it's bits to "0", since clocks to the module will be stopped immediately. The
 * tables above show the register mapings for the different CGR's. The clock connectivity table
 * should be used to match the "CCM output affected" to the actual clocks going into the modules.
 * The figure below represents the CCM Clock Gating Register 0 (CCM_CCGR0). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 7 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr0
{
    uint32_t value;
    struct _hw_ccm_ccgr0_bitfields
    {
        unsigned CG0 : 2;           //!< [1:0] aips_tz1 clocks (aips_tz1_clk_enable)
        unsigned CG1 : 2;           //!< [3:2] aips_tz2 clocks (aips_tz2_clk_enable)
        unsigned CG2 : 2;           //!< [5:4] apbhdma hclk clock (apbhdma_hclk_enable)
        unsigned CG3 : 2;           //!< [7:6] asrc clock (asrc_clk_enable)
        unsigned CG4 : 2;           //!< [9:8] caam_secure_mem clock (caam_secure_mem_clk_enable)
        unsigned CG5 : 2;           //!< [11:10] caam_wrapper_aclk clock (caam_wrapper_aclk_enable)
        unsigned CG6 : 2;           //!< [13:12] caam_wrapper_ipg clock (caam_wrapper_ipg_enable)
        unsigned CG7 : 2;           //!< [15:14] can1 clock (can1_clk_enable)
        unsigned CG8 : 2;           //!< [17:16] can1_serial clock (can1_serial_clk_enable)
        unsigned CG9 : 2;           //!< [19:18] can2 clock (can2_clk_enable)
        unsigned CG10 : 2;          //!< [21:20] can2_serial clock (can2_serial_clk_enable)
        unsigned CG11 : 2;          //!< [23:22] CPU debug clocks (arm_dbg_clk_enable)
        unsigned CG12 : 2;          //!< [25:24] dcic 1 clocks (dcic1_clk_enable)
        unsigned CG13 : 2;          //!< [27:26] dcic2 clocks (dcic2_clk_enable)
        unsigned CG14 : 2;          //!< [29:28] dtcp clocks (dtcp_dtcp_clk_enable)
        unsigned CG15 : 2;          //!< [31:30] Reserved
    } bits;
} hw_ccm_ccgr0_t;

/*!
 * @brief HW_CCM_CCGR1 - CCM Clock Gating Register 1 (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Clock Gating Register 1(CCM_CCGR1). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr1
{
    uint32_t value;
    struct _hw_ccm_ccgr1_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] ecspi1 clocks (ecspi1_clk_enable)
        unsigned CG1 : 2; //!< [3:2] ecspi2 clocks (ecspi2_clk_enable)
        unsigned CG2 : 2; //!< [5:4] ecspi3 clocks (ecspi3_clk_enable)
        unsigned CG3 : 2; //!< [7:6] ecspi4 clocks (ecspi4_clk_enable)
        unsigned CG4 : 2; //!< [9:8] ecspi5 clocks (ecspi5_clk_enable)
        unsigned CG5 : 2; //!< [11:10] clock (enet_clk_enable)
        unsigned CG6 : 2; //!< [13:12] epit1 clocks (epit1_clk_enable)
        unsigned CG7 : 2; //!< [15:14] epit2 clocks (epit2_clk_enable)
        unsigned CG8 : 2; //!< [17:16] esai clocks (esai_clk_enable)
        unsigned CG9 : 2; //!< [19:18] Reserved
        unsigned CG10 : 2; //!< [21:20] gpt bus clock (gpt_clk_enable)
        unsigned CG11 : 2; //!< [23:22] gpt serial clock (gpt_serial_clk_enable)
        unsigned CG12 : 2; //!< [25:24] gpu2d clock (gpu2d_clk_enable)
        unsigned CG13 : 2; //!< [27:26] gpu3d clock (gpu3d_clk_enable)
        unsigned CG14 : 2; //!< [29:28] Reserved
        unsigned CG15 : 2; //!< [31:30] Reserved
    } bits;
} hw_ccm_ccgr1_t;

/*!
 * @brief HW_CCM_CCGR2 - CCM Clock Gating Register 2 (RW)
 *
 * Reset value: 0xfc3fffff
 *
 * The figure below represents the CCM Clock Gating Register 2 (CCM_CCGR2). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr2
{
    uint32_t value;
    struct _hw_ccm_ccgr2_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] hdmi_tx_iahbclk, hdmi_tx_ihclk clock (hdmi_tx_iahbclk_enable)
        unsigned CG1 : 2; //!< [3:2] Reserved
        unsigned CG2 : 2; //!< [5:4] hdmi_tx_isfrclk clock (hdmi_tx_isfrclk_enable)
        unsigned CG3 : 2; //!< [7:6] i2c1_serial clock (i2c1_serial_clk_enable)
        unsigned CG4 : 2; //!< [9:8] i2c2_serial clock (i2c2_serial_clk_enable)
        unsigned CG5 : 2; //!< [11:10] i2c3_serial clock (i2c3_serial_clk_enable)
        unsigned CG6 : 2; //!< [13:12] OCOTP_CTRL clock (iim_clk_enable)
        unsigned CG7 : 2; //!< [15:14] iomux_ipt_clk_io clock (iomux_ipt_clk_io_enable)
        unsigned CG8 : 2; //!< [17:16] ipmux1 clock (ipmux1_clk_enable)
        unsigned CG9 : 2; //!< [19:18] ipmux2 clock (ipmux2_clk_enable)
        unsigned CG10 : 2; //!< [21:20] ipmux3 clock (ipmux3_clk_enable)
        unsigned CG11 : 2; //!< [23:22] ipsync_ip2apb_tzasc1_ipg clocks (ipsync_ip2apb_tzasc1_ipg_master_clk_enable)
        unsigned CG12 : 2; //!< [25:24] ipsync_vdoa_ipg clocks (ipsync_ip2apb_tzasc2_ipg clocks)
        unsigned CG13 : 2; //!< [27:26] ipsync_vdoa_ipg clocks (ipsync_vdoa_ipg_master_clk_enable)
        unsigned CG14 : 2; //!< [29:28] Reserved
        unsigned CG15 : 2; //!< [31:30] Reserved
    } bits;
} hw_ccm_ccgr2_t;

/*!
 * @brief HW_CCM_CCGR3 - CCM Clock Gating Register 3 (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Clock Gating Register 3 (CCM_CCGR3). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr3
{
    uint32_t value;
    struct _hw_ccm_ccgr3_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] ipu1_ipu clock (ipu1_ipu_clk_enable)
        unsigned CG1 : 2; //!< [3:2] ipu1_ipu_di0 clock (ipu1_ipu_di0_clk_enable)
        unsigned CG2 : 2; //!< [5:4] ipu1_ipu_di1 clock (ipu1_ipu_di1_clk_enable)
        unsigned CG3 : 2; //!< [7:6] ipu2_ipu clock (ipu2_ipu_clk_enable)
        unsigned CG4 : 2; //!< [9:8] ipu2_ipu_di0 clock (ipu2_ipu_di0_clk_enable)
        unsigned CG5 : 2; //!< [11:10] ipu2_ipu_di1 clock (ipu2_ipu_di1_clk_enable)
        unsigned CG6 : 2; //!< [13:12] ldb_di0 clock (ldb_di0_clk_enable)
        unsigned CG7 : 2; //!< [15:14] ldb_di1 clock (ldb_di1_clk_enable)
        unsigned CG8 : 2; //!< [17:16] mipi_core_cfg clock (mipi_core_cfg_clk_enable)
        unsigned CG9 : 2; //!< [19:18] mlb clock (mlb_clk_enable)
        unsigned CG10 : 2; //!< [21:20] mmdc_core_aclk_fast_core_p0 clock (mmdc_core_aclk_fast_core_p0_enable)
        unsigned CG11 : 2; //!< [23:22] mmdc_core_aclk_fast_core_p1 clock (mmdc_core_aclk_fast_core_p1_enable)
        unsigned CG12 : 2; //!< [25:24] mmdc_core_ipg_clk_p0 clock (mmdc_core_ipg_clk_p0_enable)
        unsigned CG13 : 2; //!< [27:26] mmdc_core_ipg_clk_p1 clock (mmdc_core_ipg_clk_p1_enable)
        unsigned CG14 : 2; //!< [29:28] ocram clock (ocram_clk_enable)
        unsigned CG15 : 2; //!< [31:30] openvgaxiclk clock (openvgaxiclk_clk_root_enable)
    } bits;
} hw_ccm_ccgr3_t;

/*!
 * @brief HW_CCM_CCGR4 - CCM Clock Gating Register 4 (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Clock Gating Register 4 (CCM_CCGR4). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr4
{
    uint32_t value;
    struct _hw_ccm_ccgr4_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] 125M clocks (125M_root_enable)
        unsigned CG1 : 2; //!< [3:2] Reserved.
        unsigned CG2 : 2; //!< [5:4] Reserved.
        unsigned CG3 : 2; //!< [7:6] Reserved.
        unsigned CG4 : 2; //!< [9:8] pl301_mx6qfast1_s133 clock (pl301_mx6qfast1_s133clk_enable)
        unsigned CG5 : 2; //!< [11:10] Reserved
        unsigned CG6 : 2; //!< [13:12] pl301_mx6qper1_bch clocks (pl301_mx6qper1_bchclk_enable)
        unsigned CG7 : 2; //!< [15:14] pl301_mx6qper2_mainclk_enable (pl301_mx6qper2_mainclk_enable)
        unsigned CG8 : 2; //!< [17:16] pwm1 clocks (pwm1_clk_enable)
        unsigned CG9 : 2; //!< [19:18] pwm2 clocks (pwm2_clk_enable)
        unsigned CG10 : 2; //!< [21:20] pwm3 clocks (pwm3_clk_enable)
        unsigned CG11 : 2; //!< [23:22] pwm4 clocks (pwm4_clk_enable)
        unsigned CG12 : 2; //!< [25:24] rawnand_u_bch_input_apb clock (rawnand_u_bch_input_apb_clk_enable)
        unsigned CG13 : 2; //!< [27:26] rawnand_u_gpmi_bch_input_bch clock (rawnand_u_gpmi_bch_input_bch_clk_enable)
        unsigned CG14 : 2; //!< [29:28] rawnand_u_gpmi_bch_input_gpmi_io clock (rawnand_u_gpmi_bch_input_gpmi_io_clk_enable)
        unsigned CG15 : 2; //!< [31:30] rawnand_u_gpmi_input_apb clock rawnand_u_gpmi_input_apb_clk_enable)
    } bits;
} hw_ccm_ccgr4_t;

/*!
 * @brief HW_CCM_CCGR5 - CCM Clock Gating Register 5 (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Clock Gating Register 5 (CCM_CCGR5). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr5
{
    uint32_t value;
    struct _hw_ccm_ccgr5_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] rom clock (rom_clk_enable)
        unsigned CG1 : 2; //!< [3:2] Reserved
        unsigned CG2 : 2; //!< [5:4] 100M clock (100M_clk_enable)
        unsigned CG3 : 2; //!< [7:6] sdma clock (sdma_clk_enable)
        unsigned CG4 : 2; //!< [9:8] Reserved
        unsigned CG5 : 2; //!< [11:10] Reserved
        unsigned CG6 : 2; //!< [13:12] spba clock (spba_clk_enable)
        unsigned CG7 : 2; //!< [15:14] spdif clock (spdif_clk_enable)
        unsigned CG8 : 2; //!< [17:16] Reserved
        unsigned CG9 : 2; //!< [19:18] ssi1 clocks (ssi1_clk_enable)
        unsigned CG10 : 2; //!< [21:20] ssi2 clocks (ssi2_clk_enable)
        unsigned CG11 : 2; //!< [23:22] ssi3 clocks (ssi3_clk_enable)
        unsigned CG12 : 2; //!< [25:24] uart clock (uart_clk_enable)
        unsigned CG13 : 2; //!< [27:26] uart_serial clock (uart_serial_clk_enable)
        unsigned CG14 : 2; //!< [29:28] Reserved
        unsigned CG15 : 2; //!< [31:30] Reserved
    } bits;
} hw_ccm_ccgr5_t;

/*!
 * @brief HW_CCM_CCGR6 - CCM Clock Gating Register 6 (RW)
 *
 * Reset value: 0xffffffff
 *
 * The figure below represents the CCM Clock Gating Register 6 (CCM_CCGR6). The clock gating
 * Registers define the clock gating for power reduction of each clock (CG(i) bits). There are 8 CGR
 * registers. The number of registers required is according to the number of peripherals in the
 * system.
 */
typedef union _hw_ccm_ccgr6
{
    uint32_t value;
    struct _hw_ccm_ccgr6_bitfields
    {
        unsigned CG0 : 2; //!< [1:0] usboh3 clock (usboh3_clk_enable)
        unsigned CG1 : 2; //!< [3:2] usdhc1 clocks (usdhc1_clk_enable)
        unsigned CG2 : 2; //!< [5:4] usdhc2 clocks (usdhc2_clk_enable)
        unsigned CG3 : 2; //!< [7:6] usdhc3 clocks (usdhc3_clk_enable)
        unsigned CG4 : 2; //!< [9:8] usdhc4 clocks (usdhc4_clk_enable)
        unsigned CG5 : 2; //!< [11:10] eim_slow clocks (eim_slow_clk_enable)
        unsigned CG6 : 2; //!< [13:12] vdoaxiclk root clock (vdoaxiclk_clk_enable)
        unsigned CG7 : 2; //!< [15:14] vpu clocks (vpu_clk_enable)
        unsigned CG8 : 2; //!< [17:16] Reserved
        unsigned CG9 : 2; //!< [19:18] Reserved
        unsigned CG10 : 2; //!< [21:20] Reserved
        unsigned CG11 : 2; //!< [23:22] Reserved
        unsigned CG12 : 2; //!< [25:24] Reserved
        unsigned CG13 : 2; //!< [27:26] Reserved
        unsigned CG14 : 2; //!< [29:28] Reserved
        unsigned CG15 : 2; //!< [31:30] Reserved
    } bits;
} hw_ccm_ccgr6_t;

/*!
 * @brief HW_CCM_CMEOR - CCM Module Enable Overide Register (RW)
 *
 * Reset value: 0xffffffff
 *
 * The follow figure represents the CCM Module Enable Override Register (CMEOR). The CMEOR register
 * contains bits to override the clock enable signal from the module. This should be used in case
 * that it is decided to bypass the clock enable signals from the modules. This bit will be
 * applicable only for module that their clock enable signal is used. The following table provides
 * its field descriptions.
 */
typedef union _hw_ccm_cmeor
{
    uint32_t value;
    struct _hw_ccm_cmeor_bitfields
    {
        unsigned RESERVED0 : 5;         //!< [4:0] Reserved
        unsigned MOD_EN_OV_GPT : 1;     //!< [5] overide clock enable signal from gpt - clock will not be gated based on gpt's signal 'ipg_enable_clk' .
        unsigned MOD_EN_OV_EPIT : 1;    //!< [6] overide clock enable signal from epit - clock will not be gated based on epit's signal 'ipg_enable_clk' .
        unsigned MOD_EN_USDHC : 1;      //!< [7] overide clock enable signal from usdhc.
        unsigned MOD_EN_OV_DAP : 1;     //!< [8] overide clock enable signal from dap- clock will not be gated based on dap's signal 'dap_dbgen' .
        unsigned MOD_EN_OV_VPU : 1;     //!< [9] overide clock enable signal from vpu- clock will not be gated based on vpu's signal 'vpu_idle' .
        unsigned MOD_EN_OV_GPU2D : 1;   //!< [10] overide clock enable signal from gpu2d - clock will not be gated based on gpu2d's signal 'gpu2d_busy' .
        unsigned MOD_EN_OV_GPU3D : 1;   //!< [11] overide clock enable signal from gpu3d - clock will not be gated based on gpu3d's signal.
        unsigned RESERVED1 : 16;        //!< [27:12] Reserved
        unsigned MOD_EN_OV_CAN2_CPI : 1; //!< [28] overide clock enable signal from can2 - clock will not be gated based on can's signal 'enable_clk_cpi'.
        unsigned RESERVED2 : 1;         //!< [29] Reserved
        unsigned MOD_EN_OV_CAN1_CPI : 1;//!< [30] overide clock enable signal from can1 - clock will not be gated based on can's signal 'enable_clk_cpi'.
        unsigned RESERVED3 : 1;         //!< [31] Reserved
    } bits;
} hw_ccm_cmeor_t;

typedef struct HW_CCM
{
    volatile hw_ccm_ccr_t   CCR; //!< CCM Control Register
    volatile hw_ccm_ccdr_t  CCDR; //!< CCM Control Divider Register
    volatile hw_ccm_csr_t   CSR; //!< CCM Status Register
    volatile hw_ccm_ccsr_t  CCSR; //!< CCM Clock Swither Register
    volatile hw_ccm_cacrr_t CACRR; //!< CCM Arm Clock Root Register
    volatile hw_ccm_cbcdr_t CBCDR; //!< CCM Bus Clock Divider Register
    volatile hw_ccm_cbcmr_t CBCMR; //!< CCM Bus Clock Multiplexer Register
    volatile hw_ccm_cscmr1_t CSCMR1; //!< CCM Serial Clock Multiplexer Register 1
    volatile hw_ccm_cscmr2_t CSCMR2; //!< CCM Serial Clock Multiplexer Register 2
    volatile hw_ccm_cscdr1_t CSCDR1; //!< CCM Serial Clock Divider Register 1
    volatile hw_ccm_cs1cdr_t CS1CDR; //!< CCM SSI1 Clock Divider Register
    volatile hw_ccm_cs2cdr_t CS2CDR; //!< CCM SSI2 Clock Divider Register
    volatile hw_ccm_cdcdr_t CDCDR; //!< CCM D1 Clock Divider Register
    volatile hw_ccm_chsccdr_t CHSCCDR; //!< CCM HSC Clock Divider Register
    volatile hw_ccm_cscdr2_t CSCDR2; //!< CCM Serial Clock Divider Register 2
    volatile hw_ccm_cscdr3_t CSCDR3; //!< CCM Serial Clock Divider Register 3
    uint32_t _reserved0;
    volatile hw_ccm_cwdr_t CWDR; //!< CCM Wakeup Detector Register
    volatile hw_ccm_cdhipr_t CDHIPR; //!< CCM Divider Handshake In-Process Register
    uint32_t _reserved1;
    volatile hw_ccm_ctor_t CTOR; //!< CCM Testing Observability Register
    volatile hw_ccm_clpcr_t CLPCR; //!< CCM Low Power Control Register
    volatile hw_ccm_cisr_t CISR; //!< CCM Interrupt Status Register
    volatile hw_ccm_cimr_t CIMR; //!< CCM Interrupt Mask Register
    volatile hw_ccm_ccosr_t CCOSR; //!< CCM Clock Output Source Register
    volatile hw_ccm_cgpr_t CGPR; //!< CCM General Purpose Register
    volatile hw_ccm_ccgr0_t CCGR0; //!< CCM Clock Gating Register 0
    volatile hw_ccm_ccgr1_t CCGR1; //!< CCM Clock Gating Register 1
    volatile hw_ccm_ccgr2_t CCGR2; //!< CCM Clock Gating Register 2
    volatile hw_ccm_ccgr3_t CCGR3; //!< CCM Clock Gating Register 3
    volatile hw_ccm_ccgr4_t CCGR4; //!< CCM Clock Gating Register 4
    volatile hw_ccm_ccgr5_t CCGR5; //!< CCM Clock Gating Register 5
    volatile hw_ccm_ccgr6_t CCGR6; //!< CCM Clock Gating Register 6
    uint32_t _reserved2;
    volatile hw_ccm_cmeor_t CMEOR; //!< CCM Module Enable Overide Register
}hw_ccm_t;

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_CCM_REGS_H */

/* end of file */
