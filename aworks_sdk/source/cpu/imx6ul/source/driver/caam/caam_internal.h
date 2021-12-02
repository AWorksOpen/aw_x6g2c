#ifndef     __CAAM_INTERNAL_H__
#define     __CAAM_INTERNAL_H__

#include "caam_sm.h"
#include "aw_sem.h"

#define CONFIG_CAAM_JOBR_DEPTH          1
#define CONFIG_CAAM_DMA_ALIGN           128

// 检查配置项的一些错误
#if CONFIG_CAAM_DMA_ALIGN%16 >0
#error "CONFIG_CAAM_DMA_ALIGN must be the number of 16"
#endif

/* Private sub-storage for a single JobR */
struct caam_drv_jr_info {
    struct caam_job_ring           *rregs;          /* JobR's register space */
    int                             irq;

    dma_addr_t                     *inpring;        /* Base of input ring, alloc DMA-safe */
    dma_addr_t                     *outring;

    AW_SEMB_DECL(sem_job_finished);
};

/*
 * Driver-private storage for a single CAAM block instance
 */
struct caam_drv_private {

    //硬件相关映射
    volatile struct caam_ctrl      *ctrl;
    volatile struct caam_deco     **deco;           /* DECO/CCB views */
    volatile struct caam_assurance *ac;
    volatile struct caam_queue_if  *qi;             /* QI control region */

    uint32_t                       *sm_base;        /* Secure memory storage base */

    struct caam_drv_jr_info         jr_info[8];
    int                             jr_count;       /* job ring count */
    /*
     * Detected geometry block. Filled in from device tree if powerpc,
     * or from register-based version detection code
     */
    u8                              total_jobrs;    /* Total Job Rings in device */
    u8                              qi_present;     /* Nonzero if QI present in device */

#define RNG4_MAX_HANDLES                2
    /* RNG4 block */
    /*
     * This bitmap shows which of the State Handles of the RNG4 block are initialized
     *  by this driver.
     */
    u32                             rng4_sh_init;
    struct caam_drv_private_sm      sec_mem;
};

int caam_init_obj(struct caam_drv_private *ctrlpriv);
void caam_deinit_obj(struct caam_drv_private *ctrlpriv);

//ctrl
int caam_probe(struct caam_drv_private *ctrlpriv);
int caam_remove(struct caam_drv_private *ctrlpriv);
//jr
int caam_jr_init(struct caam_drv_jr_info *jrpriv);
int caam_jr_deinit(struct caam_drv_jr_info *jrpriv);
//sm
int caam_sm_init(struct caam_drv_private *ctrlpriv);
int caam_sm_deinit(struct caam_drv_private *ctrlpriv);

#endif


