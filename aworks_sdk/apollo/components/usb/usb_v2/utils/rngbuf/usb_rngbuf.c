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
 * 初始化环形缓冲区
 *
 * @param p_rb   要初始化的环形缓冲区
 * @param p_buf  缓存
 * @param size   缓存大小
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
 * 往环形缓冲区写入数据
 *
 * @param rb     环形缓冲区
 * @param p_buf  要写入的数据缓存
 * @param nbytes 要写入的数据字节数
 *
 * @return 成功返回写入的字节数
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

    /* 写标记在环形头，读标记在环形尾*/
    if (in < out) {
        /* 计算写和读的距离*/
        n = out - in - 1;
        /* 如果距离大于要写入的数据量，那可以写入全部数据
         * 否则只能写入距离字节数*/
        if (n > nbytes) {
            n = nbytes;
        }
        /* 复制数据*/
        memcpy(&rb->buf[in],p_buf,n);
        /* 计算放入的字节数*/
        bytes_put += n;
    }
    else {
        /* 此种情况，数据不能一次性复制完成*/
        /* 第一次复制*/
        /* 计算写和读的距离*/
        n = out + size -in - 1;
        /* 如果距离大于要写入的数据量，那可以写入全部数据
         * 否则只能写入距离字节数*/
        if (n > nbytes) {
            n = nbytes;
        }
        /* 回写到环形末尾，分两部分写*/
        if (n + in > size) {
            n = size - in ;
        }
        memcpy(&rb->buf[in],p_buf,n);
        bytes_put += n;

        /* 第二次复制*/
        /* 剩下没写的数据*/
        n = nbytes - bytes_put;
        /* 读标志在环形开头，不能继续写*/
        if (out == 0) {
            n = 0;
        }
        else if (n > out - 1) {
            /* 不能写超过读标志*/
            n = out - 1;
        }
        if (n > 0) {
            memcpy(&rb->buf[0],p_buf + bytes_put,n);
            bytes_put += n;
        }
    }
    /* 重新设置写标记*/
    in = (in + bytes_put);
    /* 到了环形末尾，回到环形头*/
    if (in >= size) {
        in -= size;
    }
    rb->in = in;
    /* 返回写入的字节数*/
    return (bytes_put);
}

/**
 * 从环形缓冲区读数据
 *
 * @param rb     环形缓冲区
 * @param p_buf  要读的数据缓存
 * @param nbytes 要读的数据字节数
 *
 * @return 成功返回读的字节数
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
    /* 读标记在环形头，写标记在环形尾*/
    if (in >= out) {
        /* 计算写和读的距离*/
        n = in - out;
        /* 如果距离大于要读出的数据量，那可以读全部数据
         * 否则只能读距离字节数*/
        if (n > nbytes) {
            n = nbytes;
        }
        memcpy(p_buf, &rb->buf[out], n);
        /* 计算读出的字节数*/
        bytes_got += n;
    } else {
        /*
         * in has wrapped around.  Grab chars up to the size of the
         * buffer, then wrap around if we need to.
         */
        /* 计算读标记到环形尾的距离*/
        n = size - out;
        /* 如果距离大于要写入的数据量，那可以写入全部数据
         * 否则只能写入距离字节数*/
        if (n > nbytes) {
            n = nbytes;
        }

        memcpy(p_buf, &rb->buf[out], n);
        bytes_got += n;
        /* 计算环形头到写标记的距离*/
        n = in;
        /* 如果距离大于要写入的数据量，那可以写入全部数据
         * 否则只能写入距离字节数*/
        if (n > (nbytes - bytes_got)) {
            n = nbytes - bytes_got;
        }
        if (n > 0) {
            memcpy(p_buf + bytes_got, &rb->buf[0], n);
            bytes_got += n;
        }
    }
    /* 重新设置读标记*/
    out = out + bytes_got;
    /* 到了环形末尾，回到环形头*/
    if (out >= size) {
        out -= size;
    }

    rb->out = out;
    /* 返回读的字节数*/
    return (bytes_got);
}

