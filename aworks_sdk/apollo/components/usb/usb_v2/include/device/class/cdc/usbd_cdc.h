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
#ifndef __USBD_CDC__H
#define __USBD_CDC__H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* 通信设备类规范版本1.1.0*/
#define USBD_CDC_V1_10                               0x0110

/* 通讯接口类代码*/
/* (usbcdc11.pdf, 4.2, Table 15) */
#define USBD_CDC_COMMUNICATION_INTERFACE_CLASS       0x02

/* 通讯接口类子类代码*/
/* (usbcdc11.pdf, 4.3, Table 16) */
#define USBD_CDC_DIRECT_LINE_CONTROL_MODEL           0x01
#define USBD_CDC_ABSTRACT_CONTROL_MODEL              0x02
#define USBD_CDC_TELEPHONE_CONTROL_MODEL             0x03
#define USBD_CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04
#define USBD_CDC_CAPI_CONTROL_MODEL                  0x05
#define USBD_CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06
#define USBD_CDC_ATM_NETWORKING_CONTROL_MODEL        0x07

/* Communication interface class control protocol codes */
/* (usbcdc11.pdf, 4.4, Table 17) */
#define USBD_CDC_PROTOCOL_COMMON_AT_COMMANDS         0x01

/* Data interface class code */
/* (usbcdc11.pdf, 4.5, Table 18) */
#define USBD_CDC_DATA_INTERFACE_CLASS                0x0A

/* Data interface class protocol codes */
/* (usbcdc11.pdf, 4.7, Table 19) */
#define USBD_CDC_PROTOCOL_ISDN_BRI                   0x30
#define USBD_CDC_PROTOCOL_HDLC                       0x31
#define USBD_CDC_PROTOCOL_TRANSPARENT                0x32
#define USBD_CDC_PROTOCOL_Q921_MANAGEMENT            0x50
#define USBD_CDC_PROTOCOL_Q921_DATA_LINK             0x51
#define USBD_CDC_PROTOCOL_Q921_MULTIPLEXOR           0x52
#define USBD_CDC_PROTOCOL_V42                        0x90
#define USBD_CDC_PROTOCOL_EURO_ISDN                  0x91
#define USBD_CDC_PROTOCOL_V24_RATE_ADAPTATION        0x92
#define USBD_CDC_PROTOCOL_CAPI                       0x93
#define USBD_CDC_PROTOCOL_HOST_BASED_DRIVER          0xFD
#define USBD_CDC_PROTOCOL_DESCRIBED_IN_PUFD          0xFE

/* 功能描述符的描述符类型字段的类型值*/
/* (usbcdc11.pdf, 5.2.3, Table 24) */
#define USBD_CDC_CS_INTERFACE                        0x24
#define USBD_CDC_CS_ENDPOINT                         0x25

/* 功能描述符的描述符子类型字段的类型值*/
/* (usbcdc11.pdf, 5.2.3, Table 25) */
#define USBD_CDC_HEADER                              0x00
#define USBD_CDC_CALL_MANAGEMENT                     0x01
#define USBD_CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define USBD_CDC_DIRECT_LINE_MANAGEMENT              0x03
#define USBD_CDC_TELEPHONE_RINGER                    0x04
#define USBD_CDC_REPORTING_CAPABILITIES              0x05
#define USBD_CDC_UNION                               0x06
#define USBD_CDC_COUNTRY_SELECTION                   0x07
#define USBD_CDC_TELEPHONE_OPERATIONAL_MODES         0x08
#define USBD_CDC_USB_TERMINAL                        0x09
#define USBD_CDC_NETWORK_CHANNEL                     0x0A
#define USBD_CDC_PROTOCOL_UNIT                       0x0B
#define USBD_CDC_EXTENSION_UNIT                      0x0C
#define USBD_CDC_MULTI_CHANNEL_MANAGEMENT            0x0D
#define USBD_CDC_CAPI_CONTROL_MANAGEMENT             0x0E
#define USBD_CDC_ETHERNET_NETWORKING                 0x0F
#define USBD_CDC_ATM_NETWORKING                      0x10

/* CDC 类特定请求代码*/
/* (usbcdc11.pdf, 6.2, Table 46) */
/* see Table 45 for info about the specific requests. */
#define USBD_CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define USBD_CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define USBD_CDC_SET_COMM_FEATURE                    0x02
#define USBD_CDC_GET_COMM_FEATURE                    0x03
#define USBD_CDC_CLEAR_COMM_FEATURE                  0x04
#define USBD_CDC_SET_AUX_LINE_STATE                  0x10
#define USBD_CDC_SET_HOOK_STATE                      0x11
#define USBD_CDC_PULSE_SETUP                         0x12
#define USBD_CDC_SEND_PULSE                          0x13
#define USBD_CDC_SET_PULSE_TIME                      0x14
#define USBD_CDC_RING_AUX_JACK                       0x15
#define USBD_CDC_SET_LINE_CODING                     0x20
#define USBD_CDC_GET_LINE_CODING                     0x21
#define USBD_CDC_SET_CONTROL_LINE_STATE              0x22
#define USBD_CDC_SEND_BREAK                          0x23
#define USBD_CDC_SET_RINGER_PARMS                    0x30
#define USBD_CDC_GET_RINGER_PARMS                    0x31
#define USBD_CDC_SET_OPERATION_PARMS                 0x32
#define USBD_CDC_GET_OPERATION_PARMS                 0x33
#define USBD_CDC_SET_LINE_PARMS                      0x34
#define USBD_CDC_GET_LINE_PARMS                      0x35
#define USBD_CDC_DIAL_DIGITS                         0x36
#define USBD_CDC_SET_UNIT_PARAMETER                  0x37
#define USBD_CDC_GET_UNIT_PARAMETER                  0x38
#define USBD_CDC_CLEAR_UNIT_PARAMETER                0x39
#define USBD_CDC_GET_PROFILE                         0x3A
#define USBD_CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define USBD_CDC_SET_ETHERNET_PMP_FILTER             0x41
#define USBD_CDC_GET_ETHERNET_PMP_FILTER             0x42
#define USBD_CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define USBD_CDC_GET_ETHERNET_STATISTIC              0x44
#define USBD_CDC_SET_ATM_DATA_FORMAT                 0x50
#define USBD_CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define USBD_CDC_SET_ATM_DEFAULT_VC                  0x52
#define USBD_CDC_GET_ATM_VC_STATISTICS               0x53

/* Communication feature selector codes */
/* (usbcdc11.pdf, 6.2.2..6.2.4, Table 47) */
#define USBD_CDC_ABSTRACT_STATE                      0x01
#define USBD_CDC_COUNTRY_SETTING                     0x02

/* Feature Status returned for ABSTRACT_STATE Selector */
/* (usbcdc11.pdf, 6.2.3, Table 48) */
#define USBD_CDC_IDLE_SETTING                        (1 << 0)
#define USBD_CDC_DATA_MULTPLEXED_STATE               (1 << 1)


/* Control signal bitmap values for the SetControlLineState request */
/* (usbcdc11.pdf, 6.2.14, Table 51) */
#define USBD_CDC_DTE_PRESENT                         (1 << 0)
#define USBD_CDC_ACTIVATE_CARRIER                    (1 << 1)

/* CDC class-specific notification codes */
/* (usbcdc11.pdf, 6.3, Table 68) */
/* see Table 67 for Info about class-specific notifications */
#define USBD_CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define USBD_CDC_RESPONSE_AVAILABLE                  0x01
#define USBD_CDC_AUX_JACK_HOOK_STATE                 0x08
#define USBD_CDC_RING_DETECT                         0x09
#define USBD_CDC_NOTIFICATION_SERIAL_STATE           0x20
#define USBD_CDC_CALL_STATE_CHANGE                   0x28
#define USBD_CDC_LINE_STATE_CHANGE                   0x29
#define USBD_CDC_CONNECTION_SPEED_CHANGE             0x2A

/* UART state bitmap values (Serial state notification). */
/* (usbcdc11.pdf, 6.3.5, Table 69) */
#define USBD_CDC_SERIAL_STATE_OVERRUN                (1 << 6)  /* receive data overrun error has occurred */
#define USBD_CDC_SERIAL_STATE_PARITY                 (1 << 5)  /* parity error has occurred */
#define USBD_CDC_SERIAL_STATE_FRAMING                (1 << 4)  /* framing error has occurred */
#define USBD_CDC_SERIAL_STATE_RING                   (1 << 3)  /* state of ring signal detection */
#define USBD_CDC_SERIAL_STATE_BREAK                  (1 << 2)  /* state of break detection */
#define USBD_CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  /* state of transmission carrier */
#define USBD_CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  /* state of receiver carrier */



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif
