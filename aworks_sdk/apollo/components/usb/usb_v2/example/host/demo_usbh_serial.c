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
#include "Aworks.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "host/class/cdc/serial/usbh_cdc_serial.h"
#include "host/core/usbh.h"
#include <string.h>

static uint32_t              __g_userial_dev_num  = 0;
static struct usbh_serial   *__g_userial          = NULL;
static aw_bool_t             __g_mode;

/** 查询USB内存堆的使用情况shell命令*/
static int __userial_mem_sta (int argc, char *argv[], struct aw_shell_user *p_user){
    usb_err_t              ret;
    struct usbh_mem_record mem_record;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_mem\r\n\r\n");
            aw_kprintf("Description:Check USB Serial mem status\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"userial_mem --help\" for more information.\r\n");
        }
        return 0;
    }


    ret = userial_lib_mem_record_get(&mem_record);
    if(ret == USB_OK){
        aw_kprintf("mem record:%s\r\n", mem_record.drv_name);
        aw_kprintf("  mem   alloc : %d\r\n", mem_record.mem_alloc_num);
        aw_kprintf("  mem   free  : %d\r\n", mem_record.mem_free_num);
        aw_kprintf("  mem   total : %d\r\n", mem_record.mem_total_num);
        aw_kprintf("  mutex alloc : %d\r\n", mem_record.mutex_alloc_num);
        aw_kprintf("  mutex free  : %d\r\n", mem_record.mutex_free_num);
        aw_kprintf("  sem   alloc : %d\r\n", mem_record.sem_alloc_num);
        aw_kprintf("  sem   free  : %d\r\n", mem_record.sem_free_num);
    }
    else{
        aw_kprintf("userial mem record get failed\r\n");
    }
    return 0;
}

/** 检测当前存在的USB转串口设备*/
static int __userial_dev_detect (int argc, char *argv[], struct aw_shell_user *p_user){
    int                 userial_num  = 0;
    int                 port_num     = 0;
    int                 i;
    struct usbh_serial *userial_dev  = NULL;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_detect\r\n\r\n");
            aw_kprintf("Description:Detect all USB Serial devices in system\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"userial_detect --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_userial_dev_num == 0){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    /* 获取USB转串口设备数量*/
    userial_num = usbh_serial_dev_num_get();
    if(userial_num <= 0){
        return 0;
    }
    for(i = 0;i < userial_num; i++){
        userial_dev = usbh_serial_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID,
                USB_SERIAL_DEFAULT_INTF, i + 1);
        if(userial_dev != NULL){
            port_num = usbh_serial_dev_nport_get(userial_dev);
            aw_kprintf("USB Serial device %d(PID0x%x,VID0x%x),support %d port\r\n", i + 1,
                    USBH_DEV_PID_GET(userial_dev->p_fun),
                    USBH_DEV_VID_GET(userial_dev->p_fun), port_num);;

            usbh_serial_dev_close(userial_dev);
        }
        else{
            aw_kprintf("Do not find USB Serial device %d\r\n", i + 1);
        }

    }
    return 0;
}

/** 异步接收回调函数*/
static void rx_async_cb(void *buf, int len){
    aw_kprintf("data len:%d\r\n", len);
    aw_kprintf("data:%s\r\n", buf);
    aw_mem_free(buf);
}

/** 打开USB转串口设备*/
static int __userial_dev_open (int argc, char *argv[], struct aw_shell_user *p_user){
    int                 inedx     = 0;
    int                 intf_num  = 0;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_open\r\n\r\n");
            aw_kprintf("Description:Open a USB Serial device\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.device index  :device index\r\n");
            aw_kprintf("    2.interface num :interface number,-1 is the first interface\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"userial_open --help\" for more information.\r\n");
        return 0;
    }

    /* 获取设备索引*/
    inedx = atoi(argv[0]);
    if(inedx == 0){
        aw_kprintf("device index invalid\r\n");
        return 0;
    }
    /* 获取接口号*/
    intf_num = atoi(argv[1]);

    __g_userial = usbh_serial_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, intf_num, inedx);
    if(__g_userial == NULL){
        aw_kprintf("USB Serial device open failed\r\n");
        return 0;
    }
    usbh_serial_dev_rxcb_set(__g_userial, rx_async_cb);

    aw_kprintf("USB Serial device open success\r\n");
    return 0;
}

/** 关闭USB转串口设备*/
static int __userial_dev_close (int argc, char *argv[], struct aw_shell_user *p_user){
    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_close\r\n\r\n");
            aw_kprintf("Description:Close a USB Serial device\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"userial_close --help\" for more information.\r\n");
        }
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    usbh_serial_dev_close(__g_userial);

    aw_kprintf("USB Serial device close success\r\n");
    return 0;
}

/** 获取USB转串口设备配置*/
static int __userial_dev_config_get (int argc, char *argv[], struct aw_shell_user *p_user){
    struct usb_serial_dcb *p_userial_dcb = NULL;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_config_get\r\n\r\n");
            aw_kprintf("Description:Get USB Serial device configure\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"userial_config_get --help\" for more information.\r\n");
        }
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    p_userial_dcb = usbh_serial_dev_dcb_get(__g_userial);
    if(p_userial_dcb == NULL){
        aw_kprintf("USB Serial Data Control Block get failed\r\n");
        return 0;
    }
    aw_kprintf("%s data control block:\r\n"
            "    baudrate  : %d\r\n"
            "    data bit  : %d\r\n"
            "    stop_bits : %s\r\n"
            "    parity    : %s\r\n"
            ,__g_userial->name,
            p_userial_dcb->baud_rate,
            p_userial_dcb->byte_size,
            (p_userial_dcb->stop_bits)?((p_userial_dcb->parity > 1)?
                    ((p_userial_dcb->parity > 2)?"invalid":"2"):"1.5"):"1",
            ((p_userial_dcb->f_parity)?((p_userial_dcb->parity)?"odd":"even"):"none"));

    return 0;
}

/** 设置USB转串口设备配置*/
static int __userial_dev_config_set (int argc, char *argv[], struct aw_shell_user *p_user){
    struct usb_serial_dcb userial_dcb;
    uint32_t              baudrate;
    uint8_t               databits;
    uint8_t               stopbits;
    aw_err_t              ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_config_set\r\n\r\n");
            aw_kprintf("Description:Set USB Serial device configure\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.baudrate  :baudrate\r\n");
            aw_kprintf("    2.data bit  :data bits\r\n");
            aw_kprintf("    3.stop bits :stop bits\r\n");
            aw_kprintf("    4.parity    :parity(none, odd, or even)\r\n");
            return 0;
        }
    }

    if(argc != 4){
        aw_kprintf("Try \"userial_open --help\" for more information.\r\n");
        return 0;
    }

    memset(&userial_dcb, 0, sizeof(struct usb_serial_dcb));
    baudrate = atoi(argv[0]);
    databits = atoi(argv[1]);
    stopbits = atoi(argv[2]);

    userial_dcb.baud_rate = baudrate;
    userial_dcb.byte_size = databits;

    if(stopbits == 1){
        userial_dcb.stop_bits = USB_SERIAL_ONESTOPBIT;
    }
    else if(stopbits == 2){
        userial_dcb.stop_bits = USB_SERIAL_TWOSTOPBITS;
    }
    else{
        aw_kprintf("invalid stop bits\r\n");
        return 0;
    }

    if(strcmp(argv[3], "none") == 0){
        userial_dcb.f_parity = AW_FALSE;
    }
    else if(strcmp(argv[3], "odd") == 0){
        userial_dcb.f_parity = AW_TRUE;
        userial_dcb.parity   = USB_SERIAL_ODDPARITY;
    }
    else if(strcmp(argv[3], "even") == 0){
        userial_dcb.f_parity = AW_TRUE;
        userial_dcb.parity   = USB_SERIAL_EVENPARITY;
    }
    else{
        aw_kprintf("parity invalid\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    ret = usbh_serial_dev_dcb_set(__g_userial, &userial_dcb);
    if(ret != AW_OK){
        aw_kprintf("Set USB Serial device configure failed\r\n");
        return 0;
    }

    aw_kprintf("Set USB Serial device configure success\r\n");
    return 0;
}

/** 获取USB转串口设备超时时间 */
static int __userial_dev_timeout_get (int argc, char *argv[], struct aw_shell_user *p_user){
    uint32_t timeout = 0;
    aw_err_t ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_timeout_get\r\n\r\n");
            aw_kprintf("Description:Get USB Serial device timeout\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.pipe direction  :the data pipe direction(in/out)\r\n");
            return 0;
        }
    }

    if(argc != 1){
        aw_kprintf("Try \"userial_timeout_get --help\" for more information.\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    if(strcmp(argv[0], "in") == 0){
        ret = usbh_serial_dev_timeout_get(__g_userial, USB_SERIAL_PIPE_IN, &timeout);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe in timeout is %d\r\n", timeout);
            return 0;
        }
    }
    else if(strcmp(argv[0], "out") == 0){
        ret = usbh_serial_dev_timeout_get(__g_userial, USB_SERIAL_PIPE_OUT, &timeout);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe out timeout is %d\r\n", timeout);
            return 0;
        }
    }
    else{
        aw_kprintf("Try \"userial_timeout_get --help\" for more information.\r\n");
        return 0;
    }

    aw_kprintf("Data Pipe %s get timeout failed\r\n", argv[0]);
    return 0;
}

/** 设置USB转串口设备超时时间 */
static int __userial_dev_timeout_set (int argc, char *argv[], struct aw_shell_user *p_user){
    uint32_t timeout = 0;
    aw_err_t ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_timeout_set\r\n\r\n");
            aw_kprintf("Description:Set USB Serial device timeout\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.pipe direction  :the data pipe direction(in/out/both)\r\n");
            aw_kprintf("    2.timeout         :timeout\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"userial_timeout_set --help\" for more information.\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    timeout = atoi(argv[1]);


    if(strcmp(argv[0], "in") == 0){
        ret = usbh_serial_dev_timeout_set(__g_userial, USB_SERIAL_PIPE_IN, timeout);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe in set timeout %d success\r\n", timeout);
            return 0;
        }
    }
    else if(strcmp(argv[0], "out") == 0){
        ret = usbh_serial_dev_timeout_set(__g_userial, USB_SERIAL_PIPE_OUT, timeout);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe out set timeout %d success\r\n", timeout);
            return 0;
        }
    }
    else if(strcmp(argv[0], "both") == 0){
        ret = usbh_serial_dev_timeout_set(__g_userial, USB_SERIAL_PIPE_BOTH, timeout);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe set timeout %d success\r\n", timeout);
            return 0;
        }
    }
    else{
        aw_kprintf("Try \"userial_timeout_set --help\" for more information.\r\n");
        return 0;
    }

    aw_kprintf("Data Pipe %s set timeout failed\r\n", argv[0]);
    return 0;

}

/* 获取USB转串口设备单次传输大小 */
static int __userial_dev_tranfersize_get (int argc, char *argv[], struct aw_shell_user *p_user){
    uint32_t tranfersize = 0;
    aw_err_t ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_tranfersize_get\r\n\r\n");
            aw_kprintf("Description:Get USB Serial device tranfer size\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.pipe direction  :the data pipe direction(in/out)\r\n");
            return 0;
        }
    }

    if(argc != 1){
        aw_kprintf("Try \"userial_tranfersize_get --help\" for more information.\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    if(strcmp(argv[0], "in") == 0){
        ret = usbh_serial_dev_tranfersize_get(__g_userial, USB_SERIAL_PIPE_IN, &tranfersize);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe in tranfer size is %d\r\n", tranfersize);
            return 0;
        }
    }
    else if(strcmp(argv[0], "out") == 0){
        ret = usbh_serial_dev_tranfersize_get(__g_userial, USB_SERIAL_PIPE_OUT, &tranfersize);
        if(ret == AW_OK){
            aw_kprintf("Data Pipe out tranfer size is %d\r\n", tranfersize);
            return 0;
        }
    }
    else{
        aw_kprintf("Try \"userial_tranfersize_get --help\" for more information.\r\n");
        return 0;
    }

    aw_kprintf("Data Pipe %s get tranfer size failed\r\n", argv[0]);
    return 0;
}

/* 设置USB转串口设备单次传输大小 */
static int __userial_dev_tranfersize_set (int argc, char *argv[], struct aw_shell_user *p_user){
    uint32_t tranfersize = 0;
    aw_err_t ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_tranfersize_set\r\n\r\n");
            aw_kprintf("Description:Set USB Serial device tranfer size\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.pipe direction  :the data pipe direction(in/out)\r\n");
            aw_kprintf("    2.tranfer size    :tranfer size\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"userial_tranfersize_set --help\" for more information.\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    tranfersize = atoi(argv[1]);


    if(strcmp(argv[0], "in") == 0){
        ret = usbh_serial_dev_tranfersize_set(__g_userial, USB_SERIAL_PIPE_IN, tranfersize);
        if(ret == AW_OK){
            usbh_serial_dev_tranfersize_get(__g_userial, USB_SERIAL_PIPE_IN, &tranfersize);
            aw_kprintf("Data Pipe in set tranfer size %d success\r\n", tranfersize);
            return 0;
        }
    }
    else if(strcmp(argv[0], "out") == 0){
        ret = usbh_serial_dev_tranfersize_set(__g_userial, USB_SERIAL_PIPE_OUT, tranfersize);
        if(ret == AW_OK){
            usbh_serial_dev_tranfersize_get(__g_userial, USB_SERIAL_PIPE_OUT, &tranfersize);
            aw_kprintf("Data Pipe out set tranfer size %d success\r\n", tranfersize);
            return 0;
        }
    }
    else if(strcmp(argv[0], "both") == 0){
        ret = usbh_serial_dev_tranfersize_set(__g_userial, USB_SERIAL_PIPE_BOTH, tranfersize);
        if(ret == AW_OK){
            usbh_serial_dev_tranfersize_get(__g_userial, USB_SERIAL_PIPE_IN, &tranfersize);
            aw_kprintf("Data Pipe set tranfer size %d success\r\n", tranfersize);
            return 0;
        }
    }
    else{
        aw_kprintf("Try \"userial_tranfersize_set --help\" for more information.\r\n");
        return 0;
    }

    aw_kprintf("Data Pipe %s set tranfersize failed\r\n", argv[0]);
    return 0;
}

/** USB转串口设备发送数据 */
static int __userial_dev_send (int argc, char *argv[], struct aw_shell_user *p_user){
    int len;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_send\r\n\r\n");
            aw_kprintf("Description:USB Serial device send data\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.data  :the data want to send\r\n");
            return 0;
        }
    }

    if(argc != 1){
        aw_kprintf("Try \"userial_send --help\" for more information.\r\n");
        return 0;
    }

    if(__g_mode == 1){
        aw_kprintf("USB Serial device is in auto read/write mode\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    len = usbh_serial_dev_send(__g_userial, argv[0], strlen(argv[0]));
    if(len != strlen(argv[0])){
        aw_kprintf("USB Serial device send data failed\r\n");
        return 0;
    }
    aw_kprintf("USB Serial device send data success\r\n");
    return 0;
}

/** USB转串口设备读数据 */
static int __userial_dev_read (int argc, char *argv[], struct aw_shell_user *p_user){
    int      len;
    aw_err_t ret;
    uint8_t *buf      = NULL;
    uint32_t buf_size = 0;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_read\r\n\r\n");
            aw_kprintf("Description:USB Serial device read data\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.buffer size  :receive data buffer size\r\n");
            aw_kprintf("    2.receive mode :receive mode(sync/async)\r\n");
            return 0;
        }
    }

    if(argc != 2){
        aw_kprintf("Try \"userial_read --help\" for more information.\r\n");
        return 0;
    }

    if(__g_mode == 1){
        aw_kprintf("USB Serial device is in auto read/write mode\r\n");
        return 0;
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }

    buf_size = atoi(argv[0]);
    buf = aw_mem_alloc(buf_size);
    if(buf == NULL){
        aw_kprintf("buffer alloc failed\r\n");
        return 0;
    }
    memset(buf, 0, buf_size);

    if(strcmp(argv[1], "sync") == 0){
        len = usbh_serial_dev_recieve_sync(__g_userial, buf, buf_size);
        if(len > 0){
            aw_kprintf("data len:%d\r\n", strlen((char *)buf));
            aw_kprintf("data:%s\r\n", buf);
            aw_mem_free(buf);
            return 0;
        }
    }
    else if(strcmp(argv[1], "async") == 0){
        ret = usbh_serial_dev_recieve_async(__g_userial, buf, buf_size);
        if(ret == AW_OK){
            aw_kprintf("USB Serial device submit trp success\r\n");
            return 0;
        }
    }

    aw_kprintf("USB Serial device read data failed\r\n");

    aw_mem_free(buf);
    return 0;
}

/* 停止USB转串口设备读数据 */
static int __userial_dev_stop (int argc, char *argv[], struct aw_shell_user *p_user){
    aw_err_t ret;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_stop\r\n\r\n");
            aw_kprintf("Description:Stop USB Serial device read data\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            return 0;
        }
    }

    if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
        aw_kprintf("Do not have USB Serial device\r\n");
        return 0;
    }
    if(__g_mode == 1){
        aw_kprintf("USB Serial device is in auto read/write mode\r\n");
        return 0;
    }

    ret = usbh_serial_dev_stop(__g_userial);
    if(ret == USB_OK){
        aw_kprintf("USB Serial device stop\r\n");
        return 0;
    }

    aw_kprintf("USB Serial device stop failed\r\n");
    return 0;
}

/** 设置USB转串口设备自动读数据 */
static int __userial_dev_wr_auto (int argc, char *argv[], struct aw_shell_user *p_user){

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:userial_wr_auto\r\n\r\n");
            aw_kprintf("Description:enable/disable USB Serial device auto read/write data\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.status  :on/off\r\n");
            return 0;
        }
    }

    if(argc != 1){
        aw_kprintf("Try \"userial_wr_auto --help\" for more information.\r\n");
        return 0;
    }

    if(strcmp(argv[0], "on") == 0){
        __g_mode = 1;
        aw_kprintf("USB Serial device is in auto read/write mode\r\n");
    }
    else if(strcmp(argv[0], "off") == 0){
        __g_mode = 0;
        aw_kprintf("USB Serial device is in normal read/write mode\r\n");
    }
    else{
        aw_kprintf("status invalid\r\n");
    }
    return 0;
}

/* 命令集 */
aw_local const struct aw_shell_cmd __g_userial_cmdset_cmds[] = {
        /* 检查USB协议栈内存状态*/
        {__userial_mem_sta,             "userial_mem",             "check USB Serial mem status"},
        /* 遍历当前的USB转串口设备 */
        {__userial_dev_detect,          "userial_detect",          "Detect USB Serial device"},
        /* 打开USB转串口设备 */
        {__userial_dev_open,            "userial_open",            "Open USB Serial device"},
        /* 关闭USB转串口设备 */
        {__userial_dev_close,           "userial_close",           "Close USB Serial device"},
        /* 获取USB转串口设备配置 */
        {__userial_dev_config_get,      "userial_config_get",      "Get USB Serial device configure"},
        /* 设置USB转串口设备配置 */
        {__userial_dev_config_set,      "userial_config_set",      "Set USB Serial device configure"},
        /* 获取USB转串口设备超时时间 */
        {__userial_dev_timeout_get,     "userial_timeout_get",     "Get USB Serial device timeout"},
        /* 设置USB转串口设备超时时间 */
        {__userial_dev_timeout_set,     "userial_timeout_set",     "Set USB Serial device timeout"},
        /* 获取USB转串口设备单次传输大小 */
        {__userial_dev_tranfersize_get, "userial_tranfersize_get", "Get USB Serial device tranfer size"},
        /* 设置USB转串口设备单次传输大小  */
        {__userial_dev_tranfersize_set, "userial_tranfersize_set", "Set USB Serial device tranfer size"},
        /* USB转串口设备发送数据 */
        {__userial_dev_send,            "userial_send",            "USB Serial device send data"},
        /* USB转串口设备读数据 */
        {__userial_dev_read,            "userial_read",            "USB Serial device read data"},
        /* 停止USB转串口设备读数据 */
        {__userial_dev_stop,            "userial_stop",            "Stop USB Serial device read data"},
        /* 设置USB转串口设备自动读写数据 */
        {__userial_dev_wr_auto,         "userial_wr_auto",         "enable/disable USB Serial device auto read/write data"},

};

/* 命令集描述 */
aw_local struct aw_shell_cmdset __g_userial_cmdset = {
    .p_enter_name   = "userial_test",                  /* 命令集进入名字*/
    .p_exit_name    = "userial_exit",                  /* 命令集退出 */
    .p_man          = "This cmdset is use to test USB Serial",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/** USB转串口设备热插拔检测回调函数*/
static void userial_monitor(void *arg, uint8_t type){
    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find a USB Serial device\r\n");
        __g_userial_dev_num++;
    }
    else if(type == USBDEV_EJECT){
        aw_kprintf("remove a USB Serial device\r\n");
        __g_userial_dev_num--;
    }
}

/* USB转串口测试任务*/
static void __userial_task (void) {
    int     len;
    uint8_t buf[256] = {0};

    while(1){
        if(__g_mode == 1){
            if((__g_userial_dev_num == 0) || (__g_userial == NULL)){
                continue;
            }

            len = usbh_serial_dev_recieve_sync(__g_userial, buf, 256);
            if(len > 0){
                usbh_serial_dev_send(__g_userial, buf, len);
                memset(buf ,0, 256);
            }
        }
        else{
            aw_mdelay(10);
        }
    }
}

/** USB转串口设备例程入口函数*/
void demo_usbh_serial_entry(void){
    usbh_monitor_handle   userial_monitor_handle;

    /* 注册USB转串口设备监控*/
    userial_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                       USBH_DEV_USERIAL | USBH_DEV_UNIC,
                                                       USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                       userial_monitor, NULL);
    if(userial_monitor_handle == NULL){
        aw_kprintf("userial monitor register failed!\r\n");
        return;
    }

    /* 注册一个命令子集 */
    AW_SHELL_REGISTER_CMDSET(&__g_userial_cmdset, __g_userial_cmdset_cmds);

    AW_TASK_DECL_STATIC(userial_task, 1024 * 10 );

    /* 初始化任务*/
    AW_TASK_INIT(userial_task,             /* 任务实体 */
                "userial_task",            /* 任务名字 */
                 12,                       /* 任务优先级 */
                 1024 * 10 ,               /* 任务堆栈大小 */
               __userial_task,             /* 任务入口函数 */
                (void *)NULL);             /* 任务入口参数 */

    /* 启动任务uvc_task */
    AW_TASK_STARTUP(userial_task);
}
