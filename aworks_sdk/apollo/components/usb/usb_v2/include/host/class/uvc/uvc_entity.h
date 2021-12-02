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
#ifndef __UVC_ENTITY_H
#define __UVC_ENTITY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
#include "host/class/uvc/uvc_driver.h"

/* 获取UVC控件实体类型*/
#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)

/* UVC控件数据的数据类型*/
#define UVC_CTRL_DATA_TYPE_RAW      0
#define UVC_CTRL_DATA_TYPE_SIGNED   1
#define UVC_CTRL_DATA_TYPE_UNSIGNED 2
#define UVC_CTRL_DATA_TYPE_BOOLEAN  3
#define UVC_CTRL_DATA_TYPE_ENUM     4
#define UVC_CTRL_DATA_TYPE_BITMASK  5

/* 控件标志*/
#define UVC_CTRL_FLAG_SET_CUR       (1 << 0)     /* 设置当前值*/
#define UVC_CTRL_FLAG_GET_CUR       (1 << 1)     /* 获取当前值*/
#define UVC_CTRL_FLAG_GET_MIN       (1 << 2)     /* 获取最小值*/
#define UVC_CTRL_FLAG_GET_MAX       (1 << 3)     /* 获取最大值*/
#define UVC_CTRL_FLAG_GET_RES       (1 << 4)
#define UVC_CTRL_FLAG_GET_DEF       (1 << 5)
/* 控件应该在挂起时保存，在恢复时还原*/
#define UVC_CTRL_FLAG_RESTORE       (1 << 6)
/* 控件可以通过摄像头更新 */
#define UVC_CTRL_FLAG_AUTO_UPDATE   (1 << 7)
/* 获取控件范围值*/
#define UVC_CTRL_FLAG_GET_RANGE \
    (UVC_CTRL_FLAG_GET_CUR | UVC_CTRL_FLAG_GET_MIN | \
     UVC_CTRL_FLAG_GET_MAX | UVC_CTRL_FLAG_GET_RES | \
     UVC_CTRL_FLAG_GET_DEF)
/* 控件索引掩码*/
#define UVC_CTRL_ID_MASK         (0x0fffffff)
/* 寻找下一个控件标志*/
#define UVC_CTRL_FLAG_NEXT_CTRL    0x80000000

/* A.5 特定视频类VC接口描述符子类型 */
#define UVC_VC_DESCRIPTOR_UNDEFINED         0x00       /* UVC未定义描述符类型*/
#define UVC_VC_HEADER                       0x01       /* UVC头描述符*/
#define UVC_VC_INPUT_TERMINAL               0x02       /* UVC输入终端描述符*/
#define UVC_VC_OUTPUT_TERMINAL              0x03       /* UVC输出终端描述符*/
#define UVC_VC_SELECTOR_UNIT                0x04       /* UVC选择器单元描述符*/
#define UVC_VC_PROCESSING_UNIT              0x05       /* UVC处理单元描述符*/
#define UVC_VC_EXTENSION_UNIT               0x06       /* UVC扩展单元描述符*/
/* A.6. 特定视频类VC接口描述符子类型 */
#define UVC_VS_UNDEFINED                    0x00       /* UVC未定义描述符*/
#define UVC_VS_INPUT_HEADER                 0x01       /* UVC输入头描述符*/
#define UVC_VS_OUTPUT_HEADER                0x02       /* UVC输出头描述符*/
#define UVC_VS_STILL_IMAGE_FRAME            0x03
#define UVC_VS_FORMAT_UNCOMPRESSED          0x04       /* 无压缩视频格式*/
#define UVC_VS_FRAME_UNCOMPRESSED           0x05       /* 无压缩帧*/
#define UVC_VS_FORMAT_MJPEG                 0x06       /* MJPEG格式*/
#define UVC_VS_FRAME_MJPEG                  0x07       /* MJPEG帧*/
#define UVC_VS_FORMAT_MPEG2TS               0x0a       /* MPEG2-TS格式*/
#define UVC_VS_FORMAT_DV                    0x0c       /* DV格式*/
#define UVC_VS_COLORFORMAT                  0x0d
#define UVC_VS_FORMAT_FRAME_BASED           0x10       /* 供应商自定义格式*/
#define UVC_VS_FRAME_FRAME_BASED            0x11       /* 通用基本帧*/
#define UVC_VS_FORMAT_STREAM_BASED          0x12       /* MPEG1-SS/MPEG2-PS格式*/
/* A.8.视频特定类请求代码*/
#define UVC_RC_UNDEFINED            0x00
#define UVC_SET_CUR                 0x01
#define UVC_GET_CUR                 0x81          /* 返回要协商的字段的默认值*/
#define UVC_GET_MIN                 0x82          /* 返回要协商的字段的最小值*/
#define UVC_GET_MAX                 0x83          /* 返回要协商的字段的最大值*/
#define UVC_GET_RES                 0x84
#define UVC_GET_LEN                 0x85
#define UVC_GET_INFO                0x86
#define UVC_GET_DEF                 0x87
/* A.9.4. 摄像头终端控制选择器*/
#define UVC_CT_CONTROL_UNDEFINED                0x00
#define UVC_CT_SCANNING_MODE_CONTROL            0x01
#define UVC_CT_AE_MODE_CONTROL                  0x02
#define UVC_CT_AE_PRIORITY_CONTROL              0x03
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   0x04
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL   0x05
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL           0x06
#define UVC_CT_FOCUS_RELATIVE_CONTROL           0x07
#define UVC_CT_FOCUS_AUTO_CONTROL               0x08
#define UVC_CT_IRIS_ABSOLUTE_CONTROL            0x09
#define UVC_CT_IRIS_RELATIVE_CONTROL            0x0a
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL            0x0b
#define UVC_CT_ZOOM_RELATIVE_CONTROL            0x0c
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL         0x0d
#define UVC_CT_PANTILT_RELATIVE_CONTROL         0x0e
#define UVC_CT_ROLL_ABSOLUTE_CONTROL            0x0f
#define UVC_CT_ROLL_RELATIVE_CONTROL            0x10
#define UVC_CT_PRIVACY_CONTROL                  0x11

/* A.9.5. 处理单元控制选择器*/
#define UVC_PU_CONTROL_UNDEFINED                      0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL         0x01
#define UVC_PU_BRIGHTNESS_CONTROL                     0x02
#define UVC_PU_CONTRAST_CONTROL                       0x03
#define UVC_PU_GAIN_CONTROL                           0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL           0x05
#define UVC_PU_HUE_CONTROL                            0x06
#define UVC_PU_SATURATION_CONTROL                     0x07
#define UVC_PU_SHARPNESS_CONTROL                      0x08
#define UVC_PU_GAMMA_CONTROL                          0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL 0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL        0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL             0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       0x0f
#define UVC_PU_HUE_AUTO_CONTROL                       0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL          0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL             0x12

/* B.1. USB终端类型*/
#define UVC_TT_VENDOR_SPECIFIC              0x0100    /* 特定供应商终端类型*/
#define UVC_TT_STREAMING                    0x0101    /* 流终端类型*/

/* B.2. 输入终端类型*/
#define UVC_ITT_VENDOR_SPECIFIC             0x0200    /* 供应商特定输入终端*/
#define UVC_ITT_CAMERA                      0x0201    /* 摄像头传感器。仅用于摄像头终端描述符*/
#define UVC_ITT_MEDIA_TRANSPORT_INPUT       0x0202    /* 序列媒体。仅用于媒体传输终端描述符*/

/* B.3. 输出终端类型*/
#define UVC_OTT_VENDOR_SPECIFIC             0x0300    /* 特定供应商输出终端*/
#define UVC_OTT_DISPLAY                     0x0301    /* 显示输出终端*/
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT      0x0302    /* 媒体输出终端*/

/* UVC控件类型*/
enum uvc_ctrl_type {
    UVC_CTRL_TYPE_INTEGER       = 1,
    UVC_CTRL_TYPE_BOOLEAN       = 2,
    UVC_CTRL_TYPE_MENU          = 3,
    UVC_CTRL_TYPE_BUTTON        = 4,
    UVC_CTRL_TYPE_INTEGER64     = 5,
    UVC_CTRL_TYPE_CTRL_CLASS    = 6,
    UVC_CTRL_TYPE_STRING        = 7,
    UVC_CTRL_TYPE_BITMASK       = 8,
    UVC_CTRL_TYPE_INTEGER_MENU  = 9,

    /* 命令类型是 >= 0x0100*/
    UVC_CTRL_COMPOUND_TYPES = 0x0100,
    UVC_CTRL_TYPE_U8        = 0x0100,
    UVC_CTRL_TYPE_U16       = 0x0101,
    UVC_CTRL_TYPE_U32       = 0x0102,
};


#define UVC_TERM_INPUT          0x0000                                  /* 输入终端*/
#define UVC_TERM_OUTPUT         0x8000                                  /* 输出终端*/
#define UVC_TERM_DIRECTION(term)    ((term)->type & 0x8000)             /* 检查终端方向*/

#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)           /* 检查UVC实体类型*/
#define UVC_ENTITY_IS_UNIT(entity)  (((entity)->type & 0xff00) == 0)    /* 检查UVC实体是不是一个单元*/
#define UVC_ENTITY_IS_TERM(entity)  (((entity)->type & 0xff00) != 0)    /* 检查UVC实体是不是一个终端*/
/* 判断实体是不是输入终端类型*/
#define UVC_ENTITY_IS_ITERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_INPUT)
/* 判断实体是不是输出终端类型*/
#define UVC_ENTITY_IS_OTERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_OUTPUT)


#define MEDIA_PAD_FL_SINK           (1 << 0)
#define MEDIA_PAD_FL_SOURCE         (1 << 1)
#define MEDIA_PAD_FL_MUST_CONNECT   (1 << 2)

#define UVC_ENTITY_FLAG_DEFAULT     (1 << 0)

/* 媒体PAD结构体*/
struct media_pad {
    uint16_t      index;            /* 实体PAD数组的PAD索引*/
    unsigned long flags;            /* PAD索引 (MEDIA_PAD_FL_*) */
};

struct uvc_menu_info {
    uint32_t value;
    uint8_t  name[32];
};

/* UVC控件信息结构体*/
struct uvc_control_info {
    struct usb_list_head mappings;   /* 控件图链表*/

    uint8_t              entity[16]; /* UVC实体GUID(全局唯一标识符)*/
    uint8_t              index;      /* bmControls的位索引 */
    uint8_t              selector;   /* 选择项*/

    uint16_t             size;       /* 大小*/
    uint32_t             flags;      /* 标志*/
};

/* UVC控件映射结构体*/
struct uvc_control_mapping {
    struct usb_list_head  list;
    struct usb_list_head  ev_subs;

    uint32_t              id;
    uint8_t               name[32];
    uint8_t               entity[16];
    uint8_t               selector;

    uint8_t               size;
    uint8_t               offset;
    enum uvc_ctrl_type    ctrl_type;
    uint32_t              data_type;

    struct uvc_menu_info *menu_info;
    uint32_t              menu_count;

    uint32_t              master_id;
    int                   master_manual;
    uint32_t              slave_ids[2];

    int (*get) (struct uvc_control_mapping *mapping, uint8_t query,
              const uint8_t *data);
    void (*set) (struct uvc_control_mapping *mapping, int value,
            uint8_t *data);
};

struct uvc_control;
/* UVC实体结构体*/
struct uvc_entity {
    struct usb_list_head list;                     /* 当前UVC实体节点*/
    struct usb_list_head chain;                    /* 视频设备链节点*/

    uint32_t             flags;                    /* UVC实体标志*/

    uint8_t              id;                       /* UVC实体ID*/
    uint16_t             type;                     /* UVC实体类型*/
    char                 name[64];                 /* UVC实体名字*/

    uint32_t             num_pads;
    uint32_t             num_links;
    struct media_pad    *pads;

    union {
        /* 摄像头信息*/
        struct {
            uint16_t     wObjectiveFocalLengthMin; /* L(min)的值，如果不支持光学变焦，则为0*/
            uint16_t     wObjectiveFocalLengthMax; /* L(max)的值，如果不支持光学变焦，则为0*/
            uint16_t     wOcularFocalLength;       /* L(ocular)的值，如果不支持光学变焦，则为0*/
            uint8_t      bControlSize;             /* 位图大小*/
            uint8_t     *bmControls;               /* bmControls 是一个位图，表示视频流的某些摄像头控件的可用性*/
        } camera;
        /* 媒体*/
        struct {
            uint8_t      bControlSize;
            uint8_t     *bmControls;
            uint8_t      bTransportModeSize;
            uint8_t     *bmTransportModes;
        } media;
        /* 输出终端*/
        struct {
        } output;
        /* 处理单元*/
        struct {
            uint16_t     wMaxMultiplier;            /* 数字倍增(此字段指示最大数字倍增率乘以100)*/
            uint8_t      bControlSize;              /* 控件位图的大小*/
            uint8_t     *bmControls;                /* 控件位图地址*/
            uint8_t      bmVideoStandards;          /* 处理单元支持的所有模拟视频标准的位图*/
        } processing;
        /* 选择器单元*/
        struct {
        } selector;
        /* 扩展单元*/
        struct {
            uint8_t      guidExtensionCode[16];     /* 特定供应商的代码*/
            uint8_t      bNumControls;              /* 扩展单元的控件数量*/
            uint8_t      bControlSize;              /* 控件位图的大小*/
            uint8_t     *bmControls;                /* 控件位图的地址*/
            uint8_t     *bmControlsType;
        } extension;
    };
    uint8_t              bNrInPins;                 /* 输入引脚个数*/
    uint8_t             *baSourceID;                /* 连接点ID*/

    uint32_t             ncontrols;                 /* 控件数量*/
    struct uvc_control  *controls;                  /* UVC控件结构体*/
};

/* UVC控件结构体*/
struct uvc_control {
    struct uvc_entity      *entity;                 /* 所属的UVC实体*/
    struct uvc_control_info info;                   /* 控件信息结构体*/

    uint8_t                 index;                  /* 用 来匹配 uvc_control 入口和 uvc_control_info*/
    uint8_t                 dirty      :1,
                            loaded     :1,
                            modified   :1,
                            cached     :1,
                            initialized:1;          /* 初始化完成标志*/

    uint8_t                 *uvc_data;
};

/* uvc视频链结构体*/
struct uvc_video_chain {
    struct uvc_device   *dev;       /* 所属的uvc设备结构体*/
    struct usb_list_head list;      /* 当前视频链节点*/

    struct usb_list_head entities;  /* 所有实体 */
    struct uvc_entity   *processing;/* 处理单元*/
    struct uvc_entity   *selector;  /* 选择单元*/

    usb_mutex_handle_t   mutex;     /* 保护控件信息*/
    uint32_t             caps;      /* chain-wide caps */
};

/** 获取终端数量*/
int uvc_entity_num_get(struct uvc_device *uvc_dev);
/** 根据索引获取具体终端*/
struct uvc_entity *uvc_entity_get(struct uvc_device *uvc_dev, int idx);
/** 获取控件数量*/
int uvc_control_num_get(struct uvc_entity *entity);
/** 根据索引获取控件*/
struct uvc_control *uvc_control_get(struct uvc_entity *entity, int idx);
/** 获取控件映射数量*/
int uvc_control_mapping_num_get(struct uvc_control *ctrl);
/** 通过索引获取控件映射名字*/
char *uvc_control_mapping_name_get(struct uvc_control *ctrl, int idx);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif


