/*******************************************************************************
 *                                 AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
* Copyright (c) 2001-20217 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn
 * e-mail:      support@zlg.cn
 *******************************************************************************/

/**
 * \file
 * \brief GPRS例程（该例程演示GPRS设备发送短信和联网通信的功能）
 *
 * - 操作步骤:
 *   1. aw_prj_params.h 中使能：
 *      - 对应平台的USBH宏
 *      - 及对应平台串口宏
 *      - AW_COM_CONSOLE
 *      - 对应的GPRS设备宏(例如：AW_DEV_GPRS_ME909S)
 *   2. 将GPRS模块插入开发板的USB HOST0接口；
 *   3. 在电脑上打开 TCP&UDP 调试软件，建立并启动服务器。
 *
 * - 实验现象:
 *   1. 手机客户端会收到短信：Hello,i'm aworks。
 *   1. 串口打印： GPRS设备连接网络的调试信息；以及服务端发送的数据信息。
 *   2. TCP&UDP 调试软件收到GPRS设备发送的数据，
 *      串口打印TCP&UDP上位机发送的数据。
 *
 * \code
 */

/** [src_gprs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_gprs.h"
#include <string.h>
#include "aw_delay.h"
#include "aw_demo_config.h"
//#include "aw_ping.h"
#include "aw_mem.h"
#include "aw_gnss.h"
#include "host/awbl_usbh.h"

/* 使用TCP协议连接网络 */
#define TEST_GPRS_MODE                  "TCP"

/*访问的服务器IP，用户需根据具体情况修改*/
#define TEST_SERVER_IP                  "121.33.243.38"

/*访问的服务器端口，用户需根据具体情况修改*/
//#define TEST_SEEVER_PORT                33339
#define TEST_SEEVER_PORT                55599

/* 测试使用手机号，用户需修改, 有些要求前面加“+86”*/
#define TEST_PHONE_NUM                  "+8613538731537"
//#define TEST_PHONE_NUM                  "13538731537"

/* 测试短信*/
//#define TEST_SMS_EN
//#define TEST_SMS_CH
//#define TEST_SMS_CH_EN
#if (defined(TEST_SMS_EN) && defined(TEST_SMS_CH) && defined(TEST_SMS_CH_EN))
#error "TEST_SMS_EN ,TEST_SMS_CH and TEST_SMS_CH_EN cannot defined at the same time!"
#endif


uint8_t is_gprs_inject = 0;
uint8_t is_gprs_eject = 0;

#define GPRS_PID 0x0125
#define GPRS_VID 0x2C7C
//#define GPRS_VID 0x305a
//#define GPRS_PID 0x1415
//#define GPRS_VID 0x12D1
//#define GPRS_PID 0x15C1

/* USB设备热插拔回调*/
void __gprs_dev_info(void *arg, uint8_t type) {
    if (type == USBDEV_INJECT) {
        is_gprs_inject = 1;
        is_gprs_eject = 0;
        aw_kprintf("GPRS device insert\r\n");
    } else if (type == USBDEV_EJECT) {
        is_gprs_inject = 0;
        is_gprs_eject = 1;
        aw_kprintf("GPRS device remove\r\n");
    }
}

/*
 * \brief GPRS 网络通信例程入口
 * \return 无
 */
void demo_usb_gprs_entry(void) {
    aw_err_t ret = AW_ERROR;
    int gprs_id, i;
    int socket_hd;
#ifdef TEST_SMS_EN
    char *sms = "Hello,i'm aworks!";
#elif defined(TEST_SMS_CH)
    char *sms = "我是吴彦祖";
#elif defined(TEST_SMS_CH_EN)
    char *sms = "i am 蝙蝠侠";
#endif
    uint8_t p_buf[1000];
    uint8_t *buf_write;
    usbh_monitor_handle monitor_handle;

    /* 申请内存*/
    buf_write = (uint8_t *) aw_mem_alloc(64);
    if (buf_write == NULL) {
        return;
    }
    for (i = 0; i < 64; i++) {
        buf_write[i] = i % 255;
    }
    /* 注册4G模块热插拔*/
    monitor_handle = usbh_dev_monitor_register(GPRS_VID, GPRS_PID, USBH_DEV_UNKNOWN,
                    USBDEV_EJECT | USBDEV_INJECT, __gprs_dev_info, NULL);
    /* 先获取存在的GPRS设备的ID*/
    gprs_id = aw_gprs_any_get(5);
    if (gprs_id < 0) {
        /* 等待GPRS设备插入*/
        aw_kprintf("waiting GPRS device ...\r\n");
        while (is_gprs_inject == 0) {
            aw_mdelay(100);
        }
        /* 获取存在的GPRS设备的ID*/
        gprs_id = aw_gprs_any_get(5);
        if (gprs_id < 0) {
            aw_kprintf("GPRS get dev id failed\r\n");
            return;
        }
    }

    ret = aw_gprs_power_on(gprs_id);
    if (ret != AW_OK) {
        aw_kprintf("aw_gprs_power_on error:%d\r\n", ret);
        return;
    }
#if defined(TEST_SMS_EN) || defined(TEST_SMS_CH) || defined(TEST_SMS_CH_EN)
    /*发送短信*/
    ret = aw_gprs_sms_send(gprs_id, (uint8_t *)sms, strlen(sms), TEST_PHONE_NUM);
    if (ret != AW_OK) {
        aw_kprintf("aw_gprs_sms_send error:%d\r\n", ret);
    }
    /* 接收短信*/
    ret = aw_gprs_sms_recv(gprs_id, NULL, 20000);
    if (ret != AW_OK) {
        aw_kprintf("aw_gprs_sms_recv error:%d\r\n", ret);
    }
#endif

    /* 建立GPRS连接 */
    socket_hd = aw_gprs_start(gprs_id, TEST_GPRS_MODE, TEST_SERVER_IP,
            TEST_SEEVER_PORT, 3000);
    if (socket_hd < 0) {
        aw_kprintf("aw_gprs_start error:%d\r\n", socket_hd);
        goto exit;
    }
    while (1) {
        /*清空缓冲区*/
        memset(p_buf, '\0', sizeof(p_buf));
        /*接收服务器端发送的数据，非阻塞*/
        ret = aw_gprs_recv(gprs_id, socket_hd, p_buf, sizeof(p_buf));
        if (ret > 0) {
            aw_gprs_send(gprs_id, socket_hd, p_buf, ret, 1000);
        }
        /* 检查设备是否弹出*/
        if (is_gprs_eject == 1) {
            goto exit;
        }
        aw_mdelay(100);
    }

    exit:
    /*关闭gprs连接*/
    aw_gprs_close(gprs_id, socket_hd);
    /*关闭gprs设备*/
    aw_gprs_power_off(gprs_id);
    /* 注销设备热插拔回调函数*/
    usbh_dev_monitor_unregister(monitor_handle);

    return;
}

/** [src_gprs] */

/* end of file */

