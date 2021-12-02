#ifndef __IMX6UL_BARRIER_H
#define __IMX6UL_BARRIER_H

static inline void barrier(void)
{
    asm volatile("" : : : "memory");
}

static inline void barrier_isb(void)
{
    asm volatile("isb" : : : "memory");
}

static inline void barrier_dsb(void)
{
    asm volatile("dsb" : : : "memory");
}

static inline void barrier_dmb(void)
{
    asm volatile("dmb" : : : "memory");
}


#endif /* __IMX6UL_BARRIER_H */

/* end of file */

