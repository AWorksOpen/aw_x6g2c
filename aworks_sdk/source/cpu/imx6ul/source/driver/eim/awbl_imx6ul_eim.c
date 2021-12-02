/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief EIM source file
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-16  tee, first implementation
 * \endinternal
 */

/*******************************************************************************
    include
*******************************************************************************/
#include "apollo.h"
#include "aw_delay.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_clk.h"
#include "string.h"
#include "awbus_lite.h"

#include "driver/eim/awbl_imx6ul_eim.h"
#include "driver/eim/imx6ul_eim_regs.h"

#include "awbl_eim_bus.h"
#include "imx6ul_iomuxc_regs.h"

/*******************************************************************************
    macro operation
*******************************************************************************/
#define __IMX6UL_EIM_DEVINFO_DECL(p_info, p_awdev) \
        struct awbl_imx6ul_eim_devinfo *p_info = \
            (struct awbl_imx6ul_eim_devinfo *)AWBL_DEVINFO_GET(p_awdev)

#define __IMX6UL_EIM_DEV_DECL(p_dev, p_awdev) \
        struct awbl_imx6ul_eim_dev *p_dev = \
            (struct awbl_imx6ul_eim_dev *)(p_awdev)

/******************************************************************************/
aw_local void __eim_config (struct awbl_eim_master *p_master,
                            uint8_t                 cs_select,
                            void                   *p_info)
{
    __IMX6UL_EIM_DEV_DECL(p_dev, p_master);

    struct imx6ul_eim_config *cfg_info = (struct imx6ul_eim_config *)p_info;

    /* ÉèÖÃÅäÖÃ  */
    writel(cfg_info->GCR1,&(p_dev->cs_regbase[cs_select]->cfg.GCR1));
    writel(cfg_info->GCR2,&(p_dev->cs_regbase[cs_select]->cfg.GCR2));
}

/******************************************************************************/
aw_local void __eim_select (struct awbl_eim_master *p_master,
                            uint8_t                 cs_select,
                            void                   *p_info)
{
    __IMX6UL_EIM_DEV_DECL(p_dev, p_master);

    struct imx6ul_eim_timing *timing_info = (struct imx6ul_eim_timing *)p_info;

    /* ÉèÖÃÊ±Ðò  */
    writel(timing_info->RCR1,&(p_dev->cs_regbase[cs_select]->timing.RCR1));
    writel(timing_info->RCR2,&(p_dev->cs_regbase[cs_select]->timing.RCR2));
    writel(timing_info->WCR1,&(p_dev->cs_regbase[cs_select]->timing.WCR1));
}

/******************************************************************************/
aw_local void __eim_write (struct awbl_eim_master *p_master,
                               uint8_t             cs_select,
                               uint32_t            addr,
                               uint16_t            data)
{
    __IMX6UL_EIM_DEV_DECL(p_dev, p_master);

    uint16_t *w_addr = (uint16_t *)(p_dev->cs_transmission_base[cs_select]);
    *((volatile uint16_t *)(w_addr+addr)) = data;
}

/******************************************************************************/
aw_local uint16_t __eim_read (struct awbl_eim_master *p_master,
                              uint8_t                 cs_select,
                              uint32_t                addr)
{
    __IMX6UL_EIM_DEV_DECL(p_dev, p_master);

    uint8_t data;
    uint16_t *r_addr = (uint16_t *)(p_dev->cs_transmission_base[cs_select]);
    data = *((const volatile uint16_t *)(r_addr+addr));

    return data;
}

/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_eim_master_devinfo2 __g_eim_devinfo2 = {
        __eim_config,
        __eim_select,
        __eim_write,
        __eim_read,
};

/******************************************************************************/
aw_local void __eim_hard_init(void)
{
    uint32_t ctrl = IOMUX_GPR_REG_GET(IOMUXC_GPR_GPR1, 0, 12);

    ctrl |= __EIM_CS_EN;
    ctrl &= ~(0x3<<1);
    ctrl |= __EIM_CS_SPACE;
    IOMUX_GPR_REG_MOD(IOMUXC_GPR_GPR1, 0, 12, ctrl);
}

/******************************************************************************/
aw_local void __eim_reg_config (struct imx6ul_eim_reg   *regbase,
                                struct awbl_eim_config  *cfg)
{
    uint32_t gcr1 = 0;
    uint32_t gcr2 = 0;
    uint32_t rcr1 = 0;
    uint32_t rcr2 = 0;
    uint32_t wcr1 = 0;
    uint32_t wcr2 = 0;

    gcr1 = __EIM_SET_CS(cfg->gcr.CSEN) | __EIM_SET_SWR(cfg->gcr.SWR) | __EIM_SET_SRD(cfg->gcr.SRD) | \
           __EIM_SET_MUM(cfg->gcr.MUM) | __EIM_SET_WFL(cfg->gcr.WFL) | __EIM_SET_RFL(cfg->gcr.RFL) | \
           __EIM_SET_CRE(cfg->gcr.CRE) | __EIM_SET_CREP(cfg->gcr.CREP) | __EIM_SET_BL(cfg->gcr.BL) | \
           __EIM_SET_WC(cfg->gcr.WC) | __EIM_SET_BCD(cfg->gcr.BCD) | __EIM_SET_BCS(cfg->gcr.BCS) | \
           __EIM_SET_DSZ(cfg->gcr.DSZ) | __EIM_SET_SP(cfg->gcr.SP) | __EIM_SET_CSREC(cfg->gcr.CSREC) | \
           __EIM_SET_AUS(cfg->gcr.AUS) | __EIM_SET_GBC(cfg->gcr.GBC) | __EIM_SET_WP(cfg->gcr.WP) | \
           __EIM_SET_PZS(cfg->gcr.PZS);

    gcr2 = __EIM_SET_ADH(cfg->gcr.ADH) | __EIM_SET_DAPS(cfg->gcr.DAPS) | __EIM_SET_DAE(cfg->gcr.DAE) | \
            __EIM_SET_DAP(cfg->gcr.DAP) | __EIM_SET_BYP(cfg->gcr.MUX16_BYP_GRANT);

    rcr1 = __EIM_SET_RCSN(cfg->rcr.RCSN) | __EIM_SET_RCSA(cfg->rcr.RCSA) | __EIM_SET_OEN(cfg->rcr.OEN) | \
           __EIM_SET_OEA(cfg->rcr.OEA) | __EIM_SET_RADVN(cfg->rcr.RADVN) | __EIM_SET_RAL(cfg->rcr.RAL) | \
           __EIM_SET_RADVA(cfg->rcr.RADVA) | __EIM_SET_RWSC(cfg->rcr.RWSC);

    rcr2 = __EIM_SET_RBEN(cfg->rcr.RBEN) | __EIM_SET_RBE_EN(cfg->rcr.RBE_EN) | __EIM_SET_RBEA(cfg->rcr.RBEA) | \
           __EIM_SET_RL(cfg->rcr.RL) | __EIM_SET_PAT(cfg->rcr.PAT) | __EIM_SET_APR(cfg->rcr.APR);

    wcr1 = __EIM_SET_WCSN(cfg->wcr.WCSN) | __EIM_SET_WCSA(cfg->wcr.WCSA) | __EIM_SET_WEN(cfg->wcr.WEN) | \
           __EIM_SET_WEA(cfg->wcr.WEA) | __EIM_SET_WBEN(cfg->wcr.WBEN) | __EIM_SET_WBEA(cfg->wcr.WBEA) | \
           __EIM_SET_WADVN(cfg->wcr.WADVN) | __EIM_SET_WADVA(cfg->wcr.WADVA) | __EIM_SET_WWSC(cfg->wcr.WWSC) | \
           __EIM_SET_WBED(cfg->wcr.WBED) | __EIM_SET_WAL(cfg->wcr.WAL);

    wcr2 = __EIM_SET_WBCDD(cfg->wcr.WBCDD);

    /* ÉèÖÃÅäÖÃ  */
    writel(gcr1,&(regbase->cfg.GCR1));
    writel(gcr2,&(regbase->cfg.GCR2));
    writel(rcr1,&(regbase->timing.RCR1));
    writel(rcr2,&(regbase->timing.RCR2));
    writel(wcr1,&(regbase->timing.WCR1));
    writel(wcr2,&(regbase->timing.WCR2));
}

/******************************************************************************/
aw_local void __eim_inst_init2 (struct awbl_dev *p_awdev)
{
    __IMX6UL_EIM_DEVINFO_DECL(p_info, p_awdev);
    __IMX6UL_EIM_DEV_DECL(p_dev, p_awdev);

    struct awbl_eim_master        *p_master;

    int i;

    /* platform initialization */
    if (p_info->pfn_plfm_init != NULL) {
        p_info->pfn_plfm_init();
    }

    /* Chip Select and Address Space */
    __eim_hard_init();

    for (i=0; i<4; i++) {
        p_dev->cs_regbase[i] = \
                ((struct imx6ul_eim_reg *)p_info->regbase  + i);
        p_dev->cs_transmission_base[i] = \
                p_info->transmission_base + i * p_info->cs_size;

        __eim_reg_config(p_dev->cs_regbase[i], &(p_info->cfg[i]));
    }

    p_master = &(p_dev->super);
    p_master->p_devinfo2 = &__g_eim_devinfo2;

    awbl_eimbus_create(p_master);

    return;
}
/******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_imx6ul_eim_drvfuncs = {
        NULL,
        __eim_inst_init2,
        NULL
};

aw_local aw_const struct awbl_drvinfo __g_imx6ul_eim_drv_registration = {
    AWBL_VER_1,                           /* awb_ver */
    AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,  /* bus_id */
    AWBL_IMX6UL_EIM_NAME,                 /* p_drvname */
    &__g_imx6ul_eim_drvfuncs,             /* p_busfuncs */
    NULL,                                 /* p_methods */
    NULL                                  /* pfunc_drv_probe */
};


void awbl_imx6ul_eim_drv_register (void)
{
    aw_err_t err;
    err = awbl_drv_register(&__g_imx6ul_eim_drv_registration);
    aw_assert (err == 0);
    return;
}
