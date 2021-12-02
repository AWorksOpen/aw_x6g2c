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
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AnyWhere ���̲����Զ������ļ�
 */

#ifndef __AW_PRJ_PARAMS_AUTO_CFG_H
#define __AW_PRJ_PARAMS_AUTO_CFG_H



/* HW480272FҺ�����豸���� frame buffer���������
 * add by CYX at 2019.1.9
 */
#if defined  AW_DEV_HW480272F || \
    defined  AW_DEV_HW800480F
#define AW_DRV_TFT_PANEL
#define AW_DRV_FB
#define AW_DEV_FB
#ifndef AW_DEV_IMX6UL_PWM
#define AW_DEV_IMX6UL_PWM
#endif
#endif

/**
 * \name �ⲿFM175XX mifare����ģ��
 * @{
 */
#ifdef AW_DEV_FM175XX
#define AW_DRV_FM175XX

#ifndef AW_DEV_IMX6UL_ECSPI_1
#define AW_DEV_IMX6UL_ECSPI_1
#endif
#endif
/** @} */

/**
 * \name APL_D01ģ��
 * @{
 */
#ifdef AW_DEV_APL_D01
#define AW_DRV_APL_D01
#endif
/** @} */

/**
 * \name TinyHMIģ��
 * @{
 */
#ifdef AW_DEV_TINY_HMI
#define AW_DRV_TINY_HMI
#endif
/** @} */

/**
 * \name PM1000ǰ���
 * @{
 */
#ifdef AW_DEV_PANEL_PM1000
#define AW_DRV_PANEL_PM1000
#endif
/** @} */

/*
 * \name gps�豸
 * @{
 */
#ifdef AW_DEV_GNSS_UBLOX
#define AW_DRV_UBLOX
#ifndef AW_DEV_IMX6UL_UART7
#define AW_DEV_IMX6UL_UART7
#endif
#endif

#ifdef AW_DEV_GNSS_GPRS
#define AW_DRV_GNSS_GPRS
#endif /* AW_DEV_GNSS_GPRS */

#if defined(AW_DEV_GNSS_UBLOX) || defined(AW_DEV_GNSS_GPRS)
#define AW_COM_GNSS
#endif


/* SJA1000���� */
#ifdef AW_DEV_SJA1000
#define AW_DRV_SJA1000

#ifndef AW_DEV_IMX6UL_EIM
#define AW_DEV_IMX6UL_EIM
#endif
#endif

/* EIM����������EIM���� */
#ifdef AW_DEV_IMX6UL_EIM
#define AW_DRV_IMX6UL_EIM
#ifndef AW_COM_EIMBUS
#define AW_COM_EIMBUS
#endif
#endif

/**
 * \name ��ΪME909S
 * @{
 */
#ifdef AW_DEV_GPRS_ME909S
#define AW_DRV_GPRS_ME909S
#endif
/** @} */

/**
 * \name 
 * @{
 */
#ifdef AW_DEV_GPRS_EC20
#define AW_DRV_GPRS_EC20
#endif
/** @} */



/**
 * \name SIM800 GPRS
 * @{
 */
#ifdef AW_DEV_GPRS_SIM800
#define AW_DRV_GPRS_SIM800
#endif
/** @} */




/**
 * \name ����U9300
 * @{
 */
#ifdef AW_DEV_GPRS_U9300C
#define AW_DRV_GPRS_U9300C
#endif
/** @} */

/**
 * \name GPRS�豸
 * @{
 */
#if defined(AW_DEV_GPRS_SIM800) || \
    defined(AW_DEV_GPRS_ME909S) || \
    defined(AW_DEV_GPRS_U9300C) || \
    defined(AW_DEV_GPRS_EC20)
#ifndef AW_COM_NETWORK
#define AW_COM_NETWORK
#endif
#define AW_COM_GPRS
#endif

#if defined(AW_DEV_GPRS_ME909S) || \
    defined(AW_DEV_GPRS_U9300C) || \
    defined(AW_DEV_GPRS_EC20)
#define AW_DRV_USBH_WIRELESS                   /**< \brief �ƶ�������������(anydata����Ϊ�����е�ϵ��3G/4Gģ��) */
#endif
/** @} */

/* OV7725�豸����CSI���� */
#ifdef AW_DEV_OV7725
#define AW_DRV_OV7725
#ifndef AW_DEV_CAMERA
#define AW_DEV_CAMERA
#endif
#ifndef AW_DEV_IMX6UL_CSI
#define AW_DEV_IMX6UL_CSI
#endif
#endif

/* GC0308�豸����CSI���� */
#ifdef AW_DEV_GC0308
#define AW_DRV_GC0308
#ifndef AW_DEV_CAMERA
#define AW_DEV_CAMERA
#endif
#ifndef AW_DEV_IMX6UL_CSI
#define AW_DEV_IMX6UL_CSI
#endif
#endif

/* ADV7280�豸����CSI���� */
#ifdef AW_DEV_ADV7280
#define AW_DRV_ADV7280
#ifndef AW_DEV_CAMERA
#define AW_DEV_CAMERA
#endif
#ifndef AW_DEV_IMX6UL_CSI
#define AW_DEV_IMX6UL_CSI
#endif
#endif


/* CSI�������豸����CSI���� */
#ifdef AW_DEV_IMX6UL_CSI
#define AW_DRV_IMX6UL_CSI
#endif

/* name �������� */
#ifdef AW_DEV_GPIO_KEY
#define AW_DRV_GPIO_KEY
#ifndef AW_COM_EVENT
#define AW_COM_EVENT
#endif
#ifndef AW_COM_INPUT_EV
#define AW_COM_INPUT_EV
#endif
#ifndef AW_COM_INPUT_EV_KEY
#define AW_COM_INPUT_EV_KEY
#endif
#endif

/* PCF85063ʵʱʱ���豸����RTC�����GPIO I2C�豸 */
#ifdef AW_DEV_EXTEND_PCF85063_0
#define AW_DRV_EXTEND_PCF85063_0
#ifndef AW_DEV_GPIO_I2C_0
#define AW_DEV_GPIO_I2C_0
#endif
#define AW_COM_RTC
#endif

/* I2C����������I2C�����I2C���� */
#if defined(AW_DEV_IMX6UL_I2C_1) ||\
    defined(AW_DEV_IMX6UL_I2C_2) ||\
    defined(AW_DEV_IMX6UL_I2C_3) ||\
    defined(AW_DEV_IMX6UL_I2C_4)

#define  AW_DRV_IMX6UL_I2C
#ifndef AW_COM_I2CBUS
#define AW_COM_I2CBUS
#endif
#endif

/* GPIO I2C������I2C�����GPIO I2C���� */
#ifdef AW_DEV_GPIO_I2C_0
#define AW_DRV_GPIO_I2C
#ifndef AW_COM_I2CBUS
#define AW_COM_I2CBUS
#endif
#endif

/* OCOTP����������OCOTP���� */
#ifdef AW_DEV_IMX6UL_OCOTP
#define AW_DRV_IMX6UL_OCOTP
#endif

/* ����̨������� UARTx */
#ifdef AW_COM_CONSOLE
#if AW_CFG_CONSOLE_COMID == COM0
#ifndef AW_DEV_IMX6UL_UART1
#define AW_DEV_IMX6UL_UART1
#endif
#endif
#endif

/* shell�����ڴ���1�豸 */
#ifdef AW_COM_SHELL_SERIAL
#ifndef AW_DEV_IMX6UL_UART1
#define AW_DEV_IMX6UL_UART1
#endif
#endif

/* ZIGBEE�豸����ZIGBEE������ZIGBEE��� */
#ifdef AW_DEV_ZIGBEE
#define AW_DEV_IMX6UL_UART2
#define AW_COM_ZIGBEE
#define AW_DRV_ZIGBEE
#endif

/* UART����������SERIAL�����UART����*/
#if defined (AW_DEV_IMX6UL_UART1) ||\
    defined (AW_DEV_IMX6UL_UART2) ||\
    defined (AW_DEV_IMX6UL_UART3) ||\
    defined (AW_DEV_IMX6UL_UART4) ||\
    defined (AW_DEV_IMX6UL_UART5) ||\
    defined (AW_DEV_IMX6UL_UART6) ||\
    defined (AW_DEV_IMX6UL_UART7) ||\
    defined (AW_DEV_IMX6UL_UART8)

#define AW_DRV_IMX6UL_UART
#ifndef AW_COM_SERIAL
#define AW_COM_SERIAL
#endif
#endif

/* APBHDMA������APBHDMA���� */
#ifdef AW_DEV_IMX6UL_APBHDMA
#define AW_DRV_IMX6UL_APBHDMA
#endif

/* SDMA������SDMA���� */
#ifdef AW_DEV_IMX6UL_SDMA
#define AW_DRV_IMX6UL_SDMA
#endif

/* SAI1������SAI���� */
#ifdef AW_DEV_IMX6UL_SAI1
#define AW_DRV_IMX6UL_SAI
#endif

/* MQS������MQS���� */
#ifdef AW_DEV_IMX6UL_MQS
#if !defined (AW_DEV_IMX6UL_SAI1)
#error "AW_DEV_IMX6UL_MQS relay on sai1"
#endif
#define AW_DRV_IMX6UL_MQS
#endif

/* GPTIMER������GPTIMER���� */
#if defined (AW_DEV_IMX6UL_GPTIMER_1) ||\
    defined (AW_DEV_IMX6UL_GPTIMER_2)

#define AW_DRV_IMX6UL_GPTIMER
#endif

#ifdef AW_DEV_IMX6UL_TIMESTAMP
#define AW_DRV_IMX6UL_TIMESTAMP
#endif

/* DC_BUZZER ���� DC_BUZZER���� */
#ifdef AW_DEV_DC_BUZZER
#define AW_DRV_DC_BUZZER
#endif

/* PWM_BUZZER ���� PWM���� */
#ifdef AW_DEV_PWM_BUZZER
#define AW_DRV_PWM_BUZZER
#define AW_DEV_IMX6UL_PWM             /**< \brief IMX6UL PWM */
#endif

/* PWM����������PWM�����PWM���� */
#ifdef AW_DEV_IMX6UL_PWM
#define AW_DRV_IMX6UL_PWM
#define AW_COM_PWM
#endif


/* EP24CXX������EEPROMͨ��������I2C������� */
#ifdef AW_DEV_EXTEND_EP24CXX_0
#define AW_DRV_EXTEND_EP24CXX
#ifndef AW_COM_I2CBUS
#define AW_COM_I2CBUS
#endif
#endif

/* CAAM������CAAM���� */
#ifdef AW_DEV_IMX6UL_CAAM
#define AW_DRV_IMX6UL_CAAM
#endif


/* GPIO WDT������GPIO WDT������WDT��� */
#ifdef AW_DEV_GPIO_WDT
#define AW_DRV_GPIO_WDT
#ifndef AW_COM_WDT
#define AW_COM_WDT
#endif
#endif


/* WDT����������WDT������WDT��� */
#if defined(AW_DEV_IMX6UL_WDT1) || \
    defined(AW_DEV_IMX6UL_WDT2) || \
    defined(AW_DEV_IMX6UL_WDT3)
#define AW_DRV_IMX6UL_WDT
#ifndef AW_COM_WDT
#define AW_COM_WDT
#endif
#endif

/* �������豸����������������ADC�豸 */
#ifdef AW_DEV_IMX6UL_TS
#define AW_DRV_IMX6UL_TS
#ifndef AW_DEV_IMX6UL_ADC1
#define AW_DEV_IMX6UL_ADC1
#endif
#define AW_DEV_IMX6UL_TOUCH_CALIBRATE_NVRAM
#endif

/**
 * \name �ⲿ���ݴ�����(I2C�ӿ�)
 * \brief add by CYX
 * @{
 */
#ifdef  AW_DEV_GT911
#define AW_DRV_GT911_TS
#define AW_DEV_GPIO_I2C_0
#endif
/** @} */


/* ADC����������ADC���� */
#ifdef AW_DEV_IMX6UL_ADC1
#define AW_DRV_IMX6UL_ADC
#define AW_COM_ADC
#endif

/* ECSPI����������ECSPI������SPIBUS��� */
#if defined (AW_DEV_IMX6UL_ECSPI_1) || \
    defined (AW_DEV_IMX6UL_ECSPI_2) || \
    defined (AW_DEV_IMX6UL_ECSPI_3) || \
    defined (AW_DEV_IMX6UL_ECSPI_4)
#define AW_DRV_IMX6UL_ECSPI
#ifndef AW_COM_SPIBUS
#define AW_COM_SPIBUS
#endif
#endif

/* GPIO SPI����������GPIO SPI������SPIBUS��� */
#ifdef AW_DEV_GPIO_SPI
#define AW_DRV_GPIO_SPI
#ifndef AW_COM_SPIBUS
#define AW_COM_SPIBUS
#endif
#endif

/* QSPI����������QSPI������SPIBUS��� */
#if defined(AW_DEV_IMX6UL_QSPI_1)
#define AW_DRV_IMX6UL_QSPI
#ifndef AW_COM_SPIBUS
#define AW_COM_SPIBUS
#endif
#endif


/* �¶��豸�����¶�������������� */
#ifdef AW_DEV_IMX6UL_TEMPMON
#define AW_COM_TEMPMON
#define AW_DRV_IMX6UL_TEMPMON
#endif

/* ENET */
#if defined( AW_DEV_IMX6UL_ENET0) || defined(AW_DEV_IMX6UL_ENET1)
#define AW_DRV_IMX6UL_ENET
#ifndef AW_COM_NETWORK          /* ʹ������Э��ջ */
#define AW_COM_NETWORK
#endif
//#define AW_DEV_GENERIC_PHY0   /* ͨ�� PHY �豸0 */
//#define AW_DEV_GENERIC_PHY1   /* ͨ�� PHY �豸1 */
#ifdef AW_DEV_IMX6UL_ENET0
#define AW_DEV_DP83848_PHY0     /* DP83848 PHY �豸0 */
#endif
#ifdef AW_DEV_IMX6UL_ENET1
#define AW_DEV_DP83848_PHY1     /* DP83848 PHY �豸0 */
#endif
#endif


#if defined(AW_DEV_GENERIC_PHY0) || defined(AW_DEV_GENERIC_PHY1)
#define AW_DRV_GENERIC_PHY
#define AW_COM_MIIBUS
#endif


#if defined(AW_DEV_DP83848_PHY0) || defined(AW_DEV_DP83848_PHY1)
#define AW_DRV_DP83848_PHY
#define AW_COM_MIIBUS
#endif

/* GPIO LED����GPIO���GPIO LEDͨ������ */
#ifdef AW_DEV_GPIO_LED
#define AW_COM_LED
#define AW_DRV_GPIO_LED
#endif

/* SIM��������SIM������SIM��� */
#if defined(AW_DEV_IMX6UL_SIM1_PORT0) || \
    defined(AW_DEV_IMX6UL_SIM1_PORT1) || \
    defined(AW_DEV_IMX6UL_SIM2_PORT0) || \
    defined(AW_DEV_IMX6UL_SIM2_PORT1)
#define AW_DRV_IMX6UL_SIM
#define AW_COM_SIM
#endif

#if 0
/* HW480272FҺ�����豸���� frame buffer��������� */
#ifdef AW_DEV_HW480272F
#define AW_DRV_MXS_HW480272F
#define AW_DRV_FB
#define AW_DEV_FB
#endif
#endif


/* SDCARD��������SDCARD���� �Ϳ��豸��� , SDIO�豸*/
#ifdef AW_DEV_SDCARD
#define AW_DRV_SDCARD
#ifndef AW_DEV_IMX6UL_USDHC1
#define AW_DEV_IMX6UL_USDHC1
#endif
#ifndef AW_COM_BLOCK_DEV
#define AW_COM_BLOCK_DEV
#endif
#endif

/**
 * \name SDIO����
 * @{
 */
#if defined(AW_DEV_IMX6UL_USDHC1) ||\
    defined(AW_DEV_IMX6UL_USDHC2)
#ifndef AW_COM_SDIOBUS
#define AW_COM_SDIOBUS
#endif

#define AW_DRV_IMX6UL_USDHC
#endif
/** @} */

/* SPI_FLASH����nvram�����SPI FLASH���� */
#if defined(AW_DEV_SPI_FLASH0) ||\
    defined(AW_DEV_SPI_FLASH1)
#define AW_DRV_SPI_FLASH

#define AW_CFG_SPI_FLASH_MTD
#define AW_CFG_SPI_FLASH_NVRAM
#endif

/* MTD��������MTD��� */
#ifdef AW_CFG_SPI_FLASH_MTD
#ifndef AW_COM_MTD_DEV
#define AW_COM_MTD_DEV
#endif
#endif

/* CAN����������CAN���� */
#if defined(AW_DEV_IMX6UL_CAN1) || defined(AW_DEV_IMX6UL_CAN2)
#define AW_DRV_IMX6UL_CAN
#endif

/* CANopen��������CAN���� */
#ifdef AW_COM_CANOPEN
#ifndef AW_DRV_IMX6UL_CAN
#error "AW_COM_CANOPEN can not work without AW_DRV_IMX6UL_CAN"
#endif
#endif

/* USB Host ������USB host���� */
#if defined(AW_DEV_IMX6UL_USB_HOST1) ||\
    defined(AW_DEV_IMX6UL_USB_HOST2)
#define AW_DRV_IMX6UL_USB_HOST
#ifndef AW_COM_USBH
#define AW_COM_USBH
#endif
#endif

/* USB Device ������USB Device���� */
#ifdef AW_DEV_IMX6UL_USB_DEVICE0
#define AW_DRV_IMX6UL_USB_DEVICE
#ifndef AW_COM_USBD
#define AW_COM_USBD
#endif
#ifndef AW_COM_FS_FATFS
#define AW_COM_FS_FATFS
#endif
#endif

/* ZLG600A I2C�豸����USZLG600A I2C���� */
#ifdef AW_DEV_ZLG600A_I2C_0
#define AW_DRV_ZLG600A_I2C
#endif

/* ZLG600A UART�豸����USZLG600A UART���� */
#ifdef AW_DEV_ZLG600A_UART_0
#define AW_DRV_ZLG600A_UART
#endif

/* NAND FLASH�豸����NAND BUS��NAND���� */
#if defined(AW_DEV_NAND_FLASH_S34ML01G2) ||\
    defined(AW_DEV_NAND_FLASH_S34ML02G2)
#define AW_DRV_NANDBUS
#define AW_DRV_IMX6UL_NANDFLASH
#define AW_DEV_IMX6UL_GPMI
#ifndef AW_COM_MTD_DEV
#define AW_COM_MTD_DEV
#endif
#endif

/* GPMI����������GPMI���� */
#ifdef AW_DEV_IMX6UL_GPMI
#define AW_DRV_IMX6UL_GPMI
#endif

/**
 * \name 16550 UART
 * @{
 */
#ifdef AW_DEV_NS16550_PLB
#define AW_DRV_NS16550_PLB
#endif
/** @} */

/* 16550 PLB �������������ڴ��ж˿�ͨ�ÿ� */
#ifdef AW_DRV_NS16550_PLB
#ifndef AW_COM_SERIAL
#define AW_COM_SERIAL
#endif
#endif


/******************************************************************************/

/**
 * \name �ļ�ϵͳ������IO��ϵͳ
 * @{
 */
#if defined AW_COM_FS_RAWFS  || \
    defined AW_COM_FS_FATFS  || \
    defined AW_COM_FS_LFFS   || \
    defined AW_COM_FS_TXFS   || \
    defined AW_COM_FS_DUMPFS || \
    defined AW_COM_FS_SAFEFS || \
    defined AW_COM_FS_YAFFS  || \
    defined AW_COM_FS_ROMFS

#ifndef AW_COM_IO_SYSTEM
#define AW_COM_IO_SYSTEM
#endif

#ifndef AW_COM_MOUNT
#define AW_COM_MOUNT
#endif

#ifndef AW_COM_FS_ROOTFS
#define AW_COM_FS_ROOTFS
#endif
#endif
/** @} */

/**
 * \name �����ļ�ϵͳ�������豸
 * @{
 */
#if defined AW_COM_FS_RAWFS  || \
    defined AW_COM_FS_FATFS  || \
    defined AW_COM_FS_TXFS   || \
    defined AW_COM_FS_ROMFS

#ifndef AW_COM_BLOCK_DEV
#define AW_COM_BLOCK_DEV
#endif
#endif
/** @} */

/**
 * \name �����ļ�ϵͳ����mtd�豸
 * @{
 */
#if defined AW_COM_FS_LFFS || \
    defined AW_COM_FS_SAFEFS

#ifndef AW_COM_MTD_DEV
#define AW_COM_MTD_DEV
#endif
#endif
/** @} */

/**
 * \name �����ļ�ϵͳ�����¼�
 * @{
 */
#if defined AW_COM_FS_FATFS
#ifndef AW_COM_EVENT
#define AW_COM_EVENT
#endif
#endif
/** @} */

/* USDHC and SDIO_SPI depend on SIDOBUS */
#if defined(AW_DEV_IMX6UL_USDHC1) || \
    defined(AW_DRV_SDIO_SPI)

#ifndef AW_COM_SDIOBUS
#define AW_COM_SDIOBUS
#endif

#endif


/* add by CYX*/
#ifdef AW_DEV_CYW43362_WIFI_SDIO

    #ifndef AW_COM_NETWORK
        #define AW_COM_NETWORK
    #endif

    #define AW_DRV_AWBL_CYW43362_WIFI_SDIO

    #ifndef AW_DEV_IMX6UL_USDHC2
    #define AW_DEV_IMX6UL_USDHC2
    #define AW_DRV_IMX6UL_USDHC2
    #define AW_COM_AWBL_SDIOBUS
    #endif
#endif


/*******************************************************************************
  ��ͻ���
*******************************************************************************/
/** \cond */

#if defined(AW_DEV_IMX6UL_USB_HOST1) && defined(AW_DEV_IMX6UL_USB_DEVICE0)
#error "AW_DEV_IMX6UL_USB_HOST1 and AW_DEV_IMX6UL_USB_DEVICE0  cannot defined at the same time!"
#endif


#if defined(AW_DEV_IMX6UL_ECSPI_1) && defined(AW_DEV_IMX6UL_SIM2_PORT1)
#error "AW_DEV_IMX6UL_ECSPI_1 and AW_DEV_IMX6UL_SIM2_PORT1 cannot defined at the same time!"
#endif


#if defined(AW_DEV_IMX6UL_GPMI) && defined(AW_DEV_IMX6UL_QSPI_1)
#error "AW_DEV_IMX6UL_GPMI and AW_DEV_IMX6UL_QSPI_1 cannot defined at the same time!"
#endif


#if defined(AW_DEV_IMX6UL_I2C_1) && defined(AW_DEV_IMX6UL_UART4)
#error "AW_DEV_IMX6UL_I2C_1 and AW_DEV_IMX6UL_UART4 cannot defined at the same time!"
#endif

#if defined(AW_DEV_ZLG600A_UART_0) && defined(AW_DEV_ZLG600A_I2C_0)
#error "AW_DEV_ZLG600A_UART_0 and AW_DEV_ZLG600A_I2C_0 cannot defined at the same time!"
#endif

/* ��OV7725��ͻ������ */
#if (defined(AW_DEV_OV7725) || defined(AW_DEV_ADV7280) || defined(AW_DEV_GC0308)) && defined(AW_DEV_ZIGBEE)
#error "AW_DEV_OV7725 or AW_DEV_ADV7280 or AW_DEV_GC0308 configuration conflict!"
#endif

/* ��CSI��������ͻ������ */
#if defined(AW_DEV_IMX6UL_CSI) && (defined(AW_DEV_IMX6UL_UART6) || defined(AW_DEV_GPIO_SPI) || defined(AW_DEV_IMX6UL_EIM) \
    || defined(AW_DEV_IMX6UL_ECSPI_1) || defined(AW_DEV_SPI_FLASH1) || defined(AW_DEV_ZIGBEE) || defined(AW_DEV_IMX6UL_SIM1_PORT1) \
    || defined(AW_DEV_IMX6UL_SIM2_PORT1))
#error "AW_DEV_IMX6UL_CSI  configuration conflict!"
#endif

#if defined(AW_DEV_IMX6UL_UART2) && defined(AW_DEV_IMX6UL_ECSPI_3) || \
    defined(AW_DEV_IMX6UL_UART2) && defined(AW_DEV_IMX6UL_CAN2) || \
    defined(AW_DEV_IMX6UL_ECSPI_3) && defined(AW_DEV_IMX6UL_CAN2)
#error "Two of the AW_DEV_IMX6UL_UART2, AW_DEV_IMX6UL_ECSPI_3 and AW_DEV_IMX6UL_CAN2 cannot defined at the same time!"
#endif

/* ��EIM��������ͻ������ */
#if defined(AW_DEV_IMX6UL_EIM) && ((defined(AW_DEV_IMX6UL_UART6) || defined(AW_DEV_IMX6UL_ECSPI_1) \
    || defined(AW_DEV_IMX6UL_SIM1_PORT1) || defined(AW_DEV_IMX6UL_CSI) ))
#error "AW_DEV_IMX6UL_EIM  configuration conflict!"
#endif

/* ���繤�߱�������������� */
#ifndef AW_COM_NETWORK
#ifdef AW_NET_TOOLS
#warning "AW_NET_TOOLS can not work without AW_COM_NETWORK"
#undef AW_NET_TOOLS
#endif
#endif

#endif  /* __AW_PRJ_PARAMS_AUTO_CFG_H */

/* end of file */
