#include "imx6ul_iomuxc_regs.h"
#include "imx6ul_regs_base.h"

static inline uint8_t *hw_get_iomuxc_gpr_base()
{
    return (uint8_t *)IMX6UL_IOMUXC_GPR_REGS_BASE_ADDR;
}

aw_spinlock_isr_t __spinlock_isr_iomuxc_gpr = AW_SPINLOCK_ISR_UNLOCK;

uint32_t iomuxc_reg_bits_get(uint32_t offset,uint32_t start,uint32_t len)
{
    uint32_t        val;
    aw_spinlock_isr_take(&__spinlock_isr_iomuxc_gpr);
    val = AW_REG_BITS_GET32(hw_get_iomuxc_gpr_base() + offset,start,len );
    aw_spinlock_isr_give(&__spinlock_isr_iomuxc_gpr);
    
    return val;
}

void iomuxc_reg_bits_set(uint32_t offset,uint32_t start,uint32_t len,uint32_t _val)
{
    aw_spinlock_isr_take(&__spinlock_isr_iomuxc_gpr);
    AW_REG_BITS_SET32(hw_get_iomuxc_gpr_base() + offset,start,len,_val);
    aw_spinlock_isr_give(&__spinlock_isr_iomuxc_gpr);
}
