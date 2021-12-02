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
#include "aworks.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "usb_os.h"
#include "interior/mem/usb_mem.h"
#include <string.h>


/* USB 信号量*/
struct __usb_sem {
    AW_SEMC_DECL(s);
};

/* USB 互斥锁*/
struct __usb_mutex {
    AW_MUTEX_DECL(m);
};

/** 寄存器写函数*/
 void usb_writel (uint32_t val, volatile void *addr){
     writel(val, (volatile void __iomem *)(addr));
 }

 /** 寄存器读函数*/
 uint32_t usb_readl (volatile void *addr){
     return readl((volatile void __iomem *)(addr));
 }

/** 任务创建函数*/
usb_task_handle_t usb_task_create (const char  *name,
                                   int          prio,
                                   size_t       stk_s,
                                   void       (*pfnc) (void *p_arg),
                                   void        *p_arg)
{
    return (usb_task_handle_t)aw_task_create(name, prio, stk_s, pfnc, p_arg);
}

/** 任务删除函数*/
usb_err_t usb_task_delete (usb_task_handle_t tsk)
{
    return aw_task_delete((aw_task_id_t)tsk);
}

/** 任务启动函数*/
usb_err_t usb_task_startup (usb_task_handle_t tsk)
{
    return aw_task_startup((aw_task_id_t)tsk);
}

/** 任务挂起函数*/
usb_err_t usb_task_suspend (usb_task_handle_t tsk)
{
    return aw_task_suspend((aw_task_id_t)tsk);
}

/** 任务恢复函数*/
usb_err_t usb_task_resume (usb_task_handle_t tsk)
{
    return aw_task_resume((aw_task_id_t)tsk);
}

/** 创建信号量*/
usb_sem_handle_t usb_sem_create (void)
{
    struct __usb_sem *p_sem;

    p_sem = (struct __usb_sem *)usb_mem_alloc(sizeof(struct __usb_sem));
    if (p_sem == NULL) {
        return NULL;
    }
    memset(p_sem, 0, sizeof(*p_sem));
    if (AW_SEMC_INIT(p_sem->s, 0, AW_SEM_Q_PRIORITY) == NULL) {
        usb_mem_free(p_sem);
        return NULL;
    }
    return (usb_sem_handle_t)p_sem;
}

/** 删除信号量*/
void usb_sem_delete (usb_sem_handle_t sem)
{
    AW_SEMC_TERMINATE(((struct __usb_sem *)sem)->s);
    usb_mem_free(sem);
}

/** 等待信号量*/
usb_err_t usb_sem_take (usb_sem_handle_t sem,
                        int              timeout)
{
    return AW_SEMC_TAKE(((struct __usb_sem *)sem)->s, timeout);
}

/** 释放信号量*/
usb_err_t usb_sem_give (usb_sem_handle_t sem)
{
    return AW_SEMC_GIVE(((struct __usb_sem *)sem)->s);
}

/** 创建互斥锁*/
usb_mutex_handle_t usb_mutex_create (void)
{
    struct __usb_mutex *p_mutex;

    p_mutex = (struct __usb_mutex *)usb_mem_alloc(sizeof(struct __usb_mutex));
    if (p_mutex == NULL) {
        return NULL;
    }

    memset(p_mutex, 0, sizeof(*p_mutex));
    if (AW_MUTEX_INIT(p_mutex->m, AW_SEM_Q_PRIORITY) == NULL) {
        usb_mem_free(p_mutex);
        return NULL;
    }
    return (usb_mutex_handle_t)p_mutex;
}

/** 删除互斥锁*/
void usb_mutex_delete (usb_mutex_handle_t mutex)
{
    AW_MUTEX_TERMINATE(((struct __usb_mutex *)mutex)->m);
    usb_mem_free(mutex);
}

/** 互斥锁上锁*/
usb_err_t usb_mutex_lock (usb_mutex_handle_t mutex,
                          int                timeout)
{
    return AW_MUTEX_LOCK(((struct __usb_mutex *)mutex)->m, timeout);
}

/** 互斥锁解锁*/
usb_err_t usb_mutex_unlock (usb_mutex_handle_t mutex)
{
    return AW_MUTEX_UNLOCK(((struct __usb_mutex *)mutex)->m);
}

