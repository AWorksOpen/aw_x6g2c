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
 * \brief AWBus IMX6UL on chip watchdog driver source file
 *
 * \internal
 * \par Modification History
 * - 1.00 16-07-01  sky, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "driver/wdt/awbl_imx6ul_wdt.h"

/*******************************************************************************
  typedef
*******************************************************************************/

/**
 * \brief imx6ul wdt register block
 */
typedef struct __imx6ul_wdt_regs {
    uint16_t wcr;      /**< \brief Watchdog Control Register */
    uint16_t wsr;      /**< \brief Watchdog Service Register */
    uint16_t wrsr;     /**< \brief Watchdog Reset Status Register */
    uint16_t wicr;     /**< \brief Watchdog Interrupt Control Register */
    uint16_t wmcr;     /**< \brief Watchdog Miscellaneous Control Register */
} __imx6ul_wdt_regs_t;

/*******************************************************************************
  define
*******************************************************************************/

/**
 * \brief tempmon reg_field
 * @{
 */

#define __WDT_WDZST               0x01          /* Watchdog Low Power */
#define __WDT_WDBG                0x02          /* Watchdog DEBUG Enable */
#define __WDT_WDE                 0x04          /* enable wdt */
#define __WDT_WDT                 0x08          /* enable time_out */
#define __WDT_SRS                 0x10          /* Software Reset Signal */
#define __WDT_WT_SET(x)           ((x) << 8)    /* countdown timer count (0.5s per 1) */
#define __WDT_WSR_5555            0x5555        /* reload first cmd */
#define __WDT_WSR_AAAA            0xAAAA        /* reload second cmd */
#define __WDT_PDE                 0             /* Power Down Counter of WDOG is disabled. */

/** @} */

#define __IMX6UL_WDT_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx6ul_wdt_devinfo *p_devinfo = \
        (struct awbl_imx6ul_wdt_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define  __INFO_TO_IMX6UL_WDT(p_info) \
    AW_CONTAINER_OF(p_info, struct awbl_imx6ul_wdt_dev, info)

/******************************************************************************/

/** \brief watchdog feed */
aw_local void __imx6ul_wdt_feed (struct awbl_hwwdt_info *pwdt_info)
{
    struct awbl_imx6ul_wdt_dev *p_wdt_dev = __INFO_TO_IMX6UL_WDT(pwdt_info);
    __imx6ul_wdt_regs_t        *p_reg     = (__imx6ul_wdt_regs_t *)(p_wdt_dev->reg_base);

    writew(__WDT_WSR_5555, &p_reg->wsr);    /* first reload */
    writew(__WDT_WSR_AAAA, &p_reg->wsr);    /* second reload */
}

/******************************************************************************/

/** \brief instance initializing stage 1 */
aw_local void  __imx6ul_wdt_inst_init (struct awbl_dev *p_dev)
{
    __IMX6UL_WDT_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_imx6ul_wdt_dev  *p_wdt_dev = (struct awbl_imx6ul_wdt_dev *)p_dev;
    __imx6ul_wdt_regs_t         *p_reg     = (__imx6ul_wdt_regs_t *)(p_devinfo->reg_base);

    uint16_t reg;

    p_wdt_dev->info.wdt_time = p_devinfo->wdt_time;
    p_wdt_dev->info.feed     = __imx6ul_wdt_feed;
    p_wdt_dev->reg_base      = p_devinfo->reg_base;

    /* platform init (enable wdt clock) */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* register IMX6UL watchdog info */
    awbl_wdt_device_register(&p_wdt_dev->info);

    /* disable Power Down Counter of WDOG */
    reg = readw(&p_reg->wmcr);
    reg &= ~AW_BIT(__WDT_PDE);
    writew(reg, &p_reg->wmcr);

    /* set IMX6UL watchdog watch time & open */
    writew((__WDT_WDZST | __WDT_WDBG | __WDT_WDE | __WDT_WDT | __WDT_SRS |
            __WDT_WT_SET(p_devinfo->wdt_time / 500 -1)), &p_reg->wcr);

    /* load count once */
    writew(__WDT_WSR_5555, &p_reg->wsr);    /* first reload */
    writew(__WDT_WSR_AAAA, &p_reg->wsr);    /* second reload */
}

/*******************************************************************************
  locals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_imx6ul_wdt = {
    __imx6ul_wdt_inst_init, /* devInstanceInit */
    NULL,                   /* devInstanceInit2 */
    NULL                    /* devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_imx6ul_wdt = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    IMX6UL_WDT_NAME,                    /* p_drvname */
    &__g_awbl_drvfuncs_imx6ul_wdt,      /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/******************************************************************************/
void awbl_imx6ul_wdt_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_imx6ul_wdt);
}

/* end of file */
