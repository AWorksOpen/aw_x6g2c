/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

    /* 说明下面的指令是ARM和THUMB通用格式的 */
    .syntax  unified
    .thumb

    .global imx6ul_system_reset

    .set    REG_SRC_SCR,     0x020D8000
    .set    BIT_SOFT_RESET, (1 << 21)

    /**
     * void imx6ul_system_reset(void)
     *
     * \brief 此函数用于imx6ul软件复位，默认复位模式为上一次硬件复位的模式，并不会查询当前的外部复位模式
     *        进行复位（例如上电时复位模式为sd卡启动，上电后将外部复位模式改为改为norflash启动，此时依旧会
     *        按照sd卡启动的模式来复位）。
     */
    .section  .text.imx6ul_system_reset
    .type  imx6ul_system_reset, %function

imx6ul_system_reset:
    /* 关闭全局中断 */
    cpsid   i

    //设置 REG_SRC_SCR，等待系统复位
    ldr     r0, = REG_SRC_SCR
    ldr     r1, = BIT_SOFT_RESET
    str     r1, [r0]
    dsb

/* 不能用b . */
loop:
    nop
    b      loop

    .size  imx6ul_system_reset, . - imx6ul_system_reset



