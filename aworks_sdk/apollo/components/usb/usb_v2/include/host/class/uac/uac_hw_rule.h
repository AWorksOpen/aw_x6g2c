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
#define UAC_PCM_HW_PARAM_FORMAT       1   /* ��ʽ*/
#define UAC_PCM_HW_PARAM_SUBFORMAT    2   /* �Ӹ�ʽ*/
#define UAC_PCM_HW_PARAM_FIRST_MASK   UAC_PCM_HW_PARAM_ACCESS
#define UAC_PCM_HW_PARAM_LAST_MASK    UAC_PCM_HW_PARAM_SUBFORMAT

#define UAC_PCM_HW_PARAM_SAMPLE_BITS  8   /* ÿһ��������λ��*/
#define UAC_PCM_HW_PARAM_FRAME_BITS   9   /* ÿһ֡��λ��*/
#define UAC_PCM_HW_PARAM_CHANNELS     10  /* ͨ������*/
#define UAC_PCM_HW_PARAM_RATE         11  /* ������*/
#define UAC_PCM_HW_PARAM_PERIOD_TIME  12  /* ÿһ���ж�֮��ļ��(΢��)*/
#define UAC_PCM_HW_PARAM_PERIOD_SIZE  13  /* ÿһ���ж�֮���֡��*/
#define UAC_PCM_HW_PARAM_PERIOD_BYTES 14  /* ÿһ���ж�֮����ֽ�*/
#define UAC_PCM_HW_PARAM_PERIODS      15  /* һ��������ж�����*/
#define UAC_PCM_HW_PARAM_BUFFER_TIME  16  /* ����֮��ļ��(΢��)*/
#define UAC_PCM_HW_PARAM_BUFFER_SIZE  17  /* �����С(֡)*/
#define UAC_PCM_HW_PARAM_BUFFER_BYTES 18  /* �����С(�ֽ�)*/
#define UAC_PCM_HW_PARAM_TICK_TIME    19  /* ÿ��ʱ�Ӽ��(΢��) */

#define UAC_PCM_HW_PARAM_FIRST_INFO   UAC_PCM_HW_PARAM_SAMPLE_BITS
#define UAC_PCM_HW_PARAM_LAST_INFO    UAC_PCM_HW_PARAM_TICK_TIME


#define SNDRV_PCM_HW_PARAM_SAMPLE_BITS       8  /* Bits per sample */
#define SNDRV_PCM_HW_PARAM_TICK_TIME        19  /* Approx tick duration in us */

/* PCMӲ����Ϣ�ṹ*/
struct uac_pcm_hardware {
    uint32_t formats;           /* ��ʽλ��*/
    uint32_t rates;             /* ������*/
    uint32_t rate_min;          /* ��С������*/
    uint32_t rate_max;          /* ��������*/
    uint32_t channels_min;      /* ��Сͨ����*/
    uint32_t channels_max;      /* ���ͨ����*/
    size_t   buffer_bytes_max;  /* ��󻺴��С(�ֽ�)*/
    size_t   period_bytes_min;  /* ��С�����ֽ���*/
    size_t   period_bytes_max;  /* ��������ֽ���*/
    uint32_t periods_min;       /* ��С����*/
    uint32_t periods_max;       /* �������*/
    size_t   fifo_size;         /* �����С */
};

struct uac_pcm_hw_rule;
struct uac_pcm_hw_params;
/* USB��ƵPCMӲ������ص�����*/
typedef int (*uac_pcm_hw_rule_func_t)(struct uac_pcm_hw_params *params,
                                      struct uac_pcm_hw_rule   *rule);

struct uac_mask {
    uint32_t bits[(UAC_MASK_MAX + 31)/32];
};

/* USB��ƵPCMӲ��Լ���б�*/
struct uac_pcm_hw_constraint_list {
    uint32_t  count;
    uint32_t *list;
    uint32_t  mask;
};

/* USB��ƵPCMӲ����������ṹ��*/
struct uac_pcm_hw_rule {
    uint32_t               cond;
    uac_pcm_hw_rule_func_t func;
    int                    var;
    int                    deps[3];
    void                  *private;
};

/* USB��ƵPCMӲ����Ϣ����*/
struct uac_pcm_hw_interval {
    uint32_t min, max;    /* */
    uint32_t openmin:1,   /* min�Ƿ��ǿ�����Ϊ1��ʾ�����䣬Ϊ0��ʾ������*/
             openmax:1,   /* max�Ƿ��ǿ�����Ϊ1��ʾ�����䣬Ϊ0��ʾ������*/
             integer:1,   /* Ϊ1��ʾ�������һ����Χ���䣬����һ���̶�������*/
             empty:1;     /* �Ƿ���Ч��1Ϊ��Ч��0Ϊ��Ч*/
};

/* USB��ƵPCMӲ�������ṹ��*/
struct uac_pcm_hw_params {
    uint64_t                   format;
    int                        pcm_format_size_bit;  /* PCM��ʽλ��λ*/
    struct uac_pcm_hw_interval hw_info[UAC_PCM_HW_PARAM_LAST_INFO -
                                       UAC_PCM_HW_PARAM_FIRST_INFO + 1];
    uint32_t                   rmask;                /* ��������*/
    uint32_t                   cmask;                /* �仯����*/
    uint32_t                   msbits;               /* ʹ�����λ*/
    uint32_t                   rate_num;             /* �����ʷ���*/
    uint32_t                   rate_den;             /* �����ʷ�ĸ*/
    uint32_t                   fifo_size;            /* ֡�����С*/
};

/* USB��ƵPCMӲ���������ƽṹ��*/
struct uac_pcm_hw_constraints {
    struct uac_mask            masks[UAC_PCM_HW_PARAM_LAST_MASK -
                                     UAC_PCM_HW_PARAM_FIRST_MASK + 1];
    struct uac_pcm_hw_interval hw_interval[UAC_PCM_HW_PARAM_LAST_INFO -
                                           UAC_PCM_HW_PARAM_FIRST_INFO + 1];
    uint32_t                   rules_num;
    uint32_t                   rules_all;
    struct uac_pcm_hw_rule    *rules;
};

/** ����a����b*/
static inline uint32_t div_down(uint32_t a, uint32_t b){
    if (b == 0){
        return USB_UINT_MAX;
    }
    return a / b;
}

/** �����������ദ��������remainder*/
static inline uint64_t div_u64_rem(uint64_t dividend, uint32_t divisor, uint32_t *remainder){
    *remainder = dividend % divisor;
    return dividend / divisor;
}


/** ����a��b�ĳ˻�*/
static inline uint32_t mul(uint32_t a, uint32_t b){
    if (a == 0){
        return 0;
    }
    /* ���˻��Ƿ񳬹���Χ*/
    if (div_down(USB_UINT_MAX, a) < b){
        return USB_UINT_MAX;
    }
    return a * b;
}

/** �������س�����r��������*/
static inline uint32_t div32(uint32_t a, uint32_t b, uint32_t *r){
    if (b == 0) {
        *r = 0;
        return USB_UINT_MAX;
    }
    *r = a % b;
    return a / b;
}

/** ����a*b/c��������r*/
static inline uint32_t muldiv32(uint32_t a, uint32_t b,
                                uint32_t c, uint32_t *r){
    uint64_t n = (uint64_t) a * b;
    /* ����Ϊ0*/
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

/** ��ʼ��USB��ƵPCMӲ����Ϣ*/
static inline void uac_pcm_hw_interval_any(struct uac_pcm_hw_interval *i){
    i->min = 0;
    i->openmin = 0;
    i->max = USB_UINT_MAX;
    i->openmax = 0;
    i->integer = 0;
    i->empty = 0;
}
/** ��ȡӲ������������Ϣ*/
static inline struct uac_pcm_hw_interval *hw_param_hw_interval(struct uac_pcm_hw_params *params,
                                                               int                       var){
    return &params->hw_info[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
static inline const struct uac_pcm_hw_interval *hw_param_hw_interval_c(const struct uac_pcm_hw_params *params,
                                                                       int                             var){
    return &params->hw_info[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
/** ���val�Ƿ������䷶Χ��*/
static inline int uac_pcm_hw_interval_test(const struct uac_pcm_hw_interval *i, uint32_t val){
    return !((i->min > val || (i->min == val && i->openmin) ||
          i->max < val || (i->max == val && i->openmax)));
}
/** ���Ӳ��������Ϣ�Ƿ���Ч*/
static inline int uac_pcm_hw_interval_empty(const struct uac_pcm_hw_interval *i){
    return i->empty;
}
/** ������������Ϣ�Ƿ�ֻ��Ψһֵ*/
static inline int uac_pcm_hw_interval_single(const struct uac_pcm_hw_interval *i){
    return ((i->min == i->max) ||
        (i->min + 1 == i->max && i->openmax));
}
/** ��ȡ������Ϣ��Сֵ*/
static inline int uac_pcm_hw_interval_value(const struct uac_pcm_hw_interval *i){
    return i->min;
}
/** ����Ӳ��������Ϣ��Ч*/
static inline void uac_pcm_hw_interval_none(struct uac_pcm_hw_interval *i){
    i->empty = 1;
}
/** ���Ӳ��������Ϣ�Ƿ���Ч*/
static inline int uac_pcm_hw_interval_checkempty(const struct uac_pcm_hw_interval *i){
    return ((i->min > i->max) ||
        ((i->min == i->max) && (i->openmin || i->openmax)));
}
/** ��ȡ�����Ӳ��������Ϣ*/
static inline struct uac_pcm_hw_interval *constrs_hw_interval(struct uac_pcm_hw_constraints *constrs,
                                                              int                            var){
    return &constrs->hw_interval[var - UAC_PCM_HW_PARAM_FIRST_INFO];
}
/** ����USB��ƵPCMӲ����ϢΪ�̶���������*/
static inline int uac_pcm_hw_interval_setinteger(struct uac_pcm_hw_interval *i){
    if (i->integer){
        return 0;
    }
    /* �������ǿ����䣬����Сֵ�������ֵ�����ش���*/
    if ((i->openmin) && (i->openmax) && (i->min == i->max)){
        return -USB_EINVAL;
    }
    /* ����Ϊ�̶���������*/
    i->integer = 1;
    return 1;
}

/** ��ȡͨ����*/
static inline uint32_t params_channels(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_CHANNELS)->min;
}
/** ��ȡ�����С(�ֽ�)*/
static inline uint32_t params_buffer_bytes(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_BUFFER_BYTES)->min;
}
/** ��ȡ������*/
static inline uint32_t params_rate(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_RATE)->min;
}
/** ��ȡ���ڸ���*/
static inline uint32_t params_periods(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIODS)->min;
}
/** ��ȡ���ڴ�С(֡)*/
static inline uint32_t params_period_size(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIOD_SIZE)->min;
}
/** ��ȡ���ڴ�С(�ֽ�)*/
static inline uint32_t params_period_bytes(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_PERIOD_BYTES)->min;
}
/** ��ȡ����֡����*/
static inline uint32_t params_buffer_size(const struct uac_pcm_hw_params *p){
    return hw_param_hw_interval_c(p, UAC_PCM_HW_PARAM_BUFFER_SIZE)->min;
}

/**
 * �����ʹ�����
 *
 * @param params PCMӲ������
 * @param rule   ����ṹ��
 *
 * @return ����1��ʾֵ�ı䣬����0��ʾֵû�ı�
 */
int hw_rule_rate(struct uac_pcm_hw_params *params,
                 struct uac_pcm_hw_rule   *rule);
/**
 * ͨ����������
 *
 * @param params PCMӲ������
 * @param rule   ����ṹ��
 *
 * @return ����1��ʾֵ�ı䣬����0��ʾֵû�ı�
 */
int hw_rule_channels(struct uac_pcm_hw_params *params,
                     struct uac_pcm_hw_rule   *rule);
/**
 * ��Ƶ��ʽ������
 *
 * @param params PCMӲ������
 * @param rule   ����ṹ��
 *
 * @return ����1��ʾֵ�ı䣬����0��ʾֵû�ı�
 */
int hw_rule_format(struct uac_pcm_hw_params *params,
                   struct uac_pcm_hw_rule   *rule);
/**
 * ����ʱ�������
 *
 * @param params PCMӲ������
 * @param rule   ����ṹ��
 *
 * @return ����1��ʾֵ�ı䣬����0��ʾֵû�ı�
 */
int hw_rule_period_time(struct uac_pcm_hw_params *params,
                        struct uac_pcm_hw_rule   *rule);
/**
 * ���һ��USB��ƵPCMӲ�������ƹ���
 *
 * @param constrs PCMӲ����������
 * @param cond    ����λ
 * @param var     �����Ӳ������
 * @param func    ������
 * @param private ���ݸ��������Ĳ���
 * @param dep     ��������
 *
 * @return �ɹ�����USB_OK
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
 * ����PCMӲ��������С���ֵ
 *
 * @param constrs Ӳ��Լ��
 * @param var     ����Ӳ������
 * @param min     ��Сֵ
 * @param max     ���ֵ
 *
 * @return ����1��ʾֵ�ı䣬����0��ʾֵû�ı�
 */
int uac_pcm_hw_constraint_minmax(struct uac_pcm_hw_constraints *constrs,
                                 int                            var,
                                 uint32_t                       min,
                                 uint32_t                       max);
/**
 * ��ʼ��USB��ƵPCMӲ������
 *
 * @param constrs PCMӲ����������
 *
 * @return �ɹ�����USB_OK
 */
usb_err_t uac_pcm_hw_constraints_init(struct uac_pcm_hw_constraints *constrs);
/** ����PCMӲ����������*/
usb_err_t uac_pcm_hw_constraints_complete(struct uac_pcm_hw_constraints *constrs,
                                          struct uac_pcm_hardware       *hw,
                                          size_t                        *buffer_bytes_max);
/** ����usb��ƵӲ������*/
struct uac_pcm_hw_params *uac_get_hw_params(void *params_usr);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif


