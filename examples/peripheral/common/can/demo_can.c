/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 高速CAN控制器演示例程
 * \note  本例程进行收发回显。
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      AW_COM_CONSOLE、
 *      相应平台的CAN、UART设备。
 *      若需要测试其他CAN设备，可修改例程中的CAN通道并使能aw_prj_params.h头文件中对应的CAN设备。
 *   2. 连接CAN的相关引脚到CAN收发器 RX=>RX TX=>TX，
 *      收发器再接到USBCAN-II设备上 H=>H L=>L, CAN波特率500K。
 *   3. 打开串口调试终端，波特率115200-8-N-1。
 *
 * - 实验现象：
 *   1. __CAN_TEST_MODE = __CAN_TEST_MSG_SEND
 *      则打开ZCANPRO上位机连接启动设备后，会不断的收取到报文。
 *   2. __CAN_TEST_MODE = __CAN_TEST_MSG_ECHO
 *      则打开ZCANPRO上位机连接启动设备后，发送报文会进行回显。
 *   3. 串口打印相关调试信息。
 *
 * \par 源代码
 * \snippet demo_can.c src_can
 *
 * \internal
 * \par Modification History
 * - 1.00 18-09-18  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_can 高速CAN控制器(收发回显)
 * \copydoc demo_can.c
 */

/** [src_can] */

#include "aworks.h"
#include "string.h"
#include "aw_can.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_spinlock.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define __CAN_CHN                  DE_CAN_CH    /* CAN通道 */
#define __CAN_RECV_MSG_TIMEOUT     10           /* CAN报文接收超时时间 */
#define __CAN_SEND_MSG_TIMEOUT     300          /* CAN报文发送超时时间 */
#define __CAN_MSG_CNT              128          /* CAN消息个数 */

#define __CAN_TASK_STACK_SIZE      4096         /* CAN报文接收任务堆栈大小 */
#define __CAN_TASK_PRIO            12           /* CAN报文接收任务优先级 */

/* 该宏为1时 接收到CAN帧后将打印接收到的数据 */
#define __CAN_TEST_MSG_PRINT       0
#define __CAN_TEST_MSG_ECHO        0            /* CAN测试消息回显 */
#define __CAN_TEST_MSG_SEND        1            /* CAN测试消息发送 */
#define __CAN_TEST_MODE            __CAN_TEST_MSG_ECHO /* CAN测试模式选择 */

#undef AW_INFOF
#define AW_INFOF(x) aw_kprintf("CAN%d : ", __CAN_CHN); aw_kprintf x

/* CAN总线状态 */
aw_local volatile aw_can_bus_status_t __g_can_bus_status = AW_CAN_BUS_STATUS_OK;

aw_local aw_can_baud_param_t __g_can_btr = AW_CFG_CAN_BTR_500K;

/******************************************************************************/
aw_local uint32_t __can_timestamp_get_cb (uint32_t timestamp);
aw_local aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

aw_local aw_can_app_callbacks_t __g_can_app_callback_fun = {
    __can_timestamp_get_cb,
    NULL,
    NULL,
    __can_err_sta_cb,
    NULL
};

/**
 * \brief 时间戳获取的回调函数
 *
 * \param[in] timestamp 当前报文的时间戳
 *
 * \return 用户修改后的时间戳
 */
aw_local uint32_t __can_timestamp_get_cb (uint32_t timestamp)
{
    return timestamp;
}

/**
 * \brief 错误状态的回调函数
 *
 * \param[in] chn 通道号
 * \param[in] err CAN 总线的错误类型
 * \param[in] status CAN 总线的状态
 *
 * \retval AW_TRUE  不保存消息到接收rngbuf
 * \retval AW_FALSE 保存消息到接收rngbuf
 */
aw_local aw_bool_t __can_err_sta_cb (
    int chn, aw_can_bus_err_t err, aw_can_bus_status_t status)
{
    aw_can_err_cnt_t  can_err_reg;

    aw_can_err_cnt_get(chn, &can_err_reg);
    if ((can_err_reg.tx_err_cnt > 127) || (can_err_reg.rx_err_cnt > 127)) {
        aw_can_err_cnt_clr(chn);
    }

    __g_can_bus_status = status;

    return AW_TRUE; /* 函数返回值决定错误消息是否存入rngbuf,AW_TRUE:不保存,AW_FALSE:保存 */
}

/**
 * \brief CAN初始化
 *
 * \param[in] 无
 *
 * \return AW_CAN_ERR_NONE正常启动，其他错误请参考错误代码
 */
aw_local aw_can_err_t __can_startup (void)
{
    aw_can_err_t ret;

    /** \brief 初始化CAN(通道0，正常模式，500K波特率) */
    ret = aw_can_init(__CAN_CHN,                   /** \brief CAN通道号 */
                      AW_CAN_WORK_MODE_NORMAL,     /** \brief 控制器工作模式 */
                      &__g_can_btr,                /** \brief 当前波特率 */
                      &__g_can_app_callback_fun);  /** \brief 用户回调函数 */

    AW_INFOF(("controller init...\n"));

    if (ret != AW_CAN_ERR_NONE) {
        goto exit;
    }

    /** \brief 启动CAN */
    ret = aw_can_start(__CAN_CHN);
    AW_INFOF(("controller start!\n"));

exit:
    return ret;
}

/**
 * \brief CAN任务入口
 *
 * \param[in] p_arg 任务参数
 *
 * \return 无
 */
aw_local void* __task_handle (void *p_arg)
{
    aw_can_err_t        ret;
    aw_spinlock_isr_t   lock;
    aw_can_bus_status_t bus_status;         /* CAN bus总线状态 */
    uint32_t            recv_fifo_size = 0; /* CAN FIFO大小 */
    uint32_t            recv_cnt       = 0; /* 一次接收的个数 */
    uint32_t            send_done_cnt  = 0; /* 发送完成个数 */
#if (__CAN_TEST_MSG_PRINT == 1)
    uint32_t            i              = 0;
    uint32_t            j              = 0;
#endif
    /* 是否是测试CAN消息发送*/
#if !(__CAN_TEST_MODE == __CAN_TEST_MSG_SEND)
    uint32_t            recv_done_cnt  = 0; /* 接收完成个数 */
#endif
    aw_can_std_msg_t    msg[__CAN_MSG_CNT]; /* CAN信息缓冲区 */

    /* 初始化CAN*/
    ret = __can_startup();

    if (ret != AW_CAN_ERR_NONE) {
        AW_INFOF(("controller startup failed:%d!\n", ret));
        goto exit;
    }

    aw_spinlock_isr_init(&lock, 0);

    /* 获取接收FIFO大小 */
    recv_fifo_size = aw_can_rcv_fifo_size(__CAN_CHN);

    AW_INFOF(("receive fifo size %d messages.\n", recv_fifo_size));

    /* 取接收FIFO和信息缓冲区最小值 */
    recv_cnt = min(recv_fifo_size, __CAN_MSG_CNT);

    AW_INFOF(("receive cnt %d messages.\n", recv_cnt));

#if __CAN_TEST_MODE == __CAN_TEST_MSG_SEND
    memset(msg, 0x00, sizeof(msg));
    msg[0].can_msg.length = 8;
#endif

    AW_FOREVER {

#if __CAN_TEST_MODE == __CAN_TEST_MSG_SEND

        msg[0].can_msg.id++;

        /* 发送标准CAN报文 */
        ret = aw_can_std_msgs_send(__CAN_CHN,
                                   msg,
                                   1,
                                   &send_done_cnt,
                                   __CAN_SEND_MSG_TIMEOUT);

        if (ret == AW_CAN_ERR_NONE && send_done_cnt == 1) {
            AW_INFOF(("send done! sent:%d\n", send_done_cnt));
        } else {
            AW_INFOF(("send error! sent:%d\n", send_done_cnt));
        }
#else
        /* 接收标准CAN报文 */
        ret = aw_can_std_msgs_rcv(__CAN_CHN,
                                  msg,
                                  recv_cnt,
                                  &recv_done_cnt,
                                  __CAN_RECV_MSG_TIMEOUT);

        if ((ret == AW_CAN_ERR_NONE) && (recv_done_cnt > 0)) {

            AW_INFOF(("recv cnt:%d\n", recv_done_cnt));
#if (__CAN_TEST_MSG_PRINT == 1)
            AW_INFOF(("recv data:\r\n"));
            for (i = 0; i < recv_done_cnt; i++) {
                for (j = 0; j < AW_CAN_HS_MAX_LENGTH; j++) {
                    aw_kprintf(" %02X", msg[i].msgbuff[j]);
                }
                aw_kprintf("\r\n");
            }
#endif

            /* 发送标准CAN报文 */
            ret = aw_can_std_msgs_send(__CAN_CHN,
                                       msg,
                                       recv_done_cnt,
                                       &send_done_cnt,
                                       __CAN_SEND_MSG_TIMEOUT);

            if (ret == AW_CAN_ERR_NONE && send_done_cnt == recv_done_cnt) {
                AW_INFOF(("send done! sent:%d\n", send_done_cnt));
            } else {
                AW_INFOF(("send error! sent:%d\n", send_done_cnt));
            }
        }
#endif

        aw_spinlock_isr_take(&lock);
        bus_status = __g_can_bus_status;
        __g_can_bus_status = AW_CAN_BUS_STATUS_OK;
        aw_spinlock_isr_give(&lock);

        if (bus_status == AW_CAN_BUS_STATUS_OFF) {

            /* 停止CAN */
            aw_can_stop(__CAN_CHN);
            AW_INFOF(("controller stop!\n"));

            /* 启动CAN */
            aw_can_start(__CAN_CHN);
            AW_INFOF(("controller start!\n"));
        }
    }

exit:
    AW_FOREVER {
        aw_mdelay(1000);
    }

    return 0;
}

/**
 * \brief 建立CAN任务
 *
 * \param[in] 无
 *
 * \return 无
 */
void demo_can_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Can demo",
                         __CAN_TASK_PRIO,
                         __CAN_TASK_STACK_SIZE,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Can demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_can] */

/* end of file */
