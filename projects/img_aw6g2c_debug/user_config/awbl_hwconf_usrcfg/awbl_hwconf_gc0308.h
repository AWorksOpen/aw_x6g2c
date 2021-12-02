/*
 * awbl_hwconf_gc0308.h
 *
 *  Created on: 2019��8��13��
 *      Author: weike
 */

#ifndef USER_CODE_AWBL_HWCONF_GC0308_H_
#define USER_CODE_AWBL_HWCONF_GC0308_H_

#ifdef  AW_DEV_GC0308

#include "driver/camera/awbl_gc0308.h"
#include "aw_gpio.h"

aw_local void __gc0308_plfm_init (void)
{
    int gc0308_ctr_gpios[] = { GPIO3_4, GPIO5_6};//, GPIO5_7, GPIO5_8};

    if (aw_gpio_pin_request("ov7725_ctr_gpios",
            gc0308_ctr_gpios, AW_NELEMENTS(gc0308_ctr_gpios)) == AW_OK) {
        /* csi_rst */
        aw_gpio_pin_cfg(GPIO3_4, AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT | AW_GPIO_PULL_UP );
        /* csi_en */
        aw_gpio_pin_cfg(GPIO5_6, AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
    }

    aw_gpio_set(GPIO5_6, 1);
    aw_gpio_set(GPIO3_4, 0);
    aw_mdelay(2);
    aw_gpio_set(GPIO5_6, 0);
    aw_mdelay(2);
    aw_gpio_set(GPIO3_4, 1);
    aw_mdelay(30);
}

aw_local aw_const struct awbl_gc0308_devinfo __g_gc0308_devinfo = {
        0,                                  /* ����ͷ��������Ӧ��ID��  */
        IMX6UL_GPIO_I2C0_BUSID,                        /* I2C �豸�� */
        AW_RESOLUTION_SET(640, 320),
        AW_RESOLUTION_SET(640, 320),
        RGB565,                                        /* ���ظ�ʽ  */
        24000000,                                      /* ����ͷоƬ������ʱ��Ƶ��  */
        (AW_CAM_VSYNC_ACTIVE_LOW |
         AW_CAM_HREF_ACTIVE_HIGH |
         AW_CAM_DATA_LATCH_ON_FALLING_EDGE),            /* ʱ�Ӽ��Ե�����  */
        4,                                             /* Ԥ��buffer����ӦΪ >=3 */
        "gc0308",                                      /* ����ͷ��  */
        __gc0308_plfm_init,
};

/* allocate wdt1 device instance RAM */
aw_local struct awbl_gc0308_dev __g_gc0308_dev;

#define  AWBL_HWCONF_GC0308          \
    {                                \
        AWBL_GC0308_NAME,            \
        0,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        (struct awbl_dev *)&__g_gc0308_dev, \
        &(__g_gc0308_devinfo)  \
    },

#else
#define AWBL_HWCONF_GC0308

#endif  /* AW_DEV_GC0308 */

#endif /* USER_CODE_AWBL_HWCONF_GC0308_H_ */
