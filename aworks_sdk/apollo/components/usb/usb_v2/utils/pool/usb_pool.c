/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "usb_adapter_inter.h"
#include "usb_common.h"
#include "pool/usb_pool.h"

static usb_mutex_handle_t __g_usb_pool_mutex;
static usb_bool_t         __g_usb_pool_init = USB_FALSE;

/** \brief tructure representing a free block in the Native Memory Pool */
typedef struct __free_item {
    struct __free_item *p_next;
} __free_item_t;


/**
 * 初始化一个内存池
 *
 * @param p_pool     内存池句柄
 * @param p_pool_mem 内存池缓存
 * @param pool_size  内存池大小
 * @param item_size  内存池每一项大小
 *
 * @return 成功返回内存池句柄，失败返回NULL
 */
usb_pool_id_t usb_pool_init(usb_pool_t *p_pool,
                            void      *p_pool_mem,
                            size_t     pool_size,
                            size_t     item_size)
{
    __free_item_t  *fb;
    size_t          corr;
    size_t          nblocks;

    /* 初始化互斥锁*/
    if(__g_usb_pool_init == USB_FALSE){
        __g_usb_pool_mutex = usb_mutex_create();
        if(__g_usb_pool_mutex == NULL){
            return NULL;
        }
        __g_usb_pool_init = USB_TRUE;
    }

    /* The memory block must be valid
     * and the poolSize must fit at least one free block
     * and the blockSize must not be too close to the top of the dynamic range
     */
    if (!((p_pool_mem != NULL) &&
          (pool_size >= (uint32_t)sizeof(__free_item_t)) &&
          (item_size + sizeof(__free_item_t) > item_size))) {

        return NULL;
    }

    corr = (size_t)p_pool_mem & (sizeof(__free_item_t) - 1);
    if (corr != 0) {                        /* alignment needed? */
        corr = sizeof(__free_item_t) - corr;/*amount to align poolSto*/
        pool_size -= corr;                  /* reduce the available pool size */
    }

    p_pool->p_free = (void *)((uint8_t *)p_pool_mem + corr);

    /* round up the blockSize to fit an integer # free blocks, no division */
    /* start with just one */
    p_pool->item_size= sizeof(__free_item_t);
    /* # free blocks that fit in one memory block */
    nblocks = 1;
    while (p_pool->item_size < item_size) {
        p_pool->item_size += sizeof(__free_item_t);
        ++nblocks;
    }
    item_size= p_pool->item_size;  /* use the rounded-up value from now on */

    /* the pool buffer must fit at least one rounded-up block */
    if (pool_size < item_size) {
        return NULL;
    }

    /* chain all blocks together in a free-list... */
    pool_size -= item_size;         /* don't count the last block */
    p_pool->item_count= 1;          /* the last block already in the pool */
    fb = (__free_item_t *)p_pool->p_free;/*start at the head of the free list */
    while (pool_size>= item_size) {
        fb->p_next = &fb[nblocks];  /* point the next link to the next block */
        fb = fb->p_next;                    /* advance to the next block */
        pool_size -= (uint32_t)item_size;   /* reduce the available pool size */
        ++p_pool->item_count;       /* increment the number of blocks so far */
    }

    fb->p_next  = NULL;                 /* the last link points to NULL */
    p_pool->nfree = p_pool->item_count; /* all blocks are free */
    p_pool->nmin  = p_pool->item_count; /* the minimum number of free blocks */
    p_pool->p_start = p_pool_mem;       /* the original start this pool buffer */
    p_pool->p_end   = fb;               /* the last block in this pool */

    return p_pool;
}

/******************************************************************************/
void *usb_pool_item_get (usb_pool_id_t pool)
{
    usb_pool_t *p_pool = (usb_pool_t *)pool;
    __free_item_t *fb;

    usb_mutex_lock(__g_usb_pool_mutex, USB_WAIT_FOREVER);

    fb = (__free_item_t *)p_pool->p_free; /* get a free block or NULL */
    if (fb != NULL) {                   /* free block available? */
        p_pool->p_free = fb->p_next;    /* adjust list head to the next free block */
        --p_pool->nfree;                /* one less free block */
        if (p_pool->nmin > p_pool->nfree) {
            p_pool->nmin = p_pool->nfree;            /* remember the minimum so far */
        }
    }

    usb_mutex_unlock(__g_usb_pool_mutex);

    return fb;            /* return the block or NULL pointer to the caller */
}

/******************************************************************************/
usb_err_t usb_pool_item_return (usb_pool_id_t pool, void *p_item)
{
    usb_pool_t *p_pool = (usb_pool_t *)pool;

    usb_mutex_lock(__g_usb_pool_mutex, USB_WAIT_FOREVER);
    /*  must be in range */
    if (!((p_pool->p_start <= p_item) && (p_item <= p_pool->p_end)
              && (p_pool->nfree <= p_pool->item_count))) {

        usb_mutex_unlock(__g_usb_pool_mutex);
        return -USB_EFAULT;
    }

    /* link into free list */
    ((__free_item_t *)p_item)->p_next = (__free_item_t *)p_pool->p_free;
    p_pool->p_free = p_item;       /* set as new head of the free list */
    ++p_pool->nfree;               /* one more free block in this pool */

    usb_mutex_unlock(__g_usb_pool_mutex);

    return USB_OK;
}

/******************************************************************************/
size_t usb_pool_margin_get (usb_pool_id_t pool)
{
    usb_pool_t *p_pool = (usb_pool_t *)pool;
    size_t margin;

    usb_mutex_lock(__g_usb_pool_mutex, USB_WAIT_FOREVER);
    margin = p_pool->nmin;
    usb_mutex_unlock(__g_usb_pool_mutex);

    return margin;
}
