/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief USB device cdc 虚拟串口演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USB宏
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 将开发板USB DEVICE接口与电脑使用USB线连接
 *
 * - 实验现象：
 *   1. 电脑出现一个虚拟串口设备，在设备管理其端口可以看到对应的COM口
 *   2. 打开串口软件，这里等待会比较久
 *   3. 往串口软件键入数据值，会收到相应的数据
 *
 */


#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_assert.h"
#include "device/usbd.h"
#include "device/usbd_obj.h"
#include "string.h"
#include "device/class/cdc/serial/usbd_cdc_virtual_serial.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

static struct usbd_cdc_vs    __g_usbd_cdc;

/* 定义全局usb设备实例和usb设备信息结构体 */
static struct usbd_obj            __g_usbd_obj;
const static struct usbd_obj_info __g_usbd_info = {
    0xEF,
    0x02,
    0x01,
    64,
    0x0100,
    0x6047,
    0x5052,
    "ZLG-VS",
    "AWorks-vs",
    "20200828"
};

/* USB任务入口
 * p_arg : 任务参数*/
static void task_entry (void *p_arg)
{
    aw_err_t  ret;
    char *buff = "Hello,i am AWorks\r\n";
    char *read_buf = NULL;
    unsigned int len;

    /* 申请读缓存*/
    read_buf = usb_mem_alloc(1024);
    if(read_buf == NULL){
        AW_ERRF(("read_buf alloc failed\r\n"));
        return;
    }

    /* 初始化一个USB设备*/
    ret = usbd_obj_init(&__g_usbd_obj,
                        &__g_usbd_info,
                        NULL,
                        DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\r\n", ret));
        return;
    }
    /* 创建一个虚拟串口*/
    ret = usbd_cdc_vs_create (&__g_usbd_obj,
                              &__g_usbd_cdc,
                              0,
                              "cdc0",
                              1024);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc init failed: %d\r\n", ret));
        return;
    }

    /* 设置传输模式*/
    ret = usbd_cdc_vs_set_rxmode(&__g_usbd_cdc, USBD_CDC_VS_TRANFER_SYNC);
    //ret = usbd_cdc_vs_set_rxmode(&__g_usbd_cdc, USBD_CDC_VS_TRANFER_ASYNC);
    if(ret != AW_OK){
        AW_ERRF(("usbd_cdc set rx mode failed: %d\r\n", ret));
        return;
    }

    /* 启动USB设备*/
    usbd_obj_start(&__g_usbd_obj);

    aw_mdelay(10);
    usbd_cdc_vs_write_sync(&__g_usbd_cdc, buff, 19, 1000);

    for (;;) {
        while (!usbd_obj_fun_valid(&__g_usbd_cdc.dfun)) {
            AW_INFOF(("USBD_CDC: wait for connect.\n"));
            usb_sem_take(__g_usbd_cdc.sync_semb, USB_WAIT_FOREVER);
        }
        if(usbd_cdc_vs_rx_is_async(&__g_usbd_cdc) == AW_TRUE){
            memset(read_buf, 0x00, 1024);
            ret = usbd_cdc_vs_read_async(&__g_usbd_cdc, read_buf, 1024, &len);
            if(len > 0){
                usbd_cdc_vs_write_sync(&__g_usbd_cdc, read_buf, len, 5000);
            }
            aw_mdelay(10);
        }
        else{
            memset(read_buf, 0x00, 1024);
            len = usbd_cdc_vs_read_sync(&__g_usbd_cdc, read_buf, 1024, USB_WAIT_FOREVER);
            if(len > 0){
                usbd_cdc_vs_write_sync(&__g_usbd_cdc, read_buf, len, 5000);
            }
        }
    }
}

/* USB cdc 串口 demo*/
void demo_std_usbd_cdc_serial_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,        /* 任务实体 */
                "task",       /* 任务名字 */
                 6,           /* 任务优先级 */
                 2048,        /* 任务堆栈大小 */
                 task_entry,  /* 任务入口函数 */
                (void*)0);    /* 任务入口参数 */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_cdc] */

/* end of file */


