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

#ifndef __IMX6UL_CCM_ANALOG_REGS_H
#define __IMX6UL_CCM_ANALOG_REGS_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*!
 * @brief HW_CCM_ANALOG_PLL_ARM - Analog ARM PLL control Register (RW)
 *
 * Reset value: 0x00013042
 *
 * The control register provides control for the system PLL.
 */
typedef union _hw_ccm_analog_pll_arm
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_arm_bitfields
    {
        unsigned DIV_SELECT : 7;    //!< [6:0] This field controls the pll loop divider.
        unsigned RESERVED0 : 5;     //!< [11:7] Reserved.
        unsigned POWERDOWN : 1;     //!< [12] Powers down the PLL.
        unsigned ENABLE : 1;        //!< [13] Enable the clock output.
        unsigned BYPASS_CLK_SRC : 2;//!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;        //!< [16] Bypass the pll.
        unsigned LVDS_SEL : 1;      //!< [17] Analog Debug Bit
        unsigned LVDS_24MHZ_SEL : 1;//!< [18] Analog Debug Bit
        unsigned PLL_SEL : 1;       //!< [19] Reserved
        unsigned RESERVED1 : 11;    //!< [30:20] Always set to zero (0).
        unsigned LOCK : 1;          //!< [31] 1 - PLL is currently locked.
    } bits;
} hw_ccm_analog_pll_arm_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_USB1 - Analog USB1 480MHz PLL Control Register (RW)
 *
 * Reset value: 0x00012000
 *
 * The control register provides control for USBPHY0 480MHz PLL.
 */
typedef union _hw_ccm_analog_pll_usb1
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_usb1_bitfields
    {
        unsigned DIV_SELECT : 2;    //!< [1:0] This field controls the pll loop divider.
        unsigned RESERVED0 : 4;     //!< [5:2] Always set to zero (0).
        unsigned EN_USB_CLKS : 1;   //!< [6] Powers the 9-phase PLL outputs for USBPHYn.
        unsigned RESERVED1 : 5;     //!< [11:7] Always set to zero (0).
        unsigned POWER : 1;         //!< [12] Powers up the PLL.
        unsigned ENABLE : 1;        //!< [13] Enable the PLL clock output.
        unsigned BYPASS_CLK_SRC : 2;//!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;        //!< [16] Bypass the pll.
        unsigned RESERVED2 : 14;    //!< [30:17] Always set to zero (0).
        unsigned LOCK : 1;          //!< [31] 1 - PLL is currently locked.
    } bits;
} hw_ccm_analog_pll_usb1_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_USB2 - Analog USB2 480MHz PLL Control Register (RW)
 *
 * Reset value: 0x00012000
 *
 * The control register provides control for USBPHY1 480MHz PLL.
 */
typedef union _hw_ccm_analog_pll_usb2
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_usb2_bitfields
    {
        unsigned DIV_SELECT : 2;    //!< [1:0] This field controls the pll loop divider.
        unsigned RESERVED0 : 4;     //!< [5:2] Always set to zero (0).
        unsigned EN_USB_CLKS : 1;   //!< [6] 0: 8-phase PLL outputs for USBPHY1 are powered down.
        unsigned RESERVED1 : 5;     //!< [11:7] Always set to zero (0).
        unsigned POWER : 1;         //!< [12] Powers up the PLL.
        unsigned ENABLE : 1;        //!< [13] Enable the PLL clock output.
        unsigned BYPASS_CLK_SRC : 2;//!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;        //!< [16] Bypass the pll.
        unsigned RESERVED2 : 14;    //!< [30:17] Always set to zero (0).
        unsigned LOCK : 1;          //!< [31] 1 - PLL is currently locked.
    } bits;
} hw_ccm_analog_pll_usb2_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_SYS - Analog System PLL Control Register (RW)
 *
 * Reset value: 0x00013001
 *
 * The control register provides control for the 528MHz PLL.
 */
typedef union _hw_ccm_analog_pll_sys
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_sys_bitfields
    {
        unsigned DIV_SELECT : 1;    //!< [0] This field controls the pll loop divider.
        unsigned RESERVED0 : 11;    //!< [11:1] Reserved.
        unsigned POWERDOWN : 1;     //!< [12] Powers down the PLL.
        unsigned ENABLE : 1;        //!< [13] Enable PLL output
        unsigned BYPASS_CLK_SRC : 2;//!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;        //!< [16] Bypass the pll.
        unsigned RESERVED1 : 1;     //!< [17] Reserved
        unsigned PFD_OFFSET_EN : 1; //!< [18] Enables an offset in the phase frequency detector.
        unsigned RESERVED2 : 12;    //!< [30:19] Always set to zero (0).
        unsigned LOCK : 1;          //!< [31] 1 - PLL is currently locked; 0 - PLL is not currently locked.
    } bits;
} hw_ccm_analog_pll_sys_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_SYS_SS - 528MHz System PLL Spread Spectrum Register. (RW)
 *
 * Reset value: 0x00000000
 *
 * This register contains the 528 PLL spread spectrum controls.
 */
typedef union _hw_ccm_analog_pll_sys_ss
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_sys_ss_bitfields
    {
        unsigned STEP : 15; //!< [14:0] frequency change step = step/bits*24MHz.
        unsigned ENABLE : 1; //!< [15] This bit enables the spread spectrum modulation.
        unsigned STOP : 16; //!< [31:16] Frequency change = stop/bits*24MHz.
    } bits;
} hw_ccm_analog_pll_sys_ss_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_SYS_NUM - Numerator of 528MHz System PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x00000000
 *
 * This register contains the numerator of 528MHz PLL fractional loop divider (signed number).
 * Absoulte value should be less than denominator
 */
typedef union _hw_ccm_analog_pll_sys_num
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_sys_num_bitfields
    {
        unsigned A : 30; //!< [29:0] 30 bit numerator (A) of fractional loop divider (signed integer).
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_sys_num_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_SYS_DENOM - Denominator of 528MHz System PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x00000012
 *
 * This register contains the Denominator of 528MHz PLL fractional loop divider.
 */
typedef union _hw_ccm_analog_pll_sys_denom
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_sys_denom_bitfields
    {
        unsigned B : 30; //!< [29:0] 30 bit Denominator (B) of fractional loop divider (unsigned integer).
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_sys_denom_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_AUDIO - Analog Audio PLL control Register (RW)
 *
 * Reset value: 0x00011006
 *
 * The control register provides control for the audio PLL.
 */
typedef union _hw_ccm_analog_pll_audio
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_audio_bitfields
    {
        unsigned DIV_SELECT : 7;    //!< [6:0] This field controls the pll loop divider.
        unsigned RESERVED0 : 5;     //!< [11:7] Reserved.
        unsigned POWERDOWN : 1;     //!< [12] Powers down the PLL.
        unsigned ENABLE : 1;        //!< [13] Enable PLL output
        unsigned BYPASS_CLK_SRC : 2; //!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;        //!< [16] Bypass the pll.
        unsigned RESERVED1 : 1;     //!< [17] Revsered
        unsigned PFD_OFFSET_EN : 1; //!< [18] Enables an offset in the phase frequency detector.
        unsigned POST_DIV_SELECT : 2; //!< [20:19] These bits implement a divider after the PLL, but before the enable and bypass mux.
        unsigned SSC_EN : 1;        //!< [21] Reserved Bit
        unsigned RESERVED2 : 9;     //!< [30:22] Always set to zero (0).
        unsigned LOCK : 1;          //!< [31] 1 - PLL is currently locked.
    } bits;
} hw_ccm_analog_pll_audio_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_AUDIO_NUM - Numerator of Audio PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x05f5e100
 *
 * This register contains the numerator (A) of Audio PLL fractional loop divider.(Signed number),
 * absolute value should be less than denominator Absolute value should be less than denominator
 */
typedef union _hw_ccm_analog_pll_audio_num
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_audio_num_bitfields
    {
        unsigned A : 30;        //!< [29:0] 30 bit numerator of fractional loop divider.
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_audio_num_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_AUDIO_DENOM - Denominator of Audio PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x2964619c
 *
 * This register contains the Denominator (B) of Audio PLL fractional loop divider.(unsigned number)
 */
typedef union _hw_ccm_analog_pll_audio_denom
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_audio_denom_bitfields
    {
        unsigned B : 30;        //!< [29:0] 30 bit Denominator of fractional loop divider.
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_audio_denom_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_VIDEO - Analog Video PLL control Register (RW)
 *
 * Reset value: 0x0001100c
 *
 * The control register provides control for the Video PLL.
 */
typedef union _hw_ccm_analog_pll_video
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_video_bitfields
    {
        unsigned DIV_SELECT : 7; //!< [6:0] This field controls the pll loop divider.
        unsigned RESERVED0 : 5; //!< [11:7] Reserved.
        unsigned POWERDOWN : 1; //!< [12] Powers down the PLL.
        unsigned ENABLE : 1; //!< [13] Enalbe PLL output
        unsigned BYPASS_CLK_SRC : 2; //!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1; //!< [16] Bypass the pll.
        unsigned RESERVED1 : 1; //!< [17] Reserved
        unsigned PFD_OFFSET_EN : 1; //!< [18] Enables an offset in the phase frequency detector.
        unsigned POST_DIV_SELECT : 2; //!< [20:19] These bits implement a divider after the PLL, but before the enable and bypass mux.
        unsigned SSC_EN : 1; //!< [21] Revserved BIt
        unsigned RESERVED2 : 9; //!< [30:22] Always set to zero (0).
        unsigned LOCK : 1; //!< [31] 1 - PLL is currently locked;
    } bits;
} hw_ccm_analog_pll_video_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_VIDEO_NUM - Numerator of Video PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x05f5e100
 *
 * This register contains the numerator (A) of Video PLL fractional loop divider.(Signed number)
 * Absolute value should be less than denominator
 */
typedef union _hw_ccm_analog_pll_video_num
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_video_num_bitfields
    {
        unsigned A : 30; //!< [29:0] 30 bit numerator of fractional loop divider(Signed number), absolute value should be less than denominator
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_video_num_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_VIDEO_DENOM - Denominator of Video PLL Fractional Loop Divider Register (RW)
 *
 * Reset value: 0x10a24447
 *
 * This register contains the Denominator (B) of Video PLL fractional loop divider.(Unsigned number)
 */
typedef union _hw_ccm_analog_pll_video_denom
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_video_denom_bitfields
    {
        unsigned B : 30; //!< [29:0] 30 bit Denominator of fractional loop divider.
        unsigned RESERVED0 : 2; //!< [31:30] Always set to zero (0).
    } bits;
} hw_ccm_analog_pll_video_denom_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_MLB - MLB PLL Control Register (RW)
 *
 * Reset value: 0x00010000
 *
 * This register defines the control bits for the MLB PLL.
 */
typedef union _hw_ccm_analog_pll_mlb
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_mlb_bitfields
    {
        unsigned RESERVED0 : 11; //!< [10:0] Reserved.
        unsigned HOLD_RING_OFF : 1; //!< [11] Analog debug bit.
        unsigned PHASE_SEL : 2; //!< [13:12] Analog debut bit.
        unsigned RESERVED1 : 2; //!< [15:14] Reserved.
        unsigned BYPASS : 1; //!< [16] Bypass the PLL.
        unsigned VDDA_DELAY_CFG : 3; //!< [19:17] Configure the phase delay of the MLB PLL by adjusting the delay line in Vddio power domain.
        unsigned VDDD_DELAY_CFG : 3; //!< [22:20] Configure the phase delay of the MLB PLL by adjusting the delay line in core Vdd poser domain.
        unsigned RX_CLK_DELAY_CFG : 3; //!< [25:23] Configure the phase delay of the MLB PLL RX Clock.
        unsigned MLB_FLT_RES_CFG : 3; //!< [28:26] Configure the filter resistor for different divider ratio of MLB PLL.
        unsigned RESERVED2 : 2; //!< [30:29] Reserved.
        unsigned LOCK : 1; //!< [31] Lock bit
    } bits;
} hw_ccm_analog_pll_mlb_t;

/*!
 * @brief HW_CCM_ANALOG_PLL_ENET - Analog ENET PLL Control Register (RW)
 *
 * Reset value: 0x00011001
 *
 * The control register provides control for the ENET PLL.
 */
typedef union _hw_ccm_analog_pll_enet
{
    uint32_t        value;
    struct _hw_ccm_analog_pll_enet_bitfields
    {
        unsigned ENET0_DIV_SELECT : 2;  //!< [1:0] Controls the frequency of the ethernet0 reference clock.
        unsigned ENET1_DIV_SELECT : 2;  //!< [3:2] Controls the frequency of the ethernet1 reference clock.
        unsigned RESERVED0 : 8;         //!< [11:4] Reserved.
        unsigned POWERDOWN : 1;         //!< [12] Powers down the PLL.
        unsigned ENET1_125M_EN : 1;     //!< [13] Enable the ethernet clock output.
        unsigned BYPASS_CLK_SRC : 2;    //!< [15:14] Determines the bypass source.
        unsigned BYPASS : 1;            //!< [16] Bypass the pll.
        unsigned RESERVED1 : 1;         //!< [17] Reserved
        unsigned PFD_OFFSET_EN : 1;     //!< [18] Enables an offset in the phase frequency detector.
        unsigned ENABLE_125M : 1;       //!< [19] Enables an offset in the phase frequency detector.
        unsigned ENET2_125M_EN : 1;     //!< [20] Enable the PLL providing the ENET2 125 MHz reference clock
        unsigned ENET_25M_EN : 1;       //!< [21] Enable the PLL providing ENET 25 MHz reference clock
        unsigned RESERVED2 : 9;         //!< [30:22] Always set to zero (0).
        unsigned LOCK : 1;              //!< [31] 1 - PLL is currently locked; 0 - PLL is not currently locked.
    } bits;
} hw_ccm_analog_pll_enet_t;

/*!
 * @brief HW_CCM_ANALOG_PFD_480 - 480MHz Clock (from PLL_USB2) Phase Fractional Divider Control Register (RW)
 *
 * Reset value: 0x1311100c
 *
 * The PFD_480 control register provides control for PFD clock generation. This register controls
 * the 4-phase fractional clock dividers. The fractional clock frequencies are a product of the
 * values in these registers.
 */
typedef union _hw_ccm_analog_pfd_480
{
    uint32_t        value;
    struct _hw_ccm_analog_pfd_480_bitfields
    {
        unsigned PFD0_FRAC : 6;     //!< [5:0] This field controls the fractional divide value.
        unsigned PFD0_STABLE : 1;   //!< [6] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD0_CLKGATE : 1;  //!< [7] If set to 1, the IO fractional divider clock (reference ref_pfd0) is off (power savings).
        unsigned PFD1_FRAC : 6;     //!< [13:8] This field controls the fractional divide value.
        unsigned PFD1_STABLE : 1;   //!< [14] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD1_CLKGATE : 1;  //!< [15] IO Clock Gate.
        unsigned PFD2_FRAC : 6;     //!< [21:16] This field controls the fractional divide value.
        unsigned PFD2_STABLE : 1;   //!< [22] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD2_CLKGATE : 1;  //!< [23] IO Clock Gate.
        unsigned PFD3_FRAC : 6;     //!< [29:24] This field controls the fractional divide value.
        unsigned PFD3_STABLE : 1;   //!< [30] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD3_CLKGATE : 1;  //!< [31] IO Clock Gate.
    } bits;
} hw_ccm_analog_pfd_480_t;

/*!
 * @brief HW_CCM_ANALOG_PFD_528 - 528MHz Clock (From PLL_SYS) Phase Fractional Divider Control Register (RW)
 *
 * Reset value: 0x1018101b
 *
 * The PFD_528 control register provides control for PFD clock generation. This register controls
 * the 4-phase fractional clock dividers. The fractional clock frequencies are a product of the
 * values in these registers.
 */
typedef union _hw_ccm_analog_pfd_528
{
    uint32_t        value;
    struct _hw_ccm_analog_pfd_528_bitfields
    {
        unsigned PFD0_FRAC : 6; //!< [5:0] This field controls the fractional divide value.
        unsigned PFD0_STABLE : 1; //!< [6] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD0_CLKGATE : 1; //!< [7] If set to 1, the IO fractional divider clock (reference ref_pfd0) is off (power savings).
        unsigned PFD1_FRAC : 6; //!< [13:8] This field controls the fractional divide value.
        unsigned PFD1_STABLE : 1; //!< [14] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD1_CLKGATE : 1; //!< [15] IO Clock Gate.
        unsigned PFD2_FRAC : 6; //!< [21:16] This field controls the fractional divide value.
        unsigned PFD2_STABLE : 1; //!< [22] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD2_CLKGATE : 1; //!< [23] IO Clock Gate.
        unsigned PFD3_FRAC : 6; //!< [29:24] This field controls the fractional divide value.
        unsigned PFD3_STABLE : 1; //!< [30] This read-only bitfield is for DIAGNOSTIC PURPOSES ONLY since the fractional divider should become stable quickly enough that this field will never need to be used by either device driver or application code.
        unsigned PFD3_CLKGATE : 1; //!< [31] IO Clock Gate.
    } bits;
} hw_ccm_analog_pfd_528_t;

/*!
 * @brief HW_CCM_ANALOG_MISC0 - Miscellaneous Control Register (RW)
 *
 * Reset value: 0x02000000
 *
 * This register defines the control for miscellaneous CCM Analog blocks.
 */
typedef union _hw_ccm_analog_misc0
{
    uint32_t        value;
    struct _hw_ccm_analog_misc0_bitfields
    {
        unsigned REFTOP_PWD : 1;            //!< [0] Control bit to power-down the analog bandgap reference circuitry.
        unsigned RESERVED0 : 2;             //!< [2:1]
        unsigned REFTOP_SELFBIASOFF : 1;    //!< [3] Control bit to disable the self-bias circuit in the analog bandgap.
        unsigned REFTOP_VBGADJ:3;           //!< [6:4]
        unsigned REFTOP_VBGUP : 1;          //!< [7] Status bit that signals the analog bandgap voltage is up and stable.
        unsigned RESERVED1 : 2;             //!< [9:8]
        unsigned STOP_MODE_CONFIG : 2;      //!< [11:10] Configure the analog behavior in stop mode.
        unsigned RTC_RINGOSC_EN : 1 ;       //!< [12] Enables the internal ring oscillator that can be used in lieu of an external 32k crystal..
        unsigned OSC_I : 2;                 //!< [14:13]
        unsigned OSC_XTALOK:1;              //!< [15]
        unsigned OSC_XTALOK_EN:1;           //!< [16]
        unsigned RESERVED2:8;               //!< [24-17]
        unsigned CLKGATE_CTRL:1;            //!< [25]
        unsigned CLKGATE_DELAY:3;           //!< [28:26]
        unsigned RTC_XTAL_SOURCE:1;         //!< [29]
        unsigned XTAL_24M_PWD:1;            //!< [30]
        unsigned VID_PLL_PREDIV:1;          //!< [31]
    } bits;
} hw_ccm_analog_misc0_t;

/*!
 * @brief HW_CCM_ANALOG_MISC1 - Miscellaneous Control Register (RW)
 *
 * Reset value: 0x00000000
 *
 * This register defines the control for miscellaneous CCM Analog blocks.
 */
typedef union _hw_ccm_analog_misc1
{
    uint32_t        value;
    struct _hw_ccm_analog_misc1_bitfields
    {
        unsigned RESERVED0 :16 ;        //!< [15:0] reserved.
        unsigned PFD_480_AUTOGATE_EN : 1;//!< [16] This enables a feature that will clkgate (reset) all PFD_480 clocks anytime the USB1_PLL_480 is unlocked or powered off.
        unsigned PFD_528_AUTOGATE_EN : 1;//!< [17].
        unsigned RESERVED1 : 9;         //!< [26:18]
        unsigned IRQ_TEMPPANIC : 1;     //!< [27] temperature sensor panic interrupt asserts
        unsigned IRQ_TEMPLOW : 1;       //!< [28] the temperature sensor low interrupt asserts
        unsigned IRQ_TEMPHIGH : 1;      //!< [29] temperature sensor high interrupt asserts.
        unsigned IRQ_ANA_BO : 1;        //!< [30] analog regulator brownout interrupts assert.
        unsigned RQ_DIG_BO : 1;         //!< [31] the digital regulator brownout interrupts assert..
    } bits;
} hw_ccm_analog_misc1_t;

/*!
 * @brief HW_CCM_ANALOG_MISC2 - Miscellaneous Control Register (RW)
 *
 * Reset value: 0x00272727
 *
 * This register defines the control for miscellaneous CCM Analog blocks.
 */
typedef union _hw_ccm_analog_misc2
{
    uint32_t        value;
    struct _hw_ccm_analog_misc2_bitfields
    {
        unsigned REG0_BO_OFFSET : 3;    //!< [2:0] the brown out voltage offset for the CORE power domain
        unsigned REG0_BO_STATUS : 1;    //!< [3] Reg0 brownout status bit.
        unsigned RESERVED0 : 1;         //!< [4] reserved.
        unsigned REG0_ENABLE_BO : 1;    //!< [5] Enables the brownout detection.
        unsigned RESERVED1 : 1;         //!< [6] reserved.
        unsigned PLL3_DISABLE : 1;      //!< [7]
        unsigned REG1_BO_OFFSET : 3;    //!< [10:8] the brown out voltage offset for the xPU power domain.
        unsigned REG1_BO_STATUS : 1;    //!< [11] Reg1 brownout status bit.
        unsigned RESERVED2 : 1;         //!< [12] reserved.
        unsigned REG1_ENABLE_BO : 1;    //!< [13] Enables the brownout detection.
        unsigned RESERVED3 : 1;         //!< [14] reserved.
        unsigned AUDIO_DIV_LSB : 1;     //!< [15] LSB of Post-divider for Audio PLL..
        unsigned REG2_BO_OFFSET : 3;    //!< [18:16] the brown out voltage offset for the xPU power domain.
        unsigned REG2_BO_STATUS : 1;    //!< [19] Reg2 brownout status bit.
        unsigned RESERVED4 : 1;         //!< [20] reserved.
        unsigned REG2_ENABLE_BO : 1;    //!< [21] Enables the brownout detection.
        unsigned REG2_OK : 1;           //!< [22] Signals that the voltage is above the brownout level for the SOC supply.
        unsigned AUDIO_DIV_MSB : 1;     //!< [23] MSB of Post-divider for Audio PLL.
        unsigned REG0_STEP_TIME :2;     //!< [25:24] Number of clock periods (24MHz clock).
        unsigned REG1_STEP_TIME :2;     //!< [27:26] Number of clock periods (24MHz clock).
        unsigned REG2_STEP_TIME :2;     //!< [29:28] Number of clock periods (24MHz clock).
        unsigned VIDEO_DIV :2;          //!< [31:30] Post-divider for video.
    } bits;
} hw_ccm_analog_misc2_t;

//-------------------------------------------------------------------------------------------
// hw_ccm_analog_t - module struct
//-------------------------------------------------------------------------------------------
/*!
 * @brief All CCM_ANALOG module registers.
 */
#pragma pack(push)
#pragma pack(1)
typedef struct _hw_ccm_analog
{
    volatile hw_ccm_analog_pll_arm_t PLL_ARM; //!< Analog ARM PLL control Register
    volatile uint32_t PLL_ARM_SET; //!< Analog ARM PLL control Register Set
    volatile uint32_t PLL_ARM_CLR; //!< Analog ARM PLL control Register Clear
    volatile uint32_t PLL_ARM_TOG; //!< Analog ARM PLL control Register Toggle
    volatile hw_ccm_analog_pll_usb1_t PLL_USB1; //!< Analog USB1 480MHz PLL Control Register
    volatile uint32_t PLL_USB1_SET; //!< Analog USB1 480MHz PLL Control Register Set
    volatile uint32_t PLL_USB1_CLR; //!< Analog USB1 480MHz PLL Control Register Clear
    volatile uint32_t PLL_USB1_TOG; //!< Analog USB1 480MHz PLL Control Register Toggle
    volatile hw_ccm_analog_pll_usb2_t PLL_USB2; //!< Analog USB2 480MHz PLL Control Register
    volatile uint32_t PLL_USB2_SET; //!< Analog USB2 480MHz PLL Control Register Set
    volatile uint32_t PLL_USB2_CLR; //!< Analog USB2 480MHz PLL Control Register Clear
    volatile uint32_t PLL_USB2_TOG; //!< Analog USB2 480MHz PLL Control Register Toggle
    volatile hw_ccm_analog_pll_sys_t PLL_SYS; //!< Analog System PLL Control Register
    volatile uint32_t PLL_SYS_SET; //!< Analog System PLL Control Register Set
    volatile uint32_t PLL_SYS_CLR; //!< Analog System PLL Control Register Clear
    volatile uint32_t PLL_SYS_TOG; //!< Analog System PLL Control Register Toggle
    volatile hw_ccm_analog_pll_sys_ss_t PLL_SYS_SS; //!< 528MHz System PLL Spread Spectrum Register.
    uint32_t _reserved0[3];
    volatile hw_ccm_analog_pll_sys_num_t PLL_SYS_NUM; //!< Numerator of 528MHz System PLL Fractional Loop Divider Register
    uint32_t _reserved1[3];
    volatile hw_ccm_analog_pll_sys_denom_t PLL_SYS_DENOM; //!< Denominator of 528MHz System PLL Fractional Loop Divider Register
    uint32_t _reserved2[3];
    volatile hw_ccm_analog_pll_audio_t PLL_AUDIO; //!< Analog Audio PLL control Register
    volatile uint32_t PLL_AUDIO_SET; //!< Analog Audio PLL control Register Set
    volatile uint32_t PLL_AUDIO_CLR; //!< Analog Audio PLL control Register Clear
    volatile uint32_t PLL_AUDIO_TOG; //!< Analog Audio PLL control Register Toggle
    volatile hw_ccm_analog_pll_audio_num_t PLL_AUDIO_NUM; //!< Numerator of Audio PLL Fractional Loop Divider Register
    uint32_t _reserved3[3];
    volatile hw_ccm_analog_pll_audio_denom_t PLL_AUDIO_DENOM; //!< Denominator of Audio PLL Fractional Loop Divider Register
    uint32_t _reserved4[3];
    volatile hw_ccm_analog_pll_video_t PLL_VIDEO; //!< Analog Video PLL control Register
    volatile uint32_t PLL_VIDEO_SET; //!< Analog Video PLL control Register Set
    volatile uint32_t PLL_VIDEO_CLR; //!< Analog Video PLL control Register Clear
    volatile uint32_t PLL_VIDEO_TOG; //!< Analog Video PLL control Register Toggle
    volatile hw_ccm_analog_pll_video_num_t PLL_VIDEO_NUM; //!< Numerator of Video PLL Fractional Loop Divider Register
    uint32_t _reserved5[3];
    volatile hw_ccm_analog_pll_video_denom_t PLL_VIDEO_DENOM; //!< Denominator of Video PLL Fractional Loop Divider Register
    uint32_t _reserved6[3];
    volatile hw_ccm_analog_pll_mlb_t PLL_MLB; //!< MLB PLL Control Register
    volatile uint32_t PLL_MLB_SET; //!< MLB PLL Control Register Set
    volatile uint32_t PLL_MLB_CLR; //!< MLB PLL Control Register Clear
    volatile uint32_t PLL_MLB_TOG; //!< MLB PLL Control Register Toggle
    volatile hw_ccm_analog_pll_enet_t PLL_ENET; //!< Analog ENET PLL Control Register
    volatile uint32_t PLL_ENET_SET; //!< Analog ENET PLL Control Register Set
    volatile uint32_t PLL_ENET_CLR; //!< Analog ENET PLL Control Register Clear
    volatile uint32_t PLL_ENET_TOG; //!< Analog ENET PLL Control Register Toggle
    volatile hw_ccm_analog_pfd_480_t PFD_480; //!< 480MHz Clock (from PLL_USB2) Phase Fractional Divider Control Register
    volatile uint32_t PFD_480_SET; //!< 480MHz Clock (from PLL_USB2) Phase Fractional Divider Control Register Set
    volatile uint32_t PFD_480_CLR; //!< 480MHz Clock (from PLL_USB2) Phase Fractional Divider Control Register Clear
    volatile uint32_t PFD_480_TOG; //!< 480MHz Clock (from PLL_USB2) Phase Fractional Divider Control Register Toggle
    volatile hw_ccm_analog_pfd_528_t PFD_528; //!< 528MHz Clock (From PLL_SYS) Phase Fractional Divider Control Register
    volatile uint32_t PFD_528_SET; //!< 528MHz Clock (From PLL_SYS) Phase Fractional Divider Control Register Set
    volatile uint32_t PFD_528_CLR; //!< 528MHz Clock (From PLL_SYS) Phase Fractional Divider Control Register Clear
    volatile uint32_t PFD_528_TOG; //!< 528MHz Clock (From PLL_SYS) Phase Fractional Divider Control Register Toggle
    uint32_t _reserved7[16];
    volatile hw_ccm_analog_misc0_t MISC0; //!< Miscellaneous Control Register
    volatile uint32_t MISC0_SET; //!< Miscellaneous Control Register Set
    volatile uint32_t MISC0_CLR; //!< Miscellaneous Control Register Clear
    volatile uint32_t MISC0_TOG; //!< Miscellaneous Control Register Toggle

    volatile hw_ccm_analog_misc1_t MISC1; //!< Miscellaneous Control Register
    volatile uint32_t MISC1_SET; //!< Miscellaneous Control Register Set
    volatile uint32_t MISC1_CLR; //!< Miscellaneous Control Register Clear
    volatile uint32_t MISC1_TOG; //!< Miscellaneous Control Register Toggle

    volatile hw_ccm_analog_misc2_t MISC2; //!< Miscellaneous Control Register
    volatile uint32_t MISC2_SET; //!< Miscellaneous Control Register Set
    volatile uint32_t MISC2_CLR; //!< Miscellaneous Control Register Clear
    volatile uint32_t MISC2_TOG; //!< Miscellaneous Control Register Toggle
} hw_ccm_analog_t;
#pragma pack(pop)

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_CCM_ANALOG_REGS_H */

/* end of file */
