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
 * \brief inum defines for i.MX6UL .
 *
 * \internal
 * \par modification history:
 * - 1.00 29-09-15, jac, first implementation.
 * \endinternal
 */

#ifndef __IMX6UL_INUM_H
#define __IMX6UL_INUM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_imx6ul_inums IMX6UL平台中断号定义
 * @{
 */

/** 
 * \name CPU中断号定义
 * @{ 
 */

#define INUM_IOMUXC_IRQ             32  /**< \brief Used to notify cores on exception condition while boot.      */
#define INUM_DAP_IRQ                33  /**< \brief Debug Access Port interrupt request.                         */
#define INUM_SDMA_IRQ               34  /**< \brief SDMA interrupt request from all channels.                    */
#define INUM_TSC_IRQ                35  /**< \brief TSC interrupt.                                               */
#define INUM_SNVS_LP_HP_IRQ         36  /**< \brief Logic OR of SNVS_LP and SNVS_HP interrupts.                  */
#define INUM_LCDIF_IRQ              37  /**< \brief LCDIF sync interrupt.                                        */
#define INUM_BEE_IRQ                38  /**< \brief BEE interrupt.                                               */
#define INUM_CSI_IRQ                39  /**< \brief CMOS Sensor Interface interrupt request.                     */

#define INUM_PXP_IRQ                40  /**< \brief PXP interrupt.                                               */
#define INUM_SCTR1_IRQ              41  /**< \brief SCTR compare interrupt.                                      */
#define INUM_SCTR2_IRQ              42  /**< \brief SCTR compare interrupt.                                      */
#define INUM_WDOG3_IRQ              43  /**< \brief WDOG3 timer reset interrupt request.                         */
#define INUM_RESERVED0_IRQ          44  /**< \brief Reserved.                                                    */
#define INUM_APBHDMA_IRQ            45  /**< \brief APBH DMA channels 0-3 completion and error interrupts.       */
#define INUM_WEIM_IRQ               46  /**< \brief WEIM interrupt.                                              */
#define INUM_RAWNAND_IRQ            47  /**< \brief BCH operation complete interrupt.                            */
#define INUM_GPMI_TIMEOUT_IRQ       48  /**< \brief GPMI operation timeout error interrupt.                      */
#define INUM_UART6_IRQ              49  /**< \brief UART6 interrupt request.                                     */

#define INUM_RESERVED1_IRQ          50  /**< \brief Reserved.                                                    */
#define INUM_SRTC_CONSOLIDATED_IRQ  51  /**< \brief SRTC consolidated interrupt.                                 */
#define INUM_SRTC_SECURITY_IRQ      52  /**< \brief SRTC security interrupt.                                     */
#define INUM_CSU_IRQ                53  /**< \brief CSU interrupt request 1.                                     */
#define INUM_USDHC1_IRQ             54  /**< \brief uSDHC1 (Enhanced SDHC) interrupt request.                    */
#define INUM_USDHC2_IRQ             55  /**< \brief uSDHC2 (Enhanced SDHC) interrupt request.                    */
#define INUM_SAI3_1_IRQ             56  /**< \brief SAI3 interrupt.                                              */
#define INUM_SAI3_2_IRQ             57  /**< \brief SAI3 interrupt.                                              */
#define INUM_UART1_IRQ              58  /**< \brief UART1 interrupt request.                                     */
#define INUM_UART2_IRQ              59  /**< \brief UART2 interrupt request.                                     */

#define INUM_UART3_IRQ              60  /**< \brief UART3 interrupt request.                                     */
#define INUM_UART4_IRQ              61  /**< \brief UART4 interrupt request.                                     */
#define INUM_UART5_IRQ              62  /**< \brief UART5 interrupt request.                                     */
#define INUM_ESPI1_IRQ              63  /**< \brief ESPI1 interrupt request.                                     */
#define INUM_ESPI2_IRQ              64  /**< \brief ESPI2 interrupt request.                                     */
#define INUM_ESPI3_IRQ              65  /**< \brief ESPI3 interrupt request.                                     */
#define INUM_ESPI4_IRQ              66  /**< \brief ESPI4 interrupt request.                                     */
#define INUM_I2C4_IRQ               67  /**< \brief I2C4 interrupt request.                                      */
#define INUM_I2C1_IRQ               68  /**< \brief I2C1 interrupt request.                                      */
#define INUM_I2C2_IRQ               69  /**< \brief I2C2 interrupt request.                                      */

#define INUM_I2C3_IRQ               70  /**< \brief I2C3 interrupt request.                                      */
#define INUM_UART7_IRQ              71  /**< \brief UART7 interrupt request.                                     */
#define INUM_UART8_IRQ              72  /**< \brief UART8 interrupt request.                                     */
#define INUM_RESERVED2_IRQ          73  /**< \brief Reserved.                                                    */
#define INUM_OTG2_IRQ               74  /**< \brief USB OTG2                                                     */
#define INUM_OTG1_IRQ               75  /**< \brief USB OTG1                                                     */
#define INUM_UTMI0_IRQ              76  /**< \brief UTMI0 interrupt request.                                     */
#define INUM_UTMI1_IRQ              77  /**< \brief UTMI1 interrupt request.                                     */
#define INUM_CAAM_JR2_IRQ           78  /**< \brief CAAM interrupt queue for JQ2.                                */
#define INUM_CAAM_RECOVERABLE_ERR_IRQ 79 /**< \brief CAAM interrupt for recoverable error.                       */

#define INUM_CAAM_RTIC_IRQ          80  /**< \brief CAAM interrupt for RTIC.                                     */
#define INUM_TEMP_SENSOR_IRQ        81  /**< \brief Temperature Sensor                                           */
#define INUM_ASRC_IRQ               82  /**< \brief ASRC interrupt request.                                      */
#define INUM_RESERVED3_IRQ          83  /**< \brief Reserved.                                                    */
#define INUM_SPDIF_IRQ              84  /**< \brief SPDIF interrupt request.                                     */
#define INUM_RESERVED4_IRQ          85  /**< \brief Reserved.                                                    */
#define INUM_PMU_IRQ                86  /**< \brief Brown-out event on either the 1.1, 2.5 or 3.0 regulators.    */
#define INUM_GPT1_IRQ               87  /**< \brief GPT1 interrupt request.                                      */
#define INUM_EPIT1_IRQ              88  /**< \brief EPIT1 output compare interrupt.                              */
#define INUM_EPIT2_IRQ              89  /**< \brief EPIT2 output compare interrupt.                              */

#define INUM_INT7                   90  /**< \brief INT7 interrupt request. */
#define INUM_INT6                   91  /**< \brief INT6 interrupt request. */
#define INUM_INT5                   92  /**< \brief INT5 interrupt request. */
#define INUM_INT4                   93  /**< \brief INT4 interrupt request. */
#define INUM_INT3                   94  /**< \brief INT3 interrupt request. */
#define INUM_INT2                   95  /**< \brief INT2 interrupt request. */
#define INUM_INT1                   96  /**< \brief INT1 interrupt request. */
#define INUM_INT0                   97  /**< \brief INT0 interrupt request. */
#define INUM_GPIO1_0_15             98  /**< \brief Combined interrupt indication for GPIO1 signals 0 - 15. */
#define INUM_GPIO1_16_31            99  /**< \brief Combined interrupt indication for GPIO1 signals 16 - 31. */

#define INUM_GPIO2_0_15             100 /**< \brief Combined interrupt indication for GPIO2 signals 0 - 15. */
#define INUM_GPIO2_16_31            101 /**< \brief Combined interrupt indication for GPIO2 signals 16 - 31. */
#define INUM_GPIO3_0_15             102 /**< \brief Combined interrupt indication for GPIO3 signals 0 - 15. */
#define INUM_GPIO3_16_31            103 /**< \brief Combined interrupt indication for GPIO3 signals 16 - 31. */
#define INUM_GPIO4_0_15             104 /**< \brief Combined interrupt indication for GPIO4 signals 0 - 15. */
#define INUM_GPIO4_16_31            105 /**< \brief Combined interrupt indication for GPIO4 signals 16 - 31. */
#define INUM_GPIO5_0_15             106 /**< \brief Combined interrupt indication for GPIO5 signals 0 - 15. */
#define INUM_GPIO5_16_31            107 /**< \brief Combined interrupt indication for GPIO5 signals 16 - 31. */
#define INUM_RESERVED5_IRQ          108 /**< \brief Reserved */
#define INUM_RESERVED6_IRQ          109 /**< \brief Reserved */

#define INUM_RESERVED7_IRQ          110 /**< \brief Reserved */
#define INUM_RESERVED8_IRQ          111 /**< \brief Reserved */
#define INUM_WDOG1                  112 /**< \brief WDOG1 timer reset interrupt request. */
#define INUM_WDOG2                  113 /**< \brief WDOG2 timer reset interrupt request. */
#define INUM_KPP                    114 /**< \brief Key Pad interrupt request. */
#define INUM_PWM1                   115 /**< \brief Cumulative interrupt line for PWM1. */
#define INUM_PWM2                   116 /**< \brief Cumulative interrupt line for PWM2. */
#define INUM_PWM3                   117 /**< \brief Cumulative interrupt line for PWM3. */
#define INUM_PWM4                   118 /**< \brief Cumulative interrupt line for PWM4. */
#define INUM_CCM_1                  119 /**< \brief CCM interrupt request 1. */

#define INUM_CCM_2                  120 /**< \brief CCM interrupt request 2. */
#define INUM_GPC                    121 /**< \brief GPC interrupt request 1. */
#define INUM_RESERVED9_IRQ          122 /**< \brief Reserved. */
#define INUM_SRC                    123 /**< \brief SRC interrupt request. */
#define INUM_RESERVED10_IRQ         124 /**< \brief Reserved. */
#define INUM_RESERVED11_IRQ         125 /**< \brief Reserved. */
#define INUM_CPU_PUI                126 /**< \brief Performance Unit interrupt. */
#define INUM_CPU_CTI                127 /**< \brief CTI trigger outputs interrupt. */
#define INUM_SRC_WDOG               128 /**< \brief Combined CPU wdog interrupts (4x) out of SRC. */
#define INUM_SAI1                   129 /**< \brief SAI1 interrupt request. */

#define INUM_SAI2                   130 /**< \brief SAI2 interrupt request. */
#define INUM_RESERVED12_IRQ         131 /**< \brief Reserved. */
#define INUM_ADC1_IRQ               132 /**< \brief ADC1 interrupt request. */
#define INUM_ADC2_IRQ               133 /**< \brief ADC2 interrupt request. */
#define INUM_RESERVED13_IRQ         134 /**< \brief Reserved. */
#define INUM_RESERVED14_IRQ         135 /**< \brief Reserved. */
#define INUM_SJC                    136 /**< \brief SJC interrupt from General Purpose register. */
#define INUM_CAAM_JR0_IRQ           137 /**< \brief CAAM job ring 0 interrupt. */
#define INUM_CAAM_JR1_IRQ           138 /**< \brief CAAM job ring 1 interrupt. */
#define INUM_QSPI                   139 /**< \brief QSP1 interrupt request. */

#define INUM_TZASC                  140 /**< \brief SAI1 interrupt request. */
#define INUM_GPT2_IRQ               141 /**< \brief GPT2 interrupt request. */
#define INUM_CAN1_IRQ               142 /**< \brief CAN1 interrupt request. */
#define INUM_CAN2_IRQ               143 /**< \brief CAN2 interrupt request. */
#define INUM_SIM1                   144 /**< \brief SIM1 interrupt. */
#define INUM_SIM2                   145 /**< \brief SIM2 interrupt. */
#define INUM_PWM5                   146 /**< \brief PWM5. */
#define INUM_PWM6                   147 /**< \brief PWM6. */
#define INUM_PWM7                   148 /**< \brief PWM7. */
#define INUM_PWM8                   149 /**< \brief PWM8. */

#define INUM_ENET1                  150 /**< \brief ENET1 interrupt */
#define INUM_ENET1_1588             151 /**< \brief ENET1 1588 Timer interrupt request. */
#define INUM_ENET2                  152 /**< \brief ENET2 interrupt. */
#define INUM_ENET2_1588             153 /**< \brief ENET2 1588 Timer interrupt request. */
#define INUM_RESERVED15_IRQ         154 /**< \brief Reserved. */
#define INUM_RESERVED16_IRQ         155 /**< \brief Reserved. */
#define INUM_RESERVED17_IRQ         156 /**< \brief Reserved */
#define INUM_RESERVED18_IRQ         157 /**< \brief Reserved. */
#define INUM_RESERVED19_IRQ         158 /**< \brief Reserved. */
#define INUM_PMU                    159 /**< \brief Brown-out event on either core, gpu or soc regulators. */

#define INUM_GPIO_PIN0              160 /**< \brief GPIO PIN0 */
#define INUM_GPIO_PIN1              161 /**< \brief GPIO PIN1 */
#define INUM_GPIO_PIN2              162 /**< \brief GPIO PIN2 */
#define INUM_GPIO_PIN3              163 /**< \brief GPIO PIN3 */
#define INUM_GPIO_PIN4              164 /**< \brief GPIO PIN4 */
#define INUM_GPIO_PIN5              165 /**< \brief GPIO PIN5 */
#define INUM_GPIO_PIN6              166 /**< \brief GPIO PIN6 */
#define INUM_GPIO_PIN7              167 /**< \brief GPIO PIN7 */
#define INUM_GPIO_PIN8              168 /**< \brief GPIO PIN8 */
#define INUM_GPIO_PIN9              169 /**< \brief GPIO PIN9 */
#define INUM_GPIO_PIN10             170 /**< \brief GPIO PIN10 */
#define INUM_GPIO_PIN11             171 /**< \brief GPIO PIN11 */
#define INUM_GPIO_PIN12             172 /**< \brief GPIO PIN12 */
#define INUM_GPIO_PIN13             173 /**< \brief GPIO PIN13 */
#define INUM_GPIO_PIN14             174 /**< \brief GPIO PIN14 */
#define INUM_GPIO_PIN15             175 /**< \brief GPIO PIN15 */
#define INUM_GPIO_PIN16             176 /**< \brief GPIO PIN16 */
#define INUM_GPIO_PIN17             177 /**< \brief GPIO PIN17 */
#define INUM_GPIO_PIN18             178 /**< \brief GPIO PIN18 */
#define INUM_GPIO_PIN19             179 /**< \brief GPIO PIN19 */
#define INUM_GPIO_PIN20             180 /**< \brief GPIO PIN20 */
#define INUM_GPIO_PIN21             181 /**< \brief GPIO PIN21 */
#define INUM_GPIO_PIN22             182 /**< \brief GPIO PIN22 */
#define INUM_GPIO_PIN23             183 /**< \brief GPIO PIN23 */
#define INUM_GPIO_PIN24             184 /**< \brief GPIO PIN24 */
#define INUM_GPIO_PIN25             185 /**< \brief GPIO PIN25 */
#define INUM_GPIO_PIN26             186 /**< \brief GPIO PIN26 */
#define INUM_GPIO_PIN27             187 /**< \brief GPIO PIN27 */
#define INUM_GPIO_PIN28             188 /**< \brief GPIO PIN28 */
#define INUM_GPIO_PIN29             189 /**< \brief GPIO PIN29 */
#define INUM_GPIO_PIN30             190 /**< \brief GPIO PIN30 */
#define INUM_GPIO_PIN31             191 /**< \brief GPIO PIN31 */
#define INUM_GPIO_PIN32             192 /**< \brief GPIO PIN32 */
#define INUM_GPIO_PIN33             193 /**< \brief GPIO PIN33 */
#define INUM_GPIO_PIN34             194 /**< \brief GPIO PIN34 */
#define INUM_GPIO_PIN35             195 /**< \brief GPIO PIN35 */
#define INUM_GPIO_PIN36             196 /**< \brief GPIO PIN36 */
#define INUM_GPIO_PIN37             197 /**< \brief GPIO PIN37 */
#define INUM_GPIO_PIN38             198 /**< \brief GPIO PIN38 */
#define INUM_GPIO_PIN39             199 /**< \brief GPIO PIN39 */
#define INUM_GPIO_PIN40             200 /**< \brief GPIO PIN40 */
#define INUM_GPIO_PIN41             201 /**< \brief GPIO PIN41 */
#define INUM_GPIO_PIN42             202 /**< \brief GPIO PIN42 */
#define INUM_GPIO_PIN43             203 /**< \brief GPIO PIN43 */
#define INUM_GPIO_PIN44             204 /**< \brief GPIO PIN44 */
#define INUM_GPIO_PIN45             205 /**< \brief GPIO PIN45 */
#define INUM_GPIO_PIN46             206 /**< \brief GPIO PIN46 */
#define INUM_GPIO_PIN47             207 /**< \brief GPIO PIN47 */
#define INUM_GPIO_PIN48             208 /**< \brief GPIO PIN48 */
#define INUM_GPIO_PIN49             209 /**< \brief GPIO PIN49 */
#define INUM_GPIO_PIN50             210 /**< \brief GPIO PIN50 */
#define INUM_GPIO_PIN51             211 /**< \brief GPIO PIN51 */
#define INUM_GPIO_PIN52             212 /**< \brief GPIO PIN52 */
#define INUM_GPIO_PIN53             213 /**< \brief GPIO PIN53 */
#define INUM_GPIO_PIN54             214 /**< \brief GPIO PIN54 */
#define INUM_GPIO_PIN55             215 /**< \brief GPIO PIN55 */
#define INUM_GPIO_PIN56             216 /**< \brief GPIO PIN56 */
#define INUM_GPIO_PIN57             217 /**< \brief GPIO PIN57 */
#define INUM_GPIO_PIN58             218 /**< \brief GPIO PIN58 */
#define INUM_GPIO_PIN59             219 /**< \brief GPIO PIN59 */
#define INUM_GPIO_PIN60             220 /**< \brief GPIO PIN60 */
#define INUM_GPIO_PIN61             221 /**< \brief GPIO PIN61 */
#define INUM_GPIO_PIN62             222 /**< \brief GPIO PIN62 */
#define INUM_GPIO_PIN63             223 /**< \brief GPIO PIN63 */
#define INUM_GPIO_PIN64             224 /**< \brief GPIO PIN64 */
#define INUM_GPIO_PIN65             225 /**< \brief GPIO PIN65 */
#define INUM_GPIO_PIN66             226 /**< \brief GPIO PIN66 */
#define INUM_GPIO_PIN67             227 /**< \brief GPIO PIN67 */
#define INUM_GPIO_PIN68             228 /**< \brief GPIO PIN68 */
#define INUM_GPIO_PIN69             229 /**< \brief GPIO PIN69 */
#define INUM_GPIO_PIN70             230 /**< \brief GPIO PIN70 */
#define INUM_GPIO_PIN71             231 /**< \brief GPIO PIN71 */
#define INUM_GPIO_PIN72             232 /**< \brief GPIO PIN72 */
#define INUM_GPIO_PIN73             233 /**< \brief GPIO PIN73 */
#define INUM_GPIO_PIN74             234 /**< \brief GPIO PIN74 */
#define INUM_GPIO_PIN75             235 /**< \brief GPIO PIN75 */
#define INUM_GPIO_PIN76             236 /**< \brief GPIO PIN76 */
#define INUM_GPIO_PIN77             237 /**< \brief GPIO PIN77 */
#define INUM_GPIO_PIN78             238 /**< \brief GPIO PIN78 */
#define INUM_GPIO_PIN79             239 /**< \brief GPIO PIN79 */
#define INUM_GPIO_PIN80             240 /**< \brief GPIO PIN80 */
#define INUM_GPIO_PIN81             241 /**< \brief GPIO PIN81 */
#define INUM_GPIO_PIN82             242 /**< \brief GPIO PIN82 */
#define INUM_GPIO_PIN83             243 /**< \brief GPIO PIN83 */
#define INUM_GPIO_PIN84             244 /**< \brief GPIO PIN84 */
#define INUM_GPIO_PIN85             245 /**< \brief GPIO PIN85 */
#define INUM_GPIO_PIN86             246 /**< \brief GPIO PIN86 */
#define INUM_GPIO_PIN87             247 /**< \brief GPIO PIN87 */
#define INUM_GPIO_PIN88             248 /**< \brief GPIO PIN88 */
#define INUM_GPIO_PIN89             249 /**< \brief GPIO PIN89 */
#define INUM_GPIO_PIN90             250 /**< \brief GPIO PIN90 */
#define INUM_GPIO_PIN91             251 /**< \brief GPIO PIN91 */
#define INUM_GPIO_PIN92             252 /**< \brief GPIO PIN92 */
#define INUM_GPIO_PIN93             253 /**< \brief GPIO PIN93 */
#define INUM_GPIO_PIN94             254 /**< \brief GPIO PIN94 */
#define INUM_GPIO_PIN95             255 /**< \brief GPIO PIN95 */
#define INUM_GPIO_PIN96             256 /**< \brief GPIO PIN96 */
#define INUM_GPIO_PIN97             257 /**< \brief GPIO PIN97 */
#define INUM_GPIO_PIN98             258 /**< \brief GPIO PIN98 */
#define INUM_GPIO_PIN99             259 /**< \brief GPIO PIN99 */
#define INUM_GPIO_PIN100            260 /**< \brief GPIO PIN100 */
#define INUM_GPIO_PIN101            261 /**< \brief GPIO PIN101 */
#define INUM_GPIO_PIN102            262 /**< \brief GPIO PIN102 */
#define INUM_GPIO_PIN103            263 /**< \brief GPIO PIN103 */
#define INUM_GPIO_PIN104            264 /**< \brief GPIO PIN104 */
#define INUM_GPIO_PIN105            265 /**< \brief GPIO PIN105 */
#define INUM_GPIO_PIN106            266 /**< \brief GPIO PIN106 */
#define INUM_GPIO_PIN107            267 /**< \brief GPIO PIN107 */
#define INUM_GPIO_PIN108            268 /**< \brief GPIO PIN108 */
#define INUM_GPIO_PIN109            269 /**< \brief GPIO PIN109 */
#define INUM_GPIO_PIN110            270 /**< \brief GPIO PIN110 */
#define INUM_GPIO_PIN111            271 /**< \brief GPIO PIN111 */
#define INUM_GPIO_PIN112            272 /**< \brief GPIO PIN112 */
#define INUM_GPIO_PIN113            273 /**< \brief GPIO PIN113 */
#define INUM_GPIO_PIN114            274 /**< \brief GPIO PIN114 */
#define INUM_GPIO_PIN115            275 /**< \brief GPIO PIN115 */
#define INUM_GPIO_PIN116            276 /**< \brief GPIO PIN116 */
#define INUM_GPIO_PIN117            277 /**< \brief GPIO PIN117 */
#define INUM_GPIO_PIN118            278 /**< \brief GPIO PIN118 */
#define INUM_GPIO_PIN119            279 /**< \brief GPIO PIN119 */
#define INUM_GPIO_PIN120            280 /**< \brief GPIO PIN120 */
#define INUM_GPIO_PIN121            281 /**< \brief GPIO PIN121 */
#define INUM_GPIO_PIN122            282 /**< \brief GPIO PIN122 */
#define INUM_GPIO_PIN123            283 /**< \brief GPIO PIN123 */
#define INUM_GPIO_PIN124            284 /**< \brief GPIO PIN124 */
#define INUM_GPIO_PIN125            285 /**< \brief GPIO PIN125 */
#define INUM_GPIO_PIN126            286 /**< \brief GPIO PIN126 */
#define INUM_GPIO_PIN127            287 /**< \brief GPIO PIN127 */
#define INUM_GPIO_PIN128            288 /**< \brief GPIO PIN128 */
#define INUM_GPIO_PIN129            289 /**< \brief GPIO PIN129 */
#define INUM_GPIO_PIN130            290 /**< \brief GPIO PIN130 */
#define INUM_GPIO_PIN131            291 /**< \brief GPIO PIN131 */
#define INUM_GPIO_PIN132            292 /**< \brief GPIO PIN132 */
#define INUM_GPIO_PIN133            293 /**< \brief GPIO PIN133 */
#define INUM_GPIO_PIN134            294 /**< \brief GPIO PIN134 */
#define INUM_GPIO_PIN135            295 /**< \brief GPIO PIN135 */
#define INUM_GPIO_PIN136            296 /**< \brief GPIO PIN136 */
#define INUM_GPIO_PIN137            297 /**< \brief GPIO PIN137 */
#define INUM_GPIO_PIN138            298 /**< \brief GPIO PIN138 */
#define INUM_GPIO_PIN139            299 /**< \brief GPIO PIN139 */
#define INUM_GPIO_PIN140            300 /**< \brief GPIO PIN140 */
#define INUM_GPIO_PIN141            301 /**< \brief GPIO PIN141 */
#define INUM_GPIO_PIN142            302 /**< \brief GPIO PIN142 */
#define INUM_GPIO_PIN143            303 /**< \brief GPIO PIN143 */
#define INUM_GPIO_PIN144            304 /**< \brief GPIO PIN144 */
#define INUM_GPIO_PIN145            305 /**< \brief GPIO PIN145 */
#define INUM_GPIO_PIN146            306 /**< \brief GPIO PIN146 */
#define INUM_GPIO_PIN147            307 /**< \brief GPIO PIN147 */
#define INUM_GPIO_PIN148            308 /**< \brief GPIO PIN148 */
#define INUM_GPIO_PIN149            309 /**< \brief GPIO PIN149 */
#define INUM_GPIO_PIN150            310 /**< \brief GPIO PIN150 */
#define INUM_GPIO_PIN151            311 /**< \brief GPIO PIN151 */
#define INUM_GPIO_PIN152            312 /**< \brief GPIO PIN152 */
#define INUM_GPIO_PIN153            313 /**< \brief GPIO PIN153 */
#define INUM_GPIO_PIN154            314 /**< \brief GPIO PIN154 */
#define INUM_GPIO_PIN155            315 /**< \brief GPIO PIN155 */
#define INUM_GPIO_PIN156            316 /**< \brief GPIO PIN156 */
#define INUM_GPIO_PIN157            317 /**< \brief GPIO PIN157 */
#define INUM_GPIO_PIN158            318 /**< \brief GPIO PIN158 */
#define INUM_GPIO_PIN159            319 /**< \brief GPIO PIN159 */


/**@} CPU中断号定义*/

#ifdef __cplusplus
}
#endif


#endif /* } __IMX28_INUM_H */

/* end of file */

