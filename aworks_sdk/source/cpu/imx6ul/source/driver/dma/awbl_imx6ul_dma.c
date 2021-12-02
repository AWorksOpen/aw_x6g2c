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
 * \brief The IMX6UL AHB-to-APBH Bridge DMA driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-07, tee, first implementation.
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

#include "driver/dma/awbl_imx6ul_dma.h"

/*******************************************************************************
  Global variable
*******************************************************************************/
aw_local struct awbl_imx6ul_dma_dev  *__gp_imx6ul_dma_dev = NULL;

/*******************************************************************************
  register define
*******************************************************************************/
struct __imx6ul_dma_reg1 {
    uint32_t value;
    uint32_t value_set;
    uint32_t value_clr;
    uint32_t value_tog;
};

struct __imx6ul_dma_reg2 {
    uint32_t value;
    uint32_t reserved[3];
};

struct __imx6ul_dma_regs {

    /* GPMI Control Register 0 */
    struct __imx6ul_dma_reg1 ctrl0;
#define __DMA_CTRL0_SFTRST                    (1u << 31)
#define __DMA_CTRL0_CLKGATE                   (1u << 30)
#define __DMA_CTRL0_AHB_BURST8_EN             (1u << 29)
#define __DMA_CTRL0_APB_BURST_EN              (1u << 28)
#define __DMA_CTRL0_APB_CLKGATE_CHANNEL(chan) (1u << chan)

    struct __imx6ul_dma_reg1 ctrl1;
    struct __imx6ul_dma_reg1 ctrl2;
    struct __imx6ul_dma_reg1 channel_ctrl;
    struct __imx6ul_dma_reg2 devsel;
    struct __imx6ul_dma_reg2 burst_size;
    struct __imx6ul_dma_reg2 apbh_debug;

    /** \brief reserved : 0x70 ~ 0xF0 */
    struct __imx6ul_dma_reg2 reserved0[9];

    struct {
        struct __imx6ul_dma_reg2 cur_cmd_addr;
        struct __imx6ul_dma_reg2 next_cmd_addr;
        struct __imx6ul_dma_reg2 cmd;
        struct __imx6ul_dma_reg2 buffer_addr;
        struct __imx6ul_dma_reg2 semaphore;
        struct __imx6ul_dma_reg2 debug1;
        struct __imx6ul_dma_reg2 debug2;
    } chan[16];

    /** \brief The APBH Bridge Version Register */
    struct __imx6ul_dma_reg2 version;
};

/*******************************************************************************
  Local defines
*******************************************************************************/

/* declare imx6ul dma infomation */
#define __DMA_DEV_DECL(p_this, p_dev) \
    struct awbl_imx6ul_dma_dev *p_this = \
        (struct awbl_imx6ul_dma_dev *)(p_dev)

#define __DMA_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx6ul_dma_devinfo *p_devinfo = \
        (struct awbl_imx6ul_dma_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __DMA_REGS_DECL(p_regs, p_devinfo) \
    struct __imx6ul_dma_regs *p_regs = \
        (struct __imx6ul_dma_regs *)(p_devinfo->regbase)

#define __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan) \
        struct __imx6ul_dma_regs *p_regs = \
            (struct __imx6ul_dma_regs *)(p_dma_chan->regbase)

/*******************************************************************************
  locals functions
*******************************************************************************/

aw_local aw_err_t __imx6ul_dma_init (struct __imx6ul_dma_regs *p_regs)
{
    writel(__DMA_CTRL0_SFTRST, &p_regs->ctrl0.value_clr);

    while (readl(&p_regs->ctrl0.value) & __DMA_CTRL0_SFTRST) {
        ;
    }

    writel(__DMA_CTRL0_CLKGATE, &p_regs->ctrl0.value_clr);

    while (readl(&p_regs->ctrl0.value) & __DMA_CTRL0_CLKGATE) {
        ;
    }

    writel(__DMA_CTRL0_SFTRST, &p_regs->ctrl0.value_set);

    while (!((readl(&p_regs->ctrl0.value)) & __DMA_CTRL0_CLKGATE)) {
        ;
    }

    writel(__DMA_CTRL0_SFTRST, &p_regs->ctrl0.value_clr);

    while (readl(&p_regs->ctrl0.value) & __DMA_CTRL0_SFTRST) {
        ;
    }

    writel(__DMA_CTRL0_CLKGATE, &p_regs->ctrl0.value_clr);

    while (readl(&p_regs->ctrl0.value) & __DMA_CTRL0_CLKGATE) {
        ;
    }

    return AW_OK;
}

/******************************************************************************/

/** \brief Set the channel's semaphore value to be zero */
aw_local aw_err_t __imx6ul_dma_chan_zero_semaphore (awbl_imx6ul_dma_chan_t *p_dma_chan)
{
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    uint32_t phore;

    phore = (readl(&p_regs->chan[p_dma_chan->channel].semaphore.value) >> 16) & 0xFF;

    phore = ~phore + 1;

    /* Increment sema, to implement Zero Semaphore */
    writel(phore, &p_regs->chan[p_dma_chan->channel].semaphore.value);

    return AW_OK;
}

/******************************************************************************/

/* Freeze or unfreeze the dma channel */
aw_local aw_err_t __imx6ul_dma_chan_freeze (awbl_imx6ul_dma_chan_t *p_dma_chan,
                                            aw_bool_t                  freeze)
{
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    if (freeze) {               /* Freeze the channel   */
        writel((1 << (p_dma_chan->channel)), &p_regs->channel_ctrl.value_set);
    } else {                    /* unFreeze the channel */
        writel((1 << (p_dma_chan->channel)), &p_regs->channel_ctrl.value_clr);
    }

    return AW_OK;
}

/******************************************************************************/

/* Take the channel into or out of reset state */
aw_local aw_err_t __imx6ul_dma_chan_reset (awbl_imx6ul_dma_chan_t *p_dma_chan,
                                           uint8_t                 reset)
{
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    uint32_t count;

    if (reset) {
        writel((1 << (16 + p_dma_chan->channel)), &p_regs->channel_ctrl.value_set);

    } else {

        writel((1 << (16 + p_dma_chan->channel)), &p_regs->channel_ctrl.value_clr);
    }

    count = 1000000;
    while ((readl(&p_regs->channel_ctrl.value) & (1 << (16 + p_dma_chan->channel))) &&
           (count--));


    __imx6ul_dma_chan_zero_semaphore(p_dma_chan);

    if (count > 0) {
        return AW_OK;
    }

    return AW_ERROR;
}

/******************************************************************************/

/* Enable or disable the dma channel's command complete irq */
aw_local aw_err_t __imx6ul_dma_chan_enable_cmplt_irq (
    awbl_imx6ul_dma_chan_t  *p_dma_chan,
    aw_bool_t                   enable_irq)
{
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    if (enable_irq) {     /* Enable interrupt  */

        writel(1 << (16 + p_dma_chan->channel), &p_regs->ctrl1.value_set);

    } else {              /* Disable interrupt */

        writel(1 << (16 + p_dma_chan->channel), &p_regs->ctrl1.value_clr);
    }

    return AW_OK;
}

/******************************************************************************/

/* Initialize the dma channel */
aw_local aw_err_t __imx6ul_dma_chan_init (awbl_imx6ul_dma_chan_t *p_dma_chan,
                                          aw_bool_t                  enable_irq)
{
    /* unFreeze channel */
    if (__imx6ul_dma_chan_freeze(p_dma_chan, AW_FALSE) != AW_OK) {
        AW_DBGF("failed to UnFreeze channel!\r\n");
        return AW_ERROR;
    }

    /* come out from reset state */
    if (__imx6ul_dma_chan_reset(p_dma_chan, AW_FALSE) != AW_OK) {
        AW_DBGF("Failed to reset channel!\r\n");
        return AW_ERROR;
    }

    /* Enable/Disable Irq based on the request */
    if (__imx6ul_dma_chan_enable_cmplt_irq(p_dma_chan, enable_irq) != AW_OK) {
        AW_DBGF("Failed to Enable/Disable IRQ!\r\n");
        return AW_ERROR;
    }

    return AW_OK;
}

/******************************************************************************/

/* Start the dma channel */
aw_local aw_err_t __imx6ul_dma_chan_start (awbl_imx6ul_dma_chan_t *p_dma_chan,
                                           uint8_t                 dma_semaphore)
{
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    awbl_imx6ul_dma_desc_t *p_desc = p_dma_chan->p_dma_desc;

    int i;

    /* validate the memAddrPA */
    if ((p_dma_chan == NULL) || (p_desc == NULL)) {
        return -AW_EINVAL;
    }

    if (dma_semaphore > 1) {
        for (i = 1; i < dma_semaphore; i++) {

            p_desc->bits &= ~IMX6UL_DMA_CMD_IRQONCMPLT(1);

            p_desc = (awbl_imx6ul_dma_desc_t *)(p_desc->next_cmd_desc);
            p_desc->bits |= IMX6UL_DMA_CMD_IRQONCMPLT(1);
        }
    }

    /* Load command chain pointer */
    writel((uint32_t)p_dma_chan->p_dma_desc,  \
                       &p_regs->chan[p_dma_chan->channel].next_cmd_addr.value);

    /* Increment dma semaphore    */
    writel(dma_semaphore, &p_regs->chan[p_dma_chan->channel].semaphore.value);

    return AW_OK;
}

/******************************************************************************/

/* DMA interrupt service routine */
aw_local void __imx6ul_dma_isr (void *p_arg)
{
    __DMA_DEV_DECL(p_this, p_arg);
    __DMA_DEVINFO_DECL(p_devinfo, p_arg);
    __DMA_REGS_DECL(p_regs, p_devinfo);

    awbl_imx6ul_dma_chan_t *p_dma_chan  = NULL;
    int                     err_status, com_status;
    int                     i;

    com_status = readl(&p_regs->ctrl1.value);
    err_status = readl(&p_regs->ctrl2.value);

    for (i = 0; i < IMX6UL_DMA_CHANNEL_NUM; i++) {

        p_dma_chan = &p_this->chan[i];

        if (err_status & (1 << i)) {

            p_dma_chan->status = -AW_EIO;
            writel((1 << i), &p_regs->ctrl2.value_clr);

            if (p_dma_chan->pfn_complete_cb) {
                p_dma_chan->pfn_complete_cb(p_dma_chan->p_arg,
                                            p_dma_chan->status);
            }

        } else if (com_status & (1 << i)) {

            p_dma_chan->status = AW_OK;
            writel((1 << i), &p_regs->ctrl1.value_clr);

            if (p_dma_chan->pfn_complete_cb) {
                p_dma_chan->pfn_complete_cb(p_dma_chan->p_arg,
                                            p_dma_chan->status);
            }
        }
    }
}

/******************************************************************************/

/* synchronous dma transfer */
aw_local void __imx6ul_dma_trans_sync_complete (void *p_arg, int status)
{
    if (p_arg) {
        aw_semb_give(p_arg);
    }
}

/******************************************************************************/

/* start a dma transmission */
aw_err_t aw_imx6ul_dma_chan_start_sync (int                      chan,
                                        awbl_imx6ul_dma_desc_t  *p_desc,
                                        uint8_t                  dma_semaphore,
                                        uint32_t                 timeout)
{
    struct awbl_imx6ul_dma_dev *p_this     = __gp_imx6ul_dma_dev;
    awbl_imx6ul_dma_chan_t     *p_dma_chan = &(p_this->chan[chan]);
    __DMA_CHAN_REGS_DECL(p_regs, p_dma_chan);

    AW_SEMB_DECL(sem_sync);

    aw_err_t       err;
    aw_semb_id_t   sem_id;

    if ((p_this == NULL) || (p_desc == NULL)) {
        return -AW_EINVAL;
    }

    p_dma_chan = &(p_this->chan[chan]);

    sem_id = AW_SEMB_INIT(sem_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    aw_imx6ul_dma_chan_start_async(chan,
                                   p_desc,
                                   dma_semaphore,
                                   __imx6ul_dma_trans_sync_complete,
                                   (void *)sem_id);

    err = AW_SEMB_TAKE(sem_sync, timeout);

    if (err != AW_OK) {

        writel((1 << (16 + p_dma_chan->channel)), &p_regs->channel_ctrl.value_clr);

        __imx6ul_dma_chan_reset(p_dma_chan, AW_TRUE);
    }

    /* Just in case that give the signal in reset procedure */
    AW_SEMB_TERMINATE(sem_sync);

    return (err != AW_OK) ? err : (p_dma_chan->status);
}

/******************************************************************************/

/* start a dma transmission */
aw_err_t  aw_imx6ul_dma_chan_start_async (int                           chan,
                                          awbl_imx6ul_dma_desc_t       *p_desc,
                                          uint8_t                       dma_semaphore,
                                          awbl_imx6ul_dma_complete_cb_t pfn_complete,
                                          void                         *p_arg)
{
    struct awbl_imx6ul_dma_dev *p_this     = __gp_imx6ul_dma_dev;
    awbl_imx6ul_dma_chan_t     *p_dma_chan = NULL;

    if ((p_this == NULL) || (p_desc == NULL)) {
        return -AW_EINVAL;
    }

    p_dma_chan = &(p_this->chan[chan]);

    p_dma_chan->pfn_complete_cb = pfn_complete;
    p_dma_chan->p_arg           = p_arg;
    p_dma_chan->trans_counter   = dma_semaphore;
    p_dma_chan->p_dma_desc      = p_desc;

    __imx6ul_dma_chan_reset(p_dma_chan, AW_TRUE);

    p_dma_chan->status = -AW_EINPROGRESS;

    __imx6ul_dma_chan_start(p_dma_chan, dma_semaphore);

    return AW_OK;
}

/******************************************************************************/

/* start a dma stop */
aw_err_t aw_imx6ul_dma_chan_stop (int chan)
{
    struct awbl_imx6ul_dma_dev *p_this     = __gp_imx6ul_dma_dev;
    awbl_imx6ul_dma_chan_t     *p_dma_chan = NULL;

    int status;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    p_dma_chan = &(p_this->chan[chan]);

    status = p_dma_chan->status;

    p_dma_chan->status = -AW_ECANCELED;

    __imx6ul_dma_chan_reset(p_dma_chan, AW_TRUE);

    if ((p_dma_chan->pfn_complete_cb) && (status == -AW_EINPROGRESS)) {
        p_dma_chan->pfn_complete_cb(p_dma_chan->p_arg, p_dma_chan->status);
    }

    return AW_OK;
}

/******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __dma_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __dma_inst_init2 (awbl_dev_t *p_dev)
{
    __DMA_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_imx6ul_dma_dev *p_this     = (struct awbl_imx6ul_dma_dev *)p_dev;
    awbl_imx6ul_dma_chan_t     *p_dma_chan = NULL;
    int i;

    /* platform initialization */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    __imx6ul_dma_init((struct __imx6ul_dma_regs *)(p_devinfo->regbase));

    for (i = 0; i < IMX6UL_DMA_CHANNEL_NUM; i++) {

        p_dma_chan = &p_this->chan[i];

        p_dma_chan->pfn_complete_cb = NULL;
        p_dma_chan->p_arg           = NULL;
        p_dma_chan->channel         = i;
        p_dma_chan->regbase         = p_devinfo->regbase;
        p_dma_chan->trans_counter   = 0;
        p_dma_chan->p_dma_desc      = NULL;
        p_dma_chan->status          = AW_OK;

        __imx6ul_dma_chan_zero_semaphore(p_dma_chan);
        __imx6ul_dma_chan_init(p_dma_chan, AW_TRUE);

    }

    /* connect and enable isr */
    aw_int_connect(p_devinfo->dma_inum, __imx6ul_dma_isr, (void *)p_this);
    aw_int_enable(p_devinfo->dma_inum);

    __gp_imx6ul_dma_dev = p_this;

    return;
}

/******************************************************************************/

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __dma_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_dma_drvfuncs = {
    __dma_inst_init1,
    __dma_inst_init2,
    __dma_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_dma_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX6UL_DMA_NAME,                   /* p_drvname */
        &__g_dma_drvfuncs,                      /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx6ul_dma_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_dma_drv_registration);
    return;
}

/* end of file */

