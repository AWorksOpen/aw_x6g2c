/*******************************************************************************
*                                 Apollo
*                       ----------------------------
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
 * \brief Freescale i.MX6ul系列CPU通用中断控制器驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-05-06  zen, first implemetation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_assert.h"
#include "aw_exc.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "driver/intctlr/awbl_imx6ul_intc.h"
#include "gic.h"
#include <string.h>


/*******************************************************************************
  defines
*******************************************************************************/

#define __INTC_GET_REG_BASE(p_this) \
    ((uint32_t) \
     (((awbl_imx6ul_intc_devinfo_t *)AWBL_DEVINFO_GET(p_this))->regbase))

#define __INTC_DEV_TO_THIS(p_dev) \
    (awbl_imx6ul_intc_dev_t *)(p_dev)

#define __INTC_DEV_TO_PARAM(p_dev) \
    (awbl_imx6ul_intc_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __INTC_DEV_LOCK_INIT(p_this) \
    aw_spinlock_isr_init(&(p_this)->devlock, 0);

#define __INTC_DEV_LOCK(p_this) \
    aw_spinlock_isr_take(&(p_this)->devlock);

#define __INTC_DEV_UNLOCK(p_this) \
    aw_spinlock_isr_give(&(p_this)->devlock);


/*******************************************************************************
  typedefs
*******************************************************************************/

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __intc_inst_init1(awbl_dev_t *p_dev);
aw_local void __intc_inst_init2(awbl_dev_t *p_dev);
aw_local void __intc_inst_connect(awbl_dev_t *p_dev);

aw_err_t __intc_int_connect(void            *p_cookie,
                            int              inum,
                            aw_pfuncvoid_t   pfunc_isr,
                            void            *p_arg);

aw_err_t __intc_int_disconnect(void            *p_cookie,
                               int              inum,
                               aw_pfuncvoid_t   pfunc_isr,
                               void            *p_arg);

aw_err_t __intc_int_enable(void *p_cookie, int inum);

aw_err_t __intc_int_disable(void *p_cookie, int inum);


/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_intc_drvfuncs = {
    __intc_inst_init1,
    __intc_inst_init2,
    __intc_inst_connect
};

/* intctlr service functions (must defined as aw_const) */
aw_local aw_const struct awbl_intctlr_servfuncs __g_intc_servfuncs = {
    __intc_int_connect,                 /* pfunc_int_connect */
    __intc_int_disconnect,              /* pfunc_int_disconnect */
    __intc_int_enable,                  /* pfunc_int_enable */
    __intc_int_disable                  /* pfunc_int_disable */
};


/* driver registration (must defined as aw_const) */

aw_local aw_const awbl_plb_drvinfo_t __g_intc_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_IMX6UL_INTC_NAME,      /* p_drvname */
        &__g_intc_drvfuncs,         /* p_busfuncs */
        NULL,
        NULL                        /* pfunc_drv_probe */
    }
};

/* pointer to INTC */
aw_local awbl_imx6ul_intc_dev_t *__gp_intc_dev = NULL;
aw_local uint32_t               __g_ui_gic_reg_base = 0;

/*******************************************************************************
  implementation
*******************************************************************************/

uint32_t get_arm_private_peripheral_base()
{
    return __g_ui_gic_reg_base;
}
/**
 * \brief armv6m EINT exception service routine
 */
aw_local void __eint_esr (void)
{
    aw_pfuncvoid_t          ppfunc_isr;
    void                    *p_arg;
    unsigned                cpu,irq;
    uint32_t                vectNum;
    awbl_imx6ul_intc_dev_t  *p_this;

    /* send ack and get ID source */
    vectNum = gic_read_irq_ack();

    /* Check that INT_ID isn't 1023 or 1022 (spurious interrupt) */
    if ( !(vectNum & 0x0200)  )
    {
        //get cpu and irq
        cpu = (vectNum >> 10) & 0x7;
        irq = vectNum & 0x1FF;
        cpu = cpu;

        p_this = __gp_intc_dev;
        ppfunc_isr = p_this->isrinfo[irq].pfunc_isr;
        p_arg = p_this->isrinfo[irq].p_arg;

        if(NULL != ppfunc_isr)
        {
            (*ppfunc_isr)(p_arg);
        }
        // send end of irq
    }

    /* Signal the end of the irq. */
    gic_write_end_of_irq(vectNum);
}

/* todo: remove it */
void irq_entry (void)
{
    ENTER_INT_CONTEXT();
	__eint_esr();
	EXIT_INT_CONTEXT();
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __intc_inst_init1 (awbl_dev_t *p_dev)
{
    awbl_imx6ul_intc_dev_t         *p_this = __INTC_DEV_TO_THIS(p_dev);
    struct awbl_intctlr_service    *p_serv = NULL;
    awbl_imx6ul_intc_devinfo_t      *p_devinfo = __INTC_DEV_TO_PARAM(p_dev);

    /* clean isrinfo-table */
    memset(p_this->isrinfo, 0, sizeof(p_this->isrinfo));
    __g_ui_gic_reg_base = (uint32_t)__INTC_GET_REG_BASE(p_this);
    /* initialize interrupt controller hardware */
    gic_init();

    /* initialize spinlock */
    __INTC_DEV_LOCK_INIT(p_this);

    /* make a local copy of this interrupt controller */
    __gp_intc_dev = p_this;

    p_serv = &p_this->intctlr_serv;

    /* initialize the intctlr service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->intctlr_servinfo;
    p_serv->p_servfuncs = &__g_intc_servfuncs;
    p_serv->p_cookie    = (void *)p_dev;

    awbl_register_interrupt_controller(p_serv);

}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __intc_inst_init2 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __intc_inst_connect (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief connect isr
 */
aw_err_t __intc_int_connect (void            *p_cookie,
                             int              inum,
                             aw_pfuncvoid_t   pfunc_isr,
                             void            *p_arg)
{
    awbl_imx6ul_intc_dev_t     *p_this    = __INTC_DEV_TO_THIS(p_cookie);

    __INTC_DEV_LOCK(p_this);

    /* can't connect new isr while an isr is already connected */
    if (p_this->isrinfo[inum].pfunc_isr != NULL) {

        __INTC_DEV_UNLOCK(p_this);
        return -AW_EPERM;
    }

    p_this->isrinfo[inum].pfunc_isr = pfunc_isr;
    p_this->isrinfo[inum].p_arg     = p_arg;

    __INTC_DEV_UNLOCK(p_this);

    return AW_OK;
}

/**
 * \brief disconnect isr
 */
aw_err_t __intc_int_disconnect(void            *p_cookie,
                               int              inum,
                               aw_pfuncvoid_t   pfunc_isr,
                               void            *p_arg)
{
    awbl_imx6ul_intc_dev_t  *p_this = __INTC_DEV_TO_THIS(p_cookie);


    __INTC_DEV_LOCK(p_this);

    if ((p_this->isrinfo[inum].pfunc_isr != pfunc_isr) &&
        (p_this->isrinfo[inum].p_arg     != p_arg)) {

        __INTC_DEV_UNLOCK(p_this);
        return -AW_ENOENT;
    }

    p_this->isrinfo[inum].pfunc_isr = NULL;
    gic_enable_irq(inum,AW_FALSE);
    __INTC_DEV_UNLOCK(p_this);

    return AW_OK;
}

/**
 * \brief enable interrupt
 */
aw_err_t __intc_int_enable(void *p_cookie, int inum)
{
    /* 我们始终使用cpu 0处理外设中断 */
    gic_set_cpu_target(inum,0,AW_TRUE);
    gic_enable_irq(inum,AW_TRUE);
    return AW_OK;
}

/**
 * \brief disable interrupt
 */
aw_err_t __intc_int_disable(void *p_cookie, int inum)
{
    gic_enable_irq(inum,AW_FALSE);
    return AW_OK;
}

/******************************************************************************/
void awbl_imx6ul_intc_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_intc_drv_registration);
}

/* end of file */



