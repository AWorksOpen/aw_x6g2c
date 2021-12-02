
#include "awbl_imx6ul_pxp.h"
#include "base/g2d.h"
#include "stdint.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_fb.h"
#include "base/pixel.h"
#include "imx6ul_pxp_regs.h"


ret_t g2d_fill_rect(bitmap_t* fb, const rect_t* dst, color_t c)
{
    uint8_t* fb_data = NULL;

    //暂时不用，效率不及软件实现
    return RET_NOT_IMPL;
    return_value_if_fail(fb != NULL && dst != NULL, RET_BAD_PARAMS);
    return_value_if_fail(c.rgba.a > 0xf8, RET_NOT_IMPL);
    return_value_if_fail(fb->format == BITMAP_FMT_BGR565, RET_NOT_IMPL);
    return_value_if_fail(dst->w != 0 && dst->h != 0, RET_BAD_PARAMS);

    awbl_pxp_output_buf_cfg_t pxp_output_config = {0};
    awbl_pxp_ps_buf_cfg_t     ps_buffer_config = {0};

    uint32_t out_addr = 0;
    uint8_t  out_pixsize = 2;

    /* 复位pxp */
    awbl_pxp_hard_reset();

    fb_data = bitmap_lock_buffer_for_write(fb);

    /* 计算PS起始地址 */
    out_addr = (uint32_t)fb_data + (fb->w * dst->y + dst->x) * out_pixsize;
    uint32_t fb_flush_size = ((dst->h - 1) * fb->w + dst->w) * out_pixsize;
    aw_cache_flush((void *)out_addr,  fb_flush_size);

    /* 输出缓冲配置 */
    pxp_output_config.pixel_fmt = PXP_OUTPUTPIXEL_RGB565;
    pxp_output_config.output_mode = PXP_OUTPUTMODE_PROGRESSIVE;
    pxp_output_config.buf0_addr = out_addr;
    pxp_output_config.pitch_bytes = fb->w * out_pixsize;
    pxp_output_config.width  = dst->w;
    pxp_output_config.height = dst->h;
    awbl_pxp_set_output_buffer_config(&pxp_output_config);

    /* 配置PS buffer */
    ps_buffer_config.pixel_fmt = PXP_OUTPUTPIXEL_RGB888;
    ps_buffer_config.buf_addr  = out_addr;
    ps_buffer_config.pitch_bytes  = fb->w * out_pixsize;
    awbl_pxp_set_ps_buffer_config(&ps_buffer_config);
    awbl_pxp_set_ps_position(0, 0, dst->w - 1, dst->h - 1);
    awbl_pxp_set_as_position(1, 1, 0, 0);

    awbl_pxp_set_ps_color_key(0, 0xFFFFFF);

    /* 设置填充的颜色 */
    awbl_pxp_set_ps_background_color((uint32_t)c.color & (0xFFFFFF));
    awbl_pxp_start();

    /* Wait for process complete. */
    awbl_pxp_complete_status_sync();
    aw_cache_invalidate((void *)out_addr, fb_flush_size);

    bitmap_unlock_buffer(fb);
    return RET_OK;
}

ret_t g2d_copy_image(bitmap_t* fb, bitmap_t* img, const rect_t* src, xy_t dx, xy_t dy)
{
    //暂时不用，效率不及软件实现
    return RET_NOT_IMPL;
}

ret_t g2d_blend_image(bitmap_t* fb, bitmap_t* img, const rect_t* dst, const rect_t* src, uint8_t global_alpha)
{
    uint8_t* fb_data = NULL;
    uint8_t* img_data = NULL;

    return_value_if_fail((fb        != NULL) &&                 \
                         (fb->buffer  != NULL), RET_BAD_PARAMS);
    return_value_if_fail((img       != NULL) &&                 \
                         (img->buffer != NULL) &&                 \
                         (src       != NULL) &&                 \
                         (dst       != NULL), RET_BAD_PARAMS);

    return_value_if_fail((fb->format  == BITMAP_FMT_BGR565) ||  \
                         (fb->format  == BITMAP_FMT_BGRA8888), RET_BAD_PARAMS);
    return_value_if_fail((img->format == BITMAP_FMT_BGR565) ||  \
                         (img->format == BITMAP_FMT_BGRA8888), RET_BAD_PARAMS);

    /* 硬件不支持BGRA8888数据的缩放 */
    if (((src->w != dst->w) || (src->h != dst->h)) &&
         (img->format == BITMAP_FMT_BGRA8888)) {

        return RET_NOT_IMPL;
    }

    uint16_t as_format   = 0;
    uint16_t as_pixsize  = 0;
    uint16_t ps_format   = 0;
    uint16_t ps_pixsize  = 0;
    uint16_t out_format  = 0;
    uint16_t out_pixsize = 0;

    uint16_t as_x        = 0;   /* AS的起始坐标点x */
    uint16_t as_y        = 0;   /* AS的起始坐标点y */
    uint16_t as_w        = 0;   /* 位图的宽度 */
    uint32_t as_addr     = 0;   /* AS的起始地址 */

    uint16_t ps_x        = 0;
    uint16_t ps_y        = 0;
    uint16_t ps_w        = 0;
    uint32_t ps_addr     = 0;
    uint32_t out_addr    = 0;

    uint8_t *p_scale_buf = NULL;

    awbl_pxp_output_buf_cfg_t pxp_output_config = {0};
    awbl_pxp_ps_buf_cfg_t     ps_buffer_config  = {0};

    if (img->format == BITMAP_FMT_BGR565) {

        as_pixsize = 2;
        as_format  = PXP_ASPIXELFMT_RGB565;
        ps_pixsize = 2;
        ps_format  = PXP_PSPIXELFMT_RGB565;
    } else {

        as_pixsize = 4;
        as_format  = PXP_ASPIXELFMT_ARGB8888;
        ps_pixsize = 4;
        ps_format  = PXP_PSPIXELFMT_RGB888;
    }

    if (fb->format == BITMAP_FMT_BGR565) {

        out_pixsize = 2;
        out_format  = PXP_OUTPUTPIXEL_RGB565;
    } else {

        out_pixsize = 4;
        out_format  = PXP_OUTPUTPIXEL_RGB888;
    }

    as_x = src->x;
    as_y = src->y;
    as_w = img->w;

    fb_data = bitmap_lock_buffer_for_write(fb);
    img_data = bitmap_lock_buffer_for_read(img);

    /* 计算AS起始地址 */
    as_addr = ((uint32_t)img_data + as_pixsize * (as_w * as_y + as_x));
    uint32_t img_flush_size = ((src->h - 1) * img->w + src->w) * as_pixsize;
    aw_cache_flush((void *)as_addr, img_flush_size);

    /* 复位pxp */
    awbl_pxp_hard_reset();

    /* 判断是否需要缩放 */
    if ((src->w != dst->w) || (src->h != dst->h)) {

        ps_x = src->x;
        ps_y = src->y;
        ps_w = img->w;

        /* 申请一片dst目标相同面积的缓存 */
        p_scale_buf = (uint8_t *)aw_mem_align(dst->w * dst->h * out_pixsize, 32);
        return_value_if_fail(p_scale_buf != NULL, RET_FAIL);

        /* 计算PS起始地址 */
        ps_addr = ((uint32_t)img_data + ps_pixsize * (ps_w * ps_y + ps_x));

        /* PS configure */
        ps_buffer_config.pixel_fmt   = ps_format;
        ps_buffer_config.buf_addr    = (uint32_t)ps_addr;
        ps_buffer_config.pitch_bytes = ps_w * ps_pixsize;

        /* 配置PS buffer */
        awbl_pxp_set_ps_buffer_config(&ps_buffer_config);

        /* 禁能AS */
        awbl_pxp_set_as_position(0xFFFF, 0xFFFF, 0, 0);

        /* output config */
        pxp_output_config.pixel_fmt = out_format;
        pxp_output_config.output_mode = PXP_OUTPUTMODE_PROGRESSIVE;
        pxp_output_config.buf0_addr = (uint32_t) p_scale_buf;
        pxp_output_config.pitch_bytes = dst->w * out_pixsize;
        pxp_output_config.width  = dst->w;
        pxp_output_config.height = dst->h;

        awbl_pxp_set_output_buffer_config(&pxp_output_config);

        awbl_pxp_enable_csc1(0);

        awbl_pxp_enable_csc2(0);

        /* 设置缩放的输入和输出图像的长、宽 */
        awbl_pxp_set_ps_scaler(src->w, src->h, dst->w, dst->h);

        /* 设置PS图形大小为目标窗口的大小 */
        awbl_pxp_set_ps_position(0, 0, dst->w - 1, dst->h - 1);

        /* 开始缩放处理 */
        awbl_pxp_start();

        /* wait for process complete */
        awbl_pxp_complete_status_sync();

        /**
         * \note 至此，缩放处理的图像已经存放在p_scale_buf缓冲区中，
         *       此时重新赋值AS起始地址，用于接下来的blend。
         */
        as_addr = (uint32_t)p_scale_buf;
        as_x = dst->x;
        as_y = dst->y;
        as_w = dst->w;

        as_format  = out_format;
        as_pixsize = out_pixsize;

        /* 复位PXP，准备配置blend */
        awbl_pxp_hard_reset();
    }

    uint32_t fb_flush_size = ((dst->h - 1) * fb->w + dst->w) * out_pixsize;

    /* 计算输出缓冲区起始地址 */
    out_addr = (uint32_t)fb_data + (fb->w * dst->y + dst->x) * out_pixsize;
    aw_cache_flush((void *)out_addr, fb_flush_size);

    /* BGR565 不需要渲染的情况 */
    if ((img->format == BITMAP_FMT_BGR565) && (global_alpha > 0xfb)) {

        /* PS configure */
        ps_buffer_config.pixel_fmt   = ps_format;
        ps_buffer_config.buf_addr    = (uint32_t)as_addr;
        ps_buffer_config.pitch_bytes = as_w * ps_pixsize;

        awbl_pxp_set_ps_buffer_config(&ps_buffer_config);
        awbl_pxp_set_ps_position(0, 0, dst->w - 1, dst->h - 1);
        awbl_pxp_set_as_position(1, 1, 0, 0);
    } else {

        /* PS configure */
        ps_buffer_config.pixel_fmt   = out_format;
        ps_buffer_config.buf_addr    = (uint32_t)out_addr;
        ps_buffer_config.pitch_bytes = fb->w * out_pixsize;

        /* 配置PS buffer */
        awbl_pxp_set_ps_buffer_config(&ps_buffer_config);

        /* AS配置参数设置 */
        awbl_pxp_as_buf_cfg_t as_buffer_config = {

            .pixel_fmt   = as_format,
            .buf_addr    = (uint32_t)as_addr,
            .pitch_bytes = as_w * as_pixsize,
        };

        /* 配置AS buffer */
        awbl_pxp_set_as_buffer_config(&as_buffer_config);

        /* 设置as的一些属性 */
        awbl_pxp_as_blend_cfg_t as_blend_config = {

            .alpha        = global_alpha,       /* 设置透明度 */
            .invert_alpha = 0,
            .alpha_mode   = PXP_ALPHA_MULTIPLY,
            .rop_mode     = PXP_ROP_MERGEAS     /* 混合颜色 */
        };

        awbl_pxp_set_as_blend_config(&as_blend_config);

        awbl_pxp_set_as_position(0, 0, dst->w - 1, dst->h - 1);
        awbl_pxp_set_ps_position(0, 0, dst->w - 1, dst->h - 1);
    }

    /* output config */
    pxp_output_config.pixel_fmt   = out_format;
    pxp_output_config.output_mode = PXP_OUTPUTMODE_PROGRESSIVE;
    pxp_output_config.buf0_addr   = (uint32_t)out_addr;
    pxp_output_config.buf1_addr   = 0;
    pxp_output_config.pitch_bytes = fb->w * out_pixsize;
    pxp_output_config.width       = dst->w;
    pxp_output_config.height      = dst->h;

    awbl_pxp_set_output_buffer_config(&pxp_output_config);

    awbl_pxp_enable_csc1(0);
    awbl_pxp_enable_csc2(0);

    awbl_pxp_start();

    awbl_pxp_complete_status_sync();

    aw_cache_invalidate((void *)out_addr, fb_flush_size);

    /* 释放内存（设置缩放时需要获取临时缓存） */
    if (p_scale_buf != NULL) {
        aw_mem_free(p_scale_buf);
    }

    bitmap_unlock_buffer(fb);
    bitmap_unlock_buffer(img);
    return RET_OK;
}

ret_t g2d_rotate_image(bitmap_t* fb, bitmap_t* img, const rect_t* src_p, lcd_orientation_t o)
{
    uint8_t* fb_data = NULL;
    uint8_t* img_data = NULL;
    rect_t temp_src = *src_p;
    rect_t* src = &temp_src;

    assert(src->x >= 0);
    assert(src->y >= 0);
    assert(src->w >= 0);
    assert(src->h >= 0);

    return_value_if_fail((fb  != NULL)     && \
                         (img != NULL)     && \
                         (src != NULL), RET_BAD_PARAMS);
    return_value_if_fail((src->w != 0)     && \
                         (src->h != 0), RET_NOT_IMPL);

    return_value_if_fail((fb->w == img->h) && \
                         (fb->h == img->w), RET_BAD_PARAMS);

    return_value_if_fail((fb->format == BITMAP_FMT_RGBA8888) || \
                         (fb->format == BITMAP_FMT_BGR565), RET_NOT_IMPL);

    return_value_if_fail((fb->format == img->format), RET_BAD_PARAMS);
    return_value_if_fail((o   == LCD_ORIENTATION_90), RET_NOT_IMPL);

    int32_t  ps_x        = 0;
    int32_t  ps_y        = 0;
    int32_t  ps_w        = 0;

    uint16_t ps_format   = 0;
    uint16_t ps_pixsize  = 0;
    uint16_t out_format  = 0;
    uint16_t out_pixsize = 0;
    uint32_t ps_addr     = 0;
    uint32_t out_addr    = 0;

    awbl_pxp_output_buf_cfg_t pxp_output_config = {0};
    awbl_pxp_ps_buf_cfg_t     ps_buffer_config  = {0};

    if (img->format == BITMAP_FMT_BGR565) {
        ps_pixsize = 2;
        ps_format = PXP_PSPIXELFMT_RGB565;
    } else {
        ps_pixsize = 4;
        ps_format = PXP_PSPIXELFMT_RGB888;
    }

    if (fb->format == BITMAP_FMT_BGR565) {
        out_pixsize = 2;
        out_format = PXP_OUTPUTPIXEL_RGB565;
    } else {
        out_pixsize = 4;
        out_format = PXP_OUTPUTPIXEL_RGB888;
    }

    ps_x = src->x;
    ps_y = src->y;
    ps_w = img->w;

    /**
     * \note 补偿图形的边长为8的整数倍,否则旋转后有锯齿
     */
    int re   = src->h % 8;
    int tmp  = src->h ;
    int diff = 0;
    if (re != 0) {
       diff = 8 - re;
       tmp += diff;
       if (src->y + tmp > img->h) {
           return RET_NOT_IMPL;  /* 返回，使用软件旋转 */
       } else {
           src->h = tmp;
       }
    }
    re  = src->w % 8;
    tmp = src->w;
    if (re != 0) {
        diff = 8 - re;
        tmp  += diff;
        if (src->x + tmp > img->w) {
           return RET_NOT_IMPL;  /* 返回，使用软件旋转 */
       } else {
           src->w = tmp;
       }
    }

    int32_t targ_x, targ_y;

    targ_x = src->y;
    targ_y = fb->h - src->x - src->w;

    fb_data = bitmap_lock_buffer_for_write(fb);
    img_data = bitmap_lock_buffer_for_read(img);

    /* 计算输出缓冲区起始地址 */
    uint32_t fb_flush_size = ((src->w - 1) * fb->w + src->h) * out_pixsize;
    out_addr = (uint32_t)fb_data + (fb->w * targ_y + targ_x) * out_pixsize;
    aw_cache_flush((void *)out_addr,  fb_flush_size);

    /* 计算PS起始地址 */
    ps_addr = ((uint32_t)img_data) + (ps_y * ps_w + ps_x)* ps_pixsize ;
    uint32_t img_flush_size = ((src->h - 1) * img->w + src->w) * ps_pixsize;
    aw_cache_flush((void *)ps_addr, img_flush_size);

    /**
     * \note 开始配置PXP
     */

    /* 复位pxp */
    awbl_pxp_hard_reset();

    /* 配置PS buffer */
    ps_buffer_config.pixel_fmt   = ps_format;
    ps_buffer_config.buf_addr    = (uint32_t)ps_addr;
    ps_buffer_config.pitch_bytes = ps_w * ps_pixsize;

    awbl_pxp_set_ps_buffer_config(&ps_buffer_config);

    awbl_pxp_set_ps_position(0, 0, src->h - 1, src->w - 1);

    /* 禁能AS ？？？ */
    awbl_pxp_set_as_position(1, 1, 0, 0);

    /* Output config. */
    pxp_output_config.pixel_fmt   = out_format;
    pxp_output_config.output_mode = PXP_OUTPUTMODE_PROGRESSIVE;
    pxp_output_config.buf0_addr   = (uint32_t) out_addr;
    pxp_output_config.pitch_bytes = fb->w * out_pixsize;
    pxp_output_config.width       = src->h;
    pxp_output_config.height      = src->w;

    awbl_pxp_set_output_buffer_config(&pxp_output_config);

    awbl_pxp_enable_csc1(0);
    awbl_pxp_enable_csc2(0);

    /* 旋转270°(旋转结果和软件实现的选装90°的结果一致) */
    awbl_pxp_set_rotate_config(PXP_ROTATE_PS, PXP_ROTATE_270, PXP_FLIP_DISABLE);

    /* 开始旋转 */
    awbl_pxp_start();

    /* Wait for process complete */
    awbl_pxp_complete_status_sync();
    aw_cache_invalidate((void *)out_addr, fb_flush_size);

    bitmap_unlock_buffer(fb);
    bitmap_unlock_buffer(img);
    return RET_OK;
}


