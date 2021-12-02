/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief IMX6UL timestamp 驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 19-09-03  zaf, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_int.h"
#include "aw_timestamp.h"
#include "awbl_timestamp.h"
#include "driver/timestamp/awbl_imx6ul_timestamp.h"

extern uint32_t read_timestamp(void);
extern uint32_t read_timestamp_ovfl_bit(void);

extern void timestamp_enable(void);
extern void clr_timestamp_ovfl_bit(void);
extern void timestamp_ovfl_int_enable(void);

/**
 * \brief 宏定义
 */
#define __IMX6UL_TIMESTAMP_DEV_DECL(p_this, p_dev)              \
          struct awbl_imx6ul_timestamp_dev *p_this =            \
         (struct awbl_imx6ul_timestamp_dev *)p_dev

#define __IMX6UL_TIMESTAMP_DEVINFO_DECL(p_devinfo, p_dev)       \
          struct awbl_imx6ul_timestamp_devinfo *p_devinfo =     \
         (struct awbl_imx6ul_timestamp_devinfo *)AWBL_DEVINFO_GET(p_dev)

static volatile uint32_t __g_imx6ul_timestamp_int_count = 0;

static struct awbl_timestamp __g_imx6ul_timestamp = {NULL, 0};

volatile uint8_t is_timestamp_enable = 0;


/**
 * \brief systick 回调函数
 */
aw_local void cortexa7_systick_timestamp_handler (void)
{
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);

    /* clear the overflow bit */
    clr_timestamp_ovfl_bit();

    __g_imx6ul_timestamp_int_count++;

    RTK_EXIT_CRITICAL(old);
}

/**
 * \brief  imx6ul timestamp 初始化
 */
aw_local void __cortexa7_timestamp_init (void)
{
    /* timestamp enabled, user mode access */
    timestamp_enable();

    /* timestamp overflow interrupt enable */
    timestamp_ovfl_int_enable();
}

/**
 * \brief 获取timestamp 计数值
 */
aw_timestamp_t __cortexa7_systick_timestamp_get (struct awbl_timestamp *p_awbl_timestamp)
{
    uint32_t        t1, t2, ctrl;
    uint64_t        t;
    uint64_t        temp_interrupt_count;

    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);

    /* read pmccntr count value */
    t1 = read_timestamp();

    /* read overflow bit */
    ctrl = read_timestamp_ovfl_bit();

    /* read pmccntr count value */
    t2 = read_timestamp();

    temp_interrupt_count = __g_imx6ul_timestamp_int_count;

    if (ctrl & (1 << 31)) {
        temp_interrupt_count ++;
    } else {
        if (t1 > t2) {
            temp_interrupt_count ++;
        }
    }

    RTK_EXIT_CRITICAL(old);

    t = temp_interrupt_count;
    t *= 0x100000000;
    t += t2;

    return t;
}

/**
 * \brief timestamp  第一阶段初始化
 */
aw_local void __imx6ul_timestamp_inst_init (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief timestamp  第二阶段初始化
 */
aw_local void __imx6ul_timestamp_inst_init2 (awbl_dev_t *p_dev)
{
    __IMX6UL_TIMESTAMP_DEV_DECL(p_this, p_dev);
    __IMX6UL_TIMESTAMP_DEVINFO_DECL(p_devinfo, p_this);

    __cortexa7_timestamp_init();

    /* timestamp 驱动被加载了 */
    is_timestamp_enable = 1;

    p_this->freq = aw_clk_rate_get(p_devinfo->clk_id);

    __g_imx6ul_timestamp.fn_timestamp_get = __cortexa7_systick_timestamp_get;
    __g_imx6ul_timestamp.freq = p_this->freq;

    /* connect the interrupt */
    aw_int_connect(p_devinfo->inum, (void *)cortexa7_systick_timestamp_handler, NULL);
    aw_int_enable(p_devinfo->inum);

    awbl_timestamp_dev_register(&__g_imx6ul_timestamp);

    return;
}

/**
 * \brief timestamp  第三阶段初始化
 */
aw_local void __imx6ul_timestamp_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief timestamp drvfuncs
 */
aw_local aw_const struct awbl_drvfuncs __g_imx6ul_timestamp_drvfuncs = {
    __imx6ul_timestamp_inst_init,
    __imx6ul_timestamp_inst_init2,
    __imx6ul_timestamp_inst_connect
};

/**
 * \brief drvinfo for driver register
 */
aw_local aw_const awbl_plb_drvinfo_t __g_drvinfo_imx6ul_timestamp = {
    {
        AWBL_VER_1,                      /* awb_ver */
        AWBL_BUSID_PLB,                  /* bus_id */
        AWBL_IMX6UL_TIMESTAMP_NAME,      /* p_drvname */
        &__g_imx6ul_timestamp_drvfuncs,  /* p_busfuncs */
        NULL,                            /* p_methods */
        NULL                             /* pfunc_drv_probe */
    }
};

/**
 * \brief timestamp 驱动注册
 */
void awbl_imx6ul_timestamp_drv_register (void)
{
    awbl_drv_register((const struct awbl_drvinfo *)&__g_drvinfo_imx6ul_timestamp);
}

