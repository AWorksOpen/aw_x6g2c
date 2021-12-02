/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_GPIO_LED_H
#define __AWBL_HWCONF_GPIO_LED_H

#ifdef AW_DEV_GPIO_LED

/*******************************************************************************
    LED������Ϣ
*******************************************************************************/
#include "driver/led/awbl_gpio_led.h"
#include "aw_gpio.h"

/* LED�豸GPIO��Ϣ */
aw_local aw_const uint16_t __g_led_gpios[] = {
    GPIO4_14,   /* LED_ERR */
    GPIO4_16,   /* LED_RUN */
};

/* ƽ̨��س�ʼ�� */
aw_local void __awbl_led_gpio_plfm_init (void)
{
    int i = 0;
    int led_gpio[] = {GPIO4_14, GPIO4_16};
    if (aw_gpio_pin_request("led_gpios",
                            led_gpio,
                             AW_NELEMENTS(led_gpio)) == AW_OK) {
        for (i = 0; i < AW_NELEMENTS(led_gpio); i ++) {
            aw_gpio_pin_cfg(led_gpio[i], AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
        }
    }
}


/* LED�豸��Ϣ */
aw_local aw_const struct awbl_led_gpio_param __g_led_gpio_param = {
    __g_led_gpios,                      /**< \brief led ��ż���Ӧgpio����       */
    {
        0,
        AW_NELEMENTS(__g_led_gpios)-1
    },                                  /**< \brief led ��ʼ��źͽ������        */
    AW_NELEMENTS(__g_led_gpios),        /**< \brief led ����                                     */
    AW_TRUE,                            /**< \brief led ����                                     */
    __awbl_led_gpio_plfm_init
};

/* LED�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_led_gpio_dev __g_led_gpio_dev;

#define AWBL_HWCONF_GPIO_LED        \
    {                               \
        GPIO_LED_NAME,              \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_led_gpio_dev.dev),    \
        &(__g_led_gpio_param)       \
    },

#else
#define AWBL_HWCONF_GPIO_LED
#endif  /* AW_DEV_GPIO_LED */

#endif  /* __AWBL_HWCONF_GPIO_LED_H */

/* end of file */
