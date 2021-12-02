/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus IMX6UL on chip watchdog interface header file
 *
 * use this module should include follows :
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-07-01  sky, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_WDT_H
#define __AWBL_IMX6UL_WDT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"

/*******************************************************************************
  define
*******************************************************************************/

/** \brief define IMX6UL watchdog's name */
#define  IMX6UL_WDT_NAME  "imx6ul-wdt"

/** \brief define IMX6UL watchdog's time info */
typedef struct  awbl_imx6ul_wdt_devinfo {
    uint32_t    reg_base;           /**< \brief RTC register base address */
    uint32_t    wdt_time;           /**< \brief watchdog time (ms)*/
    void (*pfn_plfm_init) (void);   /**< \brief platform init */
} awbl_imx6ul_wdt_devinfo_t;

/** \brief define IMX6UL watchdog's device */
typedef struct  awbl_imx6ul_wdt_dev {
    struct awbl_dev          dev;    /**< the AWBus device */
    struct awbl_hwwdt_info   info;   /**< hw wdt info */
    uint32_t                 reg_base;
} awbl_imx6ul_wdt_dev_t;

/**
 * \brief IMX6UL wdt driver register
 */
void awbl_imx6ul_wdt_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_WDT_H */

/* end of file */
