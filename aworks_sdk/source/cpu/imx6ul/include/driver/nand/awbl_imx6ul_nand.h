/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx6ul nandflash driver head file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 20-04-14 vih, refactoring
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_NAND_H
#define __AWBL_IMX6UL_NAND_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aworks.h"
#include "awbus_lite.h"
#include "mtd/aw_mtd.h"

#include "awbl_nand_pf.h"
#include "awbl_nand_bus.h"


struct awbl_imx6ul_nand_dev;

/** \brief  NandFlash芯片控制器时序 */
struct awbl_imx6ul_nand_timing {

    /* ns for all */
    uint8_t        tcs;        /**< \brief CE setup */
    uint8_t        tch;        /**< \brief CE hold */
    uint8_t        tcls;       /**< \brief CLE setup */
    uint8_t        tclh;       /**< \brief CLE hold */
    uint8_t        tals;       /**< \brief ALE setup */
    uint8_t        talh;       /**< \brief ALE hold */
    uint8_t        tds;        /**< \brief data setup */
    uint8_t        tdh;        /**< \brief data hold */
    uint8_t        trp;        /**< \brief RE pulse width */
    uint8_t        trea;       /**< \brief RE access time */
    uint8_t        tcea;       /**< \brief CE access time */
    uint8_t        tchz;       /**< \brief CE high to output Hi-Z */
    uint8_t        trc;        /**< \brief read cycle time */
    uint8_t        trhz;       /**< \brief RE high to output Hi-Z */
    uint8_t        tclr;       /**< \brief CLE to RE delay */
    uint8_t        twp;        /**< \brief WE pulse width */
    uint8_t        twc;        /**< \brief WE high hold time */
};

struct awbl_imx6ul_nand_info {
    /** \brief 控制器操作Nand芯片的时序 */
    struct awbl_imx6ul_nand_timing       tim;

    /** \brief 控制器在对Nand芯片进行片选操作时所需要的id */
    int                                  gpmi_ce_id;
};

//struct awbl_imx6ul_nand_dev {
//
//    struct awbl_nand_chip nand;
//
//    struct awbl_nandbus  *p_bus;
//
//    struct awbl_imx6ul_nand_info *p_info;
//
//    void                         *p_bch_geo;
//
//    /* General-use Variables */
//    int         current_chip;
//
//    /* private */
//    void            *priv;
//};

//void awbl_imx6ul_nand_drv_register (void);

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif    /* __AWBL_IMX6UL_NAND_H */


