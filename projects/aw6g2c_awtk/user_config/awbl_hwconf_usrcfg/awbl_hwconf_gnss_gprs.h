/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_GNSS_GPRS_H
#define __AWBL_HWCONF_GNSS_GPRS_H

#ifdef AW_DEV_GNSS_GPRS
#include "driver/gps/awbl_gnss_gprs.h"
#include "driver/gprs/awbl_gprs_ec20.h"
#include "aw_serial.h"


/*******************************************************************************
    配置信息
*******************************************************************************/

aw_local struct awbl_gnss_gprs_devinfo __g_gnss_gnss_gprs_devinfo = {

    .gnss_servinfo = {
        GNSS_ID_GPRS                           /* GNSS 设备编号  */
    },

    .gnss_cfg = {
        .startup_mode       = AW_GPRS_GNSS_STARTUP_AUTO,
        .gps.flag_enable    = AW_TRUE,
        .gps.nmea_type      = AW_GPRS_GNSS_NMEA_RMC,
    },

    .gprs_dev_id = AWBL_GPRS_EC20_ID_BASE,
};

aw_local struct awbl_gnss_gprs_dev __g_gnss_gnss_gprs_dev;

#define AWBL_HWCONF_GNSS_GPRS                                \
    {                                                     \
		AWBL_GNSS_GPRS,                                 \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_gnss_gnss_gprs_dev.super,    \
        &__g_gnss_gnss_gprs_devinfo                          \
    },

#else
#define AWBL_HWCONF_GNSS_GPRS

#endif    /* AW_DEV_GNSS_GPRS */

#endif  /* __AWBL_HWCONF_GNSS_GPRS_H */

/* end of file */
