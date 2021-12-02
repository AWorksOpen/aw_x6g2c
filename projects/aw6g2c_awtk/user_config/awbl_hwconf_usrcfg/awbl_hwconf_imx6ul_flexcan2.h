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
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_CAN2_H
#define __AWBL_HWCONF_IMX6UL_CAN2_H

#ifdef AW_DEV_IMX6UL_CAN2

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_rngbuf.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"
#include "driver/can/awbl_imx6ul_can.h"

#define AW_CFG_CAN2_CTLR_HS         AW_CAN_CTLR_HS/**< \brief 高速CAN */
#define AW_CFG_CAN2_RX_BUF_SIZE     512

aw_local void __imx6ul_can2_plfm_init (void)
{
    int can2_gpios[] = {IMX6UL_CAN2_RX_UART2_RTS_B_ALT2, IMX6UL_CAN2_TX_UART2_CTS_B_ALT2};

    aw_clk_enable(IMX6UL_CLK_CAN2_SERIAL);
    aw_clk_enable(IMX6UL_CLK_CAN2_IPG);

    if (aw_gpio_pin_request("can2_gpios",
                             can2_gpios,
                             AW_NELEMENTS(can2_gpios)) == AW_OK) {

        /* 配置CAN RX引脚 */
        aw_gpio_pin_cfg(IMX6UL_CAN2_RX_UART2_RTS_B_ALT2, GPIO1_23_FLEXCAN2_RX);

        /* 配置CAN TX引脚 */
        aw_gpio_pin_cfg(IMX6UL_CAN2_TX_UART2_CTS_B_ALT2, GPIO1_22_FLEXCAN2_TX);
    }




}

struct aw_rngbuf  can2_rd_buf_ctr;          /**< \brief CAN2接收报文环形缓冲 */

AW_RNGBUF_SPACE_DEFINE(
        can2_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CAN2_RX_BUF_SIZE);

/** \brief CAN2 设备信息 */
aw_local aw_const struct awbl_imx6ul_can_dev_info __g_imx6ul_can2_devinfo = {
    {
        IMX6UL_FLAX_CAN2_BUSID,                 /**< \brief 当前通道号 */
        AW_CFG_CAN2_CTLR_HS,                    /**< \brief 控制器类型*/
        sizeof(struct aw_can_std_msg),          /**< \brief 一帧报文长度*/
        AW_CFG_CAN2_RX_BUF_SIZE,                /**< \brief 接收缓冲区大小*/
        AW_RNGBUF_SPACE_GET(can2_rx_msg_buff),  /**< \brief 接收缓冲区首地址*/
        &can2_rd_buf_ctr                        /**< \brief 接收rngbuf控制字*/
    },
    IMX6UL_CAN2_BASE_ADDR,                  /**< \brief 寄存器基地址*/
    INUM_CAN2_IRQ,                          /**< \brief 中断号 */
    __imx6ul_can2_plfm_init                 /**< \brief 平台初始化函数*/
};

/* \brief CAN2设备实例内存静态分配 */
aw_local struct awbl_imx6ul_can_dev __g_imx6ul_can2_dev;

#define AWBL_HWCONF_IMX6UL_CAN2     \
    {                               \
        AWBL_IMX6UL_CAN_NAME,       \
        2,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_can2_dev.dev), \
        &(__g_imx6ul_can2_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX6UL_CAN2

#endif  /* AW_DEV_IMX6UL_CAN2 */

#endif  /* __AWBL_HWCONF_IMX6UL_CAN2_H */

/* end of file */
