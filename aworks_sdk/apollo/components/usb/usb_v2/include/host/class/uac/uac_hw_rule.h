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
#ifndef __UAC_HW_RULE_H_
#define __UAC_HW_RULE_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "usb_common.h"
#include "usb_errno.h"
#include <string.h>
#include <stdio.h>
//#include "host/class/uac/uac_pcm.h"

#define UAC_MASK_MAX  256

#define UAC_PCM_HW_PARAM_ACCESS       0   /* Access type */
#define UAC_PCM_HW_PARAM_FORMAT       1   /* 格式*/
#define UAC_PCM_HW_PARAM_SUBFORMAT    2   /* 子格式*/
#define UAC_PCM_HW_PARAM_FIRST_MASK   UAC_PCM_HW_PARAM_ACCESS
#define UAC_PCM_HW_PARAM_LAST_MASK    UAC_PCM_HW_PARAM_SUBFORMAT

#define UAC_PCM_HW_PARAM_SAMPLE_BITS  8   /* 每一个样本的位数*/
#define UAC_PCM_HW_PARAM_FRAME_BITS   9   /* 每一帧的位数*/
#define UAC_PCM_HW_PARAM_CHANNELS     10  /* 通道数量*/
#define UAC_PCM_HW_PARAM_RATE         11  /* 采样率*/
#define UAC_PCM_HW_PARAM_PERIOD_TIME  12  /* 每一个中断之间的间隔(微秒)*/
#define UAC_PCM_HW_PARAM_PERIOD_SIZE  13  /* 每一个中断之间的帧数*/
#define UAC_PCM_HW_PARAM_PERIOD_BYTES 14  /* 每一个中断之间的字节*/
#define UAC_PCM_HW_PARAM_PERIODS      15  /* 一个缓存的中断数量*/
#define UAC_PCM_HW_PARAM_BUFFER_TIME  16  /* 缓存之间的间隔(微秒)*/
#define UAC_PCM_HW_PARAM_BUFFER_SIZE  17  /* 缓存大小(帧)*/
#define UAC_PCM_HW_PARAM_BUFFER_BYTES 18  /* 缓存大小(字节)*/
#define UAC_PCM_HW_PARAM_TICK_TIME    19  /* 每个时钟间隔(微秒) */

#define UAC_PCM_HW_PARAM_FIRST_INFO   UAC_PCM_HW_PARAM_SAMPLE_BITS
#define UAC_PCM_HW_PARAM_LAST_INFO    UAC_PCM_HW_PARAM_TICK_TIME


#define SNDRV_PCM_HW_PARAM_SAMPLE_BITS       8  /* Bits per sample */
#define SNDRV_PCM_HW_PARAM_TICK_TIME        19  /* Approx tick duration in us */

/* PCM硬件信息结构*/
struct uac_pcm_hardware {
    uint32_t formats;           /* 格式位数*/
    uint32_t rates;             /* 采样率*/
    uint32_t rate_min;          /* 最小采样率*/
    uint32_t rate_max;          /* 最大采样率*/
    uint32_t channels_min;      /* 最小通道数*/
    uint32_t channels_max;      /* 最大通道数*/
    size_t   buffer_bytes_max;  /* 最大缓存大小(字节)*/
    size_t   period_bytes_min;  /* 最小周期字节数*/
    size_t   period_bytes_max;  /* 最大周期字节数*/
    uint32_t periods_min;       /* 最小周期*/
    uint32_t periods_max;       /* 最大周期*/
    size_t   fifo_size;         /* 缓存大小 */
};

struct uac_pcm_hw_rule;
struct uac_pcm_hw_params;
/* USB音频PCM硬件规则回调函数*/
typedef int (*uac_pcm_hw_rule_func_t)(struct uac_pcm_hw_params *params,
                                      struct uac_pcm_hw_rule   *rule);

struct uac_mask {
    uint32_t bits[(UAC_MASK_MAX + 31)/32];
};

/* USB音频PCM硬件约束列表*/
struct uac_pcm_hw_constraint_list {
    uint32_t  count;
    uint32_t *list;
    uint32_t  mask;
};

/* USB音频PCM硬件参数规则结构体*/
struct uac_pcm_hw_rule {
    uint32_t               cond;
    uac_pcm_hw_rule_func_t func;
    int                    var;
    int                    deps[3];
    void                  *private;
};

/* USB音频PCM硬件信息区间*/
struct uac_pcm_hw_interval {
    uint32_t min, max;    /* */
    uint32_t openmin:1,   /* min是否是开集，为1表示开区间，为0表示闭区间*/
             openmax:1,   /* max是否是开集，为1表示开区间，为0表示闭区间*/
             integer:1,   /* 为1表示这个不是一个范围区间，而是一个固定的整数*/
             empty:1;     /* 是否有效，1为无效，0为有效*/
};

/* USB音频PCM硬件参数结构体*/
struct uac_pcm_hw_params {
    uint64_t                   format;
    int                        pcm_format_size_bit;  /* PCM格式位宽位*/
    struct uac_pcm_hw_interval hw_info[UAC_PCM_HW_PARAM_LAST_INFO -
                                       UAC_PCM_HW_PARAM_FIRST_INFO + 1];
    uint32_t                   rmask;                /* 请求掩码*/
    uint32_t                   cmask;                /* 变化掩码*/
    uint32_t                   msbits;               /* 使用最高位*/
    uint32_t                   rate_num;             /* 采样率分子*/
    uint32_t                   rate_den;             /* 采样率分母*/
    uint32_t                   fifo_size;            /* 帧缓存大小*/
};

/* USB音频PCM硬件参数限制结构体*/
struct uac_pcm_hw_constraints {
    struct uac_mask            masks[UAC_PCM_HW_PARAM_LAST_MASK -
                                     UAC_PCM_HW_PARAM_FIRST_MASK + 1];
    struct uac_pcm_hw_interval hw_interval[UAC_PCM_HW_PARAM_LAST_INFO -
                                           UAC_PCM_HW_PARAM_FIRST_INFO + 1];
    uint32_t                   rules_num;
    uint32_t                   rules_all;
    struct uac_pcm_hw_rule    *rules;
};

/** 计算a除以b*/
static inline uint32_t div_down(uint32_t a, uint32_t b){
    if (b == 0){
        return USB_UINT_MAX;
    }
    return a / b;
}

/** 计算两个数相处，余数是remainder*/
static inline uint64_t div_u64_rem(uint64_t dividend, uint32_t divisor, uint32_t *remainder){
    *remainder = dividend % divisor;
    return dividend / divisor;
}


/** 计算a和b的乘积*/
static inline uint32_t mul(uint32_t a, uint32_t b){
    if (a == 0){
        return 0;
    }
    /* 检查乘积是否超过范围*/
    if (div_down(USB_UINT_MAX, a) < b){
        return USB_UINT_MAX;
    }
    return a * b;
}

/** 函数返回除数，r返回余数*/
static inline uint32_t div32(uint32_t a, uint32_t b, uint32_t *r){
    if (b == 0) {
        *r = 0;
        return USB_UINT_MAX;
    }
    *r = a % b;
    return a / b;
}

/** 计算a*b/c，余数是r*/
static inline uint32_t muldiv32(uint32_t a, uint32_t b,
                                uint32_t c, uint32_t *r){
    uint64_t n = (uint64_t) a * b;
    /* 除数为0*/
    if (c == 0) {
        *r = 0;
        return USB_UINT_MAX;
    }
    n = div_u64_rem(n, c, r);
    if (n >= USB_UINT_MAX) {
        *r = 0;
        return USB_UINT_MAX;
    }
    return n;
}

/** 初始化USB音频PCM硬件信息*/
static inline void uac_pcm_hw_interval_any(struct uac_pcm_hw_interval *i){
    i->min = 0;
    i->openmin = 0;
    i->max = USB_UINT_MAX;
    i->openmax = 0;
    i->integer = 0;
    i->empty = 0;
}
/** 获取硬件参数区间信息*/
static inline struct uac_pcm_hw_interval *hw_param_hw_interval(struct uac_pcm_hw_params *params,
                                                               int                       var){
    return &params->hw_info[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
static inline const struct uac_pcm_hw_interval *hw_param_hw_interval_c(const struct uac_pcm_hw_params *params,
                                                                       int                             var){
    return &params->hw_info[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
/** 检查val是否在区间范围内*/
static inline int uac_pcm_hw_interval_test(const struct uac_pcm_hw_interval *i, uint32_t val){
    return !((i->min > val || (i->min == val && i->openmin) ||
          i->max < val || (i->max == val && i->openmax)));
}
/** 检查硬件区间信息是否有效*/
static inline int uac_pcm_hw_interval_empty(const struct uac_pcm_hw_interval *i){
    return i->empty;
}
/** 检查这个区间信息是否只有唯一值*/
static inline int uac_pcm_hw_interval_single(const struct uac_pcm_hw_interval *i){
    return ((i->min == i->max) ||
        (i->min + 1 == i->max && i->openmax));
}
/** 获取区间信息最小值*/
static inline int uac_pcm_hw_interval_value(const struct uac_pcm_hw_interval *i){
    return i->min;
}
/** 设置硬件区间信息无效*/
static inline void uac_pcm_hw_interval_none(struct uac_pcm_hw_interval *i){
    i->empty = 1;
}
/** 检查硬件区间信息是否有效*/
static inline int uac_pcm_hw_interval_checkempty(const struct uac_pcm_hw_interval *i){
    return ((i->min > i->max) ||
        ((i->min == i->max) && (i->openmin || i->openmax)));
}
/** 获取具体的硬件区间信息*/
static inline struct uac_pcm_hw_interval *constrs_hw_interval(struct uac_pcm_hw_constraints *constrs,
                                                              int                            var){
    return &constrs->hw_interval[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
/** 设置USB音频PCM硬件信息为固定整数类型*/
static inline int uac_pcm_hw_interval_setinteger(struct uac_pcm_hw_interval *i){
    if (i->integer){
        return 0;
    }
    /* 两个都是开区间，且最小值等于最大值，返回错误*/
    if ((i->openmin) && (i->openmax) && (i->min == i->max)){
        return -USB_EINVAL;
    }
    /* 设置为固定整数类型*/
    i->integer = 1;
    return 1;
}

/** 获取通道数*/
static inline uint32_t params_channels(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_CHANNELS)->min;
}
/** 获取缓存大小(字节)*/
static inline uint32_t params_buffer_bytes(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_BUFFER_BYTES)->min;
}
/** 获取采样率*/
static inline uint32_t params_rate(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_RATE)->min;
}
/** 获取周期个数*/
static inline uint32_t params_periods(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIODS)->min;
}
/** 获取周期大小(帧)*/
static inline uint32_t params_period_size(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIOD_SIZE)->min;
}
/** 获取周期大小(字节)*/
static inline uint32_t params_period_bytes(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIOD_BYTES)->min;
}
/** 获取缓存帧数量*/
static inline uint32_t params_buffer_size(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_BUFFER_SIZE)->min;
}

/**
 * 采样率规则函数
 *
 * @param params PCM硬件参数
 * @param rule   规则结构体
 *
 * @return 返回1表示值改变，返回0表示值没改变
 */
int hw_rule_rate(struct uac_pcm_hw_params *params,
                 struct uac_pcm_hw_rule   *rule);
/**
 * 通道数规则函数
 *
 * @param params PCM硬件参数
 * @param rule   规则结构体
 *
 * @return 返回1表示值改变，返回0表示值没改变
 */
int hw_rule_channels(struct uac_pcm_hw_params *params,
                     struct uac_pcm_hw_rule   *rule);
/**
 * 音频格式规则函数
 *
 * @param params PCM硬件参数
 * @param rule   规则结构体
 *
 * @return 返回1表示值改变，返回0表示值没改变
 */
int hw_rule_format(struct uac_pcm_hw_params *params,
                   struct uac_pcm_hw_rule   *rule);
/**
 * 周期时间规则函数
 *
 * @param params PCM硬件参数
 * @param rule   规则结构体
 *
 * @return 返回1表示值改变，返回0表示值没改变
 */
int hw_rule_period_time(struct uac_pcm_hw_params *params,
                        struct uac_pcm_hw_rule   *rule);
/**
 * 添加一个USB音频PCM硬件啊限制规则
 *
 * @param constrs PCM硬件限制条件
 * @param cond    条件位
 * @param var     具体的硬件参数
 * @param func    规则函数
 * @param private 传递给规则函数的参数
 * @param dep     独立变量
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_pcm_hw_rule_add(struct uac_pcm_hw_constraints *constrs,
                              uint32_t                       cond,
                              int                            var,
                              uac_pcm_hw_rule_func_t         func,
                              void                          *private,
                              int                            dep1,
                              int                            dep2,
                              int                            dep3);
/**
 * 设置PCM硬件容器最小最大值
 *
 * @param constrs 硬件约束
 * @param var     具体硬件参数
 * @param min     最小值
 * @param max     最大值
 *
 * @return 返回1表示值改变，返回0表示值没改变
 */
int uac_pcm_hw_constraint_minmax(struct uac_pcm_hw_constraints *constrs,
                                 int                            var,
                                 uint32_t                       min,
                                 uint32_t                       max);
/**
 * 初始化USB音频PCM硬件限制
 *
 * @param constrs PCM硬件限制条件
 *
 * @return 成功返回USB_OK
 */
usb_err_t uac_pcm_hw_constraints_init(struct uac_pcm_hw_constraints *constrs);
/** 设置PCM硬件限制条件*/
usb_err_t uac_pcm_hw_constraints_complete(struct uac_pcm_hw_constraints *constrs,
                                          struct uac_pcm_hardware       *hw,
                                          size_t                        *buffer_bytes_max);
/** 设置usb音频硬件参数*/
struct uac_pcm_hw_params *uac_get_hw_params(void *params_usr);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


