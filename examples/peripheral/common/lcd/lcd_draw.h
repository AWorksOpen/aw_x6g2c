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

/* 16λ����� */
#define DRAW_16BPP 16
/* 24λ����� */
#define DRAW_24BPP 24

/*
 * ���24bpp���ؽṹ
 */
typedef struct pixel_24bpp {
  uint8_t b;
  uint8_t g;
  uint8_t r;
} pixel_24bpp_t;

/**
 * \brief  ���û�����ɫ
 *
 * \param[in] r   : R��ֵ
 * \param[in] g   : G��ֵ
 * \param[in] b   : B��ֵ
 * \param[in] bpp : ���λ���
 */
void set_pen_color (uint8_t r, uint8_t g, uint8_t b, int bpp);

/**
 * \brief  ����һ����
 *
 * \param[in] ts_name : �豸��
 * \param[in] x       : x��λ��
 * \param[in] y       : y��λ��
 * \param[in] x_res   : ���������
 * \param[in] bpp     : ���λ���
 */
void draw_point (void *p_buf, int x, int y, int x_res, int bpp);

/**
 * \brief ����һ������
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x         ����λ��
 * \param[in] y1        ������ʼλ��
 * \param[in] y2        �������λ��
 */
void draw_v_line (void *p_fb, int x, int y1, int y2);

/**
 * \brief ����һ������
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x1        ������ʼλ��
 * \param[in] x2        �������λ��
 * \param[in] y         ����λ��
 */
void draw_h_line (void *p_fb, int x1, int x2, int y);

/**
 * \brief ����һ������
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x1        ������ʼλ��
 * \param[in] x2        �������λ��
 * \param[in] y1        ������ʼλ��
 * \param[in] y2        �������λ��
 */
void draw_rect (void *p_fb, int x1, int x2, int y1, int y2);

/**
 * \brief ����һ��Բ
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x0        ����Բ��λ��
 * \param[in] y0        ����Բ��λ��
 * \param[in] size      pen_size
 */
void draw_circle (void *p_fb, int x0, int y0, int pen_size);

/**
 * \brief ����һ����������
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x1        ������ʼλ��
 * \param[in] y1        ������ʼλ��
 * \param[in] x2        �������λ��
 * \param[in] y2        �������λ��
 * \param[in] size      ���ʴ�С
 */
void draw_line (void *p_fb, int x1, int y1, int x2, int y2, int size);

/**
 * \brief ��������
 *
 * \param[in] p_fb      ֡�����豸��Ϣ
 * \param[in] x         ����λ��
 * \param[in] y         ����λ��
 * \param[in] p_words   ��ģָ��
 * \param[in] words_len ��ģ����
 */
void draw_words(void *p_fb, int x, int y, const uint8_t *p_words, int words_len);

/**
 * \brief ����x,yֵ
 *
 * \param[in] p_fb   ֡�����豸��Ϣ
 * \param[in] x      ����λ��
 * \param[in] y      ����λ��
 * \param[in] x_num  ��������ֵ
 * \param[in] y_num  ��������ֵ
 * \param[in] color  ��ɫ����
 */
void draw_xy(void *p_fb, int x, int y, int x_num, int y_num, uint8_t color[]);

/**
 * \brief ���ƹ�꣨���׹�꣩
 *
 * \param[in] p_fb   ֡�����豸��Ϣ
 * \param[in] x      ����λ��
 * \param[in] y      ����λ��
 * \param[in] size   ����С
 */
void draw_cursor (void *p_fb, int x, int y, int size);

/**
 * \brief �������꣩����Ĳ�����Ҫ�ͻ��ƹ��ʱ�����һģһ��
 *
 * \param[in] p_fb   ֡�����豸��Ϣ
 * \param[in] x      ����λ��
 * \param[in] y      ����λ��
 * \param[in] size   ����С
 */
void clear_cursor (void *p_fb, int x, int y, int size);

/**
 * \brief ��LCDͼ��
 *
 * \param[in] p_fb   ֡�����豸��Ϣ
 */
void draw_lcd (void *p_fb);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __LCD_DRAW_H */

/* end of file */
