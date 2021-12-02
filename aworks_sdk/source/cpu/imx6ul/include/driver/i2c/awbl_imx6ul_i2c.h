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
 * \brief Freescale i.MX6UL系列I2C外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 16-03-31  hgo, first implemetation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_I2C_H
#define __AWBL_IMX6UL_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_i2cbus.h"

#define AWBL_IMX6UL_I2C_NAME            "imx6ul_i2c"

/**
 * \brief imx6ul I2C 设备信息
 */
struct awbl_imx6ul_i2c_devinfo {

    /** \brief 继承自 AWBus I2C 控制器设备信息 */
    struct awbl_i2c_master_devinfo i2c_master_devinfo;

    uint32_t    regbase;                /**< 寄存器基地址 */
    int         inum;                   /**< 中断号 */
    aw_clk_id_t ref_clk_id;             /**< 输入时钟ID */
    void (*pfunc_plfm_init)(void);      /**< 平台相关初始化：开启时钟、初始化引脚 */
};


/**
 * \brief imx6ul I2C 设备实例
 */
struct awbl_imx6ul_i2c_dev {

    /** \brief 继承自 AWBus I2C 主机控制器 */
    struct awbl_i2c_master  super;

    /** \brief 传输数据计数 */
    size_t      data_ptr;

    /** \brief 用于同步传输任务的信号量 */
    AW_SEMC_DECL(i2c_sem);

    /* dir 为0 则p_buf是写buf， 为1则p_buf为读buf */
    uint8_t    dir;

    /* 码间超时时间ms */
    uint32_t    timeout;

    /* 记录最近进入中断的tick数 */
    uint32_t    late_tick;

    /** \brief 在开始新的传输前总线需要最小保持空闲的时间(us) */
    uint32_t    bus_idle_time;

    /* 传递给中断的写buffer数组  */
    struct awbl_trans_buf  *p_trans_buf;

    /* 传递给中断的写buffer个数 */
    uint32_t                trans_buf_num;
};

/**
 * \brief imx6ul I2C master driver register
 */
void awbl_imx6ul_i2c_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_IMX6UL_I2C_H_ */
