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
#ifndef __UVC_CTRLS_H
#define __UVC_CTRLS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uvc/uvc_entity.h"

/* 控件类*/
#define UVC_CTRL_CLASS_USER          0x00980000         /* 旧类型的“用户”控制*/
#define UVC_CTRL_CLASS_MPEG          0x00990000         /* MPEG压缩控制*/
#define UVC_CTRL_CLASS_CAMERA        0x009a0000         /* 摄像头类控制*/
#define UVC_CTRL_CLASS_FM_TX         0x009b0000         /* FM调制器控制*/
#define UVC_CTRL_CLASS_FLASH         0x009c0000         /* 摄像头闪光灯控制*/
#define UVC_CTRL_CLASS_JPEG          0x009d0000         /* JPEG压缩控制*/
#define UVC_CTRL_CLASS_IMAGE_SOURCE  0x009e0000         /* 图像源控制*/
#define UVC_CTRL_CLASS_IMAGE_PROC    0x009f0000         /* 图像处理控制*/
#define UVC_CTRL_CLASS_DV            0x00a00000         /* 数字视频控制*/
#define UVC_CTRL_CLASS_FM_RX         0x00a10000         /* FM接收器控制*/
#define UVC_CTRL_CLASS_RF_TUNER      0x00a20000         /* RF调谐器控制*/
#define UVC_CTRL_CLASS_DETECT        0x00a30000         /* 检测控制*/

/* 用户类控制ID*/
#define UVC_CID_BASE                (UVC_CTRL_CLASS_USER | 0x900)
#define UVC_CID_USER_BASE            UVC_CID_BASE
#define UVC_CID_USER_CLASS          (UVC_CTRL_CLASS_USER | 1)
#define UVC_CID_BRIGHTNESS          (UVC_CID_BASE+0)
#define UVC_CID_CONTRAST            (UVC_CID_BASE+1)
#define UVC_CID_SATURATION          (UVC_CID_BASE+2)
#define UVC_CID_HUE                 (UVC_CID_BASE+3)
#define UVC_CID_AUDIO_VOLUME        (UVC_CID_BASE+5)
#define UVC_CID_AUDIO_BALANCE       (UVC_CID_BASE+6)
#define UVC_CID_AUDIO_BASS          (UVC_CID_BASE+7)
#define UVC_CID_AUDIO_TREBLE        (UVC_CID_BASE+8)
#define UVC_CID_AUDIO_MUTE          (UVC_CID_BASE+9)
#define UVC_CID_AUDIO_LOUDNESS      (UVC_CID_BASE+10)
#define UVC_CID_BLACK_LEVEL         (UVC_CID_BASE+11) /* Deprecated */
#define UVC_CID_AUTO_WHITE_BALANCE  (UVC_CID_BASE+12)
#define UVC_CID_DO_WHITE_BALANCE    (UVC_CID_BASE+13)
#define UVC_CID_RED_BALANCE         (UVC_CID_BASE+14)
#define UVC_CID_BLUE_BALANCE        (UVC_CID_BASE+15)
#define UVC_CID_GAMMA               (UVC_CID_BASE+16)
#define UVC_CID_WHITENESS           (UVC_CID_GAMMA) /* Deprecated */
#define UVC_CID_EXPOSURE            (UVC_CID_BASE+17)
#define UVC_CID_AUTOGAIN            (UVC_CID_BASE+18)
#define UVC_CID_GAIN                (UVC_CID_BASE+19)
#define UVC_CID_HFLIP               (UVC_CID_BASE+20)
#define UVC_CID_VFLIP               (UVC_CID_BASE+21)

#define UVC_CID_POWER_LINE_FREQUENCY       (UVC_CID_BASE+24)

#define UVC_CID_HUE_AUTO                   (UVC_CID_BASE+25)
#define UVC_CID_WHITE_BALANCE_TEMPERATURE  (UVC_CID_BASE+26)
#define UVC_CID_SHARPNESS                  (UVC_CID_BASE+27)
#define UVC_CID_BACKLIGHT_COMPENSATION     (UVC_CID_BASE+28)
#define UVC_CID_CHROMA_AGC                 (UVC_CID_BASE+29)
#define UVC_CID_COLOR_KILLER               (UVC_CID_BASE+30)
#define UVC_CID_COLORFX                    (UVC_CID_BASE+31)

/*  摄像头类控制ID*/
#define UVC_CID_CAMERA_CLASS_BASE  (UVC_CTRL_CLASS_CAMERA | 0x900)
#define UVC_CID_CAMERA_CLASS       (UVC_CTRL_CLASS_CAMERA | 1)

#define UVC_CID_EXPOSURE_AUTO      (UVC_CID_CAMERA_CLASS_BASE+1)
enum  uvc_exposure_auto_type {
    UVC_EXPOSURE_AUTO              = 0,
    UVC_EXPOSURE_MANUAL            = 1,
    UVC_EXPOSURE_SHUTTER_PRIORITY  = 2,
    UVC_EXPOSURE_APERTURE_PRIORITY = 3
};
#define UVC_CID_EXPOSURE_ABSOLUTE           (UVC_CID_CAMERA_CLASS_BASE+2)
#define UVC_CID_EXPOSURE_AUTO_PRIORITY      (UVC_CID_CAMERA_CLASS_BASE+3)

#define UVC_CID_PAN_RELATIVE                (UVC_CID_CAMERA_CLASS_BASE+4)
#define UVC_CID_TILT_RELATIVE               (UVC_CID_CAMERA_CLASS_BASE+5)
#define UVC_CID_PAN_RESET                   (UVC_CID_CAMERA_CLASS_BASE+6)
#define UVC_CID_TILT_RESET                  (UVC_CID_CAMERA_CLASS_BASE+7)

#define UVC_CID_PAN_ABSOLUTE                (UVC_CID_CAMERA_CLASS_BASE+8)
#define UVC_CID_TILT_ABSOLUTE               (UVC_CID_CAMERA_CLASS_BASE+9)

#define UVC_CID_FOCUS_ABSOLUTE              (UVC_CID_CAMERA_CLASS_BASE+10)
#define UVC_CID_FOCUS_RELATIVE              (UVC_CID_CAMERA_CLASS_BASE+11)
#define UVC_CID_FOCUS_AUTO                  (UVC_CID_CAMERA_CLASS_BASE+12)

#define UVC_CID_ZOOM_ABSOLUTE               (UVC_CID_CAMERA_CLASS_BASE+13)
#define UVC_CID_ZOOM_RELATIVE               (UVC_CID_CAMERA_CLASS_BASE+14)
#define UVC_CID_ZOOM_CONTINUOUS             (UVC_CID_CAMERA_CLASS_BASE+15)

#define UVC_CID_PRIVACY                     (UVC_CID_CAMERA_CLASS_BASE+16)

#define UVC_CID_IRIS_ABSOLUTE               (UVC_CID_CAMERA_CLASS_BASE+17)
#define UVC_CID_IRIS_RELATIVE               (UVC_CID_CAMERA_CLASS_BASE+18)

#define UVC_CID_AUTO_EXPOSURE_BIAS          (UVC_CID_CAMERA_CLASS_BASE+19)

#define UVC_CID_AUTO_N_PRESET_WHITE_BALANCE (UVC_CID_CAMERA_CLASS_BASE+20)


#define UVC_CID_PAN_SPEED                   (UVC_CID_CAMERA_CLASS_BASE+32)
#define UVC_CID_TILT_SPEED                  (UVC_CID_CAMERA_CLASS_BASE+33)

#ifdef __cplusplus
}
#endif  /* __cplusplus  */
#endif

