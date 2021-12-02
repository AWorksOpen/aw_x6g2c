//- Copyright (c) 2010 James Grenning and Contributed to Unity Project
/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include "unity_fixture.h"
//#include "unity_output_Spy.h"
#include "host/core/usbh.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_common.h"
#include <stdlib.h>
#include <string.h>

/* 测试主机控制器*/
struct test_ctrllor{
    struct usbh_ctrllor_head head;
    int                      test_data;
};

/* 测试主机控制器发送请求函数*/
usb_err_t test_request (struct usbh     *p_hc,
                        struct usbh_trp *p_trp){
    if((p_hc == NULL) || (p_trp == NULL)){
        return -USB_EINVAL;
    }

    return USB_OK;
}

/* 测试主机控制器取消请求函数*/
usb_err_t test_cancel (struct usbh     *p_hc,
                       struct usbh_trp *p_trp){
    if((p_hc == NULL) || (p_trp == NULL)){
        return -USB_EINVAL;
    }

    return USB_OK;
}

/* 测试主机控制器获取帧号函数*/
int test_framenum_get(struct usbh *p_hc){
    if(p_hc == NULL){
        return -USB_EINVAL;
    }

    return 1024;
}

/* 测试主机控制器端点使能函数函数*/
aw_err_t test_ep_enable(struct usbh *p_hc, struct usbh_endpoint *p_ep){
    if((p_hc == NULL) || (p_ep == NULL)){
        return -USB_EINVAL;
    }

    return AW_OK;
}

/* 测试主机控制器端点禁能函数函数*/
aw_err_t test_ep_disable(struct usbh *p_hc, struct usbh_endpoint *p_ep){
    if((p_hc == NULL) || (p_ep == NULL)){
        return -USB_EINVAL;
    }

    return AW_OK;
}

/* 测试主机控制器驱动函数*/
struct usb_ctrllor_drv test_ctrllor_drv1 = {
        .pfn_request      = NULL,
        .pfn_cancel       = NULL,
        .pfn_framenum_get = NULL,
        .pfn_ep_en        = NULL,
        .pfn_ep_dis       = NULL,
};

/* 测试主机控制器驱动函数*/
struct usb_ctrllor_drv test_ctrllor_drv2 = {
        .pfn_request      = test_request,
        .pfn_cancel       = test_cancel,
        .pfn_framenum_get = test_framenum_get,
        .pfn_ep_en        = test_ep_enable,
        .pfn_ep_dis       = test_ep_disable,
};

TEST_GROUP(USB_HOST);

TEST_SETUP(USB_HOST){
}

TEST_TEAR_DOWN(USB_HOST){
}

/** 获取USB主机信息*/
TEST(USB_HOST, USBH_Info_Get){
    struct usbh_info info;

    UNITY_PRINT_EOL();
    usbh_info_get(NULL, USB_FALSE);
    usbh_info_get(NULL, USB_TRUE);
    aw_kprintf("test 1:\r\n");
    usbh_info_get(&info, USB_FALSE);
    aw_kprintf("lib version  : %s\r\n"
            "     build date   : %s\r\n"
            "     modified date: %s\r\n",info.version ,info.build_date, info.modified_date);
    aw_kprintf("test 2:\r\n");
    usbh_info_get(&info, USB_TRUE);
    aw_kprintf("lib version  : %s\r\n"
            "     build date   : %s\r\n"
            "     modified date: %s\r\n",info.version ,info.build_date, info.modified_date);
}

/** 创建销毁USB主机*/
TEST(USB_HOST, USBH_Create_Destory_Test){
    struct usbh         *p_hc    = NULL;
    aw_err_t             ret;
    void                *usrdara = NULL;
    void                *ctrllor = NULL;
    struct test_ctrllor  test;

    memset(&test, 0, sizeof(struct test_ctrllor));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(7);
    TEST_ASSERT_NOT_NULL(p_hc);
    /* 重复创建相同索引的USB主机控制器*/
    p_hc = usbh_create(7);
    TEST_ASSERT_NULL(p_hc);
    p_hc = usbh_hc_get(7);
    TEST_ASSERT_NOT_NULL(p_hc);
    /* 销毁USB主机控制器*/
    usbh_destory(p_hc);
    p_hc = usbh_hc_get(7);
    TEST_ASSERT_NULL(p_hc);
    p_hc = usbh_create(7);
    TEST_ASSERT_NOT_NULL(p_hc);

    p_hc = usbh_hc_get(5);
    TEST_ASSERT_NULL(p_hc);
    p_hc = usbh_hc_get(7);
    TEST_ASSERT_NOT_NULL(p_hc);
    /* 设置用户私有数据*/
    ret = usbh_usrdata_set(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);
    /* 获取用户私有数据*/
    usrdara = usbh_usrdata_get(p_hc);
    TEST_ASSERT_NULL(usrdara);
    usrdara = usbh_usrdata_get(NULL);
    TEST_ASSERT_NULL(usrdara);

    ret = usbh_usrdata_set(p_hc, NULL);
    TEST_ASSERT(ret != AW_OK);
    usrdara = usbh_usrdata_get(p_hc);
    TEST_ASSERT_NULL(usrdara);
    usrdara = usbh_usrdata_get(NULL);
    TEST_ASSERT_NULL(usrdara);

    ret = usbh_usrdata_set(p_hc, p_hc);
    TEST_ASSERT(ret == AW_OK);
    usrdara = usbh_usrdata_get(NULL);
    TEST_ASSERT_NULL(usrdara);
    usrdara = usbh_usrdata_get(p_hc);
    TEST_ASSERT_NOT_NULL(usrdara);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = 6;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;
    test.test_data         = 10;
    /* 设置主机控制器*/
    ret = usbh_ctrllor_set(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);
    /* 获取主机控制器*/
    ctrllor = usbh_ctrllor_get(NULL);
    TEST_ASSERT_NULL(ctrllor);
    ctrllor = usbh_ctrllor_get(p_hc);
    TEST_ASSERT_NULL(ctrllor);

    ret = usbh_ctrllor_set(p_hc, NULL);
    TEST_ASSERT(ret != AW_OK);
    ctrllor = usbh_ctrllor_get(NULL);
    TEST_ASSERT_NULL(ctrllor);
    ctrllor = usbh_ctrllor_get(p_hc);
    TEST_ASSERT_NULL(ctrllor);

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret != AW_OK);
    ctrllor = usbh_ctrllor_get(NULL);
    TEST_ASSERT_NULL(ctrllor);
    ctrllor = usbh_ctrllor_get(p_hc);
    TEST_ASSERT_NULL(ctrllor);

    test.head.ctrllor_type = OHCI;
    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);
    ctrllor = usbh_ctrllor_get(NULL);
    TEST_ASSERT_NULL(ctrllor);
    ctrllor = usbh_ctrllor_get(p_hc);
    TEST_ASSERT_NOT_NULL(ctrllor);
    TEST_ASSERT(((struct test_ctrllor *)ctrllor)->test_data == 10)

    usbh_destory(NULL);
    usbh_destory(p_hc);
}

/** 分配释放USB主机设备地址*/
TEST(USB_HOST, USBH_Addr_Alloc_Free_Test){
    struct usbh *p_hc  = NULL;
    aw_err_t     ret;
    int          addr  = -1;

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(7);
    TEST_ASSERT_NOT_NULL(p_hc);
    /* 申请设备地址*/
    addr = usbh_dev_addr_alloc(NULL);
    TEST_ASSERT(addr < 0);
    addr = usbh_dev_addr_alloc(p_hc);
    TEST_ASSERT((addr >= 1) && (addr <= 128));
    /* 释放设备地址*/
    ret = usbh_dev_addr_free(NULL, -121);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(NULL, 0);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(NULL, 1234);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(NULL, addr);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(p_hc, -121);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(p_hc, 0);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(p_hc, 1234);
    TEST_ASSERT(ret != AW_OK);
    ret = usbh_dev_addr_free(p_hc, addr);
    TEST_ASSERT(ret == AW_OK);

    usbh_destory(p_hc);
}

/** 获取当前帧号*/
TEST(USB_HOST, USBH_FrameNum_Get){
    struct usbh         *p_hc       = NULL;
    aw_err_t             ret;
    int                  frame_num  = -1;
    struct test_ctrllor  test;

    memset(&test, 0, sizeof(struct test_ctrllor));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(6);
    TEST_ASSERT_NOT_NULL(p_hc);
    /* 获取当前帧号*/
    frame_num = usbh_frame_num_get(NULL);
    TEST_ASSERT(frame_num < 0);
    /* 获取当前帧号*/
    frame_num = usbh_frame_num_get(p_hc);
    TEST_ASSERT(frame_num < 0);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = EHCI;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);

    /* 获取当前帧号*/
    frame_num = usbh_frame_num_get(p_hc);
    TEST_ASSERT(frame_num < 0);

    test.head.ctrllor_drv  = &test_ctrllor_drv1;
    frame_num = usbh_frame_num_get(p_hc);
    TEST_ASSERT(frame_num < 0);

    test.head.ctrllor_drv  = &test_ctrllor_drv2;
    frame_num = usbh_frame_num_get(p_hc);
    TEST_ASSERT(frame_num == 1024);

    usbh_destory(p_hc);
}

/** 使能端点*/
TEST(USB_HOST, USBH_EP_Enable){
    struct usbh         *p_hc       = NULL;
    aw_err_t             ret;
    struct test_ctrllor  test;
    struct usbh_endpoint test_ep;

    memset(&test, 0, sizeof(struct test_ctrllor));
    memset(&test_ep, 0, sizeof(struct usbh_endpoint));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(5);
    TEST_ASSERT_NOT_NULL(p_hc);

    ret = usbh_ep_enable(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_ep_enable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = XHCI;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);

    ret = usbh_ep_enable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv1;
    ret = usbh_ep_enable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv2;
    ret = usbh_ep_enable(p_hc, &test_ep);
    TEST_ASSERT(ret == AW_OK);

    usbh_destory(p_hc);
}

/** 禁能端点*/
TEST(USB_HOST, USBH_EP_Disable){
    struct usbh         *p_hc       = NULL;
    aw_err_t             ret;
    struct test_ctrllor  test;
    struct usbh_endpoint test_ep;

    memset(&test, 0, sizeof(struct test_ctrllor));
    memset(&test_ep, 0, sizeof(struct usbh_endpoint));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(4);
    TEST_ASSERT_NOT_NULL(p_hc);

    ret = usbh_ep_disable(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_ep_disable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = XHCI;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);

    ret = usbh_ep_disable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv1;
    ret = usbh_ep_disable(p_hc, &test_ep);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv2;
    ret = usbh_ep_disable(p_hc, &test_ep);
    TEST_ASSERT(ret == AW_OK);

    usbh_destory(p_hc);
}

/** 启动传输*/
TEST(USB_HOST, USBH_Request){
    struct usbh         *p_hc       = NULL;
    aw_err_t             ret;
    struct test_ctrllor  test;
    struct usbh_trp      test_trp;

    memset(&test, 0, sizeof(struct test_ctrllor));
    memset(&test_trp, 0, sizeof(struct usbh_trp));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(3);
    TEST_ASSERT_NOT_NULL(p_hc);

    ret = usbh_request(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_request(NULL, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = XHCI;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv1;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv2;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test_trp.p_ep = unity_malloc(sizeof(struct usbh_endpoint));
    TEST_ASSERT_NOT_NULL(test_trp.p_ep);
    memset(test_trp.p_ep, 0, sizeof(struct usbh_endpoint));

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test_trp.p_ep->p_desc = unity_malloc(sizeof(struct usb_endpoint_desc));
    TEST_ASSERT_NOT_NULL(test_trp.p_ep->p_desc);
    memset(test_trp.p_ep->p_desc, 0, sizeof(struct usb_endpoint_desc));

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret == AW_OK);

    test_trp.len = 1;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test_trp.len = 0;
    test_trp.p_data = unity_malloc(1);
    TEST_ASSERT_NOT_NULL(test_trp.p_data);
    memset(test_trp.p_data, 0, 1);

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test_trp.len = 1;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret == AW_OK);

    test_trp.p_ctrl = unity_malloc(sizeof(struct usb_ctrlreq));
    TEST_ASSERT_NOT_NULL(test_trp.p_ctrl);
    memset(test_trp.p_ctrl, 0, sizeof(struct usb_ctrlreq));

    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret == AW_OK);

    test_trp.len = 0;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    unity_free(test_trp.p_data);
    test_trp.p_data = NULL;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret == AW_OK);

    test_trp.len = 1;
    ret = usbh_request(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);
    usbh_destory(p_hc);

    unity_free(test_trp.p_ep->p_desc);
    unity_free(test_trp.p_ep);
    unity_free(test_trp.p_ctrl);
}

/** 取消传输*/
TEST(USB_HOST, USBH_Cancel){
    struct usbh         *p_hc       = NULL;
    aw_err_t             ret;
    struct test_ctrllor  test;
    struct usbh_trp      test_trp;

    memset(&test, 0, sizeof(struct test_ctrllor));
    memset(&test_trp, 0, sizeof(struct usbh_trp));

    UNITY_PRINT_EOL();
    /* 创建USB主机控制器*/
    p_hc = usbh_create(3);
    TEST_ASSERT_NOT_NULL(p_hc);

    ret = usbh_cancel(NULL, NULL);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_cancel(NULL, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    ret = usbh_cancel(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = NULL;
    test.head.ctrllor_type = XHCI;
    test.head.p_hc         = p_hc;
    test.head.is_init      = AW_TRUE;

    ret = usbh_ctrllor_set(p_hc, &test);
    TEST_ASSERT(ret == AW_OK);

    ret = usbh_cancel(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv1;
    ret = usbh_cancel(p_hc, &test_trp);
    TEST_ASSERT(ret != AW_OK);

    test.head.ctrllor_drv  = &test_ctrllor_drv2;
    ret = usbh_cancel(p_hc, &test_trp);
    TEST_ASSERT(ret == AW_OK);

    usbh_destory(p_hc);
}

