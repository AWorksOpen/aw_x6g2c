/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL 系列定时器驱动
 *
 * 本驱动实现了定时器的周期中断功能，以及匹配和捕获功能。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:  "imx6ul_gptimer"(32-bit timer)
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_imx6ul_gptimer_param
 *
 * \par 2.兼容设备
 *
 *  - 飞思卡尔IMX6UL系列MCU
 *
 * \par 3.设备定义/配置
 *
 *  - \ref GPTIMER
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_hwtimer
 *
 * \internal
 * \par modification history
 * - 1.00 16-05-20  sky, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_GPTIMER_H
#define __AWBL_IMX6UL_GPTIMER_H

#include "awbl_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AWBL_IMX6UL_GPT_DRV_NAME   "imx6ul_gpt"

/**
 * \addtogroup grp_awbl_drv_imx6ul_gptimer IMX6UL定时器驱动
 * \copydetails awbl_imx6ul_gptimer.h
 * @{
 */


/** \brief IMX6UL gptimer registers */
typedef struct imx6ul_gptimer_reg
{
    uint32_t cr;            /**< \brief Control */
    uint32_t pr;            /**< \brief Prescaler */
    uint32_t sr;            /**< \brief Status */
    uint32_t ir;            /**< \brief Interrupt */
    uint32_t ocr1;          /**< \brief Output Compare 1 */
    uint32_t ocr2;          /**< \brief Output Compare 2 */
    uint32_t ocr3;          /**< \brief Output Compare 3 */
    uint32_t icr1;          /**< \brief Input Capture 1 */
    uint32_t icr2;          /**< \brief Input Capture 2 */
    uint32_t cnt;           /**< \brief Counter */
} imx6ul_gptimer_reg_t;

/******************************************************************************/

/** \brief timer mode parameter */
#define IMX6UL_GPT_MODE_TIMER   0           /**< \brief 定时器功能 */
#define IMX6UL_GPT_MODE_INPUT   1           /**< \brief 输入捕获功能 */
#define IMX6UL_GPT_MODE_OUTPUT  2           /**< \brief 输出比较功能 */

/** \brief timer feature parameter */
#define IMX6UL_GPT_INPUT_DIS        0       /**< \brief 输入失效 */
#define IMX6UL_GPT_INPUT_RISE       1       /**< \brief 输入上升沿触发 */
#define IMX6UL_GPT_INPUT_FALL       2       /**< \brief 输入下降沿触发 */
#define IMX6UL_GPT_INPUT_BOTH       3       /**< \brief 输入双边沿触发 */

#define IMX6UL_GPT_OUTPUT_DIS       0       /**< \brief 输出失效 */
#define IMX6UL_GPT_OUTPUT_TOG       1       /**< \brief 输出翻转 */
#define IMX6UL_GPT_OUTPUT_CLR       2       /**< \brief 输出清零 */
#define IMX6UL_GPT_OUTPUT_SET       3       /**< \brief 输出置位 */
#define IMX6UL_GPT_OUTPUT_LOW_PULSE 4       /**< \brief 输出低脉中 */

/** \brief timer pin parameter */
#define IMX6UL_GPT1_INPUT1_GPIO1_IO00           GPIO1_0,  PIN_MUX_ALT1
#define IMX6UL_GPT1_INPUT1_UART2_TX_DATA        GPIO1_20, PIN_MUX_ALT4
#define IMX6UL_GPT1_INPUT2_ENET1_RX_ER          GPIO2_7,  PIN_MUX_ALT8
#define IMX6UL_GPT1_INPUT2_UART2_RX_DATA        GPIO1_21, PIN_MUX_ALT4
#define IMX6UL_GPT1_OUTPUT1_GPIO1_IO01          GPIO1_1,  PIN_MUX_ALT1
#define IMX6UL_GPT1_OUTPUT1_UART1_TX_DATA       GPIO1_16, PIN_MUX_ALT4
#define IMX6UL_GPT1_OUTPUT2_GPIO1_IO02          GPIO1_2,  PIN_MUX_ALT1
#define IMX6UL_GPT1_OUTPUT2_UART2_CTS_B         GPIO1_22, PIN_MUX_ALT4
#define IMX6UL_GPT1_OUTPUT3_GPIO1_IO03          GPIO1_3,  PIN_MUX_ALT1
#define IMX6UL_GPT1_OUTPUT3_UART2_RTS_B         GPIO1_23, PIN_MUX_ALT4

#define IMX6UL_GPT2_INPUT1_JTAG_TMS             GPIO1_11, PIN_MUX_ALT1
#define IMX6UL_GPT2_INPUT1_SD1_DATA2            GPIO2_20, PIN_MUX_ALT1
#define IMX6UL_GPT2_INPUT2_JTAG_TDO             GPIO1_12, PIN_MUX_ALT1
#define IMX6UL_GPT2_INPUT2_SD1_DATA3            GPIO2_21, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT1_JTAG_TDI            GPIO1_13, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT1_SD1_CMD             GPIO2_16, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT2_JTAG_TCK            GPIO1_14, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT2_SD1_CLK             GPIO2_17, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT3_JTAG_TRST_B         GPIO1_15, PIN_MUX_ALT1
#define IMX6UL_GPT2_OUTPUT3_SD1_DATA0           GPIO1_18, PIN_MUX_ALT1


/******************************************************************************/

/** \brief timer fixed parameter */
typedef struct awbl_imx6ul_gptimer_param {
    uint32_t         alloc_byname;      /**< \brief the bit with 1 means the chan the timer can any be alloc by name */
    uint32_t         reg_addr;          /**< \brief register base address */
    int              inum;              /**< \brief interrupt number */
    const aw_clk_id_t  *dev_clks;       /**< \brief the clocks which the timer need */
    int                 dev_clk_count;  /**< \brief count of device clocks */
    uint32_t         mode;              /**< \brief mode for timer, input or output */
    uint32_t         feature;           /**< \brief feature for input or output */
    uint32_t         chan;              /**< \brief chan for input or output */
    uint32_t         output_compare;    /**< \brief output compare val */
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing */
} awbl_imx6ul_gptimer_param_t;

/** \brief convert AWBus timer device info to imx6ul timer parameter */
#define awbldev_get_imx6ul_gptimer_param(p_dev) \
    ((struct awbl_imx6ul_gptimer_param *)AWBL_DEVINFO_GET(p_dev))

/******************************************************************************/

/** \brief imx6ul Timer type for AWBus */
typedef struct awbl_imx6ul_gptimer {
    /* always go first */
    awbl_dev_t              dev;                /**< \brief AWBus device data */
    struct awbl_timer       tmr;                /**< \brief AWBL timer instance */
    imx6ul_gptimer_reg_t   *reg;                /**< \brief register base */

    uint32_t                count;              /**< \brief current max counts */
    uint32_t                rollover_period;    /**< \brief rollover_period */

    struct awbl_timer_param param;              /**< \brief the timer's param */

    void (*pfunc_isr) (void *p_arg);            /**< \brief ISR */
    void                   *p_arg;              /**< \brief ISR argument */
    volatile int            enabled;
} awbl_imx6ul_gptimer_t;

/** \brief Convert a awbl_timer pointer to a awbl_imx6ul_gptimer pointer */
#define awbl_timer_to_imx6ul_gptimer(p_timer) \
    ((struct awbl_imx6ul_gptimer *)(p_timer))

/** \brief convert a awbl_dev pointer to awbl_imx6ul_gptimer pointer */
#define awbldev_to_imx6ul_gptimer(p_dev) \
          ((struct awbl_imx6ul_gptimer *)(p_dev))

/** \brief get IMX6UL gptimer parameter */
#define imx6ul_gptimer_get_param(p_imxtimer)  ((struct awbl_imx6ul_gptimer_param *) \
    AWBL_DEVINFO_GET(&((p_imxtimer)->tmr.dev)))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* } __AWBL_IMX6UL_GPTIMER_H */

/* end of file */
