/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SPI_FLASH1_H
#define __AWBL_HWCONF_SPI_FLASH1_H

#ifdef  AW_DEV_SPI_FLASH1

#include "aw_gpio.h"
#include "aw_spi.h"
#include "driver/norflash/awbl_spi_flash.h"
#include "driver/norflash/awbl_spi_flash_mtd.h"
#include "driver/norflash/awbl_spi_flash_nvram.h"


#ifdef AW_CFG_SPI_FLASH_NVRAM

/* NVRAM �ӿڻ��� */
aw_local uint8_t __g_snvram_buf1[4096] = {0};

/* NVRAM �ζ��� */
aw_local aw_const struct awbl_nvram_segment __g_snvram_seglst1[] = {
    /* �洢������, ��Ԫ��, ��ʼ��ַ������ */
    {"spi_flash1_nvram",  0, 0, 8*1024*1024},
    {"spi_flash1_nvram0", 0, 5*1024*1024, 4096},
    {"spi_flash1_nvram1", 0, 6*1024*1024, 4096},
    {"spi_flash1_nvram2", 0, 7*1024*1024, 4096},
};

/* NVRAM ��Ϣ�ṹ */
aw_local aw_const struct awbl_spi_flash_nvram_info __g_fnvram_info1 = {
    __g_snvram_seglst1,
    AW_NELEMENTS(__g_snvram_seglst1),
    __g_snvram_buf1
};
#endif


#ifdef AW_CFG_SPI_FLASH_MTD
/* MTD ��Ϣ�ṹ */
aw_local aw_const struct awbl_spi_flash_mtd_info __g_fmtd_info1  = {
        1024,   /* start block for MTD */
        1024    /* number of blocks for MTD */
};

/* MTD ����ṹ */
aw_local struct awbl_spi_flash_mtd __g_fmtd1;

/* MTD ��չ�ṹ */
struct awbl_spi_flash_mtd_ex __g_fmtd_ex1 = {
        &__g_fmtd1,
        &__g_fmtd_info1,
        awbl_spi_flash_mtd_init
};
#endif


aw_local aw_const struct awbl_spi_flash_devinfo __g_spi_flash1_devinfo0 = {
        "/sflash1",                         /* flash ע��ɿ��豸������ */
        4096,                               /* flash ���������Ŀ��С */
        2048,                               /* flash ��Ӧ������ */
        256,                                /* flash д������ҳ��С */
        AW_SPI_MODE_0                 ,     /* �ӿ�ʱ��ģʽ */
        GPIO4_26,                            /* Ƭѡ���� */
        100000,                            /* SPI����ʱ�� */
#ifdef AW_CFG_SPI_FLASH_MTD
        &__g_fmtd_ex1,
#else
        NULL,
#endif
#ifdef AW_CFG_SPI_FLASH_NVRAM
        (void *)&__g_fnvram_info1,
#else
        NULL,
#endif
        NULL                                /* ��ʼ�� */
};


/* SPI FLASH �豸ʵ���ڴ澲̬����  */
aw_local struct awbl_spi_flash_dev __g_spi_flash1_dev0;

#define AWBL_HWCONF_SPI_FLASH1                 \
    {                                          \
        AWBL_SPI_FLASH_NAME,                   \
        0,                                     \
        AWBL_BUSID_SPI,                        \
        IMX6UL_ECSPI1_BUSID,                   \
        &(__g_spi_flash1_dev0.spi.super),      \
        &__g_spi_flash1_devinfo0               \
    },

#else
#define AWBL_HWCONF_SPI_FLASH1

#endif  /* AW_DEV_SPI_FLASH0 */

#endif  /* __AWBL_HWCONF_SPI_FLASH1_H */

/* end of file */
