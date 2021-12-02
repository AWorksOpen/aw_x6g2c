/**
 * \file    imx283_ram.ld
 * \copy    COPYRIGHT (C) 2014, Liu.Weiyun
 * \brief   IMX283 linker script file
 *
 * \internal
 * \par History
 * - 140504 liuweiyun, first implementation.
 * \endinternal
 */

/*****************************************************************************
                   debug 配置内存分配图

        +---------------------------------------+
        | 0x00000000    | MMP | 0x00000000      |   向量表
        +---------------------------------------+
                       ........
        +---------------------------------------+
        | 0x30000000    |  =  | 0x30000000      |   代码段 + ctor/dtor 表
        +---------------------------------------+
        | 0x30600000    |  =  | ??????????      |   数据段
        +---------------------------------------+
        | ??????????    |  =  | ??????????      |   清零段
        +---------------------------------------+

                       ........
        +---------------------------------------+
        | ??????????    |  =  | ??????????      |   堆栈
        +---------------------------------------+
                       ........
        +---------------------------------------+
        | 0x33ffff00    |  =  | 0x33ffff00      |   ramvector
        +---------------------------------------+
                       ........
        +---------------------------------------+
        | 0x40000000    |  =  | 0x40000000      |   reserve
        +---------------------------------------+
                       ........
*****************************************************************************/

/*****************************************************************************
    链接配置
*****************************************************************************/
STACK_SIZE = 0x10000;            /*  stack size config           */
OUTPUT_FORMAT(elf32-littlearm)   /*  binary file format          */
OUTPUT_ARCH(ARM)                 /*  target select               */
ENTRY(_imx6ul_vectors)

DMA_SAFE_MEM_SIZE = 8M;          /*  dma-safe memory size        */
/*****************************************************************************
    内存布局定义
*****************************************************************************/
MEMORY
{
    DMAHEAP (rw) :  ORIGIN = 0x80000000, LENGTH = 8M        /* DDR2，用于DMA堆*/
    ROM(rx) :       ORIGIN = 0x80800000, LENGTH = 8M        /* DDR2，用于存放代码 */
    RAM(rw) :       ORIGIN = 0x81000000, LENGTH = 16M       /* DDR2，用于堆栈和全局数据 */
    HEAP (rw) :     ORIGIN = 0x82400000, LENGTH = 92M       /* DDR2，用于堆*/
    VRAM (rw) :     ORIGIN = 0x82000000, LENGTH = 4M        /* DDR2，用于显存 */
    OCRAM (rw) :    ORIGIN = 0x00900000, LENGTH = 128K      /* On-Chip RAM */
}

/*****************************************************************************
    段定义
*****************************************************************************/
SECTIONS
{
     /* 页表段 */
    .page_table (NOLOAD) :
    {
        . = ALIGN(0x4000);
        *(.page_table)
        *(.page_table.*)
        PROVIDE (__ocram_code_start__ = . );
        . += 4096;
        PROVIDE (__ocram_code_end__ = . );
    } > OCRAM

    /* 代码段  LMA == VMA == 0x30000000 */
    /* 具有常量属性段 */
    . = ORIGIN(ROM);
    . = ALIGN(0x100000);
    __code_start = .;
    .text :
    {
        KEEP(*(.vectors))       /* 使用KEEP使.vector段不被移除 */
        aw_backtrace_text_start = .;
        *(.text)
        *(.text.*)              /*  cpp namespace function      */
        *(.romrun)              /*  rom中必须的函数             */
        aw_backtrace_text_end = .;
        *(.rodata)              /*  read-only data (constants)  */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
    }>ROM

    /* section information for simple shell symbols */
    .text :
    {
        . = ALIGN(4);
        __shellsym_tab_start = .;
        KEEP(*(.shellsymbol))
        __shellsym_tab_end = .;
    }>ROM

    /* .ARM.exidx is sorted, so has to go in its own output section */
    . = ALIGN(4);
     __exidx_start = .;
     PROVIDE(__exidx_start = __exidx_start);
    .ARM.exidx :
    {
        /* __exidx_start = .; */
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* __exidx_end = .;   */
    }>ROM
    . = ALIGN(4);
     __exidx_end = .;
     PROVIDE(__exidx_end = __exidx_end);

    /*
     * C++ 全局对象构造与析构函数表
     * 这里放在 .text 和 .ARM.exidx 之后, .data 之前,
     * 这里的  LMA 和 VMA 相同, 如果放在 .data 之后, LMA 与 VMA 不同,
     * 则需要启动程序从装载区搬运到运行区
     */

    . = ALIGN(4);
    .ctors :
    {
        PROVIDE (__AW_CTOR_LIST__ = . );
        KEEP (*(.preinit_array))
        KEEP (*(.init_array))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        PROVIDE (__AW_CTOR_END__ = . );
    }>ROM

    .dtors :
    {
        PROVIDE (__AW_DTOR_LIST__ = . );
        KEEP (*(.fini_array))
        KEEP (*(SORT(.dtors.*)))        
        KEEP (*(.dtors))
        PROVIDE (__AW_DTOR_END__ = . );
    }>ROM

    . = ORIGIN(ROM) + LENGTH(ROM);
    PROVIDE (__code_end = .);
    
    /* 具有变量属性段 */
    . = ORIGIN(RAM);
    __data_start = .;
    /* 堆栈段 (暂不使用) */
    .stack (NOLOAD):
    {
        . = ALIGN(4);
        PROVIDE (__stacks_start = .);
        . += STACK_SIZE;
        PROVIDE (top_of_stacks = .);
    }>RAM

    /* 数据段 */
    .data ALIGN(4):
    {
         __data_section_start = . ;
         *(.data)
         *(.data.*)
         . = ALIGN(4);
         __data_section_end = .;
    }>RAM AT>ROM
    PROVIDE(__rom_data_start = LOADADDR(.data) );
    /*
     * 清零段
     * NOLOAD 表示不装载, 启动程序只需要借助 __bss_start 和 __bss_end 两个符号
     * 指定的起始地址和结束地址 将 .bss 区域清零即可
     */
    .bss (NOLOAD) : 
    {
        . = ALIGN(4);
        PROVIDE (__bss_start__ = .);
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        PROVIDE (__bss_end__ = .);
    }>RAM
    
    __data_end = ORIGIN(RAM) + LENGTH(RAM);

    /* 堆相关 */
    /* dma-safe 堆 */
    PROVIDE (__heap_dma_start__ = ORIGIN(DMAHEAP) );
    PROVIDE (__heap_dma_end__ = ORIGIN(DMAHEAP) + LENGTH(DMAHEAP) );

    /* lcd 显存 */ 
    PROVIDE (__lcd_vram_start__ = ORIGIN(VRAM) );
    PROVIDE (__lcd_vram_end__ = ORIGIN(VRAM) + LENGTH(VRAM) );

    /* 系统堆 */
    PROVIDE (__heap_start__ = ORIGIN(HEAP) );
    PROVIDE (__heap_end__ = ORIGIN(HEAP) + LENGTH(HEAP) );
}

/* end of file */
