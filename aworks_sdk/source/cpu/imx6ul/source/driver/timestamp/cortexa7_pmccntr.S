.code 32

#if __thumb__
# define FUNC(name) .type name, %function; .thumb_func; name:
# define SET .thumb_set
#else
# define FUNC(name) .type name, %function; name:
# define SET .set
#endif
#define GLOBAL(name) .global name; FUNC(name)
#define SIZE(name) .size name, .-name
#define FUNC_BEGIN(name) GLOBAL(## name)
#define FUNC_END(name) SIZE(## name)

/*
 * uint32_t read_timestamp(void)
 * 读取pmccntr中的值
 */
FUNC_BEGIN(read_timestamp)
    MRC p15, 0, r0, c9, c13, 0
    bx lr
FUNC_END(read_timestamp)

/*
 * void timestamp_ovfl_int_enable(void)
 * 使能pmccntr溢出中断
 */
FUNC_BEGIN(timestamp_ovfl_int_enable)
	LDR r0, =0x80000000
    MCR p15, 0, r0, c9, c14, 1
    bx lr
FUNC_END(timestamp_ovfl_int_enable)

/*
 * uint32_t read_timestamp_ovfl_bit(void)
 * 读取pmovsr中溢出bit
 */
FUNC_BEGIN(read_timestamp_ovfl_bit)
    MRC p15, 0, r0, c9, c12, 3
    bx lr
FUNC_END(read_timestamp_ovfl_bit)

/*
 * void clr_timestamp_ovfl_bit(void)
 * 清除pmccntr溢出bit
 */
FUNC_BEGIN(clr_timestamp_ovfl_bit)
	LDR r0, =0x80000000
    MCR p15, 0, r0, c9, c12, 3
    bx lr
FUNC_END(clr_timestamp_ovfl_bit)


/*
 * void timestamp_enable(void)
 * 使能内核计数器
 */
FUNC_BEGIN(timestamp_enable)

    /* PMUSERENR = 1  User mode access to the Performance Monitors enabled*/
    MOV r0, #1
    MCR p15, 0, r0, c9, c14, 0

    /*
     * PMCR.E(bit 0) = 1 All counters are enabled, PMCR.C(bit 2) = 1 Reset PMCCNTR to zero.
     */
    MOV r2, #5
    MCR p15, 0, r2, c9, c12, 0

    /*
     * PMCNTENSET.C(bit 31) = 1, Cycle counter enabled
     * 不加获取出来的pmccntr永远是0ss
     */
    MOV R0, #1
    LSL R3, R0, #31
    MCR p15, 0, R3, c9, c12, 1

    bx lr
FUNC_END(timestamp_enable)
