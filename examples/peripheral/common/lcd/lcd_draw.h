/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
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

#ifndef __LCD_DRAW_H
#define __LCD_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "aw_errno.h"

/* 16位域深度 */
#define DRAW_16BPP 16
/* 24位域深度 */
#define DRAW_24BPP 24

/*
 * 软件24bpp像素结构
 */
typedef struct pixel_24bpp {
  uint8_t b;
  uint8_t g;
  uint8_t r;
} pixel_24bpp_t;

/**
 * \brief  设置画笔颜色
 *
 * \param[in] r   : R的值
 * \param[in] g   : G的值
 * \param[in] b   : B的值
 * \param[in] bpp : 软件位深度
 */
void set_pen_color (uint8_t r, uint8_t g, uint8_t b, int bpp);

/**
 * \brief  绘制一个点
 *
 * \param[in] ts_name : 设备名
 * \param[in] x       : x轴位置
 * \param[in] y       : y轴位置
 * \param[in] x_res   : 屏的最大宽度
 * \param[in] bpp     : 软件位深度
 */
void draw_point (void *p_buf, int x, int y, int x_res, int bpp);

/**
 * \brief 绘制一条竖线
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x         横向位置
 * \param[in] y1        纵向起始位置
 * \param[in] y2        纵向结束位置
 */
void draw_v_line (void *p_fb, int x, int y1, int y2);

/**
 * \brief 绘制一条横线
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x1        横向起始位置
 * \param[in] x2        横向结束位置
 * \param[in] y         纵向位置
 */
void draw_h_line (void *p_fb, int x1, int x2, int y);

/**
 * \brief 绘制一个矩形
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x1        横向起始位置
 * \param[in] x2        横向结束位置
 * \param[in] y1        纵向起始位置
 * \param[in] y2        纵向结束位置
 */
void draw_rect (void *p_fb, int x1, int x2, int y1, int y2);

/**
 * \brief 绘制一个圆
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x0        横向圆心位置
 * \param[in] y0        纵向圆心位置
 * \param[in] size      pen_size
 */
void draw_circle (void *p_fb, int x0, int y0, int pen_size);

/**
 * \brief 绘制一条连续的线
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x1        横向起始位置
 * \param[in] y1        纵向起始位置
 * \param[in] x2        横向结束位置
 * \param[in] y2        纵向结束位置
 * \param[in] size      画笔大小
 */
void draw_line (void *p_fb, int x1, int y1, int x2, int y2, int size);

/**
 * \brief 绘制文字
 *
 * \param[in] p_fb      帧缓冲设备信息
 * \param[in] x         横向位置
 * \param[in] y         纵向位置
 * \param[in] p_words   字模指针
 * \param[in] words_len 字模长度
 */
void draw_words(void *p_fb, int x, int y, const uint8_t *p_words, int words_len);

/**
 * \brief 绘制x,y值
 *
 * \param[in] p_fb   帧缓冲设备信息
 * \param[in] x      横向位置
 * \param[in] y      纵向位置
 * \param[in] x_num  横向坐标值
 * \param[in] y_num  纵向坐标值
 * \param[in] color  颜色数据
 */
void draw_xy(void *p_fb, int x, int y, int x_num, int y_num, uint8_t color[]);

/**
 * \brief 绘制光标（简易光标）
 *
 * \param[in] p_fb   帧缓冲设备信息
 * \param[in] x      横向位置
 * \param[in] y      纵向位置
 * \param[in] size   光标大小
 */
void draw_cursor (void *p_fb, int x, int y, int size);

/**
 * \brief （清除光标）传入的参数需要和绘制光标时传入的一模一样
 *
 * \param[in] p_fb   帧缓冲设备信息
 * \param[in] x      横向位置
 * \param[in] y      纵向位置
 * \param[in] size   光标大小
 */
void clear_cursor (void *p_fb, int x, int y, int size);

/**
 * \brief 画LCD图形
 *
 * \param[in] p_fb   帧缓冲设备信息
 */
void draw_lcd (void *p_fb);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __LCD_DRAW_H */

/* end of file */
