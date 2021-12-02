/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file startup.s
 * @brief this file contains startup code with support for multi-core startup
 */

#include "asm_defines.h"

    // External symbol references
    .global top_of_stacks
    .global __bss_start__
    .global __bss_end__
    .global main
    .global data_abort_handler
    .global prefetch_abort_handler
    .global undef_instruction_handler
    .global swi_handler
    .global irq_handle
    .global fiq_handler
    .global imx6ul_cpu_power_init
    .global _imx6ul_vectors

    //数据段加载地址
    .global __rom_data_start
    //数据段实际地址开始位置
    .global __data_section_start
    //数据段实际地址结束位置
    .global __data_section_end

    .code 32
    .section .vectors,"ax"
    .global _imx6ul_vectors
_imx6ul_vectors:
    ldr     pc, .Lreset_addr
    ldr     pc, .Lundefined_addr
    ldr     pc, .Lswi_addr
    ldr     pc, .Lprefetch_abort_addr
    ldr     pc, .Ldata_abort_addr
    ldr     pc, .Lreserved_addr /* reserved vector */
    ldr     pc, .Lirq_addr
    ldr     pc, .Lfiq_addr
    .word 0                     // extra word in the RAM vectors

.Lreset_addr:
    .word     _start
.Lundefined_addr:
    .word    undef_instruction_handler
.Lswi_addr:
    .word     swi_handler
.Lprefetch_abort_addr:
    .word     prefetch_abort_handler
.Ldata_abort_addr:
    .word     data_abort_handler
.Lreserved_addr:
    .word     0x0
.Lirq_addr:
    .word     irq_handle
.Lfiq_addr:
    .word     fiq_handler
.Lvector_end:
    .word 0 // extra word in RAM vectors

    .section ".text"
    .global _start
    .func _start
    .fnstart
_start:
    /* disable all interrupt */
    cpsid   i

    ldr     r0, =top_of_stacks  @ symbol defined in linker file
    mov     r1, #EXCEPTION_STACK_SIZE

    @ set stacks for all other modes
    msr     CPSR_c, #MODE_FIQ | I_BIT | F_BIT
    mov     sp, r0
    sub     r0, r0, r1


    msr     CPSR_c, #MODE_IRQ | I_BIT | F_BIT
    mov     sp, r0
    sub     r0, r0, r1

    msr     CPSR_c, #MODE_ABT | I_BIT | F_BIT
    mov     sp, r0
    sub     r0, r0, r1

    msr     CPSR_c, #MODE_UND | I_BIT | F_BIT
    mov     sp, r0
    sub     r0, r0, r1

    @ Set SVC mode stack with interrupts disabled
    msr     CPSR_c, #MODE_SVC | I_BIT | F_BIT
    mov     sp, r0

    //数据段重定位
    ldr     r0,=__rom_data_start
    ldr     r1,=__data_section_start
    ldr     r2,=__data_section_end
1:
    ldr     r3,[r0]
    stmia   r1!,{r3}
    add     r0,r0,#4
    cmp     r1,r2
    blt     1b
    /*
     * fill the stacks with a pattern
     * the stack must be 8 byte aligned
     */
    ldr     r1,=__stacks_start      @ stack region start address
    ldr     r2,=top_of_stacks       @ stack region end address

    /* 确保栈大小足够，栈的最小需求是EXCEPTION_STACK_SIZE * 4 + SVC_STACK_SIZE*/
    ldr     r5,=(EXCEPTION_STACK_SIZE * 4 + SVC_STACK_SIZE)
    sub     r6,r2,r1
    cmp     r6,r5
    blo     .

    bic     r2,r2,#0x7              @ round length down to nearest 8 byte alignment
    ldr     r3,=.Ldeadfeed          @ get fill pattern address
    ldr     r3,[r3]                 @ read fill pattern into r3
    mov     r4,r3                   @ copy fill pattern to r4
1:  cmp     r1,r2                   @ the fill loop
    stmltia r1!,{r3-r4}
    blt     1b

   	/* init .bss */
    /* clear the .bss section (zero init) */
    ldr     r1,=__bss_start__
    ldr     r2,=__bss_end__
    mov     r3,#0
1:  cmp     r1,r2
    stmltia r1!,{r3}
    blt     1b

    @ 重定位中断向量表
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0,#0x2000
    mcr     P15, 0, r0, c1, c0, 0
    ldr     r0,=_imx6ul_vectors
    mcr     p15, 0, r0, c12,c0, 0
    mrc     p15, 0, r1, c12,c0, 0


    mrc     p15, 0, r0, c1, c0, 2
    orr     r0,r0,#0x00F00000
    mcr     P15, 0, r0, c1, c0, 2
    mrc     p15, 0, r0, c1, c1, 2
    orr     r0,r0,#0x600
    mcr     P15, 0, r0, c1, c1, 2
    bl      imx6ul_cpu_power_init

    @ branch to c library entry point
    mov     r0,#0 @ argc
    mov     r1,#0 @ argv
    mov     r2,#0 @ env

    ldr     r12, =main                  /* save this in register for possible long jump */
    bx      r12                         /* branch to __main */
    b       .

    .cantunwind
    .fnend
    .endfunc    @ _start()

    /* local label for the stack fill pattern */
.Ldeadfeed:
    .word   0xdeadfeed

    .end

