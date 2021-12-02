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
#include "usb_errno.h"
#include "usb_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * 计算2的(v-1)次幂
 */
int ilog2(unsigned long v)
{
    int l = 0;
    while ((1UL << l) < v)
        l++;
    return l;
}

/**
 * 无符号64位除法
 *
 * @param dividend 被除数
 * @param divisor  除数
 *
 * @return 返回商
 */
uint64_t usb_div_u64(uint64_t dividend, uint32_t divisor){
    return dividend / divisor;
}

/**
 * 有符号64位除法，且返回余数
 *
 * @param dividend  被除数
 * @param divisor   除数
 * @param remainder 存储余数
 *
 * @return 返回商
 */
int64_t usb_div_s64_rem(int64_t dividend, int32_t divisor, int32_t *remainder){
    *remainder = dividend % divisor;
    return dividend / divisor;
}

/**
 * 从传入的地址开始，返回连续两个字节的值
 *
 * @param p 起始地址
 */
uint16_t get_unaligned_le16(uint8_t *p){
    if(p != NULL)
        return ((*(p + 1) << 8) | *p);
    return 0;
}

/**
 * 从传入的地址开始，返回连续四个字节的值
 *
 * @param p 起始地址
 */
uint32_t get_unaligned_le32(uint8_t *p){
    if(p != NULL)
        return ((*(p + 3) << 24) | (*(p + 2) << 16) | (*(p + 1) << 8) | *p);
    return 0;
}


/**
 * 检查内存区域指定的位是不是为1
 *
 * @param data 内存区域起始地址
 * @param bit  指定位
 *
 * @return 是返回1，否则为0
 */
int usb_test_bit(const uint8_t *data, int bit)
{
    return (data[bit >> 3] >> (bit & 7)) & 1;
}

/**
 * 清除内存区域指定的位
 *
 * @param data 内存区域起始地址
 * @param bit  指定位
 */
void usb_clear_bit(uint8_t *data, int bit)
{
    data[bit >> 3] &= ~(1 << (bit & 7));
}

/* 掩码定义*/
#define __UNICODE_MAX       0x0010ffff
#define __PLANE_SIZE        0x00010000

#define __SURROGATE_MASK    0xfffff800
#define __SURROGATE_PAIR    0x0000d800
#define __SURROGATE_LOW     0x00000400
#define __SURROGATE_BITS    0x000003ff

/* UTF8表结构体*/
struct __usb_utf8_tab {
    int     cmask;
    int     cval;
    int     shift;
    long    lmask;
    long    lval;
};

static const struct __usb_utf8_tab __gp_usb_utf8_tab[] =
{
    {0x80,  0x00,   0*6,    0x7F,           0,         /* 1 byte sequence */},
    {0xE0,  0xC0,   1*6,    0x7FF,          0x80,      /* 2 byte sequence */},
    {0xF0,  0xE0,   2*6,    0xFFFF,         0x800,     /* 3 byte sequence */},
    {0xF8,  0xF0,   3*6,    0x1FFFFF,       0x10000,   /* 4 byte sequence */},
    {0xFC,  0xF8,   4*6,    0x3FFFFFF,      0x200000,  /* 5 byte sequence */},
    {0xFE,  0xFC,   5*6,    0x7FFFFFFF,     0x4000000, /* 6 byte sequence */},
    {0,                                                /* end of table    */}
};

/** UTF8转换UTF32*/
static int __utf8_to_utf32 (const uint8_t *s,
                            int            len,
                            uint32_t      *pu)
{
    unsigned long                 l;
    int                           c0, c, nc;
    const struct __usb_utf8_tab  *t;

    nc = 0;
    c0 = *s;
    l = c0;

    for (t = __gp_usb_utf8_tab; t->cmask; t++) {
        nc++;
        if ((c0 & t->cmask) == t->cval) {

            l &= t->lmask;

            if (l < t->lval || l > __UNICODE_MAX ||
                (l & __SURROGATE_MASK) == __SURROGATE_PAIR) {
                return -1;
            }

            *pu = (uint32_t)l;
            return nc;
        }

        if (len <= nc) {
            return -1;
        }

        s++;
        c = (*s ^ 0x80) & 0xFF;

        if (c & 0xC0) {
            return -1;
        }

        l = (l << 6) | c;
    }
    return -1;
}

static int __utf32s_to_utf8s (uint32_t  u,
                              uint8_t  *s,
                              int       maxout)
{
    unsigned long                 l;
    int                           c, nc;
    const struct __usb_utf8_tab  *t;

    if (!s) {
        return 0;
    }

    l = u;
    if (l > __UNICODE_MAX ||
        (l & __SURROGATE_MASK) == __SURROGATE_PAIR) {
        return -1;
    }

    nc = 0;
    for (t = __gp_usb_utf8_tab; t->cmask && maxout; t++, maxout--) {
        nc++;
        if (l <= t->lmask) {

            c   =   t->shift;
            *s  =   (uint8_t) (t->cval | (l >> c));

            while (c > 0) {
                c -= 6;
                s++;
                *s = (uint8_t) (0x80 | ((l >> c) & 0x3F));
            }
            return nc;
        }
    }
    return -1;
}

int usb_utf16s_to_utf8s (const uint16_t  *pwcs,
                         int              inlen,
                         uint8_t         *str,
                         int              maxout)
{
    uint8_t         *op;
    int             size;
    unsigned long   u, v;

    op = str;
    while (inlen > 0 && maxout > 0) {
        u = USB_CPU_TO_LE16(*pwcs);
        if (!u) {
            break;
        }
        pwcs++;
        inlen--;
        if (u > 0x7f) {
            if ((u & __SURROGATE_MASK) == __SURROGATE_PAIR) {
                if (u & __SURROGATE_LOW) {
                    /* Ignore character and move on */
                    continue;
                }

                if (inlen <= 0) {
                    break;
                }

                v = USB_CPU_TO_LE16(*pwcs);
                if ((v & __SURROGATE_MASK) != __SURROGATE_PAIR ||
                        !(v & __SURROGATE_LOW)) {
                    /* Ignore character and move on */
                    continue;
                }
                u = __PLANE_SIZE + ((u & __SURROGATE_BITS) << 10)
                        + (v & __SURROGATE_BITS);
                pwcs++;
                inlen--;
            }

            size = __utf32s_to_utf8s(u, op, maxout);
            if (size == -1) {
                /* Ignore character and move on */
            } else {
                op += size;
                maxout -= size;
            }

        } else {
            *op++ = (uint8_t) u;
            maxout--;
        }
    }
    return op - str;
}

int usb_utf8s_to_utf16s (const uint8_t   *str,
                         int              len,
                         uint16_t        *pwcs)
{
    uint16_t  *op;
    int        size;
    uint32_t   u;

    op = pwcs;

    while (*str && len > 0) {
        if (*str & 0x80) {

            size = __utf8_to_utf32(str, len, &u);

            if (size < 0) {
                return -USB_EINVAL;
            }

            if (u >= __PLANE_SIZE) {
                u -= __PLANE_SIZE;
                *op++ = (uint16_t) (__SURROGATE_PAIR |
                        ((u >> 10) & __SURROGATE_BITS));
                *op++ = (uint16_t) (__SURROGATE_PAIR |
                        __SURROGATE_LOW |
                        (u & __SURROGATE_BITS));
            } else {
                *op++ = (uint16_t) u;
            }
            str += size;
            len -= size;
        } else {
            *op++ = *str++;
            len--;
        }
    }
    return op - pwcs;
}

/**
 * 将十六进制数字转换为其实际值
 *
 * @param ch ASCII字符表示十六进制数字
 *
 * @return 成功返回对应二进制值
 */
int usb_hex_to_bin_byte(char ch){
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    return -1;
}

/**
 * 将ASCII十六进制字符串转换为其二进制表示
 *
 * @param dst   二进制结果
 * @param src   ASCII十六进制字符串
 * @param count 结果长度
 *
 * @return 成功返回0
 */
int usb_hex_to_bin(uint8_t *dst, const char *src, size_t count){
    while (count--) {
        int hi = usb_hex_to_bin_byte(*src++);
        int lo = usb_hex_to_bin_byte(*src++);

        if ((hi < 0) || (lo < 0))
            return -1;

        *dst++ = (hi << 4) | lo;
    }
    return 0;
}
