/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief draw lcd api
 *
 * \internal
 * \par modification history:
 * - 1.00 21-04-13  swh, first implemetation
 * \endinternal
 */

#include "aworks.h"
#include "aw_ts.h"
#include "aw_fb.h"
#include <string.h>
#include <stdio.h>
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "lcd_draw.h"

/* ��ģ */
aw_local const uint8_t __g_lcd_words[] =
{0x00,0x00,0x00,0x18,0x3C,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,0x00,0x00,0x00, /*"A"*/
0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x6B,0x36,0x36,0x36,0x00,0x00,0x00,0x00,  /*"W"*/
0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00,  /*"o"*/
0x00,0x00,0x00,0x00,0x00,0x66,0x6E,0x70,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00,  /*"r"*/
0x00,0x00,0x00,0x60,0x60,0x66,0x66,0x6C,0x78,0x6C,0x66,0x66,0x00,0x00,0x00,0x00,  /*"k"*/
0x00,0x00,0x00,0x00,0x00,0x3E,0x60,0x60,0x3C,0x06,0x06,0x7C,0x00,0x00,0x00,0x00}; /*"s"*/

aw_local const uint8_t __g_lcd_nums[13][16] = {
{0x00,0x00,0x00,0x1E,0x33,0x37,0x37,0x33,0x3B,0x3B,0x33,0x1E,0x00,0x00,0x00,0x00},/*"0"*/
{0x00,0x00,0x00,0x0C,0x1C,0x7C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,0x00,0x00,0x00},/*"1"*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},/*"2"*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x1C,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"3"*/
{0x00,0x00,0x00,0x30,0x30,0x36,0x36,0x36,0x66,0x7F,0x06,0x06,0x00,0x00,0x00,0x00},/*"4"*/
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C,0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00},/*"5"*/
{0x00,0x00,0x00,0x1C,0x18,0x30,0x7C,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"6"*/
{0x00,0x00,0x00,0x7E,0x06,0x0C,0x0C,0x18,0x18,0x30,0x30,0x30,0x00,0x00,0x00,0x00},/*"7"*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x76,0x3C,0x6E,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"8"*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3E,0x0C,0x18,0x38,0x00,0x00,0x00,0x00},/*"9"*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},/*","*/
{0x00,0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00},/*"("*/
{0x00,0x00,0x00,0x30,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x18,0x30,0x00,0x00},/*")"*/
};

aw_local uint8_t __g_color_16bpp[][3] = {{0x1f, 0x00, 0x00},   //��ɫ
                                         {0x00, 0x3f, 0x00},   //��ɫ
                                         {0x00, 0x00, 0x1f},   //��ɫ
                                         {0x1f, 0x3f, 0x00},   //��ɫ
                                         {0x1f, 0x00, 0x1f},   //��ɫ
                                         {0x00, 0x3f, 0x1f},   //��ɫ
                                         {0x1f, 0x3f, 0x1f}};  //��ɫ
aw_local uint8_t __g_color_24bpp[][3] = {{0xff, 0x00, 0x00},   //��ɫ
                                         {0x00, 0xff, 0x00},   //��ɫ
                                         {0x00, 0x00, 0xff},   //��ɫ
                                         {0xff, 0xff, 0x00},   //��ɫ
                                         {0xff, 0x00, 0xff},   //��ɫ
                                         {0x00, 0xff, 0xff},   //��ɫ
                                         {0xff, 0xff, 0xff}};  //��ɫ

aw_local uint16_t __g_16bpp_color;         /* 16bpp������ɫ */
aw_local pixel_24bpp_t __g_24bpp_color;    /* 24bpp������ɫ */
/******************************************************************************/
/**
 * \brief ������������
 */
aw_local void __swap(int *p1, int *p2)
{
    int t;

    t = *p1;
    *p1 = *p2;
    *p2 = t;
}

/**
 * \brief ���û�����ɫ
 */
void set_pen_color (uint8_t r, uint8_t g, uint8_t b, int bpp)
{
    switch (bpp) {
    case DRAW_16BPP:
        __g_16bpp_color = (r << 11) | (g << 6) | b;
    break;
    case DRAW_24BPP:
        __g_24bpp_color.r = r;
        __g_24bpp_color.g = g;
        __g_24bpp_color.b = b;
    break;
    default:
        break;
    }
}

/**
 * \brief  ����һ����
 */
void draw_point (void *p_buf, int x, int y, int x_res, int bpp)
{
    uint16_t *p_fbuf = (uint16_t *)p_buf;
    pixel_24bpp_t* p = (pixel_24bpp_t*)p_buf+(y * x_res + x);
    switch (bpp) {
    case DRAW_16BPP:
        p_fbuf[y * x_res + x] = __g_16bpp_color;
    break;
    case DRAW_24BPP:
        p->r=__g_24bpp_color.r;
        p->g=__g_24bpp_color.g;
        p->b=__g_24bpp_color.b;
    break;
    default:
        break;
    }
}

/**
 * \brief ����һ������
 */
void draw_v_line (void *p_fb, int x, int y1, int y2)
{
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
    int i;
    if (y1 > y2) {
        __swap(&y1, &y2);
    }
    for (i = y1; i <= y2; i++) {
        draw_point((void *)fix_info.vram_addr,
                   x,
                   i,
                   var_info.res.x_res,
                   var_info.bpp.word_lbpp);
    }
}

/**
 * \brief ����һ������
 */
void draw_h_line (void *p_fb, int x1, int x2, int y)
{
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
    int i;
    if (x1 > x2) {
        __swap(&x1, &x2);
    }
    for (i = x1; i <= x2; i++) {
        draw_point((void *)fix_info.vram_addr,
                   i,
                   y,
                   var_info.res.x_res,
                   var_info.bpp.word_lbpp);
    }
}

/**
 * \brief ����һ������
 */
void draw_rect (void *p_fb, int x1, int x2, int y1, int y2)
{
    int i;

    if (x1 > x2) {
        __swap(&x1, &x2);
    }
    if (y1 > y2) {
        __swap(&y1, &y2);
    }

    for (i = y1; i <= y2; i++) {
        draw_h_line(p_fb, x1, x2, i);
    }
}

/**
 * \brief ����һ��Բ
 */
void draw_circle (void *p_fb, int x0, int y0, int pen_size)
{
    int x = 0;
    int y = 0;
    int x_min = 0;
    int y_min = 0;
    int x_max = 0;
    int y_max = 0;
    int r = 0;
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    if (pen_size == 1) {
        draw_point ((void *)fix_info.vram_addr,
                     x0,
                     y0,
                     var_info.res.x_res,
                     var_info.bpp.word_lbpp);
    } else if (pen_size < 1) {
        return;
    } else {
        r = pen_size/2;
    }

    x_min = x0 - r;
    x_max = x0 + r;
    y_min = y0 - r;
    y_max = y0 + r;

    for(y = y_min; y < y_max; y++) {
        for(x = x_min; x < x_max; x++) {
            if((x - x0)*(x - x0) + (y - y0)*(y - y0) <= r*r &&
                           x < var_info.res.x_res && x >= 0 &&
                           y < var_info.res.y_res && y >= 0) {
                draw_point ((void *)fix_info.vram_addr,
                             x,
                             y,
                             var_info.res.x_res,
                             var_info.bpp.word_lbpp);
            }
        }
    }

}

/**
 * \brief �����½ǵ����Ͻǻ���
 */
aw_local void __draw_compensation1 (void *p_fb, int x1, int y1,
                                    int x2,     int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 <= x2) {
            draw_circle(p_fb, x1, y1, size);
            if (e> 0) {     //�����½ǵ����Ͻǣ�0 <= ���� <= 45
                y1 += 1;
                e  -= dx;
            }
            x1 += 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 <= y2) {
            draw_circle(p_fb, x1, y1, size);
            if(e > 0) {     //�����½ǵ����Ͻǣ�45 < ���� <= 90
                x1 += 1;
                e  -= dy;
            }
            y1 += 1;
            e  += dx;
        }
    }

}

/**
 * \brief �����Ͻǵ����½ǻ���
 */
aw_local void __draw_compensation2 (void *p_fb, int x1, int y1,
                                    int x2,     int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 <= x2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {     //�����Ͻǵ����½ǣ�135 <= ���� < 180
                y1 -= 1;
                e  -= dx;
            }
            x1 += 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 >= y2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {     //�����Ͻǵ����½ǣ�90 <= ���� < 135
                x1 += 1;
                e  -= dy;
            }
            y1 -= 1;
            e  += dx;
        }
    }

}

/**
 * \brief �����½ǵ����Ͻǻ���
 */
aw_local void __draw_compensation3 (void *p_fb, int x1, int y1,
                                    int x2,     int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 >= x2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {    //�����½ǵ����Ͻǣ�135 <= ���� <= 180
                y1 += 1;
                e  -= dx;
            }
            x1 -= 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 <= y2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {    //�����½ǵ����Ͻǣ�90 < ���� < 135
                x1 -= 1;
                e  -= dy;
            }
            y1 += 1;
            e  += dx;
        }
    }

}

/**
 * \brief �����½ǵ����Ͻǻ���
 */
aw_local void __draw_compensation4 (void *p_fb, int x1, int y1,
                                    int x2,     int y2, int size)
{
    int dx, dy, e;

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx =- dx;
    }

    if (dy < 0) {
        dy =- dy;
    }

    if (dx >= dy) {
        e = dy - dx/2;
        while (x1 >= x2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {    //�����Ͻǵ����½ǣ�0 < ���� <= 45
                y1 -= 1;
                e  -= dx;
            }
            x1 -= 1;
            e  += dy;
        }
    } else {
        e = dx - dy/2;
        while (y1 >= y2) {
            draw_circle(p_fb, x1, y1, size);
            if (e > 0) {    //�����Ͻǵ����½ǣ�45 < ���� < 90
                x1 -= 1;
                e  -= dy;
            }
            y1 -= 1;
            e  += dx;
        }
    }

}

/**
 * \brief ����һ����������
 */
void draw_line (void *p_fb, int x1, int y1, int x2, int y2, int size)
{
    int dx, dy;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx >= 0) {
        if (dy >= 0) {
            __draw_compensation1 (p_fb, x1, y1, x2, y2, size);
        } else {
            __draw_compensation2 (p_fb, x1, y1, x2, y2, size);
        }
    } else {
        if (dy >= 0) {
            __draw_compensation3 (p_fb, x1, y1, x2, y2, size);
        } else {
            __draw_compensation4 (p_fb, x1, y1, x2, y2, size);
        }
    }
}

/**
 * \brief ��������
 */
void draw_words(void *p_fb, int x, int y, const uint8_t *p_words, int words_len)
{
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
    int i, j, k;
    for (i = 0; i < words_len / 16; i++) {
        for (j = 0; j < 16; j++) {
            for (k = 7; k >= 0; k--) {
                if ((p_words[i * 16 + j] >> k) & 1) {
                    draw_point((void *)fix_info.vram_addr,
                               x + (8 * i) + (7 - k),
                               y + j,
                               var_info.res.x_res,
                               var_info.bpp.word_lbpp);
                }
            }
        }
    }
}

/**
 * \brief ����x,yֵ
 */
void draw_xy(void *p_fb, int x, int y, int x_num, int y_num, uint8_t color[])
{
    int i;
    char buf[10] = {0};
    uint8_t xy_num[144] = {0};
    static int len = 0;
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    snprintf(buf, sizeof(buf), "(%d,%d)", x_num, y_num);

    /* ����ԭxyֵ */
    set_pen_color(0x00, 0x00, 0x00, var_info.bpp.word_lbpp);
    draw_rect(p_fb, x, x+8*len, y, y+20);
    set_pen_color(color[0], color[1], color[2], var_info.bpp.word_lbpp);

    len = strlen(buf);

    for(i=0; i<len; i++) {
        switch(buf[i]) {
        case '0': memcpy(xy_num+i*16, __g_lcd_nums[0],  16);break;
        case '1': memcpy(xy_num+i*16, __g_lcd_nums[1],  16);break;
        case '2': memcpy(xy_num+i*16, __g_lcd_nums[2],  16);break;
        case '3': memcpy(xy_num+i*16, __g_lcd_nums[3],  16);break;
        case '4': memcpy(xy_num+i*16, __g_lcd_nums[4],  16);break;
        case '5': memcpy(xy_num+i*16, __g_lcd_nums[5],  16);break;
        case '6': memcpy(xy_num+i*16, __g_lcd_nums[6],  16);break;
        case '7': memcpy(xy_num+i*16, __g_lcd_nums[7],  16);break;
        case '8': memcpy(xy_num+i*16, __g_lcd_nums[8],  16);break;
        case '9': memcpy(xy_num+i*16, __g_lcd_nums[9],  16);break;
        case ',': memcpy(xy_num+i*16, __g_lcd_nums[10], 16);break;
        case '(': memcpy(xy_num+i*16, __g_lcd_nums[11], 16);break;
        case ')': memcpy(xy_num+i*16, __g_lcd_nums[12], 16);break;
        default : break;
        }
    }

    draw_words(p_fb, x, y, xy_num, len*16);
}

/**
 * \brief ���ƹ�꣨���׹�꣩
 */
void draw_cursor (void *p_fb, int x, int y, int size)
{
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
    set_pen_color(0xff, 0xff, 0xff, var_info.bpp.word_lbpp);
    draw_h_line(p_fb, x - size, x + size, y);
    draw_v_line(p_fb, x, y - size, y + size);
}

/**
 * \brief �������꣩����Ĳ�����Ҫ�ͻ��ƹ��ʱ�����һģһ��
 */
void clear_cursor (void *p_fb, int x, int y, int size)
{
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);
    set_pen_color(0, 0, 0, var_info.bpp.word_lbpp);
    draw_h_line(p_fb, x - size, x + size, y);
    draw_v_line(p_fb, x, y - size, y + size);
}

/******************************************************************************/
/**
 * \brief ��LCDͼ��
 */
void draw_lcd (void *p_fb)
{
    int i, ylen;

    /*frame buffer �豸��Ϣ����*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_info);

    memset((void *)fix_info.vram_addr,
            0,
            var_info.buffer.buffer_size * var_info.buffer.buffer_num);

    switch (var_info.bpp.word_lbpp) {
    case DRAW_16BPP:
        /* ����һ������ */
        set_pen_color(0x1f, 0, 0, var_info.bpp.word_lbpp);
        draw_v_line(p_fb, var_info.res.x_res / 8 * 7, 0, var_info.res.y_res);
        /* ����һ������ */
        set_pen_color(0, 0x3f, 0, var_info.bpp.word_lbpp);
        draw_h_line(p_fb, 0, var_info.res.x_res / 4, var_info.res.y_res / 2);
        /* ���Ʋ��� */
        ylen = var_info.res.y_res / AW_NELEMENTS(__g_color_16bpp);
        for (i = 0; i < AW_NELEMENTS(__g_color_16bpp); i++) {
            set_pen_color(__g_color_16bpp[i][0],
                          __g_color_16bpp[i][1],
                          __g_color_16bpp[i][2],
                          var_info.bpp.word_lbpp);
            draw_rect(p_fb,
                      var_info.res.x_res / 4,
                      var_info.res.x_res / 4 * 3,
                      ylen * i,
                      ylen * (i + 1));
        }
        break;
    case DRAW_24BPP:
        /* ����һ������ */
        set_pen_color(0xff, 0, 0, var_info.bpp.word_lbpp);
        draw_v_line(p_fb, var_info.res.x_res / 8 * 7, 0, var_info.res.y_res);
        /* ����һ������ */
        set_pen_color(0, 0xff, 0, var_info.bpp.word_lbpp);
        draw_h_line(p_fb, 0, var_info.res.x_res / 4, var_info.res.y_res / 2);
        /* ���Ʋ��� */
        ylen = var_info.res.y_res / AW_NELEMENTS(__g_color_24bpp);
        for (i = 0; i < AW_NELEMENTS(__g_color_24bpp); i++) {
            set_pen_color(__g_color_24bpp[i][0],
                          __g_color_24bpp[i][1],
                          __g_color_24bpp[i][2],
                          var_info.bpp.word_lbpp);
            draw_rect(p_fb,
                      var_info.res.x_res / 4,
                      var_info.res.x_res / 4 * 3,
                      ylen * i,
                      ylen * (i + 1));
        }
        break;
    default:
        break;
    }
    /* �������� */
    draw_words(p_fb, 0, 0, __g_lcd_words, sizeof(__g_lcd_words));
}



