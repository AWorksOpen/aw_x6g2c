#include "apollo.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"


extern void imx6ul_cpu_idle_asm();


extern int __ocram_code_start__;
extern int __ocram_code_end__;
extern int __code_imx6ul_cpu_power_end;
extern int __code_imx6ul_cpu_power_start;

extern volatile uint8_t is_timestamp_enable;

typedef void (* pfn_cpu_idle_t)(void );

volatile pfn_cpu_idle_t fn_imx6ul_cpu_idle = NULL;
volatile pfn_cpu_idle_t fn_imx6ul_cpu_idle_dup = NULL;

void imx6ul_cpu_power_init()
{
    char       *p_src1,*p_src2,*p_dst1,*p_dst2;
    int         src_len,dst_len;

    p_src1 = (char *)&__code_imx6ul_cpu_power_start;
    p_src2 = (char *)&__code_imx6ul_cpu_power_end;
    p_dst1 = (char *)&__ocram_code_start__;
    p_dst2 = (char *)&__ocram_code_end__;

    src_len = p_src2 - p_src1;
    dst_len = p_dst2 - p_dst1;

    if(src_len > dst_len) {
        while(1);
    }
    memcpy(p_dst1,p_src1,src_len);

    fn_imx6ul_cpu_idle_dup =(pfn_cpu_idle_t) (
            (char *)imx6ul_cpu_idle_asm - p_src1 + p_dst1);

}

static uint32_t __g_sys_tick_freq = 0;
static uint32_t __g_idle_tick_count = 0;

static uint32_t __g_append_count;
static uint32_t __g_roll_over_count;

void imx6ul_cpu_idle_sys_tick_init()
{
    __g_sys_tick_freq = systick_timer_get_clk_freq();

    systick_timer_get_rollover_count(&__g_roll_over_count);

    fn_imx6ul_cpu_idle = fn_imx6ul_cpu_idle_dup;
}

void pm_cpu_idle(void) 
{
    register int old;
    
    if (is_timestamp_enable) {

    } else {
        old = arch_interrupt_disable();
        fn_imx6ul_cpu_idle_dup();
        arch_interrupt_enable(old);
    }
}

void imx6ul_cpu_idle(void)
{
    uint32_t            count1,count2;

    systick_timer_get_count(&count1);
    fn_imx6ul_cpu_idle();
    systick_timer_get_count(&count2);

    if (count1 < count2) {
        count2 -= count1;
    }
    else {
        count2 = count2 + __g_roll_over_count - count1;
    }

    __g_append_count += count2;
    while(__g_append_count > __g_roll_over_count) {
        __g_append_count -= __g_roll_over_count;
        __g_idle_tick_count ++;
    }
}

void get_cpu_run_counters (uint32_t *p_total,uint32_t *p_idle)
{
    *p_total = aw_sys_tick_get();
    *p_idle = __g_idle_tick_count;
}
