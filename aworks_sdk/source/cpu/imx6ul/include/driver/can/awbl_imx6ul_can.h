/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * awbl_imx6ul_can.h
 * \internal
 * \par modification history:
 * - 1.00 16-06-01  sky, first implementation
 * \endinternal
 */

#include "awbl_can.h"

#ifndef __AWBL_IMX6UL_CAN_H
#define __AWBL_IMX6UL_CAN_H

#define AWBL_IMX6UL_CAN_NAME "imx6ul_can"

/** \brief timer pin parameter */
#define IMX6UL_CAN1_RX_ENET1_RX_DATA1_ALT4      GPIO2_1
#define IMX6UL_CAN1_RX_LCD_DATA09_ALT8          GPIO3_14
#define IMX6UL_CAN1_RX_SD1_DATA1_ALT3           GPIO2_19
#define IMX6UL_CAN1_RX_UART3_RTS_B_ALT2         GPIO1_27
#define IMX6UL_CAN1_TX_ENET1_RX_DATA0_ALT4      GPIO2_0
#define IMX6UL_CAN1_TX_LCD_DATA08_ALT8          GPIO3_13
#define IMX6UL_CAN1_TX_SD1_DATA0_ALT3           GPIO2_18
#define IMX6UL_CAN1_TX_UART3_CTS_B_ALT2         GPIO1_26

#define IMX6UL_CAN2_RX_ENET1_TX_DATA0_ALT4      GPIO2_3
#define IMX6UL_CAN2_RX_LCD_DATA11_ALT8          GPIO3_16
#define IMX6UL_CAN2_RX_SD1_DATA3_ALT3           GPIO2_21
#define IMX6UL_CAN2_RX_UART2_RTS_B_ALT2         GPIO1_23
#define IMX6UL_CAN2_TX_ENET1_RX_EN_ALT4         GPIO2_2
#define IMX6UL_CAN2_TX_LCD_DATA10_ALT8          GPIO3_15
#define IMX6UL_CAN2_TX_SD1_DATA2_ALT3           GPIO2_20
#define IMX6UL_CAN2_TX_UART2_CTS_B_ALT2         GPIO1_22

/*******************************************************************************
    types
*******************************************************************************/

/** \brief imx6ul can initialization structure for plb based AWBus devices */
typedef struct awbl_imx6ul_can_dev_info {

    awbl_can_servinfo_t  servinfo;
    uint32_t             reg_base;
    uint8_t              int_num;

    void   (*pfn_plfm_init)(void);
} awbl_imx6ul_can_dev_info_t;

/** \brief imx6ul can device channel structure for awbus lite */
typedef struct awbl_imx6ul_can_dev {

    /**< \brief device */
    struct awbl_dev  dev;

     /** \brief driver functions, always goes first */
    struct awbl_can_service can_serv;

    /**< \brief 用于记录发送类型 */
    uint8_t sendtype;

} awbl_imx6ul_can_dev_t;

/**
 * \brief imx6ul CAN 驱动注册
 */
void awbl_imx6ul_flexcan_drv_register (void);

#endif /* __AWBL_IMX6UL_CAN_H */
