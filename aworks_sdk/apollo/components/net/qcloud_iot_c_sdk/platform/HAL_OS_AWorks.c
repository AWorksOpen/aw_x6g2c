/*
 * Tencent is pleased to support the open source community by making IoT Hub
 available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights
 reserved.

 * Licensed under the MIT License (the "License"); you may not use this file
 except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software
 distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND,
 * either express or implied. See the License for the specific language
 governing permissions and
 * limitations under the License.
 *
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "AWorks.h"
#include "aw_mem.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_time.h"

#include "qcloud_iot_export_error.h"
#include "qcloud_iot_import.h"

typedef struct hal_mutex {
    AW_MUTEX_DECL(lock);
} hal_mutex_t ;

void *HAL_MutexCreate(void)
{
#ifdef MULTITHREAD_ENABLED
    hal_mutex_t *my_mutex;
    my_mutex = (hal_mutex_t *)aw_mem_alloc(sizeof(hal_mutex_t));
    if (my_mutex == NULL) {
        return NULL;
    }

    AW_MUTEX_INIT(my_mutex->lock, AW_SEM_Q_PRIORITY);

    return my_mutex;
#else
    return (void *)0xFFFFFFFF;
#endif
}

void HAL_MutexDestroy(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    hal_mutex_t *my_mutex = mutex;
    AW_MUTEX_TERMINATE(my_mutex->lock);
    aw_mem_free(my_mutex);
    return ;
#else
    return;
#endif
}

void HAL_MutexLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    int err;
    hal_mutex_t *my_mutex = mutex;
    err = AW_MUTEX_LOCK(my_mutex->lock, AW_SEM_WAIT_FOREVER);
    if (err < 0) {
        HAL_Printf("%s: lock mutex failed\n", __FUNCTION__);
    }
#else
    return;
#endif
}

int HAL_MutexTryLock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    int err;
    hal_mutex_t *my_mutex = mutex;
    err = AW_MUTEX_LOCK(my_mutex->lock, AW_SEM_NO_WAIT);
    if (err < 0) {
        HAL_Printf("%s: lock mutex failed\n", __FUNCTION__);
    }
    return err;
#else
    return 0;
#endif
}

void HAL_MutexUnlock(_IN_ void *mutex)
{
#ifdef MULTITHREAD_ENABLED
    int err;
    hal_mutex_t *my_mutex = mutex;
    err = AW_MUTEX_UNLOCK(my_mutex->lock);
    if (err < 0) {
        HAL_Printf("%s: unlock mutex failed\n", __FUNCTION__);
    }
#else
    return;
#endif
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aw_mem_alloc(size);
}

void HAL_Free(_IN_ void *ptr)
{
    if (ptr)
        aw_mem_free(ptr);
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    return;
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

uint32_t HAL_GetTimeMs(void)
{
    struct aw_timespec tv;
    int ret;
    uint32_t time_ms;

    ret = aw_timespec_get(&tv);
    if (ret < 0) {
        return 0;
    }
    time_ms = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;

    return time_ms;
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aw_mdelay(ms);
}

#if ((defined(MULTITHREAD_ENABLED)) || (defined AT_TCP_ENABLED))

#define _HAL_TASK_DEFAULT_PRIORITY  (6)
#define _HAL_TASK_DEFAULT_STACK     (2*1024)

// platform-dependant thread routine/entry function
static void *_HAL_thread_func_wrapper_(void *ptr)
{
    ThreadParams *params = (ThreadParams *)ptr;

    params->thread_func(params->user_arg);

    return NULL;
}

// platform-dependant thread create function
int HAL_ThreadCreate(ThreadParams *params)
{
    if (params == NULL)
        return QCLOUD_ERR_INVAL;

    aw_task_id_t id;
    id = aw_task_create("iot task",
                        _HAL_TASK_DEFAULT_PRIORITY,
                        _HAL_TASK_DEFAULT_STACK,
                        _HAL_thread_func_wrapper_,
                        (void *)params);
    if (id == NULL) {
        HAL_Printf("%s: pthread_create failed: %d\n", __FUNCTION__, ret);
        return QCLOUD_ERR_FAILURE;
    }

    params->thread_id = id;
    aw_task_startup(params->thread_id);

    return QCLOUD_RET_SUCCESS;
}

int HAL_ThreadDestroy(void *threadId)
{
    int ret;

    if (NULL == threadId) {
        return QCLOUD_ERR_FAILURE;
    }

    if (aw_task_terminate(params->thread_id) == AW_OK) {
        ret = QCLOUD_RET_SUCCESS;
    } else {
        ret = QCLOUD_ERR_FAILURE;
    }

    return ret;
}

#endif

#ifdef AT_TCP_ENABLED

void HAL_DelayMs(_IN_ uint32_t ms)
{
    aw_mdelay(ms);
}

typedef struct {
    AW_SEMC_DECL(sem);
} hal_sem_t ;

void *HAL_SemaphoreCreate(void)
{
    hal_sem_t *sem = (hal_sem_t *) aw_mem_alloc (sizeof(hal_sem_t));
    if (NULL == sem) {
        return NULL;
    }

    if (AW_SEMC_INIT(sem->sem, 0, AW_SEM_Q_PRIORITY) == NULL) {
        aw_mem_free(sem);
        return NULL;
    }

    return sem;
}

void HAL_SemaphoreDestroy(void *sem)
{
    AW_SEMC_TERMINATE(sem->sem);
    aw_mem_free(sem);
}

void HAL_SemaphorePost(void *sem)
{
    AW_SEMC_GIVE(sem->sem);
}

int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
    AW_SEMC_TAKE(sem->sem, timeout_ms);
}

#endif
