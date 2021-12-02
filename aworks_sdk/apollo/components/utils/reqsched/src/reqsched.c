/*
 * reqsched.c
 *
 *  Created on: 2020年9月11日
 *      Author: wanglinfu
 */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "errno.h"

#include "reqsched_os.h"
#include "reqsched.h"

/******************************************************************************/
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask) && __g_os.pfn_log) {\
         __g_os.pfn_log(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define PFX  "REQSCHED"

/******************************************************************************/
#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef ASSERT
#define ASSERT(cond) assert(cond)
#endif

#define MUTEX_INIT(p_m)   __g_os.pfn_mutex_init(p_m)
#define MUTEX_LOCK(m)   __g_os.pfn_mutex_lock(m, -1)
#define MUTEX_UNLOCK(m) __g_os.pfn_mutex_unlock(m)

#define SEMB_INIT(p_m)    __g_os.pfn_semb_init(p_m)
#define SEMB_GET(m)     __g_os.pfn_semb_pend(m, -1)
#define SEMB_PUT(m)     __g_os.pfn_semb_release(m)

#ifndef ALIGN
#define ALIGN(x, align)   (((int) (x) + (align - 1)) & ~(align - 1))
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type,member)))
#endif
/******************************************************************************/

typedef struct reqsched {
    struct reqsched     *p_next;
    reqsched_hdr_t       pfn_req_hdr;
    uint32_t             req_max_cache_num;
    size_t               req_arg_size;

    reqsched_arg_t      *p_req_arg_queue;
    reqsched_arg_t      *p_free_list;
    reqsched_arg_t      *p_wait_list;       /* A list waiting to be processed. */
    reqsched_arg_t      *p_wait_rsp_list;   /* A list waiting for a response. */
    reqsched_arg_t      *p_comp_rsp_list;   /* A list of completed response. */
    void                *p_mutex;
} reqsched_t;

/******************************************************************************/
static reqsched_os_t __g_os;
static reqsched_t  *__gp_rs_list = NULL;

/******************************************************************************/
static rs_err_t __reqsched_arg_list_remove (reqsched_arg_t *p_list,
                                            reqsched_arg_t *p_target)
{
    reqsched_arg_t  *p_req;

    ASSERT(p_target);
    if (p_list == p_target) {
        p_req  = p_list;
        p_list = p_list->p_next;
        return RS_OK;
    }

    for (p_req = p_list; p_req != NULL; p_req = p_req->p_next) {
        if (p_req->p_next == p_target) {
            p_req->p_next    = p_req->p_next->p_next;
            p_target->p_next = NULL;
            return RS_OK;
        }
    }
    return -RS_ENOTFOUND;
}

/******************************************************************************/
static void __reqsched_task (void)
{
    __LOG_PFX(1, "Startup reqsched_task %d.", 0);

    while (1) {
        reqsched_t      *p_rs = __gp_rs_list;
        reqsched_arg_t  *p_arg;
        uint8_t          find = 0;

        for (;p_rs != NULL; p_rs = p_rs->p_next) {
            MUTEX_LOCK(p_rs->p_mutex);
            p_arg = p_rs->p_wait_list;
            if (p_arg) {
                /* 从等待处理链表中删除，加入到等待响应链表 */
                p_rs->p_wait_list      = p_arg->p_next;
                p_arg->p_next          = p_rs->p_wait_rsp_list;
                p_rs->p_wait_rsp_list  = p_arg;
                MUTEX_UNLOCK(p_rs->p_mutex);

                /* 处理请求，可在该请求处理回调中进行响应  */
                p_rs->pfn_req_hdr((uint32_t)p_rs,
                                  p_arg->data,
                                  p_rs->req_arg_size);
                find = 1;
            } else {
                MUTEX_UNLOCK(p_rs->p_mutex);
            }
        }

        /* Go to sleep when find=0.  */
        if (!find) {
            /* 处理完所有请求后，进行休眠，等待唤醒 */
            SEMB_GET(__g_os.wakeup);
            __LOG_PFX(1, "Wake up reqsched_task %d.", 0);
        }
    }
}
/******************************************************************************/
#define TASK_STACK_SIZE  1024

void reqsched_os_init (reqsched_os_t *p_os)
{
    __g_os = *p_os;

    SEMB_INIT(&__g_os.wakeup);
    __g_os.pfn_task_create("reqsched_task", __reqsched_task);
}

/**
 * \brief
 *
 * \param   pfn_req_hdr
 *
 * \retval  0
 * \retval  > 0
 */
uint32_t reqsched_create (reqsched_hdr_t pfn_req_hdr,
                          uint32_t       req_max_cache_num,
                          size_t         req_arg_size)
{
    reqsched_t      *p_rs;
    size_t           arg_total_size;
    size_t           arg_size;
    reqsched_arg_t  *p_arg;
    void            *p;

    ASSERT(req_arg_size && req_max_cache_num && pfn_req_hdr);

    p_rs = malloc(sizeof(reqsched_t));
    ASSERT(p_rs);
    memset(p_rs, 0, sizeof(*p_rs));

    arg_size = ALIGN(req_arg_size + sizeof(reqsched_arg_t), 4);
    arg_total_size = req_max_cache_num * arg_size;
    p_rs->p_req_arg_queue = malloc(arg_total_size);
    ASSERT(p_rs->p_req_arg_queue);
    memset(p_rs->p_req_arg_queue, 0, arg_total_size);
    __LOG_PFX(0, "malloc(%d Byte)", arg_total_size);

    MUTEX_INIT(&p_rs->p_mutex);

    p_rs->req_arg_size      = req_arg_size;
    p_rs->req_max_cache_num = req_max_cache_num;
    p_rs->pfn_req_hdr       = pfn_req_hdr;

    p_rs->p_free_list = NULL;
    p = p_rs->p_req_arg_queue;
    for (int i = 0; i < req_max_cache_num; i++) {
        p_arg = p + arg_size * i;
        p_arg->p_next = p_rs->p_free_list;
        p_rs->p_free_list = p_arg;
    }
    p = p_rs->p_req_arg_queue;
    ASSERT(p_arg == (p + (req_max_cache_num - 1) * arg_size));

    p_rs->p_next = __gp_rs_list;
    __gp_rs_list = p_rs;

    return (uint32_t)p_rs;
}

#if 0
rs_err_t reqsched_destroy (uint32_t reqid)
{
    reqsched_t      *p_rs = (reqsched_t *)reqid;
    reqsched_t      *p_rs_tmp;


    ASSERT(p_rs && p_rs->p_req_arg_queue);

    MUTEX_LOCK(p_rs->p_mutex);
    free(p_rs->p_req_arg_queue);
    p_rs->p_req_arg_queue = NULL;

    /* 从 rs 链表中删除 */
    if (__gp_rs_list == p_rs) {
        __gp_rs_list = p_rs->p_next;
    }

    for (p_rs_tmp = __gp_rs_list; p_rs_tmp != NULL; p_rs_tmp = p_rs_tmp->p_next) {
        if (p_rs_tmp->p_next == p_rs) {
            p_rs_tmp->p_next = p_rs->p_next;
            p_rs->p_next     = NULL;
            break;
        }
    }
    memset(p_rs, 0, sizeof(*p_rs));
    MUTEX_UNLOCK(p_rs->p_mutex);

    free(p_rs);
    return RS_OK;
}

#else
rs_err_t reqsched_destroy (uint32_t reqid)
{
    return -RS_ENOTSUP;
}
#endif

rs_err_t reqsched_req_send (uint32_t  reqid,
                            void     *p_data,
                            size_t    size,
                            uint32_t *p_token)
{
    rs_err_t         ret;
    reqsched_t      *p_rs = (reqsched_t *)reqid;
    reqsched_arg_t  *p_req;
    reqsched_arg_t  *p_req_tail;

    if (!p_rs) {
        return -RS_ESCHED;
    }
    ASSERT(p_rs && p_data && (size == p_rs->req_arg_size) && p_token);
    MUTEX_LOCK(p_rs->p_mutex);

    if (p_rs->p_free_list == NULL) {
        ret = -RS_ENOMEM;
        goto exit;
    }

    /* 从空闲链表中获取 */
    p_req               = p_rs->p_free_list;
    p_rs->p_free_list   = p_req->p_next;

    /* 加入到等待处理链表尾部 */
    p_req_tail = p_rs->p_wait_list;
    if (p_req_tail == NULL) {
        p_rs->p_wait_list   = p_req;
        p_req->p_next       = NULL;
    } else {
        while (p_req_tail != NULL) {
            if (p_req_tail->p_next == NULL) {
                break;
            }
            p_req_tail = p_req_tail->p_next;
        }
        ASSERT(p_req_tail);
        p_req_tail->p_next  = p_req;
        p_req->p_next       = NULL;
    }

    memcpy(p_req->data, p_data, size);
    *p_token = (uint32_t)p_req;

    /* 唤醒处理任务 */
    SEMB_PUT(__g_os.wakeup);

    MUTEX_UNLOCK(p_rs->p_mutex);
    return RS_OK;

exit:
    MUTEX_UNLOCK(p_rs->p_mutex);
    return ret;
}

int reqsched_req_available (uint32_t   reqid)
{
    rs_err_t         ret;
    reqsched_t      *p_rs = (reqsched_t *)reqid;
    reqsched_arg_t  *p_req;
    int              num;

    if (!p_rs) {
        /* Operation failed, return a negative value. */
        return -RS_ESCHED;
    }
    ASSERT(p_rs);
    MUTEX_LOCK(p_rs->p_mutex);

    p_req = p_rs->p_free_list;
    num = 0;
    while (p_req != NULL) {
         num++;
         p_req = p_req->p_next;
    }

    /* Operation successfully, return number. */
    MUTEX_UNLOCK(p_rs->p_mutex);
    return num;
}

rs_err_t reqsched_req_in_process (uint32_t reqid)
{
    reqsched_t      *p_rs = (reqsched_t *)reqid;
    MUTEX_LOCK(p_rs->p_mutex);

    MUTEX_UNLOCK(p_rs->p_mutex);
    return RS_OK;
}


rs_err_t reqsched_rsp_send (uint32_t reqid, void *p_data)
{
    rs_err_t         ret;
    reqsched_t      *p_rs  = (reqsched_t *)reqid;
    reqsched_arg_t  *p_req = NULL;
    reqsched_arg_t  *p_req_tmp;

    if (!p_rs) {
        return -RS_ESCHED;
    }
    ASSERT(p_rs && p_data);
    MUTEX_LOCK(p_rs->p_mutex);

    ASSERT(p_rs->p_wait_rsp_list);

    /* 从等待响应 链表中删除 */
    if (p_rs->p_wait_rsp_list->data == p_data) {
        p_req = p_rs->p_wait_rsp_list;
        p_rs->p_wait_rsp_list = p_rs->p_wait_rsp_list->p_next;
    }

    p_req_tmp = p_rs->p_wait_rsp_list;
    for (; p_req_tmp != NULL; p_req_tmp = p_req_tmp->p_next) {
        if (p_req_tmp->p_next->data == p_data) {
            /* 从等待响应 链表中删除 */
            p_req             = p_req_tmp->p_next;
            p_req_tmp->p_next = p_req->p_next;
            break;
        }
    }

    if (p_req == NULL) {
        ret = -RS_ENOREQ;
        goto exit;
    }

    /* 加入到已响应链表 */
    p_req->p_next         = p_rs->p_comp_rsp_list;
    p_rs->p_comp_rsp_list = p_req;

    MUTEX_UNLOCK(p_rs->p_mutex);
    return RS_OK;

exit:
    MUTEX_UNLOCK(p_rs->p_mutex);
    return ret;
}

rs_err_t reqsched_rsp_tryget (uint32_t   reqid,
                              uint32_t   token,
                              void      *p_data,
                              size_t     size)
{
    rs_err_t         ret;
    reqsched_t      *p_rs = (reqsched_t *)reqid;
    reqsched_arg_t  *p_rsp = (reqsched_arg_t *)token;

    if (!p_rs) {
        return -RS_ESCHED;
    }
    ASSERT(p_rs && token && (size == p_rs->req_arg_size));
    MUTEX_LOCK(p_rs->p_mutex);

    ret = __reqsched_arg_list_remove(p_rs->p_comp_rsp_list, p_rsp);
    if (ret == -RS_ENOTFOUND) {
        ret = -RS_EPROCESSING;
        goto exit;
    }
    ASSERT(RS_OK == ret);

    memcpy(p_data, p_rsp->data, size);
    /* 移动到空闲链表 */
    p_rsp->p_next     = p_rs->p_free_list;
    p_rs->p_free_list = p_rsp;

    MUTEX_UNLOCK(p_rs->p_mutex);
    return RS_OK;

exit:
    MUTEX_UNLOCK(p_rs->p_mutex);
    return ret;
}
