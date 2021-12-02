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

#ifndef __AWBL_HWCONF_ADV7280_H
#define __AWBL_HWCONF_ADV7280_H

#ifdef  AW_DEV_ADV7280

#include "driver/camera/awbl_adv7280.h"
#include "aw_gpio.h"


aw_local void __adv7280_plfm_init (void)
{
    int adv7280_ctr_gpios[] = { GPIO5_5, GPIO1_30};

    if (aw_gpio_pin_request("adv7280_ctr_gpios",
            adv7280_ctr_gpios, AW_NELEMENTS(adv7280_ctr_gpios)) == AW_OK) {
        /* csi_rst */
        aw_gpio_pin_cfg(GPIO5_5,  AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PUSH_PULL);

    }

    aw_gpio_set(GPIO5_5, 0);
    aw_mdelay(12);
    aw_gpio_set(GPIO5_5, 1);
    aw_mdelay(10);
};

/* adv7280 device infomation */
aw_local aw_const struct awbl_adv7280_devinfo __g_adv7280_devinfo = {
        0,                                            /* 摄像头控制器对应的ID号  */
        4,
        ADV7280_IIC_USER_ADDR1,
        0x01,                                            /* 通道号 */
        AW_RESOLUTION_SET(720, 576),
        AW_RESOLUTION_SET(720, 576),
        (AW_CAM_VSYNC_ACTIVE_LOW |
         AW_CAM_HREF_ACTIVE_HIGH |
         AW_CAM_DATA_LATCH_ON_FALLING_EDGE),            /* 时钟极性的设置  */
        PROGRESSIVE_MODE,                               /* interlace 模式 */
        0,                                              /* 使能快速切换通道功能 */
        4,                                              /* 预览buffer个数应为 >=3 */
        "adv7280",                                      /* 摄像头名  */
        __adv7280_plfm_init
};

/* allocate wdt1 device instance RAM */
aw_local struct awbl_adv7280_dev __g_adv7280_dev;

#define  AWBL_HWCONF_ADV7280          \
    {                                \
        AWBL_ADV7280_NAME,            \
        0,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        (struct awbl_dev *)&__g_adv7280_dev, \
        &(__g_adv7280_devinfo)  \
    },

#else
#define AWBL_HWCONF_ADV7280

#endif  /* AW_DEV_ADV7280 */


#endif /* __AWBL_HWCONF_ADV7280_H */
