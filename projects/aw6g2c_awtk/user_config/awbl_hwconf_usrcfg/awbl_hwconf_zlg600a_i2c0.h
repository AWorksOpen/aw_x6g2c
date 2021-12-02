/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_ZLG600A_I2C0_H
#define __AWBL_HWCONF_ZLG600A_I2C0_H

#ifdef  AW_DEV_ZLG600A_I2C_0

#include "imx6ul_gpio_pin.h"
#include "driver/rfid/awbl_zlg600a_i2c.h"

/**
 * \addtogroup grp_aw_plfm_ext_zlg600a_i2c_usrcfg
 * @{
 */

/* 平台相关初始化 */
aw_local void __zlg600a_i2c_0_plfm_init (void)
//{
#if 0
    /* 拉低复位引脚最多2ms */
    aw_gpio_pin_cfg(GPIO3_4, AW_GPIO_OUTPUT);
    aw_gpio_set(GPIO3_4, 0);
    aw_mdelay(2);
    aw_gpio_set(GPIO3_4, 1);
    aw_mdelay(20);
#endif
}

/* ZLG600A I2C 设备信息 */
aw_local aw_const awbl_zlg600a_i2c_devinfo_t __g_zlg600a_i2c_0_devinfo = {
    {
        0,
    },
    GPIO5_6,
    AWBL_ZLG600A_FRAME_FMT_OLD,
    0xB2 >> 1,
    AWBL_ZLG600A_MODE_AUTO_CHECK,
    200,
    __zlg600a_i2c_0_plfm_init
};

/* ZLG600A I2C 设备实例内存静态分配 */
aw_local struct awbl_zlg600a_i2c_dev __g_zlg600a_i2c_0_dev;

#define AWBL_HWCONF_ZLG600A_I2C_0                   \
    {                                               \
        AWBL_ZLG600A_I2C_NAME,                      \
        0,                                          \
        AWBL_BUSID_I2C,                             \
        IMX6UL_I2C1_BUSID,                          \
        (struct awbl_dev *)&__g_zlg600a_i2c_0_dev,  \
        &__g_zlg600a_i2c_0_devinfo                  \
    },

/** @} */

#else
#define AWBL_HWCONF_ZLG600A_I2C_0

#endif  /* AW_DEV_IMX28_ZLG600A_I2C_0 */

#endif /* __AWBL_HWCONF_ZLG600A_I2C0_H */

/* end of file */

