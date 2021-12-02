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
#include "unity_fixture.h"

static void runAllTests(void){
    RUN_TEST_GROUP(USB_HOST);
//    RUN_TEST_GROUP(UnityCommandOptions);
//    RUN_TEST_GROUP(LeakDetection);
//    RUN_TEST_GROUP(InternalMalloc);
}

/** 单元测试入口*/
int usb_unity_test_main(int argc, const char* argv[]){
    return UnityMain(argc, argv, runAllTests);
}

