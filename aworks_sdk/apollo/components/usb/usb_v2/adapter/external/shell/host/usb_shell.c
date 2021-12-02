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
#include "host/class/uvc/uvc_opt.h"
#include "apollo.h"
#include "aw_list.h"
#include "aw_pool.h"
#include "aw_mem.h"
#include "aw_shell.h"
#include "aw_cache.h"
#include "aw_vdebug.h"
#include "host/controller/ehci/usbh_ehci.h"
#include "string.h"
#include "host/awbl_usbh.h"
#include "interior/mem/usb_mem.h"
#include "host/usb_shell_cfg.h"


/* Ѱ��USB�豸shell����*/
static int awbl_shell_usbdev_find (int argc, char *argv[], struct aw_shell_user *p_user){
    usbh_dev_find(0, 0, USBH_DEV_UNKNOWN,AW_TRUE);
    return 0;
}

/** ��ѯEHCI���ݽṹshell����*/
static int awbl_shell_ehci_mem_sta (int argc, char *argv[], struct aw_shell_user *p_user){
    int num;

    if(argc != 1){
        aw_kprintf("error!need 1 argc\r\n");
        return 0;
    }

    num = atoi(argv[0]);

    return usbh_ehci_mem_sta(num);
}

/** ��ѯUSB�ڴ�ѵ�ʹ�����shell����*/
static int awbl_shell_usbh_mem_sta (int argc, char *argv[], struct aw_shell_user *p_user){
    return usb_mem_sta();
}

/** ���USB����Э��ջ�汾*/
static int awbl_shell_usbh_version (int argc, char *argv[], struct aw_shell_user *p_user){
    struct usbh_lib_info  p_usbh_info;
    usbh_lib_info_get(&p_usbh_info);
    aw_kprintf("lib version  : %s\r\n"
            "     build date   : %s\r\n"
            "     modified date: %s\r\n",p_usbh_info.version ,
            p_usbh_info.build_date, p_usbh_info.modified_date);
    return 0;
}

/** ���³�ʼ��USB�豸����*/
static int awbl_shell_usbh_dev_reinit (int argc, char *argv[], struct aw_shell_user *p_user){
    void *temp = NULL;

    if(argc != 1){
        aw_kprintf("error!need 1 argc\r\n");
        return -AW_EINVAL;
    }

    temp = usbh_dev_find_byname(argv[0]);
    if(temp == NULL){
        aw_kprintf("error!can not find usb device\r\n");
        return -AW_EINVAL;
    }
    usbh_dev_reinit(temp);

    return 0;
}

/* ���*/
aw_local const struct aw_shell_cmd __g_usb_cmdset_cmds[] = {
        {awbl_shell_ehci_mem_sta,        "emem",        "check EHCI data structure status"},
        {awbl_shell_usbh_mem_sta,        "umem",        "check USB mem status"},
        {awbl_shell_usbh_version,        "uversion",    "check USB Host stack version"},
        {awbl_shell_usbdev_find,         "udev",        "find all USB devices on USB bus"},
        {awbl_shell_usbh_dev_reinit,     "udev_reinit", "make a USB device hot plug"}
};

/* �������*/
aw_local struct aw_shell_cmdset __g_usb_cmdset = {
    .p_enter_name   = "usb_cmd",                  /* �����������*/
    .p_exit_name    = "usb_exit",                 /* ����˳�  */
    .p_man          = "This cmdset is use to debug usb",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};


/* USB ����������
 * test_mode������ģʽ(�������豸)*/
void usb_shell_register(void){
    /* ע��һ�������Ӽ� */
    AW_SHELL_REGISTER_CMDSET(&__g_usb_cmdset, __g_usb_cmdset_cmds);
}


