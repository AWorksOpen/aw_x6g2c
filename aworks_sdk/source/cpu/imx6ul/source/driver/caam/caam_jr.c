#include <apollo.h>
#include <string.h>
#include "imx6ul_caam_regs.h"
#include "caam_internal.h"
#include "imx6ul_regs_base.h"
#include "aw_bitops.h"
#include "desc_constr.h"
#include "aw_int.h"
#include "aw_cache.h"


#define JOBR_INTC 0
#define JOBR_INTC_TIME_THLD 0
#define JOBR_INTC_COUNT_THLD 0

static void caam_jr_interrupt( void *st_dev)
{

    struct caam_drv_jr_info *jrp = (struct caam_drv_jr_info *)st_dev;
    u32 irqstate;

    /*
     * Check the output ring for ready responses, kick
     * tasklet if jobs done.
     */
    irqstate = rd_reg32( &jrp->rregs->jrintstatus );
    if (!irqstate)
        return ;

    /* mask valid interrupts */
    setbits32(&jrp->rregs->rconfig_lo, JRCFG_IMSK);
    /* Have valid interrupt at this point, just ACK and trigger */
    wr_reg32(&jrp->rregs->jrintstatus, irqstate);

     while (rd_reg32(&jrp->rregs->outring_used)) {
        /* set done */
        wr_reg32(&jrp->rregs->outring_rmvd, 1);
    }
    /* reenable / unmask IRQs */
    AW_SEMB_GIVE(jrp->sem_job_finished);
    clrbits32(&jrp->rregs->rconfig_lo, JRCFG_IMSK);
    return ;

}


int caam_jr_enqueue(struct caam_drv_jr_info *jrp, u32 *desc)
{
    dma_addr_t              desc_dma;
    int                     ret;


    desc_dma = virt_to_phys(desc);

    if (!rd_reg32(&jrp->rregs->inpring_avail) ) {
        return -AW_EBUSY;
    }

    AW_SEMB_RESET(jrp->sem_job_finished,0);
    jrp->inpring[0] = desc_dma;
    wr_reg32(&jrp->rregs->inpring_jobadd, 1);
    ret = AW_SEMB_TAKE(jrp->sem_job_finished,1000);

    return ret;
}

static int caam_reset_hw_jr(struct caam_drv_jr_info *jrp)
{
    unsigned int    timeout = 100000;
    int             err;

    /*
     * mask interrupts since we are going to poll
     * for reset completion status
     */
    setbits32(&jrp->rregs->rconfig_lo, JRCFG_IMSK);

    /* initiate flush (required prior to reset) */
    wr_reg32(&jrp->rregs->jrcommand, JRCR_RESET);
    while (((rd_reg32(&jrp->rregs->jrintstatus) & JRINT_ERR_HALT_MASK) ==
        JRINT_ERR_HALT_INPROGRESS) && --timeout)
        cpu_relax();

    if ((rd_reg32(&jrp->rregs->jrintstatus) & JRINT_ERR_HALT_MASK) !=
        JRINT_ERR_HALT_COMPLETE || timeout == 0) {
        err = -AW_EIO;
        goto cleanup;
    }

    /* initiate reset */
    timeout = 100000;
    wr_reg32(&jrp->rregs->jrcommand, JRCR_RESET);
    while ((rd_reg32(&jrp->rregs->jrcommand) & JRCR_RESET) && --timeout)
        cpu_relax();

    if (timeout == 0) {
        err = -AW_EIO;
        goto cleanup;
    }

    /* unmask interrupts */
    clrbits32(&jrp->rregs->rconfig_lo, JRCFG_IMSK);
    err = AW_OK;
cleanup:
    return err;
}
/*
 * Init JobR independent of platform property detection
 */
int caam_jr_init(struct caam_drv_jr_info *jrp)
{
    dma_addr_t      inpbusaddr, outbusaddr;
    int             error;

    jrp->inpring = aw_cache_dma_align(
            sizeof(dma_addr_t) * CONFIG_CAAM_JOBR_DEPTH,CONFIG_CAAM_DMA_ALIGN);
    jrp->outring = aw_cache_dma_align(
            sizeof(dma_addr_t) * CONFIG_CAAM_JOBR_DEPTH,CONFIG_CAAM_DMA_ALIGN);

    if ((jrp->inpring == NULL) || (jrp->outring == NULL) ) {
        return -AW_ENOMEM;
    }

    error = caam_reset_hw_jr(jrp);
    if (error)
        return error;


    inpbusaddr = virt_to_phys(jrp->inpring);
    outbusaddr  = virt_to_phys (jrp->outring);
    /* Setup rings */

    wr_reg64(&jrp->rregs->inpring_base, (u64)(u32)inpbusaddr);
    wr_reg64(&jrp->rregs->outring_base, (u64)(u32)outbusaddr);
    wr_reg32(&jrp->rregs->inpring_size, CONFIG_CAAM_JOBR_DEPTH);
    wr_reg32(&jrp->rregs->outring_size, CONFIG_CAAM_JOBR_DEPTH);


    /* Select interrupt coalescing parameters */
    setbits32(&jrp->rregs->rconfig_lo, JOBR_INTC |
          (JOBR_INTC_COUNT_THLD << JRCFG_ICDCT_SHIFT) |
          (JOBR_INTC_TIME_THLD << JRCFG_ICTT_SHIFT));
    AW_SEMB_INIT(jrp->sem_job_finished,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    /* Connect job ring interrupt handler. */
    aw_int_connect(jrp->irq, caam_jr_interrupt,jrp);
    aw_int_enable(jrp->irq);

    return 0;
}

int caam_jr_deinit(struct caam_drv_jr_info *jrp)
{
    aw_int_disable(jrp->irq);
    aw_int_disconnect(jrp->irq, caam_jr_interrupt,jrp);
    caam_reset_hw_jr(jrp);

    if (NULL != jrp->outring) {
        aw_cache_dma_free(jrp->outring);
        jrp->outring  = NULL;
    }

    if (NULL != jrp->inpring) {
        aw_cache_dma_free(jrp->inpring);
        jrp->inpring  = NULL;
    }

    return 0;
}

