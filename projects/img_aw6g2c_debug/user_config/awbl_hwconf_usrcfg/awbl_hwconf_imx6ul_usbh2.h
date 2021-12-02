/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX6UL_USB_HC2_H
#define __AWBL_HWCONF_IMX6UL_USB_HC2_H

#ifdef AW_DEV_IMX6UL_USB_HOST2

#include "aw_gpio.h"
#include "aw_io.h"
#include "driver/usb/awbl_imx6ul_usbh.h"
//#include "awbl_imx6ul_usbh.h"


#define OTG_PAD_CTRL    (SRE_0_Fast_Slew_Rate | DSE_3_R0_3 | \
                            SPEED_1_medium_100MHz|     \
                            PKE_1_Pull_Keeper_Enabled | PUE_1_Pull | \
                            PUS_1_47K_Ohm_Pull_Up| HYS_1_Hysteresis_Enabled)
/* USB PHY2 �����Ĵ���ƫ��*/
#define __IMX6UL_USBPHY2_CHARGE_DETECT      (0x210)
/* USB PHY2 ��������üĴ���ƫ��*/
#define __IMX6UL_USBPHY2_CHARGE_DETECT_SET  (__IMX6UL_USBPHY2_CHARGE_DETECT+4)
/* USB PHY2 ���������Ĵ���ƫ��*/
#define __IMX6UL_USBPHY2_CHARGE_DETECT_CLR  (__IMX6UL_USBPHY2_CHARGE_DETECT+8)
/* USB PHY2 ����ⷭת�Ĵ���ƫ��*/
#define __IMX6UL_USBPHY2_CHARGE_DETECT_TOG  (__IMX6UL_USBPHY2_CHARGE_DETECT+12)
/* �رճ����*/
#define __IMX6UL_USBPHY2_CHARGE_DETECT_DISABLE  AW_BIT(20)

/** ����ƽ̨��ʼ��*/
aw_local void __imx6ul_usb_hc2_plfm_init (void)
{
    uint32_t        temp;
    void           *p = (void *)IMX6UL_PMU_REG_3P0;
    int otg2_gpios[] = {GPIO1_5};

    /* �ϵ� 3P0������*/
    temp = readl(p);
    if( !( temp & PMU_REG_3P0_OK_VDD3P0 ) ) {
        temp = PMU_REG_3P0_ENABLE| PMU_REG_3P0_ENABLE_BO;
        /* ѡ�� OTG2 VBUS��Ϊ�����ѹ*/
        temp |= PMU_REG_3P0_VBUS_SEL_OTG2;
        /* 3.0V ��ѹ*/
        temp |= PMU_REG_3P0_OUTPUT_TRIG(0xF);
        temp |= PMU_REG_3P0_BO_OFFSET(0);

        writel(temp,p);

        do {
            temp = readl(p);
        }while( !( temp & PMU_REG_3P0_OK_VDD3P0 ) ) ;
    }

    /* �رճ����*/
    p = (void *)(IMX6UL_ANALOG_DIG_BASE_ADDR +
            __IMX6UL_USBPHY2_CHARGE_DETECT_SET);
    writel(__IMX6UL_USBPHY2_CHARGE_DETECT_DISABLE,p);
    /* ����OTG����*/
    if (aw_gpio_pin_request("otg2_gpios",
                             otg2_gpios,
                             AW_NELEMENTS(otg2_gpios)) == AW_OK) {

        /* ���ó�OTG2 ID ����*/
        aw_gpio_pin_cfg(GPIO1_5,
                GPIO1_5_ANATOP_OTG2_ID|IMX6UL_PAD_CTL(OTG_PAD_CTRL));
    }

    aw_clk_enable(IMX6UL_CLK_USBOH3);
    aw_clk_enable(IMX6UL_CLK_USBPHY2_GATE);

};

/* �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_usbh_info  __g_imx6ul_usbh2_info = {
        {IMX6UL_USB2_BUSID},
        1,
        IMX6UL_USBCTRL2_BASE_ADDR,      /**< USB�Ĵ�������ַ */
        IMX6UL_USBPHY2_BASE_ADDR,       /**< USB PHY�Ĵ�������ַ */
        INUM_OTG2_IRQ,                  /**< �жϺ� */
        16,                             /**< ���������U�̣��޸ĸò���*/
        32,                             /**< ���������U�̣��޸ĸò���*/
        32,
        32,
        NULL,
        __imx6ul_usb_hc2_plfm_init      /**< ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
static struct awbl_imx6ul_usbh __g_imx6ul_usbh2_dev;

#define AWBL_HWCONF_IMX6UL_USBH2                  \
    {                                             \
        AWBL_IMX6UL_USBH_DRV_NAME,                \
        0,                                        \
        AWBL_BUSID_PLB,                           \
        0,                                        \
        (struct awbl_dev *)&__g_imx6ul_usbh2_dev, \
        &(__g_imx6ul_usbh2_info)                  \
    },

#else
#define AWBL_HWCONF_IMX6UL_USBH2

#endif  /* __AWBL_HWCONF_IMX6UL_USB_HC2_H */

#endif /* __AWBL_HWCONF_IMX6UL_USB_HC1_H */

/* end of file */
