/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "host/class/cdc/serial/usbh_cdc_serial.h"
#include "host/class/cdc/serial/chip/usbh_serial_chip.h"

#define GPRS_INFO_END  {                  \
                            .vid  = 0,    \
                            .pid  = 0,    \
                            .type = "",   \
                            .init = NULL, \
                       }

/* USB转串口设备信息*/
const struct usbh_serial_dev_info __g_userial_dev_tab[] = {
    /* FTDI 串口模块*/
    {
            .vid    = 0x0403,
            .pid    = 0x6001,
            .type   = "FTDI",
            .init   = usbh_serial_ftdi_init,
            .uninit = usbh_serial_ftdi_uninit,
    },
    /* CP210x 串口模块*/
    {
            .vid    = 0x10C4,
            .pid    = 0xEA60,
            .type   = "CP210x",
            .init   = usbh_serial_cp210x_init,
            .uninit = usbh_serial_cp210x_uninit,
    },
    /* CH34x 串口模块*/
    {
            .vid    = 0x4348,
            .pid    = 0x5523,
            .type   = "CH34x",
            .init   = usbh_serial_ch34x_init,
            .uninit = usbh_serial_ch34x_uninit,
    },
    /* CH34x 串口模块*/
    {
            .vid    = 0x1A86,
            .pid    = 0x7523,
            .type   = "CH34x",
            .init   = usbh_serial_ch34x_init,
            .uninit = usbh_serial_ch34x_uninit,
    },
    /* CH34x 串口模块*/
    {
            .vid    = 0x1A86,
            .pid    = 0x5523,
            .type   = "CH34x",
            .init   = usbh_serial_ch34x_init,
            .uninit = usbh_serial_ch34x_uninit,
    },
    /* PL2303 串口模块*/
    {
            .vid    = 0x067B,
            .pid    = 0x2303,
            .type   = "PL2303",
            .init   = usbh_serial_pl2303_init,
            .uninit = usbh_serial_pl2303_uninit,
    },
    /* 移远EC20 4G模块*/
    {
            .vid    = 0x2C7C,
            .pid    = 0x0125,
            .type   = "4G",
            .init   = usbh_serial_other_init,
            .uninit = usbh_serial_other_uninit,
    },
    /* FIBOCOM L610 4G模块*/
    {
            .vid    = 0x1782,
            .pid    = 0x4D10,
            .type   = "4G",
            .init   = usbh_serial_other_init,
            .uninit = usbh_serial_other_uninit,
    },
    GPRS_INFO_END
};
