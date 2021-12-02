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

#ifndef __AWBL_HWCONF_IMX6UL_SJA1000_H
#define __AWBL_HWCONF_IMX6UL_SJA1000_H

#ifdef AW_DEV_SJA1000

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_rngbuf.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"
#include "driver/can/awbl_imx6ul_can.h"
#include "driver/can/awbl_sja1000_can.h"
#include "driver/eim/awbl_imx6ul_eim.h"

#define AW_CFG_SJA1000_CAN_CTLR_HS         AW_CAN_CTLR_HS/**< \brief 高速CAN */
#define AW_CFG_SJA1000_CAN_RX_BUF_SIZE     512

#define AW_CFG_SJA1000_RST_PIN             GPIO3_25             /**< \brief 根据实际使用情况更改 */
#define AW_CFG_SJA1000_INT_PIN             GPIO3_26             /**< \brief 根据实际使用情况更改 */

aw_local void __imx6ul_sja1000_plfm_init (void)
{
    int sja1000_gpios[] = {AW_CFG_SJA1000_RST_PIN, AW_CFG_SJA1000_INT_PIN};

    aw_gpio_pin_request("sja1000_gpios",
                         sja1000_gpios,
                         AW_NELEMENTS(sja1000_gpios));
}

struct aw_rngbuf  sja1000_can_rd_buf_ctr;          /**< \brief CAN接收报文环形缓冲 */

AW_RNGBUF_SPACE_DEFINE(
        sja1000_can_rx_msg_buff, struct aw_can_std_msg, AW_CFG_SJA1000_CAN_RX_BUF_SIZE);

/** \brief sja1000 设备信息 */
aw_local aw_const struct awbl_sja1000_can_dev_info __g_imx6ul_sja1000_devinfo = {
    {
        IMX6UL_SJA1000_CAN3_BUSID,                     /**< \brief 当前通道号 */
        AW_CFG_SJA1000_CAN_CTLR_HS,                    /**< \brief 控制器类型*/
        sizeof(struct aw_can_std_msg),                 /**< \brief 一帧报文长度*/
        AW_CFG_SJA1000_CAN_RX_BUF_SIZE,                /**< \brief 接收缓冲区大小*/
        AW_RNGBUF_SPACE_GET(sja1000_can_rx_msg_buff),  /**< \brief 接收缓冲区首地址*/
        &sja1000_can_rd_buf_ctr                        /**< \brief 接收rngbuf控制字*/
    },
    AWBL_IMX6UL_EIM_CS2,
    AW_CFG_SJA1000_RST_PIN,                            /**< \brief rst 引脚编号*/
    AW_CFG_SJA1000_INT_PIN,                            /**< \brief int 引脚编号 */
    __imx6ul_sja1000_plfm_init                         /**< \brief 平台初始化函数*/
};

/* \brief sja1000设备实例内存静态分配 */
aw_local struct awbl_sja1000_can_dev __g_imx6ul_sja1000_dev;

#define AWBL_HWCONF_IMX6UL_SJA1000      \
    {                                   \
        AWBL_SJA1000_CAN_NAME,          \
        0,                              \
        AWBL_BUSID_EIM,                 \
        IMX6UL_EIM0_BUSID,                   \
        &(__g_imx6ul_sja1000_dev.dev.super), \
        &(__g_imx6ul_sja1000_devinfo)        \
    },

#else
#define AWBL_HWCONF_IMX6UL_SJA1000

#endif  /* AW_DEV_SJA1000 */

#endif  /* __AWBL_HWCONF_IMX6UL_SJA1000_H */

/* end of file */
