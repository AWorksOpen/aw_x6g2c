/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef AWBL_HWCONF_FM175XX_SPI_H
#define AWBL_HWCONF_FM175XX_SPI_H

#include "aw_gpio.h"
#include "aw_delay.h"
#include "driver/card_reader/awbl_fm175xx.h"

#ifdef AW_DEV_FM175XX

/**
 * \addtogroup grp_aw_plfm_ext_fm175xx_usrcfg
 * @{
 */

#define NPD_PIN GPIO3_4
#define IRQ_PIN GPIO3_25
#define CS_PIN  GPIO3_26

/* 平台相关初始化 */
aw_local void __fm175xx_plfm_init (void)
{
    int fm175xx_gpios[] = {NPD_PIN, IRQ_PIN, CS_PIN};  /* NPD管脚, IRQ管脚，片选管脚 */

    aw_gpio_pin_request("fm175xx_gpios",
                         fm175xx_gpios,
                         AW_NELEMENTS(fm175xx_gpios));
}

/* FM175XX 设备信息 */
aw_local aw_const awbl_fm175xx_devinfo_t __g_fm175xx_dev0info = {
    AW_SPI_MODE_0,                     /* 接口时序模式 */
    CS_PIN,                            /* NSS 管脚(CS) */
    NPD_PIN,                           /* NPD管脚 */
    IRQ_PIN,                           /* IRQ 管脚 */
//    GPIO1_24,                           /* XXX GPIO1_24 作为 IRQ 管脚 LPCD不能工作 */
    10000000,                          /* SPI总线时钟 */
    AWBL_ISO_IEC_14443_A |             /* ISO/IEC 14443 A协议 */
    AWBL_ISO_IEC_14443_B,              /* ISO/IEC 14443 B协议, \note FM17510不支持 */
    AWBL_FM175XX_CFG_TYPEA_1,          /* 默认配置为支持A卡 */
    __fm175xx_plfm_init
};

/* FM175XX 设备实例内存静态分配 */
aw_local awbl_fm175xx_dev_t __g_fm175xx_dev0;

#define AWBL_HWCONF_FM175XX_0                 \
    {                                         \
        AWBL_FM175XX_NAME,                    \
        0,                                    \
        AWBL_BUSID_SPI,                       \
        IMX6UL_ECSPI1_BUSID,                  \
        &(__g_fm175xx_dev0.spi_dev.super),    \
        &__g_fm175xx_dev0info                 \
    },

/** @} */

#else
#define AWBL_HWCONF_FM175XX_0

#endif

#endif
