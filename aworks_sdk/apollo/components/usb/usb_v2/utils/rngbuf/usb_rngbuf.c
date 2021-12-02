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
#include "usb_common.h"
#include "rngbuf/usb_rngbuf.h"
#include "string.h"

/**
 * ��ʼ�����λ�����
 *
 * @param p_rb   Ҫ��ʼ���Ļ��λ�����
 * @param p_buf  ����
 * @param size   �����С
 */
void usb_rngbuf_init (struct usb_rngbuf *p_rb, char *p_buf, uint32_t size)
{
    if((p_buf == NULL) || (p_rb == NULL)){
        return;
    }

    p_rb->in   = 0;
    p_rb->out  = 0;
    p_rb->buf  = p_buf;
    p_rb->size = size > 0 ? size : 1;
}

/**
 * �����λ�����д������
 *
 * @param rb     ���λ�����
 * @param p_buf  Ҫд������ݻ���
 * @param nbytes Ҫд��������ֽ���
 *
 * @return �ɹ�����д����ֽ���
 */
uint32_t usb_rngbuf_put (struct usb_rngbuf *rb, const char *p_buf, uint32_t nbytes)
{
    uint32_t    in;
    uint32_t    out;
    int         size;
    int         bytes_put = 0;
    int         n;

    if((rb == NULL) || (rb->buf == NULL) || (rb->size <= 0)){
        return 0;
    }

    in = rb->in;
    out = rb->out;
    size = rb->size;

    /* д����ڻ���ͷ��������ڻ���β*/
    if (in < out) {
        /* ����д�Ͷ��ľ���*/
        n = out - in - 1;
        /* ����������Ҫд������������ǿ���д��ȫ������
         * ����ֻ��д������ֽ���*/
        if (n > nbytes) {
            n = nbytes;
        }
        /* ��������*/
        memcpy(&rb->buf[in],p_buf,n);
        /* ���������ֽ���*/
        bytes_put += n;
    }
    else {
        /* ������������ݲ���һ���Ը������*/
        /* ��һ�θ���*/
        /* ����д�Ͷ��ľ���*/
        n = out + size -in - 1;
        /* ����������Ҫд������������ǿ���д��ȫ������
         * ����ֻ��д������ֽ���*/
        if (n > nbytes) {
            n = nbytes;
        }
        /* ��д������ĩβ����������д*/
        if (n + in > size) {
            n = size - in ;
        }
        memcpy(&rb->buf[in],p_buf,n);
        bytes_put += n;

        /* �ڶ��θ���*/
        /* ʣ��ûд������*/
        n = nbytes - bytes_put;
        /* ����־�ڻ��ο�ͷ�����ܼ���д*/
        if (out == 0) {
            n = 0;
        }
        else if (n > out - 1) {
            /* ����д��������־*/
            n = out - 1;
        }
        if (n > 0) {
            memcpy(&rb->buf[0],p_buf + bytes_put,n);
            bytes_put += n;
        }
    }
    /* ��������д���*/
    in = (in + bytes_put);
    /* ���˻���ĩβ���ص�����ͷ*/
    if (in >= size) {
        in -= size;
    }
    rb->in = in;
    /* ����д����ֽ���*/
    return (bytes_put);
}

/**
 * �ӻ��λ�����������
 *
 * @param rb     ���λ�����
 * @param p_buf  Ҫ�������ݻ���
 * @param nbytes Ҫ���������ֽ���
 *
 * @return �ɹ����ض����ֽ���
 */
uint32_t usb_rngbuf_get (struct usb_rngbuf *rb, char *p_buf, uint32_t nbytes)
{
    uint32_t    in;
    uint32_t    out;
    int         size;
    int         bytes_got = 0;
    int         n;

    if((rb == NULL) || (rb->buf == NULL) || (rb->size <= 0)){
        return 0;
    }

    in = rb->in;
    out = rb->out;
    size = rb->size;
    /* ������ڻ���ͷ��д����ڻ���β*/
    if (in >= out) {
        /* ����д�Ͷ��ľ���*/
        n = in - out;
        /* ����������Ҫ���������������ǿ��Զ�ȫ������
         * ����ֻ�ܶ������ֽ���*/
        if (n > nbytes) {
            n = nbytes;
        }
        memcpy(p_buf, &rb->buf[out], n);
        /* ����������ֽ���*/
        bytes_got += n;
    } else {
        /*
         * in has wrapped around.  Grab chars up to the size of the
         * buffer, then wrap around if we need to.
         */
        /* �������ǵ�����β�ľ���*/
        n = size - out;
        /* ����������Ҫд������������ǿ���д��ȫ������
         * ����ֻ��д������ֽ���*/
        if (n > nbytes) {
            n = nbytes;
        }

        memcpy(p_buf, &rb->buf[out], n);
        bytes_got += n;
        /* ���㻷��ͷ��д��ǵľ���*/
        n = in;
        /* ����������Ҫд������������ǿ���д��ȫ������
         * ����ֻ��д������ֽ���*/
        if (n > (nbytes - bytes_got)) {
            n = nbytes - bytes_got;
        }
        if (n > 0) {
            memcpy(p_buf + bytes_got, &rb->buf[0], n);
            bytes_got += n;
        }
    }
    /* �������ö����*/
    out = out + bytes_got;
    /* ���˻���ĩβ���ص�����ͷ*/
    if (out >= size) {
        out -= size;
    }

    rb->out = out;
    /* ���ض����ֽ���*/
    return (bytes_got);
}

