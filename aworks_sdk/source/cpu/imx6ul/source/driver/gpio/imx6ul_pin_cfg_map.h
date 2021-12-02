/*******************************************************************************
 *                                 AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn/
 *******************************************************************************/

/**
 * \file  imx6ul_pin_cfg_map.h
 * \brief 定义imx6ul引脚配置结构体
 *
 * \internal
 * \par modification history:
 * - 2018-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __IMX6UL_PIN_CFG_MAP_H__
#define         __IMX6UL_PIN_CFG_MAP_H__

typedef struct __select_input_cfg{
    uint16_t        select_input_offset;
    uint16_t        select_input_val;
}select_input_cfg_t;

typedef struct __tag_imx6ul_gpio_pin_cfg_map
{
    uint16_t            mux_ctl_offset;
    uint16_t            pad_ctl_offset;
    select_input_cfg_t  select_input_cfgs[9];
}imx6ul_pin_cfg_map_t;

aw_const imx6ul_pin_cfg_map_t *_imx6ul_get_pin_cfg_map(int pin);

#endif          //__IMX6UL_PIN_CFG_MAP_H__
