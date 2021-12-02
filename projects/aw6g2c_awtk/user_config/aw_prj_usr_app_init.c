/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Apollo user application initialization entry file.
 *
 * \internal
 * \par History
 * - 1.02 19-05-07  cat, add lcd vram.
 * - 1.01 14-07-16  liuweiyun, fix code style and compiler warnings.
 * - 1.00 13-01-10  yangtao, first implementation.
 * \endinternal
 */

/*******************************************************************************
  headers
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"
#include "arch/arm/armv7/armv7_mmu.h"
#include "string.h"

/*******************************************************************************
  implementation
*******************************************************************************/

aw_import const struct awbl_devhcf *awbl_devhcf_list_get(void)
{
    extern aw_const struct awbl_devhcf g_awbl_devhcf_list[];

    return g_awbl_devhcf_list;
}

aw_import size_t awbl_devhcf_list_count_get(void)
{
    extern aw_const size_t g_awbl_devhcf_list_count;

    return g_awbl_devhcf_list_count;
}

/**
 * \brief user application initialization entry
 */
int aw_prj_usr_app_init (void)
{
    extern int aw_main (void);

    /* add user application initialization code here */
    return aw_main();
}

/******************************************************************************/

extern int __heap_dma_start__;
extern int __heap_dma_end__;
extern int __code_start;
extern int __code_end;
extern int __data_start;
extern int __data_end;
extern int __heap_start__;
extern int __heap_end__;
extern int __lcd_vram_start__;
extern int __lcd_vram_end__;
extern int __ocram_code_start__;
extern int __ocram_code_end__;


#define __CODE_ADDR_START       ((uint32_t)&__code_start)
#define __CODE_ADDR_END         ((uint32_t)&__code_end -1 )
#define __DATA_ADDR_START       ((uint32_t)&__data_start)
#define __DATA_ADDR_END         ((uint32_t)&__data_end - 1 )
#define __HEAP_ADDR_START       ((uint32_t)&__heap_start__)
#define __HEAP_ADDR_END         ((uint32_t)&__heap_end__ - 1 )
#define __HEAP_DMA_ADDR_START   ((uint32_t)&__heap_dma_start__)
#define __HEAP_DMA_ADDR_END     ((uint32_t)&__heap_dma_end__ - 1 )
#define __HEAP_VRAM_ADDR_START  ((uint32_t)&__lcd_vram_start__)
#define __HEAP_VRAM_ADDR_END    ((uint32_t)&__lcd_vram_end__ - 1 )


static unsigned int _page_table[16 * 1024] __attribute__((aligned(16 * 1024),
                                                        section(".page_table")));



const static struct sub_mem_desc __ocram_code = {
        (uint32_t)&__ocram_code_start__,
        (uint32_t)&__ocram_code_end__-1,
        (uint32_t)&__ocram_code_start__,
        kStronglyOrdered,
        kNonshareable,
        kROAccess,
        kExecutable
};



const static struct mem_desc g_mem_desc[] =
{
    /* ocram code */
    {
            0x00900000,
            0x009FFFFF,
            0x00000000,
            kOuterInner_WB_NOWA,
            kNonshareable,
            kROAccess,
            kExecutable,
            &__ocram_code,
            1,
    },
    /* DDR 常量*/
    {
            __CODE_ADDR_START,
            __CODE_ADDR_END,
            __CODE_ADDR_START,
            kOuterInner_WB_NOWA,
            kNonshareable,
            kROAccess,
            kExecutable
    },
    /* DDR 变量*/
    {
            __DATA_ADDR_START,
            __DATA_ADDR_END,
            __DATA_ADDR_START,
            kOuterInner_WB_NOWA,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },
    /* DDR DMA HEAP*/
    {
            __HEAP_DMA_ADDR_START,
            __HEAP_DMA_ADDR_END,
            __HEAP_DMA_ADDR_START,
            kStronglyOrdered,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },
    /* DDR 堆*/
    {
            __HEAP_ADDR_START,
            __HEAP_ADDR_END,
            __HEAP_ADDR_START,
            kOuterInner_WB_NOWA,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },

    /* APBHDMA */
    {
            0x01800000,
            0x01900000-1,
            0x01800000,
            kStronglyOrdered,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },

    /* AIPS-1和AIPS-2 */
    {
            0x02000000,
            0x02200000-1,
            0x02000000,
            kStronglyOrdered,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },
    /* GIC */
    {
            0x00A00000,
            0x00B00000-1,
            0x00A00000,
            kStronglyOrdered,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },

    /* QSPI */
    {
            0x60000000,
            0x60800000-1,
            0x60000000,
            kNoncacheable,
            kNonshareable,
            kROAccess,
            kNonExecutable
    },

    /* CAAM (32K secure RAM) */
    {
        0x00100000,
        0x00200000-1,
        0x00100000,
        kStronglyOrdered,
        kNonshareable,
        kRWAccess,
        kNonExecutable,
    },

    /* EIM */
    {
        0x50000000,
        0x57FFFFFF,
        0x50000000,
        kStronglyOrdered,
        kNonshareable,
        kRWAccess,
        kNonExecutable,
    },

    /* LCD VRAM */
    {
            __HEAP_VRAM_ADDR_START,
            __HEAP_VRAM_ADDR_END,
            __HEAP_VRAM_ADDR_START,
            kOuterInner_WT,
            kNonshareable,
            kRWAccess,
            kNonExecutable
    },
};

void target_init (void)
{
    const struct mem_desc  *p_desc = g_mem_desc;
    int                     count = AW_NELEMENTS(g_mem_desc);

    aw_hw_mmu_init(p_desc,
            count, _page_table,sizeof(_page_table));
}

/* end of file */
