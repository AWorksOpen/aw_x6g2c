
/*
 *  此文件由配置工具生产，请勿修改。
 *  现在作为一个例子。
 */
#undef  AW_IMG_PRJ_BUILD
#define AW_IMG_PRJ_BUILD

#include "apollo.h"
#include "awbus_lite.h"
#include "rtk.h"
#include "aw_task.h"
#include "aw_exc.h"
#include "aw_serial.h"
#include "aw_isr_defer.h"
#include "aw_vdebug.h"

#include "imx6ul_iomuxc_regs.h"
#include "aw_clk.h"
#include "aw_gpio.h"
#include "imx6ul_gpio_pin.h"
#include "aw_prj_params.h"
#include "aw_sdk_version.h"

#include "awbl_driver_if.h"
#include "awbl_bsp_driver_if.h"
#include "aw_imx6ul_caam.h"

#ifdef AW_COM_BLOCK_DEV
#include "fs/aw_blk_dev.h"
#endif  /* AW_COM_BLOCK_DEV */

#ifdef AW_COM_FS_ROOTFS
#include "fs/aw_root_fs.h"
#endif  /* AW_COM_FS_ROOTFS */

#ifdef AW_COM_FS_DUMPFS
#include "fs/aw_dump_fs.h"
#endif  /* AW_COM_FS_DUMPFS */

#ifdef AW_COM_MTD_DEV
#include "mtd/aw_mtd.h"
#endif  /* AW_COM_MTD_DEV */

#ifdef AW_SHELL_LUA
#include "aw_lua_shell_init.h"
#endif  /* AW_SHELL_LUA */

#ifdef AW_COM_TIMESTAMP
#include "driver/timer/awbl_imx6ul_gptimer.h"
#endif  /* AW_COM_TIMESTAMP */

#ifdef AW_DRV_SPI_FLASH
#include "driver/norflash/awbl_spi_flash_cfg.c"
#endif /* AW_DRV_SPI_FLASH */

/* including c source files */
#include "all/aworks_startup.c"
#include "all/aw_psp_startup.c"
#include "all/aw_psp_usr_app_init.c"
#include "core/awbus_lite_cfg.c"

#ifdef AW_COM_INPUT_EV
#include "core/awbus_input_cfg.c"
#endif  /* AW_COM_INPUT_EV */

#ifdef AW_COM_EVENT
#include "util/aw_event_cfg.c"
#endif  /* AW_COM_EVENT */

#ifdef AW_COM_IO_SYSTEM
#include "io/aw_io_cfg.c"
#endif  /* AW_COM_IO_SYSTEM */

#ifdef AW_COM_MOUNT
#include "fs/aw_mount_cfg.c"
#endif  /* AW_COM_MOUNT */

#ifdef AW_COM_FS_RAWFS
#include "fs/aw_rawfs_cfg.c"
#endif  /* AW_COM_FS_RAWFS */

#ifdef AW_COM_FS_FATFS
#include "fs/aw_fatfs_cfg.c"
#endif  /* AW_COM_FS_FATFS */

#ifdef AW_COM_FS_LFFS
#include "fs/aw_lffs_cfg.c"
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_SAFEFS
#include "fs/aw_safefs_cfg.c"
#endif  /* AW_COM_FS_SAFEFS */


#ifdef AW_COM_FS_ROMFS
#include "fs/aw_romfs_cfg.c"
#endif  /* AW_COM_FS_ROMFS */

#ifdef AW_COM_SHELL_SERIAL
#include "aw_shell_serial_cfg.c"
#endif  /* AW_COM_SHELL_SERIAL */

#ifdef AW_COM_SHELL_TELNET
#include "config/aw_telnet_cfg.c"
#endif  /* AW_COM_SHELL_TELNET */

#ifdef AW_COM_FTPD
#include "aw_ftpd_cfg.c"
#endif  /* AW_COM_FTPD */


#ifdef AW_COM_CANOPEN
#if defined(AW_COM_CANOPEN_MST)
#include "mst/config/aw_cop_cfg.c"
#elif defined(AW_COM_CANOPEN_SLV)
#include "slv/config/aw_cop_cfg.c"
#else
#error "use AW_COM_CANOPEN must define AW_COM_CANOPEN_MST or AW_COM_CANOPEN_SLV at project"
#endif
#endif

#include "os/aw_isr_defer_cfg.c"
#include "os/aw_nor_defer_cfg.c"


#ifdef AW_COM_NETWORK
#include "aw_net.h"
#include "ppp/sio_mgn.h"
#endif  /* AW_COM_NETWORK */

#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
#include "host/usb_shell_cfg.h"
#include "awbl_usb_cfg.c"
#endif

#if defined(AW_DRV_IMX6UL_TS) || defined(AW_DRV_GT911_TS)
#include "ts/aw_ts_calc_cfg.c"
#endif

#ifdef AW_COM_LOG
#include "util/aw_loglib_cfg.c"
#endif

#ifdef AW_COM_FTL
#include "ftl/aw_ftl_cfg.c"
#endif

#if defined(AW_COM_NETWORK) && defined(AW_NET_TOOLS)
    #include "aw_net_tools_cfg.h"
#endif  /* AW_NET_TOOLS */

#ifdef AW_COM_FS_YAFFS
#include "fs/aw_yaffs_cfg.c"
#endif  /* AW_COM_FS_YAFFS */

//#include "driver/input/touchscreen/awbl_gt911_ts.h" /*add by CYX*/

#include "awbl_digitron_if.h"
#include "awbl_led.h"

aw_import void aw_io_shell_init (void);

#ifdef AW_COM_CONSOLE
#if AW_CFG_CONSOLE_SHOW_BANNER  != 0
static const char __g_aw_banner[] = {
    "\nApollo for Freescale iMX6UL, Project build " __DATE__ "\n"
};
void aw_console_show_banner(void)
{
    AW_LOGF(("%s", __g_aw_banner));
    AW_LOGF(("AWorks SDK Version is %s  \r\n", aw_sdk_version_get()));
}
#endif  /* AW_CFG_CONSOLE_SHOW_BANNER */
#endif  /* AW_COM_CONSOLE */

static void awbl_group_init( void )
{
    awbus_lite_init();
    awbl_plb_register();
    awbl_imx6ul_gpio_drv_register();
    awbl_imx6ul_intc_drv_register();

    awbl_imx6ul_timer_drv_register();

#ifdef AW_DRV_IMX6UL_GPTIMER
    awbl_imx6ul_gptimer_drv_register();
#endif /* AW_DRV_IMX6UL_GPTIMER */

#ifdef AW_DRV_IMX6UL_ADC
    awbl_imx6ul_adc_drv_register();
#endif /* AW_DRV_IMX6UL_ADC */

#ifdef AW_DRV_IMX6UL_CAN
    awbl_imx6ul_flexcan_drv_register();
#endif /* AW_DRV_IMX6UL_CAN */

#ifdef AW_COM_SERIAL
    extern void aw_serial_init (void);
    aw_serial_init();
#endif  /* AW_COM_SERIAL */


#ifdef AW_DRV_IMX6UL_UART
    awbl_imx6ul_uart_drv_register();
#endif  /* AW_DRV_IMX6UL_UART */

#ifdef AW_DRV_IMX6UL_APBHDMA
    awbl_imx6ul_dma_drv_register();
#endif  /* AW_DRV_IMX6UL_APBHDMA */

#ifdef AW_DRV_IMX6UL_SDMA
    awbl_imx6ul_sdma_drv_register();
#endif /* AW_DRV_IMX6UL_SDMA */

#ifdef AW_DRV_IMX6UL_PWM
    awbl_imx6ul_pwm_drv_register();
#endif  /* AW_DRV_IMX6UL_PWM */

#ifdef AW_COM_DIGITRON_DISP
    awbl_digitron_dev_list_init();
#endif  /* AW_COM_DIGITRON_DISP */

#ifdef AW_DRV_TINY_HMI
    awbl_tiny_hmi_drv_register();
#endif /* AW_DRV_TINY_HMI */

#ifdef AW_DRV_APL_D01
    awbl_apl_d01_drv_register();
#endif /* AW_DRV_APL_D01 */

#ifdef AW_DRV_PANEL_PM1000
    awbl_panel_pm1000_drv_register();
#endif /* AW_DRV_PANEL_PM1000 */

#ifdef AW_DRV_IMX6UL_OCOTP
    awbl_imx6ul_ocotp_register();
#endif /* AW_DEV_IMX6UL_OCOTP */

#ifdef AW_DRV_IMX6UL_TS
    awbl_imx6ul_touch_screen_drv_register();
#endif /* AW_DRV_IMX6UL_TS */

#ifdef AW_DRV_GT911_TS
    extern void awbl_gt911_drv_register(void);
    awbl_gt911_drv_register();
#endif/* AW_DRV_GT911_TS */

#ifdef AW_DRV_ZIGBEE
    awbl_zb_module_drv_register();
#endif /* AW_DRV_ZIGBEE */

#ifdef AW_DRV_IMX6UL_TEMPMON
    awbl_imx6ul_tempmon_drv_register();
#endif /* AW_DRV_IMX6UL_TEMPMON */

#if 0
#ifdef AW_DRV_MXS_HW480272F
    awbl_mxs_hw480272f_drv_register();
#endif /* AW_DRV_MXS_HW480272F */
#endif

/* add by CYX at 2019.1.9*/
#ifdef AW_DRV_TFT_PANEL
    awbl_mxs_tft_panel_drv_register();
#endif

#ifdef AW_DRV_FB
    awbl_mxs_fb_drv_register();
#endif /* AW_DRV_FB */

#ifdef AW_DRV_EXTEND_EP24CXX
    awbl_ep24cxx_drv_register();
#endif /* AW_DRV_EXTEND_EP24CXX */

#ifdef AW_DRV_NS16550_PLB
    awbl_ns16550_plb_drv_register();
#endif /* AW_DRV_NS16550_PLB */

#ifdef AW_DRV_GPIO_KEY
    awbl_gpio_key_drv_register();
#endif /* AW_DRV_GPIO_KEY */

#ifdef AW_DRV_DC_BUZZER
    awbl_dc_buzzer_drv_register();
#endif /* AW_DRV_DC_BUZZER */

#ifdef AW_DRV_PWM_BUZZER
    awbl_pwm_buzzer_drv_register();
#endif /* AW_DRV_PWM_BUZZER */

#ifdef AW_DRV_GPIO_LED
    awbl_led_gpio_drv_register();
#endif  /* AW_DRV_GPIO_LED */

#ifdef AW_COM_I2CBUS
    awbl_i2cbus_init();
#endif /* AW_COM_I2CBUS */

#ifdef AW_DRV_IMX6UL_I2C
    awbl_imx6ul_i2c_drv_register();
#endif /* AW_DRV_IMX6UL_I2C */

#ifdef AW_DRV_GPIO_I2C
    awbl_gpio_i2c_drv_register();
#endif /* AW_DRV_GPIO_I2C */

#ifdef AW_COM_SPIBUS
    awbl_spibus_init();
#endif /* AW_COM_SPIBUS */

#ifdef AW_DRV_IMX6UL_ENET
    awbl_imx6ul_enet_drv_register();
#endif /* AW_DRV_IMX6UL_ENET */

#ifdef AW_COM_MIIBUS
    awbl_miibus_init();
#endif /* AW_COM_MIIBUS */

#ifdef AW_DRV_GENERIC_PHY
    awbl_generic_phy_drv_register();
#endif /* AW_DRV_GENERIC_PHY */

#ifdef AW_DRV_DP83848_PHY
    awbl_dp83848_phy_drv_register();
#endif /* AW_DRV_DP83848_PHY */

#ifdef AW_DRV_IMX6UL_WDT
    awbl_imx6ul_wdt_drv_register();
#endif /* AW_DRV_IMX6UL_WDT */

#ifdef AW_DRV_GPIO_WDT
    awbl_gpio_wdt_drv_register();
#endif /* AW_DRV_GPIO_WDT */

#ifdef AW_DRV_SPI_FLASH
    awbl_spi_flash_cfg();
    awbl_spi_flash_drv_register();
#endif /* AW_DRV_SPI_FLASH */

#ifdef AW_COM_SDIOBUS
    awbl_sdiobus_init();
#endif /* AW_COM_SDIOBUS */

#ifdef AW_DRV_SDIO_SPI
    awbl_sdio_spi_drv_register();
#endif /* AW_DRV_SDIO_SPI */

#ifdef AW_DRV_SDCARD
    awbl_sdcard_drv_register();
#endif /* AW_DRV_SDCARD */

#ifdef AW_DRV_GPIO_SPI
    awbl_gpio_spi_drv_register();
#endif /* AW_DRV_GPIO_SPI */

#ifdef AW_DRV_NANDBUS
    void awbl_nand_bus_init(void);
    awbl_nand_bus_init();
#endif /* AW_DRV_NANDBUS */

#ifdef AW_DRV_IMX6UL_GPMI
    awbl_imx6ul_gpmi_drv_register();
#endif /* AW_DRV_IMX6UL_GPMI */

#ifdef AW_DRV_IMX6UL_NANDFLASH
    void awbl_nand_drv_register(void);
    awbl_nand_drv_register();
#endif /* AW_DRV_IMX6UL_NANDFLASH */

#ifdef AW_DRV_IMX6UL_USDHC
    awbl_imx6ul_sdio_drv_register();
#endif /* AW_DRV_IMX6UL_USDHC */

#ifdef AW_DRV_IMX6UL_ECSPI
    awbl_imx6ul_ecspi_drv_register();
#endif /* AW_DRV_IMX6UL_ECSPI */

#ifdef AW_DRV_IMX6UL_SAI
    awbl_imx6ul_sai_drv_register();
#endif /* AW_DRV_IMX6UL_SAI */

#ifdef AW_DRV_IMX6UL_MQS
    awbl_imx6ul_mqs_drv_register();
#endif /* AW_DRV_IMX6UL_MQS */

#ifdef AW_DRV_EXTEND_PCF85063_0
    awbl_pcf85063_drv_register();
#endif /* AW_DRV_EXTEND_PCF85063_0 */

#ifdef AW_COM_USBH
    extern void awbl_usbh_bus_init (void);
    awbl_usbh_bus_init();
    awbl_usbh_hub_drv_register();
#endif

#ifdef AW_DRV_USBH_MASS_STORAGE
    extern void awbl_usbh_mass_storage_drv_register (int buffer_size);
    awbl_usbh_mass_storage_drv_register(AW_CFG_USB_MS_BUF_SIZE);
#endif /* AW_DRV_USBH_MASS_STORAGE */
#ifdef AW_DRV_USBH_UVC
    extern void awbl_usbh_uvc_drv_register (void);
    awbl_usbh_uvc_drv_register();
#endif
#ifdef AW_DRV_USBH_CDC_SERIAL
    extern void awbl_usbh_cdc_serial_drv_register (void);
    awbl_usbh_cdc_serial_drv_register();
#endif
#ifdef AW_DRV_USBH_AUDIO
    extern void awbl_usbh_uac_drv_register (void);
    awbl_usbh_uac_drv_register();
#endif

#ifdef AW_DRV_IMX6UL_USB_HOST
    awbl_imx6ul_usbh_drv_register();
#endif /* AW_DRV_IMX6UL_USB_HOST */

#ifdef AW_DRV_IMX6UL_USB_DEVICE
    awbl_imx6ul_usbd_drv_register();
#endif /* AW_DRV_IMX6UL_USB_DEVICE */

#ifdef AW_DRV_IMX6UL_SIM
    awbl_imx6ul_sim_drv_register();
#endif /* AW_DRV_IMX6UL_SIM */

#ifdef AW_DRV_IMX6UL_QSPI
    awbl_imx6ul_qspi_drv_register();
#endif /* AW_DRV_IMX6UL_QSPI */

#ifdef AW_DRV_IMX6UL_CAAM
    awbl_imx6ul_caam_drv_register();
#endif /* AW_DRV_IMX6UL_CAAM */

#ifdef AW_DRV_ZLG600A_UART
    awbl_zlg600a_uart_drv_register();
#endif /* AW_DRV_ZLG600A_UART */

#ifdef AW_DRV_ZLG600A_I2C
    awbl_zlg600a_i2c_drv_register();
#endif /* AW_DRV_ZLG600A_I2C */

#ifdef AW_DRV_ZIGBEE
    awbl_zb_module_drv_register();
#endif /* AW_DRV_ZIGBEE */

#ifdef AW_DRV_OV7725
    awbl_ov7725_drv_register();
#endif

#ifdef AW_DRV_GC0308
    awbl_gc0308_drv_register();
#endif

#ifdef AW_DRV_ADV7280
    awbl_adv7280_drv_register();
#endif

#ifdef AW_DRV_IMX6UL_CSI
    awbl_imx6ul_csi_drv_register ();
#endif

#ifdef AW_DRV_USBH_WIRELESS
//    awbl_usbh_wireless_drv_register();
#endif

    void awbl_gprs_default_drv_register (void);
    awbl_gprs_default_drv_register();

#ifdef AW_DRV_GPRS_SIM800
    awbl_gprs_sim800_drv_register();
#endif

#ifdef AW_DRV_GPRS_ME909S
    awbl_gprs_me909s_drv_register();
#endif

#ifdef AW_DEV_GPRS_U9300C
    awbl_gprs_u9300c_drv_register();
#endif

#ifdef AW_DEV_GPRS_EC20
    extern void awbl_gprs_ec20_drv_register(void);
    awbl_gprs_ec20_drv_register();
#endif

#ifdef AW_COM_EIMBUS
    awbl_eim_bus_init();
#endif

#ifdef AW_DRV_IMX6UL_EIM
    awbl_imx6ul_eim_drv_register();
#endif

#ifdef AW_DRV_SJA1000
    awbl_sja1000_can_drv_register();
#endif

#ifdef AW_DRV_UBLOX
    awbl_ublox_drv_register();
#endif

#ifdef AW_DRV_GNSS_GPRS
    awbl_gnss_gprs_drv_register();
#endif

#ifdef AW_DRV_FM175XX
    awbl_fm175xx_drv_register();
#endif

    extern void awbl_touch_nvram_drv_register (void);
    awbl_touch_nvram_drv_register();

#ifdef AW_DRV_IMX6UL_TIMESTAMP
    awbl_imx6ul_timestamp_drv_register();
#endif

#ifdef AW_DRV_AWBL_CYW43362_WIFI_SDIO
   extern void awbl_cyw43362_sdio_drv_register (void);
   awbl_cyw43362_sdio_drv_register();
#endif

}


void aw_prj_early_init( void )
{

    awbl_group_init();

    awbl_intctlr_init();    /* AWBus IntCtlr service up */

    /* AWBus init phase 1 */
    awbl_dev_init1();

    awbl_gpio_init();       /* AWBus GPIO service up */

#ifdef AW_COM_LED
    awbl_led_init();        /* AWBus LED service up */
#endif

#ifdef AW_COM_PWM
    awbl_pwm_init();        /* AWBus PWM service up */
#endif /* AW_COM_PWM */

#ifdef AW_DEV_IMX6UL_SDMA
    awbl_imx6ul_sdma_init();
#endif

#ifdef AW_DEV_CAMERA
    aw_camera_serve_init();
#endif

#ifdef AW_COM_ADC
    awbl_adc_init();
#endif  /* AW_COM_ADC */

}

void aw_prj_task_level_init( void )
{
/*
 * Initialize serial I/O for kernel output. after this call, kprintf()
 * is ready for use
 */
#ifdef AW_COM_CONSOLE
    aw_koutput_sio_init(AW_CFG_CONSOLE_COMID, AW_CFG_CONSOLE_BAUD_RATE);
    aw_serial_ioctl(AW_CFG_CONSOLE_COMID, SIO_MODE_SET, (void *)SIO_MODE_INT);
#if AW_CFG_CONSOLE_SHOW_BANNER != 0
    aw_console_show_banner();
#endif  /* AW_CFG_CONSOLE_SHOW_BANNER */
#endif  /* AW_COM_CONSOLE */

    /* isr deferral library initialization */
    aw_isr_defer_lib_init();
    aw_nor_defer_lib_init();

#ifdef AW_COM_WDT
    awbl_wdt_init();            /* AWBus WDT service up */
#endif  /* AW_COM_WDT */

#ifdef AW_COM_EVENT
    aw_event_lib_init();        /* event lib initialization */
#endif  /* AW_COM_EVENT */

#ifdef AW_COM_IO_SYSTEM
    aw_io_lib_init();
#endif  /* AW_COM_IO_SYSTEM */

#ifdef AW_COM_BLOCK_DEV
    aw_blk_dev_lib_init(AW_BLOCK_DEV_EVT_CNT);    /* block device lib initialization */
#endif  /* AW_COM_BLOCK_DEV */

#ifdef AW_COM_MTD_DEV
    aw_mtd_lib_init();
#endif  /* AW_COM_MTD_DEV */

#ifdef AW_COM_FS_ROOTFS
    aw_root_fs_init();
#endif  /* AW_COM_FS_ROOTFS */

#ifdef AW_COM_FS_DUMPFS
    aw_dump_fs_init();
#endif  /* AW_COM_FS_DUMPFS */

#ifdef AW_COM_FS_RAWFS
    aw_rawfs_lib_init();
#endif  /* AW_COM_FS_RAWFS */

#ifdef AW_COM_FS_FATFS
    aw_fatfs_lib_init();
#endif  /* AW_COM_FS_FATFS */

#ifdef AW_COM_FS_LFFS
    aw_lffs_lib_init();
#endif  /* AW_COM_FS_LFFS */

#ifdef AW_COM_FS_SAFEFS
    aw_safefs_lib_init();
#endif  /* AW_COM_FS_SAFEFS */

#ifdef AW_COM_FS_YAFFS
    aw_yaffs_lib_init();
#endif  /* AW_COM_FS_YAFFS */

#ifdef AW_COM_FS_ROMFS
    aw_romfs_lib_init();
#endif  /* AW_COM_FS_ROMFS */

#ifdef AW_COM_MOUNT
    aw_mount_lib_init();
//   (void)aw_mount_table_add("/sd", "/dev/sd0", "vfat", 0);
#endif  /* AW_COM_MOUNT */

#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
    awbl_usb_cfg();
#endif

    /* system tick initialization */
    systick_init(AW_CFG_TICKS_PER_SECOND);

#ifdef AW_DRV_IMX6UL_NANDFLASH
    void awbl_imx6ul_nand_init(void);
    awbl_imx6ul_nand_init();
#endif /* AW_DRV_IMX6UL_NANDFLASH */

    /* AWBus init phase 2 */
    awbl_dev_init2();

#ifdef AW_COM_TIMESTAMP
    awbl_timestamp_lib_init(AW_TIMESTAMP_NAME, AW_TIMESTAMP_BUSID, 0); /* timestamp service up */
#endif  /* AW_COM_TIMESTAMP */

#ifdef AW_COM_RTC
    awbl_rtc_init(); /* AWBus RTC service up */
#endif  /* AW_COM_RTC */

    /* system time initialization */
    aw_time_lib_init(aw_sys_clkrate_get(),0);

#ifdef AW_COM_SHELL_SERIAL
    aw_serial_shell_cfg();
#endif  /* AW_COM_SHELL_SERIAL */

#ifdef AW_SHELL_IO
    aw_io_shell_init();
#endif  /* AW_SHELL_IO */

#ifdef AW_COM_LOG
    aw_loglib_cfg_init();
#endif

#ifdef AW_COM_FTL
    aw_ftl_lib_init();
#endif

#ifdef AW_COM_NETWORK
    aw_net_init();

#ifdef AW_COM_CANOPEN
    aw_cop_lib_init();
#endif

#if defined(AW_COM_SHELL_SERIAL) || defined(AW_COM_SHELL_TELNET)
#ifdef AW_NET_TOOLS
    aw_net_tools_register();
#endif
#endif
#endif  /* AW_COM_NETWORK */

#ifdef AW_COM_SHELL_TELNET
    aw_telnet_shell_cfg();
#endif  /* AW_COM_SHELL_TELNET */

#ifdef AW_SHELL_LUA
    aw_lua_shell_init();
#endif
#ifdef AW_SHELL_LUAC
    aw_luac_shell_init();
#endif  /* AW_SHELL_LUA */

#ifdef AW_COM_GPRS
    awbl_gprs_init();
    sio_list_init();
#endif

#ifdef AW_COM_GNSS
    awbl_gnss_init();
#endif

    /* AWBus init phase 3 */
    awbl_dev_connect();

#ifdef AW_COM_INPUT_EV
    awbl_input_ev_mgr_register();
#endif  /* AW_COM_INPUT_EV */

#ifdef AW_COM_SIM
    awbl_sim_init();
#endif  /* AW_COM_SIM */

#ifdef AW_COM_ZIGBEE
    awbl_zigbee_init();
#endif  /* AW_COM_ZIGBEE */

#ifdef AW_DEV_IMX6UL_CAAM
    aw_imx6ul_caam_init();
#endif

#if (defined AW_DEV_ZLG600A_I2C_0) || \
    (defined AW_DEV_ZLG600A_UART_0)
    awbl_zlg600a_init();
#endif

    aw_cplusplus_runtime_init();

    usb_shell_register();
}


/* end of file */
