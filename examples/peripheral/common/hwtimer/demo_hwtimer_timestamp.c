/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief hwtimer 测试程序
 *
 * hwtimer 通用定时器，提供如下功能:
 *
 * - 操作步骤：
 *   1. 在aw_prj_params.h中使能
 *      - AW_DEV_xxx_TIMESTAMP
 *
 * - 实验现象：
 *   1. 串口打印相应信息。
 *
 * \par 源代码
 * \snippet demo_hwtimer_timestamp.c src_hwtimer_timestamp
 *
 * \internal
 * \par modification history:
 * - 1.00 2019-05-30  may, first implementation
 * \endinternal
 */
/**
 * \addtogroup demo_if_hwtimer_timestamp
 * \copydoc demo_hwtimer_timestamp.c
 */

/** [src_hwtimer_timestamp] */
#include "aworks.h"
#include "aw_hwtimer.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_timestamp.h"

/**
 * \brief hwtimer timestamp 测试函数
 */
aw_local void* __task_handle (void *arg)
{
    unsigned int stamps = 0;
    unsigned int i;

    stamps = aw_timestamp_get();

    /* do something */
    aw_mdelay(100);

    stamps = aw_timestamp_get() - stamps;
    aw_kprintf("stamps used = %d,\n \
                \rtime        = %d us\n",
                stamps,
                aw_timestamps_to_us(stamps));

    return 0;
}

/******************************************************************************/
void demo_hwtimer_timestamp_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Hwtimer timestamp demo",
                         12,
                         1024,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Hwtimer timestamp demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);

}

/** [src_hwtimer_timestamp] */

/*end of file */
