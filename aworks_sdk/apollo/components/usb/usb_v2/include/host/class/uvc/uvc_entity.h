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

/* ��ȡUVC�ؼ�ʵ������*/
#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)

/* UVC�ؼ����ݵ���������*/
#define UVC_CTRL_DATA_TYPE_RAW      0
#define UVC_CTRL_DATA_TYPE_SIGNED   1
#define UVC_CTRL_DATA_TYPE_UNSIGNED 2
#define UVC_CTRL_DATA_TYPE_BOOLEAN  3
#define UVC_CTRL_DATA_TYPE_ENUM     4
#define UVC_CTRL_DATA_TYPE_BITMASK  5

/* �ؼ���־*/
#define UVC_CTRL_FLAG_SET_CUR       (1 << 0)     /* ���õ�ǰֵ*/
#define UVC_CTRL_FLAG_GET_CUR       (1 << 1)     /* ��ȡ��ǰֵ*/
#define UVC_CTRL_FLAG_GET_MIN       (1 << 2)     /* ��ȡ��Сֵ*/
#define UVC_CTRL_FLAG_GET_MAX       (1 << 3)     /* ��ȡ���ֵ*/
#define UVC_CTRL_FLAG_GET_RES       (1 << 4)
#define UVC_CTRL_FLAG_GET_DEF       (1 << 5)
/* �ؼ�Ӧ���ڹ���ʱ���棬�ڻָ�ʱ��ԭ*/
#define UVC_CTRL_FLAG_RESTORE       (1 << 6)
/* �ؼ�����ͨ������ͷ���� */
#define UVC_CTRL_FLAG_AUTO_UPDATE   (1 << 7)
/* ��ȡ�ؼ���Χֵ*/
#define UVC_CTRL_FLAG_GET_RANGE \
    (UVC_CTRL_FLAG_GET_CUR | UVC_CTRL_FLAG_GET_MIN | \
     UVC_CTRL_FLAG_GET_MAX | UVC_CTRL_FLAG_GET_RES | \
     UVC_CTRL_FLAG_GET_DEF)
/* �ؼ���������*/
#define UVC_CTRL_ID_MASK         (0x0fffffff)
/* Ѱ����һ���ؼ���־*/
#define UVC_CTRL_FLAG_NEXT_CTRL    0x80000000

/* A.5 �ض���Ƶ��VC�ӿ������������� */
#define UVC_VC_DESCRIPTOR_UNDEFINED         0x00       /* UVCδ��������������*/
#define UVC_VC_HEADER                       0x01       /* UVCͷ������*/
#define UVC_VC_INPUT_TERMINAL               0x02       /* UVC�����ն�������*/
#define UVC_VC_OUTPUT_TERMINAL              0x03       /* UVC����ն�������*/
#define UVC_VC_SELECTOR_UNIT                0x04       /* UVCѡ������Ԫ������*/
#define UVC_VC_PROCESSING_UNIT              0x05       /* UVC����Ԫ������*/
#define UVC_VC_EXTENSION_UNIT               0x06       /* UVC��չ��Ԫ������*/
/* A.6. �ض���Ƶ��VC�ӿ������������� */
#define UVC_VS_UNDEFINED                    0x00       /* UVCδ����������*/
#define UVC_VS_INPUT_HEADER                 0x01       /* UVC����ͷ������*/
#define UVC_VS_OUTPUT_HEADER                0x02       /* UVC���ͷ������*/
#define UVC_VS_STILL_IMAGE_FRAME            0x03
#define UVC_VS_FORMAT_UNCOMPRESSED          0x04       /* ��ѹ����Ƶ��ʽ*/
#define UVC_VS_FRAME_UNCOMPRESSED           0x05       /* ��ѹ��֡*/
#define UVC_VS_FORMAT_MJPEG                 0x06       /* MJPEG��ʽ*/
#define UVC_VS_FRAME_MJPEG                  0x07       /* MJPEG֡*/
#define UVC_VS_FORMAT_MPEG2TS               0x0a       /* MPEG2-TS��ʽ*/
#define UVC_VS_FORMAT_DV                    0x0c       /* DV��ʽ*/
#define UVC_VS_COLORFORMAT                  0x0d
#define UVC_VS_FORMAT_FRAME_BASED           0x10       /* ��Ӧ���Զ����ʽ*/
#define UVC_VS_FRAME_FRAME_BASED            0x11       /* ͨ�û���֡*/
#define UVC_VS_FORMAT_STREAM_BASED          0x12       /* MPEG1-SS/MPEG2-PS��ʽ*/
/* A.8.��Ƶ�ض����������*/
#define UVC_RC_UNDEFINED            0x00
#define UVC_SET_CUR                 0x01
#define UVC_GET_CUR                 0x81          /* ����ҪЭ�̵��ֶε�Ĭ��ֵ*/
#define UVC_GET_MIN                 0x82          /* ����ҪЭ�̵��ֶε���Сֵ*/
#define UVC_GET_MAX                 0x83          /* ����ҪЭ�̵��ֶε����ֵ*/
#define UVC_GET_RES                 0x84
#define UVC_GET_LEN                 0x85
#define UVC_GET_INFO                0x86
#define UVC_GET_DEF                 0x87
/* A.9.4. ����ͷ�ն˿���ѡ����*/
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

/* A.9.5. ����Ԫ����ѡ����*/
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

/* B.1. USB�ն�����*/
#define UVC_TT_VENDOR_SPECIFIC              0x0100    /* �ض���Ӧ���ն�����*/
#define UVC_TT_STREAMING                    0x0101    /* ���ն�����*/

/* B.2. �����ն�����*/
#define UVC_ITT_VENDOR_SPECIFIC             0x0200    /* ��Ӧ���ض������ն�*/
#define UVC_ITT_CAMERA                      0x0201    /* ����ͷ������������������ͷ�ն�������*/
#define UVC_ITT_MEDIA_TRANSPORT_INPUT       0x0202    /* ����ý�塣������ý�崫���ն�������*/

/* B.3. ����ն�����*/
#define UVC_OTT_VENDOR_SPECIFIC             0x0300    /* �ض���Ӧ������ն�*/
#define UVC_OTT_DISPLAY                     0x0301    /* ��ʾ����ն�*/
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT      0x0302    /* ý������ն�*/

/* UVC�ؼ�����*/
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

    /* ���������� >= 0x0100*/
    UVC_CTRL_COMPOUND_TYPES = 0x0100,
    UVC_CTRL_TYPE_U8        = 0x0100,
    UVC_CTRL_TYPE_U16       = 0x0101,
    UVC_CTRL_TYPE_U32       = 0x0102,
};


#define UVC_TERM_INPUT          0x0000                                  /* �����ն�*/
#define UVC_TERM_OUTPUT         0x8000                                  /* ����ն�*/
#define UVC_TERM_DIRECTION(term)    ((term)->type & 0x8000)             /* ����ն˷���*/

#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)           /* ���UVCʵ������*/
#define UVC_ENTITY_IS_UNIT(entity)  (((entity)->type & 0xff00) == 0)    /* ���UVCʵ���ǲ���һ����Ԫ*/
#define UVC_ENTITY_IS_TERM(entity)  (((entity)->type & 0xff00) != 0)    /* ���UVCʵ���ǲ���һ���ն�*/
/* �ж�ʵ���ǲ��������ն�����*/
#define UVC_ENTITY_IS_ITERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_INPUT)
/* �ж�ʵ���ǲ�������ն�����*/
#define UVC_ENTITY_IS_OTERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_OUTPUT)


#define MEDIA_PAD_FL_SINK           (1 << 0)
#define MEDIA_PAD_FL_SOURCE         (1 << 1)
#define MEDIA_PAD_FL_MUST_CONNECT   (1 << 2)

#define UVC_ENTITY_FLAG_DEFAULT     (1 << 0)

/* ý��PAD�ṹ��*/
struct media_pad {
    uint16_t      index;            /* ʵ��PAD�����PAD����*/
    unsigned long flags;            /* PAD���� (MEDIA_PAD_FL_*) */
};

struct uvc_menu_info {
    uint32_t value;
    uint8_t  name[32];
};

/* UVC�ؼ���Ϣ�ṹ��*/
struct uvc_control_info {
    struct usb_list_head mappings;   /* �ؼ�ͼ����*/

    uint8_t              entity[16]; /* UVCʵ��GUID(ȫ��Ψһ��ʶ��)*/
    uint8_t              index;      /* bmControls��λ���� */
    uint8_t              selector;   /* ѡ����*/

    uint16_t             size;       /* ��С*/
    uint32_t             flags;      /* ��־*/
};

/* UVC�ؼ�ӳ��ṹ��*/
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
/* UVCʵ��ṹ��*/
struct uvc_entity {
    struct usb_list_head list;                     /* ��ǰUVCʵ��ڵ�*/
    struct usb_list_head chain;                    /* ��Ƶ�豸���ڵ�*/

    uint32_t             flags;                    /* UVCʵ���־*/

    uint8_t              id;                       /* UVCʵ��ID*/
    uint16_t             type;                     /* UVCʵ������*/
    char                 name[64];                 /* UVCʵ������*/

    uint32_t             num_pads;
    uint32_t             num_links;
    struct media_pad    *pads;

    union {
        /* ����ͷ��Ϣ*/
        struct {
            uint16_t     wObjectiveFocalLengthMin; /* L(min)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint16_t     wObjectiveFocalLengthMax; /* L(max)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint16_t     wOcularFocalLength;       /* L(ocular)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint8_t      bControlSize;             /* λͼ��С*/
            uint8_t     *bmControls;               /* bmControls ��һ��λͼ����ʾ��Ƶ����ĳЩ����ͷ�ؼ��Ŀ�����*/
        } camera;
        /* ý��*/
        struct {
            uint8_t      bControlSize;
            uint8_t     *bmControls;
            uint8_t      bTransportModeSize;
            uint8_t     *bmTransportModes;
        } media;
        /* ����ն�*/
        struct {
        } output;
        /* ����Ԫ*/
        struct {
            uint16_t     wMaxMultiplier;            /* ���ֱ���(���ֶ�ָʾ������ֱ����ʳ���100)*/
            uint8_t      bControlSize;              /* �ؼ�λͼ�Ĵ�С*/
            uint8_t     *bmControls;                /* �ؼ�λͼ��ַ*/
            uint8_t      bmVideoStandards;          /* ����Ԫ֧�ֵ�����ģ����Ƶ��׼��λͼ*/
        } processing;
        /* ѡ������Ԫ*/
        struct {
        } selector;
        /* ��չ��Ԫ*/
        struct {
            uint8_t      guidExtensionCode[16];     /* �ض���Ӧ�̵Ĵ���*/
            uint8_t      bNumControls;              /* ��չ��Ԫ�Ŀؼ�����*/
            uint8_t      bControlSize;              /* �ؼ�λͼ�Ĵ�С*/
            uint8_t     *bmControls;                /* �ؼ�λͼ�ĵ�ַ*/
            uint8_t     *bmControlsType;
        } extension;
    };
    uint8_t              bNrInPins;                 /* �������Ÿ���*/
    uint8_t             *baSourceID;                /* ���ӵ�ID*/

    uint32_t             ncontrols;                 /* �ؼ�����*/
    struct uvc_control  *controls;                  /* UVC�ؼ��ṹ��*/
};

/* UVC�ؼ��ṹ��*/
struct uvc_control {
    struct uvc_entity      *entity;                 /* ������UVCʵ��*/
    struct uvc_control_info info;                   /* �ؼ���Ϣ�ṹ��*/

    uint8_t                 index;                  /* �� ��ƥ�� uvc_control ��ں� uvc_control_info*/
    uint8_t                 dirty      :1,
                            loaded     :1,
                            modified   :1,
                            cached     :1,
                            initialized:1;          /* ��ʼ����ɱ�־*/

    uint8_t                 *uvc_data;
};

/* uvc��Ƶ���ṹ��*/
struct uvc_video_chain {
    struct uvc_device   *dev;       /* ������uvc�豸�ṹ��*/
    struct usb_list_head list;      /* ��ǰ��Ƶ���ڵ�*/

    struct usb_list_head entities;  /* ����ʵ�� */
    struct uvc_entity   *processing;/* ����Ԫ*/
    struct uvc_entity   *selector;  /* ѡ��Ԫ*/

    usb_mutex_handle_t   mutex;     /* �����ؼ���Ϣ*/
    uint32_t             caps;      /* chain-wide caps */
};

/** ��ȡ�ն�����*/
int uvc_entity_num_get(struct uvc_device *uvc_dev);
/** ����������ȡ�����ն�*/
struct uvc_entity *uvc_entity_get(struct uvc_device *uvc_dev, int idx);
/** ��ȡ�ؼ�����*/
int uvc_control_num_get(struct uvc_entity *entity);
/** ����������ȡ�ؼ�*/
struct uvc_control *uvc_control_get(struct uvc_entity *entity, int idx);
/** ��ȡ�ؼ�ӳ������*/
int uvc_control_mapping_num_get(struct uvc_control *ctrl);
/** ͨ��������ȡ�ؼ�ӳ������*/
char *uvc_control_mapping_name_get(struct uvc_control *ctrl, int idx);
#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif


