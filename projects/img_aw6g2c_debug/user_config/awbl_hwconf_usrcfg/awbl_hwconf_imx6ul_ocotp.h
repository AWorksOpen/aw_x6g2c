/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  IMX6UL 内置OTP  配置信息
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_OCOTP_H_
#define AWBL_HWCONF_IMX6UL_OCOTP_H_

#include "driver/nvram/awbl_imx6ul_ocotp.h"
#include "imx6ul_regs_base.h"

#ifdef AW_DEV_IMX6UL_OCOTP

/**
 * \brief imx283 内部 OCOTP 存储段配置列表
 */
aw_local aw_const struct awbl_nvram_segment __g_imx6ul_ocotp_seglst[] = {
    /* 存储段名称, 单元号, 起始地址，长度 */
    {"otp_zy_data", 0,  0x180,  96},    /* 公司加密区间 */
    {"otp_zy_mac",  1,  0x88,   0x06},  /* MAC1 */
    {"otp_zy_mac",  2,  0x8E,   0x06},  /* MAC2 */
    {"otp_cust",    0,  0x1C0,  0x20},  /* Customer storage space   */
    {"otp_lock",    0,  0x00,   0x04},
    {"srk_hash",    0,  0x60,   0x20},
    {"otp_osp",     0,  0x04,   0x08},  /* 该字段只有0x4C~0x53可用, 存储着唯一ID号，只读 */
    {"otp_all",     0,  0x00,   0x200},

};

/* imx283 内部 OCOTP 设备信息 */
aw_local aw_const struct awbl_imx6ul_ocotp_devinfo __g_imx6ul_ocotp_devinfo = {
    &__g_imx6ul_ocotp_seglst[0],            /* 非易失性存储段配置列表 */
    IMX6UL_OCOTP_BASE_ADDR,                 /* 寄存器基址 */
    0,                                      /* 输入时钟ID */
    AW_NELEMENTS(__g_imx6ul_ocotp_seglst)   /* 非易失性存储段配置列表条目数 */
};

/* imx283 内部 OCOTP 设备实例内存静态分配 */
aw_local struct awbl_imx6ul_ocotp_dev __g_imx6ul_ocotp_dev;

#define AWBL_HWCONF_IMX6UL_OCOTP                    \
    {                                               \
        AWBL_IMX6UL_OCOTP_NAME,                     \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_ocotp_dev,   \
        &__g_imx6ul_ocotp_devinfo                   \
    },
#else
#define AWBL_HWCONF_IMX6UL_OCOTP
#endif  /* AW_DEV_IMX_OCOTP */

#endif /* AWBL_HWCONF_IMX6UL_OCOTP_H_ */
