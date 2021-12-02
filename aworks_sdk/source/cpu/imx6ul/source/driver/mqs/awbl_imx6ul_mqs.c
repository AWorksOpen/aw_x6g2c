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
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief The IMX6UL Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "aw_clk.h"
#include "driver/mqs/awbl_imx6ul_mqs.h"

/*******************************************************************************
  Global defines
*******************************************************************************/
struct awbl_imx6ul_mqs_dev *__gp_mqs_dev = NULL;

/*******************************************************************************
  Local defines
*******************************************************************************/

#define __MQS_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_mqs_dev *p_this = \
        (struct awbl_imx6ul_mqs_dev *)(p_dev)

#define __MQS_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx6ul_mqs_devinfo *p_devinfo = \
        (struct awbl_imx6ul_mqs_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  locals functions
*******************************************************************************/

/* Reset The MQS */
aw_err_t aw_imx6ul_mqs_reset (void)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

    volatile uint32_t *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;

    AW_REG_BIT_SET_MASK32(p_reg, (1 << 24));
    AW_REG_BIT_CLR_MASK32(p_reg, (1 << 24));

    return AW_OK;
}

/******************************************************************************/

/* Enable The MQS */
aw_err_t aw_imx6ul_mqs_enable (uint32_t samples_rate,
                               uint8_t  width,
                               uint8_t  channels)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

#define __MQS_IOMUXC_GRP2_MASK ((1 << 26) | (1 << 25) | (1 << 24) | (0xFF << 16))

    volatile uint32_t  *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;
    volatile uint32_t   tmp;
    int                 div;
    uint32_t            clk_rate;
    uint32_t            desired_rate;

    if ((width != 16)) {
        return -AW_EINVAL;
    }

    if (channels != 2) {
        return -AW_EINVAL;
    }

    /* The MQS only support the 44100 and 48000 */
    if ((samples_rate != 44100) && (samples_rate != 48000)) {
        return -AW_EINVAL;
    }

    /*
     * mclk_rate / (oversample(32,64) * FS * 2 * divider ) = repeat_rate;
     * if repeat_rate is 8, mqs can achieve better quality.
     * oversample rate is fix to 32 currently.
     * When the samples_rate is 44100, the div = clk_rate / 22579200
     * When the samples_rate is 48000, the div = clk_rate / 24576000
     */
    desired_rate = (32 * 2 * samples_rate * 8);
    clk_rate = aw_clk_rate_get(p_devinfo->ref_clk_id);

    if (clk_rate % desired_rate != 0) {
        return -AW_ENOTSUP;
    }
    
    div = clk_rate / desired_rate;

    if (div) {
        div--;
    }

    tmp = readl(p_reg);

    tmp &= ~__MQS_IOMUXC_GRP2_MASK;
    tmp |= (0 << 26) | (div << 16);

    writel(tmp, p_reg);
    AW_REG_BIT_SET_MASK32(p_reg, (1 << 25));

    return AW_OK;
}

/******************************************************************************/

/* Disable The MQS */
aw_err_t aw_imx6ul_mqs_disable (void)
{
    __MQS_DEVINFO_DECL(p_devinfo, __gp_mqs_dev);

    volatile uint32_t *p_reg = (volatile uint32_t *)p_devinfo->iomux_grp2_regaddr;

    AW_REG_BIT_CLR_MASK32(p_reg, (1 << 25));

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_init2 (awbl_dev_t *p_dev)
{
    __MQS_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    __gp_mqs_dev = (struct awbl_imx6ul_mqs_dev *)p_dev;

    aw_imx6ul_mqs_reset();
    aw_imx6ul_mqs_disable();

    return;
}

/******************************************************************************/

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __sai_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_sai_drvfuncs = {
    __sai_inst_init1,
    __sai_inst_init2,
    __sai_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_mqs_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB,                         /* bus_id */
        AWBL_IMX6UL_MQS_NAME,                   /* p_drvname */
        &__g_sai_drvfuncs,                      /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx6ul_mqs_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_mqs_drv_registration);
    return;
}

/* end of file */

