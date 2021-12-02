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

#ifndef __AWBL_HWCONF_SPI_FLASH0_H
#define __AWBL_HWCONF_SPI_FLASH0_H

#ifdef  AW_DEV_SPI_FLASH0

#include "aw_gpio.h"
#include "aw_spi.h"
#include "driver/norflash/awbl_spi_flash.h"
#include "driver/norflash/awbl_spi_flash_mtd.h"
#include "driver/norflash/awbl_spi_flash_nvram.h"

#ifdef AW_CFG_SPI_FLASH_NVRAM

/* NVRAM 接口缓存 */
aw_local uint8_t __g_snvram_buf0[4096] = {0};

/* NVRAM 段定义 */
aw_local aw_const struct awbl_nvram_segment __g_snvram_seglst0[] = {
    /* 存储段名称, 单元号, 起始地址，长度 */
    {"spi_flash0_nvram",  0, 0, 8*1024*1024},
    {"spi_flash0_nvram0", 0, 5*1024*1024, 4096},
    {"spi_flash0_nvram1", 0, 6*1024*1024, 4096},
    {"spi_flash0_nvram2", 0, 7*1024*1024, 4096},
};

/* NVRAM 信息结构 */
aw_local aw_const struct awbl_spi_flash_nvram_info __g_fnvram_info0 = {
    __g_snvram_seglst0,
    AW_NELEMENTS(__g_snvram_seglst0),
    __g_snvram_buf0
};
#endif


#ifdef AW_CFG_SPI_FLASH_MTD
/* MTD 信息结构 */
aw_local aw_const struct awbl_spi_flash_mtd_info __g_fmtd_info0  = {
        1024,   /* start block for MTD */
        1024    /* number of blocks for MTD */
};

/* MTD 对象结构 */
aw_local struct awbl_spi_flash_mtd __g_fmtd0;

/* MTD 拓展结构 */
struct awbl_spi_flash_mtd_ex __g_fmtd_ex0 = {
        &__g_fmtd0,
        &__g_fmtd_info0,
        awbl_spi_flash_mtd_init
};
#endif


aw_local aw_const struct awbl_spi_flash_devinfo __g_spi_flash_devinfo0 = {
        "/sflash0",                         /* flash 注册成块设备的名字 */
        4096,                               /* flash 擦除操作的块大小 */
        2048,                               /* flash 对应块数量 */
        256,                                /* flash 写操作的页大小 */
        AW_SPI_MODE_0 ,                     /* 接口时序模式 */
        GPIO4_1,                            /* 片选引脚 */
        30000000,                           /* SPI总线时钟 */
#ifdef AW_CFG_SPI_FLASH_MTD
        &__g_fmtd_ex0,
#else
        NULL,
#endif
#ifdef AW_CFG_SPI_FLASH_NVRAM
        (void *)&__g_fnvram_info0,
#else
        NULL,
#endif
        NULL                                /* 初始化 */
};


/* SPI FLASH 设备实例内存静态分配  */
aw_local struct awbl_spi_flash_dev __g_spi_flash_dev0;

#define AWBL_HWCONF_SPI_FLASH0                 \
    {                                          \
        AWBL_SPI_FLASH_NAME,                   \
        0,                                     \
        AWBL_BUSID_SPI,                        \
        IMX6UL_QSPI1_BUSID,                    \
        &(__g_spi_flash_dev0.spi.super),       \
        &__g_spi_flash_devinfo0                \
    },

#else
#define AWBL_HWCONF_SPI_FLASH0

#endif  /* AW_DEV_SPI_FLASH0 */

#endif  /* __AWBL_HWCONF_SPI_FLASH0_H */


/* end of file */
