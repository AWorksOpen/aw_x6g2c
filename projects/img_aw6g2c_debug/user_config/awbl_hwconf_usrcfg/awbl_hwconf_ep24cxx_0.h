/*******************************************************************************
  EEPROM CAT24C04 配置信息
*******************************************************************************/
#ifndef AWBL_HWCONFG_EP24CXX_0_H
#define AWBL_HWCONFG_EP24CXX_0_H

#include "driver/nvram/awbl_ep24cxx.h"
#ifdef AW_DEV_EXTEND_EP24CXX_0

/* 存储段配置列表【1】 */
aw_local aw_const struct awbl_nvram_segment __g_ep24cxx_seglst[] = {

    /* 存储段名称, 存储段单元号, 起始地址, 长度 */
    {"ep24c02", 0, 0, 256},             /* 整个EEPROM存储空间 */
    {"data", 0, 0, 16},                 /* 存储段 "data"_0 */
    {"data", 1, 16, 16},                /* 存储段 "data"_1 */
    {"data2", 0, 32, 16}                /* 存储段 "data2"_0 */
};

/* CAT24C256 设备信息 */
aw_local aw_const struct awbl_ep24cxx_devinfo __g_ep24cxx_devinfo = {
    0x50,                               /* I2C从机地址 【2】 */
    AWBL_EP24CXX_EP24C02,               /* 芯片型号 【3】 */
    &__g_ep24cxx_seglst[0],             /* 非易失性存储段配置列表 */
    AW_NELEMENTS(__g_ep24cxx_seglst)    /* 非易失性存储段配置列表条目数 */
};

/* CAT24C04 0 设备实例内存静态分配 */
aw_local struct awbl_ep24cxx_dev __g_ep24cxx_dev;

/*******************************************************************************
  硬件设备列表
*******************************************************************************/

#define AWBL_HWCONF_EP24CXX_0  \
    {                          \
        "ep24cxx",             \
        0,                     \
        AWBL_BUSID_I2C,        \
        IMX6UL_I2C1_BUSID,     \
        (struct awbl_dev *)&__g_ep24cxx_dev, \
        &__g_ep24cxx_devinfo   \
    },
#else
#define AWBL_HWCONF_EP24CXX_0

#endif
#endif
