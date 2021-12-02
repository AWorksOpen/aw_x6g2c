/*******************************************************************************
*                                 Apollo
*                       ----------------------------
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
 * \brief memory management
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-13  zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_mem.h"
#include "aw_bsp_mem.h"
#include "aw_memheap.h"
#include "aw_cache.h"

#include <string.h>

/*******************************************************************************
  locals
*******************************************************************************/
static struct aw_memheap __g_system_heap;       /* system heap object */
static struct aw_memheap __g_dma_safe_heap;     /* dma-safe heap object */


/* 判断一个数是否为2的n次方 */
#define     __IS_NOT_SECOND_PARTY(a)    (((a) & ((a)-1)) ? 1 : 0)

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
void aw_bsp_system_heap_init(void)
{
#ifdef  __GNUC__
    extern int __heap_start__;		/* Defined by the linker */
    extern int __heap_end__;		/* Defined by the linker */

    char *heap_start = (char *)&__heap_start__;
    char *heap_end   = (char *)&__heap_end__;

    /* initialize a default heap in the system */
    aw_memheap_init(&__g_system_heap,
                    "system_heap",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);

#elif defined(__CC_ARM)
    extern int __heap_base;  /* define in the **startup.s */
    extern int __heap_limit; /* define in the **startup.s */

    int   *heap_start = (int *)&__heap_base;
    int   *heap_end   = (int *)&__heap_limit;

    /* initialize a default heap in the system */
    aw_memheap_init(&__g_system_heap,
                    "system_heap",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);
#endif
}

/******************************************************************************/
void *aw_mem_align(size_t size, size_t align)
{
    void    *align_ptr;
    void    *ptr;
    size_t   align_size;
    
    
    if (__IS_NOT_SECOND_PARTY(align) || (align <= 1)) {
        return NULL;
    }

    /* align the alignment size to 4 byte */
    align = AW_ROUND_UP(align, sizeof(void *));

    /* get total aligned size */
    align_size =  AW_ROUND_UP(size, sizeof(void *)) + align + sizeof(void *);

    /* allocate memory block from heap */
    ptr = aw_memheap_alloc(&__g_system_heap, align_size);
    if (ptr != NULL) {

        /* round up ptr to align */
        align_ptr = (void *)AW_ROUND_UP((uint32_t)ptr + sizeof(void *), align);

        /* set the pointer before alignment pointer to the real pointer */
        *((uint32_t *)((uint32_t)align_ptr - sizeof(void *))) = (uint32_t)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

/******************************************************************************/
void *aw_mem_alloc(size_t size)
{
    /* allocate in the system heap */
    return aw_mem_align(size, 4);
}

/******************************************************************************/
void *aw_mem_calloc(size_t nelem, size_t size)
{
    void       *ptr;
    size_t      total_size;

    total_size = nelem * size;
    ptr = aw_mem_alloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

/******************************************************************************/
void aw_mem_free(void *ptr)
{
    void *real_ptr = NULL;

    if (ptr != NULL) {
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    }
    aw_memheap_free(real_ptr);
}

/******************************************************************************/
void *aw_mem_realloc(void *ptr, size_t newsize)
{
    void  *real_ptr     = NULL;
    void  *new_ptr      = NULL;

    if (ptr != NULL) {
        /* get real pointer */
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    } else {
        /* equal malloc when ptr==NULL */
        return aw_mem_alloc(newsize);
    }

    /* allocate newsize plus one more pointer, used for save real pointer */
    real_ptr = aw_memheap_realloc(&__g_system_heap,
                                  real_ptr,
                                  newsize + sizeof(void *));

    if (real_ptr != NULL) {
        *(uint32_t *)real_ptr = (uint32_t)real_ptr;
        new_ptr               = (void *)((size_t)real_ptr + sizeof(void *));
    } else {
        new_ptr = NULL;
    }

    return new_ptr;
}

/******************************************************************************/
void *aw_memp_alloc(aw_heap_id_t heap_id,size_t size)
{
    aw_memheap_t *heap = (aw_memheap_t *)heap_id;

    /* allocate memory from system heap */
    if (heap == NULL) {
        return aw_mem_alloc(size);
    }

    /* allocate memory from specify heap */
    return aw_memheap_alloc(heap, size);
}

/******************************************************************************/
void aw_bsp_dma_safe_heap_init(void)
{
#ifdef  __GNUC__
    extern char __heap_dma_start__;     /* Defined by the linker */
    extern char __heap_dma_end__;       /* Defined by the linker */

    static char *heap_start = &__heap_dma_start__;
    static char *heap_end   = &__heap_dma_end__;

    /* initialize a default heap in the system */
    aw_memheap_init(&__g_dma_safe_heap,
                    "dma_safe_heap",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);

#elif defined(__CC_ARM)
    extern int __heap_dma_start__;     /* Defined by the linker */
    extern int __heap_dma_end__;       /* Defined by the linker */

    int   *heap_dma_start = (int *)__heap_dma_start__;
    int   *heap_dma_end   = (int *)__heap_dma_end__;

    /* initialize a dma heap in the system */
    aw_memheap_init(&__g_dma_safe_heap,
                    "dma_safe_heap",
                    (void *)heap_dma_start,
                    (uint32_t)heap_dma_end - (uint32_t)heap_dma_start);

#endif
}


/******************************************************************************/
void *aw_cache_dma_align (size_t nbytes, size_t align)
{
    size_t  align_size;
    void   *ptr;
    void   *aligned_ptr;
    
    if (__IS_NOT_SECOND_PARTY(align) || (align <= 1)) {
        return NULL;
    }

    /* align the alignment size to cache line size */
    align  = AW_ROUND_UP(align, AW_CACHE_LINE_SIZE);

    /* align allocate size to cache line size */
    nbytes = AW_ROUND_UP(nbytes, AW_CACHE_LINE_SIZE);

    /* get total align size */
    align_size = nbytes + align;

    /* allocate in the dma-safe heap */
    ptr = aw_memheap_alloc(&__g_dma_safe_heap, align_size);
    if (ptr != NULL) {

        /* the allocated memory block is aligned */
        if (AW_ALIGNED((uint32_t)ptr, align)) {
            aligned_ptr = (void *)((uint32_t)ptr + align);
        } else {
            aligned_ptr = (void *)(AW_ROUND_UP((uint32_t)ptr, align));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((uint32_t *)((uint32_t)aligned_ptr - sizeof(void *))) = (uint32_t)ptr;

        ptr = aligned_ptr;
    }

    return ptr;
}

/******************************************************************************/
void *aw_cache_dma_malloc (size_t nbytes)
{
    return aw_cache_dma_align(nbytes, AW_CACHE_LINE_SIZE);
}

/******************************************************************************/
void aw_cache_dma_free (void *ptr)
{
    void *real_ptr = NULL;

    if (ptr != NULL) {
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    }
    aw_memheap_free(real_ptr);
}

/* end of file */
