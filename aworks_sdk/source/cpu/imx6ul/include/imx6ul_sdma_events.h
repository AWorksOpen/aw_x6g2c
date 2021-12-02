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
 * - 1.00 02-06-16, jac, first implementation.
 * \endinternal
 */

#ifndef __IMX6UL_SDMA_EVENTS_H
#define __IMX6UL_SDMA_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_imx6ul_sdma_events definition the sdma events of imx6ul
 * @{
 */

/**
 * \name sdma events
 * @{
 */

#define SDMA_EVENTS_UART6_RX_SIM2_RECV      0   /**< \brief UART6 Rx FIFO; SIM2 receive DMA request     */
#define SDMA_EVENTS_ADC1                    1   /**< \brief ADC1 DMA request                            */
#define SDMA_EVENTS_EPIT2_PXP1              2   /**< \brief EPIT2 DMA request; PXP DMA Event            */
#define SDMA_EVENTS_ECSPI1_RX               3   /**< \brief eCSPI1 Rx request                           */
#define SDMA_EVENTS_ECSPI1_TX               4   /**< \brief eCSPI1 Tx request                           */
#define SDMA_EVENTS_ECSPI2_RX               5   /**< \brief eCSPI2 Rx request                           */
#define SDMA_EVENTS_ECSPI2_TX               6   /**< \brief eCSPI2 Tx request                           */
#define SDMA_EVENTS_ECSPI3_RX_I2C1          7   /**< \brief eCSPI3 Rx request;I2C1 DMA request          */
#define SDMA_EVENTS_ECSPI3_TX_I2C2          8   /**< \brief eCSPI3 Tx request;I2C2 DMA request          */
#define SDMA_EVENTS_ECSPI4_RX_I2C3          9   /**< \brief eCSPI4 Rx request;I2C3 DMA request          */


#define SDMA_EVENTS_ECSPI4_TX_I2C4          10  /**< \brief eCSPI4 Tx request;I2C4 DMA request          */
#define SDMA_EVENTS_QSPI_RX                 11  /**< \brief QSPI Rx request                             */
#define SDMA_EVENTS_QSPI_TX                 12  /**< \brief QSPI Tx request                             */
#define SDMA_EVENTS_ADC2_TSC                13  /**< \brief ADC2 DMA request; TSC interrupt             */
#define SDMA_EVENTS_EXTERNAL_DMA_PAD_1      14  /**< \brief external DMA pad #1                         */
#define SDMA_EVENTS_EXTERNAL_DMA_PAD_2      15  /**< \brief external DMA pad #2                         */
#define SDMA_EVENTS_EPIT1_CSI               16  /**< \brief EPIT1 DMA request: CSI DMA Event.           */
#define SDMA_EVENTS_ASRC_PAIR_A_INPUT       17  /**< \brief ASRC DMA1 request (Pair A input Request)    */
#define SDMA_EVENTS_ASRC_PAIR_B_INPUT       18  /**< \brief ASRC DMA2 request (Pair B input Request)    */
#define SDMA_EVENTS_ASRC_PAIR_C_INPUT       19  /**< \brief ASRC DMA3 request (Pair C input Request)    */


#define SDMA_EVENTS_ASRC_PAIR_A_OUTPUT      20  /**< \brief ASRC DMA4 request (Pair A output Request)   */
#define SDMA_EVENTS_ASRC_PAIR_B_OUTPUT      21  /**< \brief ASRC DMA5 request (Pair B output Request)   */
#define SDMA_EVENTS_ASRC_PAIR_C_OUTPUT      22  /**< \brief ASRC DMA6 request (Pair C output Request)   */
#define SDMA_EVENTS_GPT1                    23  /**< \brief GPT1 counter event                          */
#define SDMA_EVENTS_GPT2_LCDIF              24  /**< \brief GPT2 counter event; LCDIF DMA Event         */
#define SDMA_EVENTS_UART1_RX                25  /**< \brief UART1 Rx FIFO                               */
#define SDMA_EVENTS_UART1_TX                26  /**< \brief UART1 Tx FIFO                               */
#define SDMA_EVENTS_UART2_RX                27  /**< \brief UART2 Rx FIFO                               */
#define SDMA_EVENTS_UART2_TX                28  /**< \brief UART2 Tx FIFO                               */
#define SDMA_EVENTS_UART3_RX                29  /**< \brief UART3 Rx FIFO                               */


#define SDMA_EVENTS_UART3_TX                30  /**< \brief UART3 Tx FIFO                               */
#define SDMA_EVENTS_UART4_RX                31  /**< \brief UART4 Rx FIFO                               */
#define SDMA_EVENTS_UART4_TX                32  /**< \brief UART4 Tx FIFO                               */
#define SDMA_EVENTS_UART5_RX_SIM1_RX        33  /**< \brief UART5 Rx FIFO; SIM1 receive DMA request     */
#define SDMA_EVENTS_UART5_TX_SIM1_TX        34  /**< \brief UART5 Tx FIFO; SIM1 transmit DMA request    */
#define SDMA_EVENTS_SAI1_RX                 35  /**< \brief SAI1 Rx FIFO                                */
#define SDMA_EVENTS_SAI1_TX                 36  /**< \brief SAI1 Tx FIFO                                */
#define SDMA_EVENTS_SAI2_RX                 37  /**< \brief SAI2 Rx FIFO                                */
#define SDMA_EVENTS_SAI2_TX                 38  /**< \brief SAI2 Tx FIFO                                */
#define SDMA_EVENTS_SAI3_RX                 39  /**< \brief SAI3 Rx FIFO                                */


#define SDMA_EVENTS_SAI3_TX                 40  /**< \brief SAI3 Tx FIFO                                */
#define SDMA_EVENTS_SPDIF_RX                41  /**< \brief SPDIF Rx DMA request                        */
#define SDMA_EVENTS_SPDIF_TX                42  /**< \brief SPDIF Tx DMA request                        */
#define SDMA_EVENTS_UART7_RX_ENET1_1588_0   43  /**< \brief UART7 Rx FIFO; ENET1 1588 Event0 out        */
#define SDMA_EVENTS_UART7_RX_ENET1_1588_1   44  /**< \brief UART7 Tx FIFO; ENET1 1588 Event1 out        */
#define SDMA_EVENTS_UART8_RX_ENET2_1588_0   45  /**< \brief UART8 Rx FIFO; ENET2 1588 Event0 out        */
#define SDMA_EVENTS_UART8_TX_ENET2_1588_2   46  /**< \brief UART8 Tx FIFO; ENET2 1588 Event1 out        */
#define SDMA_EVENTS_UART6_TX_SIM2_TX        47  /**< \brief UART6 Tx FIFO; SIM2 transmit DMA request    */
/**@} sdma events */

#ifdef __cplusplus
}
#endif


#endif /* } __IMX28_INUM_H */

/* end of file */

