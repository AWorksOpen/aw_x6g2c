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
 * \brief USB device hid mouse��ʾ����
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
 *   1. �����豸����������һ��hid�豸
 *   2. ���һֱ���ƶ�
 *
 */

/**
 * \addtogroup demo_if_std_usbd_hid
 * \copydoc demo_std_usbd_hid.c
 */

/** [src_std_usbd_hid] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_assert.h"
#include "device/usbd.h"
#include "device/usbd_obj.h"
#include "string.h"
#include "aw_demo_config.h"
#include "device/class/hid/usbd_hid.h"

static struct usbd_hid    __g_usbd_hid;

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
    "ZLG-MOUSE",
    "AWorks-mouse",
    "20200828"
};


/**
 * \brief USB�������
 * \param[in] p_arg : �������
 * \return ��
 */
static void task_entry (void *p_arg)
{
    int  ret;

    /* ����һ���������ṹ���ڴ�ռ䲢��ʼ��*/
    usbd_mouse_in_report_t *p_report =
            usb_mem_alloc(sizeof(usbd_mouse_in_report_t));
    memset(p_report, 0x00, sizeof(usbd_mouse_in_report_t));

    /* ��ʼ��һ��USB�豸*/
    ret = usbd_obj_init(&__g_usbd_obj,
                        &__g_usbd_info,
                        NULL,
                        DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return;
    }
    /* ����һ��HID ����豸*/
    ret = usbd_hid_mouse_create(&__g_usbd_obj,
                                &__g_usbd_hid,
                                "hid0_mouse");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_hid init failed: %d\n", ret));
        return;
    }
    //aw_usbd_hid_set_rev_cb(&__g_usbd_hid, NULL, &__g_usbd_hid);
    /* ����USB�豸*/
    ret = usbd_obj_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
        return;
    }

    for (;;) {
        /* �ȴ�USB�豸���ӵ���*/
        while (!usbd_obj_fun_valid(&__g_usbd_hid.fun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            usb_sem_take(__g_usbd_hid.semb, AW_SEM_WAIT_FOREVER);
        }
        aw_mdelay(500);
        /* ģ�������������*/
        p_report->x = 10;
        p_report->y = 10;
        /* �������ϱ�������*/
        usbd_hid_mouse_send(&__g_usbd_hid, p_report, AW_SEM_WAIT_FOREVER);
    }
}


/**
 * \brief USB hid�豸 demo
 * \return ��
 */
void demo_std_usbd_hid_mouse_entry (void)
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

/** [src_std_usbd_hid] */

/* end of file */