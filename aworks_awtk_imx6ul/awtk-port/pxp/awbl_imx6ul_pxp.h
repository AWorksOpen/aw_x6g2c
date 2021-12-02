/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Freescale i.MX6UL Pixel Pipeline driver
 *
 * \internal
 * \par Modification History
 * - 1.00  19-09-04  cat, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_PXP_H
#define __AWBL_IMX6UL_PXP_H

#include "apollo.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \brief PXP process surface buffer pixel format.
 */
typedef enum awbl_pxp_ps_pixel_fmt {

    PXP_PSPIXELFMT_RGB888       = 0x4,  /**< \brief 32-bit pixels without alpha (unpacked 24-bit format) */
    PXP_PSPIXELFMT_RGB555       = 0xC,  /**< \brief 16-bit pixels without alpha. */
    PXP_PSPIXELFMT_RGB444       = 0xD,  /**< \brief 16-bit pixels without alpha. */
    PXP_PSPIXELFMT_RGB565       = 0xE,  /**< \brief 16-bit pixels without alpha. */
    PXP_PSPIXELFMT_YUV1P444     = 0x10, /**< \brief 32-bit pixels (1-plane XYUV unpacked). */
    PXP_PSPIXELFMT_UYVY1P422    = 0x12, /**< \brief 16-bit pixels (1-plane U0,Y0,V0,Y1 interleaved bytes) */
    PXP_PSPIXELFMT_VYUY1P422    = 0x13, /**< \brief 16-bit pixels (1-plane V0,Y0,U0,Y1 interleaved bytes) */
    PXP_PSPIXELFMT_Y8           = 0x14, /**< \brief 8-bit monochrome pixels (1-plane Y luma output) */
    PXP_PSPIXELFMT_Y4           = 0x15, /**< \brief 4-bit monochrome pixels (1-plane Y luma, 4 bit truncation) */
    PXP_PSPIXELFMT_YUV2P422     = 0x18, /**< \brief 16-bit pixels (2-plane UV interleaved bytes) */
    PXP_PSPIXELFMT_YUV2P420     = 0x19, /**< \brief 16-bit pixels (2-plane UV) */
    PXP_PSPIXELFMT_YVU2P422     = 0x1A, /**< \brief 16-bit pixels (2-plane VU interleaved bytes) */
    PXP_PSPIXELFMT_YVU2P420     = 0x1B, /**< \brief 16-bit pixels (2-plane VU) */
    PXP_PSPIXELFMT_YVU422       = 0x1E, /**< \brief 16-bit pixels (3-plane) */
    PXP_PSPIXELFMT_YVU420       = 0x1F, /**< \brief 16-bit pixels (3-plane) */
} awbl_pxp_ps_pixel_fmt_t;

/**
 * \brief  PXP alpha surface buffer pixel format.
 */
typedef enum awbl_pxp_as_pixel_fmt {
    PXP_ASPIXELFMT_ARGB8888     = 0x0,  /**< \brief 32-bit pixels with alpha. */
    PXP_ASPIXELFMT_RGB888       = 0x4,  /**< \brief 32-bit pixels without alpha (unpacked 24-bit format) */
    PXP_ASPIXELFMT_ARGB1555     = 0x8,  /**< \brief 16-bit pixels with alpha. */
    PXP_ASPIXELFMT_ARGB4444     = 0x9,  /**< \brief 16-bit pixels with alpha. */
    PXP_ASPIXELFMT_RGB555       = 0xC,  /**< \brief 16-bit pixels without alpha. */
    PXP_ASPIXELFMT_RGB444       = 0xD,  /**< \brief 16-bit pixels without alpha. */
    PXP_ASPIXELFMT_RGB565       = 0xE,  /**< \brief 16-bit pixels without alpha. */
} awbl_pxp_as_pixel_fmt_t;

/**
 * \brief  PXP ROP mode during blending.
 *
 * Explanation:
 * - AS: Alpha surface
 * - PS: Process surface
 * - nAS: Alpha surface NOT value
 * - nPS: Process surface NOT value
 */
typedef enum awbl_pxp_rop_mode {

    PXP_ROP_MASKAS              = 0x0,  /**< \brief AS AND PS. */
    PXP_ROP_MASKNOTAS           = 0x1,  /**< \brief nAS AND PS. */
    PXP_ROP_MASKASNOT           = 0x2,  /**< \brief AS AND nPS. */
    PXP_ROP_MERGEAS             = 0x3,  /**< \brief AS OR PS. */
    PXP_ROP_MERGENOTAS          = 0x4,  /**< \brief nAS OR PS. */
    PXP_ROP_MERGEASNOT          = 0x5,  /**< \brief AS OR nPS. */
    PXP_ROP_NOTCOPYAS           = 0x6,  /**< \brief nAS. */
    PXP_ROP_NOT                 = 0x7,  /**< \brief nPS. */
    PXP_ROP_NOTMASKAS           = 0x8,  /**< \brief AS NAND PS. */
    PXP_ROP_NOTMERGEAS          = 0x9,  /**< \brief AS NOR PS. */
    PXP_ROP_XORAS               = 0xA,  /**< \brief AS XOR PS. */
    PXP_ROP_NOTXORAS            = 0xB   /**< \brief AS XNOR PS. */
} awbl_pxp_rop_mode_t;


/**
 * \brief PXP alpha mode during blending.
 */
typedef enum awbl_pxp_alpha_mode {

    PXP_ALPHA_EMBEDDED,                 /**< \brief The alpha surface pixel alpha value will be used for blend. */
    PXP_ALPHA_OVERRIDE,                 /**< \brief The user defined alpha value will be used for blend directly. */
    PXP_ALPHA_MULTIPLY,                 /**< \brief The alpha surface pixel alpha value scaled the user defined
                                              alpha value will be used for blend, for example, pixel alpha set
                                              set to 200, user defined alpha set to 100, then the reault alpha
                                              is 200 * 100 / 255. */
    PXP_ALPHA_ROP                       /**< \brief Raster operation. */
} awbl_pxp_alpha_mode_t;

/**
 * \brief PXP output mode.
 *
 * \note  progressive -- 逐行扫描
 *        field       -- 场扫描
 *        interlaced  -- 隔行扫描
 */
typedef enum awbl_pxp_output_mode {

    PXP_OUTPUTMODE_PROGRESSIVE  = 0,    /**< \brief All data written in progressive format to output buffer 0. */
    PXP_OUTPUTMODE_FIELD0,              /**< \brief Only write field 0 data to output buffer 0. */
    PXP_OUTPUTMODE_FIELD1,              /**< \brief Only write field 1 data to output buffer 0. */
    PXP_OUTPUTMODE_INTERLACED,          /**< \brief Field 0 write to buffer 0, field 1 write to buffer 1. */
} awbl_pxp_output_mode_t;

/**
 * \brief PXP rotate mode.
 */
typedef enum awbl_pxp_rotate_position {

    PXP_ROTATE_OUTPUTBUF        = 0,    /**< \brief rotate the output buffer. */
    PXP_ROTATE_PS,                      /**< \brief Rotate the process surface. */
} awbl_pxp_rotate_position_t;

/**
 * \brief PXP rotate degree.
 */
typedef enum awbl_pxp_rotate_degree {

    PXP_ROTATE_0                = 0,    /**< \brief Clock wise rotate 0 deg. */
    PXP_ROTATE_90,                      /**< \brief Clock wise rotate 90 deg. */
    PXP_ROTATE_180,                     /**< \brief Clock wise rotate 180 deg. */
    PXP_ROTATE_270,                     /**< \brief Clock wise rotate 270 deg. */
} awbl_pxp_rotate_degree_t;

/**
 * \brief PXP output flip mode.
 */
typedef enum awbl_pxp_flip_mode {

    PXP_FLIP_DISABLE            = 0,    /**< \brief Flip disable. */
    PXP_FLIP_HORIZONTAL         = 0x01, /**< \brief Horizontal flip. */
    PXP_FLIP_VERTICAL           = 0x02, /**< \brief Vertical flip. */
    PXP_FLIP_BOTH               = 0x03, /**< \brief Flip both directions. */
} awbl_pxp_flip_mode_t;

/**
 * \brief PXP output pixel format.
 */
typedef enum awbl_pxp_output_pixel_fmt {

    PXP_OUTPUTPIXEL_ARGB8888    = 0x0,
    PXP_OUTPUTPIXEL_RGB888      = 0x4,
    PXP_OUTPUTPIXEL_RGB888P     = 0x5,
    PXP_OUTPUTPIXEL_ARGB1555    = 0x8,
    PXP_OUTPUTPIXEL_ARGB4444    = 0x9,
    PXP_OUTPUTPIXEL_RGB555      = 0xC,
    PXP_OUTPUTPIXEL_RGB444      = 0xD,
    PXP_OUTPUTPIXEL_RGB565      = 0xE,
    PXP_OUTPUTPIXEL_YUV1P444    = 0x10,
    PXP_OUTPUTPIXEL_UYVY1P422   = 0x12,
    PXP_OUTPUTPIXEL_VYUY1P422   = 0x13,
    PXP_OUTPUTPIXEL_Y8          = 0x14,
    PXP_OUTPUTPIXEL_Y4          = 0x15,
    PXP_OUTPUTPIXEL_YUV2P422    = 0x18,
    PXP_OUTPUTPIXEL_YUV2P420    = 0x19,
    PXP_OUTPUTPIXEL_YVU2P422    = 0x1A,
    PXP_OUTPUTPIXEL_YVU2P420    = 0x1B,
} awbl_pxp_output_pixel_fmt_t;

/**
 * \brief PXP process surface buffer configuration.
 */
typedef struct awbl_pxp_ps_buf_cfg {

    awbl_pxp_ps_pixel_fmt_t     pixel_fmt;      /**< \brief PS buffer pixel format. */
    uint8_t                     swap_byte;      /**< \brief For each 16 bit word, set true to swap the two bytes. */
    uint32_t                    buf_addr;       /**< \brief Input buffer address for the first panel. */
    uint32_t                    buf_addr_u;     /**< \brief Input buffer address for the second panel. */
    uint32_t                    buf_addr_v;     /**< \brief Input buffer address for the third panel. */
    uint16_t                    pitch_bytes;    /**< \brief Number of bytes between two vertically adjacent pixels. */
} awbl_pxp_ps_buf_cfg_t;

/**
 * \brief  PXP alphs surface buffer configuration.
 */
typedef struct awbl_pxp_as_buf_cfg {

    awbl_pxp_as_pixel_fmt_t     pixel_fmt;      /**< \brief AS buffer pixel format. */
    uint32_t                    buf_addr;       /**< \brief Input buffer address. */
    uint16_t                    pitch_bytes;    /**< \brief Number of bytes between two vertically adjacent pixels. */
} awbl_pxp_as_buf_cfg_t;

/**
 * \brief  PXP output buffer configuration.
 */
typedef struct awbl_pxp_output_buf_cfg {

    awbl_pxp_output_pixel_fmt_t pixel_fmt;      /**< \brief Output buffer pixel format. */
    awbl_pxp_output_mode_t      output_mode;    /**< \brief Interlaced output mode. */
    uint32_t                    buf0_addr;      /**< \brief Output buffer 0 address. */
    uint32_t                    buf1_addr;      /**< \brief Output buffer 1 address, used for UV data in YUV 2-plane mode, or
                                                      field 1 in output interlaced mode. */
    uint16_t                    pitch_bytes;    /**< \brief Number of bytes between two vertically adjacent pixels. */
    uint16_t                    width;          /**< \brief Pixels per line. */
    uint16_t                    height;         /**< \brief How many lines in output buffer. */
} awbl_pxp_output_buf_cfg_t;

/**
 * \brief  PXP alpha surface blending configuration.
 */
typedef struct awbl_pxp_as_blend_config
{
    uint8_t                     alpha;          /**< \brief User defined alpha value, only used when @ref alphaMode is @ref kPXP_AlphaOverride or @refkPXP_AlphaRop. */
    uint8_t                     invert_alpha;   /**< \brief Set true to invert the alpha. */
    awbl_pxp_alpha_mode_t       alpha_mode;     /**< \brief Alpha mode. */
    awbl_pxp_rop_mode_t         rop_mode;       /**< \brief ROP mode, only valid when @ref alphaMode is @ref kPXP_AlphaRop. */
} awbl_pxp_as_blend_cfg_t;


/**
 * \brief 设置PS的背景颜色
 *
 * \param[in] bg_color 背景颜色值(最大支持24位)
 *
 * \return    无
 *
 */
void awbl_pxp_set_ps_background_color (uint32_t bg_color);

/**
 * \brief PS Buffer配置
 *
 * \param[in] p_config  buffer配置信息
 *
 * \return    无
 *
 */
void awbl_pxp_set_ps_buffer_config (const awbl_pxp_ps_buf_cfg_t *p_cfg);


/**
 * \brief AS Buffer配置
 *
 * \param[in] p_config  buffer配置信息
 *
 * \return    无
 *
 */
void awbl_pxp_set_as_buffer_config (const awbl_pxp_as_buf_cfg_t *p_cfg);


/**
 * \brief AS 颜色混合配置
 *
 * \param[in] p_config  配置信息
 *
 * \return    无
 *
 */
void awbl_pxp_set_as_blend_config (const awbl_pxp_as_blend_cfg_t *p_cfg);


/**
 * \brief 输出缓冲区配置
 *
 * \param[in] p_config  Buffer配置信息
 *
 * \return    无
 *
 */
void awbl_pxp_set_output_buffer_config (const awbl_pxp_output_buf_cfg_t *p_cfg);



/**
 * \brief PS 图形位置设置
 *
 * \param[in] ul_x  左上角x坐标
 * \param[in] ul_y  左上角y坐标
 * \param[in] lr_x  右下角x坐标
 * \param[in] lr_y  右下角y坐标
 *
 * \return    无
 *
 */
void awbl_pxp_set_ps_position (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * \brief AS 图形位置设置
 *
 * \param[in] ul_x  左上角x坐标
 * \param[in] ul_y  左上角y坐标
 * \param[in] lr_x  右下角x坐标
 * \param[in] lr_y  右下角y坐标
 *
 * \return    无
 *
 */
void awbl_pxp_set_as_position (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * \brief PS 缩放功能时, 输入输出图形宽度、高度属性设置
 *
 * \param[in] input_width   输入图形的宽度
 * \param[in] input_height  输入图形的高度
 * \param[in] output_width  输出图形的宽度
 * \param[in] output_height 输出图形的宽度
 *
 * \return    无
 *
 */
void awbl_pxp_set_ps_scaler (uint16_t input_width,
                             uint16_t input_height,
                             uint16_t output_width,
                             uint16_t output_height);

/**
 * \brief 图形旋转配置
 *
 * \param[in] position   旋转图形位置选择
 * \param[in] degree     旋转的角度
 * \param[in] flip_mode  Flip模式设置
 *
 * \return    无
 *
 */
void awbl_pxp_set_rotate_config (awbl_pxp_rotate_position_t position,
                                 awbl_pxp_rotate_degree_t   degree,
                                 awbl_pxp_flip_mode_t       flip_mode);

/**
 * \brief 设置PS ColorKey
 *
 * \param[in] color_key_l   Low range of color key applied to PS buffer.
 * \param[in] color_key_h   High range of color key applied to PS buffer.
 *
 * \return    无
 */
void awbl_pxp_set_ps_color_key (uint32_t color_key_l, uint32_t color_key_h);

/**
 * \brief 设置是否使能颜色空间转换
 *
 * \param[in] enable  0:禁能  1:使能
 *
 * \return    无
 *
 */
void awbl_pxp_enable_csc1 (uint8_t enable);

/**
 * \brief 设置是否使能二次颜色空间转换
 *
 * \param[in] enable  0:禁能  1:使能
 *
 * \return    无
 *
 */
void awbl_pxp_enable_csc2 (uint8_t enable);

/**
 * \brief 启动PXP图形处理
 *
 * \return    无
 *
 */
void awbl_pxp_start (void);


/**
 * \brief 等待PXP图形处理完成 (注意该函数会阻塞)
 *
 * \return    无
 */
void awbl_pxp_complete_status_sync (void);


/**
 * \brief 复位PXP模块
 *
 * \return    无
 */
void awbl_pxp_hard_reset (void);


/**
 * \brief 初始化 PXP硬件
 *
 * \return    无
 */
void awbl_imx6ul_pxp_init (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_PXP_H */

/* end of file */
