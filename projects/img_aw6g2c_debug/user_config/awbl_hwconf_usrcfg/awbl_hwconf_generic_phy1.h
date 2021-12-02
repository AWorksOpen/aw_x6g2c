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
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_GENERIC_PHY1_H
#define __AWBL_HWCONF_GENERIC_PHY1_H

#ifdef AW_DEV_GENERIC_PHY1

#include "aw_gpio.h"
#include "aw_delay.h"

#include "driver/phy/awbl_generic_phy.h"


/* Ӳ�� PHY ��ַ */
#define  PHY1_ADDR            0x05

/* PHY��λ���� */
#define  DP83848_PHY1_RESET_PIN  GPIO5_4

/* ƽ̨��س�ʼ�� */
aw_local void __generic_phy1_plfm_init (void)
{
    aw_gpio_pin_cfg(DP83848_PHY1_RESET_PIN, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);

    /**
     * Hard Reset PHY
     */
    aw_gpio_set(DP83848_PHY1_RESET_PIN, 1);
    aw_udelay(100);
    aw_gpio_set(DP83848_PHY1_RESET_PIN, 0);
    aw_udelay(100);          /* ����1us�ĵ͵�ƽ���ḴλPHYоƬ��DP83848��*/
    aw_gpio_set(DP83848_PHY1_RESET_PIN, 1);
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_generic_phy_devinfo __g_gen_phy1_devinfo = {
    "eth0",
    PHY1_ADDR,                  /* phy addr */
    AWBL_PHY_LINK_AUTONEG,      /* PHY mode */
    __generic_phy1_plfm_init,   /* ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_generic_phy_dev __g_gen_phy1_dev;

#define AWBL_HWCONF_GENERIC_PHY1                \
    {                                           \
        AWBL_GENERIC_PHY_NAME,                  \
        1,                                      \
        AWBL_BUSID_MII,                         \
        IMX6UL_MII0_BUSID,                      \
        (struct awbl_dev *)&__g_gen_phy1_dev,   \
        &__g_gen_phy1_devinfo                   \
    },

#else
#define AWBL_HWCONF_GENERIC_PHY1

#endif  /* AW_DEV_GENERIC_PHY1 */

#endif  /* __AWBL_HWCONF_GENERIC_PHY1_H */

/* end of file */
