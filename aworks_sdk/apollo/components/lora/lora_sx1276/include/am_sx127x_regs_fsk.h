/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SX1276 FSK modem registers and bits definitions
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-25  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_SX127X_REGS_FSK_H
#define __AM_SX127X_REGS_FSK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup am_if_sx1276_regs_fsk
 * @copydoc am_sx1276_regs_fsk.h
 * @{
 */

/**
 * \brief SX1276 Internal registers Address
 */
#define AM_SX127X_FSK_REG_FIFO                              0x00
/* Common settings */
#define AM_SX127X_FSK_REG_OPMODE                            0x01
#define AM_SX127X_FSK_REG_BITRATEMSB                        0x02
#define AM_SX127X_FSK_REG_BITRATELSB                        0x03
#define AM_SX127X_FSK_REG_FDEVMSB                           0x04
#define AM_SX127X_FSK_REG_FDEVLSB                           0x05
#define AM_SX127X_FSK_REG_FRFMSB                            0x06
#define AM_SX127X_FSK_REG_FRFMID                            0x07
#define AM_SX127X_FSK_REG_FRFLSB                            0x08
/* Tx settings */
#define AM_SX127X_FSK_REG_PACONFIG                          0x09
#define AM_SX127X_FSK_REG_PARAMP                            0x0A
#define AM_SX127X_FSK_REG_OCP                               0x0B
/* Rx settings */
#define AM_SX127X_FSK_REG_LNA                               0x0C
#define AM_SX127X_FSK_REG_RXCONFIG                          0x0D
#define AM_SX127X_FSK_REG_RSSICONFIG                        0x0E
#define AM_SX127X_FSK_REG_RSSICOLLISION                     0x0F
#define AM_SX127X_FSK_REG_RSSITHRESH                        0x10
#define AM_SX127X_FSK_REG_RSSIVALUE                         0x11
#define AM_SX127X_FSK_REG_RXBW                              0x12
#define AM_SX127X_FSK_REG_AFCBW                             0x13
#define AM_SX127X_FSK_REG_OOKPEAK                           0x14
#define AM_SX127X_FSK_REG_OOKFIX                            0x15
#define AM_SX127X_FSK_REG_OOKAVG                            0x16
#define AM_SX127X_FSK_REG_RES17                             0x17
#define AM_SX127X_FSK_REG_RES18                             0x18
#define AM_SX127X_FSK_REG_RES19                             0x19
#define AM_SX127X_FSK_REG_AFCFEI                            0x1A
#define AM_SX127X_FSK_REG_AFCMSB                            0x1B
#define AM_SX127X_FSK_REG_AFCLSB                            0x1C
#define AM_SX127X_FSK_REG_FEIMSB                            0x1D
#define AM_SX127X_FSK_REG_FEILSB                            0x1E
#define AM_SX127X_FSK_REG_PREAMBLEDETECT                    0x1F
#define AM_SX127X_FSK_REG_RXTIMEOUT1                        0x20
#define AM_SX127X_FSK_REG_RXTIMEOUT2                        0x21
#define AM_SX127X_FSK_REG_RXTIMEOUT3                        0x22
#define AM_SX127X_FSK_REG_RXDELAY                           0x23
/* Oscillator settings */
#define AM_SX127X_FSK_REG_OSC                               0x24
/* Packet handler settings */
#define AM_SX127X_FSK_REG_PREAMBLEMSB                       0x25
#define AM_SX127X_FSK_REG_PREAMBLELSB                       0x26
#define AM_SX127X_FSK_REG_SYNCCONFIG                        0x27
#define AM_SX127X_FSK_REG_SYNCVALUE1                        0x28
#define AM_SX127X_FSK_REG_SYNCVALUE2                        0x29
#define AM_SX127X_FSK_REG_SYNCVALUE3                        0x2A
#define AM_SX127X_FSK_REG_SYNCVALUE4                        0x2B
#define AM_SX127X_FSK_REG_SYNCVALUE5                        0x2C
#define AM_SX127X_FSK_REG_SYNCVALUE6                        0x2D
#define AM_SX127X_FSK_REG_SYNCVALUE7                        0x2E
#define AM_SX127X_FSK_REG_SYNCVALUE8                        0x2F
#define AM_SX127X_FSK_REG_PACKETCONFIG1                     0x30
#define AM_SX127X_FSK_REG_PACKETCONFIG2                     0x31
#define AM_SX127X_FSK_REG_PAYLOADLENGTH                     0x32
#define AM_SX127X_FSK_REG_NODEADRS                          0x33
#define AM_SX127X_FSK_REG_BROADCASTADRS                     0x34
#define AM_SX127X_FSK_REG_FIFOTHRESH                        0x35
/* SM settings */
#define AM_SX127X_FSK_REG_SEQCONFIG1                        0x36
#define AM_SX127X_FSK_REG_SEQCONFIG2                        0x37
#define AM_SX127X_FSK_REG_TIMERRESOL                        0x38
#define AM_SX127X_FSK_REG_TIMER1COEF                        0x39
#define AM_SX127X_FSK_REG_TIMER2COEF                        0x3A
/* Service settings */
#define AM_SX127X_FSK_REG_IMAGECAL                          0x3B
#define AM_SX127X_FSK_REG_TEMP                              0x3C
#define AM_SX127X_FSK_REG_LOWBAT                            0x3D
/* Status */
#define AM_SX127X_FSK_REG_IRQFLAGS1                         0x3E
#define AM_SX127X_FSK_REG_IRQFLAGS2                         0x3F
/* I/O settings */
#define AM_SX127X_FSK_REG_DIOMAPPING1                       0x40
#define AM_SX127X_FSK_REG_DIOMAPPING2                       0x41
/* Version */
#define AM_SX127X_FSK_REG_VERSION                           0x42
/* Additional settings */
#define AM_SX127X_FSK_REG_PLLHOP                            0x44
#define AM_SX127X_FSK_REG_TCXO                              0x4B
#define AM_SX127X_FSK_REG_PADAC                             0x4D
#define AM_SX127X_FSK_REG_FORMERTEMP                        0x5B
#define AM_SX127X_FSK_REG_BITRATEFRAC                       0x5D
#define AM_SX127X_FSK_REG_AGCREF                            0x61
#define AM_SX127X_FSK_REG_AGCTHRESH1                        0x62
#define AM_SX127X_FSK_REG_AGCTHRESH2                        0x63
#define AM_SX127X_FSK_REG_AGCTHRESH3                        0x64
#define AM_SX127X_FSK_REG_PLL                               0x70

/**
 * \brief SX1276 FSK bits control definition
 */

/**
 * \brief RegFifo
 */

/**
 * \brief RegOpMode
 */
#define AM_SX127X_FSK_RF_OPMODE_LONGRANGEMODE_MASK          0x7F
#define AM_SX127X_FSK_RF_OPMODE_LONGRANGEMODE_OFF           0x00
#define AM_SX127X_FSK_RF_OPMODE_LONGRANGEMODE_ON            0x80

#define AM_SX127X_FSK_RF_OPMODE_MODULATIONTYPE_MASK         0x9F
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONTYPE_FSK          0x00  /* Default */
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONTYPE_OOK          0x20

#define AM_SX127X_FSK_RF_OPMODE_MODULATIONSHAPING_MASK      0xE7
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONSHAPING_00        0x00  /* Default */
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONSHAPING_01        0x08
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONSHAPING_10        0x10
#define AM_SX127X_FSK_RF_OPMODE_MODULATIONSHAPING_11        0x18

#define AM_SX127X_FSK_RF_OPMODE_MASK                        0xF8
#define AM_SX127X_FSK_RF_OPMODE_SLEEP                       0x00
#define AM_SX127X_FSK_RF_OPMODE_STANDBY                     0x01  /* Default */
#define AM_SX127X_FSK_RF_OPMODE_SYNTHESIZER_TX              0x02
#define AM_SX127X_FSK_RF_OPMODE_TRANSMITTER                 0x03
#define AM_SX127X_FSK_RF_OPMODE_SYNTHESIZER_RX              0x04
#define AM_SX127X_FSK_RF_OPMODE_RECEIVER                    0x05

/**
 * \brief RegBitRate (bits/sec)
 */
#define AM_SX127X_FSK_RF_BITRATEMSB_1200_BPS                0x68
#define AM_SX127X_FSK_RF_BITRATELSB_1200_BPS                0x2B
#define AM_SX127X_FSK_RF_BITRATEMSB_2400_BPS                0x34
#define AM_SX127X_FSK_RF_BITRATELSB_2400_BPS                0x15
#define AM_SX127X_FSK_RF_BITRATEMSB_4800_BPS                0x1A  /* Default */
#define AM_SX127X_FSK_RF_BITRATELSB_4800_BPS                0x0B  /* Default */
#define AM_SX127X_FSK_RF_BITRATEMSB_9600_BPS                0x0D
#define AM_SX127X_FSK_RF_BITRATELSB_9600_BPS                0x05
#define AM_SX127X_FSK_RF_BITRATEMSB_15000_BPS               0x08
#define AM_SX127X_FSK_RF_BITRATELSB_15000_BPS               0x55
#define AM_SX127X_FSK_RF_BITRATEMSB_19200_BPS               0x06
#define AM_SX127X_FSK_RF_BITRATELSB_19200_BPS               0x83
#define AM_SX127X_FSK_RF_BITRATEMSB_38400_BPS               0x03
#define AM_SX127X_FSK_RF_BITRATELSB_38400_BPS               0x41
#define AM_SX127X_FSK_RF_BITRATEMSB_76800_BPS               0x01
#define AM_SX127X_FSK_RF_BITRATELSB_76800_BPS               0xA1
#define AM_SX127X_FSK_RF_BITRATEMSB_153600_BPS              0x00
#define AM_SX127X_FSK_RF_BITRATELSB_153600_BPS              0xD0
#define AM_SX127X_FSK_RF_BITRATEMSB_57600_BPS               0x02
#define AM_SX127X_FSK_RF_BITRATELSB_57600_BPS               0x2C
#define AM_SX127X_FSK_RF_BITRATEMSB_115200_BPS              0x01
#define AM_SX127X_FSK_RF_BITRATELSB_115200_BPS              0x16
#define AM_SX127X_FSK_RF_BITRATEMSB_12500_BPS               0x0A
#define AM_SX127X_FSK_RF_BITRATELSB_12500_BPS               0x00
#define AM_SX127X_FSK_RF_BITRATEMSB_25000_BPS               0x05
#define AM_SX127X_FSK_RF_BITRATELSB_25000_BPS               0x00
#define AM_SX127X_FSK_RF_BITRATEMSB_50000_BPS               0x02
#define AM_SX127X_FSK_RF_BITRATELSB_50000_BPS               0x80
#define AM_SX127X_FSK_RF_BITRATEMSB_100000_BPS              0x01
#define AM_SX127X_FSK_RF_BITRATELSB_100000_BPS              0x40
#define AM_SX127X_FSK_RF_BITRATEMSB_150000_BPS              0x00
#define AM_SX127X_FSK_RF_BITRATELSB_150000_BPS              0xD5
#define AM_SX127X_FSK_RF_BITRATEMSB_200000_BPS              0x00
#define AM_SX127X_FSK_RF_BITRATELSB_200000_BPS              0xA0
#define AM_SX127X_FSK_RF_BITRATEMSB_250000_BPS              0x00
#define AM_SX127X_FSK_RF_BITRATELSB_250000_BPS              0x80
#define AM_SX127X_FSK_RF_BITRATEMSB_32768_BPS               0x03
#define AM_SX127X_FSK_RF_BITRATELSB_32768_BPS               0xD1

/**
 * \brief RegFdev (Hz)
 */
#define AM_SX127X_FSK_RF_FDEVMSB_2000_HZ                    0x00
#define AM_SX127X_FSK_RF_FDEVLSB_2000_HZ                    0x21
#define AM_SX127X_FSK_RF_FDEVMSB_5000_HZ                    0x00  /* Default */
#define AM_SX127X_FSK_RF_FDEVLSB_5000_HZ                    0x52  /* Default */
#define AM_SX127X_FSK_RF_FDEVMSB_10000_HZ                   0x00
#define AM_SX127X_FSK_RF_FDEVLSB_10000_HZ                   0xA4
#define AM_SX127X_FSK_RF_FDEVMSB_15000_HZ                   0x00
#define AM_SX127X_FSK_RF_FDEVLSB_15000_HZ                   0xF6
#define AM_SX127X_FSK_RF_FDEVMSB_20000_HZ                   0x01
#define AM_SX127X_FSK_RF_FDEVLSB_20000_HZ                   0x48
#define AM_SX127X_FSK_RF_FDEVMSB_25000_HZ                   0x01
#define AM_SX127X_FSK_RF_FDEVLSB_25000_HZ                   0x9A
#define AM_SX127X_FSK_RF_FDEVMSB_30000_HZ                   0x01
#define AM_SX127X_FSK_RF_FDEVLSB_30000_HZ                   0xEC
#define AM_SX127X_FSK_RF_FDEVMSB_35000_HZ                   0x02
#define AM_SX127X_FSK_RF_FDEVLSB_35000_HZ                   0x3D
#define AM_SX127X_FSK_RF_FDEVMSB_40000_HZ                   0x02
#define AM_SX127X_FSK_RF_FDEVLSB_40000_HZ                   0x8F
#define AM_SX127X_FSK_RF_FDEVMSB_45000_HZ                   0x02
#define AM_SX127X_FSK_RF_FDEVLSB_45000_HZ                   0xE1
#define AM_SX127X_FSK_RF_FDEVMSB_50000_HZ                   0x03
#define AM_SX127X_FSK_RF_FDEVLSB_50000_HZ                   0x33
#define AM_SX127X_FSK_RF_FDEVMSB_55000_HZ                   0x03
#define AM_SX127X_FSK_RF_FDEVLSB_55000_HZ                   0x85
#define AM_SX127X_FSK_RF_FDEVMSB_60000_HZ                   0x03
#define AM_SX127X_FSK_RF_FDEVLSB_60000_HZ                   0xD7
#define AM_SX127X_FSK_RF_FDEVMSB_65000_HZ                   0x04
#define AM_SX127X_FSK_RF_FDEVLSB_65000_HZ                   0x29
#define AM_SX127X_FSK_RF_FDEVMSB_70000_HZ                   0x04
#define AM_SX127X_FSK_RF_FDEVLSB_70000_HZ                   0x7B
#define AM_SX127X_FSK_RF_FDEVMSB_75000_HZ                   0x04
#define AM_SX127X_FSK_RF_FDEVLSB_75000_HZ                   0xCD
#define AM_SX127X_FSK_RF_FDEVMSB_80000_HZ                   0x05
#define AM_SX127X_FSK_RF_FDEVLSB_80000_HZ                   0x1F
#define AM_SX127X_FSK_RF_FDEVMSB_85000_HZ                   0x05
#define AM_SX127X_FSK_RF_FDEVLSB_85000_HZ                   0x71
#define AM_SX127X_FSK_RF_FDEVMSB_90000_HZ                   0x05
#define AM_SX127X_FSK_RF_FDEVLSB_90000_HZ                   0xC3
#define AM_SX127X_FSK_RF_FDEVMSB_95000_HZ                   0x06
#define AM_SX127X_FSK_RF_FDEVLSB_95000_HZ                   0x14
#define AM_SX127X_FSK_RF_FDEVMSB_100000_HZ                  0x06
#define AM_SX127X_FSK_RF_FDEVLSB_100000_HZ                  0x66
#define AM_SX127X_FSK_RF_FDEVMSB_110000_HZ                  0x07
#define AM_SX127X_FSK_RF_FDEVLSB_110000_HZ                  0x0A
#define AM_SX127X_FSK_RF_FDEVMSB_120000_HZ                  0x07
#define AM_SX127X_FSK_RF_FDEVLSB_120000_HZ                  0xAE
#define AM_SX127X_FSK_RF_FDEVMSB_130000_HZ                  0x08
#define AM_SX127X_FSK_RF_FDEVLSB_130000_HZ                  0x52
#define AM_SX127X_FSK_RF_FDEVMSB_140000_HZ                  0x08
#define AM_SX127X_FSK_RF_FDEVLSB_140000_HZ                  0xF6
#define AM_SX127X_FSK_RF_FDEVMSB_150000_HZ                  0x09
#define AM_SX127X_FSK_RF_FDEVLSB_150000_HZ                  0x9A
#define AM_SX127X_FSK_RF_FDEVMSB_160000_HZ                  0x0A
#define AM_SX127X_FSK_RF_FDEVLSB_160000_HZ                  0x3D
#define AM_SX127X_FSK_RF_FDEVMSB_170000_HZ                  0x0A
#define AM_SX127X_FSK_RF_FDEVLSB_170000_HZ                  0xE1
#define AM_SX127X_FSK_RF_FDEVMSB_180000_HZ                  0x0B
#define AM_SX127X_FSK_RF_FDEVLSB_180000_HZ                  0x85
#define AM_SX127X_FSK_RF_FDEVMSB_190000_HZ                  0x0C
#define AM_SX127X_FSK_RF_FDEVLSB_190000_HZ                  0x29
#define AM_SX127X_FSK_RF_FDEVMSB_200000_HZ                  0x0C
#define AM_SX127X_FSK_RF_FDEVLSB_200000_HZ                  0xCD

/**
 * \brief RegFrf (MHz)
 */
#define AM_SX127X_FSK_RF_FRFMSB_863_MHZ                     0xD7
#define AM_SX127X_FSK_RF_FRFMID_863_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_863_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_864_MHZ                     0xD8
#define AM_SX127X_FSK_RF_FRFMID_864_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_864_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_865_MHZ                     0xD8
#define AM_SX127X_FSK_RF_FRFMID_865_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_865_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_866_MHZ                     0xD8
#define AM_SX127X_FSK_RF_FRFMID_866_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_866_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_867_MHZ                     0xD8
#define AM_SX127X_FSK_RF_FRFMID_867_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_867_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_868_MHZ                     0xD9
#define AM_SX127X_FSK_RF_FRFMID_868_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_868_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_869_MHZ                     0xD9
#define AM_SX127X_FSK_RF_FRFMID_869_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_869_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_870_MHZ                     0xD9
#define AM_SX127X_FSK_RF_FRFMID_870_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_870_MHZ                     0x00

#define AM_SX127X_FSK_RF_FRFMSB_902_MHZ                     0xE1
#define AM_SX127X_FSK_RF_FRFMID_902_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_902_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_903_MHZ                     0xE1
#define AM_SX127X_FSK_RF_FRFMID_903_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_903_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_904_MHZ                     0xE2
#define AM_SX127X_FSK_RF_FRFMID_904_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_904_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_905_MHZ                     0xE2
#define AM_SX127X_FSK_RF_FRFMID_905_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_905_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_906_MHZ                     0xE2
#define AM_SX127X_FSK_RF_FRFMID_906_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_906_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_907_MHZ                     0xE2
#define AM_SX127X_FSK_RF_FRFMID_907_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_907_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_908_MHZ                     0xE3
#define AM_SX127X_FSK_RF_FRFMID_908_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_908_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_909_MHZ                     0xE3
#define AM_SX127X_FSK_RF_FRFMID_909_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_909_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_910_MHZ                     0xE3
#define AM_SX127X_FSK_RF_FRFMID_910_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_910_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_911_MHZ                     0xE3
#define AM_SX127X_FSK_RF_FRFMID_911_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_911_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_912_MHZ                     0xE4
#define AM_SX127X_FSK_RF_FRFMID_912_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_912_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_913_MHZ                     0xE4
#define AM_SX127X_FSK_RF_FRFMID_913_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_913_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_914_MHZ                     0xE4
#define AM_SX127X_FSK_RF_FRFMID_914_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_914_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_915_MHZ                     0xE4  /* Default */
#define AM_SX127X_FSK_RF_FRFMID_915_MHZ                     0xC0  /* Default */
#define AM_SX127X_FSK_RF_FRFLSB_915_MHZ                     0x00  /* Default */
#define AM_SX127X_FSK_RF_FRFMSB_916_MHZ                     0xE5
#define AM_SX127X_FSK_RF_FRFMID_916_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_916_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_917_MHZ                     0xE5
#define AM_SX127X_FSK_RF_FRFMID_917_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_917_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_918_MHZ                     0xE5
#define AM_SX127X_FSK_RF_FRFMID_918_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_918_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_919_MHZ                     0xE5
#define AM_SX127X_FSK_RF_FRFMID_919_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_919_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_920_MHZ                     0xE6
#define AM_SX127X_FSK_RF_FRFMID_920_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_920_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_921_MHZ                     0xE6
#define AM_SX127X_FSK_RF_FRFMID_921_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_921_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_922_MHZ                     0xE6
#define AM_SX127X_FSK_RF_FRFMID_922_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_922_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_923_MHZ                     0xE6
#define AM_SX127X_FSK_RF_FRFMID_923_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_923_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_924_MHZ                     0xE7
#define AM_SX127X_FSK_RF_FRFMID_924_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_924_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_925_MHZ                     0xE7
#define AM_SX127X_FSK_RF_FRFMID_925_MHZ                     0x40
#define AM_SX127X_FSK_RF_FRFLSB_925_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_926_MHZ                     0xE7
#define AM_SX127X_FSK_RF_FRFMID_926_MHZ                     0x80
#define AM_SX127X_FSK_RF_FRFLSB_926_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_927_MHZ                     0xE7
#define AM_SX127X_FSK_RF_FRFMID_927_MHZ                     0xC0
#define AM_SX127X_FSK_RF_FRFLSB_927_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFMSB_928_MHZ                     0xE8
#define AM_SX127X_FSK_RF_FRFMID_928_MHZ                     0x00
#define AM_SX127X_FSK_RF_FRFLSB_928_MHZ                     0x00

/**
 * \brief RegPaConfig
 */
#define AM_SX127X_FSK_RF_PACONFIG_PASELECT_MASK             0x7F
#define AM_SX127X_FSK_RF_PACONFIG_PASELECT_PABOOST          0x80
#define AM_SX127X_FSK_RF_PACONFIG_PASELECT_RFO              0x00 /* Default */

#define AM_SX127X_FSK_RF_PACONFIG_MAX_POWER_MASK            0x8F

#define AM_SX127X_FSK_RF_PACONFIG_OUTPUTPOWER_MASK          0xF0

/**
 * \brief RegPaRamp
 */
#define AM_SX127X_FSK_RF_PARAMP_MODULATIONSHAPING_MASK      0x9F
#define AM_SX127X_FSK_RF_PARAMP_MODULATIONSHAPING_00        0x00  /* Default */
#define AM_SX127X_FSK_RF_PARAMP_MODULATIONSHAPING_01        0x20
#define AM_SX127X_FSK_RF_PARAMP_MODULATIONSHAPING_10        0x40
#define AM_SX127X_FSK_RF_PARAMP_MODULATIONSHAPING_11        0x60

#define AM_SX127X_FSK_RF_PARAMP_LOWPNTXPLL_MASK             0xEF
#define AM_SX127X_FSK_RF_PARAMP_LOWPNTXPLL_OFF              0x10
#define AM_SX127X_FSK_RF_PARAMP_LOWPNTXPLL_ON               0x00  /* Default */

#define AM_SX127X_FSK_RF_PARAMP_MASK                        0xF0
#define AM_SX127X_FSK_RF_PARAMP_3400_US                     0x00
#define AM_SX127X_FSK_RF_PARAMP_2000_US                     0x01
#define AM_SX127X_FSK_RF_PARAMP_1000_US                     0x02
#define AM_SX127X_FSK_RF_PARAMP_0500_US                     0x03
#define AM_SX127X_FSK_RF_PARAMP_0250_US                     0x04
#define AM_SX127X_FSK_RF_PARAMP_0125_US                     0x05
#define AM_SX127X_FSK_RF_PARAMP_0100_US                     0x06
#define AM_SX127X_FSK_RF_PARAMP_0062_US                     0x07
#define AM_SX127X_FSK_RF_PARAMP_0050_US                     0x08
#define AM_SX127X_FSK_RF_PARAMP_0040_US                     0x09  /* Default */
#define AM_SX127X_FSK_RF_PARAMP_0031_US                     0x0A
#define AM_SX127X_FSK_RF_PARAMP_0025_US                     0x0B
#define AM_SX127X_FSK_RF_PARAMP_0020_US                     0x0C
#define AM_SX127X_FSK_RF_PARAMP_0015_US                     0x0D
#define AM_SX127X_FSK_RF_PARAMP_0012_US                     0x0E
#define AM_SX127X_FSK_RF_PARAMP_0010_US                     0x0F

/**
 * \brief RegOcp
 */
#define AM_SX127X_FSK_RF_OCP_MASK                           0xDF
#define AM_SX127X_FSK_RF_OCP_ON                             0x20  /* Default */
#define AM_SX127X_FSK_RF_OCP_OFF                            0x00

#define AM_SX127X_FSK_RF_OCP_TRIM_MASK                      0xE0
#define AM_SX127X_FSK_RF_OCP_TRIM_045_MA                    0x00
#define AM_SX127X_FSK_RF_OCP_TRIM_050_MA                    0x01
#define AM_SX127X_FSK_RF_OCP_TRIM_055_MA                    0x02
#define AM_SX127X_FSK_RF_OCP_TRIM_060_MA                    0x03
#define AM_SX127X_FSK_RF_OCP_TRIM_065_MA                    0x04
#define AM_SX127X_FSK_RF_OCP_TRIM_070_MA                    0x05
#define AM_SX127X_FSK_RF_OCP_TRIM_075_MA                    0x06
#define AM_SX127X_FSK_RF_OCP_TRIM_080_MA                    0x07
#define AM_SX127X_FSK_RF_OCP_TRIM_085_MA                    0x08
#define AM_SX127X_FSK_RF_OCP_TRIM_090_MA                    0x09
#define AM_SX127X_FSK_RF_OCP_TRIM_095_MA                    0x0A
#define AM_SX127X_FSK_RF_OCP_TRIM_100_MA                    0x0B  /* Default */
#define AM_SX127X_FSK_RF_OCP_TRIM_105_MA                    0x0C
#define AM_SX127X_FSK_RF_OCP_TRIM_110_MA                    0x0D
#define AM_SX127X_FSK_RF_OCP_TRIM_115_MA                    0x0E
#define AM_SX127X_FSK_RF_OCP_TRIM_120_MA                    0x0F
#define AM_SX127X_FSK_RF_OCP_TRIM_130_MA                    0x10
#define AM_SX127X_FSK_RF_OCP_TRIM_140_MA                    0x11
#define AM_SX127X_FSK_RF_OCP_TRIM_150_MA                    0x12
#define AM_SX127X_FSK_RF_OCP_TRIM_160_MA                    0x13
#define AM_SX127X_FSK_RF_OCP_TRIM_170_MA                    0x14
#define AM_SX127X_FSK_RF_OCP_TRIM_180_MA                    0x15
#define AM_SX127X_FSK_RF_OCP_TRIM_190_MA                    0x16
#define AM_SX127X_FSK_RF_OCP_TRIM_200_MA                    0x17
#define AM_SX127X_FSK_RF_OCP_TRIM_210_MA                    0x18
#define AM_SX127X_FSK_RF_OCP_TRIM_220_MA                    0x19
#define AM_SX127X_FSK_RF_OCP_TRIM_230_MA                    0x1A
#define AM_SX127X_FSK_RF_OCP_TRIM_240_MA                    0x1B

/**
 * \brief RegLna
 */
#define AM_SX127X_FSK_RF_LNA_GAIN_MASK                      0x1F
#define AM_SX127X_FSK_RF_LNA_GAIN_G1                        0x20  /* Default */
#define AM_SX127X_FSK_RF_LNA_GAIN_G2                        0x40
#define AM_SX127X_FSK_RF_LNA_GAIN_G3                        0x60
#define AM_SX127X_FSK_RF_LNA_GAIN_G4                        0x80
#define AM_SX127X_FSK_RF_LNA_GAIN_G5                        0xA0
#define AM_SX127X_FSK_RF_LNA_GAIN_G6                        0xC0

#define AM_SX127X_FSK_RF_LNA_BOOST_MASK                     0xFC
#define AM_SX127X_FSK_RF_LNA_BOOST_OFF                      0x00 /* Default */
#define AM_SX127X_FSK_RF_LNA_BOOST_ON                       0x03

/**
 * \brief RegRxConfig
 */
#define AM_SX127X_FSK_RF_RXCONFIG_RESTARTRXONCOLLISION_MASK    0x7F
#define AM_SX127X_FSK_RF_RXCONFIG_RESTARTRXONCOLLISION_ON      0x80
#define AM_SX127X_FSK_RF_RXCONFIG_RESTARTRXONCOLLISION_OFF     0x00 /* Default */

#define AM_SX127X_FSK_RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK      0x40 /* Write only */

#define AM_SX127X_FSK_RF_RXCONFIG_RESTARTRXWITHPLLLOCK         0x20 /* Write only */

#define AM_SX127X_FSK_RF_RXCONFIG_AFCAUTO_MASK                 0xEF
#define AM_SX127X_FSK_RF_RXCONFIG_AFCAUTO_ON                   0x10
#define AM_SX127X_FSK_RF_RXCONFIG_AFCAUTO_OFF                  0x00 /* Default */

#define AM_SX127X_FSK_RF_RXCONFIG_AGCAUTO_MASK                 0xF7
#define AM_SX127X_FSK_RF_RXCONFIG_AGCAUTO_ON                   0x08 /* Default */
#define AM_SX127X_FSK_RF_RXCONFIG_AGCAUTO_OFF                  0x00

#define AM_SX127X_FSK_RF_RXCONFIG_RXTRIGER_MASK                0xF8
#define AM_SX127X_FSK_RF_RXCONFIG_RXTRIGER_OFF                 0x00
#define AM_SX127X_FSK_RF_RXCONFIG_RXTRIGER_RSSI                0x01
#define AM_SX127X_FSK_RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT      0x06 /* Default */
#define AM_SX127X_FSK_RF_RXCONFIG_RXTRIGER_RSSI_PREAMBLEDETECT 0x07

/**
 * \brief RegRssiConfig
 */
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_MASK             0x07
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_00_DB          0x00  /* Default */
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_01_DB          0x08
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_02_DB          0x10
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_03_DB          0x18
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_04_DB          0x20
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_05_DB          0x28
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_06_DB          0x30
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_07_DB          0x38
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_08_DB          0x40
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_09_DB          0x48
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_10_DB          0x50
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_11_DB          0x58
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_12_DB          0x60
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_13_DB          0x68
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_14_DB          0x70
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_P_15_DB          0x78
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_16_DB          0x80
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_15_DB          0x88
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_14_DB          0x90
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_13_DB          0x98
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_12_DB          0xA0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_11_DB          0xA8
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_10_DB          0xB0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_09_DB          0xB8
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_08_DB          0xC0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_07_DB          0xC8
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_06_DB          0xD0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_05_DB          0xD8
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_04_DB          0xE0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_03_DB          0xE8
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_02_DB          0xF0
#define AM_SX127X_FSK_RF_RSSICONFIG_OFFSET_M_01_DB          0xF8

#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_MASK          0xF8
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_2             0x00
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_4             0x01
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_8             0x02  /* Default */
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_16            0x03
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_32            0x04
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_64            0x05
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_128           0x06
#define AM_SX127X_FSK_RF_RSSICONFIG_SMOOTHING_256           0x07

/**
 * \brief RegRssiCollision
 */
#define AM_SX127X_FSK_RF_RSSICOLISION_THRESHOLD             0x0A  /* Default */

/**
 * \brief RegRssiThresh
 */
#define AM_SX127X_FSK_RF_RSSITHRESH_THRESHOLD               0xFF  /* Default */

/**
 * \brief RegRssiValue (Read Only)
 */

/**
 * \brief RegRxBw
 */
#define AM_SX127X_FSK_RF_RXBW_MANT_MASK                     0xE7
#define AM_SX127X_FSK_RF_RXBW_MANT_16                       0x00
#define AM_SX127X_FSK_RF_RXBW_MANT_20                       0x08
#define AM_SX127X_FSK_RF_RXBW_MANT_24                       0x10  /* Default */

#define AM_SX127X_FSK_RF_RXBW_EXP_MASK                      0xF8
#define AM_SX127X_FSK_RF_RXBW_EXP_0                         0x00
#define AM_SX127X_FSK_RF_RXBW_EXP_1                         0x01
#define AM_SX127X_FSK_RF_RXBW_EXP_2                         0x02
#define AM_SX127X_FSK_RF_RXBW_EXP_3                         0x03
#define AM_SX127X_FSK_RF_RXBW_EXP_4                         0x04
#define AM_SX127X_FSK_RF_RXBW_EXP_5                         0x05  /* Default */
#define AM_SX127X_FSK_RF_RXBW_EXP_6                         0x06
#define AM_SX127X_FSK_RF_RXBW_EXP_7                         0x07

/**
 * \brief RegAfcBw
 */
#define AM_SX127X_FSK_RF_AFCBW_MANTAFC_MASK                 0xE7
#define AM_SX127X_FSK_RF_AFCBW_MANTAFC_16                   0x00
#define AM_SX127X_FSK_RF_AFCBW_MANTAFC_20                   0x08  /* Default */
#define AM_SX127X_FSK_RF_AFCBW_MANTAFC_24                   0x10

#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_MASK                  0xF8
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_0                     0x00
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_1                     0x01
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_2                     0x02
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_3                     0x03  /* Default */
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_4                     0x04
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_5                     0x05
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_6                     0x06
#define AM_SX127X_FSK_RF_AFCBW_EXPAFC_7                     0x07

/**
 * \brief RegOokPeak
 */
#define AM_SX127X_FSK_RF_OOKPEAK_BITSYNC_MASK               0xDF  /* Default */
#define AM_SX127X_FSK_RF_OOKPEAK_BITSYNC_ON                 0x20  /* Default */
#define AM_SX127X_FSK_RF_OOKPEAK_BITSYNC_OFF                0x00

#define AM_SX127X_FSK_RF_OOKPEAK_OOKTHRESHTYPE_MASK         0xE7
#define AM_SX127X_FSK_RF_OOKPEAK_OOKTHRESHTYPE_FIXED        0x00
#define AM_SX127X_FSK_RF_OOKPEAK_OOKTHRESHTYPE_PEAK         0x08  /* Default */
#define AM_SX127X_FSK_RF_OOKPEAK_OOKTHRESHTYPE_AVERAGE      0x10

#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_MASK     0xF8
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_0_5_DB   0x00  /* Default */
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_1_0_DB   0x01
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_1_5_DB   0x02
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_2_0_DB   0x03
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_3_0_DB   0x04
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_4_0_DB   0x05
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_5_0_DB   0x06
#define AM_SX127X_FSK_RF_OOKPEAK_OOKPEAKTHRESHSTEP_6_0_DB   0x07

/**
 * \brief RegOokFix
 */
#define AM_SX127X_FSK_RF_OOKFIX_OOKFIXEDTHRESHOLD           0x0C  /* Default */

/**
 * \brief RegOokAvg
 */
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_MASK       0x1F
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_000        0x00  /* Default */
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_001        0x20
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_010        0x40
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_011        0x60
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_100        0x80
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_101        0xA0
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_110        0xC0
#define AM_SX127X_FSK_RF_OOKAVG_OOKPEAKTHRESHDEC_111        0xE0

#define AM_SX127X_FSK_RF_OOKAVG_AVERAGEOFFSET_MASK          0xF3
#define AM_SX127X_FSK_RF_OOKAVG_AVERAGEOFFSET_0_DB          0x00  /* Default */
#define AM_SX127X_FSK_RF_OOKAVG_AVERAGEOFFSET_2_DB          0x04
#define AM_SX127X_FSK_RF_OOKAVG_AVERAGEOFFSET_4_DB          0x08
#define AM_SX127X_FSK_RF_OOKAVG_AVERAGEOFFSET_6_DB          0x0C

#define AM_SX127X_FSK_RF_OOKAVG_OOKAVERAGETHRESHFILT_MASK   0xFC
#define AM_SX127X_FSK_RF_OOKAVG_OOKAVERAGETHRESHFILT_00     0x00
#define AM_SX127X_FSK_RF_OOKAVG_OOKAVERAGETHRESHFILT_01     0x01
#define AM_SX127X_FSK_RF_OOKAVG_OOKAVERAGETHRESHFILT_10     0x02  /* Default */
#define AM_SX127X_FSK_RF_OOKAVG_OOKAVERAGETHRESHFILT_11     0x03

/**
 * \brief RegAfcFei
 */
#define AM_SX127X_FSK_RF_AFCFEI_AGCSTART                    0x10

#define AM_SX127X_FSK_RF_AFCFEI_AFCCLEAR                    0x02

#define AM_SX127X_FSK_RF_AFCFEI_AFCAUTOCLEAR_MASK           0xFE
#define AM_SX127X_FSK_RF_AFCFEI_AFCAUTOCLEAR_ON             0x01
#define AM_SX127X_FSK_RF_AFCFEI_AFCAUTOCLEAR_OFF            0x00  /* Default */

/**
 * \brief RegAfcMsb (Read Only)
 */

/**
 * \brief RegAfcLsb (Read Only)
 */

/**
 * \brief RegFeiMsb (Read Only)
 */

/**
 * \brief RegFeiLsb (Read Only)
 */

/**
 * \brief RegPreambleDetect
 */
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTOR_MASK       0x7F
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTOR_ON         0x80  /* Default */
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTOR_OFF        0x00

#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORSIZE_MASK   0x9F
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORSIZE_1      0x00
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORSIZE_2      0x20  /* Default */
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORSIZE_3      0x40
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORSIZE_4      0x60

#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_MASK    0xE0
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_0       0x00
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_1       0x01
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_2       0x02
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_3       0x03
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_4       0x04
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_5       0x05
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_6       0x06
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_7       0x07
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_8       0x08
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_9       0x09
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_10      0x0A  /* Default */
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_11      0x0B
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_12      0x0C
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_13      0x0D
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_14      0x0E
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_15      0x0F
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_16      0x10
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_17      0x11
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_18      0x12
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_19      0x13
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_20      0x14
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_21      0x15
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_22      0x16
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_23      0x17
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_24      0x18
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_25      0x19
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_26      0x1A
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_27      0x1B
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_28      0x1C
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_29      0x1D
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_30      0x1E
#define AM_SX127X_FSK_RF_PREAMBLEDETECT_DETECTORTOL_31      0x1F

/**
 * \brief RegRxTimeout1
 */
#define AM_SX127X_FSK_RF_RXTIMEOUT1_TIMEOUTRXRSSI           0x00  /* Default */

/**
 * \brief RegRxTimeout2
 */
#define AM_SX127X_FSK_RF_RXTIMEOUT2_TIMEOUTRXPREAMBLE       0x00  /* Default */

/**
 * \brief RegRxTimeout3
 */
#define AM_SX127X_FSK_RF_RXTIMEOUT3_TIMEOUTSIGNALSYNC       0x00  /* Default */

/**
 * \brief RegRxDelay
 */
#define AM_SX127X_FSK_RF_RXDELAY_INTERPACKETRXDELAY         0x00  /* Default */

/**
 * \brief RegOsc
 */
#define AM_SX127X_FSK_RF_OSC_RCCALSTART                     0x08

#define AM_SX127X_FSK_RF_OSC_CLKOUT_MASK                    0xF8
#define AM_SX127X_FSK_RF_OSC_CLKOUT_32_MHZ                  0x00
#define AM_SX127X_FSK_RF_OSC_CLKOUT_16_MHZ                  0x01
#define AM_SX127X_FSK_RF_OSC_CLKOUT_8_MHZ                   0x02
#define AM_SX127X_FSK_RF_OSC_CLKOUT_4_MHZ                   0x03
#define AM_SX127X_FSK_RF_OSC_CLKOUT_2_MHZ                   0x04
#define AM_SX127X_FSK_RF_OSC_CLKOUT_1_MHZ                   0x05  /* Default */
#define AM_SX127X_FSK_RF_OSC_CLKOUT_RC                      0x06
#define AM_SX127X_FSK_RF_OSC_CLKOUT_OFF                     0x07

/**
 * \brief RegPreambleMsb/RegPreambleLsb
 */
#define AM_SX127X_FSK_RF_PREAMBLEMSB_SIZE                   0x00  /* Default */
#define AM_SX127X_FSK_RF_PREAMBLELSB_SIZE                   0x03  /* Default */

/**
 * \brief RegSyncConfig
 */
#define AM_SX127X_FSK_RF_SYNCCONFIG_AUTORESTARTRXMODE_MASK        0x3F
#define AM_SX127X_FSK_RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON  0x80  /* Default */
#define AM_SX127X_FSK_RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_OFF 0x40
#define AM_SX127X_FSK_RF_SYNCCONFIG_AUTORESTARTRXMODE_OFF         0x00


#define AM_SX127X_FSK_RF_SYNCCONFIG_PREAMBLEPOLARITY_MASK         0xDF
#define AM_SX127X_FSK_RF_SYNCCONFIG_PREAMBLEPOLARITY_55           0x20
#define AM_SX127X_FSK_RF_SYNCCONFIG_PREAMBLEPOLARITY_AA           0x00  /* Default */

#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNC_MASK                     0xEF
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNC_ON                       0x10  /* Default */
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNC_OFF                      0x00


#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_MASK                 0xF8
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_1                    0x00
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_2                    0x01
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_3                    0x02
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_4                    0x03  /* Default */
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_5                    0x04
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_6                    0x05
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_7                    0x06
#define AM_SX127X_FSK_RF_SYNCCONFIG_SYNCSIZE_8                    0x07

/**
 * \brief RegSyncValue1-8
 */
#define AM_SX127X_FSK_RF_SYNCVALUE1_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE2_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE3_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE4_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE5_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE6_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE7_SYNCVALUE               0x01  /* Default */
#define AM_SX127X_FSK_RF_SYNCVALUE8_SYNCVALUE               0x01  /* Default */

/**
 * \brief RegPacketConfig1
 */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_PACKETFORMAT_MASK          0x7F
#define AM_SX127X_FSK_RF_PACKETCONFIG1_PACKETFORMAT_FIXED         0x00
#define AM_SX127X_FSK_RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE      0x80  /* Default */

#define AM_SX127X_FSK_RF_PACKETCONFIG1_DCFREE_MASK                0x9F
#define AM_SX127X_FSK_RF_PACKETCONFIG1_DCFREE_OFF                 0x00  /* Default */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_DCFREE_MANCHESTER          0x20
#define AM_SX127X_FSK_RF_PACKETCONFIG1_DCFREE_WHITENING           0x40

#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRC_MASK                   0xEF
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRC_ON                     0x10  /* Default */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRC_OFF                    0x00

#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCAUTOCLEAR_MASK          0xF7
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCAUTOCLEAR_ON            0x00  /* Default */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF           0x08

#define AM_SX127X_FSK_RF_PACKETCONFIG1_ADDRSFILTERING_MASK         0xF9
#define AM_SX127X_FSK_RF_PACKETCONFIG1_ADDRSFILTERING_OFF          0x00  /* Default */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_ADDRSFILTERING_NODE         0x02
#define AM_SX127X_FSK_RF_PACKETCONFIG1_ADDRSFILTERING_NODEBROADCAST 0x04

#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCWHITENINGTYPE_MASK      0xFE
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT     0x00  /* Default */
#define AM_SX127X_FSK_RF_PACKETCONFIG1_CRCWHITENINGTYPE_IBM       0x01

/**
 * \brief RegPacketConfig2
 */

#define AM_SX127X_FSK_RF_PACKETCONFIG2_WMBUS_CRC_ENABLE_MASK      0x7F
#define AM_SX127X_FSK_RF_PACKETCONFIG2_WMBUS_CRC_ENABLE           0x80
#define AM_SX127X_FSK_RF_PACKETCONFIG2_WMBUS_CRC_DISABLE          0x00  /* Default */

#define AM_SX127X_FSK_RF_PACKETCONFIG2_DATAMODE_MASK              0xBF
#define AM_SX127X_FSK_RF_PACKETCONFIG2_DATAMODE_CONTINUOUS        0x00
#define AM_SX127X_FSK_RF_PACKETCONFIG2_DATAMODE_PACKET            0x40  /* Default */

#define AM_SX127X_FSK_RF_PACKETCONFIG2_IOHOME_MASK                0xDF
#define AM_SX127X_FSK_RF_PACKETCONFIG2_IOHOME_ON                  0x20
#define AM_SX127X_FSK_RF_PACKETCONFIG2_IOHOME_OFF                 0x00  /* Default */

#define AM_SX127X_FSK_RF_PACKETCONFIG2_BEACON_MASK                0xF7
#define AM_SX127X_FSK_RF_PACKETCONFIG2_BEACON_ON                  0x08
#define AM_SX127X_FSK_RF_PACKETCONFIG2_BEACON_OFF                 0x00  /* Default */

#define AM_SX127X_FSK_RF_PACKETCONFIG2_PAYLOADLENGTH_MSB_MASK     0xF8

/**
 * \brief RegPayloadLength
 */
#define AM_SX127X_FSK_RF_PAYLOADLENGTH_LENGTH                     0x40  /* Default */

/**
 * \brief RegNodeAdrs
 */
#define AM_SX127X_FSK_RF_NODEADDRESS_ADDRESS                      0x00

/**
 * RegBroadcastAdrs
 */
#define AM_SX127X_FSK_RF_BROADCASTADDRESS_ADDRESS                 0x00

/**
 * \brief RegFifoThresh
 */
#define AM_SX127X_FSK_RF_FIFOTHRESH_TXSTARTCONDITION_MASK         0x7F
#define AM_SX127X_FSK_RF_FIFOTHRESH_TXSTARTCONDITION_FIFOTHRESH   0x00  /* Default */
#define AM_SX127X_FSK_RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY 0x80

#define AM_SX127X_FSK_RF_FIFOTHRESH_FIFOTHRESHOLD_MASK            0xC0
#define AM_SX127X_FSK_RF_FIFOTHRESH_FIFOTHRESHOLD_THRESHOLD       0x0F  /* Default */

/**
 * \brief RegSeqConfig1
 */
#define AM_SX127X_FSK_RF_SEQCONFIG1_SEQUENCER_START               0x80

#define AM_SX127X_FSK_RF_SEQCONFIG1_SEQUENCER_STOP                0x40

#define AM_SX127X_FSK_RF_SEQCONFIG1_IDLEMODE_MASK                 0xDF
#define AM_SX127X_FSK_RF_SEQCONFIG1_IDLEMODE_SLEEP                0x20
#define AM_SX127X_FSK_RF_SEQCONFIG1_IDLEMODE_STANDBY              0x00  /* Default */

#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMSTART_MASK                0xE7
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMSTART_TOLPS               0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMSTART_TORX                0x08
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMSTART_TOTX                0x10
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMSTART_TOTX_ONFIFOLEVEL    0x18

#define AM_SX127X_FSK_RF_SEQCONFIG1_LPS_MASK                      0xFB
#define AM_SX127X_FSK_RF_SEQCONFIG1_LPS_SEQUENCER_OFF             0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG1_LPS_IDLE                      0x04

#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMIDLE_MASK                 0xFD
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMIDLE_TOTX                 0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMIDLE_TORX                 0x02

#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMTX_MASK                   0xFE
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMTX_TOLPS                  0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG1_FROMTX_TORX                   0x01

/**
 * \brief RegSeqConfig2
 */
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_MASK                   0x1F
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOUNUSED_000           0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TORXPKT_ONPLDRDY       0x20
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOLPS_ONPLDRDY         0x40
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TORXPKT_ONCRCOK        0x60
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOSEQUENCEROFF_ONRSSI  0x80
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOSEQUENCEROFF_ONSYNC  0xA0
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOSEQUENCEROFF_ONPREAMBLE 0xC0
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRX_TOUNUSED_111           0xE0

#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXTIMEOUT_MASK            0xE7
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXTIMEOUT_TORXRESTART     0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXTIMEOUT_TOTX            0x08
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXTIMEOUT_TOLPS           0x10
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXTIMEOUT_TOSEQUENCEROFF  0x18

#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_MASK                0xF8
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_TOSEQUENCEROFF      0x00  /* Default */
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_TOTX_ONFIFOEMPTY    0x01
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_TOLPS               0x02
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_TOSYNTHESIZERRX     0x03
#define AM_SX127X_FSK_RF_SEQCONFIG2_FROMRXPKT_TORX                0x04

/**
 * \brief RegTimerResol
 */
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER1RESOL_MASK              0xF3
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER1RESOL_OFF               0x00  /* Default */
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER1RESOL_000064_US         0x04
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER1RESOL_004100_US         0x08
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER1RESOL_262000_US         0x0C

#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER2RESOL_MASK              0xFC
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER2RESOL_OFF               0x00  /* Default */
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER2RESOL_000064_US         0x01
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER2RESOL_004100_US         0x02
#define AM_SX127X_FSK_RF_TIMERRESOL_TIMER2RESOL_262000_US         0x03

/**
 * \brief RegTimer1Coef
 */
#define AM_SX127X_FSK_RF_TIMER1COEF_TIMER1COEFFICIENT             0xF5  /* Default */

/**
 * \brief RegTimer2Coef
 */
#define AM_SX127X_FSK_RF_TIMER2COEF_TIMER2COEFFICIENT             0x20  /* Default */

/**
 * \brief RegImageCal
 */
#define AM_SX127X_FSK_RF_IMAGECAL_AUTOIMAGECAL_MASK         0x7F
#define AM_SX127X_FSK_RF_IMAGECAL_AUTOIMAGECAL_ON           0x80
#define AM_SX127X_FSK_RF_IMAGECAL_AUTOIMAGECAL_OFF          0x00  /* Default */

#define AM_SX127X_FSK_RF_IMAGECAL_IMAGECAL_MASK             0xBF
#define AM_SX127X_FSK_RF_IMAGECAL_IMAGECAL_START            0x40

#define AM_SX127X_FSK_RF_IMAGECAL_IMAGECAL_RUNNING          0x20
#define AM_SX127X_FSK_RF_IMAGECAL_IMAGECAL_DONE             0x00  /* Default */

#define AM_SX127X_FSK_RF_IMAGECAL_TEMPCHANGE_HIGHER         0x08
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPCHANGE_LOWER          0x00

#define AM_SX127X_FSK_RF_IMAGECAL_TEMPTHRESHOLD_MASK        0xF9
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPTHRESHOLD_05          0x00
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPTHRESHOLD_10          0x02  /* Default */
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPTHRESHOLD_15          0x04
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPTHRESHOLD_20          0x06

#define AM_SX127X_FSK_RF_IMAGECAL_TEMPMONITOR_MASK          0xFE
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPMONITOR_ON            0x00 /* Default */
#define AM_SX127X_FSK_RF_IMAGECAL_TEMPMONITOR_OFF           0x01

/**
 * \brief RegTemp (Read Only)
 */

/**
 * \brief RegLowBat
 */
#define AM_SX127X_FSK_RF_LOWBAT_MASK                        0xF7
#define AM_SX127X_FSK_RF_LOWBAT_ON                          0x08
#define AM_SX127X_FSK_RF_LOWBAT_OFF                         0x00  /* Default */

#define AM_SX127X_FSK_RF_LOWBAT_TRIM_MASK                   0xF8
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_1695                   0x00
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_1764                   0x01
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_1835                   0x02  /* Default */
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_1905                   0x03
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_1976                   0x04
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_2045                   0x05
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_2116                   0x06
#define AM_SX127X_FSK_RF_LOWBAT_TRIM_2185                   0x07

/**
 * \brief RegIrqFlags1
 */
#define AM_SX127X_FSK_RF_IRQFLAGS1_MODEREADY                0x80
#define AM_SX127X_FSK_RF_IRQFLAGS1_RXREADY                  0x40
#define AM_SX127X_FSK_RF_IRQFLAGS1_TXREADY                  0x20
#define AM_SX127X_FSK_RF_IRQFLAGS1_PLLLOCK                  0x10
#define AM_SX127X_FSK_RF_IRQFLAGS1_RSSI                     0x08
#define AM_SX127X_FSK_RF_IRQFLAGS1_TIMEOUT                  0x04
#define AM_SX127X_FSK_RF_IRQFLAGS1_PREAMBLEDETECT           0x02
#define AM_SX127X_FSK_RF_IRQFLAGS1_SYNCADDRESSMATCH         0x01

/**
 * \brief RegIrqFlags2
 */
#define AM_SX127X_FSK_RF_IRQFLAGS2_FIFOFULL                 0x80
#define AM_SX127X_FSK_RF_IRQFLAGS2_FIFOEMPTY                0x40
#define AM_SX127X_FSK_RF_IRQFLAGS2_FIFOLEVEL                0x20
#define AM_SX127X_FSK_RF_IRQFLAGS2_FIFOOVERRUN              0x10
#define AM_SX127X_FSK_RF_IRQFLAGS2_PACKETSENT               0x08
#define AM_SX127X_FSK_RF_IRQFLAGS2_PAYLOADREADY             0x04
#define AM_SX127X_FSK_RF_IRQFLAGS2_CRCOK                    0x02
#define AM_SX127X_FSK_RF_IRQFLAGS2_LOWBAT                   0x01

/**
 * \brief RegDioMapping1
 */
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO0_MASK              0x3F
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO0_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO0_01                0x40
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO0_10                0x80
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO0_11                0xC0

#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO1_MASK              0xCF
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO1_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO1_01                0x10
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO1_10                0x20
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO1_11                0x30

#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO2_MASK              0xF3
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO2_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO2_01                0x04
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO2_10                0x08
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO2_11                0x0C

#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO3_MASK              0xFC
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO3_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO3_01                0x01
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO3_10                0x02
#define AM_SX127X_FSK_RF_DIOMAPPING1_DIO3_11                0x03

/**
 * \brief RegDioMapping2
 */
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO4_MASK              0x3F
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO4_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO4_01                0x40
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO4_10                0x80
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO4_11                0xC0

#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO5_MASK              0xCF
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO5_00                0x00  /* Default */
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO5_01                0x10
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO5_10                0x20
#define AM_SX127X_FSK_RF_DIOMAPPING2_DIO5_11                0x30

#define AM_SX127X_FSK_RF_DIOMAPPING2_MAP_MASK               0xFE
#define AM_SX127X_FSK_RF_DIOMAPPING2_MAP_PREAMBLEDETECT     0x01
#define AM_SX127X_FSK_RF_DIOMAPPING2_MAP_RSSI               0x00  /* Default */

/**
 * \brief RegVersion (Read Only)
 */

/**
 * \brief RegPllHop
 */
#define AM_SX127X_FSK_RF_PLLHOP_FASTHOP_MASK                0x7F
#define AM_SX127X_FSK_RF_PLLHOP_FASTHOP_ON                  0x80
#define AM_SX127X_FSK_RF_PLLHOP_FASTHOP_OFF                 0x00 /* Default */

/**
 * \brief RegTcxo
 */
#define AM_SX127X_FSK_RF_TCXO_TCXOINPUT_MASK                0xEF
#define AM_SX127X_FSK_RF_TCXO_TCXOINPUT_ON                  0x10
#define AM_SX127X_FSK_RF_TCXO_TCXOINPUT_OFF                 0x00  /* Default */

/**
 * \brief RegPaDac
 */
#define AM_SX127X_FSK_RF_PADAC_20DBM_MASK                   0xF8
#define AM_SX127X_FSK_RF_PADAC_20DBM_ON                     0x07
#define AM_SX127X_FSK_RF_PADAC_20DBM_OFF                    0x04  /* Default */

/**
 * \brief RegFormerTemp
 */

/**
 * \brief RegBitrateFrac
 */
#define AM_SX127X_FSK_RF_BITRATEFRAC_MASK                   0xF0

/**
 * \brief RegAgcRef
 */

/**
 * \brief RegAgcThresh1
 */

/**
 * \brief RegAgcThresh2
 */

/**
 * \brief RegAgcThresh3
 */

/**
 * \brief RegPll
 */
#define AM_SX127X_FSK_RF_PLL_BANDWIDTH_MASK                 0x3F
#define AM_SX127X_FSK_RF_PLL_BANDWIDTH_75                   0x00
#define AM_SX127X_FSK_RF_PLL_BANDWIDTH_150                  0x40
#define AM_SX127X_FSK_RF_PLL_BANDWIDTH_225                  0x80
#define AM_SX127X_FSK_RF_PLL_BANDWIDTH_300                  0xC0  /* Default */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_SX127X_REGS_FSK_H */

/* end of file */
