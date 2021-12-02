/*
 * awbl_hwconf_csi.h
 *
 *  Created on: 2018��5��15��
 *      Author: yanghuatao
 */

#ifndef __AWBL_HWCONF_IMX6UL_CSI_H
#define __AWBL_HWCONF_IMX6UL_CSI_H

#ifdef AW_DEV_IMX6UL_CSI

#include "aw_gpio.h"
#include "aw_clk.h"
#include "imx6ul_inum.h"
#include "driver/camera/awbl_imx6ul_csi.h"

#define CAMERA_DATA_BUS_ID_0      0

void __imx6ul_csi_plfm_init(void)
{
    int lpcsi_gpios[] = {
            GPIO4_17,
            GPIO4_18,
            GPIO4_19,
            GPIO4_20,
            GPIO4_21,
            GPIO4_22,
            GPIO4_23,
            GPIO4_24,
            GPIO4_25,
            GPIO4_26,
            GPIO4_27,
            GPIO4_28,
    };

    aw_clk_parent_set(IMX6UL_CLK_CSI_SEL,IMX6UL_CLK_OSC);
    aw_clk_rate_set(IMX6UL_CLK_CSI_PODF, 24000000);
    aw_clk_enable(IMX6UL_CLK_CSI);

    if (aw_gpio_pin_request("csi_gpios",
            lpcsi_gpios,
                             AW_NELEMENTS(lpcsi_gpios)) == AW_OK) {

        /* ���ù������� */
        aw_gpio_pin_cfg(GPIO4_17, GPIO4_17_CSI_MCLK);
        aw_gpio_pin_cfg(GPIO4_18, GPIO4_18_CSI_PIXCLK);
        aw_gpio_pin_cfg(GPIO4_19, GPIO4_19_CSI_VSYNC);
        aw_gpio_pin_cfg(GPIO4_20, GPIO4_20_CSI_HSYNC);

        aw_gpio_pin_cfg(GPIO4_21, GPIO4_21_CSI_DATA02);
        aw_gpio_pin_cfg(GPIO4_22, GPIO4_22_CSI_DATA03);
        aw_gpio_pin_cfg(GPIO4_23, GPIO4_23_CSI_DATA04);
        aw_gpio_pin_cfg(GPIO4_24, GPIO4_24_CSI_DATA05);
        aw_gpio_pin_cfg(GPIO4_25, GPIO4_25_CSI_DATA06);
        aw_gpio_pin_cfg(GPIO4_26, GPIO4_26_CSI_DATA07);
        aw_gpio_pin_cfg(GPIO4_27, GPIO4_27_CSI_DATA08);
        aw_gpio_pin_cfg(GPIO4_28, GPIO4_28_CSI_DATA09);
    }
}
/* �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_csi_devinfo __g_imx6ul_csi_devinfo = {
    CAMERA_DATA_BUS_ID_0,         /* ������ID */
    IMX6UL_CSI_BASE_ADDR,         /* �Ĵ�����ַ */
    INUM_CSI_IRQ,                 /* �жϺ� */
    GATED_CLOCK,                  /* CSI����ģʽ */
    USE_EXT_VSYNC,                /* ʹ���ⲿVSYNC */
    PROGRESSIVE_MODE,             /* progressiveģʽ��CCIR656����ģʽ����Ч�� */
    6,                            /* CSI�������ȼ�  */
    __imx6ul_csi_plfm_init
};
/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_csi_dev __g_imx6ul_csi_dev;

#define AWBL_HWCONF_IMX6UL_CSI                  \
{                                                \
    AWBL_IMX6UL_CSI_NAME,                       \
    1,                                           \
    AWBL_BUSID_PLB,                              \
    0,                                           \
    (struct awbl_dev *)&__g_imx6ul_csi_dev,     \
    &(__g_imx6ul_csi_devinfo)                   \
},

#else
#define AWBL_HWCONF_IMX6UL_CSI
#endif  /* AW_DEV_IMX6UL_CSI */

#endif /* __AWBL_HWCONF_IMX6UL_CSI_H */
