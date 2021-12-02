/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2021 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 通用USB摄像头演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USB宏
 *      - AW_DRV_USBH_UVC
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在底板的USB端口中插入通用USB摄像头
 *   4. 本例程是使用通用USB摄像头输出的YUYV422格式，请确保使用的摄像头支持该格式输出
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 */
#include "Aworks.h"
#include "aw_vdebug.h"
#include "aw_camera.h"
#include "aw_system.h"
#include "aw_types.h"
#include <stdio.h>
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_fb.h"
#include <string.h>
#include "aw_mem.h"
#include "aw_fb.h"
#include "host/class/uvc/uvc_driver.h"
#include "aw_demo_config.h"


/* 摄像头分辨率*/
#define CAMERA_WIDTH  320
#define CAMERA_HEIGHT 240
//#define CAMERA_WIDTH  640
//#define CAMERA_HEIGHT 480

/* 屏幕分辨率*/
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272
//#define SCREEN_WIDTH  800
//#define SCREEN_HEIGHT 480

//#define FRAME_COUNT

#define RGB(a,b) ((a << 8) | (b))
#define RANGE_LIMIT(x) (x > 255?255 : (x < 0?0 : x))
/* 摄像头分辨率*/
#define __VIDEO_RES AW_RESOLUTION_SET(CAMERA_WIDTH, CAMERA_HEIGHT)

extern void *aw_camera_wait_open(char *p_name, int time_out);

struct video_rect {
    int x0,x1;
    int y0,y1;
};

struct video_info{
    int xsize;
    int ysize;
    uint8_t *p_mem;
    struct video_rect rect;
};


typedef struct __yuyv{
    uint8_t  y1,u,y2,v;
}yuyv_t;

/**
 * YUYUV422转换RGB565
 * @param inbuf  输入的YUYV422格式图片
 * @param outbuf 输出的RGB565格式图片
 * @param width  图片宽
 * @param height 图片高
 */
static void YUYV422toRGB565(const void* inbuf, void* outbuf, int width, int height){
    int rows, cols;
    int y, y2, u, v, r, g, b, rgb1, rgb2, r1, g1, b1;
    uint32_t *yuv_buf;
    uint32_t *rgb_buf;
    yuyv_t *p = NULL;

    if((inbuf == NULL) || (outbuf == NULL)){
        return;
    }

    yuv_buf = (uint32_t *)inbuf;
    rgb_buf = (uint32_t *)outbuf;

    for(rows = 0;rows < height;rows++){
        for(cols = 0;cols < width;cols+=2){
            p = (yuyv_t*)yuv_buf++;

            v  = p->v - 128;
            y  = p->y1;
            u  = p->u - 128;
            y2 = p->y2;

            r = RANGE_LIMIT(y + v + ((v * 103) >> 8));
            g = RANGE_LIMIT(y - ((u * 88) >> 8) - ((v * 183) >> 8));
            b = RANGE_LIMIT(y + u + ((u * 198) >> 8));

            r1 = RANGE_LIMIT(y2 + v + ((v * 103) >> 8));
            g1 = RANGE_LIMIT(y2 - ((u * 88) >> 8) - ((v * 183) >> 8));
            b1 = RANGE_LIMIT(y2 + u + ((u * 198) >> 8));

            rgb1= (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));
            rgb2 =(((r1 & 0xf8) << 8) | ((g1 & 0xfc) << 3) | ((b1 & 0xf8) >> 3));

            *rgb_buf++ = (rgb2 << 16) | rgb1;
        }
    }
}


/* 填充帧缓存
 * handle：摄像头信息
 * p_rect：显示的坐标
 * p_data：图像数据
 * 返回：成功返回AW_OK*/
static aw_err_t app_fbuf_rect_fill (void              *handle,
                                    struct video_rect *p_rect,
                                    uint8_t           *p_data)
{
    struct video_info *p_app = (struct video_info *)handle;
    int                 idx, y;
    int                 nline, nrow;


    if ((p_rect->x1 < p_rect->x0) || (p_rect->y1 < p_rect->y0)) {
        return -AW_EINVAL;
    }

    nrow  = p_rect->x1 - p_rect->x0 + 1;
    nline = p_rect->y1 - p_rect->y0 + 1;

    if ((nrow > p_app->xsize) || (nline >  p_app->ysize)) {
        return -AW_EDOM;
    }

    for (y = 0; y < nline; y++) {
        idx = ((p_rect->y0 + y) * p_app->xsize + p_rect->x0) * 2;
        if (idx >= ((SCREEN_WIDTH*SCREEN_HEIGHT-SCREEN_WIDTH) * 2)) {
            //aw_kprintf("out of range.\n\r");
            //while (1);
            continue;
        }
        memcpy(&p_app->p_mem[idx], p_data, nrow * 2);
        p_data += (nrow * 2);
    }

    return AW_OK;
}

/* USB摄像头测试任务*/
void __test_camera_uvc_task (void) {
    aw_err_t ret = 0;
    void *fd = NULL;
    void *vbuf = NULL;
    uint8_t *rgb = NULL;
    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t fix_var_info;
    void  *p_fb  = NULL;
    struct aw_camera_cfg format;
    struct video_info video_info;
    uint32_t opt;
#ifdef FRAME_COUNT
    aw_tick_t t1,t2;
    int frame_ms;
#endif
    int get_p;

    rgb = aw_mem_alloc(CAMERA_WIDTH*CAMERA_HEIGHT*2);
    if(rgb == NULL)
        return;

    /* 打开帧缓存 */
    p_fb = aw_fb_open(DE_FB, 0);
    if (p_fb == NULL) {
        aw_kprintf("* Screen open failed!\r\n");
        return;
    }
    /* 初始化帧缓存*/
    if (aw_fb_init(p_fb) != AW_OK) {
        aw_kprintf("* Screen Initialization failed!\r\n");
        return;
    }
    /* 获取帧缓存信息*/
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &fix_var_info);
    /* 启动帧缓存*/
    aw_fb_start(p_fb);
    /* 打开背光*/
    aw_fb_backlight(p_fb, 100);
    /* 获取帧缓存地址*/
    video_info.p_mem = (uint8_t *)fix_info.vram_addr;
    /* 获取帧缓存宽和高*/
    video_info.xsize = fix_var_info.res.x_res;
    video_info.ysize = fix_var_info.res.y_res;
    memset(video_info.p_mem, 0x00, video_info.xsize * video_info.ysize * 2);
    /* 设置摄像头显示位置*/
    video_info.rect.x0 = (fix_var_info.res.x_res - CAMERA_WIDTH) / 2;
    video_info.rect.y0 = (fix_var_info.res.y_res - CAMERA_HEIGHT) / 2;
    video_info.rect.x1 = video_info.rect.x0 + CAMERA_WIDTH - 1;
    video_info.rect.y1 = video_info.rect.y0 + CAMERA_HEIGHT - 1;

    opt = AW_CAMERA_RESOLUTION | AW_CAMERA_FORMAT | AW_CAMERA_USB_TYPE;

    /* 设置USB摄像头配置*/
    format.usb_camera_type = UVC_BUF_TYPE_VIDEO_CAPTURE;
    format.video_resolution = __VIDEO_RES;
    format.format = YUV422;

restart:
    aw_kprintf("* wait for USB camera 0 insert..... \n\r");
    aw_mdelay(2000);
    /* 等待打开摄像头 */
    fd = aw_camera_wait_open("usb_video0", AW_SEM_WAIT_FOREVER);
    if (fd == NULL) {
        aw_kprintf("* USB camera 0 open error! \n\r");
        return;
    }
    aw_kprintf("* USB camera 0 open successful\n\r");

    /* 配置摄像头格式*/
    ret = aw_camera_config_set(fd, &format, &opt);
    if(ret != AW_OK){
        aw_kprintf("* USB camera cfg error,code:%d\r\n",ret);
        return ;
    }
    AW_FOREVER {
#ifdef FRAME_COUNT
        t1 = aw_sys_tick_get();
#endif
        /* 获取一帧图像*/
        ret = aw_camera_get_video_buf(fd, &vbuf);
        if(ret == AW_OK){
            /* YUYV422转RGB565*/
            YUYV422toRGB565(vbuf, rgb, CAMERA_WIDTH, CAMERA_HEIGHT);
            /* 填充帧缓冲*/
            app_fbuf_rect_fill(&video_info, &video_info.rect, (uint8_t*)rgb);
            get_p = 1;
        }
        else if(ret == -AW_ENODEV){  /* USB摄像头被拔出*/
            break;
        }
        else{
            get_p = 0;
        }
        /* 释放一帧图像*/
        ret = aw_camera_release_video_buf(fd, vbuf);
        aw_mdelay(5);
        /* 计算帧率*/
        if(get_p){
#ifdef FRAME_COUNT
            t2 = aw_sys_tick_get();
            if (aw_ticks_to_ms(t2 - t1) == 0) {
                frame_ms = 0;
            } else {
                frame_ms = 1000 / aw_ticks_to_ms(t2 - t1);
            }
            aw_kprintf("%d f/s.\n", frame_ms);
#endif
        }
    }
    if(ret == -AW_ENODEV){
        aw_camera_close(fd);
        goto restart;
    }
    return ;
}

/**
 * /brief UVC例程入口函数
 */
void demo_camera_uvc_entry(void)
{
    aw_kprintf("UVC demo test...\n");

    AW_TASK_DECL_STATIC(uvc_camera, 1024 * 10 );

    /* 初始化任务*/
    AW_TASK_INIT(uvc_camera,                /* 任务实体 */
               "uvc_camera",                /* 任务名字 */
               12,                          /* 任务优先级 */
               1024 * 10 ,                  /* 任务堆栈大小 */
               __test_camera_uvc_task,      /* 任务入口函数 */
               (void *)NULL);               /* 任务入口参数 */

    /* 启动任务uvc_camera */
    AW_TASK_STARTUP(uvc_camera);
}


