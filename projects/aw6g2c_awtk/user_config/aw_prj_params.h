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
 * \brief AnyWhere ���̲��������ļ�
 */

#ifndef __AW_PRJ_PARAMS_H
#define __AW_PRJ_PARAMS_H

/**
 * \addtogroup grp_aw_prj_param
 * @{
 */

/*******************************************************************************
  Ӳ������
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_params_hwcfg   Ӳ������
 *
 * -# ע�͵�����궨�弴�ɽ��ܶ�Ӧ�����裬��֮����ʹ��;
 * -# ʹ��ĳ������󣬸��������������������ᱻ�Զ����ý�������������������
 *    �����������ġ�������á���
 * -# �жϿ�������GPIO������������裬Ĭ�ϱ�ʹ�ܡ�
 * @{
 */


#define AW_DEV_IMX6UL_I2C_1             /**< \brief IMX6UL I2C1 */
//#define AW_DEV_IMX6UL_I2C_2             /**< \brief IMX6UL I2C2 */
//#define AW_DEV_IMX6UL_I2C_3             /**< \brief IMX6UL I2C3 */
//#define AW_DEV_IMX6UL_I2C_4             /**< \brief IMX6UL I2C4 */
#define AW_DEV_GPIO_I2C_0               /**< \brief GPIO I2C0 */
#define AW_DEV_IMX6UL_UART1             /**< \brief IMX6UL UART1 */
//#define AW_DEV_IMX6UL_UART2             /**< \brief IMX6UL UART2 */
//#define AW_DEV_IMX6UL_UART3             /**< \brief IMX6UL UART3 */
//#define AW_DEV_IMX6UL_UART4             /**< \brief IMX6UL UART4 */
//#define AW_DEV_IMX6UL_UART5             /**< \brief IMX6UL UART5 */
//#define AW_DEV_IMX6UL_UART6             /**< \brief IMX6UL UART6 */
//#define AW_DEV_IMX6UL_UART7             /**< \brief IMX6UL UART7 */
//#define AW_DEV_IMX6UL_UART8             /**< \brief IMX6UL UART8 */
#define AW_DEV_IMX6UL_ECSPI_1           /**< \brief imx6ul ecspi1 */
//#define AW_DEV_IMX6UL_ECSPI_2           /**< \brief imx6ul ecspi2 */
//#define AW_DEV_IMX6UL_ECSPI_3           /**< \brief imx6ul ecspi3 */
//#define AW_DEV_IMX6UL_ECSPI_4           /**< \brief imx6ul ecspi4 */
//#define AW_DEV_GPIO_SPI                 /**< \brief GPIO SPI */
//#define AW_DEV_IMX6UL_QSPI_1            /**< \brief QSPI���� */
//#define AW_DEV_IMX6UL_WDT1              /**< \brief WDT1 */
//#define AW_DEV_IMX6UL_WDT2              /**< \brief WDT2 for secure world purposes (couldn't work in normal modes) */
//#define AW_DEV_IMX6UL_WDT3              /**< \brief WDT3 (do not work) */
//#define AW_DEV_GPIO_WDT                 /**< \brief GPIO WDT */
//#define AW_DEV_IMX6UL_SIM1_PORT0        /**< \brief SIM1 PORT0 */
//#define AW_DEV_IMX6UL_SIM1_PORT1        /**< \brief SIM1 PORT1 */
//#define AW_DEV_IMX6UL_SIM2_PORT0        /**< \brief SIM2 PORT0 */
//#define AW_DEV_IMX6UL_SIM2_PORT1        /**< \brief SIM2 PORT1 */
#define AW_DEV_IMX6UL_USB_HOST1         /**< \brief IMX6UL USB HOST1 */
#define AW_DEV_IMX6UL_USB_HOST2         /**< \brief IMX6UL USB HOST2 */
//#define AW_DEV_IMX6UL_USB_DEVICE0       /**< \brief IMX6UL USB DEVICE */
//#define AW_DEV_NAND_FLASH_S34ML01G2     /**< \brief NAND FLASH�豸 */
#define AW_DEV_NAND_FLASH_S34ML02G2     /**< \brief NAND FLASH�豸 */
#define AW_DEV_IMX6UL_GPTIMER_1         /**< \brief IMX6UL GPTIMER1 */
#define AW_DEV_IMX6UL_GPTIMER_2         /**< \brief IMX6UL GPTIMER2 */
#define AW_DEV_IMX6UL_TIMESTAMP         /**< \brief IMX6UL TIMESTAMP */
//#define AW_DEV_SPI_FLASH0               /**< \brief SPI FLASH0�豸���� */
//#define AW_DEV_SPI_FLASH1               /**< \brief SPI FLASH1�豸���� */
//#define AW_DEV_IMX6UL_CAN1              /**< \brief iMX6UL flex can1 */
//#define AW_DEV_IMX6UL_CAN2              /**< \brief iMX6UL flex can2 */

//#define AW_DEV_IMX6UL_CAAM              /**< \brief IMX6UL CAAM */
#define AW_DEV_IMX6UL_APBHDMA           /**< \brief IMX6UL APBH-DMA */
#define AW_DEV_IMX6UL_SDMA              /**< \brief imx6ul sdma */
#define AW_DEV_IMX6UL_OCOTP             /**< \brief IMX6UL OCOTP */
#define AW_DEV_IMX6UL_ENET0              /**< \brief IMX6UL ENET0 */
//#define AW_DEV_IMX6UL_ENET1              /**< \brief IMX6UL ENET1 */

#define AW_DEV_EXTEND_PCF85063_0        /**< \brief PCF85063 */

#define AW_DEV_SDCARD                   /**< \brief SD������ */

#define AW_DEV_IMX6UL_USDHC1          /**< \brief SDIO0���� */
//#define AW_DEV_IMX6UL_USDHC2            /**< \brief SDIO1���� */

#define AW_DEV_GPIO_LED                 /**< \brief GPIO LED */

//#define AW_DEV_PWM_BUZZER               /**< \brief PWM Buzzer(����������Ҫ����PWM) */
#define AW_DEV_IMX6UL_PWM               /**< \brief IMX6UL PWM */

//#define AW_DEV_HW480272F                /**< \brief HW480272F Һ���� */
#define AW_DEV_HW800480F                /**< \brief HW800480F Һ���� */
#define AW_DEV_IMX6UL_TS                /**< \brief MX6UL Touch Screen Controller */
//#define AW_DEV_IMX6UL_ADC1
//#define AW_DEV_GPIO_KEY                 /**< \brief ���� */

//#define AW_DEV_OV7725
//#define AW_DEV_IMX6UL_CSI

//#define AW_DEV_IMX6UL_SAI1              /**< \brief IMX6UL SAI1    */
//#define AW_DEV_IMX6UL_MQS               /**< \brief IMX6UL MQS     */
//#define AW_DEV_EXTEND_EP24CXX_0         /**< \brief ����EEPROM */
//#define AW_DEV_IMX6UL_TEMPMON           /**< \brief temperature monitor */
//#define AW_DEV_ZLG600A_I2C_0
//#define AW_DEV_ZLG600A_UART_0
//#define AW_DEV_ZIGBEE                   /**< \brief zigbee */
//#define AW_DEV_GT911                    /**< \brief ���ݴ�����оƬgt911  */

//#define AW_DEV_GPRS_SIM800
//#define AW_DEV_GPRS_ME909S
//#define AW_DEV_GPRS_U9300C
//#define AW_DEV_GPRS_EC20                /**< \brief GPRS EC20  */
//#define AW_DEV_GPS_EC20

//#define AW_DEV_SJA1000
//#define AW_DEV_IMX6UL_EIM

//#define AW_DEV_GNSS_UBLOX               /*gps�豸*/

//#define AW_DEV_CYW43362_WIFI_SDIO         /**< \brief WIFIģ��(��ҪSDHC2����) */

//#define AW_DEV_DC_BUZZER                /**< \brief DC Buzzer(������) */
//#define AW_DEV_PANEL_PM1000             /**< \brief PM1000ǰ��� */
//#define AW_DEV_TINY_HMI                 /**< \brief TinyHMIģ�� */
//#define AW_DEV_APL_D01                  /**< \brief APL_D01ģ�� */
//#define AW_DEV_NS16550_PLB              /**< \brief 16550 UART ��������(PLB)(�Զ�����) */
//#define AW_DEV_FM175XX                  /**< \brief Mifare������(��ҪSPI����) */
/** @} grp_aw_plfm_params_hwcfg */

/*******************************************************************************
  �������
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_params_sftcfg   �������
 *
 * -# ע�͵�����궨�弴�ɽ��ܶ�Ӧ���������֮����ʹ��;
 * -# ʹ��ĳ������󣬸��������������������ᱻ�Զ����ý���;
 * -# ��ĳ��Ӳ������������������������Ӳ������ʹ��ʱ��������Щ�����������
 *    �Ƿ�ʹ�ܣ��������Զ����ý�����
 * -# ע���д���(����)�������ģ���ʾ��������뱻ʹ�ܣ�ע���д���(�Զ�����)��������
 *    ��ʾ������ǿ��Ա��Զ����õģ�ͨ������Щ������ֽ��ܾͺã�
 *    �������������Լ�����Ҫ�������Ҫ�õ���ʹ�ܣ�δʹ�þͿ��Խ��ܡ�
 * @{
 */

#define AW_COM_CONSOLE                  /**< \brief ����̨��� */
#define AW_COM_SHELL_SERIAL             /**< \brief ʹ�ܴ���shell */
//#define AW_COM_SHELL_TELNET             /**< \brief ʹ��telentԶ������shell */
//#define AW_COM_FTPD                     /**< \brief ftp server ��� */
#define AW_COM_EVENT                    /**< \brief ʹ���¼�ϵͳ */
//#define AW_COM_TIMESTAMP                /**< \brief ʱ������� */

//#define AW_COM_DIGITRON_DISP            /**< \brief �������ʾͨ�ÿ� */
//#define AW_COM_LOG                      /**< \brief ϵͳ��־��� */
//#define AW_COM_FTL                      /**< \brief FLASH ����㣬����MTD�豸�ľ���ĥ�𡢻������� */

//#define AW_COM_CANOPEN          /**< \brief CANӦ�ø߲�Э��ջCANopen��� */

/**
 * \name �ļ�ϵͳ�ü�����
 * @{
 */
//#define AW_COM_FS_RAWFS               /**< \brief ʹ�� RAW �ļ�ϵͳ */
//#define AW_COM_FS_DUMPFS              /**< \brief ʹ�� dump �ļ�ϵͳ */
#define AW_COM_FS_FATFS               /**< \brief ʹ�� FAT �ļ�ϵͳ */
#define AW_COM_FS_LFFS                /**< \brief ʹ�� LFFS �ļ�ϵͳ */
//#define AW_COM_FS_SAFEFS              /**< \brief ʹ�ܰ�ȫ�ļ�ϵͳ */
//#define AW_COM_FS_TXFS                /**< \brief ʹ���������ļ�ϵͳ */
#define AW_COM_FS_YAFFS               /**< \brief ʹ�� YAFFS �ļ�ϵͳ */
//#define AW_COM_FS_ROMFS               /**< \brief ʹ�� ROMFS �ļ�ϵͳ */
/** @} */

/** @} grp_aw_plfm_params_sftcfg */

/**
 * \name ����Զ����úͼ��
 * @{
 */
#include "aw_prj_param_auto_cfg.h"
/** @} */

/*******************************************************************************
  ��Դ����
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_rescfg   ��Դ����
 * @{
 */

/**
 * \name ����̨�����Դ����
 * @{
 */
#ifdef AW_COM_CONSOLE
#define AW_CFG_CONSOLE_SHOW_BANNER      1                       /** \brief ����̨����ʱ���Ƿ���ʾ��ӭ��ʶ��0-����ʾ 1-��ʾ */
#define AW_CFG_CONSOLE_COMID            IMX6UL_UART1_COMID      /** \brief ����̨���ں� */
#define AW_CFG_CONSOLE_BAUD_RATE        115200                  /** \brief ����̨������ */
#endif
/** @} */

/**
 * \name shell�����Դ����
 * @{
 */
#ifdef AW_COM_SHELL_SERIAL
#define AW_CFG_SHELL_THREAD_PRIO        6u                      /**< \brief serial shell �߳����ȼ� */
#define AW_CFG_SHELL_THREAD_STK_SIZ     (10 * 1024)             /**< \brief serial shell �̶߳�ջ��С */
#define AW_CFG_SHELL_SERIAL_COMID       IMX6UL_UART1_COMID      /**< \brief serial shell ʹ�õĴ����豸�� */

#define AW_SHELL_IO                                             /**< \brief ʹ��shell IO��������:"ls" "cd" "mkdir"������ */
#define AW_NET_TOOLS                                            /**< \brief ʹ��shell NET��������:"ip" "ping"�� */
#define AW_SHELL_LUA                                            /**< \brief ʹ��shell lua��������:"lua" �� */
#define AW_SHELL_LUAC                                           /**< \brief ʹ��shell luac��������:"luac"�� */
#endif

#ifdef AW_COM_SHELL_TELNET
#define AW_CFG_TELNET_TASK_PRIO         1           /**< \brief telent shell �߳����ȼ�  */
#define AW_CFG_TELNET_TASK_STACK_SIZE   8192        /**< \brief telent shell �̶߳�ջ��С */
#define AW_CFG_TELNET_MAX_SESSION       2           /**< \brief telent shell ���ͬʱ������Ŀ */
#define AW_CFG_TELNET_LISTEN_PORT       23          /**< \brief telent shell ����˿ں� */
#define AW_CFG_TELNET_USER_NAME         "admin"     /**< \brief telent shell �û������� */
#define AW_CFG_TELNET_USER_PSW          "123456"    /**< \brief telent shell �û��������� */
#endif
/** @} */

/**
 * \name FTP �����Դ����
 * @{
 */

/** \brief FTP �����Ҫ�ļ�ϵͳ֧�֣������Ҫ���ļ�ϵͳ��������Ҫ�����ļ�ϵͳ��mount�ӿڣ�
 *         ����һ���ļ����ڵ�
 */
#ifdef AW_COM_FTPD                                  /**< \brief ʹ��ftp server */
#define FTPD_MAX_SESSIONS               1           /**< \brief FTPD ���ͬʱ������Ŀ */
#define FTPD_LISTEN_TASK_PRIO           6           /**< \brief FTPD �������ȼ� */
#define FTPD_LISTEN_TASK_STK_SIZE       4096        /**< \brief FTPD ջ��С */
#define FTPD_CMD_BUF_SIZE               0x10000     /**< \brief FTPD ����buffer */
#endif
/** @} */

/** \name �¼�ϵͳ��Դ����
 * @{
 */
#ifdef AW_COM_EVENT
#define AW_CFG_EVENT_TASK_STK_SIZE  2048    /**< \brief �¼���ܴ��������ջ��С */
#define AW_CFG_EVENT_TASK_PRIO      8       /**< \brief �¼���ܴ����������ȼ� */
#define AW_CFG_EVENT_MSG_NUM        16      /**< \brief �¼������󲢷���Ϣ���� */

//#define AW_COM_INPUT_EV                /**< \brief ʹ�������¼����� */
//#define AW_COM_INPUT_EV_KEY            /**< \brief ʹ�ܰ����¼� */
//#define AW_COM_INPUT_EV_ABS            /**< \brief ʹ�ܾ����¼����������¼���������� */
#endif
/** @} */

/**
 * \name ���豸�������ͳ��Դ����
 * @{
 */
#ifdef AW_COM_BLOCK_DEV
#define AW_BLOCK_DEV_EVT_CNT    4
#endif
/** @} */

/** \name I/O ϵͳ����
 * @{
 */
#ifdef AW_COM_IO_SYSTEM
#define AW_CFG_IO_OPEN_FILES_MAX    8   /**< \brief ϵͳ��ͬʱ�򿪵��ļ����� */
#endif
/** @} */

/** \name I/O ϵͳ�洢����ص�ü�����
 * @{
 */
#ifdef AW_COM_MOUNT
#define AW_CFG_MOUNT_POINTS         4
#endif
/** @} */

/**
 * \name RAW �ļ�ϵͳ�ü�����
 * @{
 */
#ifdef AW_COM_FS_RAWFS
#define AW_CFG_RAWFS_VOLUMES    1   /**< \brief RAW �ļ�ϵͳ����� */
#define AW_CFG_RAWFS_FILES      1   /**< \brief RAW �ļ�ϵͳͬʱ���ļ���*/
#define AW_CFG_RAWFS_BLOCK_SIZE 512 /**< \brief RAW �ļ�ϵͳ�����С */
#endif
/** @} */

/**
 * \name FAT �ļ�ϵͳ�ü�����
 * @{
 */
#ifdef AW_COM_FS_FATFS
#define AW_CFG_FATFS_VOLUMES    2       /**< \brief FAT �ļ�ϵͳ����� */
#define AW_CFG_FATFS_FILES      4       /**< \brief FAT �ļ�ϵͳͬʱ���ļ��� */
#define AW_CFG_FATFS_BLOCK_SIZE 4096    /**< \brief FAT �ļ�ϵͳ�����С */
#endif
/** @} */

/**
 * \name LFFS �ļ�ϵͳ�ü�����
 * @{
 */
#ifdef AW_COM_FS_LFFS
#define AW_CFG_LFFS_VOLUMES     2   /**< \brief LFFS �ļ�ϵͳ����� */
#define AW_CFG_LFFS_FILES       10  /**< \brief LFFS �ļ�ϵͳͬʱ���ļ��� */
#endif
/** @} */

/**
 * \name SAFEFS �ļ�ϵͳ�ü�����
 * @{
 */
#ifdef AW_COM_FS_SAFEFS
#define AW_CFG_SAFEFS_VOLUMES   2   /**< \brief SAFEFS �ļ�ϵͳ����� */
#define AW_CFG_SAFEFS_FILES     4   /**< \brief SAFEFS �ļ�ϵͳͬʱ���ļ���*/
#endif
/** @} */

/** \name TXFS�ļ�ϵͳ��Դ����
 * @{
 */
#ifdef AW_COM_FS_TXFS
#define AW_CFG_TXFS_VOLUMES     2   /**< \brief TXFS �ļ�ϵͳ����� */
#define AW_CFG_TXFS_FILES       10  /**< \brief TXFS �ļ�ϵͳͬʱ���ļ���*/
#define AW_CFG_TXFS_BUF_CNT     50  /** \brief TXFS �ļ�ϵͳ���������ֵԽ���дԽ�죬�������ʧ����Խ�� */
#endif
/** @} */


/** \name YAFFS�ļ�ϵͳ��Դ����
 * @{
 */
#ifdef AW_COM_FS_YAFFS
#define AW_CFG_YAFFS_VOLUMES    2   /**< \brief YAFFS �ļ�ϵͳ����� */
#define AW_CFG_YAFFS_FILES      10  /**< \brief YAFFS �ļ�ϵͳͬʱ���ļ���*/
#endif
/** @} */

/** \name ROMFS�ļ�ϵͳ��Դ����
 * @{
 */
#ifdef AW_COM_FS_ROMFS
#define AW_CFG_ROMFS_VOLUMES    1   /**< \brief ROMFS �ļ�ϵͳ����� */
#define AW_CFG_ROMFS_FILES      4   /**< \brief ROMFS �ļ�ϵͳͬʱ���ļ���*/
#endif
/** @} */

/** \name USBЭ��ջ�ü�����
 * @{
 */
#ifdef AW_COM_USBH
#define AW_DRV_USBH_MASS_STORAGE               /**< \brief �������洢����(U��&Ӳ��) */
#endif
#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
#define AW_CFG_USB_MEM_SIZE          (128 * 1024)  /**< \brief USBЭ��ջ�ڴ�ʹ�ÿռ��С */
#endif
#ifdef AW_DRV_USBH_MASS_STORAGE
#define AW_CFG_USB_MS_BUF_SIZE       (16 * 1024)  /**< \brief U�����ݽ������棬ռ��AW_CFG_USB_MEM_SIZE�ռ䣬Ӱ��U�̶�д���ܣ��Ƽ�16k */
#endif
/** @} */

/**
 * \name ʱ�������ͳ��Դ����
 * @{
 */
#ifdef AW_COM_TIMESTAMP
#define AW_TIMESTAMP_NAME       AWBL_IMX6UL_GPT_DRV_NAME    /**< \brief ��ʱ������ */
#define AW_TIMESTAMP_BUSID      IMX6UL_GPT1_BUSID           /**< \brief ��ʱ��ID */
#endif
/** @} */

/*******************************************************************************
  ϵͳ����
*******************************************************************************/

/**
 * \addtogroup  grp_aw_plfm_params_system  ϵͳ����
 * @{
 */

/** \brief ϵͳʱ�ӽ���Ƶ������ */
#define AW_CFG_TICKS_PER_SECOND            1000

/** \brief main�����ջ��С���Ƽ� >= 512 */
#define AW_CFG_MAIN_TASK_STACK_SIZE       (1024 * 32)

/** \brief main�������ȼ� */
#define AW_CFG_MAIN_TASK_PRIO              1

#define AW_CFG_ISR_DEFER_TASK_PRIO         AW_TASK_SYS_PRIORITY(6)
#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE   (1024 * 10)

/** @} grp_aw_plfm_params_system */

/*******************************************************************************
  AWBus-lite ����
*******************************************************************************/

/**
 * \addtogroup  grp_aw_plfm_params_awbus_lite AWBus-lite ����
 * @{
 */

/**
 * \brief  AWBus lite ֧�ֵ����������
 *
 * Ϊ�˱��ⲻ��Ҫ���ڴ����ģ��û����Խ��˲�������Ϊϵͳ��ʵ�ʻ�ʹ�õ�����������
 * ���磬���ֻ�õ����жϿ�������GPIO��������������Խ��˲�������Ϊ2��
 *
 * \attention ��Чֵ��ΧΪ 1 ~ 127
 */
#define AW_CFG_AWBUS_LITE_DRIVER_MAX     80

/**
 * \brief AWBus lite ֧�ֵ��������������
 *
 * Ϊ�˱��ⲻ��Ҫ���ڴ����ģ��û����Խ��˲�������Ϊϵͳ��ʵ�ʻ�ʹ�õ�������������
 * ���磬���ֻ�õ���CPU�������� (����ʹ��)��I2C�����������ͣ�����Խ��˲�������Ϊ2��
 *
 * \attention ��Чֵ��ΧΪ 1 ~ 127
 */
#define AW_CFG_AWBUS_LITE_BUSTYPE_MAX    16

/** @} grp_aw_plfm_params_system */

/*******************************************************************************
  ���ж˿�����
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_serial_common_usrcfg
 * @{
 */

/**
 * \brief ϵͳ���ж˿ڸ���
 *
 * ��ǰ֧�ֵ����˿ڸ���Ϊ10����Ӧ�ø���ʵ�ʵ�Ӳ�������޸ġ�
 * ���õĶ˿ڸ�����Ӧ�ó���10����
 */
#define AW_CFG_NUM_COM              10

/**
 * \name ���ж˿ڵ��շ������С����
 * \brief ���õĴ��ж˿ڣ���ų���AW_CFG_NUM_COM�������ò������á�
 * @{
 */

#define AW_CFG_COM0_TX_BUF_SIZE     128     /**< \brief COM0 ���ͻ����С */
#define AW_CFG_COM0_RX_BUF_SIZE     128     /**< \brief COM0 ���ջ����С */

#define AW_CFG_COM1_TX_BUF_SIZE     128     /**< \brief COM1 ���ͻ����С */
#define AW_CFG_COM1_RX_BUF_SIZE     128     /**< \brief COM1 ���ջ����С */

#define AW_CFG_COM2_TX_BUF_SIZE     128     /**< \brief COM2 ���ͻ����С */
#define AW_CFG_COM2_RX_BUF_SIZE     128     /**< \brief COM2 ���ջ����С */

#define AW_CFG_COM3_TX_BUF_SIZE     128     /**< \brief COM3 ���ͻ����С */
#define AW_CFG_COM3_RX_BUF_SIZE     128     /**< \brief COM3 ���ջ����С */

#define AW_CFG_COM4_TX_BUF_SIZE     128     /**< \brief COM4 ���ͻ����С */
#define AW_CFG_COM4_RX_BUF_SIZE     128     /**< \brief COM4 ���ջ����С */

#define AW_CFG_COM5_TX_BUF_SIZE     128     /**< \brief COM5 ���ͻ����С */
#define AW_CFG_COM5_RX_BUF_SIZE     128     /**< \brief COM5 ���ջ����С */

#define AW_CFG_COM6_TX_BUF_SIZE     128     /**< \brief COM6 ���ͻ����С */
#define AW_CFG_COM6_RX_BUF_SIZE     128     /**< \brief COM6 ���ջ����С */

#define AW_CFG_COM7_TX_BUF_SIZE     128     /**< \brief COM7 ���ͻ����С */
#define AW_CFG_COM7_RX_BUF_SIZE     128     /**< \brief COM7 ���ջ����С */

#define AW_CFG_COM8_TX_BUF_SIZE     128     /**< \brief COM8 ���ͻ����С */
#define AW_CFG_COM8_RX_BUF_SIZE     128     /**< \brief COM8 ���ջ����С */

#define AW_CFG_COM9_TX_BUF_SIZE     128     /**< \brief COM9 ���ͻ����С */
#define AW_CFG_COM9_RX_BUF_SIZE     128     /**< \brief COM9 ���ջ����С */
/** @} */

/** \name CAN�����ʱ�
 * @{
 */

/** \brief �����ʱ�ʹ��CAN�����ʼ��㹤�߼���ó�,�����ο�
 *         (Ŀǰ����ʱ��ΪPLL3 6��Ƶ ��2��Ƶ 40MHz)
 *                        tseg1 tseg2 sjw smp brp
 */
#define AW_CFG_CAN_BTR_1000K {15,  4,   4,   0,   2  } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_800K  {6,   3,   3,   0,   5  } /**< \brief ������ 70% */
#define AW_CFG_CAN_BTR_500K  {15,  4,   4,   0,   4  } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_250K  {15,  4,   4,   0,   8  } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_125K  {15,  4,   4,   0,   16 } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_100K  {15,  4,   4,   0,   20 } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_50K   {15,  4,   4,   0,   40 } /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_20K   {15,  4,   4,   0,   100} /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_10K   {15,  4,   4,   0,   200} /**< \brief ������ 80% */
#define AW_CFG_CAN_BTR_6_25K {16,  8,   4,   0,   256} /**< \brief ������ 68% */

/** @} */

/** @} grp_aw_plfm_serial_common_usrcfg */

/*******************************************************************************
  IMX6UL I2C ����
*******************************************************************************/
/**
 * \addtogroup  grp_aw_plfm_imx6ul_i2c_usrcfg
 * @{
 */

/**
 * \brief IMX6UL I2C �����ٶ�
 *
 * ��λ: Hz�� ��Χ  1000 ~ 1000000 (1k ~ 1M)
 */
#define AW_CFG_IMX6UL_I2C_BUS_SPEED       200000

/** @} grp_aw_plfm_params_rescfg */

/*******************************************************************************
  ��ԴID
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_idcfg   ��ԴID����
 * @{
 */

/* ���� ID ���� */
#define IMX6UL_UART1_COMID      COM0
#define IMX6UL_UART2_COMID      COM1
#define IMX6UL_UART3_COMID      COM2
#define IMX6UL_UART4_COMID      COM3
#define IMX6UL_UART5_COMID      COM4
#define IMX6UL_UART6_COMID      COM5
#define IMX6UL_UART7_COMID      COM6
#define IMX6UL_UART8_COMID      COM7

/* I2C ����ID����  */
#define IMX6UL_I2C1_BUSID       0
#define IMX6UL_I2C2_BUSID       1
#define IMX6UL_I2C3_BUSID       2
#define IMX6UL_I2C4_BUSID       3
#define IMX6UL_GPIO_I2C0_BUSID  4

/* ECSPI ����ID���� */
#define IMX6UL_ECSPI1_BUSID     0
#define IMX6UL_ECSPI2_BUSID     1
#define IMX6UL_ECSPI3_BUSID     2
#define IMX6UL_ECSPI4_BUSID     3
#define IMX6UL_QSPI1_BUSID      4
#define IMX6UL_GPIO_SPI0_BUSID  5

#define IMX6UL_MII0_BUSID       0
#define IMX6UL_MII1_BUSID       1
/* NAND���� ID ���� */
#define IMX6UL_NAND0_BUSID      0

/* CAN ID ���� */
#define IMX6UL_FLAX_CAN1_BUSID      0
#define IMX6UL_FLAX_CAN2_BUSID      1
#define IMX6UL_SJA1000_CAN3_BUSID   2

/* USB���� ID ���� */
#define IMX6UL_USB1_BUSID       0
#define IMX6UL_USB2_BUSID       1

/* RTC ID ���� */
#define RTC_ID0                 0
#define RTC_ID1                 1

/* Timer ID���� */
#define IMX6UL_GPT1_BUSID       1
#define IMX6UL_GPT2_BUSID       2

/** \name canopen ��Դ����
 * @{
 */
#if defined(AW_COM_CANOPEN)
#define AW_COP_CFG_TIMER_NAME  "imx6ul_gpt"
#define AW_COP_CFG_TIMER_UNIT  1
#define AW_COP_CFG_TIMER_ID    0
#endif

/* EIM���� ID ���� */
#define IMX6UL_EIM0_BUSID       0


/* SDIO ID ���� */
#define IMX6UL_SDIO0_BUSID      0
#define IMX6UL_SDIO1_BUSID      1


#define GNSS_ID_EC20   0
#define GNSS_ID_UBLOX  1

#ifdef AW_DEV_GNSS_UBLOX
#define SYS_DEF_GNSS_ID  AW_DEV_GNSS_UBLOX
#elif defined(AW_DEV_GPS_EC20)
#define SYS_DEF_GNSS_ID  GNSS_ID_EC20
#endif


/** @} grp_aw_plfm_params_idcfg */


#ifdef AW_DEV_BU21029MUV
#define SYS_TS_ID  "bu21029muv"
#elif defined(AW_DEV_FT5X06)
#define SYS_TS_ID  "ft5x06"
#elif defined(AW_DEV_IMX6UL_TS)
#define SYS_TS_ID  "480x272"
#else
#define SYS_TS_ID  "none"
#endif

#if defined(AW_DEV_TFT_4_3) || defined(AW_DEV_HW480272F)
#define SYS_LCD_X_RES 480
#define SYS_LCD_Y_RES 272
#elif defined(AW_DEV_AP_TFT7_0)||defined(AW_DEV_LCD_TM070RDH12_24B)||defined(AW_DEV_HW800480F)
#define SYS_LCD_X_RES 800
#define SYS_LCD_Y_RES 480
#elif defined(AW_DEV_LCD_640480W056TR)
#define SYS_LCD_X_RES 640
#define SYS_LCD_Y_RES 480
#elif defined(AW_DEV_LCD_800600W080TR)
#define SYS_LCD_X_RES 800
#define SYS_LCD_Y_RES 600
#endif

/** @} grp_aw_prj_param */

#include "aw_prj_param_auto_cfg.h"
#endif  /* __AW_PRJ_PARAMS_H */

/* end of file */
