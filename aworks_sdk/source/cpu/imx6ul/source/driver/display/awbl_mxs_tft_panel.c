/*******************************************************************************
*                                  Apollo
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
 * \brief IMX6UL tft panel
 *
 * \internal
 * \par modification history:
 * - 1.03 19-07-08  cat, delete interface.
 * - 1.02 14-11-03  ops, fix a bug in pan_backlight, awbl_mxs_hw480272f_t obj conversion.
 * - 1.01 14-08-01  ops, add operations
 * - 1.00 14-05-08  orz, first implementation.
 * \endinternal
 *
 * \Remarks  create by CYX at 2019.1.9
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "awbus_lite.h"

#include "driver/display/awbl_mxs_tft_panel.h"

/*******************************************************************************
    macros
*******************************************************************************/

#define __DEV_TO_THIS(p_dev) \
    AW_CONTAINER_OF(p_dev,awbl_mxs_tft_panel_t, dev)

#define __DEVINFO_GET(p_dev) \
   (awbl_mxs_tft_panel_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
    implementation
*******************************************************************************/
/**
 * \brief first level initialization routine of timers on chip
 *
 * This is the function called to perform the first level initialization.
 *
 * \note
 * This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 *
 * \note  This is the function called to perform the first level initialization.
 */
aw_local void __mxs_tft_panel_inst_init (struct awbl_dev *p_dev)
{
    awbl_mxs_tft_panel_t         *p_this    = __DEV_TO_THIS(p_dev);
    awbl_mxs_tft_panel_devinfo_t *p_devinfo = __DEVINFO_GET(p_this);

    /* platform initialize */
    if (p_devinfo->pfunc_plfm_init) {
        p_devinfo->pfunc_plfm_init();
    }
}

aw_local aw_err_t __mxs_tft_panel_panel_get (struct awbl_dev     *p_dev,
                                            awbl_mxs_panel_t   **pp_panel)
{
    awbl_mxs_tft_panel_t         *p_this    = __DEV_TO_THIS(p_dev);
    awbl_mxs_tft_panel_devinfo_t *p_devinfo = __DEVINFO_GET(p_this);
    awbl_mxs_panel_t             *p_panel   = &p_this->panel;

    /* initialize panel information */
    p_panel->p_panel_info   = &p_devinfo->panel_info;
    p_panel->p_lcdif        = NULL;  /* let fb fix it */
    p_panel->p_cookie       = (void *)p_this;

    /* retrieve the panel data */
    *pp_panel = p_panel;

    return AW_OK;
}

/*******************************************************************************
    driver register
*******************************************************************************/

/* driver functions */
aw_const struct awbl_drvfuncs __g_mxs_tft_panel_drvfuncs = {
    __mxs_tft_panel_inst_init,    /* devInstanceInit */
    NULL,                         /* devInstanceInit2 */
    NULL,                         /* devConnect */
};

AWBL_METHOD_IMPORT(awbl_mxs_panel_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_mxs_tft_panel_methods[] = {
    AWBL_METHOD(awbl_mxs_panel_get, __mxs_tft_panel_panel_get),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * IMX6ul compatible timer devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_mxs_tft_panel_drvinfo = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    AWBL_MXS_TFT_PANEL_NAME,        /* p_drvname */
    &__g_mxs_tft_panel_drvfuncs,     /* p_busfuncs */
    &__g_mxs_tft_panel_methods[0],   /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/**
 * \brief register imx6ul timer driver
 *
 * This routine registers the imx6ul timer driver and device
 * recognition data with the AWBus subsystem.
 */
void awbl_mxs_tft_panel_drv_register (void)
{
    awbl_drv_register(&__g_mxs_tft_panel_drvinfo);
}

/* end of file */
