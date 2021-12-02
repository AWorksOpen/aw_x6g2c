#include <apollo.h>
#include <string.h>
#include "imx6ul_caam_regs.h"
#include "caam_internal.h"
#include "imx6ul_regs_base.h"
#include "aw_bitops.h"
#include "aw_delay.h"
#include "desc_constr.h"
#include "imx6ul_inum.h"
#include "caam_func.h"


#undef __iomem
#define __iomem             volatile

/*
 * Descriptor to instantiate RNG State Handle 0 in normal mode and
 * load the JDKEK, TDKEK and TDSK registers
 */
static void build_instantiation_desc(u32 *desc, int handle, int do_sk)
{
    u32 *jump_cmd, op_flags;

    init_job_desc(desc, 0);

    op_flags = OP_TYPE_CLASS1_ALG | OP_ALG_ALGSEL_RNG |
            (handle << OP_ALG_AAI_SHIFT) | OP_ALG_AS_INIT;

    /* INIT RNG in non-test mode */
    append_operation(desc, op_flags);

    if (!handle && do_sk) {
        /*
         * For SH0, Secure Keys must be generated as well
         */

        /* wait for done */
        jump_cmd = append_jump(desc, JUMP_CLASS_CLASS1);
        set_jump_tgt_here(desc, jump_cmd);

        /*
         * load 1 to clear written reg:
         * resets the done interrrupt and returns the RNG to idle.
         */
        append_load_imm_u32(desc, 1, LDST_SRCDST_WORD_CLRW);

        /* Initialize State Handle  */
        append_operation(desc, OP_TYPE_CLASS1_ALG | OP_ALG_ALGSEL_RNG |
                 OP_ALG_AAI_RNG4_SK);
    }

    append_jump(desc, JUMP_CLASS_CLASS1 | JUMP_TYPE_HALT);

}

/*
 * kick_trng - sets the various parameters for enabling the initialization
 *         of the RNG4 block in CAAM
 * @pdev - pointer to the platform device
 * @ent_delay - Defines the length (in system clocks) of each entropy sample.
 */
static void kick_trng(struct caam_drv_private *ctrlpriv, int ent_delay)
{
    volatile struct caam_full   *topregs;
    volatile struct rng4tst     *r4tst;
    u32                         val;

    topregs = (volatile struct caam_full *)ctrlpriv->ctrl;
    r4tst = &topregs->ctrl.r4tst[0];

    /* put RNG4 into program mode */
    AW_REG_BIT_SET_MASK32(&r4tst->rtmctl, RTMCTL_PRGM);

    /*
     * Performance-wise, it does not make sense to
     * set the delay to a value that is lower
     * than the last one that worked (i.e. the state handles
     * were instantiated properly. Thus, instead of wasting
     * time trying to set the values controlling the sample
     * frequency, the function simply returns.
     */
    val = (rd_reg32(&r4tst->rtsdctl) & RTSDCTL_ENT_DLY_MASK)
          >> RTSDCTL_ENT_DLY_SHIFT;
    if (ent_delay <= val) {
        /* put RNG4 into run mode */
        clrbits32(&r4tst->rtmctl, RTMCTL_PRGM);
        return;
    }

    val = rd_reg32(&r4tst->rtsdctl);
    val = (val & ~RTSDCTL_ENT_DLY_MASK) |
          (ent_delay << RTSDCTL_ENT_DLY_SHIFT);
    wr_reg32(&r4tst->rtsdctl, val);
    /* min. freq. count, equal to 1/4 of the entropy sample length */
    wr_reg32(&r4tst->rtfrqmin, ent_delay >> 2);
    /* max. freq. count, equal to 16 times the entropy sample length */
    wr_reg32(&r4tst->rtfrqmax, ent_delay << 4);
    /* put RNG4 into run mode */
    clrbits32(&r4tst->rtmctl, RTMCTL_PRGM);

}

/*
 * run_descriptor_deco0 - runs a descriptor on DECO0, under direct control of
 *            the software (no JR/QI used).
 * @ctrldev - pointer to device
 * @status - descriptor status, after being run
 *
 * Return: - 0 if no error occurred
 *     - -AW_ENODEV if the DECO couldn't be acquired
 *     - -AW_EAGAIN if an error occurred while executing the descriptor
 */
static inline int run_descriptor_deco0( struct caam_drv_private *ctrlpriv,
                                        u32 *desc,
                                        u32 *status)
{
    struct caam_full __iomem *topregs;
    unsigned int timeout = 100000;
    u32 deco_dbg_reg, flags;
    int i;

    /* Set the bit to request direct access to DECO0 */
    topregs = (struct caam_full __iomem *)ctrlpriv->ctrl;
    setbits32(&topregs->ctrl.deco_rq, DECORR_RQD0ENABLE);

    while (!(rd_reg32(&topregs->ctrl.deco_rq) & DECORR_DEN0) &&
                                 --timeout)
        asm volatile("" : : : "memory");

    if (!timeout) {
        clrbits32(&topregs->ctrl.deco_rq, DECORR_RQD0ENABLE);
        return -AW_ENODEV;
    }

    for (i = 0; i < desc_len(desc); i++)
        wr_reg32(&topregs->deco.descbuf[i], *(desc + i));

    flags = DECO_JQCR_WHL;
    /*
     * If the descriptor length is longer than 4 words, then the
     * FOUR bit in JRCTRL register must be set.
     */
    if (desc_len(desc) >= 4)
        flags |= DECO_JQCR_FOUR;

    /* Instruct the DECO to execute it */
    wr_reg32(&topregs->deco.jr_ctl_hi, flags);

    timeout = 10000000;
    do {
        deco_dbg_reg = rd_reg32(&topregs->deco.desc_dbg);
        /*
         * If an error occured in the descriptor, then
         * the DECO status field will be set to 0x0D
         */
        if ((deco_dbg_reg & DESC_DBG_DECO_STAT_MASK) ==
            DESC_DBG_DECO_STAT_HOST_ERR)
            break;
        asm volatile("" : : : "memory");
    } while ((deco_dbg_reg & DESC_DBG_DECO_STAT_VALID) && --timeout);

    *status = rd_reg32(&topregs->deco.op_status_hi) &
          DECO_OP_STATUS_HI_ERR_MASK;

    /* Mark the DECO as free */
    clrbits32(&topregs->ctrl.deco_rq, DECORR_RQD0ENABLE);

    if (!timeout)
        return -AW_EAGAIN;

    return 0;
}

/*
 * instantiate_rng - builds and executes a descriptor on DECO0,
 *           which initializes the RNG block.
 * @ctrldev - pointer to device
 * @state_handle_mask - bitmask containing the instantiation status
 *          for the RNG4 state handles which exist in
 *          the RNG4 block: 1 if it's been instantiated
 *          by an external entry, 0 otherwise.
 * @gen_sk  - generate data to be loaded into the JDKEK, TDKEK and TDSK;
 *        Caution: this can be done only once; if the keys need to be
 *        regenerated, a POR is required
 *
 * Return: - 0 if no error occurred
 *     - -ENOMEM if there isn't enough memory to allocate the descriptor
 *     - -AW_ENODEV if DECO0 couldn't be acquired
 *     - -AW_EAGAIN if an error occurred when executing the descriptor
 *        f.i. there was a RNG hardware error due to not "good enough"
 *        entropy being aquired.
 */
static int instantiate_rng( struct caam_drv_private *ctrlpriv,
                            int state_handle_mask,
                            int gen_sk)
{
    struct caam_full __iomem *topregs;
    u32     desc[32], status, rdsta_val;
    int ret = 0, sh_idx;

    topregs = (struct caam_full __iomem *)ctrlpriv->ctrl;


    for (sh_idx = 0; sh_idx < RNG4_MAX_HANDLES; sh_idx++) {
        /*
         * If the corresponding bit is set, this state handle
         * was initialized by somebody else, so it's left alone.
         */
        if ((1 << sh_idx) & state_handle_mask)
            continue;

        /* Create the descriptor for instantiating RNG State Handle */
        build_instantiation_desc(desc, sh_idx, gen_sk);

        /* Try to run it through DECO0 */
        ret = run_descriptor_deco0(ctrlpriv, desc, &status);

        /*
         * If ret is not 0, or descriptor status is not 0, then
         * something went wrong. No need to try the next state
         * handle (if available), bail out here.
         * Also, if for some reason, the State Handle didn't get
         * instantiated although the descriptor has finished
         * without any error (HW optimizations for later
         * CAAM eras), then try again.
         */
        rdsta_val =
            rd_reg32(&topregs->ctrl.r4tst[0].rdsta) & RDSTA_IFMASK;
        if (status || !(rdsta_val & (1 << sh_idx)))
            ret = -AW_EAGAIN;
        if (ret)
            break;


        /* Clear the contents before recreating the descriptor */
        memset(desc, 0x00, CAAM_CMD_SZ * 7);
    }

    return ret;
}


/* Probe routine for CAAM top (controller) level */
int caam_probe(struct caam_drv_private *ctrlpriv)
{
    int                         gen_sk, ent_delay = RTSDCTL_ENT_DLY_MIN;
    volatile u32                caam_id;
    volatile struct caam_ctrl   *ctrl;
    volatile struct caam_full   *topregs;
    int                         ret = AW_ERROR;
    int                         rng_vid;

    /* Get configuration properties from device tree */
    /* First, get register page */
    ctrl = (volatile struct caam_ctrl *)IMX6UL_CAAM_BASE_ADDR;

    ctrlpriv->ctrl = (struct caam_ctrl __force *)ctrl;

    /* topregs used to derive pointers to CAAM sub-blocks only */
    topregs = (volatile struct caam_full *)ctrl;

    /* Get CAAM-SM node and of_iomap() and save */
    ctrlpriv->sm_base = (uint32_t *)IMX6UL_CAAM_SECURE_MEM_BASE_ADDR;

    /*
     * Enable DECO watchdogs and, if this is a PHYS_ADDR_T_64BIT kernel,
     * long pointers in master configuration register
     */
    AW_REG_BIT_SET_MASK32(&topregs->ctrl.mcr, MCFGR_WDENABLE |
          (sizeof(dma_addr_t) == sizeof(u64) ? MCFGR_LONG_PTR : 0));


    /*
     * ERRATA:  mx6 devices have an issue wherein AXI bus transactions
     * may not occur in the correct order. This isn't a problem running
     * single descriptors, but can be if running multiple concurrent
     * descriptors. Reworking the driver to throttle to single requests
     * is impractical, thus the workaround is to limit the AXI pipeline
     * to a depth of 1 (from it's default of 4) to preclude this situation
     * from occurring.
     */
    OUTREG32(&topregs->ctrl.mcr,
         (INREG32(&topregs->ctrl.mcr) & ~(MCFGR_AXIPIPE_MASK)) |
         ((1 << MCFGR_AXIPIPE_SHIFT) & MCFGR_AXIPIPE_MASK));


    /* 总共有3个job ring */
    ctrlpriv->jr_count = 3;
    memset(&ctrlpriv->jr_info[0],0,sizeof(ctrlpriv->jr_info) );
    ctrlpriv->jr_info[0].rregs = (struct caam_job_ring *)IMX6UL_CAAM_JR0_ADDR;
    ctrlpriv->jr_info[1].rregs = (struct caam_job_ring *)IMX6UL_CAAM_JR1_ADDR;
    ctrlpriv->jr_info[2].rregs = (struct caam_job_ring *)IMX6UL_CAAM_JR2_ADDR;
    ctrlpriv->jr_info[0].irq = INUM_CAAM_JR0_IRQ;
    ctrlpriv->jr_info[1].irq = INUM_CAAM_JR1_IRQ;
    ctrlpriv->jr_info[2].irq = INUM_CAAM_JR2_IRQ;


    /* Check to see if QI present. If so, enable */
    ctrlpriv->qi_present = !!(INREG32(&topregs->ctrl.perfmon.ctpr_ms) &
                  CTPR_MS_QI_MASK );
    if (ctrlpriv->qi_present) {
        ctrlpriv->qi = (struct caam_queue_if __force *)&topregs->qi;
        /* This is all that's required to physically enable QI */
        OUTREG32(&topregs->qi.qi_control_lo, QICTL_DQEN);
    }

    rng_vid = AW_REG_BITS_GET32(&topregs->ctrl.perfmon.cha_id_ls,
                                CHA_ID_RNG_SHIFT,4);

    /*
     * If SEC has RNG version >= 4 and RNG state handle has not been
     * already instantiated, do RNG instantiation
     */
    if (rng_vid >= 4) {
        ctrlpriv->rng4_sh_init =
            INREG32(&topregs->ctrl.r4tst[0].rdsta);
        /*
         * If the secure keys (TDKEK, JDKEK, TDSK), were already
         * generated, signal this to the function that is instantiating
         * the state handles. An error would occur if RNG4 attempts
         * to regenerate these keys before the next POR.
         */
        gen_sk = ctrlpriv->rng4_sh_init & RDSTA_SKVN ? 0 : 1;
        ctrlpriv->rng4_sh_init &= RDSTA_IFMASK;
        do {
            int inst_handles =
                INREG32(&topregs->ctrl.r4tst[0].rdsta) &
                                RDSTA_IFMASK;
            /*
             * If either SH were instantiated by somebody else
             * (e.g. u-boot) then it is assumed that the entropy
             * parameters are properly set and thus the function
             * setting these (kick_trng(...)) is skipped.
             * Also, if a handle was instantiated, do not change
             * the TRNG parameters.
             */
            if (!(ctrlpriv->rng4_sh_init || inst_handles)) {
                kick_trng(ctrlpriv, ent_delay);
                ent_delay += 400;
            }
            /*
             * if instantiate_rng(...) fails, the loop will rerun
             * and the kick_trng(...) function will modfiy the
             * upper and lower limits of the entropy sampling
             * interval, leading to a sucessful initialization of
             * the RNG.
             */
            ret = instantiate_rng(ctrlpriv, inst_handles,
                          gen_sk);
        } while ((ret == -AW_EAGAIN) && (ent_delay < RTSDCTL_ENT_DLY_MAX));
        if (ret) {

            return ret;
        }
        /*
         * Set handles init'ed by this module as the complement of the
         * already initialized ones
         */
        ctrlpriv->rng4_sh_init = ~ctrlpriv->rng4_sh_init & RDSTA_IFMASK;

        /* Enable RDB bit so that RNG works faster */
        setbits32(&topregs->ctrl.scfgr, SCFGR_RDBENABLE);
    }

    /* NOTE: RTIC detection ought to go here, around Si time */
    caam_id = INREG32(&topregs->ctrl.perfmon.caam_id);
    aw_kprintf("caam_id = 0x%x\n",caam_id);
    return 0;
}

int caam_remove(struct caam_drv_private *ctrlpriv)
{
    uint32_t        t;
    int             count = 1000;
    AW_REG_BIT_SET_MASK32(&ctrlpriv->ctrl->mcr, MCFGR_SWRESET |MCFGR_DMA_RESET);
    do {
        aw_udelay(100);
        t = rd_reg32(&ctrlpriv->ctrl->mcr);
        if ( !( (MCFGR_SWRESET |MCFGR_DMA_RESET) & t )  ) {
            break;
        }
        count --;

    }while(count);

    return 0;
}


