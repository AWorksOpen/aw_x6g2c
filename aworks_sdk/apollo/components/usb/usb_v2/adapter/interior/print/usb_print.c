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
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_koutput.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "usb_config.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

AW_MUTEX_DECL(_g_kprintf_mutex);

/* AWorks   ≈‰¥Ú”°∫Ø ˝*/
int usb_printf (const char *fmt, ...){
    int     len;
    va_list args;


    if ((NULL == gpfunc_kputc) || (NULL == gpfunc_kputs)) {
        return 0;
    }

    va_start(args, fmt);

    if ( !(AW_INT_CONTEXT() || AW_FAULT_CONTEXT() )) {
        AW_MUTEX_LOCK(_g_kprintf_mutex,AW_WAIT_FOREVER);
    }


    len = aw_vfprintf_do(gp_kout_file, gpfunc_kputc, gpfunc_kputs, fmt, args);

    if ( !(AW_INT_CONTEXT() || AW_FAULT_CONTEXT() )) {
        AW_MUTEX_UNLOCK(_g_kprintf_mutex);
    }
    va_end(args);

    return len;
}

