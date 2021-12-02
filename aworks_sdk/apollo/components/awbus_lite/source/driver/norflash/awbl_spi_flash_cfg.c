/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SPI FLASH配置示例文件
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-07  orz, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "driver/norflash/awbl_spi_flash.h"
#include "driver/norflash/awbl_spi_flash_nvram.h"


AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);


aw_const struct awbl_dev_method g_spi_flash_methods[] = {

    AWBL_METHOD(awbl_nvram_get, awbl_spi_flash_nvram_get),
    AWBL_METHOD(awbl_nvram_set, awbl_spi_flash_nvram_set),

    AWBL_METHOD_END
};



void awbl_spi_flash_cfg (void)
{
    awbl_spi_flash_lib_init();
}


