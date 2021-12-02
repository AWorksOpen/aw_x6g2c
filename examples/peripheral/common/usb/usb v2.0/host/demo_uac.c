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
/**
 *
 * \brief ͨ��USB��Ƶ�豸��ʾ����
 *
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBH���USBHЭ��ջ
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. U������һ��test.wav���ֵ���Ƶ�ļ���USB-HOST2�˿ڲ���U�̣�
 *      USB-HOST1����ͨ��USB��Ƶ�豸��
 *
 * - ʵ������
 *   1. USB��Ƶ�豸������Ƶ�ļ�
 *
 */

#include "Aworks.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_system.h"
#include "aw_delay.h"
#include "aw_types.h"
#include "aw_task.h"
#include "host/core/usbh.h"
#include "host/class/uac/uac_driver.h"
#include "host/class/uac/uac_opt.h"
#include "aw_demo_config.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "fs/aw_blk_dev.h"
#include <string.h>


/* ������Ƶ�ļ�*/
#define UAC_PLAY_FILE

#ifndef UAC_PLAY_FILE
/* ��Ƶ�������� */
extern const unsigned char g_sound_wav_data[];
extern const unsigned int  g_sound_wav_data_len;
//extern const unsigned char g_sound_wav_data1[];
//extern const unsigned int  g_sound_wav_data_len1;
#else
#define UAC_TEST_FILE_PATH  "/u/test.wav"
#define BUFF_SIZE           20*1024
#endif
/* ���ݻ���*/
char *buf = NULL;
/* �ź���*/
AW_SEMB_DECL (ms_sync);
AW_SEMB_DECL (audio_sync);
/* �Ƿ����U��*/
static aw_bool_t is_mount = AW_FALSE;
/* U�̿��豸����*/
#define __BLK_NAME    DE_USB_BLK_NAME

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

/* USB��Ƶ�û���������*/
typedef enum uac_usrdata_type {
    UAC_USRDATA_TYPE_FILE = 0,
    UAC_USRDATA_TYPE_MEMORY = 1,
} uac_usrdata_type_t;

/* USB��Ƶ�û����ݽṹ��*/
struct uac_usrdata {
    uac_usrdata_type_t  type;
    uint8_t            *p_buf;
    int                 pos;
    int                 mem_len;
    int                 fd;
};

/* WAV��Ƶ�ļ�ͷ��Ϣ*/
struct wav_header {
    uint32_t riff_id;           /* "RIFF"��־*/
    uint32_t riff_sz;           /* �ļ�����*/
    uint32_t riff_fmt;          /* "WAVE"��־*/
    uint32_t fmt_id;            /* "fmt"��־*/
    uint32_t fmt_sz;            /* ��ʽ����*/
    uint16_t audio_format;      /* PCM��ʽ���*/
    uint16_t num_channels;      /* ͨ��������ͨ��Ϊ1��˫ͨ��Ϊ2*/
    uint32_t sample_rate;       /* ������(ÿ��������)����ʾÿ��ͨ���Ĳ����ٶ�*/
    uint32_t byte_rate;         /* ������Ƶ���ݴ������ʣ���ֵΪͨ����*ÿ������λ��*ÿ����������λ��/8
                                                                                                ��������������ֵ���ƻ������Ĵ�С*/
    uint16_t block_align;       /* ���ݿ������(���ֽ���)����ֵΪͨ����*ÿ������λ����8
                                                                                                ���������Ҫһ�δ�������ֵ��С���ֽ����ݣ��Ա㽫��ֵ���ڻ���������*/
    uint16_t bits_per_sample;   /* ÿ����������λ��*/
    uint32_t data_id;           /* "data"��־*/
    uint32_t data_sz;           /* �������ݵĳ���*/
};

#ifdef UAC_PLAY_FILE
/** ��ʼ���û������ļ�*/
static int uac_usrfile_init (struct uac_usrdata *p_stream,
                             const char         *p_file,
                             int                 oflag,
                             mode_t              mode){
    int             fd;
    memset(p_stream, 0, sizeof(*p_stream));
    p_stream->type = UAC_USRDATA_TYPE_FILE;
    p_stream->fd = -1;

    fd = aw_open(p_file, oflag, mode);
    if ( 0 > fd) {
        return fd;
    }

    p_stream->fd = fd;
    return 0;
}
#endif

/** ���ڴ��ж�ȡ����*/
static ssize_t uac_usrmem_read (struct uac_usrdata *p_stream,
                                void               *p_buf,
                                size_t              nbyte)
{
    int len;

    len = p_stream->mem_len - p_stream->pos;
    if (len > nbyte) {
        len = nbyte;
    }
    memcpy(p_buf, p_stream->p_buf + p_stream->pos, len);
    p_stream->pos += len;

    return len;
}

/** ���ļ��ж�ȡ����*/
static ssize_t uac_usrfile_read (struct uac_usrdata  *p_stream,
                                 void                *p_buf,
                                 size_t               nbyte)
{
    int len;
    len = aw_read(p_stream->fd, p_buf, nbyte);
    return len;
}

/** ��ȡ�û�����*/
static ssize_t uac_usrdata_read (struct uac_usrdata *p_stream,
                                 void               *p_buf,
                                 size_t              nbyte)
{
    if (UAC_USRDATA_TYPE_MEMORY == p_stream->type) {
        return uac_usrmem_read(p_stream, p_buf, nbyte);
    }

    return uac_usrfile_read(p_stream, p_buf, nbyte);
}

/** �ر�����*/
static void uac_usrdata_close (struct uac_usrdata *p_stream)
{
    if (UAC_USRDATA_TYPE_FILE == p_stream->type) {
        aw_close(p_stream->fd);
        p_stream->fd = -1;
    }
}

/** udisk�豸�Ȳ�μ��ص�����*/
static void udisk_monitor(void *arg, uint8_t type){
    aw_err_t ret;

    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find USB mass storage device\r\n");
        /* �ļ�ϵͳ���ص� "/u" ���  */
        ret = aw_mount("/u", __BLK_NAME, "vfat", 0);
        if (ret != AW_OK) {
            aw_kprintf("/u mount FATFS Fail: %d\r\n", ret);
            return;
        }
        is_mount = AW_TRUE;
        AW_SEMB_GIVE(ms_sync);
    }
    else if(type == USBDEV_EJECT){
        if(is_mount == AW_TRUE){
            aw_kprintf("USB mass storage device remove\r\n");
            aw_umount("/u", 0);
            is_mount = AW_FALSE;
        }
    }
}

/** ��Ƶ�豸�Ȳ�μ��ص�����*/
static void uac_monitor(void *arg, uint8_t type){
    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find USB Audio device\r\n");
        AW_SEMB_GIVE(audio_sync);
    }
}

/* USB��Ƶ�豸��������*/
void __test_uac_task (void) {
    aw_err_t                   ret;
    uac_handle                 handle;
    struct wav_header          header;
    struct uac_hw_usr_params   hw_params;
    struct uac_usrdata         stream;
    usbh_monitor_handle        audio_monitor_handle;
    usbh_monitor_handle        ms_monitor_handle;

#ifdef UAC_PLAY_FILE
    int         num_read;
    /* ��ʼ���ź���*/
    AW_SEMB_INIT(ms_sync, 0, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(audio_sync, 0, AW_SEM_Q_PRIORITY);
    /* ע��������洢�豸���*/
    ms_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                  USBH_DEV_UMS,
                                                  USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                  udisk_monitor, NULL);
    if(ms_monitor_handle == NULL){
        aw_kprintf("udisk monitor init failed\r\n");
        return;
    }
    /* ע����Ƶ�豸���*/
    audio_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                     USBH_DEV_UAC,
                                                     USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                     uac_monitor, NULL);
    if(audio_monitor_handle == NULL){
        aw_kprintf("audio monitor init failed\r\n");
        return;
    }

    aw_kprintf("wait USB mass storage device insert...\r\n");
    AW_SEMB_TAKE(ms_sync, AW_SEM_WAIT_FOREVER);

    /* ��ʼ���û��ļ�����*/
    ret = uac_usrfile_init(&stream,
                           UAC_TEST_FILE_PATH,
                           O_RDONLY,
                           0777);
    if (ret != AW_OK) {
        aw_kprintf("user data file init failed\r\n");
        return;
    }
    /* ��ȡWAV��Ƶ�ļ�ͷ��*/
    uac_usrdata_read(&stream, &header, sizeof(struct wav_header));
    if ((header.riff_id      != ID_RIFF)    ||
        (header.riff_fmt     != ID_WAVE)    ||
        (header.fmt_id       != ID_FMT)     ||
        (header.audio_format != FORMAT_PCM)) {

        aw_kprintf("Error: '%s' is not a riff/wave file\r\n", UAC_TEST_FILE_PATH);
        uac_usrdata_close(&stream);
        return;
    }
    /* ��ӡ��Ϣ*/
    aw_kprintf("audio file info:\r\n"
            "    audio file size:%d\r\n"
            "    PCM format     :%x\r\n"
            "    format size    :%d\r\n"
            "    channel num    :%d\r\n"
            "    sample rate    :%d\r\n"
            "    byte rate      :%d\r\n"
            "    per sample bits:%d\r\n",
            header.riff_sz, header.audio_format, header.fmt_sz,
            header.num_channels, header.sample_rate,
            header.byte_rate, header.bits_per_sample);

    buf = aw_mem_alloc(BUFF_SIZE);
    if(buf == NULL){
        return;
    }
    memset(buf, 0, BUFF_SIZE);

    aw_kprintf("wait USB Audio device insert...\r\n");
    AW_SEMB_TAKE(audio_sync, AW_SEM_WAIT_FOREVER);
    /* �Բ��ŵ�ģʽ��һ��Ĭ�ϵ�USB��Ƶ�豸*/
    handle = usbh_audio_dev_open(UAC_DEV_DEFAULT, UAC_PCM_DEFAULT, UAC_SND_PCM_STREAM_PLAYBACK);
    if(handle == NULL){
        aw_kprintf("UAC dev open failed\r\n");
        return;
    }
    aw_kprintf("UAC dev open success\r\n");

    memset(&hw_params ,0, sizeof(struct uac_hw_usr_params));
    hw_params.channels        = header.num_channels;
    hw_params.pcm_format_size = header.fmt_sz;
    hw_params.rate            = 48000;
    hw_params.bits_per_sample = header.bits_per_sample;

    /* ����Ӳ������*/
    ret = usbh_audio_hw_params_set(handle, &hw_params);
    if(ret != AW_OK){
        aw_kprintf("UAC dev set hw params failed\r\n");
        return;
    }

    /* USB��Ƶ�豸׼�����ݴ���*/
    ret = usbh_audio_dev_prepare(handle);
    if(ret != AW_OK){
        aw_kprintf("UAC dev prepare failed\r\n");
        return;
    }

    /* д��Ƶ����*/
    do {
        num_read = uac_usrdata_read(&stream, buf, BUFF_SIZE);
        if (num_read > 0) {
            /* USB��Ƶ�豸д����*/
            ret = usbh_audio_dev_write(handle, buf, num_read);
            if(ret <= 0){
                aw_kprintf("UAC dev write failed\r\n");
                usbh_audio_dev_close(handle);
                return;
            }
        }
    } while (num_read > 0);
    /* �ر���Ƶ�豸*/
    usbh_audio_dev_close(handle);
//    /* ֹͣ��Ƶ*/
//    usbh_uac_stop(handle, 0);
    /* �ͷ���Ƶ*/
//    usbh_uac_release(handle);

//    aw_mdelay(2000);
//
//    do {
//        num_read = uac_usrdata_read(&stream, buf, BUFF_SIZE);
//        if (num_read > 0) {
//            /* USB��Ƶ�豸д����*/
//            ret = usbh_uac_write(handle, buf, num_read);
//            if(ret <= 0){
//                aw_kprintf("UAC dev write failed\r\n");
//                return;
//            }
//            //memset(buf, 0, BUFF_SIZE);
//        }
//    } while (num_read > 0);

#else
    uint8_t *sound_buff = NULL;
    uint32_t sound_len = 0;

    /* ��ȡ��Ƶ����ͷ��Ϣ*/
    if(g_sound_wav_data_len <= sizeof(struct wav_header)){
        return;
    }
    memset(&header ,0, sizeof(struct wav_header));
    memcpy(&header, g_sound_wav_data, sizeof(struct wav_header));

    sound_buff = (uint8_t *)g_sound_wav_data + sizeof(struct wav_header);
    sound_len = g_sound_wav_data_len - sizeof(struct wav_header);
    /* ��ӡ��Ϣ*/
    aw_kprintf("audio file info:\r\n"
            "    audio file size:%d\r\n"
            "    PCM format     :%x\r\n"
            "    format size    :%d\r\n"
            "    channel num    :%d\r\n"
            "    sample rate    :%d\r\n"
            "    byte rate      :%d\r\n"
            "    per sample bits:%d\r\n",
            header.riff_sz, header.audio_format, header.fmt_sz,
            header.num_channels, header.sample_rate,
            header.byte_rate, header.bits_per_sample);

    /* �Բ��ŵ�ģʽ��һ��Ĭ�ϵ�USB��Ƶ�豸*/
    handle = usbh_uac_open(USB_AUDIO_DEFAULT, DEFAULT_PCM, USBH_SND_PCM_STREAM_PLAYBACK);
    if(handle == NULL){
        aw_kprintf("UAC dev open failed\r\n");
        return;
    }
    aw_kprintf("UAC dev open success\r\n");

    memset(&hw_params ,0, sizeof(struct usbh_uac_pcm_hw_params));
    hw_params.channels        = header.num_channels;
    hw_params.pcm_format_size = header.fmt_sz;
    hw_params.rate            = header.sample_rate;
    hw_params.period_frames   = 64;
    hw_params.buffer_periods  = 4;

    /* ����Ӳ������*/
    ret = usbh_uac_hw_params_set(handle, &hw_params);
    if(ret != AW_OK){
        aw_kprintf("UAC dev set hw params failed\r\n");
        return;
    }
    /* USB��Ƶ�豸׼�����ݴ���*/
    ret = usbh_uac_prepare(handle);
    if(ret != AW_OK){
        aw_kprintf("UAC dev prepare failed\r\n");
        return;
    }

    /* USB��Ƶ�豸д����*/
    ret = usbh_uac_write(handle, sound_buff, sound_len);
    if(ret <= 0){
        aw_kprintf("UAC dev write failed\r\n");
        return;
    }
#endif
    while(1){
        aw_mdelay(1000);
    }
}

/** UAC������ں���*/
void demo_uac_entry(void)
{
    aw_kprintf("UAC demo test...\n");

    AW_TASK_DECL_STATIC(uac_task, 1024 * 10 );

    /* ��ʼ������*/
    AW_TASK_INIT(uac_task,                /* ����ʵ�� */
               "uac_task",                /* �������� */
               12,                        /* �������ȼ� */
               1024 * 20 ,                /* �����ջ��С */
               __test_uac_task,           /* ������ں��� */
               (void *)NULL);             /* ������ڲ��� */

    /* ��������uac_task */
    AW_TASK_STARTUP(uac_task);
}
