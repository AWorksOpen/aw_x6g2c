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
#ifndef __UVC_STREAM_H
#define __UVC_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uvc/uvc_driver.h"
#include "host/class/uvc/uvc_entity.h"
#include "host/class/uvc/uvc_video.h"
#include "usb_adapter_inter.h"
#include "usb_errno.h"
#include "host/core/usbh.h"

#define UVC_STREAM_NAME_GET(stream) (stream->name)

/* UVC��������ʱʱ��*/
#define UVC_CTRL_STREAMING_TIMEOUT  5000

/* A.9.7. ��Ƶ���ӿڿؼ�ѡ����*/
#define UVC_VS_CONTROL_UNDEFINED                0x00             /* δ����ؼ�*/
#define UVC_VS_PROBE_CONTROL                    0x01             /* ̽��ؼ�*/
#define UVC_VS_COMMIT_CONTROL                   0x02             /* �ύ�ؼ�*/
#define UVC_VS_STILL_PROBE_CONTROL              0x03
#define UVC_VS_STILL_COMMIT_CONTROL             0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL      0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL        0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL       0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL     0x08
#define UVC_VS_SYNC_DELAY_CONTROL               0x09

/* 2.4.3.3. ����ͷ��Ϣ*/
#define UVC_STREAM_EOH                  (1 << 7)
#define UVC_STREAM_ERR                  (1 << 6)
#define UVC_STREAM_STI                  (1 << 5)
#define UVC_STREAM_RES                  (1 << 4)
#define UVC_STREAM_SCR                  (1 << 3)
#define UVC_STREAM_PTS                  (1 << 2)
#define UVC_STREAM_EOF                  (1 << 1)
#define UVC_STREAM_FID                  (1 << 0)       /* ֡ID*/

/* ��ʽ��־ */
#define UVC_FMT_FLAG_COMPRESSED     0x00000001    /* ѹ����־*/
#define UVC_FMT_FLAG_STREAM         0x00000002    /* ����־*/

/* see also http://vektor.theorem.ca/graphics/ycbcr/ */
enum uvc_colorspace {
    /* SMPTE 170M: used for broadcast NTSC/PAL SDTV */
    UVC_COLORSPACE_SMPTE170M     = 1,

    /* Obsolete pre-1998 SMPTE 240M HDTV standard, superseded by Rec 709 */
    UVC_COLORSPACE_SMPTE240M     = 2,

    /* Rec.709: used for HDTV */
    UVC_COLORSPACE_REC709        = 3,

    /*
     * Deprecated, do not use. No driver will ever return this. This was
     * based on a misunderstanding of the bt878 datasheet.
     */
    UVC_COLORSPACE_BT878         = 4,

    /*
     * NTSC 1953 colorspace. This only makes sense when dealing with
     * really, really old NTSC recordings. Superseded by SMPTE 170M.
     */
    UVC_COLORSPACE_470_SYSTEM_M  = 5,

    /*
     * EBU Tech 3213 PAL/SECAM colorspace. This only makes sense when
     * dealing with really old PAL/SECAM recordings. Superseded by
     * SMPTE 170M.
     */
    UVC_COLORSPACE_470_SYSTEM_BG = 6,

    /*
     * Effectively shorthand for V4L2_COLORSPACE_SRGB, V4L2_YCBCR_ENC_601
     * and V4L2_QUANTIZATION_FULL_RANGE. To be used for (Motion-)JPEG.
     */
    UVC_COLORSPACE_JPEG          = 7,

    /* For RGB colorspaces such as produces by most webcams. */
    UVC_COLORSPACE_SRGB          = 8,

    /* AdobeRGB colorspace */
    UVC_COLORSPACE_ADOBERGB      = 9,

    /* BT.2020 colorspace, used for UHDTV. */
    UVC_COLORSPACE_BT2020        = 10,
};

/* RGB��ʽ*/
#define UVC_PIX_FMT_RGB332   usb_fourcc('R', 'G', 'B', '1')    /*  8  RGB-3-3-2     */
#define UVC_PIX_FMT_RGB444   usb_fourcc('R', '4', '4', '4')    /* 16  xxxxrrrr ggggbbbb */
#define UVC_PIX_FMT_ARGB444  usb_fourcc('A', 'R', '1', '2')    /* 16  aaaarrrr ggggbbbb */
#define UVC_PIX_FMT_XRGB444  usb_fourcc('X', 'R', '1', '2')    /* 16  xxxxrrrr ggggbbbb */
#define UVC_PIX_FMT_RGB555   usb_fourcc('R', 'G', 'B', 'O')    /* 16  RGB-5-5-5     */
#define UVC_PIX_FMT_ARGB555  usb_fourcc('A', 'R', '1', '5')    /* 16  ARGB-1-5-5-5  */
#define UVC_PIX_FMT_XRGB555  usb_fourcc('X', 'R', '1', '5')    /* 16  XRGB-1-5-5-5  */
#define UVC_PIX_FMT_RGB565   usb_fourcc('R', 'G', 'B', 'P')    /* 16  RGB-5-6-5     */
#define UVC_PIX_FMT_RGB555X  usb_fourcc('R', 'G', 'B', 'Q')    /* 16  RGB-5-5-5 BE  */
#define UVC_PIX_FMT_ARGB555X usb_fourcc('A', 'R', '1', '5')    /* 16  ARGB-5-5-5 BE */
#define UVC_PIX_FMT_XRGB555X usb_fourcc('X', 'R', '1', '5')    /* 16  XRGB-5-5-5 BE */
#define UVC_PIX_FMT_RGB565X  usb_fourcc('R', 'G', 'B', 'R')    /* 16  RGB-5-6-5 BE  */
#define UVC_PIX_FMT_BGR666   usb_fourcc('B', 'G', 'R', 'H')    /* 18  BGR-6-6-6     */
#define UVC_PIX_FMT_BGR24    usb_fourcc('B', 'G', 'R', '3')    /* 24  BGR-8-8-8     */
#define UVC_PIX_FMT_RGB24    usb_fourcc('R', 'G', 'B', '3')    /* 24  RGB-8-8-8     */
#define UVC_PIX_FMT_BGR32    usb_fourcc('B', 'G', 'R', '4')    /* 32  BGR-8-8-8-8   */
#define UVC_PIX_FMT_ABGR32   usb_fourcc('A', 'R', '2', '4')    /* 32  BGRA-8-8-8-8  */
#define UVC_PIX_FMT_XBGR32   usb_fourcc('X', 'R', '2', '4')    /* 32  BGRX-8-8-8-8  */
#define UVC_PIX_FMT_RGB32    usb_fourcc('R', 'G', 'B', '4')    /* 32  RGB-8-8-8-8   */
#define UVC_PIX_FMT_ARGB32   usb_fourcc('B', 'A', '2', '4')    /* 32  ARGB-8-8-8-8  */
#define UVC_PIX_FMT_XRGB32   usb_fourcc('B', 'X', '2', '4')    /* 32  XRGB-8-8-8-8  */
/* ��ɫ��ʽ*/
#define UVC_PIX_FMT_GREY     usb_fourcc('G', 'R', 'E', 'Y')    /*  8  Greyscale     */
#define UVC_PIX_FMT_Y4       usb_fourcc('Y', '0', '4', ' ')    /*  4  Greyscale     */
#define UVC_PIX_FMT_Y6       usb_fourcc('Y', '0', '6', ' ')    /*  6  Greyscale     */
#define UVC_PIX_FMT_Y10      usb_fourcc('Y', '1', '0', ' ')    /* 10  Greyscale     */
#define UVC_PIX_FMT_Y12      usb_fourcc('Y', '1', '2', ' ')    /* 12  Greyscale     */
#define UVC_PIX_FMT_Y16      usb_fourcc('Y', '1', '6', ' ')    /* 16  Greyscale     */
/* ���Ⱥ�ɫ�ȸ�ʽ */
#define UVC_PIX_FMT_YVU410   usb_fourcc('Y', 'V', 'U', '9')    /*  9  YVU 4:1:0     */
#define UVC_PIX_FMT_YVU420   usb_fourcc('Y', 'V', '1', '2')    /* 12  YVU 4:2:0     */
#define UVC_PIX_FMT_YUYV     usb_fourcc('Y', 'U', 'Y', 'V')    /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_YYUV     usb_fourcc('Y', 'Y', 'U', 'V')    /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_YVYU     usb_fourcc('Y', 'V', 'Y', 'U')    /* 16 YVU 4:2:2 */
#define UVC_PIX_FMT_UYVY     usb_fourcc('U', 'Y', 'V', 'Y')    /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_VYUY     usb_fourcc('V', 'Y', 'U', 'Y')    /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_YUV422P  usb_fourcc('4', '2', '2', 'P')    /* 16  YVU422 planar */
#define UVC_PIX_FMT_YUV411P  usb_fourcc('4', '1', '1', 'P')    /* 16  YVU411 planar */
#define UVC_PIX_FMT_Y41P     usb_fourcc('Y', '4', '1', 'P')    /* 12  YUV 4:1:1     */
#define UVC_PIX_FMT_YUV444   usb_fourcc('Y', '4', '4', '4')    /* 16  xxxxyyyy uuuuvvvv */
#define UVC_PIX_FMT_YUV555   usb_fourcc('Y', 'U', 'V', 'O')    /* 16  YUV-5-5-5     */
#define UVC_PIX_FMT_YUV565   usb_fourcc('Y', 'U', 'V', 'P')    /* 16  YUV-5-6-5     */
#define UVC_PIX_FMT_YUV32    usb_fourcc('Y', 'U', 'V', '4')    /* 32  YUV-8-8-8-8   */
#define UVC_PIX_FMT_YUV410   usb_fourcc('Y', 'U', 'V', '9')    /*  9  YUV 4:1:0     */
#define UVC_PIX_FMT_YUV420   usb_fourcc('Y', 'U', '1', '2')    /* 12  YUV 4:2:0     */
#define UVC_PIX_FMT_HI240    usb_fourcc('H', 'I', '2', '4')    /*  8  8-bit color   */
#define UVC_PIX_FMT_HM12     usb_fourcc('H', 'M', '1', '2')    /*  8  YUV 4:2:0 16x16 macroblocks */
#define UVC_PIX_FMT_M420     usb_fourcc('M', '4', '2', '0')    /* 12  YUV 4:2:0 2 lines y, 1 line uv interleaved */
/* ����ƽ�� -- һ��Y��һ��Cr + Cb����  */
#define UVC_PIX_FMT_NV12     usb_fourcc('N', 'V', '1', '2')    /* 12  Y/CbCr 4:2:0  */
#define UVC_PIX_FMT_NV21     usb_fourcc('N', 'V', '2', '1')    /* 12  Y/CrCb 4:2:0  */
#define UVC_PIX_FMT_NV16     usb_fourcc('N', 'V', '1', '6')    /* 16  Y/CbCr 4:2:2  */
#define UVC_PIX_FMT_NV61     usb_fourcc('N', 'V', '6', '1')    /* 16  Y/CrCb 4:2:2  */
#define UVC_PIX_FMT_NV24     usb_fourcc('N', 'V', '2', '4')    /* 24  Y/CbCr 4:4:4  */
#define UVC_PIX_FMT_NV42     usb_fourcc('N', 'V', '4', '2')    /* 24  Y/CrCb 4:4:4  */
/* Bayer formats - see http://www.siliconimaging.com/RGB%20Bayer.htm */
#define UVC_PIX_FMT_SBGGR8   usb_fourcc('B', 'A', '8', '1')    /*  8  BGBG.. GRGR.. */
#define UVC_PIX_FMT_SGBRG8   usb_fourcc('G', 'B', 'R', 'G')    /*  8  GBGB.. RGRG.. */
#define UVC_PIX_FMT_SGRBG8   usb_fourcc('G', 'R', 'B', 'G')    /*  8  GRGR.. BGBG.. */
#define UVC_PIX_FMT_SRGGB8   usb_fourcc('R', 'G', 'G', 'B')    /*  8  RGRG.. GBGB.. */
#define UVC_PIX_FMT_SBGGR10  usb_fourcc('B', 'G', '1', '0')    /* 10  BGBG.. GRGR.. */
#define UVC_PIX_FMT_SGBRG10  usb_fourcc('G', 'B', '1', '0')    /* 10  GBGB.. RGRG.. */
#define UVC_PIX_FMT_SGRBG10  usb_fourcc('B', 'A', '1', '0')    /* 10  GRGR.. BGBG.. */
#define UVC_PIX_FMT_SRGGB10  usb_fourcc('R', 'G', '1', '0')    /* 10  RGRG.. GBGB.. */
/* 10bit raw bayer packed, 5 bytes for every 4 pixels */
#define UVC_PIX_FMT_SBGGR10P usb_fourcc('p', 'B', 'A', 'A')
#define UVC_PIX_FMT_SGBRG10P usb_fourcc('p', 'G', 'A', 'A')
#define UVC_PIX_FMT_SGRBG10P usb_fourcc('p', 'g', 'A', 'A')
#define UVC_PIX_FMT_SRGGB10P usb_fourcc('p', 'R', 'A', 'A')
/* 10bit raw bayer a-law compressed to 8 bits */
#define UVC_PIX_FMT_SBGGR10ALAW8 usb_fourcc('a', 'B', 'A', '8')
#define UVC_PIX_FMT_SGBRG10ALAW8 usb_fourcc('a', 'G', 'A', '8')
#define UVC_PIX_FMT_SGRBG10ALAW8 usb_fourcc('a', 'g', 'A', '8')
#define UVC_PIX_FMT_SRGGB10ALAW8 usb_fourcc('a', 'R', 'A', '8')
/* 10bit raw bayer DPCM compressed to 8 bits */
#define UVC_PIX_FMT_SBGGR10DPCM8 usb_fourcc('b', 'B', 'A', '8')
#define UVC_PIX_FMT_SGBRG10DPCM8 usb_fourcc('b', 'G', 'A', '8')
#define UVC_PIX_FMT_SGRBG10DPCM8 usb_fourcc('B', 'D', '1', '0')
#define UVC_PIX_FMT_SRGGB10DPCM8 usb_fourcc('b', 'R', 'A', '8')
#define UVC_PIX_FMT_SBGGR12      usb_fourcc('B', 'G', '1', '2') /* 12  BGBG.. GRGR.. */
#define UVC_PIX_FMT_SGBRG12      usb_fourcc('G', 'B', '1', '2') /* 12  GBGB.. RGRG.. */
#define UVC_PIX_FMT_SGRBG12      usb_fourcc('B', 'A', '1', '2') /* 12  GRGR.. BGBG.. */
#define UVC_PIX_FMT_SRGGB12      usb_fourcc('R', 'G', '1', '2') /* 12  RGRG.. GBGB.. */
#define UVC_PIX_FMT_SBGGR16      usb_fourcc('B', 'Y', 'R', '2') /* 16  BGBG.. GRGR.. */
/* ѹ����ʽ */
#define UVC_PIX_FMT_MJPEG       usb_fourcc('M', 'J', 'P', 'G') /* Motion-JPEG   */
#define UVC_PIX_FMT_JPEG        usb_fourcc('J', 'P', 'E', 'G') /* JFIF JPEG     */
#define UVC_PIX_FMT_DV          usb_fourcc('d', 'v', 's', 'd') /* 1394          */
#define UVC_PIX_FMT_MPEG        usb_fourcc('M', 'P', 'E', 'G') /* MPEG-1/2/4 Multiplexed */
#define UVC_PIX_FMT_H264        usb_fourcc('H', '2', '6', '4') /* H264 with start codes */
#define UVC_PIX_FMT_H264_NO_SC  usb_fourcc('A', 'V', 'C', '1') /* H264 without start codes */
#define UVC_PIX_FMT_H264_MVC    usb_fourcc('M', '2', '6', '4') /* H264 MVC */
#define UVC_PIX_FMT_H263        usb_fourcc('H', '2', '6', '3') /* H263          */
#define UVC_PIX_FMT_MPEG1       usb_fourcc('M', 'P', 'G', '1') /* MPEG-1 ES     */
#define UVC_PIX_FMT_MPEG2       usb_fourcc('M', 'P', 'G', '2') /* MPEG-2 ES     */
#define UVC_PIX_FMT_MPEG4       usb_fourcc('M', 'P', 'G', '4') /* MPEG-4 part 2 ES */
#define UVC_PIX_FMT_XVID        usb_fourcc('X', 'V', 'I', 'D') /* Xvid           */
#define UVC_PIX_FMT_VC1_ANNEX_G usb_fourcc('V', 'C', '1', 'G') /* SMPTE 421M Annex G compliant stream */
#define UVC_PIX_FMT_VC1_ANNEX_L usb_fourcc('V', 'C', '1', 'L') /* SMPTE 421M Annex L compliant stream */
#define UVC_PIX_FMT_VP8         usb_fourcc('V', 'P', '8', '0') /* VP8 */

/* "����"�ֶε�ֵ*/
#define UVC_CAP_VIDEO_CAPTURE        0x00000001  /* ��Ƶ�����豸*/
#define UVC_CAP_VIDEO_OUTPUT         0x00000002  /* ��Ƶ����豸*/
#define UVC_CAP_VIDEO_OVERLAY        0x00000004  /* Can do video overlay */
#define UVC_CAP_VBI_CAPTURE          0x00000010  /* Is a raw VBI capture device */
#define UVC_CAP_VBI_OUTPUT           0x00000020  /* Is a raw VBI output device */
#define UVC_CAP_SLICED_VBI_CAPTURE   0x00000040  /* Is a sliced VBI capture device */
#define UVC_CAP_SLICED_VBI_OUTPUT    0x00000080  /* Is a sliced VBI output device */
#define UVC_CAP_RDS_CAPTURE          0x00000100  /* RDS data capture */
#define UVC_CAP_VIDEO_OUTPUT_OVERLAY 0x00000200  /* Can do video output overlay */
#define UVC_CAP_HW_FREQ_SEEK         0x00000400  /* Can do hardware frequency seek  */
#define UVC_CAP_RDS_OUTPUT           0x00000800  /* Is an RDS encoder */

enum uvc_field {
    UVC_FIELD_ANY           = 0, /* driver can choose from none,
                                     top, bottom, interlaced
                                     depending on whatever it thinks
                                     is approximate ... */
    UVC_FIELD_NONE          = 1, /* this device has no fields ... */
    UVC_FIELD_TOP           = 2, /* top field only */
    UVC_FIELD_BOTTOM        = 3, /* bottom field only */
    UVC_FIELD_INTERLACED    = 4, /* both fields interlaced */
    UVC_FIELD_SEQ_TB        = 5, /* both fields sequential into one
                                    buffer, top-bottom order */
    UVC_FIELD_SEQ_BT        = 6, /* same as above + bottom-top order */
    UVC_FIELD_ALTERNATE     = 7, /* both fields alternating into
                                    separate buffers */
    UVC_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
                                    first and the top field is
                                    transmitted first */
    UVC_FIELD_INTERLACED_BT = 9, /* both fields interlaced, top field
                                    first and the bottom field is
                                    transmitted first */
};

/* UVC��ʽ������*/
struct uvc_format_desc {
    char    *name;         /* ��ʽ����*/
    uint8_t  guid[16];     /* ȫ��Ψһ��ʶ��*/
    uint32_t fcc;
};

/* UVC֡�ṹ��*/
struct uvc_frame {
    uint8_t   bFrameIndex;
    uint8_t   bmCapabilities;
    uint16_t  wWidth;
    uint16_t  wHeight;
    uint32_t  dwMinBitRate;
    uint32_t  dwMaxBitRate;
    uint32_t  dwMaxVideoFrameBufferSize;
    uint8_t   bFrameIntervalType;
    uint32_t  dwDefaultFrameInterval;
    uint32_t *dwFrameInterval;
};

struct uvc_frame_info {
    uint16_t  wWidth;
    uint16_t  wHeight;
};

/* UVC��ʽ�ṹ��*/
struct uvc_format {
    uint8_t           type;         /* ��ʽ����*/
    uint8_t           index;        /* ��ʽ����*/
    uint8_t           bpp;          /* �������*/
    uint8_t           colorspace;   /* ɫ��*/
    uint32_t          fcc;
    uint32_t          flags;        /* ��ʽ��־*/

    char              name[32];     /* ��ʽ����*/

    unsigned int      nframes;      /* ֡�ṹ������*/
    struct uvc_frame *frame;        /* UVC֡�ṹ��*/
};

/* ����ͷ֧�ֵ�������ظ�ʽ   */
typedef enum __uvc_pix_format {
    UVC_RGB565,
    UVC_RGB555,
    UVC_RGB444,
    UVC_RGB888,
    UVC_YUV420,
    UVC_YUV422,
    UVC_YUV444,
    UVC_JPEG,
    UVC_MJPEG,
    UVC_BAYER_RAW,
} uvc_pix_format_t;

/* ���ص��ʽ�ṹ��*/
struct uvc_pix_format {
    uint32_t           width;          /* ���*/
    uint32_t           height;         /* �߶�*/
    uint32_t           pixelformat;    /* ���ص��ʽ*/
    uint32_t           field;          /* enum v4l2_field */
    uint32_t           bytesperline;   /* for padding, zero if unused */
    uint32_t           sizeimage;
    uint32_t           colorspace;     /* ɫ��*/
    uint32_t           priv;           /* private data, depends on pixelformat */
    uint32_t           flags;          /* format flags (V4L2_PIX_FMT_FLAG_*) */
    uint32_t           ycbcr_enc;      /* enum v4l2_ycbcr_encoding */
    uint32_t           quantization;   /* enum v4l2_quantization */
};

/* UVC��Ƶ��ͷ�ṹ��*/
struct uvc_streaming_header {
    uint8_t bNumFormats;
    uint8_t bEndpointAddress;
    uint8_t bTerminalLink;
    uint8_t bControlSize;
    uint8_t *bmaControls;
    /* �����ֶ�ֻ��������ͷ������*/
    uint8_t bmInfo;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
};

/* ��Ƶ̽����ύ�ؼ�*/
struct uvc_streaming_control {
    uint16_t bmHint;                   /* ����ֻ����Щ�ֶ�Ӧ���̶ֹ���λ�ֶο���*/
    uint8_t  bFormatIndex;             /* ���Դ���Ƶ�ӿڵĸ�ʽ����������Ƶ��ʽ����*/
    uint8_t  bFrameIndex;              /* ����֡����������Ƶ֡����*/
    uint32_t dwFrameInterval;          /* ���ֶ�Ϊѡ������Ƶ����֡���������������Ƶ֡���(100nsΪ��λ��֡���)*/
    uint16_t wKeyFrameRate;            /* ÿ��Ƶ֡��λ�Ĺؼ�֡����*/
    uint16_t wPFrameRate;              /* ��֡/�ؼ�֡��λ��ʾ��֡����*/
    uint16_t wCompQuality;             /* ����Ԫѹ����������1(���)��10000(���)*/
    uint16_t wCompWindowSize;          /* ƽ�������ʿ��ƵĴ��ڴ�С*/
    uint16_t wDelay;                   /* �ڲ���Ƶ���ӿ��ӳ�(����)*/
    uint32_t dwMaxVideoFrameSize;      /* ��Ƶ֡���������ض������δ�С(�ֽ�)*/
    uint32_t dwMaxPayloadTransferSize; /* ָ���豸�ڵ�����Ч���ش����п��Է��ͻ���ܵ�����ֽ���*/
    uint32_t dwClockFrequency;         /* ָ�����ʵ��豸ʱ��Ƶ��*/
    uint8_t  bmFramingInfo;            /* �˿ؼ�����ָʾ��Ч���ش����Ƿ�����Ƶ��Ч����ͷ�а������֡��Ϣ*/
    uint8_t  bPreferedVersion;         /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ���ѡ���ظ�ʽ�汾*/
    uint8_t  bMinVersion;              /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ���С���ظ�ʽ�汾*/
    uint8_t  bMaxVersion;              /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ�����ظ�ʽ�汾*/
} __attribute__((__packed__));

/* UVC��״̬�ṹ��*/
struct uvc_stats_stream {
    struct usb_timespec start_ts;        /* ����ʼʱ���*/
    struct usb_timespec stop_ts;         /* ��ֹͣʱ���*/

    uint32_t            nb_frames;       /* ֡������*/

    uint32_t            nb_packets;      /* ��������*/
    uint32_t            nb_empty;        /* �հ�������*/
    uint32_t            nb_invalid;      /* ������ͷ����������*/
    uint32_t            nb_errors;       /* �����˴���λ�İ�������*/

    uint32_t            nb_pts_constant; /* ������ʾʱ�����֡������*/
    uint32_t            nb_pts_early;    /* Number of frames with early PTS */
    uint32_t            nb_pts_initial;  /* Number of frames with initial PTS */

    uint32_t            nb_scr_count_ok; /* Number of frames with at least one SCR per non empty packet */
    uint32_t            nb_scr_diffs_ok; /* Number of frames with varying SCR.STC */
    uint32_t            scr_sof_count;   /* STC.SOF counter accumulated since stream start */
    uint32_t            scr_sof;         /* STC.SOF of the last packet */
    uint32_t            min_sof;         /* ��С STC.SOF ֵ*/
    uint32_t            max_sof;         /* ��� STC.SOF ֵ */
};

/* UVC֡״̬�ṹ��*/
struct uvc_stats_frame {
    uint32_t   size;            /* ���񵽵��ֽ���*/
    uint32_t   first_data;      /* ��һ���ǿհ�������*/

    uint32_t   nb_packets;      /* ��������*/
    uint32_t   nb_empty;        /* �հ�������*/
    uint32_t   nb_invalid;      /* ������ͷ����������*/
    uint32_t   nb_errors;       /* �����˴���λ�İ�������*/

    uint32_t   nb_pts;          /* Number of packets with a PTS timestamp */
    uint32_t   nb_pts_diffs;    /* Number of PTS differences inside a frame */
    uint32_t   last_pts_diff;   /* Index of the last PTS difference */
    usb_bool_t has_initial_pts; /* Whether the first non-empty packet has a PTS */
    usb_bool_t has_early_pts;   /* Whether a PTS is present before the first non-empty packet */
    uint32_t   pts;             /* PTS of the last packet */

    uint32_t   nb_scr;          /* Number of packets with a SCR timestamp */
    uint32_t   nb_scr_diffs;    /* Number of SCR.STC differences inside a frame */
    uint16_t   scr_sof;         /* SCR.SOF of the last packet */
    uint32_t   scr_stc;         /* SCR.STC of the last packet */
};

/* UVC��Ƶ���ṹ��*/
struct uvc_streaming {
    char                         name[32];            /* ����������*/
    struct usb_list_head         list;                /* UVC���ṹ������*/
    struct uvc_device           *dev;                 /* UVC�豸�ṹ��*/
    struct usbh_interface       *intf;                /* USB�ӿڽṹ��*/
    int                          intfnum;             /* �ӿڱ��*/
    struct uvc_streaming_header  header;              /* UVC��Ƶ��ͷ�ṹ��*/
    usb_mutex_handle_t           lock;                /* �豸������*/
    uint32_t                     nformats;            /* ��ʽ������*/
    struct uvc_format           *format;              /* UVC��ʽ�ṹ��*/
    enum uvc_buf_type            type;                /* UVC��������*/
    uint16_t                     maxpsize;            /* �˵�������С*/
    struct uvc_video_chain      *chain;               /* ��Ƶ������*/
    struct uvc_streaming_control ctrl;                /* UVC�����ƽṹ��*/
    struct uvc_format           *def_format;          /* Ĭ�ϸ�ʽ*/
    struct uvc_format           *cur_format;          /* ��ǰ��ʽ*/
    struct uvc_frame            *cur_frame;           /* ��ǰ֡*/
    uint32_t                     sequence;            /* ���к�*/
    uint8_t                      last_fid;            /* ���֡ID*/
    uint8_t                      frozen;              /* �Ƿ����*/
    struct usbh_trp            **trp;                 /* USB�����*/
    uint32_t                     trp_size;            /* USB������С*/
    char                       **trp_buffer;          /* USB����黺��*/
    uint8_t                      trp_num;             /* USB���������*/
    usb_bool_t                   is_init;             /* �Ƿ��Ѿ���ʼ��*/
    uint8_t                      opt_status;          /* ����״̬*/
    uint8_t                      open_count;          /* �򿪴���*/
    void                        *queue;               /* �������*/
    void                        *buf_temp;            /* ��ʱ����*/

    /* ����ص�����*/
    void (*decode) (struct usbh_trp *trp, struct uvc_streaming *video,
                    struct uvc_buffer *buf);

    struct {
        struct uvc_stats_frame   frame;
        struct uvc_stats_stream  stream;
    } stats;

    /* ������ɴ������ʹ�õ�����������*/
    struct {
        uint8_t                  header[256];
        uint32_t                 header_size;
        int                      skip_payload;
        uint32_t                 payload_size;
        uint32_t                 max_payload_size;
    } bulk;

    /* ʱ���֧��*/
    struct uvc_clock {                                /* UVCʱ�ӽṹ��*/
        struct uvc_clock_sample {                     /* UVCʱ�Ӳ����ṹ��*/
            uint32_t             dev_stc;
            uint16_t             dev_sof;
            struct usb_timespec  host_ts;             /* ����ʱ���*/
            uint16_t             host_sof;
        } *samples;

        uint32_t                 head;
        uint32_t                 count;

        int16_t                  last_sof;
        int16_t                  sof_offset;
        uint32_t                 size;                /* */
        usb_mutex_handle_t       lock;                /* UVCʱ�ӻ�����*/
    } clock;
};

/** ��ȡUVC�豸��Ƶ������*/
char *uvc_dev_stream_name_get(struct uvc_device *uvc_dev, int idx);
/** ��ȡUVC�豸��Ƶ���ĸ���*/
int uvc_dev_nstream_get(struct uvc_device *p_dev);
/** ��ȡ��������ʽ����*/
int uvc_stream_nformat_get(struct uvc_streaming *stream);
/**
 * ����������ȡ��������ʽ
 *
 * @param stream UVC������
 * @param idx    �ڼ�����ʽ(��СΪ1)
 *
 * @return �ɹ�����UVC��ʽ���ݽṹ
 */
struct uvc_format *uvc_stream_format_get(struct uvc_streaming *stream, int idx);
/** ��ȡ��������ʽ����֡����*/
int uvc_format_nframe_get(struct uvc_format *format);
/**
 * ��ȡ��������ʽ����֡����
 *
 * @param format UVC��ʽ
 * @param idx    �ڼ���֡(��СΪ1)
 *
 * @return �ɹ�����UVC֡���ݽṹ
 */
struct uvc_frame *uvc_format_frame_get(struct uvc_format *format, int idx);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif

