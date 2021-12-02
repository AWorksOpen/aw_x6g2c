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
#include "aworks.h"
#include "aw_atomic.h"

/** AWorks 适配原子加操作函数*/
int usb_atomic_inc(int *target){
    return aw_atomic_inc_return(target);
}

/** AWorks 适配原子减操作函数*/
int usb_atomic_dec(int *target){
    return aw_atomic_dec_return(target);
}






