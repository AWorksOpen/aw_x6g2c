    .section  .text     /* section name */
    .code       32      /* ARM mode */
    .align      2       /* aligned at 2^n */

    .set  REG_CCM_CLPCR_ADDR, 0x20C4054

#define __FUNC_DECL(name)   .global name; .type name, %function; .func name
#define __FUNC_END          .endfunc

    .global __code_imx6ul_cpu_power_end
    .global __code_imx6ul_cpu_power_start
/*******************************************************************************
   global functions
*******************************************************************************/
    __FUNC_DECL(imx6ul_cpu_idle_asm)
__code_imx6ul_cpu_power_start:
imx6ul_cpu_idle_asm:
    ldr r1,.LREG_CCM_CLPCR_ADDR
    ldr r0,[r1]

    and r0,r0,#0xFFFFFFFC
    orr r0,r0,#1
    str r0,[r1]

    dsb
    wfi
    dsb
    bx      lr
    __FUNC_END
.LREG_CCM_CLPCR_ADDR:
    .word REG_CCM_CLPCR_ADDR

__code_imx6ul_cpu_power_end:
    .word 0
