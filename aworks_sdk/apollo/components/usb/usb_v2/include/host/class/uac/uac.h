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
#ifndef __UAC_H_
#define __UAC_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */
/* �ӿ�Э��ֵ��ʾʹ�õı�׼�汾*/
#define UAC_VERSION_1           0x00

/* A.2 ��Ƶ�ӿ��������*/
#define USB_SUBCLASS_AUDIOCONTROL   0x01
#define USB_SUBCLASS_AUDIOSTREAMING 0x02
#define USB_SUBCLASS_MIDISTREAMING  0x03

/* A.5 ��Ƶ���ض��ӿ�����������*/
#define UAC_HEADER              0x01
#define UAC_INPUT_TERMINAL      0x02
#define UAC_OUTPUT_TERMINAL     0x03
#define UAC_MIXER_UNIT          0x04
#define UAC_SELECTOR_UNIT       0x05
#define UAC_FEATURE_UNIT        0x06
#define UAC1_PROCESSING_UNIT    0x07
#define UAC1_EXTENSION_UNIT     0x08

/* A.6 ��Ƶ���ض� AS �ӿ����������� */
#define UAC_AS_GENERAL          0x01
#define UAC_FORMAT_TYPE         0x02
#define UAC_FORMAT_SPECIFIC     0x03

/* A.8 ��Ƶ���ض��˵�����������*/
#define UAC_EP_GENERAL          0x01

/* A.9 ��Ƶ���ض��������*/
#define UAC_SET_			0x00
#define UAC_GET_			0x80

#define UAC__CUR			0x1
#define UAC__MIN			0x2
#define UAC__MAX			0x3
#define UAC__RES			0x4
#define UAC__MEM			0x5

#define UAC_SET_CUR			(UAC_SET_ | UAC__CUR)
#define UAC_GET_CUR			(UAC_GET_ | UAC__CUR)
#define UAC_SET_MIN			(UAC_SET_ | UAC__MIN)
#define UAC_GET_MIN			(UAC_GET_ | UAC__MIN)
#define UAC_SET_MAX			(UAC_SET_ | UAC__MAX)
#define UAC_GET_MAX			(UAC_GET_ | UAC__MAX)
#define UAC_SET_RES			(UAC_SET_ | UAC__RES)
#define UAC_GET_RES			(UAC_GET_ | UAC__RES)
#define UAC_SET_MEM			(UAC_SET_ | UAC__MEM)
#define UAC_GET_MEM			(UAC_GET_ | UAC__MEM)

/* A.10.2 ���Ե�Ԫ����ѡ����*/
#define UAC_FU_MUTE              0x01
#define UAC_FU_VOLUME            0x02
#define UAC_FU_BASS              0x03
#define UAC_FU_MID               0x04
#define UAC_FU_TREBLE            0x05
#define UAC_FU_GRAPHIC_EQUALIZER 0x06
#define UAC_FU_AUTOMATIC_GAIN    0x07
#define UAC_FU_DELAY             0x08
#define UAC_FU_BASS_BOOST        0x09
#define UAC_FU_LOUDNESS          0x0a

/* Terminal Control Selectors */
/* 4.3.2  ��Ƶ�ӿ��ض���������*/
struct uac1_ac_header_descriptor {
    uint8_t  bLength;              /* ���������ȣ�8 + n �ֽ�*/
    uint8_t  bDescriptorType;      /* �ӿ����������� */
    uint8_t  bDescriptorSubtype;   /* �����������ͣ�UAC_MS_HEADER */
    uint16_t bcdADC;               /* 0x0100 */
    uint16_t wTotalLength;         /* ������Ԫ���ն������� */
    uint8_t  bInCollection;        /* ��Ƶ�ӿ����� n */
    uint8_t  baInterfaceNr[];      /* [n] */
} __attribute__ ((packed));

/* 4.3.2.1 �����ն�������*/
struct uac_input_terminal_descriptor {
    uint8_t  bLength;              /* ���������ȣ�12�ֽ�*/
    uint8_t  bDescriptorType;      /* �ӿ����������� */
    uint8_t  bDescriptorSubtype;   /* �����ն�����������*/
    uint8_t  bTerminalID;          /* Ψһ���ն�ID*/
    uint16_t wTerminalType;        /* USB��Ƶ�ն�����*/
    uint8_t  bAssocTerminal;       /* ��������ն�ID*/
    uint8_t  bNrChannels;          /* �߼����ͨ��������*/
    uint16_t wChannelConfig;
    uint8_t  iChannelNames;
    uint8_t  iTerminal;
} __attribute__ ((packed));

/* 4.3.2.2 ����ն�������*/
struct uac1_output_terminal_descriptor {
    uint8_t  bLength;             /* ���������ȣ�9�ֽ�*/
    uint8_t  bDescriptorType;     /* �ӿ����������� */
    uint8_t  bDescriptorSubtype;  /* ����ն�����������*/
    uint8_t  bTerminalID;         /* Ψһ���ն�ID*/
    uint16_t wTerminalType;       /* USB��Ƶ�ն�����*/
    uint8_t  bAssocTerminal;      /* ���������ն�ID*/
    uint8_t  bSourceID;           /* ���ӵĵ�Ԫ���ն˵�ID*/
    uint8_t  iTerminal;
} __attribute__ ((packed));

/* 4.3.2.3 ������Ԫ������*/
struct uac_mixer_unit_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bUnitID;
    uint8_t bNrInPins;
    uint8_t baSourceID[];
} __attribute__ ((packed));

/* 4.3.2.4 ѡ������Ԫ������*/
struct uac_selector_unit_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bUintID;
    uint8_t bNrInPins;
    uint8_t baSourceID[];
} __attribute__ ((packed));

static inline uint8_t uac_selector_unit_iSelector(struct uac_selector_unit_descriptor *desc)
{
    uint8_t *raw = (uint8_t *) desc;
    return raw[desc->bLength - 1];
}

/* 4.3.2.6 ����Ԫ������*/
struct uac_processing_unit_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bUnitID;
    uint16_t wProcessType;
    uint8_t bNrInPins;
    uint8_t baSourceID[];
} __attribute__ ((packed));

static inline uint8_t uac_processing_unit_bNrChannels(struct uac_processing_unit_descriptor *desc)
{
    return desc->baSourceID[desc->bNrInPins];
}

static inline uint32_t uac_processing_unit_wChannelConfig(struct uac_processing_unit_descriptor *desc,
                               int protocol)
{
    if (protocol == UAC_VERSION_1){
        return (desc->baSourceID[desc->bNrInPins + 2] << 8) |
            desc->baSourceID[desc->bNrInPins + 1];
    }else{
        return  (desc->baSourceID[desc->bNrInPins + 4] << 24) |
            (desc->baSourceID[desc->bNrInPins + 3] << 16) |
            (desc->baSourceID[desc->bNrInPins + 2] << 8)  |
            (desc->baSourceID[desc->bNrInPins + 1]);
    }
}

static inline uint8_t uac_processing_unit_bControlSize(struct uac_processing_unit_descriptor *desc,
                            int protocol)
{
    return (protocol == UAC_VERSION_1) ?
        desc->baSourceID[desc->bNrInPins + 4] :
        desc->baSourceID[desc->bNrInPins + 6];
}

static inline uint8_t *uac_processing_unit_bmControls(struct uac_processing_unit_descriptor *desc,
                           int protocol)
{
    return (protocol == UAC_VERSION_1) ?
        &desc->baSourceID[desc->bNrInPins + 5] :
        &desc->baSourceID[desc->bNrInPins + 7];
}

static inline uint8_t uac_processing_unit_iProcessing(struct uac_processing_unit_descriptor *desc,
                           int protocol)
{
    uint8_t control_size = uac_processing_unit_bControlSize(desc, protocol);
    return *(uac_processing_unit_bmControls(desc, protocol)
            + control_size);
}

/* 4.5.2 ���ض���AS�ӿ�������*/
struct uac1_as_header_descriptor {
    uint8_t  bLength;              /* ���������ȣ�7�ֽ�*/
    uint8_t  bDescriptorType;      /* �ӿ�������*/
    uint8_t  bDescriptorSubtype;   /* ͨ��*/
    uint8_t  bTerminalLink;        /* ���ӵ��ն˵��ն�ID*/
    uint8_t  bDelay;               /* ����·��������ӳ�*/
    uint16_t wFormatTag;           /* ��Ƶ���ݸ�ʽ*/
} __attribute__ ((packed));

/* ��ʽ - A.1.1 ��Ƶ���ݸ�ʽ I���� */
#define UAC_FORMAT_TYPE_I_UNDEFINED 0x0
#define UAC_FORMAT_TYPE_I_PCM       0x1
#define UAC_FORMAT_TYPE_I_PCM8      0x2
#define UAC_FORMAT_TYPE_I_IEEE_FLOAT    0x3
#define UAC_FORMAT_TYPE_I_ALAW      0x4
#define UAC_FORMAT_TYPE_I_MULAW     0x5

/* 4.7.2.1 ʱ��Դ������ */
struct uac_clock_source_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bClockID;
    uint8_t bmAttributes;
    uint8_t bmControls;
    uint8_t bAssocTerminal;
    uint8_t iClockSource;
} __attribute__((packed));

/* 4.7.2.3 ʱ���˲���������*/
struct uac_clock_multiplier_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bClockID;
    uint8_t bCSourceID;
    uint8_t bmControls;
    uint8_t iClockMultiplier;
} __attribute__((packed));

/*��ʽ - A.2 ��ʽ���ʹ��� */
#define UAC_FORMAT_TYPE_UNDEFINED   0x0
#define UAC_FORMAT_TYPE_I           0x1
#define UAC_FORMAT_TYPE_II          0x2
#define UAC_FORMAT_TYPE_III         0x3
#define UAC_EXT_FORMAT_TYPE_I       0x81
#define UAC_EXT_FORMAT_TYPE_II      0x82
#define UAC_EXT_FORMAT_TYPE_III     0x83

/* UAC��ʱ�˵�������*/
struct uac_iso_endpoint_descriptor {
    uint8_t  bLength;            /* ���������ȣ�7�ֽ� */
    uint8_t  bDescriptorType;    /* ����������*/
    uint8_t  bDescriptorSubtype; /* ���������ࣺͨ�ö˵�*/
    uint8_t  bmAttributes;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} __attribute__((packed));

#define UAC_EP_CS_ATTR_SAMPLE_RATE      0x01
#define UAC_EP_CS_ATTR_PITCH_CONTROL    0x02
#define UAC_EP_CS_ATTR_FILL_MAX         0x80

/* ��ʽ - A.1.1 ��Ƶ���ݸ�ʽ����I����*/
#define UAC_FORMAT_TYPE_I_UNDEFINED     0x0
#define UAC_FORMAT_TYPE_I_PCM           0x1
#define UAC_FORMAT_TYPE_I_PCM8          0x2
#define UAC_FORMAT_TYPE_I_IEEE_FLOAT    0x3
#define UAC_FORMAT_TYPE_I_ALAW          0x4
#define UAC_FORMAT_TYPE_I_MULAW         0x5

struct uac_format_type_i_continuous_descriptor {
    uint8_t  bLength;              /* ����������(�ֽ�): 8 + (ns * 3) */
    uint8_t  bDescriptorType;      /* �ӿ�������*/
    uint8_t  bDescriptorSubtype;   /* ���������ࣺFORMAT_TYPE */
    uint8_t  bFormatType;          /* FORMAT_TYPE_1 */
    uint8_t  bNrChannels;          /* ����������ͨ��*/
    uint8_t  bSubframeSize;        /* */
    uint8_t  bBitResolution;
    uint8_t  bSamFreqType;
    uint8_t  tLowerSamFreq[3];
    uint8_t  tUpperSamFreq[3];
} __attribute__ ((packed));

struct uac_format_type_i_discrete_descriptor {
    uint8_t  bLength;             /* ����������(�ֽ�): 8 + (ns * 3) */
    uint8_t  bDescriptorType;     /* �ӿ�������*/
    uint8_t  bDescriptorSubtype;  /* ���������ࣺFORMAT_TYPE */
    uint8_t  bFormatType;         /* FORMAT_TYPE_1 */
    uint8_t  bNrChannels;         /* ����������ͨ��*/
    uint8_t  bSubframeSize;       /* */
    uint8_t  bBitResolution;
    uint8_t  bSamFreqType;
    uint8_t  tSamFreq[][3];
} __attribute__ ((packed));

struct uac_format_type_i_ext_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatType;
    uint8_t bSubslotSize;
    uint8_t bBitResolution;
    uint8_t bHeaderLength;
    uint8_t bControlSize;
    uint8_t bSideBandProtocol;
} __attribute__((packed));

struct uac_format_type_ii_discrete_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatType;
    uint16_t wMaxBitRate;
    uint16_t wSamplesPerFrame;
    uint8_t  bSamFreqType;
    uint8_t  tSamFreq[][3];
} __attribute__((packed));

struct uac_format_type_ii_ext_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatType;
    uint16_t wMaxBitRate;
    uint16_t wSamplesPerFrame;
    uint8_t  bHeaderLength;
    uint8_t  bSideBandProtocol;
} __attribute__((packed));

/* ��ʽ - ��Ƶ���ݸ�ʽ����I����*/
#define UAC_FORMAT_TYPE_II_MPEG 0x1001
#define UAC_FORMAT_TYPE_II_AC3  0x1002

//////////////////////////////////////////////////////////////////////////////////

/* �ӿ�Э��ֵ��ʾʹ�õı�׼�汾*/
#define UAC_VERSION_2           0x20

/*
 * bmControl �ֶν���
 *
 * USB ��Ƶ�淶v2.0:
 *
 *   bmaControls() ��һ�� (ch+1)-4�ֽ�λͼ�����飬
 *   ÿ������һ��λ��ԡ�����ؼ����ڣ��������������ɶ��������ǰ�Ŀؼ�������
 *   ��λ�Ա�������Ϊ0b00��
 *   ���һ���ؼ����ڵ���ֻ���ģ�λ�Ա�������Ϊ0b01�����һ���ؼ��������ɱ�̣�
 *   λ�Ա�������Ϊ0b11��ֵ0b10�ǲ�����ġ�
 */
static inline usb_bool_t uac2_control_is_readable(uint32_t bmControls, uint8_t control)
{
    return (bmControls >> (control * 2)) & 0x1;
}

/* A.9 ��Ƶ���ض� AC�ӿ�����������*/
/* UAC�汾2����*/
#define UAC2_EFFECT_UNIT            0x07
#define UAC2_PROCESSING_UNIT_V2     0x08
#define UAC2_EXTENSION_UNIT_V2      0x09
#define UAC2_CLOCK_SOURCE           0x0a
#define UAC2_CLOCK_SELECTOR         0x0b
#define UAC2_CLOCK_MULTIPLIER       0x0c
#define UAC2_SAMPLE_RATE_CONVERTER  0x0d

/* 4.3.2.5 ���Ե�Ԫ������*/
struct uac_feature_unit_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bUnitID;
    uint8_t bSourceID;
    uint8_t bControlSize;
    uint8_t bmaControls[0]; /* �������� */
} __attribute__((packed));

/* 4.7.2.2 ʱ��Դ������*/
struct uac_clock_selector_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bClockID;
    uint8_t bNrInPins;
    uint8_t baCSourceID[];
    /* bmControls, bAssocTerminal ��  iClockSourceʡ�� */
} __attribute__((packed));

/* 4.7.2.4 �����ж�������(UAC�汾2) */
struct uac2_input_terminal_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bCSourceID;
    uint8_t  bNrChannels;
    uint32_t bmChannelConfig;
    uint8_t  iChannelNames;
    uint16_t bmControls;
    uint8_t  iTerminal;
} __attribute__((packed));

/* 4.10.1.2 ���ض��� AS��ʱ ��Ƶ���ݶ˵�������*/
struct uac2_iso_endpoint_descriptor {
    uint8_t  bLength;              /* ����������: 8�ֽ�*/
    uint8_t  bDescriptorType;      /* �˵������� */
    uint8_t  bDescriptorSubtype;   /* ���������ࣺͨ�ö˵�*/
    uint8_t  bmAttributes;
    uint8_t  bmControls;
    uint8_t  bLockDelayUnits;
    uint16_t wLockDelay;
} __attribute__((packed));

/* 4.9.2 Class-Specific AS Interface Descriptor */
struct uac2_as_header_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalLink;
    uint8_t  bmControls;
    uint8_t  bFormatType;
    uint32_t bmFormats;
    uint8_t  bNrChannels;
    uint32_t bmChannelConfig;
    uint8_t  iChannelNames;
} __attribute__((packed));

#define UAC2_FORMAT_TYPE_I_RAW_DATA (1 << 31)

#define UAC2_CONTROL_PITCH          (3 << 0)
#define UAC2_CONTROL_DATA_OVERRUN   (3 << 2)
#define UAC2_CONTROL_DATA_UNDERRUN  (3 << 4)

/* A.14 ��Ƶ���ض��������*/
#define UAC2_CS_CUR         0x01
#define UAC2_CS_RANGE       0x02
#define UAC2_CS_MEM         0x03

/* A.17.1 ʱ��Դ�ؼ�ѡ���� */
#define UAC2_CS_UNDEFINED           0x00
#define UAC2_CS_CONTROL_SAM_FREQ    0x01
#define UAC2_CS_CONTROL_CLOCK_VALID 0x02

/* A.17.2 ʱ��ѡ�����ؼ�ѡ���� */
#define UAC2_CX_UNDEFINED           0x00
#define UAC2_CX_CLOCK_SELECTOR      0x01
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

