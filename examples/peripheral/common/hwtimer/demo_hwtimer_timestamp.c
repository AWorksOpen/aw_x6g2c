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
 * \brief hwtimer ���Գ���
 *
 * hwtimer ͨ�ö�ʱ�����ṩ���¹���:
 *
 * - �������裺
 *   1. ��aw_prj_params.h��ʹ��
 *      - AW_DEV_xxx_TIMESTAMP
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��Ӧ��Ϣ��
 *
 * \par Դ����
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
 * \brief hwtimer timestamp ���Ժ���
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
