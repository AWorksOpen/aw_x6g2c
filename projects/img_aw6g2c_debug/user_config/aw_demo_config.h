
#ifndef __AW_DEMO_CONFIG_H
#define __AW_DEMO_CONFIG_H

#include "aw_prj_params.h"
#include "aw_gpio.h"
#include "aw_cpu_clk.h"
#include "imx6ul_pwmid.h"
#include "driver/timer/awbl_imx6ul_gptimer.h"

/*定义modbus串口id*/
#define DE_MODBUS_UART_ID               COM2
/* ADC例程通道*/
#define DE_ADC_CH                       8
/* CAN例程通道*/
#define DE_CAN_CH                       0
/* 摄像头例程设备名字*/
#define DE_CAMERA_NAME                  "ov7725"
/* 帧缓冲设备名字*/
#define DE_CAMEAR_EMWIN_FB              "mxs_emwin_fb"
/* eeprom设备名字*/
#define DE_EEPROM_NAME                  "ep24cxx"
/* PWM例程 ID配置 */
#define DE_PWMID                        PWM6
/* eth0设备名字*/
#define DE_ETH0_NAME                    "eth0"
/* LED 引脚号*/
#define DM_GPIO_LED                     GPIO4_16
/* GPIO INT */
#define DM_GPIO_TRG_OUTPUT              GPIO3_25   /* GPIO中断例程 引脚定义 */
#define DM_GPIO_TRG_INTR                GPIO3_26
/* GPIO设备号*/
#define DM_LED                          1

/* GPT 例程*/
#define DE_HWTIMER_TIMER_NAME           AWBL_IMX6UL_GPT_DRV_NAME      /* gpt定时器名称 */
#define DE_HWTIMER_TIMER_ID             IMX6UL_GPT1_BUSID             /* gpt1 ID号 */

//#define DE_QTIMER_NAME                  AWBL_IMX6UL_GPT_DRV_NAME          /* qtimer定时器名称 */
//#define DE_QTIMER_ID                    IMX6UL_GPT1_BUSID   /* qtimer3 ID号 */

#define DE_TIMER_CAPID                  IMX6UL_GPT1_BUSID  /* 定时器输入捕获ID */
#define DE_TIMER_CAPID_GPIO             GPIO1_20                  /*QTIMER3_CAP3通道对应的引脚*/

#define DE_GPIO_PWM                     GPIO3_26                      /* GPIO模拟产生PWM脉冲  */
#define DE_HWTIMER_INPUT_CAP_IO         0                             /* GPT1输入捕获功能 IO 引脚 */
#define DE_CLK_GPT                      IMX6UL_CLK_PERCLK             /* GPT定时器时钟ID */
/* 网卡设备名字*/
#define DE_ENET_NAME                    "eth0"
/* i2c例程i2c ID*/
#define DE_I2C_ID                       4
/* 触摸屏设备名字*/
#define DE_TS_NAME                      SYS_TS_ID
/* nand disk名字*/
#define DE_DISK_NAME                    "/dev/mtd-user"
/* nand fs名字*/
#define DE_NAMD_FS_NAME                 "lffs"
/* gprs ID*/
#define DE_GPRS_DEV_ID                  2
/* rtc ID*/
#define DE_RTC_ID0                      RTC_ID0
#define DE_RTC_ID1                      RTC_ID1
/* SD卡设备名字*/
#define DE_SD_DEV_NAME                  "/dev/sd0"
/* SPI总线ID*/
#define DE_SPI_DEV_BUSID                0
/* SPI片选引脚*/
#define DE_SPI_CS_PIN                   GPIO1_28
/* SPI Flash例程*/
#define DE_SPI_FLASH                    "/sflash1"
#define DE_SPIFLASH_NVRAM_NAME          "spi_flash1_nvram"

/* USB块设备名字*/
#define DE_USB_BLK_NAME                 "/dev/h1-d1-0"
#define DE_USB_DEV_NAME                 "/dev/usbd0"

/* nand flash设备名字*/
#define DE_NANDFLASH_DEV_NAME           AWBL_NAND_DRV_NAME
#define DE_NAND_FLASH_DEV_ID            1

/* otp存储段名称和单元号，在awbl_hwconf_imx6ul_ocotp.h 中查看修改  */
#define DE_OTP_MEM                      "otp_osp"

#define DE_FB                           "mxs_fb"    /*显示屏名称*/
#define DE_TFT_TS_NAME                  SYS_TS_ID   /*触摸屏名称*/

#define DE_WIFI_DEV_NAME                "wifi43362" /*wifi设备名称*/
#define DE_NAND_NAME                    "/dev/mtd-user" /* nandflash存储段名称*/

#define DE_FLASH_DEV                    "/dev/mtd-user"

#define DE_LED_ID                             1

/* mtd的名字在nandflash的硬件配置(awbl_hwconf_xxx_nandflash.h)里查找 */
#define __MTD_NAME                      "/dev/mtd-user"

/* 从机地址(用户可查看具体I2C从机的配置文件，可根据配置文件修改从机地址) */
#define I2C_SLAVE_ADDR                  0x51


#define DE_BUZZER_PWMID                 PWM6
#endif
