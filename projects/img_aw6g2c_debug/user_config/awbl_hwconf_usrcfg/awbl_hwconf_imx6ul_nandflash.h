/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief NandFlash configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  vih, first implementation
 * \endinternal
 */


#ifndef __AWBL_HWCONF_IMX6UL_NANDFLASH_H
#define __AWBL_HWCONF_IMX6UL_NANDFLASH_H

#if defined(AW_DEV_NAND_FLASH_S34ML02G2) || defined(AW_DEV_NAND_FLASH_S34ML01G2)

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include "driver/nand/awbl_imx6ul_nand.h"
#include "awbl_nand_pf.h"
#include "awbl_nand.h"
#include "awbl_nand_chip_attr.h"

#include "driver/busctlr/awbl_imx6ul_gpmi.h"


/******************************************************************************/
#ifdef AW_DEV_NAND_FLASH_S34ML02G2

/** \brief partition table */
aw_local aw_const struct awbl_nand_partition __g_imx6ul_nand_s34ml02g2_ptab[] = {
    {"/dev/mtd-syscfg",       97,       104     , 0},    /* 97 ~ 104块 存储触摸数据 */
    {"/dev/mtd-user",         105,      2047    , 0},    /* 保留  0 ~ 96块  */
};


/** \brief ECC存储位置描述  */
aw_local aw_const struct aw_mtd_oob_space __g_imx6ul_nand_s34ml02g2_ecc_pos[] = {
    // {2,   48},
    {2,   68},
};

/** \brief 空闲空间位置描述，注意：使用硬件BCH时，free oob区的布局应该为从ECC的结束地址开始到OOB区的结束地址 */
aw_local aw_const struct aw_mtd_oob_space __g_imx6ul_nand_s34ml02g2_free_pos[] = {
    // {56,  8},
    {70, 58},
};


/** \brief OOB布局描述数据 */
aw_local aw_const struct awbl_nand_ooblayout __g_imx6ul_nand_s34ml02g2_oob_layout = {
    __g_imx6ul_nand_s34ml02g2_ecc_pos,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml02g2_ecc_pos),
    __g_imx6ul_nand_s34ml02g2_free_pos,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml02g2_free_pos)
};

aw_local struct awbl_imx6ul_nand_info __g_imx6ul_nand_s34ml02g2_devinfo_priv = {
    {
         NANDFLASH_S34ML02G2_TIMING
    },
    AWBL_IMX6UL_GPMI_CE0,
};

/** \brief NandFlash device information */
aw_const awbl_nand_info_t __g_imx6ul_nand_s34ml02g2_devinfo = {
    {
         NANDFLASH_S34ML02G2
    },
    NANDFLASH_S34ML02G2_ECC_STRENGTH,
    5,
    __g_imx6ul_nand_s34ml02g2_ptab,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml02g2_ptab),
    AWBL_NAND_ECC_MODE_HW,  //    AWBL_NAND_ECC_MODE_HW  AWBL_NAND_ECC_MODE_SW_BCH
    &__g_imx6ul_nand_s34ml02g2_oob_layout,
    &__g_imx6ul_nand_s34ml02g2_devinfo_priv,
};

#endif /* AW_DEV_NAND_FLASH_S34ML02G2 */
/******************************************************************************/
#ifdef AW_DEV_NAND_FLASH_S34ML01G2

/** \brief partition table */
aw_local aw_const struct awbl_nand_partition __g_imx6ul_nand_s34ml01g2_ptab[] = {
    {"/dev/mtd-syscfg",       101,      300    , 0},        /* 101 ~ 300   */
    {"/dev/mtd-user",         301,      500    , 0},        /* 301 ~ 500   */
    {"/dev/rootfs",           501,     1023    , 0},        /* 501 ~ 1023  */
};


/** \brief ECC存储位置描述  */
aw_local aw_const struct aw_mtd_oob_space __g_imx6ul_nand_s34ml01g2_ecc_pos[] = {
    {2,   34},
};

/** \brief 空闲空间位置描述，注意：使用硬件BCH时，free oob区的布局应该为从ECC的结束地址开始到OOB区的结束地址 */
aw_local aw_const struct aw_mtd_oob_space __g_imx6ul_nand_s34ml01g2_free_pos[] = {
    {36,  28},
};

/** \brief OOB布局描述数据 */
aw_local aw_const struct awbl_nand_ooblayout __g_imx6ul_nand_s34ml01g2_oob_layout = {
    __g_imx6ul_nand_s34ml01g2_ecc_pos,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml01g2_ecc_pos),
    __g_imx6ul_nand_s34ml01g2_free_pos,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml01g2_free_pos)
};


aw_local struct awbl_imx6ul_nand_info __g_imx6ul_nand_s34ml01g2_devinfo_priv = {
    {
         NANDFLASH_S34ML01G2_TIMING
    },
    AWBL_IMX6UL_GPMI_CE0,
};

/** \brief NandFlash device information */
aw_const awbl_nand_info_t __g_imx6ul_nand_s34ml01g2_devinfo = {
    {
         NANDFLASH_S34ML01G2
    },
    NANDFLASH_S34ML01G2_ECC_STRENGTH,
    5,
    __g_imx6ul_nand_s34ml01g2_ptab,
    AW_NELEMENTS(__g_imx6ul_nand_s34ml01g2_ptab),
    AWBL_NAND_ECC_MODE_HW,  //    AWBL_NAND_ECC_MODE_HW  AWBL_NAND_ECC_MODE_SW_BCH
    &__g_imx6ul_nand_s34ml01g2_oob_layout,
    &__g_imx6ul_nand_s34ml01g2_devinfo_priv,
};

#endif /* AW_DEV_NAND_FLASH_S34ML01G2 */

/******************************************************************************/
aw_const awbl_nand_info_t *__g_imx6ul_nand_devinfo_arr[] = {

#ifdef AW_DEV_NAND_FLASH_S34ML01G2
    &__g_imx6ul_nand_s34ml01g2_devinfo,
#endif /* AW_DEV_NAND_FLASH_S34ML01G2 */

#ifdef AW_DEV_NAND_FLASH_S34ML02G2
    &__g_imx6ul_nand_s34ml02g2_devinfo,
#endif /* AW_DEV_NAND_FLASH_S34ML02G2 */

    NULL
};

/******************************************************************************/
/** \brief static memory of NandFlash device */
awbl_nand_t __g_imx6ul_nand_dev;

#define AWBL_HWCONF_IMX6UL_NANDFLASH               \
    {                                             \
        AWBL_NAND_DRV_NAME,            \
        1,                                  \
        AWBL_BUSID_NAND,                    \
        IMX6UL_NAND0_BUSID,                  \
        &(__g_imx6ul_nand_dev.awdev),    \
        __g_imx6ul_nand_devinfo_arr            \
    },

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#else
#define AWBL_HWCONF_IMX6UL_NANDFLASH

/*
 * defined(AW_DEV_NAND_FLASH_S34ML02G2) ||
 * defined(AW_DEV_NAND_FLASH_S34ML01G2)
 */
#endif


#endif /* __AWBL_HWCONF_IMX6UL_NANDFLASH_H */

/* end of file */

