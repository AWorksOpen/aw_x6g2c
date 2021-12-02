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
*******************************************************************************/
/**
 * \file
 * \brief USB �����ӡ����ʾ����
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
 *   1. ���ѡ��һ���ĵ��������ӡ���ڴ�ӡ����ѡ����Ӵ�ӡ����
 *    . �������ӱ��ش�ӡ����
 *    . �����ʹ�����еĶ˿ڡ���ѡ��USB�����ӡ���˿ڡ�
 *    . ����ѡ��Generic������ӡ��ѡ��Generic/Text Only��
 *    . �����ʹ�õ�ǰ�Ѱ�װ����������
 *   2. ��ӡ����ӳɹ��󣬵����ӡ�����ڻ��յ���ƪ�ĵ�������
 *
 */

#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_assert.h"
#include "device/core/usbd.h"
#include "device/core/usbd_obj.h"
#include "string.h"
#include "aw_demo_config.h"
#include "device/class/vprinter/usbd_vprinter.h"


#define __BUF_SIZE 4*1024

static usbd_vp_t __g_usbd_printer;
/** \brief ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
static struct usbd_obj            __g_usbd_obj;
const static struct usbd_obj_info __g_usbd_info = {
    0x00,
    0x00,
    0x00,
    64,
    0x0100,
    0x4033,
    0x5093,
    "ZLG-VP",
    "AWorks-vp",
    "20200828"
};



/**
 * \brief USB�������
 * \param[in] p_arg  �������
 * \retval  ��
 */
static void task_entry (void *p_arg)
{
    aw_err_t ret;
    void *p_buf;
    /* ��ʼ��һ��USB�豸*/
    ret = usbd_obj_init(&__g_usbd_obj,
                        &__g_usbd_info,
                        NULL,
                        DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return;
    }

    /* ����һ����ӡ���豸*/
    ret = usbd_printer_create(&__g_usbd_obj,
                              &__g_usbd_printer,
                              "usb_printer");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_printer init failed: %d\n", ret));
        return;
    }

    /* ����USB�豸*/
    ret = usbd_obj_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    /* �������ݻ���*/
    p_buf = usb_mem_alloc(__BUF_SIZE);
    if (p_buf == NULL){
        AW_ERRF(("aw_usb_mem_alloc error!"));
        return ;
    }

    while(1){

        /* �ȴ�USB�豸���ӵ���*/
        while (!usbd_obj_fun_valid(&__g_usbd_printer.fun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            usb_sem_take(__g_usbd_printer.semb, AW_SEM_WAIT_FOREVER);
        }
        /* ��������(����)*/
        ret = usbd_cdc_vp_read_sync(&__g_usbd_printer,
                                    p_buf,
                                    __BUF_SIZE,
                                    AW_SEM_WAIT_FOREVER);
        if (ret > 0) {
            memset(p_buf + ret,0,__BUF_SIZE - ret);
            AW_INFOF(("[recv]:%s\n", (char *)p_buf));
        }
    }
}



/**
 * \brief USB ��ӡ���豸 demo
 * \retval ��
 */
void demo_usbd_vprinter_entry (void)
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









