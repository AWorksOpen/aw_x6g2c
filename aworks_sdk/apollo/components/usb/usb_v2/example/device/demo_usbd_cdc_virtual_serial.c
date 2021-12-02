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
 * \brief USB device cdc ���⴮����ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USB��
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��������USB DEVICE�ӿ������ʹ��USB������
 *
 * - ʵ������
 *   1. ���Գ���һ�����⴮���豸�����豸������˿ڿ��Կ�����Ӧ��COM��
 *   2. �򿪴������������ȴ���ȽϾ�
 *   3. �����������������ֵ�����յ���Ӧ������
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

/* ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
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

/* USB�������
 * p_arg : �������*/
static void task_entry (void *p_arg)
{
    aw_err_t  ret;
    char *buff = "Hello,i am AWorks\r\n";
    char *read_buf = NULL;
    unsigned int len;

    /* ���������*/
    read_buf = usb_mem_alloc(1024);
    if(read_buf == NULL){
        AW_ERRF(("read_buf alloc failed\r\n"));
        return;
    }

    /* ��ʼ��һ��USB�豸*/
    ret = usbd_obj_init(&__g_usbd_obj,
                        &__g_usbd_info,
                        NULL,
                        DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\r\n", ret));
        return;
    }
    /* ����һ�����⴮��*/
    ret = usbd_cdc_vs_create (&__g_usbd_obj,
                              &__g_usbd_cdc,
                              0,
                              "cdc0",
                              1024);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc init failed: %d\r\n", ret));
        return;
    }

    /* ���ô���ģʽ*/
    ret = usbd_cdc_vs_set_rxmode(&__g_usbd_cdc, USBD_CDC_VS_TRANFER_SYNC);
    //ret = usbd_cdc_vs_set_rxmode(&__g_usbd_cdc, USBD_CDC_VS_TRANFER_ASYNC);
    if(ret != AW_OK){
        AW_ERRF(("usbd_cdc set rx mode failed: %d\r\n", ret));
        return;
    }

    /* ����USB�豸*/
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

/* USB cdc ���� demo*/
void demo_std_usbd_cdc_serial_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,        /* ����ʵ�� */
                "task",       /* �������� */
                 6,           /* �������ȼ� */
                 2048,        /* �����ջ��С */
                 task_entry,  /* ������ں��� */
                (void*)0);    /* ������ڲ��� */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_cdc] */

/* end of file */


