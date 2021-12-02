
#ifndef __AW_DEMO_CONFIG_H
#define __AW_DEMO_CONFIG_H

#include "aw_prj_params.h"
#include "aw_gpio.h"
#include "aw_cpu_clk.h"
#include "imx6ul_pwmid.h"
#include "driver/timer/awbl_imx6ul_gptimer.h"

/*����modbus����id*/
#define DE_MODBUS_UART_ID               COM2
/* ADC����ͨ��*/
#define DE_ADC_CH                       8
/* CAN����ͨ��*/
#define DE_CAN_CH                       0
/* ����ͷ�����豸����*/
#define DE_CAMERA_NAME                  "ov7725"
/* ֡�����豸����*/
#define DE_CAMEAR_EMWIN_FB              "mxs_emwin_fb"
/* eeprom�豸����*/
#define DE_EEPROM_NAME                  "ep24cxx"
/* PWM���� ID���� */
#define DE_PWMID                        PWM6
/* eth0�豸����*/
#define DE_ETH0_NAME                    "eth0"
/* LED ���ź�*/
#define DM_GPIO_LED                     GPIO4_16
/* GPIO INT */
#define DM_GPIO_TRG_OUTPUT              GPIO3_25   /* GPIO�ж����� ���Ŷ��� */
#define DM_GPIO_TRG_INTR                GPIO3_26
/* GPIO�豸��*/
#define DM_LED                          1

/* GPT ����*/
#define DE_HWTIMER_TIMER_NAME           AWBL_IMX6UL_GPT_DRV_NAME      /* gpt��ʱ������ */
#define DE_HWTIMER_TIMER_ID             IMX6UL_GPT1_BUSID             /* gpt1 ID�� */

//#define DE_QTIMER_NAME                  AWBL_IMX6UL_GPT_DRV_NAME          /* qtimer��ʱ������ */
//#define DE_QTIMER_ID                    IMX6UL_GPT1_BUSID   /* qtimer3 ID�� */

#define DE_TIMER_CAPID                  IMX6UL_GPT1_BUSID  /* ��ʱ�����벶��ID */
#define DE_TIMER_CAPID_GPIO             GPIO1_20                  /*QTIMER3_CAP3ͨ����Ӧ������*/

#define DE_GPIO_PWM                     GPIO3_26                      /* GPIOģ�����PWM����  */
#define DE_HWTIMER_INPUT_CAP_IO         0                             /* GPT1���벶���� IO ���� */
#define DE_CLK_GPT                      IMX6UL_CLK_PERCLK             /* GPT��ʱ��ʱ��ID */
/* �����豸����*/
#define DE_ENET_NAME                    "eth0"
/* i2c����i2c ID*/
#define DE_I2C_ID                       4
/* �������豸����*/
#define DE_TS_NAME                      SYS_TS_ID
/* nand disk����*/
#define DE_DISK_NAME                    "/dev/mtd-user"
/* nand fs����*/
#define DE_NAMD_FS_NAME                 "lffs"
/* gprs ID*/
#define DE_GPRS_DEV_ID                  2
/* rtc ID*/
#define DE_RTC_ID0                      RTC_ID0
#define DE_RTC_ID1                      RTC_ID1
/* SD���豸����*/
#define DE_SD_DEV_NAME                  "/dev/sd0"
/* SPI����ID*/
#define DE_SPI_DEV_BUSID                0
/* SPIƬѡ����*/
#define DE_SPI_CS_PIN                   GPIO1_28
/* SPI Flash����*/
#define DE_SPI_FLASH                    "/sflash1"
#define DE_SPIFLASH_NVRAM_NAME          "spi_flash1_nvram"

/* USB���豸����*/
#define DE_USB_BLK_NAME                 "/dev/h1-d1-0"
#define DE_USB_DEV_NAME                 "/dev/usbd0"

/* nand flash�豸����*/
#define DE_NANDFLASH_DEV_NAME           AWBL_NAND_DRV_NAME
#define DE_NAND_FLASH_DEV_ID            1

/* otp�洢�����ƺ͵�Ԫ�ţ���awbl_hwconf_imx6ul_ocotp.h �в鿴�޸�  */
#define DE_OTP_MEM                      "otp_osp"

#define DE_FB                           "mxs_fb"    /*��ʾ������*/
#define DE_TFT_TS_NAME                  SYS_TS_ID   /*����������*/

#define DE_WIFI_DEV_NAME                "wifi43362" /*wifi�豸����*/
#define DE_NAND_NAME                    "/dev/mtd-user" /* nandflash�洢������*/

#define DE_FLASH_DEV                    "/dev/mtd-user"

#define DE_LED_ID                             1

/* mtd��������nandflash��Ӳ������(awbl_hwconf_xxx_nandflash.h)����� */
#define __MTD_NAME                      "/dev/mtd-user"

/* �ӻ���ַ(�û��ɲ鿴����I2C�ӻ��������ļ����ɸ��������ļ��޸Ĵӻ���ַ) */
#define I2C_SLAVE_ADDR                  0x51


#define DE_BUZZER_PWMID                 PWM6
#endif
