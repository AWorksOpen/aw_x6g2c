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
 * \brief 通用USB音频设备演示例程
 *
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBH宏和USBH协议栈
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. U盘里存放一个test.wav名字的音频文件，USB-HOST2端口插入U盘，
 *      USB-HOST1插入通用USB音频设备，
 *
 * - 实验现象：
 *   1. USB音频设备播放音频文件
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


/* 播放音频文件*/
#define UAC_PLAY_FILE

#ifndef UAC_PLAY_FILE
/* 音频测试数据 */
extern const unsigned char g_sound_wav_data[];
extern const unsigned int  g_sound_wav_data_len;
//extern const unsigned char g_sound_wav_data1[];
//extern const unsigned int  g_sound_wav_data_len1;
#else
#define UAC_TEST_FILE_PATH  "/u/test.wav"
#define BUFF_SIZE           20*1024
#endif
/* 数据缓存*/
char *buf = NULL;
/* 信号量*/
AW_SEMB_DECL (ms_sync);
AW_SEMB_DECL (audio_sync);
/* 是否挂载U盘*/
static aw_bool_t is_mount = AW_FALSE;
/* U盘块设备名字*/
#define __BLK_NAME    DE_USB_BLK_NAME

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

/* USB音频用户数据类型*/
typedef enum uac_usrdata_type {
    UAC_USRDATA_TYPE_FILE = 0,
    UAC_USRDATA_TYPE_MEMORY = 1,
} uac_usrdata_type_t;

/* USB音频用户数据结构体*/
struct uac_usrdata {
    uac_usrdata_type_t  type;
    uint8_t            *p_buf;
    int                 pos;
    int                 mem_len;
    int                 fd;
};

/* WAV音频文件头信息*/
struct wav_header {
    uint32_t riff_id;           /* "RIFF"标志*/
    uint32_t riff_sz;           /* 文件长度*/
    uint32_t riff_fmt;          /* "WAVE"标志*/
    uint32_t fmt_id;            /* "fmt"标志*/
    uint32_t fmt_sz;            /* 格式长度*/
    uint16_t audio_format;      /* PCM格式类别*/
    uint16_t num_channels;      /* 通道数，单通道为1，双通道为2*/
    uint32_t sample_rate;       /* 采样率(每秒样本数)，表示每个通道的播放速度*/
    uint32_t byte_rate;         /* 波形音频数据传送速率，其值为通道数*每秒数据位数*每样本的数据位数/8
                                                                                                播放软件利用这个值估计缓冲区的大小*/
    uint16_t block_align;       /* 数据块调整数(按字节算)，其值为通道数*每秒数据位数、8
                                                                                                播发软件需要一次处理多个该值大小的字节数据，以便将其值用于缓冲区调整*/
    uint16_t bits_per_sample;   /* 每样本的数据位数*/
    uint32_t data_id;           /* "data"标志*/
    uint32_t data_sz;           /* 语音数据的长度*/
};

#ifdef UAC_PLAY_FILE
/** 初始化用户数据文件*/
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

/** 从内存中读取数据*/
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

/** 从文件中读取数据*/
static ssize_t uac_usrfile_read (struct uac_usrdata  *p_stream,
                                 void                *p_buf,
                                 size_t               nbyte)
{
    int len;
    len = aw_read(p_stream->fd, p_buf, nbyte);
    return len;
}

/** 读取用户数据*/
static ssize_t uac_usrdata_read (struct uac_usrdata *p_stream,
                                 void               *p_buf,
                                 size_t              nbyte)
{
    if (UAC_USRDATA_TYPE_MEMORY == p_stream->type) {
        return uac_usrmem_read(p_stream, p_buf, nbyte);
    }

    return uac_usrfile_read(p_stream, p_buf, nbyte);
}

/** 关闭数据*/
static void uac_usrdata_close (struct uac_usrdata *p_stream)
{
    if (UAC_USRDATA_TYPE_FILE == p_stream->type) {
        aw_close(p_stream->fd);
        p_stream->fd = -1;
    }
}

/** udisk设备热插拔检测回调函数*/
static void udisk_monitor(void *arg, uint8_t type){
    aw_err_t ret;

    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find USB mass storage device\r\n");
        /* 文件系统挂载到 "/u" 结点  */
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

/** 音频设备热插拔检测回调函数*/
static void uac_monitor(void *arg, uint8_t type){
    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find USB Audio device\r\n");
        AW_SEMB_GIVE(audio_sync);
    }
}

/* USB音频设备测试任务*/
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
    /* 初始化信号量*/
    AW_SEMB_INIT(ms_sync, 0, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(audio_sync, 0, AW_SEM_Q_PRIORITY);
    /* 注册大容量存储设备监控*/
    ms_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                  USBH_DEV_UMS,
                                                  USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                  udisk_monitor, NULL);
    if(ms_monitor_handle == NULL){
        aw_kprintf("udisk monitor init failed\r\n");
        return;
    }
    /* 注册音频设备监控*/
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

    /* 初始化用户文件数据*/
    ret = uac_usrfile_init(&stream,
                           UAC_TEST_FILE_PATH,
                           O_RDONLY,
                           0777);
    if (ret != AW_OK) {
        aw_kprintf("user data file init failed\r\n");
        return;
    }
    /* 读取WAV音频文件头部*/
    uac_usrdata_read(&stream, &header, sizeof(struct wav_header));
    if ((header.riff_id      != ID_RIFF)    ||
        (header.riff_fmt     != ID_WAVE)    ||
        (header.fmt_id       != ID_FMT)     ||
        (header.audio_format != FORMAT_PCM)) {

        aw_kprintf("Error: '%s' is not a riff/wave file\r\n", UAC_TEST_FILE_PATH);
        uac_usrdata_close(&stream);
        return;
    }
    /* 打印信息*/
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
    /* 以播放的模式打开一个默认的USB音频设备*/
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

    /* 设置硬件参数*/
    ret = usbh_audio_hw_params_set(handle, &hw_params);
    if(ret != AW_OK){
        aw_kprintf("UAC dev set hw params failed\r\n");
        return;
    }

    /* USB音频设备准备数据传输*/
    ret = usbh_audio_dev_prepare(handle);
    if(ret != AW_OK){
        aw_kprintf("UAC dev prepare failed\r\n");
        return;
    }

    /* 写音频数据*/
    do {
        num_read = uac_usrdata_read(&stream, buf, BUFF_SIZE);
        if (num_read > 0) {
            /* USB音频设备写数据*/
            ret = usbh_audio_dev_write(handle, buf, num_read);
            if(ret <= 0){
                aw_kprintf("UAC dev write failed\r\n");
                usbh_audio_dev_close(handle);
                return;
            }
        }
    } while (num_read > 0);
    /* 关闭音频设备*/
    usbh_audio_dev_close(handle);
//    /* 停止音频*/
//    usbh_uac_stop(handle, 0);
    /* 释放音频*/
//    usbh_uac_release(handle);

//    aw_mdelay(2000);
//
//    do {
//        num_read = uac_usrdata_read(&stream, buf, BUFF_SIZE);
//        if (num_read > 0) {
//            /* USB音频设备写数据*/
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

    /* 获取音频数据头信息*/
    if(g_sound_wav_data_len <= sizeof(struct wav_header)){
        return;
    }
    memset(&header ,0, sizeof(struct wav_header));
    memcpy(&header, g_sound_wav_data, sizeof(struct wav_header));

    sound_buff = (uint8_t *)g_sound_wav_data + sizeof(struct wav_header);
    sound_len = g_sound_wav_data_len - sizeof(struct wav_header);
    /* 打印信息*/
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

    /* 以播放的模式打开一个默认的USB音频设备*/
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

    /* 设置硬件参数*/
    ret = usbh_uac_hw_params_set(handle, &hw_params);
    if(ret != AW_OK){
        aw_kprintf("UAC dev set hw params failed\r\n");
        return;
    }
    /* USB音频设备准备数据传输*/
    ret = usbh_uac_prepare(handle);
    if(ret != AW_OK){
        aw_kprintf("UAC dev prepare failed\r\n");
        return;
    }

    /* USB音频设备写数据*/
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

/** UAC例程入口函数*/
void demo_uac_entry(void)
{
    aw_kprintf("UAC demo test...\n");

    AW_TASK_DECL_STATIC(uac_task, 1024 * 10 );

    /* 初始化任务*/
    AW_TASK_INIT(uac_task,                /* 任务实体 */
               "uac_task",                /* 任务名字 */
               12,                        /* 任务优先级 */
               1024 * 20 ,                /* 任务堆栈大小 */
               __test_uac_task,           /* 任务入口函数 */
               (void *)NULL);             /* 任务入口参数 */

    /* 启动任务uac_task */
    AW_TASK_STARTUP(uac_task);
}
