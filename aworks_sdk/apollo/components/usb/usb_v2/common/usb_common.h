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

/** ��ָ�� */
#ifndef NULL
#define NULL       ((void *)0)
#endif
/* ��������*/
typedef int             usb_bool_t;
#define USB_TRUE         (1)
#define USB_FALSE        (0)
/* ���ü�������*/
typedef int             usb_refcnt;

#define USB_UINT_MAX     (~0U)
#define USB_INT_MAX      ((int)(~0U >> 1))
#define USB_LONG_MAX     ((long)(~0UL >> 1))
#define USB_INT_MIN      (-USB_INT_MAX - 1)
#define USB_ULONG_MAX    (~0UL)

/* ������������ */
#define USB_DIV_ROUND_UP(n, d)   (((n) + (d) - 1) / (d))
/* ������������ */
#define USB_ROUND_UP(x,n) (((x) + (n) - 1u) & ~((n) - 1u))

/**
 * \brief ��ȡ2�����еĽϴ����ֵ
 * \param x ����1
 * \param y ����2
 * \return 2�����еĽϴ����ֵ
 * \note ��������ʹ��++��--����
 */
#ifndef max
#define max(x, y)   (((x) < (y)) ? (y) : (x))
#endif

/**
 * \brief ��ȡ2�����еĽ�С����ֵ
 * \param x ����1
 * \param y ����2
 * \return 2�����еĽ�С��ֵ
 * \note ��������ʹ��++��--����
 */
#ifndef min
#define min(x, y)   (((x) < (y)) ? (x) : (y))
#endif

/* 1�����������*/
#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC           1000000000ULL
#endif

/* �����С��*/
#define USB_LITTLE_ENDIAN 1234       /**< \brief Ŀ�����С�� */
#define USB_BIG_ENDIAN    3412       /**< \brief Ŀ�������� */
#define CPU_ENDIAN USB_LITTLE_ENDIAN

/**
 * \brief USB�ֽ���ת��
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

/* ��������Ԫ�ظ���*/
#define USB_NELEMENTS(array) (sizeof (array) / sizeof ((array) [0]))
/* ����*/
#define USB_ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))

#define USB_CONTAINER_OF(ptr, type, member) \
        ((type *)((char *)(ptr) - offsetof(type,member)))

/**
 * \brief ͨ���ṹ���Աָ���ȡ�����ýṹ���Ա�Ľṹ�壬
 *        ͬ \ref AW_CONTAINER_OF һ��
 */
#ifndef usb_container_of
#define usb_container_of(ptr, type, member)     USB_CONTAINER_OF(ptr, type, member)
#endif


#ifndef prefetch
#define prefetch(x) (void)0
#endif

/* ���һ���ֽ����ж��ٸ�1*/
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

/*  ���ַ����� (FOURCC) */
#define usb_fourcc(a, b, c, d)\
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))
#define usb_fourcc_be(a, b, c, d)  (v4l2_fourcc(a, b, c, d) | (1 << 31))

/* Լ���߽�ֵ*/
#define usb_clamp(val, lo, hi)   min(max(val, lo), hi)

/** ����2��(v-1)����*/
int ilog2(unsigned long v);
/**
 * �޷���64λ����
 *
 * @param dividend ������
 * @param divisor  ����
 *
 * @return ������
 */
uint64_t usb_div_u64(uint64_t dividend, uint32_t divisor);
/**
 * �з���64λ�������ҷ�������
 *
 * @param dividend  ������
 * @param divisor   ����
 * @param remainder �洢����
 *
 * @return ������
 */
int64_t usb_div_s64_rem(int64_t dividend, int32_t divisor, int32_t *remainder);
/**
 * �Ӵ���ĵ�ַ��ʼ���������������ֽڵ�ֵ
 *
 * @param p ��ʼ��ַ
 */
uint16_t get_unaligned_le16(uint8_t *p);

/**
 * �Ӵ���ĵ�ַ��ʼ�����������ĸ��ֽڵ�ֵ
 *
 * @param p ��ʼ��ַ
 */
uint32_t get_unaligned_le32(uint8_t *p);

/**
 * ����ڴ�����ָ����λ�ǲ���Ϊ1
 *
 * @param data �ڴ�������ʼ��ַ
 * @param bit  ָ��λ
 *
 * @return �Ƿ���1������Ϊ0
 */
int usb_test_bit(const uint8_t *data, int bit);

/**
 * ����ڴ�����ָ����λ
 *
 * @param data �ڴ�������ʼ��ַ
 * @param bit  ָ��λ
 */
void usb_clear_bit(uint8_t *data, int bit);

/**
 * utf16�����ַ���תutf8�����ַ���
 *
 * @param  pwcs   utf16����Դ�ַ���
 * @param  inlen  �ַ�������
 * @param  str    ת����utf8�����ַ���Ŀ�껺��
 * @param  maxout ���ת������
 *
 * @return ʵ��ת����ɳ���
 */
int usb_utf16s_to_utf8s (const uint16_t  *pwcs,
                         int              inlen,
                         uint8_t         *str,
                         int              maxout);


/**
 * utf8�����ַ���תutf16�����ַ���
 *
 * @param  str  utf8����Դ�ַ���
 * @param  len  �ַ����ֽڳ���
 * @param  pwcs ת����utf16�����ַ���Ŀ�껺��
 *
 * @return ʵ��ת�����ֽڳ���  ��  -AW_EINVAL(��Ч����)
 */
int usb_utf8s_to_utf16s(const uint8_t   *str,
                        int              len,
                        uint16_t        *pwcs);
/**
 * ��ʮ����������ת��Ϊ��ʵ��ֵ
 *
 * @param ch ASCII�ַ���ʾʮ����������
 *
 * @return �ɹ����ض�Ӧ������ֵ
 */
int usb_hex_to_bin_byte(char ch);
/**
 * ��ASCIIʮ�������ַ���ת��Ϊ������Ʊ�ʾ
 *
 * @param dst   �����ƽ��
 * @param src   ASCIIʮ�������ַ���
 * @param count �������
 *
 * @return �ɹ�����0
 */
int usb_hex_to_bin(uint8_t *dst, const char *src, size_t count);
#endif






