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
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite 硬件配置文件
 */

/*******************************************************************************
  headers
*******************************************************************************/

#undef  AW_IMG_PRJ_BUILD
#define AW_IMG_PRJ_BUILD


#include "apollo.h"
#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_io.h"
#include "aw_delay.h"
#include "aw_prj_params.h"
#include "imx6ul_inum.h"
#include "imx6ul_sdma_events.h"
#include "imx6ul_regs_base.h"
#include "imx6ul_gpio_pin.h"
#include "imx6ul_pmu_regs.h"
#include "aw_clk.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"


/******************************************************************************/
#include "awbl_hwconf_imx6ul_gpio.h"
#include "awbl_hwconf_imx6ul_intc.h"
#include "awbl_hwconf_imx6ul_apbhdma.h"
#include "awbl_hwconf_imx6ul_epit0.h"
#include "awbl_hwconf_gpio_led.h"

#include "awbl_hwconf_imx6ul_gptimer1.h"
#include "awbl_hwconf_imx6ul_gptimer2.h"

#include "awbl_hwconf_imx6ul_i2c1.h"
#include "awbl_hwconf_imx6ul_i2c2.h"
#include "awbl_hwconf_imx6ul_i2c3.h"
#include "awbl_hwconf_imx6ul_i2c4.h"

#include "awbl_hwconf_imx6ul_ocotp.h"
#include "awbl_hwconf_imx6ul_sdma.h"
#include "awbl_hwconf_imx6ul_uart1.h"
#include "awbl_hwconf_imx6ul_uart2.h"
#include "awbl_hwconf_imx6ul_uart3.h"
#include "awbl_hwconf_imx6ul_uart4.h"
#include "awbl_hwconf_imx6ul_uart5.h"
#include "awbl_hwconf_imx6ul_uart6.h"
#include "awbl_hwconf_imx6ul_uart7.h"
#include "awbl_hwconf_imx6ul_uart8.h"

#include "awbl_hwconf_imx6ul_adc1.h"

#include "awbl_hwconf_imx6ul_flexcan1.h"
#include "awbl_hwconf_imx6ul_flexcan2.h"

#include "awbl_hwconf_imx6ul_ts.h"
#include "awbl_hwconf_hw480272f.h"
#include "awbl_hwconf_fb.h"

#include "awbl_hwconf_imx6ul_tempmon.h"

#include "awbl_hwconf_imx6ul_pwm.h"
#include "awbl_hwconf_buzzer_pwm.h"

#include "awbl_hwconf_imx6ul_ecspi1.h"
#include "awbl_hwconf_imx6ul_ecspi2.h"
#include "awbl_hwconf_imx6ul_ecspi3.h"
#include "awbl_hwconf_imx6ul_ecspi4.h"

#include "awbl_hwconf_imx6ul_eth1.h"
#include "awbl_hwconf_imx6ul_eth0.h"

#include "awbl_hwconf_generic_phy0.h"
#include "awbl_hwconf_generic_phy1.h"

#include "awbl_hwconf_dp83848_phy0.h"
#include "awbl_hwconf_dp83848_phy1.h"

#include "awbl_hwconf_imx6ul_sdio_sd0.h"
#include "awbl_hwconf_imx6ul_sdio_sd1.h"

#include "awbl_hwconf_ep24cxx_0.h"

#include "awbl_hwconf_gpio_wdt.h"
#include "awbl_hwconf_imx6ul_wdt1.h"
#include "awbl_hwconf_imx6ul_wdt3.h"

#include "awbl_hwconf_imx6ul_gpmi.h"

#include "awbl_hwconf_imx6ul_nandflash.h"

#include "awbl_hwconf_imx6ul_usbd0.h"
#include "awbl_hwconf_imx6ul_usbh1.h"
#include "awbl_hwconf_imx6ul_usbh2.h"

#include "awbl_hwconf_imx6ul_sai1.h"
#include "awbl_hwconf_imx6ul_mqs.h"

#include "awbl_hwconf_imx6ul_sim1_port0.h"
#include "awbl_hwconf_imx6ul_sim1_port1.h"
#include "awbl_hwconf_imx6ul_sim2_port0.h"
#include "awbl_hwconf_imx6ul_sim2_port1.h"

#include "awbl_hwconf_gpio_i2c0.h"
#include "awbl_hwconf_pcf85063_0.h"

#include "awbl_hwconf_imx6ul_qspi1.h"
#include "awbl_hwconf_spi_flash0.h"
#include "awbl_hwconf_spi_flash1.h"
#include "awbl_hwconf_imx6ul_caam.h"

#include "awbl_hwconf_gt911.h"
#include "awbl_hwconf_hw800480f.h"


//#include "awbl_hwconf_zlg600a_uart0.h"
//#include "awbl_hwconf_zlg600a_i2c0.h"

#include "awbl_hwconf_touch_calibrate_nvram.h"

#include "awbl_hwconf_gpio_spi0.h"

#include "awbl_hwconf_gpio_key.h"

#include "awbl_hwconf_imx6ul_csi.h"
#include "awbl_hwconf_ov7725.h"
#include "awbl_hwconf_gc0308.h"
#include "awbl_hwconf_adv7280.h"

#include "awbl_hwconf_imx6ul_eim.h"
#include "awbl_hwconf_imx6ul_sja1000.h"

#include "awbl_hwconf_ublox.h"
#include "awbl_hwconf_gnss_gprs.h"
#include "awbl_hwconf_fm175xx_spi.h"

#include "awbl_hwconf_imx6ul_timestamp.h"
#include "awbl_hwconf_cyw43362_wifi_sdio.h"

#include "awbl_hwconf_gprs_me909s.h"
#include "awbl_hwconf_gprs_sim800.h"
#include "awbl_hwconf_gprs_u9300c.h"
#include "awbl_hwconf_gprs_ec20.h"
#include "awbl_hwconf_gprs_ec200t_cn.h"
#include "awbl_hwconf_gprs_l610.h"
#include "awbl_hwconf_gprs_gm510.h"
#include "awbl_hwconf_gprs_n58.h"

/*******************************************************************************
  globals
*******************************************************************************/

/* 硬件设备列表 */
aw_const struct awbl_devhcf g_awbl_devhcf_list[] = {
    AWBL_HWCONF_IMX6UL_INTC
    AWBL_HWCONF_IMX6UL_GPIO
    AWBL_HWCONF_IMX6UL_SDMA

    AWBL_HWCONF_IMX6UL_APBHDMA


    AWBL_HWCONF_IMX6UL_I2C1     /* I2C1 */
    AWBL_HWCONF_IMX6UL_I2C2     /* I2C2 */
    AWBL_HWCONF_IMX6UL_I2C3     /* I2C3 */
    AWBL_HWCONF_IMX6UL_I2C4     /* I2C4 */

    AWBL_HWCONF_IMX6UL_OCOTP
    AWBL_HWCONF_IMX6UL_UART1    /* UART */
    AWBL_HWCONF_IMX6UL_UART2
    AWBL_HWCONF_IMX6UL_UART3
    AWBL_HWCONF_IMX6UL_UART4
    AWBL_HWCONF_IMX6UL_UART5
    AWBL_HWCONF_IMX6UL_UART6
    AWBL_HWCONF_IMX6UL_UART7
    AWBL_HWCONF_IMX6UL_UART8

   //AWBL_HWCONF_SDIO_SPI0
    AWBL_HWCONF_GPIO_SPI0

    AWBL_HWCONF_IMX6UL_SDIO_SSP0
    AWBL_HWCONF_IMX6UL_SDIO_SSP1
    AWBL_HWCONF_IMX6UL_GPMI
    AWBL_HWCONF_IMX6UL_NANDFLASH

    AWBL_HWCONF_IMX6UL_TIMER0   /* TIMER0 */

    AWBL_HWCONF_IMX6UL_GPTIMER1 /* GPTIMER1 */
    AWBL_HWCONF_IMX6UL_GPTIMER2 /* GPTIMER2 */

    AWBL_HWCONF_IMX6UL_ADC1     /* ADC1 */

    AWBL_HWCONF_IMX6UL_CAN1     /* CAN1 */
    AWBL_HWCONF_IMX6UL_CAN2     /* CAN2 */

    AWBL_HWCONF_IMX6UL_USBH1
    AWBL_HWCONF_IMX6UL_USBH2
    AWBL_HWCONF_IMX6UL_USBD0

//    AWBL_HWCONF_BUZZER_DC     /* 直流蜂鸣器 */
    AWBL_HWCONF_GPIO_LED        /* LED */
//
    AWBL_HWCONF_IMX6UL_PWM      /* PWM */
    AWBL_HWCONF_BUZZER_PWM

    AWBL_HWCONF_IMX6UL_TS

    /* i2c电容触摸屏   add by CYX*/
    AWBL_HWCONF_GT911

    AWBL_HWCONF_HW480272F       /* HW480272F液晶屏 */
    AWBL_HWCONF_HW800480F       /* HW800480F液晶屏 */
    AWBL_HWCONF_FB              /* 帧缓冲设备 */

    AWBL_HWCONF_IMX6UL_TEMPMON  /* tempmon */

    AWBL_HWCONF_IMX6UL_ECSPI1   /* ECSPI1 */
    AWBL_HWCONF_IMX6UL_ECSPI2   /* ECSPI2 */
    AWBL_HWCONF_IMX6UL_ECSPI3   /* ECSPI3 */
    AWBL_HWCONF_IMX6UL_ECSPI4   /* ECSPI4 */

    AWBL_HWCONF_IMX6UL_ETH0
    AWBL_HWCONF_IMX6UL_ETH1

    AWBL_HWCONF_GENERIC_PHY0
    AWBL_HWCONF_GENERIC_PHY1

    AWBL_HWCONF_DP83848_PHY0
    AWBL_HWCONF_DP83848_PHY1

    AWBL_HWCONF_EP24CXX_0       /* EP24CXX */

    AWBL_HWCONF_GPIO_WDT

    AWBL_HWCONF_IMX6UL_WDT1
    AWBL_HWCONF_IMX6UL_WDT3

    AWBL_HWCONF_IMX6UL_SAI1
    AWBL_HWCONF_IMX6UL_MQS

   //AWBL_HWCONF_PANEL_PM1000_BOARD
   //AWBL_HWCONF_TINY_HMI_BOARD
   //AWBL_HWCONF_APL_D01_BOARD

   //AWBL_HWCONF_GPIO_KEY         /* GPIO按键 */
   //AWBL_HWCONF_GPIO_KNOB        /* GPIO旋钮 */

    AWBL_HWCONF_IMX6UL_SIM1_PORT0
    AWBL_HWCONF_IMX6UL_SIM1_PORT1
    AWBL_HWCONF_IMX6UL_SIM2_PORT0
    AWBL_HWCONF_IMX6UL_SIM2_PORT1

    AWBL_HWCONF_GPIO_I2C0
    AWBL_HWCONF_PCF85063_0

    AWBL_HWCONF_SPI_FLASH0
    AWBL_HWCONF_SPI_FLASH1
    AWBL_HWCONF_IMX6UL_QSPI1
    AWBL_HWCONF_IMX6UL_CAAM

    //AWBL_HWCONF_ZLG600A_UART_0
    //AWBL_HWCONF_ZLG600A_I2C_0

    AWBL_HWCONF_TOUCH_CALIBRATE_NVRAM

    AWBL_HWCONF_GPIO_KEY

    AWBL_HWCONF_IMX6UL_CSI
    AWBL_HWCONF_ADV7280
    AWBL_HWCONF_OV7725
    AWBL_HWCONF_GC0308

    AWBL_HWCONF_GPRS_ME909S
    AWBL_HWCONF_GPRS_SIM800
    AWBL_HWCONF_GPRS_U9300C
    AWBL_HWCONF_GPRS_EC20
    AWBL_HWCONF_GPRS_EC200T
    AWBL_HWCONF_GPRS_GM510
    AWBL_HWCONF_GPRS_N58
    AWBL_HWCONF_GPRS_L610

    AWBL_HWCONF_UBLOX
    AWBL_HWCONF_GNSS_GPRS

    AWBL_HWCONF_IMX6UL_EIM
    AWBL_HWCONF_IMX6UL_SJA1000
    
    AWBL_HWCONF_FM175XX_0       /* Mifare卡 */

    AWBL_HWCONF_IMX6UL_TIMESTAMP
    AWBL_HWCONF_CYW43362_WIFI_SDIO

};

/* 硬件设备列表成员个数 */
aw_const size_t g_awbl_devhcf_list_count = AW_NELEMENTS(g_awbl_devhcf_list);

/* end of file */
