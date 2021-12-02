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

#ifndef __AWBL_HWCONF_IMX6UL_CAN1_H
#define __AWBL_HWCONF_IMX6UL_CAN1_H

#ifdef AW_DEV_IMX6UL_CAN1

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_rngbuf.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"
#include "driver/can/awbl_imx6ul_can.h"

#define AW_CFG_CAN1_CTLR_HS         AW_CAN_CTLR_HS/**< \brief ����CAN */
#define AW_CFG_CAN1_RX_BUF_SIZE     512

aw_local void __imx6ul_can1_plfm_init (void)
{
    int can1_gpios[] = {IMX6UL_CAN1_RX_UART3_RTS_B_ALT2, IMX6UL_CAN1_TX_UART3_CTS_B_ALT2};

    aw_clk_enable(IMX6UL_CLK_CAN1_SERIAL);
    aw_clk_enable(IMX6UL_CLK_CAN1_IPG);

    if (aw_gpio_pin_request("can1_gpios",
                             can1_gpios,
                             AW_NELEMENTS(can1_gpios)) == AW_OK) {

        /* ����CAN RX���� */
        aw_gpio_pin_cfg(IMX6UL_CAN1_RX_UART3_RTS_B_ALT2, GPIO1_27_FLEXCAN1_RX);

        /* ����CAN TX���� */
        aw_gpio_pin_cfg(IMX6UL_CAN1_TX_UART3_CTS_B_ALT2, GPIO1_26_FLEXCAN1_TX);
    }
}

struct aw_rngbuf  can1_rd_buf_ctr;          /**< \brief CAN1���ձ��Ļ��λ��� */

AW_RNGBUF_SPACE_DEFINE(
        can1_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CAN1_RX_BUF_SIZE);

/** \brief CAN1 �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_can_dev_info __g_imx6ul_can1_devinfo = {
    {
        IMX6UL_FLAX_CAN1_BUSID,                 /**< \brief ��ǰͨ���� */
        AW_CFG_CAN1_CTLR_HS,                    /**< \brief ����������*/
        sizeof(struct aw_can_std_msg),          /**< \brief һ֡���ĳ���*/
        AW_CFG_CAN1_RX_BUF_SIZE,                /**< \brief ���ջ�������С*/
        AW_RNGBUF_SPACE_GET(can1_rx_msg_buff),  /**< \brief ���ջ������׵�ַ*/
        &can1_rd_buf_ctr                        /**< \brief ����rngbuf������*/
    },
    IMX6UL_CAN1_BASE_ADDR,                  /**< \brief �Ĵ�������ַ*/
    INUM_CAN1_IRQ,                          /**< \brief �жϺ� */
    __imx6ul_can1_plfm_init                 /**< \brief ƽ̨��ʼ������*/
};

/* \brief CAN1�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_can_dev __g_imx6ul_can1_dev;

#define AWBL_HWCONF_IMX6UL_CAN1     \
    {                               \
        AWBL_IMX6UL_CAN_NAME,       \
        1,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_can1_dev.dev), \
        &(__g_imx6ul_can1_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX6UL_CAN1

#endif  /* AW_DEV_IMX6UL_CAN1 */

#endif  /* __AWBL_HWCONF_IMX6UL_CAN1_H */

/* end of file */
