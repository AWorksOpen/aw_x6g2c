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
#include "usb_errno.h"
#include "usb_common.h"

#define USB_WAIT_FOREVER  -1
#define USB_NO_WAIT        0


typedef void*  usb_task_handle_t;
typedef void*  usb_sem_handle_t;
typedef void*  usb_mutex_handle_t;


/* �Ĵ�������*/
void usb_writel (uint32_t val, volatile void *addr);
uint32_t usb_readl (volatile void *addr);
/* С�˼Ĵ�����д*/
#define USB_LE_WRITE32(val, addr)    usb_writel(USB_CPU_TO_LE32(val), (volatile void *)addr)
#define USB_LE_READ32(addr)          USB_CPU_TO_LE32(usb_readl((volatile void *)addr))

/* �������*/
usb_task_handle_t usb_task_create (const char  *name,
                                      int          prio,
                                      size_t       stk_s,
                                      void       (*pfnc) (void *p_arg),
                                      void        *p_arg);
usb_err_t usb_task_delete (usb_task_handle_t tsk);
usb_err_t usb_task_startup (usb_task_handle_t tsk);
usb_err_t usb_task_suspend(usb_task_handle_t tsk);
usb_err_t usb_task_resume(usb_task_handle_t tsk);

/* �ź�������*/
usb_sem_handle_t usb_sem_create (void);
void usb_sem_delete (usb_sem_handle_t sem);
usb_err_t usb_sem_take (usb_sem_handle_t sem,
                           int              timeout);
usb_err_t usb_sem_give (usb_sem_handle_t sem);

/* ����������*/
usb_mutex_handle_t usb_mutex_create (void);
void usb_mutex_delete (usb_mutex_handle_t mutex);
usb_err_t usb_mutex_lock (usb_mutex_handle_t mutex,
                             int                timeout);
usb_err_t usb_mutex_unlock (usb_mutex_handle_t mutex);
