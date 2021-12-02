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
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 定义操作时钟的接口
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-18  randy, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_OCOTP_REGS_H
#define __IMX6UL_OCOTP_REGS_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


//-------------------------------------------------------------------------------------------
// hw_ccm_analog_t - module struct
//-------------------------------------------------------------------------------------------
/*!
 * @brief All CCM_ANALOG module registers.
 */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t OCOTP_CTRL;            //!< OTP Controller Control Register
    uint32_t OCOTP_CTRL_SET;        //!< OTP Controller Control Register Set
    uint32_t OCOTP_CTRL_CLR;        //!< OTP Controller Control Register Clear
    uint32_t OCOTP_CTRL_TOG;        //!< OTP Controller Control Register Toggle
    uint32_t OCOTP_TIMING;          //!< OTP Controller Timing Register
    uint32_t _reserved0[3];
    uint32_t OCOTP_DATA;            //!< OTP Controller Write Data Register
    uint32_t _reserved1[3];
    uint32_t OCOTP_READ_CTRL;       //!< OTP Controller Write Data Register
    uint32_t _reserved2[3];
    uint32_t OCOTP_READ_FUSE_DATA;  //!< OTP Controller Read Data Register
    uint32_t _reserved3[3];
    uint32_t OCOTP_SW_STICKY;       //!< Sticky bit Register
    uint32_t _reserved4[3];
    uint32_t OCOTP_SCS;             //!< Software Controllable Signals Register
    uint32_t OCOTP_SCS_SET;         //!< Software Controllable Signals Register Set
    uint32_t OCOTP_SCS_CLR;         //!< Software Controllable Signals Register Clear
    uint32_t OCOTP_SCS_TOG;         //!< Software Controllable Signals Register Toggle
    uint32_t OCOTP_CRC_ADDR;
    uint32_t _reserved5[3];
    uint32_t OCOTP_CRC_VALUE;
    uint32_t _reserved6[3];
    uint32_t OCOTP_VERSION;         //!< OTP Controller Version Register
    uint32_t _reserved7[219];
    uint32_t OCOTP_LOCK;            //!< Value of OTP Bank0 Word0 (Lock controls)
    uint32_t _reserved8[3];
    uint32_t OCOTP_CFG0;            //!< Value of OTP Bank0 Word1 (Configuration and Manufacturing Info.)
    uint32_t _reserved9[3];
    uint32_t OCOTP_CFG1;            //!< Value of OTP Bank0 Word2 (Configuration and Manufacturing Info.)
    uint32_t _reserved10[3];
    uint32_t OCOTP_CFG2;            //!< Value of OTP Bank0 Word3 (Configuration and Manufacturing Info.)
    uint32_t _reserved11[3];
    uint32_t OCOTP_CFG3;            //!< Value of OTP Bank0 Word4 (Configuration and Manufacturing Info.)
    uint32_t _reserved12[3];
    uint32_t OCOTP_CFG4;            //!< Value of OTP Bank0 Word5 (Configuration and Manufacturing Info.)
    uint32_t _reserved13[3];
    uint32_t OCOTP_CFG5;            //!< Value of OTP Bank0 Word6 (Configuration and Manufacturing Info.)
    uint32_t _reserved14[3];
    uint32_t OCOTP_CFG6;            //!< Value of OTP Bank0 Word7 (Configuration and Manufacturing Info.)
    uint32_t _reserved15[3];
    uint32_t OCOTP_MEM0;            //!< Value of OTP Bank1 Word0 (Memory Related Info.)
    uint32_t _reserved16[3];
    uint32_t OCOTP_MEM1;            //!< Value of OTP Bank1 Word1 (Memory Related Info.)
    uint32_t _reserved17[3];
    uint32_t OCOTP_MEM2;            //!< Value of OTP Bank1 Word2 (Memory Related Info.)
    uint32_t _reserved18[3];
    uint32_t OCOTP_MEM3;            //!< Value of OTP Bank1 Word3 (Memory Related Info.)
    uint32_t _reserved19[3];
    uint32_t OCOTP_MEM4;            //!< Value of OTP Bank1 Word4 (Memory Related Info.)
    uint32_t _reserved20[3];
    uint32_t OCOTP_ANA0;            //!< Value of OTP Bank1 Word5 (Memory Related Info.)
    uint32_t _reserved21[3];
    uint32_t OCOTP_ANA1;            //!< Value of OTP Bank1 Word6 (General Purpose Customer Defined Info.)
    uint32_t _reserved22[3];
    uint32_t OCOTP_ANA2;            //!< Value of OTP Bank1 Word7 (General Purpose Customer Defined Info.)
    uint32_t _reserved23[35];
    uint32_t OCOTP_SRK0;            //!< Shadow Register for OTP Bank3 Word0 (SRK Hash)
    uint32_t _reserved24[3];
    uint32_t OCOTP_SRK1;            //!< Shadow Register for OTP Bank3 Word1 (SRK Hash)
    uint32_t _reserved25[3];
    uint32_t OCOTP_SRK2;            //!< Shadow Register for OTP Bank3 Word2 (SRK Hash)
    uint32_t _reserved26[3];
    uint32_t OCOTP_SRK3;            //!< Shadow Register for OTP Bank3 Word3 (SRK Hash)
    uint32_t _reserved27[3];
    uint32_t OCOTP_SRK4;            //!< Shadow Register for OTP Bank3 Word4 (SRK Hash)
    uint32_t _reserved28[3];
    uint32_t OCOTP_SRK5;            //!< Shadow Register for OTP Bank3 Word5 (SRK Hash)
    uint32_t _reserved29[3];
    uint32_t OCOTP_SRK6;            //!< Shadow Register for OTP Bank3 Word6 (SRK Hash)
    uint32_t _reserved30[3];
    uint32_t OCOTP_SRK7;            //!< Shadow Register for OTP Bank3 Word7 (SRK Hash)
    uint32_t _reserved31[3];
    uint32_t OCOTP_HSJC_RESP0;      //!< Value of OTP Bank4 Word0 (Secure JTAG Response Field)
    uint32_t _reserved32[3];
    uint32_t OCOTP_HSJC_RESP1;      //!< Value of OTP Bank4 Word1 (Secure JTAG Response Field)
    uint32_t _reserved33[3];
    uint32_t OCOTP_MAC0;            //!< Value of OTP Bank4 Word2 (MAC Address)
    uint32_t _reserved34[3];
    uint32_t OCOTP_MAC1;            //!< Value of OTP Bank4 Word3 (MAC Address)
    uint32_t _reserved35[3];
    uint32_t OCOTP_MAC;
    uint32_t _reserved36[3];
    uint32_t OCOTP_CRC;
    uint32_t _reserved37[3];
    uint32_t OCOTP_GP1;             //!< Value of OTP Bank4 Word6 (HW Capabilities)
    uint32_t _reserved38[3];
    uint32_t OCOTP_GP2;             //!< Value of OTP Bank4 Word7 (HW Capabilities)
    uint32_t _reserved39[3];
    uint32_t OCOTP_SW_GP0;
    uint32_t _reserved40[3];
    uint32_t OCOTP_SW_GP1;
    uint32_t _reserved41[3];
    uint32_t OCOTP_SW_GP2;
    uint32_t _reserved42[3];
    uint32_t OCOTP_SW_GP3;
    uint32_t _reserved43[3];
    uint32_t OCOTP_SW_GP4;
    uint32_t _reserved44[3];
    uint32_t OCOTP_MISC_CONF;
    uint32_t _reserved45[3];
    uint32_t OCOTP_FIELD_RETURN;
    uint32_t _reserved46[3];
    uint32_t OCOTP_SRK_REVOKE;
    uint32_t _reserved47[67];
    uint32_t OCOTP_ROM_PATCH0;
    uint32_t _reserved48[3];
    uint32_t OCOTP_ROM_PATCH1;
    uint32_t _reserved49[3];
    uint32_t OCOTP_ROM_PATCH2;
    uint32_t _reserved50[3];
    uint32_t OCOTP_ROM_PATCH3;
    uint32_t _reserved51[3];
    uint32_t OCOTP_ROM_PATCH4;
    uint32_t _reserved52[3];
    uint32_t OCOTP_ROM_PATCH5;
    uint32_t _reserved53[3];
    uint32_t OCOTP_ROM_PATCH6;
    uint32_t _reserved54[3];
    uint32_t OCOTP_ROM_PATCH7;
    uint32_t _reserved55[3];
    uint32_t OCOTP_ROM_PATCH8;
    uint32_t _reserved56[3];
    uint32_t OCOTP_ROM_PATCH9;
    uint32_t _reserved57[3];
    uint32_t OCOTP_ROM_PATCH10;
    uint32_t _reserved58[3];
    uint32_t OCOTP_ROM_PATCH11;
    uint32_t _reserved59[3];
    uint32_t OCOTP_ROM_PATCH12;
    uint32_t _reserved60[3];
    uint32_t OCOTP_ROM_PATCH13;
    uint32_t _reserved61[3];
    uint32_t OCOTP_ROM_PATCH14;
    uint32_t _reserved62[3];
    uint32_t OCOTP_ROM_PATCH15;

    uint32_t _reserved63[3];
    uint32_t OCOTP_GP30;
    uint32_t _reserved64[3];
    uint32_t OCOTP_GP31;
    uint32_t _reserved65[3];
    uint32_t OCOTP_GP32;
    uint32_t _reserved66[3];
    uint32_t OCOTP_GP33;
    uint32_t _reserved67[3];
    uint32_t OCOTP_GP34;
    uint32_t _reserved68[3];
    uint32_t OCOTP_GP35;
    uint32_t _reserved69[3];
    uint32_t OCOTP_GP36;
    uint32_t _reserved70[3];
    uint32_t OCOTP_GP37;
    uint32_t _reserved71[3];
    uint32_t OCOTP_GP38;
    uint32_t _reserved72[3];
    uint32_t OCOTP_GP39;
    uint32_t _reserved73[3];
    uint32_t OCOTP_GP310;
    uint32_t _reserved74[3];
    uint32_t OCOTP_GP311;
    uint32_t _reserved75[3];
    uint32_t OCOTP_GP312;
    uint32_t _reserved76[3];
    uint32_t OCOTP_GP313;
    uint32_t _reserved77[3];
    uint32_t OCOTP_GP314;
    uint32_t _reserved78[3];
    uint32_t OCOTP_GP315;

    uint32_t _reserved79[3];
    uint32_t OCOTP_GP40;
    uint32_t _reserved80[3];
    uint32_t OCOTP_GP41;
    uint32_t _reserved81[3];
    uint32_t OCOTP_GP42;
    uint32_t _reserved82[3];
    uint32_t OCOTP_GP43;
    uint32_t _reserved83[3];
    uint32_t OCOTP_GP44;
    uint32_t _reserved84[3];
    uint32_t OCOTP_GP45;
    uint32_t _reserved85[3];
    uint32_t OCOTP_GP46;
    uint32_t _reserved86[3];
    uint32_t OCOTP_GP47;
    uint32_t _reserved87[3];
    uint32_t OCOTP_GP48;
    uint32_t _reserved88[3];
    uint32_t OCOTP_GP49;
    uint32_t _reserved89[3];
    uint32_t OCOTP_GP410;
    uint32_t _reserved90[3];
    uint32_t OCOTP_GP411;
    uint32_t _reserved91[3];
    uint32_t OCOTP_GP412;
    uint32_t _reserved92[3];
    uint32_t OCOTP_GP413;
    uint32_t _reserved93[3];
    uint32_t OCOTP_GP414;
    uint32_t _reserved94[3];
    uint32_t OCOTP_GP415;

    uint32_t _reserved95[3];
    uint32_t OCOTP_GP50;
    uint32_t _reserved96[3];
    uint32_t OCOTP_GP51;
    uint32_t _reserved97[3];
    uint32_t OCOTP_GP52;
    uint32_t _reserved98[3];
    uint32_t OCOTP_GP53;
    uint32_t _reserved99[3];
    uint32_t OCOTP_GP54;
    uint32_t _reserved100[3];
    uint32_t OCOTP_GP55;
    uint32_t _reserved101[3];
    uint32_t OCOTP_GP56;
    uint32_t _reserved102[3];
    uint32_t OCOTP_GP57;
    uint32_t _reserved103[3];
    uint32_t OCOTP_GP58;
    uint32_t _reserved104[3];
    uint32_t OCOTP_GP59;
    uint32_t _reserved105[3];
    uint32_t OCOTP_GP510;
    uint32_t _reserved106[3];
    uint32_t OCOTP_GP511;
    uint32_t _reserved107[3];
    uint32_t OCOTP_GP512;
    uint32_t _reserved108[3];
    uint32_t OCOTP_GP513;
    uint32_t _reserved109[3];
    uint32_t OCOTP_GP514;
    uint32_t _reserved110[3];
    uint32_t OCOTP_GP515;

    uint32_t _reserved111[3];
    uint32_t OCOTP_GP60;
    uint32_t _reserved112[3];
    uint32_t OCOTP_GP61;
    uint32_t _reserved113[3];
    uint32_t OCOTP_GP62;
    uint32_t _reserved114[3];
    uint32_t OCOTP_GP63;
    uint32_t _reserved115[3];
    uint32_t OCOTP_GP64;
    uint32_t _reserved116[3];
    uint32_t OCOTP_GP65;
    uint32_t _reserved117[3];
    uint32_t OCOTP_GP66;
    uint32_t _reserved118[3];
    uint32_t OCOTP_GP67;

    uint32_t _reserved119[3];
    uint32_t OCOTP_GP70;
    uint32_t _reserved120[3];
    uint32_t OCOTP_GP71;
    uint32_t _reserved121[3];
    uint32_t OCOTP_GP72;
    uint32_t _reserved122[3];
    uint32_t OCOTP_GP73;

    uint32_t _reserved123[3];
    uint32_t OCOTP_GP80;
    uint32_t _reserved124[3];
    uint32_t OCOTP_GP81;
    uint32_t _reserved125[3];
    uint32_t OCOTP_GP82;
    uint32_t _reserved126[3];
    uint32_t OCOTP_GP83;


} imx6ul_ocotp_regs_t;

#define BV_OCOTP_CTRL_WR_UNLOCK__KEY    (0x3e77)
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_IOMUXC_REGS_H */

/* end of file */
