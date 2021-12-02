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
#ifndef __USB_COMMON_H
#define __USB_COMMON_H

#include <stddef.h>
#include <stdint.h>

/** 空指针 */
#ifndef NULL
#define NULL       ((void *)0)
#endif
/* 布尔类型*/
typedef int             usb_bool_t;
#define USB_TRUE         (1)
#define USB_FALSE        (0)
/* 引用计数类型*/
typedef int             usb_refcnt;

#define USB_UINT_MAX     (~0U)
#define USB_INT_MAX      ((int)(~0U >> 1))
#define USB_LONG_MAX     ((long)(~0UL >> 1))
#define USB_INT_MIN      (-USB_INT_MAX - 1)
#define USB_ULONG_MAX    (~0UL)

/* 倍数向上舍入 */
#define USB_DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))
/* 倍数向上舍入 */
#define USB_ROUND_UP(x,n) (((x) + (n) - 1u) & ~((n) - 1u))

/**
 * \brief 获取2个数中的较大的数值
 * \param x 数字1
 * \param y 数字2
 * \return 2个数中的较大的数值
 * \note 参数不能使用++或--操作
 */
#ifndef max
#define max(x, y)   (((x) < (y)) ? (y) : (x))
#endif

/**
 * \brief 获取2个数中的较小的数值
 * \param x 数字1
 * \param y 数字2
 * \return 2个数中的较小数值
 * \note 参数不能使用++或--操作
 */
#ifndef min
#define min(x, y)   (((x) < (y)) ? (x) : (y))
#endif

/* 1秒等于纳秒数*/
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC           1000000000ULL
#endif

/* 定义大小端*/
#define USB_LITTLE_ENDIAN 1234       /**< \brief 目标机器小端 */
#define USB_BIG_ENDIAN    3412       /**< \brief 目标机器大端 */
#define CPU_ENDIAN USB_LITTLE_ENDIAN

/**
 * \brief USB字节序转换
 */
#if (CPU_ENDIAN == USB_BIG_ENDIAN)
#define USB_CPU_TO_LE32(x) \
            x = ((((x) & 0xff000000) >> 24) | \
             (((x) & 0x00ff0000) >> 8) | \
             (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

#define USB_CPU_TO_LE16(x) \
            x =(uint16_t) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
#else
#define USB_CPU_TO_LE32(x) (x)
#define USB_CPU_TO_LE16(x) (x)
#endif

/* 计算数组元素个数*/
#define USB_NELEMENTS(array) (sizeof (array) / sizeof ((array) [0]))
/* 对齐*/
#define USB_ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))

#define USB_CONTAINER_OF(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))

/**
 * \brief 通过结构体成员指针获取包含该结构体成员的结构体，
 *        同 \ref AW_CONTAINER_OF 一样
 */
#ifndef usb_container_of
#define usb_container_of(ptr, type, member)     USB_CONTAINER_OF(ptr, type, member)
#endif


#ifndef prefetch
#define prefetch(x) (void)0
#endif

/* 检查一个字节里有多少个1*/
#define usb_hweight8(w)         \
     ((unsigned int)         \
     ((!!((w) & (1ULL << 0))) + \
      (!!((w) & (1ULL << 1))) + \
      (!!((w) & (1ULL << 2))) + \
      (!!((w) & (1ULL << 3))) + \
      (!!((w) & (1ULL << 4))) + \
      (!!((w) & (1ULL << 5))) + \
      (!!((w) & (1ULL << 6))) + \
      (!!((w) & (1ULL << 7)))))

/*  四字符代码 (FOURCC) */
#define usb_fourcc(a, b, c, d)\
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#define usb_fourcc_be(a, b, c, d)  (v4l2_fourcc(a, b, c, d) | (1 << 31))

/* 约束边界值*/
#define usb_clamp(val, lo, hi)   min(max(val, lo), hi)

/** 计算2的(v-1)次幂*/
int ilog2(unsigned long v);
/**
 * 无符号64位除法
 *
 * @param dividend 被除数
 * @param divisor  除数
 *
 * @return 返回商
 */
uint64_t usb_div_u64(uint64_t dividend, uint32_t divisor);
/**
 * 有符号64位除法，且返回余数
 *
 * @param dividend  被除数
 * @param divisor   除数
 * @param remainder 存储余数
 *
 * @return 返回商
 */
int64_t usb_div_s64_rem(int64_t dividend, int32_t divisor, int32_t *remainder);
/**
 * 从传入的地址开始，返回连续两个字节的值
 *
 * @param p 起始地址
 */
uint16_t get_unaligned_le16(uint8_t *p);

/**
 * 从传入的地址开始，返回连续四个字节的值
 *
 * @param p 起始地址
 */
uint32_t get_unaligned_le32(uint8_t *p);

/**
 * 检查内存区域指定的位是不是为1
 *
 * @param data 内存区域起始地址
 * @param bit  指定位
 *
 * @return 是返回1，否则为0
 */
int usb_test_bit(const uint8_t *data, int bit);

/**
 * 清除内存区域指定的位
 *
 * @param data 内存区域起始地址
 * @param bit  指定位
 */
void usb_clear_bit(uint8_t *data, int bit);

/**
 * utf16编码字符串转utf8编码字符串
 *
 * @param  pwcs   utf16编码源字符串
 * @param  inlen  字符串长度
 * @param  str    转化的utf8编码字符串目标缓存
 * @param  maxout 最大转换长度
 *
 * @return 实际转换完成长度
 */
int usb_utf16s_to_utf8s (const uint16_t  *pwcs,
                         int              inlen,
                         uint8_t         *str,
                         int              maxout);


/**
 * utf8编码字符串转utf16编码字符串
 *
 * @param  str  utf8编码源字符串
 * @param  len  字符串字节长度
 * @param  pwcs 转化的utf16编码字符串目标缓存
 *
 * @return 实际转换的字节长度  或  -AW_EINVAL(无效参数)
 */
int usb_utf8s_to_utf16s(const uint8_t   *str,
                        int              len,
                        uint16_t        *pwcs);
/**
 * 将十六进制数字转换为其实际值
 *
 * @param ch ASCII字符表示十六进制数字
 *
 * @return 成功返回对应二进制值
 */
int usb_hex_to_bin_byte(char ch);
/**
 * 将ASCII十六进制字符串转换为其二进制表示
 *
 * @param dst   二进制结果
 * @param src   ASCII十六进制字符串
 * @param count 结果长度
 *
 * @return 成功返回0
 */
int usb_hex_to_bin(uint8_t *dst, const char *src, size_t count);
#endif






