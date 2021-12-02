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
 * \brief Freescale i.MX6UL系列PWM外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-07  hgo, first implemetation
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_PWM_H
#define __AWBL_IMX6UL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "awbl_pwm.h"

#define AWBL_IMX6UL_PWM_NAME            "imx6ul_pwm"
/*******************************************************************************
 功耗配置
*******************************************************************************/
/** \brief PWM在STOP模式下可运行  */
#define AWBL_IMX6UL_PWM_CFG_STOPEN  (0x1u << 7)
/** \brief PWM在Doze模式下可运行  */
#define AWBL_IMX6UL_PWM_CFG_DOZEN   (0x1u << 6)
/** \brief PWM在Wait模式下可运行  */
#define AWBL_IMX6UL_PWM_CFG_WAITEN  (0x1u << 5)
/** \brief PWM在Debug模式下可运行  */
#define AWBL_IMX6UL_PWM_CFG_DBGEN   (0x1u << 4)
/*******************************************************************************
 PWM时钟配置
*******************************************************************************/
/**
 * \brief PWM时钟选择为ipg_clk
 * \note  当在停止模式下PWM仍要工作时，时钟不能配置成 #AWBL_IMX6UL_PWM_CFG_CLKSRC_IPG_CLK
 *        因为停止模式下，ipg_clk已关闭。
 */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_IPG_CLK        (0x1u << 2)
/** \brief PWM时钟选择为ipg_clk_highfreq  */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_PERCLK_CLK     (0x2u << 2)
/** \brief PWM时钟选择为ipg_clk_32k  */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_CKIL_SYNC_CLK  (0x3u << 2)
/*******************************************************************************
 PWM输出特性配置
*******************************************************************************/
/** \brief 默认PWM电平为高 , 即当匹配到占空比计数时变为低电平，匹配到周期时变为高电平 */
#define AWBL_IMX6UL_PWM_CFG_LEVEL_HIGH     (0x0u << 0)
/** \brief 默认PWM电平为低 ,即 当匹配到占空比计数时变为高电平，匹配到周期时变为低电平 */
#define AWBL_IMX6UL_PWM_CFG_LEVEL_LOW      (0x1u << 0)

/** \brief PWM特性默认配置 */
#define AWBL_IMX6UL_PWM_CFG_DEFAULT  (AWBL_IMX6UL_PWM_CFG_CLKSRC_PERCLK_CLK |\
                                      AWBL_IMX6UL_PWM_CFG_LEVEL_HIGH)

#define __IMX6UL_PWM_NUM     8  /**< \brief PWM数量 */
/**
 * \brief imx6ul PWM 设备配置
 */
typedef struct awbl_imx6ul_pwm_devcfg {
    uint8_t  pid;                     /**< \brief PWM ID */
    uint32_t gpio;                    /**< \brief PWM对应的引脚 */
    uint32_t func;                    /**< \brief PWM引脚功能 */
    uint32_t dfunc;                   /**< \brief 禁能PWM后的默认引脚功能 */
    /**
     * \brief PWM特性配置
     * \note 可以使用 #AWBL_IMX6UL_PWM_CFG_*宏其中一个或多个的或值来配置
     *       一定要配置PWM的时钟源，即有且仅有AWBL_IMX6UL_PWM_CFG_CLKSRC_*宏值
     */
    uint8_t  cfgs;
}awbl_imx6ul_pwm_devcfg_t;

/**
 * \brief imx6ul PWM 设备信息
 */
typedef struct awbl_imx6ul_pwm_devinfo {
    struct awbl_pwm_servinfo pwm_servinfo;      /**< \brief PWM服务相关信息 */
    struct awbl_imx6ul_pwm_devcfg *pwm_devcfg;  /**< \brief pwm配置信息 */
    uint8_t  pnum;                              /**< \brief 使用到的pwm通道数*/
}awbl_imx6ul_pwm_devinfo_t;

/**
 * \brief imx6ul PWM 设备实例
 */
typedef struct awbl_imx6ul_pwm_dev {
    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief PWM 服务*/
    struct awbl_pwm_service pwm_serv;
    
    /** \brief 计数器源时钟*/
    aw_clk_id_t counter_src_clk_id[__IMX6UL_PWM_NUM];

}awbl_imx6ul_pwm_dev_t;

/**
 * \brief imx6ul3 pwm driver register
 */
void awbl_imx6ul3_pwm_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWBL_IMX6UL_PWM_H */
