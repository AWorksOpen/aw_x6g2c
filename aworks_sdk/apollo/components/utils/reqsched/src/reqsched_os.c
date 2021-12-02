/*
 * reqsched_os.c
 *
 *  Created on: 2020年9月11日
 *      Author: wanglinfu
 */

#include "aworks.h"
#include "stdlib.h"
#include "string.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_shell.h"
#include "reqsched.h"
#include "aw_task.h"

#include "stdlib.h"

#ifndef AW_REQSCHED_TASK_NUM
#define AW_REQSCHED_TASK_NUM  3
#endif  /* AW_REQSCHED_TASK_NUM */

#ifndef AW_REQSCHED_STACK_SIZE
#define AW_REQSCHED_STACK_SIZE 1024*4
#endif /* AW_REQSCHED_STACK_SIZE */

#ifndef AW_REQSCHED_PRIORITY
#define AW_REQSCHED_PRIORITY   5
#endif /* AW_REQSCHED_PRIORITY */

aw_local void __task_entry (void *p_arg)
{
    void (*cb) (void) = p_arg;
    cb();
}


aw_local void __task_create (const char *name, void (*cb) (void))
{
    aw_err_t ret;
    AW_TASK_DECL_STATIC(task[AW_REQSCHED_TASK_NUM], AW_REQSCHED_STACK_SIZE);

    for (int i = 0; i < AW_REQSCHED_TASK_NUM; i++) {
        AW_TASK_INIT(task[i],             /* 任务实体 */
                     name,           /* 任务名字 */
                     AW_REQSCHED_PRIORITY,                 /* 任务优先级 */
                     AW_REQSCHED_STACK_SIZE,           /* 任务堆栈大小 */
                     __task_entry,     /* 任务入口函数 */
                     cb);             /* 任务入口参数 */

        ret = AW_TASK_STARTUP(task[i]);
        aw_assert(ret == AW_OK);
    }
}

/******************************************************************************/
static void __log (void         *p_color,
                   const char   *func_name,
                   int           line,
                   const char   *fmt, ...)
{
#ifdef __VIH_DEBUG
    char        log_buf[512 + 1];
    uint16_t    len;
    va_list     ap;

    va_start( ap, fmt );
    len = aw_vsnprintf((char *)log_buf, sizeof(log_buf), fmt, ap);
    va_end( ap );
    if (len >= sizeof(log_buf)) {
        aw_assert(len < sizeof(log_buf) - 1);
    }

    extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                           const char *color,
                                           const char *func_name,
                                           const char *fmt, ...);
    aw_shell_printf_with_color(AW_DBG_SHELL_IO, p_color, func_name, log_buf);
#endif
}


/******************************************************************************/
typedef struct { 
    AW_SEMB_DECL(semb);
} semb_t;

aw_local void __semb_init (void **pp_semb)
{
    semb_t *p;    

    aw_assert(pp_semb);
    p = malloc(sizeof(semb_t));
    aw_assert(p);
    memset(p, 0, sizeof(semb_t));
    
    aw_assert(AW_SEMB_INIT(p->semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY) != NULL);
    *pp_semb = (void *)p;    
}

aw_local rs_err_t __semb_pend (void *p, uint32_t systick_tm)
{
    aw_err_t ret;
    semb_t *p_semb = (semb_t *)p;
    aw_assert(p_semb);
    
    ret = AW_SEMB_TAKE(p_semb->semb, systick_tm);    
    aw_assert(ret == AW_OK);
    if (ret != AW_OK) {
        return -RS_ETIME;
    }
    return RS_OK;
}

aw_local rs_err_t __semb_release (void *p)
{
    aw_err_t ret;
    semb_t *p_semb = (semb_t *)p;
    aw_assert(p_semb);
    
    ret = AW_SEMB_GIVE(p_semb->semb);        
    aw_assert(ret == AW_OK);
    if (ret != AW_OK) {
        return -RS_ETIME;
    }
    return RS_OK;
}
/******************************************************************************/
typedef struct {
    AW_MUTEX_DECL(mutex);
} mutex_t;

aw_local void __mutex_init (void **p)
{
    mutex_t *p_mutex;
    
    aw_assert(p);
    *p = malloc(sizeof(mutex_t));
    aw_assert(*p);
    memset(*p, 0, sizeof(mutex_t));
    p_mutex = *(mutex_t **)p;
    
    aw_assert(AW_MUTEX_INIT(p_mutex->mutex, AW_SEM_Q_PRIORITY) != NULL);
}

aw_local rs_err_t __mutex_lock (void *p_mutex, uint32_t systick_tm)
{
    aw_err_t ret;
    mutex_t *p = (mutex_t *)p_mutex;
    aw_assert(p);
    
    ret = AW_MUTEX_LOCK(p->mutex, systick_tm);    
    aw_assert(ret == AW_OK);
    if (ret != AW_OK) {
        return -RS_ETIME;
    }
    return RS_OK;
}
aw_local rs_err_t __mutex_unlock (void *p_mutex)
{
    aw_err_t ret;        
    mutex_t *p = (mutex_t *)p_mutex;
    aw_assert(p);
    
    ret = AW_MUTEX_UNLOCK(p->mutex);
    aw_assert(ret == AW_OK);
    if (ret != AW_OK) {
        return -RS_ETIME;
    }
    return RS_OK;
}
/******************************************************************************/
reqsched_os_t __g_os = {
    .pfn_task_create    = __task_create,
    .pfn_semb_init      = __semb_init,
    .pfn_semb_pend      = __semb_pend,
    .pfn_semb_release   = __semb_release,
    .pfn_mutex_init     = __mutex_init,
    .pfn_mutex_lock     = __mutex_lock,
    .pfn_mutex_unlock   = __mutex_unlock,
    .pfn_log            = __log,

};

void aw_reqschd_lib_init (void)
{
    static int __inited = AW_FALSE;
    if (__inited) {
        return;
    }
    __inited = AW_TRUE;
    reqsched_os_init(&__g_os);
}






