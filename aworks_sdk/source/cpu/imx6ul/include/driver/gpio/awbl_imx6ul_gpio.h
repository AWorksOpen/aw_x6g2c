/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX6UL GPIO 驱动
 *
 * iMX6UL GPIO 为 iMX6UL 系列MCU的片上GPIO，最多可以有160个GPIO引脚可用，有如下
 * 特性:
 *  - 引脚可以由软件配置为输入/输出
 *  - 每个单独引脚可被用作外部中断输入引脚
 *  - 每个中断可配置为下降沿、上升沿或高低电平产生中断
 *  - 所有GPIO 引脚默认为输入
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "imx6ul_gpio"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_imx6ul_gpio_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-10-24, jac, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_GPIO_H
#define __AWBL_IMX6UL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio IMX6UL GPIO驱动
 * \copydetails awbl_imx6ul_gpio.h
 * @{
 */

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_regs IMX6UL GPIO寄存器定义
 * @{
 */

#pragma pack(push)
#pragma pack(1)

/** \brief imx6ul GPIO每bank寄存器 */
typedef struct __tag_imx6ul_gpio_bank_regs
{
    uint32_t    data;           /**< \brief GPIO data register */
    uint32_t    dir;            /**< \brief GPIO direction register */
    uint32_t    psr;            /**< \brief GPIO pad status register */
    uint32_t    icr1;           /**< \brief GPIO interrupt configuration register1 */
    uint32_t    icr2;           /**< \brief GPIO interrupt configuration register2 */
    uint32_t    imr;            /**< \brief GPIO interrupt mask register */
    uint32_t    isr;            /**< \brief GPIO interrupt status register */
    uint32_t    edge_sel;       /**< \brief GPIO edge select register */
}imx6ul_gpio_bank_regs_t;

/** \brief imx6ul GPIO寄存器 */
typedef struct __tag_imx6ul_gpio_regs
{
    imx6ul_gpio_bank_regs_t     bank1;          /**< \brief GPIO bank1 */
    uint32_t                    reserved1[4088];
    imx6ul_gpio_bank_regs_t     bank2;          /**< \brief GPIO bank2 */
    uint32_t                    reserved2[4088];
    imx6ul_gpio_bank_regs_t     bank3;          /**< \brief GPIO bank3 */
    uint32_t                    reserved3[4088];
    imx6ul_gpio_bank_regs_t     bank4;          /**< \brief GPIO bank4 */
    uint32_t                    reserved4[4088];
    imx6ul_gpio_bank_regs_t     bank5;          /**< \brief GPIO bank5 */
    uint32_t                    reserved5[4088];
}imx6ul_gpio_regs_t;

/** \brief 从GPIO寄存器基址获取某个端口对应寄存器的基址 */
#define GET_GPIO_BANK_REGS(p_gpio,x)  (imx6ul_gpio_bank_regs_t *)((uint32_t *)(p_gpio)+4096*(x) )

#pragma pack(pop)

/** @} grp_awbl_drv_imx6ul_gpio_regs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_micro_defs IMX6UL GPIO驱动宏定义
 * @{
 */

/** \brief imx6ul GPIO 驱动名 */
#define AWBL_IMX6UL_GPIO_NAME   "imx6ul_gpio"

/** \brief imx6ul GPIO 的端口(bank)数 */
#define  IMX6UL_GPIO_PORTS   5
/** \brief imx6ul GPIO 每端口含的GPIO个数 */
#define  IMX6UL_PORT_PINS    32

/** @} grp_awbl_drv_imx6ul_gpio_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_structs IMX6UL GPIO驱动相关的结构
 * @{
 */

/** \brief imx6ul GPIO平台初始化函数指针 */
typedef void (*pfn_gpio_plat_init_t)(void);

/** \brief imx6ul GPIO 设备信息 */
struct awbl_imx6ul_gpio_info
{
    pfn_gpio_plat_init_t         plat_init; /**< \brief 平台相关的初始化函数 */
    uint32_t                     mem_base;  /**< \brief GPIO 寄存器基地址 */
    uint32_t                     iomuxc_base;/**< \brief IOMUXC 寄存器基地址 */
    struct awbl_gpio_servinfo    gpio_info; /**< \brief GPIO 服务配置信息 */
    struct awbl_intctlr_servinfo int_info;  /**< \brief IntCtlr 服务配置信息 */
    uint8_t  *p_pin_cfg;   /**< \brief GPIO 引脚配置状态 */
};

/******************************************************************************/
/** \brief GPIO引脚中断触发信息结构 */
struct awbl_imx6ul_gpio_trigger_info
{
    aw_pfuncvoid_t  pfunc_callback;     /**< \brief 中断回调函数 */
    void           *p_arg;              /**< \brief 传递给回调函数的参数 */
};

/******************************************************************************/

/******************************************************************************/
/** \brief imx28x GPIO 设备实例 */
struct awbl_imx6ul_gpio_dev
{
    struct awbl_dev             awdev;          /**< \brief 继承自 AWBus 设备 */
    struct awbl_gpio_service    gpio_serv;      /**< \brief GPIO 服务 */
    struct awbl_intctlr_service intctlr_serv;   /**< \brief IntCtlr 服务 */
    imx6ul_gpio_regs_t          *reg;
    uint32_t                     iomuxc_base;   /**< \brief IOMUXC 寄存器基地址 */
    /** \brief GPIO 引脚ISR信息 */
    struct awbl_imx6ul_gpio_trigger_info trig_info[IMX6UL_GPIO_PORTS][IMX6UL_PORT_PINS];
    /** \brief GPIO 引脚触发状态 */
    uint8_t  trig_state[IMX6UL_GPIO_PORTS][IMX6UL_PORT_PINS];
};
/** \brief 从Awbus 设备到imx6ul gpio设备 */
#define awbldev_to_imx6ul_gpio_dev(dev)     ((struct awbl_imx6ul_gpio_dev *)(dev))
/** \brief 从Awbus 设备到imx6ul gpio设备信息 */
#define imx6ul_gpio_get_param(dev)          ((struct awbl_imx6ul_gpio_info *)AWBL_DEVINFO_GET(dev))

/** @} grp_awbl_drv_imx6ul_gpio_structs*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_awbus_interface GPIO驱动Awbus接口
 * @{
 */
/** \brief 用户调用此函数注册GPIO驱动 */
void awbl_imx6ul_gpio_drv_register (void);

/** @} grp_awbl_drv_imx6ul_gpio_usr_register */
/** @} grp_awbl_drv_imx6ul_gpio*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
