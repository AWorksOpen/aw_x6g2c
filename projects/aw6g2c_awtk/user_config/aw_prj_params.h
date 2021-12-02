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
 * \brief AnyWhere 工程参数配置文件
 */

#ifndef __AW_PRJ_PARAMS_H
#define __AW_PRJ_PARAMS_H

/**
 * \addtogroup grp_aw_prj_param
 * @{
 */

/*******************************************************************************
  硬件剪裁
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_params_hwcfg   硬件剪裁
 *
 * -# 注释掉外设宏定义即可禁能对应的外设，反之，则使能;
 * -# 使能某个外设后，该外设所依赖的软件组件会被自动配置进来，如无特殊需求，则
 *    无需进行下面的“组件剪裁”；
 * -# 中断控制器和GPIO是最基础的外设，默认被使能。
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
//#define AW_DEV_IMX6UL_QSPI_1            /**< \brief QSPI驱动 */
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
//#define AW_DEV_NAND_FLASH_S34ML01G2     /**< \brief NAND FLASH设备 */
#define AW_DEV_NAND_FLASH_S34ML02G2     /**< \brief NAND FLASH设备 */
#define AW_DEV_IMX6UL_GPTIMER_1         /**< \brief IMX6UL GPTIMER1 */
#define AW_DEV_IMX6UL_GPTIMER_2         /**< \brief IMX6UL GPTIMER2 */
#define AW_DEV_IMX6UL_TIMESTAMP         /**< \brief IMX6UL TIMESTAMP */
//#define AW_DEV_SPI_FLASH0               /**< \brief SPI FLASH0设备驱动 */
//#define AW_DEV_SPI_FLASH1               /**< \brief SPI FLASH1设备驱动 */
//#define AW_DEV_IMX6UL_CAN1              /**< \brief iMX6UL flex can1 */
//#define AW_DEV_IMX6UL_CAN2              /**< \brief iMX6UL flex can2 */

//#define AW_DEV_IMX6UL_CAAM              /**< \brief IMX6UL CAAM */
#define AW_DEV_IMX6UL_APBHDMA           /**< \brief IMX6UL APBH-DMA */
#define AW_DEV_IMX6UL_SDMA              /**< \brief imx6ul sdma */
#define AW_DEV_IMX6UL_OCOTP             /**< \brief IMX6UL OCOTP */
#define AW_DEV_IMX6UL_ENET0              /**< \brief IMX6UL ENET0 */
//#define AW_DEV_IMX6UL_ENET1              /**< \brief IMX6UL ENET1 */

#define AW_DEV_EXTEND_PCF85063_0        /**< \brief PCF85063 */

#define AW_DEV_SDCARD                   /**< \brief SD卡驱动 */

#define AW_DEV_IMX6UL_USDHC1          /**< \brief SDIO0驱动 */
//#define AW_DEV_IMX6UL_USDHC2            /**< \brief SDIO1驱动 */

#define AW_DEV_GPIO_LED                 /**< \brief GPIO LED */

//#define AW_DEV_PWM_BUZZER               /**< \brief PWM Buzzer(蜂鸣器，需要配套PWM) */
#define AW_DEV_IMX6UL_PWM               /**< \brief IMX6UL PWM */

//#define AW_DEV_HW480272F                /**< \brief HW480272F 液晶屏 */
#define AW_DEV_HW800480F                /**< \brief HW800480F 液晶屏 */
#define AW_DEV_IMX6UL_TS                /**< \brief MX6UL Touch Screen Controller */
//#define AW_DEV_IMX6UL_ADC1
//#define AW_DEV_GPIO_KEY                 /**< \brief 按键 */

//#define AW_DEV_OV7725
//#define AW_DEV_IMX6UL_CSI

//#define AW_DEV_IMX6UL_SAI1              /**< \brief IMX6UL SAI1    */
//#define AW_DEV_IMX6UL_MQS               /**< \brief IMX6UL MQS     */
//#define AW_DEV_EXTEND_EP24CXX_0         /**< \brief 外扩EEPROM */
//#define AW_DEV_IMX6UL_TEMPMON           /**< \brief temperature monitor */
//#define AW_DEV_ZLG600A_I2C_0
//#define AW_DEV_ZLG600A_UART_0
//#define AW_DEV_ZIGBEE                   /**< \brief zigbee */
//#define AW_DEV_GT911                    /**< \brief 电容触摸屏芯片gt911  */

//#define AW_DEV_GPRS_SIM800
//#define AW_DEV_GPRS_ME909S
//#define AW_DEV_GPRS_U9300C
//#define AW_DEV_GPRS_EC20                /**< \brief GPRS EC20  */
//#define AW_DEV_GPS_EC20

//#define AW_DEV_SJA1000
//#define AW_DEV_IMX6UL_EIM

//#define AW_DEV_GNSS_UBLOX               /*gps设备*/

//#define AW_DEV_CYW43362_WIFI_SDIO         /**< \brief WIFI模块(需要SDHC2外设) */

//#define AW_DEV_DC_BUZZER                /**< \brief DC Buzzer(蜂鸣器) */
//#define AW_DEV_PANEL_PM1000             /**< \brief PM1000前面板 */
//#define AW_DEV_TINY_HMI                 /**< \brief TinyHMI模块 */
//#define AW_DEV_APL_D01                  /**< \brief APL_D01模块 */
//#define AW_DEV_NS16550_PLB              /**< \brief 16550 UART 兼容驱动(PLB)(自动配置) */
//#define AW_DEV_FM175XX                  /**< \brief Mifare卡驱动(需要SPI外设) */
/** @} grp_aw_plfm_params_hwcfg */

/*******************************************************************************
  组件剪裁
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_params_sftcfg   组件剪裁
 *
 * -# 注释掉组件宏定义即可禁能对应的组件，反之，则使能;
 * -# 使能某个组件后，该外设所依赖的软件组件会被自动配置进来;
 * -# 若某个硬件外设依赖这里的组件，当该硬件外设使能时，无论这些被依赖的组件
 *    是否被使能，都将被自动配置进来。
 * -# 注释中带“(必须)”字样的，表示该组件必须被使能；注释中带“(自动配置)”字样的
 *    表示该组件是可以被自动剪裁的，通常，这些组件保持禁能就好；
 *    其它组件则根据自己的需要，如果需要用到就使能，未使用就可以禁能。
 * @{
 */

#define AW_COM_CONSOLE                  /**< \brief 控制台组件 */
#define AW_COM_SHELL_SERIAL             /**< \brief 使能串口shell */
//#define AW_COM_SHELL_TELNET             /**< \brief 使用telent远程连接shell */
//#define AW_COM_FTPD                     /**< \brief ftp server 组件 */
#define AW_COM_EVENT                    /**< \brief 使能事件系统 */
//#define AW_COM_TIMESTAMP                /**< \brief 时间戳服务 */

//#define AW_COM_DIGITRON_DISP            /**< \brief 数码管显示通用库 */
//#define AW_COM_LOG                      /**< \brief 系统日志组件 */
//#define AW_COM_FTL                      /**< \brief FLASH 管理层，负责MTD设备的均衡磨损、坏块管理等 */

//#define AW_COM_CANOPEN          /**< \brief CAN应用高层协议栈CANopen组件 */

/**
 * \name 文件系统裁剪配置
 * @{
 */
//#define AW_COM_FS_RAWFS               /**< \brief 使能 RAW 文件系统 */
//#define AW_COM_FS_DUMPFS              /**< \brief 使能 dump 文件系统 */
#define AW_COM_FS_FATFS               /**< \brief 使能 FAT 文件系统 */
#define AW_COM_FS_LFFS                /**< \brief 使能 LFFS 文件系统 */
//#define AW_COM_FS_SAFEFS              /**< \brief 使能安全文件系统 */
//#define AW_COM_FS_TXFS                /**< \brief 使能事务型文件系统 */
#define AW_COM_FS_YAFFS               /**< \brief 使能 YAFFS 文件系统 */
//#define AW_COM_FS_ROMFS               /**< \brief 使能 ROMFS 文件系统 */
/** @} */

/** @} grp_aw_plfm_params_sftcfg */

/**
 * \name 组件自动配置和检测
 * @{
 */
#include "aw_prj_param_auto_cfg.h"
/** @} */

/*******************************************************************************
  资源配置
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_rescfg   资源配置
 * @{
 */

/**
 * \name 控制台组件资源配置
 * @{
 */
#ifdef AW_COM_CONSOLE
#define AW_CFG_CONSOLE_SHOW_BANNER      1                       /** \brief 控制台启动时，是否显示欢迎标识：0-不显示 1-显示 */
#define AW_CFG_CONSOLE_COMID            IMX6UL_UART1_COMID      /** \brief 控制台串口号 */
#define AW_CFG_CONSOLE_BAUD_RATE        115200                  /** \brief 控制台波特率 */
#endif
/** @} */

/**
 * \name shell组件资源配置
 * @{
 */
#ifdef AW_COM_SHELL_SERIAL
#define AW_CFG_SHELL_THREAD_PRIO        6u                      /**< \brief serial shell 线程优先级 */
#define AW_CFG_SHELL_THREAD_STK_SIZ     (10 * 1024)             /**< \brief serial shell 线程堆栈大小 */
#define AW_CFG_SHELL_SERIAL_COMID       IMX6UL_UART1_COMID      /**< \brief serial shell 使用的串口设备号 */

#define AW_SHELL_IO                                             /**< \brief 使能shell IO操作，如:"ls" "cd" "mkdir"等命令 */
#define AW_NET_TOOLS                                            /**< \brief 使能shell NET操作，如:"ip" "ping"等 */
#define AW_SHELL_LUA                                            /**< \brief 使能shell lua操作，如:"lua" 等 */
#define AW_SHELL_LUAC                                           /**< \brief 使能shell luac操作，如:"luac"等 */
#endif

#ifdef AW_COM_SHELL_TELNET
#define AW_CFG_TELNET_TASK_PRIO         1           /**< \brief telent shell 线程优先级  */
#define AW_CFG_TELNET_TASK_STACK_SIZE   8192        /**< \brief telent shell 线程堆栈大小 */
#define AW_CFG_TELNET_MAX_SESSION       2           /**< \brief telent shell 最大同时连接数目 */
#define AW_CFG_TELNET_LISTEN_PORT       23          /**< \brief telent shell 网络端口号 */
#define AW_CFG_TELNET_USER_NAME         "admin"     /**< \brief telent shell 用户登入名 */
#define AW_CFG_TELNET_USER_PSW          "123456"    /**< \brief telent shell 用户登入密码 */
#endif
/** @} */

/**
 * \name FTP 组件资源配置
 * @{
 */

/** \brief FTP 组件需要文件系统支持，因而需要打开文件系统，并且需要调用文件系统的mount接口，
 *         创建一个文件根节点
 */
#ifdef AW_COM_FTPD                                  /**< \brief 使用ftp server */
#define FTPD_MAX_SESSIONS               1           /**< \brief FTPD 最大同时连接数目 */
#define FTPD_LISTEN_TASK_PRIO           6           /**< \brief FTPD 任务优先级 */
#define FTPD_LISTEN_TASK_STK_SIZE       4096        /**< \brief FTPD 栈大小 */
#define FTPD_CMD_BUF_SIZE               0x10000     /**< \brief FTPD 运行buffer */
#endif
/** @} */

/** \name 事件系统资源配置
 * @{
 */
#ifdef AW_COM_EVENT
#define AW_CFG_EVENT_TASK_STK_SIZE  2048    /**< \brief 事件框架处理任务堆栈大小 */
#define AW_CFG_EVENT_TASK_PRIO      8       /**< \brief 事件框架处理任务优先级 */
#define AW_CFG_EVENT_MSG_NUM        16      /**< \brief 事件框架最大并发消息个数 */

//#define AW_COM_INPUT_EV                /**< \brief 使能输入事件管理 */
//#define AW_COM_INPUT_EV_KEY            /**< \brief 使能按键事件 */
//#define AW_COM_INPUT_EV_ABS            /**< \brief 使能绝对事件，触摸屏事件依赖此组件 */
#endif
/** @} */

/**
 * \name 块设备驱动框架统资源配置
 * @{
 */
#ifdef AW_COM_BLOCK_DEV
#define AW_BLOCK_DEV_EVT_CNT    4
#endif
/** @} */

/** \name I/O 系统配置
 * @{
 */
#ifdef AW_COM_IO_SYSTEM
#define AW_CFG_IO_OPEN_FILES_MAX    8   /**< \brief 系统能同时打开的文件个数 */
#endif
/** @} */

/** \name I/O 系统存储卷挂载点裁剪配置
 * @{
 */
#ifdef AW_COM_MOUNT
#define AW_CFG_MOUNT_POINTS         4
#endif
/** @} */

/**
 * \name RAW 文件系统裁剪配置
 * @{
 */
#ifdef AW_COM_FS_RAWFS
#define AW_CFG_RAWFS_VOLUMES    1   /**< \brief RAW 文件系统卷个数 */
#define AW_CFG_RAWFS_FILES      1   /**< \brief RAW 文件系统同时打开文件数*/
#define AW_CFG_RAWFS_BLOCK_SIZE 512 /**< \brief RAW 文件系统最大块大小 */
#endif
/** @} */

/**
 * \name FAT 文件系统裁剪配置
 * @{
 */
#ifdef AW_COM_FS_FATFS
#define AW_CFG_FATFS_VOLUMES    2       /**< \brief FAT 文件系统卷个数 */
#define AW_CFG_FATFS_FILES      4       /**< \brief FAT 文件系统同时打开文件数 */
#define AW_CFG_FATFS_BLOCK_SIZE 4096    /**< \brief FAT 文件系统最大块大小 */
#endif
/** @} */

/**
 * \name LFFS 文件系统裁剪配置
 * @{
 */
#ifdef AW_COM_FS_LFFS
#define AW_CFG_LFFS_VOLUMES     2   /**< \brief LFFS 文件系统卷个数 */
#define AW_CFG_LFFS_FILES       10  /**< \brief LFFS 文件系统同时打开文件数 */
#endif
/** @} */

/**
 * \name SAFEFS 文件系统裁剪配置
 * @{
 */
#ifdef AW_COM_FS_SAFEFS
#define AW_CFG_SAFEFS_VOLUMES   2   /**< \brief SAFEFS 文件系统卷个数 */
#define AW_CFG_SAFEFS_FILES     4   /**< \brief SAFEFS 文件系统同时打开文件数*/
#endif
/** @} */

/** \name TXFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_TXFS
#define AW_CFG_TXFS_VOLUMES     2   /**< \brief TXFS 文件系统卷个数 */
#define AW_CFG_TXFS_FILES       10  /**< \brief TXFS 文件系统同时打开文件数*/
#define AW_CFG_TXFS_BUF_CNT     50  /** \brief TXFS 文件系统缓存个数，值越大读写越快，但掉电后丢失数据越多 */
#endif
/** @} */


/** \name YAFFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_YAFFS
#define AW_CFG_YAFFS_VOLUMES    2   /**< \brief YAFFS 文件系统卷个数 */
#define AW_CFG_YAFFS_FILES      10  /**< \brief YAFFS 文件系统同时打开文件数*/
#endif
/** @} */

/** \name ROMFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_ROMFS
#define AW_CFG_ROMFS_VOLUMES    1   /**< \brief ROMFS 文件系统卷个数 */
#define AW_CFG_ROMFS_FILES      4   /**< \brief ROMFS 文件系统同时打开文件数*/
#endif
/** @} */

/** \name USB协议栈裁剪配置
 * @{
 */
#ifdef AW_COM_USBH
#define AW_DRV_USBH_MASS_STORAGE               /**< \brief 大容量存储驱动(U盘&硬盘) */
#endif
#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
#define AW_CFG_USB_MEM_SIZE          (128 * 1024)  /**< \brief USB协议栈内存使用空间大小 */
#endif
#ifdef AW_DRV_USBH_MASS_STORAGE
#define AW_CFG_USB_MS_BUF_SIZE       (16 * 1024)  /**< \brief U盘数据交互缓存，占用AW_CFG_USB_MEM_SIZE空间，影响U盘读写性能，推荐16k */
#endif
/** @} */

/**
 * \name 时间戳服务统资源配置
 * @{
 */
#ifdef AW_COM_TIMESTAMP
#define AW_TIMESTAMP_NAME       AWBL_IMX6UL_GPT_DRV_NAME    /**< \brief 定时器名字 */
#define AW_TIMESTAMP_BUSID      IMX6UL_GPT1_BUSID           /**< \brief 定时器ID */
#endif
/** @} */

/*******************************************************************************
  系统配置
*******************************************************************************/

/**
 * \addtogroup  grp_aw_plfm_params_system  系统配置
 * @{
 */

/** \brief 系统时钟节拍频率配置 */
#define AW_CFG_TICKS_PER_SECOND            1000

/** \brief main任务堆栈大小，推荐 >= 512 */
#define AW_CFG_MAIN_TASK_STACK_SIZE       (1024 * 32)

/** \brief main任务优先级 */
#define AW_CFG_MAIN_TASK_PRIO              1

#define AW_CFG_ISR_DEFER_TASK_PRIO         AW_TASK_SYS_PRIORITY(6)
#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE   (1024 * 10)

/** @} grp_aw_plfm_params_system */

/*******************************************************************************
  AWBus-lite 配置
*******************************************************************************/

/**
 * \addtogroup  grp_aw_plfm_params_awbus_lite AWBus-lite 配置
 * @{
 */

/**
 * \brief  AWBus lite 支持的最大驱动数
 *
 * 为了避免不必要的内存消耗，用户可以将此参数设置为系统中实际会使用的驱动个数。
 * 例如，如果只用到了中断控制器、GPIO两个驱动，则可以将此参数设置为2。
 *
 * \attention 有效值范围为 1 ~ 127
 */
#define AW_CFG_AWBUS_LITE_DRIVER_MAX     80

/**
 * \brief AWBus lite 支持的最大总线类型数
 *
 * 为了避免不必要的内存消耗，用户可以将此参数设置为系统中实际会使用的总线类型数。
 * 例如，如果只用到了CPU本地总线 (必须使用)、I2C两种总线类型，则可以将此参数设置为2。
 *
 * \attention 有效值范围为 1 ~ 127
 */
#define AW_CFG_AWBUS_LITE_BUSTYPE_MAX    16

/** @} grp_aw_plfm_params_system */

/*******************************************************************************
  串行端口配置
*******************************************************************************/

/**
 * \addtogroup grp_aw_plfm_serial_common_usrcfg
 * @{
 */

/**
 * \brief 系统串行端口个数
 *
 * 当前支持的最大端口个数为10个，应该根据实际的硬件配置修改。
 * 配置的端口个数不应该超过10个。
 */
#define AW_CFG_NUM_COM              10

/**
 * \name 串行端口的收发缓冲大小配置
 * \brief 不用的串行端口（编号超过AW_CFG_NUM_COM）的配置不起作用。
 * @{
 */

#define AW_CFG_COM0_TX_BUF_SIZE     128     /**< \brief COM0 发送缓冲大小 */
#define AW_CFG_COM0_RX_BUF_SIZE     128     /**< \brief COM0 接收缓冲大小 */

#define AW_CFG_COM1_TX_BUF_SIZE     128     /**< \brief COM1 发送缓冲大小 */
#define AW_CFG_COM1_RX_BUF_SIZE     128     /**< \brief COM1 接收缓冲大小 */

#define AW_CFG_COM2_TX_BUF_SIZE     128     /**< \brief COM2 发送缓冲大小 */
#define AW_CFG_COM2_RX_BUF_SIZE     128     /**< \brief COM2 接收缓冲大小 */

#define AW_CFG_COM3_TX_BUF_SIZE     128     /**< \brief COM3 发送缓冲大小 */
#define AW_CFG_COM3_RX_BUF_SIZE     128     /**< \brief COM3 接收缓冲大小 */

#define AW_CFG_COM4_TX_BUF_SIZE     128     /**< \brief COM4 发送缓冲大小 */
#define AW_CFG_COM4_RX_BUF_SIZE     128     /**< \brief COM4 接收缓冲大小 */

#define AW_CFG_COM5_TX_BUF_SIZE     128     /**< \brief COM5 发送缓冲大小 */
#define AW_CFG_COM5_RX_BUF_SIZE     128     /**< \brief COM5 接收缓冲大小 */

#define AW_CFG_COM6_TX_BUF_SIZE     128     /**< \brief COM6 发送缓冲大小 */
#define AW_CFG_COM6_RX_BUF_SIZE     128     /**< \brief COM6 接收缓冲大小 */

#define AW_CFG_COM7_TX_BUF_SIZE     128     /**< \brief COM7 发送缓冲大小 */
#define AW_CFG_COM7_RX_BUF_SIZE     128     /**< \brief COM7 接收缓冲大小 */

#define AW_CFG_COM8_TX_BUF_SIZE     128     /**< \brief COM8 发送缓冲大小 */
#define AW_CFG_COM8_RX_BUF_SIZE     128     /**< \brief COM8 接收缓冲大小 */

#define AW_CFG_COM9_TX_BUF_SIZE     128     /**< \brief COM9 发送缓冲大小 */
#define AW_CFG_COM9_RX_BUF_SIZE     128     /**< \brief COM9 接收缓冲大小 */
/** @} */

/** \name CAN波特率表
 * @{
 */

/** \brief 波特率表，使用CAN波特率计算工具计算得出,仅供参考
 *         (目前输入时钟为PLL3 6分频 再2分频 40MHz)
 *                        tseg1 tseg2 sjw smp brp
 */
#define AW_CFG_CAN_BTR_1000K {15,  4,   4,   0,   2  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_800K  {6,   3,   3,   0,   5  } /**< \brief 采样点 70% */
#define AW_CFG_CAN_BTR_500K  {15,  4,   4,   0,   4  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_250K  {15,  4,   4,   0,   8  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_125K  {15,  4,   4,   0,   16 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_100K  {15,  4,   4,   0,   20 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_50K   {15,  4,   4,   0,   40 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_20K   {15,  4,   4,   0,   100} /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_10K   {15,  4,   4,   0,   200} /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_6_25K {16,  8,   4,   0,   256} /**< \brief 采样点 68% */

/** @} */

/** @} grp_aw_plfm_serial_common_usrcfg */

/*******************************************************************************
  IMX6UL I2C 配置
*******************************************************************************/
/**
 * \addtogroup  grp_aw_plfm_imx6ul_i2c_usrcfg
 * @{
 */

/**
 * \brief IMX6UL I2C 总线速度
 *
 * 单位: Hz， 范围  1000 ~ 1000000 (1k ~ 1M)
 */
#define AW_CFG_IMX6UL_I2C_BUS_SPEED       200000

/** @} grp_aw_plfm_params_rescfg */

/*******************************************************************************
  资源ID
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_idcfg   资源ID配置
 * @{
 */

/* 串口 ID 分配 */
#define IMX6UL_UART1_COMID      COM0
#define IMX6UL_UART2_COMID      COM1
#define IMX6UL_UART3_COMID      COM2
#define IMX6UL_UART4_COMID      COM3
#define IMX6UL_UART5_COMID      COM4
#define IMX6UL_UART6_COMID      COM5
#define IMX6UL_UART7_COMID      COM6
#define IMX6UL_UART8_COMID      COM7

/* I2C 总线ID分配  */
#define IMX6UL_I2C1_BUSID       0
#define IMX6UL_I2C2_BUSID       1
#define IMX6UL_I2C3_BUSID       2
#define IMX6UL_I2C4_BUSID       3
#define IMX6UL_GPIO_I2C0_BUSID  4

/* ECSPI 总线ID分配 */
#define IMX6UL_ECSPI1_BUSID     0
#define IMX6UL_ECSPI2_BUSID     1
#define IMX6UL_ECSPI3_BUSID     2
#define IMX6UL_ECSPI4_BUSID     3
#define IMX6UL_QSPI1_BUSID      4
#define IMX6UL_GPIO_SPI0_BUSID  5

#define IMX6UL_MII0_BUSID       0
#define IMX6UL_MII1_BUSID       1
/* NAND总线 ID 分配 */
#define IMX6UL_NAND0_BUSID      0

/* CAN ID 分配 */
#define IMX6UL_FLAX_CAN1_BUSID      0
#define IMX6UL_FLAX_CAN2_BUSID      1
#define IMX6UL_SJA1000_CAN3_BUSID   2

/* USB总线 ID 分配 */
#define IMX6UL_USB1_BUSID       0
#define IMX6UL_USB2_BUSID       1

/* RTC ID 分配 */
#define RTC_ID0                 0
#define RTC_ID1                 1

/* Timer ID分配 */
#define IMX6UL_GPT1_BUSID       1
#define IMX6UL_GPT2_BUSID       2

/** \name canopen 资源配置
 * @{
 */
#if defined(AW_COM_CANOPEN)
#define AW_COP_CFG_TIMER_NAME  "imx6ul_gpt"
#define AW_COP_CFG_TIMER_UNIT  1
#define AW_COP_CFG_TIMER_ID    0
#endif

/* EIM总线 ID 分配 */
#define IMX6UL_EIM0_BUSID       0


/* SDIO ID 分配 */
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
