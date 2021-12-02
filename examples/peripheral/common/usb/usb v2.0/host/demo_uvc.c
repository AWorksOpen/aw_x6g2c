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
 * \brief 通用USB摄像头演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBH宏和USBH协议栈
 *      - AW_DRV_USBH_UVC
 *      - AW_COM_CONSOLE
 *      - 显示屏宏
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在底板的USB端口中插入通用USB摄像头
 *   4. 接入显示屏显示图像
 *   5. 本例程是使用通用USB摄像头输出的YUYV422格式，请确保使用的摄像头支持该格式输出
 *
 * - 实验现象：
 *   1. 串口打印调试信息
 *   2. 显示屏显示摄像头拍摄的画面信息
 *
 *
 * \note 注意：
 * 需要接480*272的屏幕，当前demo同一时间只能打开一个视频流，
 * 关闭之后才可以打开别的视频流
 *
 * \brief 命令说明
 *
 * uvc_mem        检查当前USB视频类驱动的内存申请释放状态
 * uvc_detect     检测当前存在的USB视频类设备，会返回设备号，视频流号，视频流名字，
 *                支持的输出格式，分辨率和帧率
 * uvc_ctrl       获取USB视频类设备所支持的控件，需要参数设备号
 * uvc_open       打开一个视频流，需要参数视频流名字
 * uvc_close      关闭一个视频流
 * uvc_config_get 获取当前USB视频类设备的配置
 * uvc_config_set 设置USB视频类设备配置，需要两个参数，第一个是配置名，第二个是配置值
 *                配置名可以通过“uvc_config_set --help”获取
 * uvc_start      启动视频流，需要一个参数，提交的请求包数量(2~5)
 * uvc_stop       停止视频流
 * uvc_suspend    挂起视频流
 * uvc_resume     恢复视频流
 * uvc_timestamp  获取视频流时间戳，需要输入参数：1表示打开/0表示关闭
 * uvc_frame      获取视频流实际帧率
 */
#include "Aworks.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_fb.h"
#include "aw_mem.h"
#include "aw_fb.h"
#include "host/class/uvc/uvc_opt.h"
#include "host/core/usbh.h"
#include <string.h>

#define FB_NAME        "mxs_fb"
/* 视频分辨率*/
#define VIDEO_WIDTH     320
#define VIDEO_HEIGHT    240
/* 屏幕分辨率*/
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

#define RGB(a,b)       ((a << 8) | (b))
#define RANGE_LIMIT(x) (x > 255?255 : (x < 0?0 : x))

/* 分辨率设置 */
#define __VIDEO_RES(width, height)   (width | (height << 16u))

static uint32_t              __g_uvc_dev_num    = 0;
static struct uvc_streaming *__g_uvc_handle     = NULL;
static aw_bool_t             __g_uvc_start      = AW_FALSE;
static aw_bool_t             __g_uvc_time       = AW_FALSE;
static aw_bool_t             __g_uvc_frame      = AW_FALSE;
static int                   __g_uvc_frame_num  = 0;

struct video_rect {
    int x0,x1;
    int y0,y1;
};

struct video_info{
    int               xsize;
    int               ysize;
    uint8_t          *p_mem;
    struct video_rect rect;
};
static struct video_info __g_lcd_info;

typedef struct __yuyv{
    uint8_t  y1,u,y2,v;
}yuyv_t;

/* 数据流状态*/
struct stream_status{
    uint8_t status;
    char   *status_text;
};
struct stream_status uvc_stream_status[6] = {
        {UVC_DEV_STREAM_CLOSE,   "close"},
        {UVC_DEV_STREAM_OPEN,    "open"},
        {UVC_DEV_STREAM_CONFIG,  "config"},
        {UVC_DEV_STREAM_START,   "start"},
        {UVC_DEV_STREAM_STOP,    "stop"},
        {UVC_DEV_STREAM_SUSPEND, "suspend"}
};

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

/**
 * 填充帧缓存
 * handle：摄像头信息
 * p_rect：显示的坐标
 * p_data：图像数据
 * 返回：成功返回AW_OK*/
static aw_err_t app_fbuf_rect_fill (void              *handle,
                                    struct video_rect *p_rect,
                                    uint8_t           *p_data){
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
        if (idx >= ((SCREEN_WIDTH * SCREEN_HEIGHT - SCREEN_WIDTH) * 2)) {
            //aw_kprintf("out of range.\n\r");
            //while (1);
            continue;
        }
        memcpy(&p_app->p_mem[idx], p_data, nrow * 2);
        p_data += (nrow * 2);
    }

    return AW_OK;
}

/** 查询USB内存堆的使用情况shell命令*/
static int __uvc_mem_sta (int argc, char *argv[], struct aw_shell_user *p_user){
    usb_err_t              ret;
    struct usbh_mem_record mem_record;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_mem\r\n\r\n");
            aw_kprintf("Description:Check UVC mem status\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"uvc_mem --help\" for more information.\r\n");
        }
        return 0;
    }


    ret = uvc_lib_mem_record_get(&mem_record);
    if(ret == USB_OK){
        aw_kprintf("mem record:%s\r\n", mem_record.drv_name);
        aw_kprintf("  mem   alloc : %d\r\n", mem_record.mem_alloc_num);
        aw_kprintf("  mem   free  : %d\r\n", mem_record.mem_free_num);
        aw_kprintf("  mem   total : %d\r\n", mem_record.mem_total_num);
        aw_kprintf("  mutex alloc : %d\r\n", mem_record.mutex_alloc_num);
        aw_kprintf("  mutex free  : %d\r\n", mem_record.mutex_free_num);
        aw_kprintf("  sem   alloc : %d\r\n", mem_record.sem_alloc_num);
        aw_kprintf("  sem   free  : %d\r\n", mem_record.sem_free_num);
    }
    else{
        aw_kprintf("uvc mem record get failed\r\n");
    }
    return 0;
}

/** 遍历当前的USB视频设备 */
static int __uvc_dev_detect (int argc, char **argv, struct aw_shell_user *p_user){
    int                   i,j,k,t;
    int                   uvc_num         = 0;
    int                   uvc_stream_num  = 0;
    int                   uvc_format_num  = 0;
    int                   uvc_frame_num   = 0;
    char                 *p_name          = NULL;
    struct uvc_device    *uvc_dev         = NULL;
    struct uvc_streaming *uvc_stream      = NULL;
    struct uvc_format    *uvc_format      = NULL;
    struct uvc_frame     *uvc_frame       = NULL;

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_detect\r\n\r\n");
            aw_kprintf("Description:Detect all UVC devices in system\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        } else {
            aw_kprintf("Try \"uvc_detect --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }
    /* 获取UVC设备数量*/
    uvc_num = uvc_dev_num_get();
    if(uvc_num <= 0){
        return 0;
    }
    for(i = 0;i < uvc_num; i++){
        uvc_dev = uvc_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, i + 1);
        if(uvc_dev != NULL){
            aw_kprintf("UVC device %d(PID0x%x,VID0x%x):\r\n", i + 1,
                    USBH_DEV_PID_GET(uvc_dev->uvc_fun),
                    USBH_DEV_VID_GET(uvc_dev->uvc_fun));
            /* 获取UVC设备数据流数量*/
            uvc_stream_num = uvc_dev_nstream_get(uvc_dev);
            if(uvc_stream_num <= 0){
                continue;
            }
            for(j = 1;j <= uvc_stream_num;j++){
                p_name = uvc_dev_stream_name_get(uvc_dev, j);
                uvc_stream = uvc_dev_stream_open(p_name);
                if(uvc_stream != NULL){
                    aw_kprintf("  stream %d:%s <%s>\r\n", j, p_name,
                            uvc_stream_status[uvc_stream->opt_status].status_text);
                    /* 获取UVC设备数据流格式数量*/
                    uvc_format_num = uvc_stream_nformat_get(uvc_stream);
                    if(uvc_format_num <= 0){
                        continue;
                    }
                    for(k = 1;k <= uvc_format_num;k++){
                        uvc_format = uvc_stream_format_get(uvc_stream, k);
                        if(uvc_format != NULL){
                            aw_kprintf("    format %d:%s\r\n", k, uvc_format->name);
                            /* 获取UVC设备数据流格式帧数量*/
                            uvc_frame_num = uvc_format_nframe_get(uvc_format);
                            if(uvc_frame_num <= 0){
                                continue;
                            }
                            for(t = 1;t <= uvc_frame_num;t++){
                                uvc_frame = uvc_format_frame_get(uvc_format, t);
                                if(uvc_frame != NULL){
                                    aw_kprintf("      frame %d:%ux%u (%u.%u fps)\r\n",
                                            t, uvc_frame->wWidth, uvc_frame->wHeight,
                                            10000000/uvc_frame->dwDefaultFrameInterval,
                                            (100000000/uvc_frame->dwDefaultFrameInterval)%10);
                                }
                            }
                        }
                    }
                    uvc_dev_stream_close(uvc_stream);
                }
            }
            uvc_dev_close(uvc_dev);
        }
    }
    return 0;
}

/** 获取USB视频设备控件 */
static int __uvc_dev_ctrl (int argc, char **argv, struct aw_shell_user *p_user){
    int                   index,i,j,k;
    int                   entity_num;
    int                   control_num;
    int                   mapping_num;
    struct uvc_device    *uvc_dev         = NULL;
    struct uvc_entity    *uvc_entity      = NULL;
    struct uvc_control   *uvc_control     = NULL;
    char                 *mapping_name    = NULL;

    if(argc != 1){
        aw_kprintf("Try \"uvc_ctrl --help\" for more information.\r\n");
        return 0;
    }

    if(argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_ctrl\r\n\r\n");
            aw_kprintf("Description:Get a UVC device controls\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            aw_kprintf("    1.dev index :UVC device index\r\n");
            return 0;
        }
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    index = atoi(argv[0]);

    uvc_dev = uvc_dev_open(USBDEV_IGONRE_PID, USBDEV_IGONRE_VID, index);
    if(uvc_dev != NULL){
        aw_kprintf("UVC device %d(PID0x%x,VID0x%x):\r\n", index,
                USBH_DEV_PID_GET(uvc_dev->uvc_fun),
                USBH_DEV_VID_GET(uvc_dev->uvc_fun));
        entity_num = uvc_entity_num_get(uvc_dev);
        if(entity_num <= 0){
            return 0;
        }
        for(i = 0;i < entity_num;i++){
            uvc_entity = uvc_entity_get(uvc_dev, i);
            if(uvc_entity != NULL){
                aw_kprintf("  entity %d:\r\n", i);
                control_num = uvc_control_num_get(uvc_entity);
                if(control_num <= 0){
                    continue;
                }
                for(j = 0;j < control_num; j++){
                    uvc_control = uvc_control_get(uvc_entity, j);
                    if(uvc_control != NULL){
                        aw_kprintf("    control %d:", j);
                        mapping_num = uvc_control_mapping_num_get(uvc_control);
                        if(mapping_num <= 0){
                            aw_kprintf("\r\n");
                            continue;
                        }
                        for(k = 0;k < mapping_num;k++){
                            mapping_name = uvc_control_mapping_name_get(uvc_control, k);
                            if(mapping_name != NULL){
                                aw_kprintf("%s ", mapping_name);
                            }
                        }
                        aw_kprintf("\r\n");
                    }
                }
            }
        }
        uvc_dev_close(uvc_dev);
    }
    else{
        aw_kprintf("UVC device get controls failed\r\n");
    }
    return 0;
}

/** 打开UVC设备数据流*/
static int __uvc_dev_open (int argc, char **argv, struct aw_shell_user *p_user){
    if (argc != 1){
        aw_kprintf("Try \"uvc_open --help\" for more information.\r\n");
        return 0;
    }

    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:uvc_open\r\n\r\n");
        aw_kprintf("Description:Open a UVC device stream\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.stream name :UVC device stream name\r\n");
        return 0;
    }
    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle != NULL){
        aw_kprintf("A UVC device stream already opened\r\n");
        return 0;
    }

    __g_uvc_handle = uvc_dev_stream_open(argv[0]);
    if (__g_uvc_handle == NULL) {
        aw_kprintf("UVC device stream open failed\r\n");
        return 0;
    }
    aw_kprintf("UVC device stream open success\r\n");
    return 0;
}

/** 关闭UVC设备数据流*/
static int __uvc_dev_close (int argc, char **argv, struct aw_shell_user *p_user){
    aw_err_t       ret;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_close\r\n\r\n");
            aw_kprintf("Description:Close a UVC device stream\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        }else {
            aw_kprintf("Try \"uvc_close --help\" for more information.\r\n");
        }
        return 0;
    }
    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    if(__g_uvc_start == AW_TRUE){
        aw_kprintf("Do not stop UVC device stream\r\n");
        return 0;
    }

    ret = uvc_dev_stream_close(__g_uvc_handle);
    if(ret != USB_OK){
        aw_kprintf("Close UVC device stream failed %d\r\n", ret);
        return 0;
    }
    aw_kprintf("Close UVC device stream success\r\n");
    __g_uvc_handle = NULL;
    return 0;
}

/* 获取UVC设备配置  */
static int __uvc_dev_config_get (int argc, char **argv, struct aw_shell_user *p_user){
    struct uvc_cfg config;
    aw_err_t       ret;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_config_get\r\n\r\n");
            aw_kprintf("Description:Get a UVC device config\r\n\r\n");
            aw_kprintf("parameter:\r\n");
            return 0;
        }else {
            aw_kprintf("Try \"uvc_config_get --help\" for more information.\r\n");
            return 0;
        }
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    /* 获取摄像头格式*/
    memset(&config, 0, sizeof(struct uvc_cfg));
    ret = uvc_dev_stream_cfg_get(__g_uvc_handle, &config);
    if(ret != AW_OK){
        aw_kprintf("Get UVC device config failed:%d\r\n",ret);
        return 0;
    }

    aw_kprintf("UVC device current config:\r\n");
    aw_kprintf("  width               : %d\r\n"
               "  height              : %d\r\n"
               "  brightness          : %d\r\n"
               "  contrast            : %d\r\n"
               "  hue                 : %d\r\n"
               "  saturation          : %d\r\n"
               "  sharpness           : %d\r\n"
               "  gama                : %d\r\n"
               "  while balance       : %d\r\n"
               "  power line frequency: %d\r\n", (config.video_resolution & 0x0000FFFF),
               (config.video_resolution >> 16), config.brightness, config.contrast,
               config.hue, config.saturation, config.sharpness, config.gama,
               config.wb, config.plf);
    return 0;
}

/* 设置UVC设备配置  */
static int __uvc_dev_config_set (int argc, char **argv, struct aw_shell_user *p_user){
    aw_err_t       ret;
    struct uvc_cfg config;
    int            value = 0;
    int            type;

    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:uvc_config_set\r\n\r\n");
        aw_kprintf("Description:Set a UVC device config\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.config name  :UVC device config name, support config:\r\n");
        aw_kprintf("                   :brightness\r\n");
        aw_kprintf("                   :contrast\r\n");
        aw_kprintf("                   :hue\r\n");
        aw_kprintf("                   :saturation\r\n");
        aw_kprintf("                   :sharpness\r\n");
        aw_kprintf("                   :gama\r\n");
        aw_kprintf("                   :wb(while balance)\r\n");
        aw_kprintf("                   :plf(power line frequency)\r\n");
        aw_kprintf("    2.config value :UVC device config set value\r\n");
        return 0;
    }

    if(argc != 2) {
        aw_kprintf("Try \"uvc_config_set --help\" for more information.\r\n");
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }


    value = atoi(argv[1]);

    /* 设置UVC设备配置*/
    memset(&config, 0, sizeof(struct uvc_cfg ));
    config.usb_camera_type  = UVC_BUF_TYPE_VIDEO_CAPTURE;
    config.video_resolution = __VIDEO_RES(VIDEO_WIDTH, VIDEO_HEIGHT);
    config.format           = UVC_YUV422;

    if(strcmp(argv[0], "brightness") == 0){
        type = UVC_BRIGHTNESS_CFG;
        config.brightness = value;
    }
    else if(strcmp(argv[0], "contrast") == 0){
        type = UVC_CONTRAST_CFG;
        config.contrast = value;
    }
    else if(strcmp(argv[0], "hue") == 0){
        type = UVC_HUE_CFG;
        config.hue = value;
    }
    else if(strcmp(argv[0], "saturation") == 0){
        type = UVC_SATURARION_CFG;
        config.saturation = value;
    }
    else if(strcmp(argv[0], "sharpness") == 0){
        type = UVC_SHARPNESS_CFG;
        config.sharpness = value;
    }
    else if(strcmp(argv[0], "gama") == 0){
        type = UVC_GAMA_CFG;
        config.gama= value;
    }
    else if(strcmp(argv[0], "wb") == 0){
        type = UVC_WB_CFG;
        config.wb = value;
    }
    else if(strcmp(argv[0], "plf") == 0){
        type = UVC_PLF_CFG;
        config.plf = value;
    }
    else{
        aw_kprintf("Try \"uvc_config_set --help\" for more information.\r\n");
        return 0;
    }

    ret = uvc_dev_stream_cfg_set(__g_uvc_handle, &config, type);
    if(ret != AW_OK){
        aw_kprintf("Set UVC device config failed:%d\r\n",ret);
        return 0;
    }
    aw_kprintf("Set UVC device config success\r\n");
    return 0;
}

/** 启动UVC设备数据流*/
static int __uvc_dev_start (int argc, char **argv, struct aw_shell_user *p_user){
    struct uvc_cfg config;
    aw_err_t       ret;

    if (argc != 1){
        aw_kprintf("Try \"uvc_start --help\" for more information.\r\n");
        return 0;
    }

    if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
        aw_kprintf("Command:uvc_start\r\n\r\n");
        aw_kprintf("Description:Start a UVC device stream\r\n\r\n");
        aw_kprintf("parameter:\r\n");
        aw_kprintf("    1.trp num :UVC device trp num(1~5)\r\n");
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    /* 设置UVC设备配置*/
    memset(&config, 0, sizeof(struct uvc_cfg ));
    config.usb_camera_type  = UVC_BUF_TYPE_VIDEO_CAPTURE;
    config.video_resolution = __VIDEO_RES(VIDEO_WIDTH, VIDEO_HEIGHT);
    config.format           = UVC_YUV422;

    /* 配置摄像头格式*/
    ret = uvc_dev_stream_cfg_set(__g_uvc_handle, &config, 0);
    if(ret != AW_OK){
        aw_kprintf("UVC device stream config failed:%d\r\n",ret);
        return 0;
    }
    /* 启动摄像头*/
    ret = uvc_dev_stream_start(__g_uvc_handle, atoi(argv[0]));
    if(ret != AW_OK){
        aw_kprintf("UVC device stream start failed,code:%d\r\n",ret);
        return 0;
    }
    aw_kprintf("UVC device stream start success\r\n");
    __g_uvc_start = AW_TRUE;
    return 0;
}

/** 停止UVC设备数据流*/
static int __uvc_dev_stop (int argc, char **argv, struct aw_shell_user *p_user){
    aw_err_t       ret;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_stop\r\n\r\n");
            aw_kprintf("Description:Stop a UVC device stream\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        }else {
            aw_kprintf("Try \"uvc_stop --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    ret = uvc_dev_stream_stop(__g_uvc_handle);
    if(ret != AW_OK){
        aw_kprintf("UVC device stream stop failed,code:%d\r\n",ret);
        return 0;
    }
    aw_kprintf("UVC device stream stop success\r\n");
    __g_uvc_start = AW_FALSE;
    return 0;
}

/** 挂起一个UVC设备数据流*/
int __uvc_dev_suspend (int argc, char **argv, struct aw_shell_user *p_user){
    aw_err_t       ret;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_suspend\r\n\r\n");
            aw_kprintf("Description:Suspend a UVC device stream\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        }else {
            aw_kprintf("Try \"uvc_suspend --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    ret = uvc_dev_stream_suspend(__g_uvc_handle);
    if(ret != AW_OK){
        aw_kprintf("UVC device stream suspend failed,code:%d\r\n",ret);
        return 0;
    }
    aw_kprintf("UVC device stream suspend success\r\n");
    __g_uvc_start = AW_FALSE;
    return 0;
}

/** 恢复一个UVC设备数据流*/
int __uvc_dev_resume (int argc, char **argv, struct aw_shell_user *p_user){
    aw_err_t       ret;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_resume\r\n\r\n");
            aw_kprintf("Description:Resume a UVC device stream\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        }else {
            aw_kprintf("Try \"uvc_resume --help\" for more information.\r\n");
        }
        return 0;
    }

    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    ret = uvc_dev_stream_resume(__g_uvc_handle);
    if(ret != AW_OK){
        aw_kprintf("UVC device stream resume failed,code:%d\r\n",ret);
        return 0;
    }
    aw_kprintf("UVC device stream resume success\r\n");
    __g_uvc_start = AW_TRUE;
    return 0;
}

/** 获取视频时间戳*/
int __uvc_dev_timestamp (int argc, char **argv, struct aw_shell_user *p_user){
    int value;

    if (argc != 1){
          aw_kprintf("Try \"uvc_timestamp  --help\" for more information.\r\n");
          return 0;
      }

      if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
          aw_kprintf("Command:uvc_timestamp\r\n\r\n");
          aw_kprintf("Description:Get a started UVC device stream time value\r\n\r\n");
          aw_kprintf("parameter:\r\n");
          aw_kprintf("    1.on/off :1 is on,0 is off\r\n");
          return 0;
      }


    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }
    value = atoi(argv[0]);

    if(value == 1){
        __g_uvc_time = AW_TRUE;
        aw_kprintf("turn on time stamp\r\n");
    }
    else if(value == 0){
        __g_uvc_time = AW_FALSE;
        aw_kprintf("turn off time stamp\r\n");
    }
    else{
        aw_kprintf("value invalid\r\n");
    }
    return 0;
}

/** 获取视频帧黑素瘤*/
int __uvc_dev_frame (int argc, char **argv, struct aw_shell_user *p_user){
    int timeout = 3;

    if (argc > 0){
        if ((argc == 1) && (strcmp(argv[0], "--help") == 0)) {
            aw_kprintf("Command:uvc_frame\r\n\r\n");
            aw_kprintf("Description:Get a started UVC device stream frame number\r\n\r\n");
            aw_kprintf("parameter:\r\n");
        }else {
            aw_kprintf("Try \"uvc_frame --help\" for more information.\r\n");
        }
        return 0;
    }


    if(__g_uvc_dev_num == 0){
        aw_kprintf("Do not have UVC device\r\n");
        return 0;
    }

    if(__g_uvc_handle == NULL){
        aw_kprintf("Do not have UVC device stream opened\r\n");
        return 0;
    }

    if(__g_uvc_start == AW_FALSE){
        aw_kprintf("Do not have UVC device stream started\r\n");
        return 0;
    }
    __g_uvc_frame_num = 0;
    __g_uvc_frame = AW_TRUE;

    while((__g_uvc_frame == AW_TRUE) && (timeout--)){
        aw_mdelay(1000);
    }
    if(__g_uvc_frame == AW_TRUE){
        __g_uvc_frame = AW_FALSE;
        aw_kprintf("Get frame number failed\r\n");
        return 0;
    }
    aw_kprintf("%d f/s.\n", __g_uvc_frame_num);

    return 0;
}

/* 命令集 */
aw_local const struct aw_shell_cmd __g_uvc_cmdset_cmds[] = {
        /* 检查USB协议栈内存状态*/
        {__uvc_mem_sta,         "uvc_mem",        "check UVC mem status"},
        /* 遍历当前的UVC设备 */
        {__uvc_dev_detect,      "uvc_detect",     "Detect UVC device"},
        /* 获取UVC设备控件 */
        {__uvc_dev_ctrl,        "uvc_ctrl",       "Get a UVC device controls"},
        /* 打开UVC设备数据流 */
        {__uvc_dev_open,        "uvc_open",       "Open a UVC device stream"},
        /* 关闭UVC设备 数据流 */
        {__uvc_dev_close,       "uvc_close",      "Close a UVC device stream"},
        /* 获取UVC设备配置  */
        {__uvc_dev_config_get,  "uvc_config_get", "Get a UVC device config"},
        /* 设置UVC设备配置  */
        {__uvc_dev_config_set,  "uvc_config_set", "Set a UVC device config"},
        /* 启动UVC设备数据流  */
        {__uvc_dev_start,       "uvc_start",      "Start a UVC device stream"},
        /* 停止UVC设备 数据流 */
        {__uvc_dev_stop,        "uvc_stop",       "Stop a UVC device stream"},
        /* 挂起UVC设备数据流*/
        {__uvc_dev_suspend,     "uvc_suspend",    "Suspend a UVC device stream"},
        /* 恢复UVC设备数据流*/
        {__uvc_dev_resume,      "uvc_resume",     "Resume a UVC device stream"},
        /* 获取视频时间戳*/
        {__uvc_dev_timestamp ,  "uvc_timestamp",  "Get a started UVC device stream time stamp"},
        /* 获取视频帧数量*/
        {__uvc_dev_frame ,      "uvc_frame",      "Get a started UVC device stream frame number"},
};

/* 命令集描述 */
aw_local struct aw_shell_cmdset __g_uvc_cmdset = {
    .p_enter_name   = "uvc_test",                  /* 命令集进入名字*/
    .p_exit_name    = "uvc_exit",                  /* 命令集退出 */
    .p_man          = "This cmdset is use to test USB Video",
    .pfn_always_run = NULL,
    .pfn_entry_init = NULL,
};

/** UVC设备热插拔检测回调函数*/
static void uvc_monitor(void *arg, uint8_t type){
    if((type == USBDEV_INJECT) || (type == USBDEV_EXIST)){
        aw_kprintf("find a USB Video device\r\n");
        __g_uvc_dev_num++;
    }
    else if(type == USBDEV_EJECT){
        aw_kprintf("remove a USB Video device\r\n");
        __g_uvc_dev_num--;
    }
}

/* USB摄像头测试任务*/
static void __uvc_task (void) {
    aw_err_t              ret;
    void                 *vbuf   = NULL;
    uint8_t              *rgb    = NULL;
    struct uvc_timeval    timeval;
    aw_tick_t             t1,t2;

    rgb = aw_mem_alloc(VIDEO_WIDTH * VIDEO_HEIGHT * 2);
    if(rgb == NULL){
         return;
    }
    memset(&timeval, 0, sizeof(struct uvc_timeval));

    AW_FOREVER {
        if(__g_uvc_start){
            /* 获取一帧图像*/
            ret = uvc_dev_stream_video_get(__g_uvc_handle, &vbuf, &timeval);
            if(ret == AW_OK){
                /* YUYV422转RGB565*/
                YUYV422toRGB565(vbuf, rgb, VIDEO_WIDTH, VIDEO_HEIGHT);
                /* 填充帧缓冲*/
                app_fbuf_rect_fill(&__g_lcd_info, &__g_lcd_info.rect, (uint8_t*)rgb);
                if(__g_uvc_time == AW_TRUE){
                    aw_kprintf("time stamp :%2d(s) %3d(ms)\r\n", timeval.ts_sec, timeval.ts_usec / 1000);
                }
                if(__g_uvc_frame == AW_TRUE){
                    if(__g_uvc_frame_num == 0){
                        t1 = aw_sys_tick_get();
                    }
                    __g_uvc_frame_num++;
                }
                /* 释放一帧图像*/
                uvc_dev_stream_video_put(__g_uvc_handle, vbuf);
            }
            else if(ret == -AW_ENODEV){  /* USB摄像头被拔出*/
                uvc_dev_stream_stop(__g_uvc_handle);
                uvc_dev_stream_close(__g_uvc_handle);
                __g_uvc_handle = NULL;
                __g_uvc_start = AW_FALSE;
                continue;
            }
            if(__g_uvc_frame == AW_TRUE){
                /* 计算帧率*/
                if(__g_uvc_frame_num == 10){
                    t2 = aw_sys_tick_get();
                    if (aw_ticks_to_ms(t2 - t1) == 0) {
                        __g_uvc_frame_num = 0;
                    } else {
                        __g_uvc_frame_num = 1000 * 10 / aw_ticks_to_ms(t2 - t1);
                        if(__g_uvc_frame_num > 0){
                            __g_uvc_frame = AW_FALSE;
                        }
                    }
                }
            }
            aw_mdelay(1);
        }
        else{
            aw_mdelay(5);
        }
    }
    return ;
}

/** UVC例程入口函数*/
void demo_uvc_entry(void){
    aw_fb_fix_info_t      fix_info;
    aw_fb_var_info_t      fix_var_info;
    void                 *p_fb   = NULL;
    usbh_monitor_handle   uvc_monitor_handle;

    /* 打开帧缓存 */
    p_fb = aw_fb_open(FB_NAME, 0);
    if (p_fb == NULL) {
        aw_kprintf("Screen fb open failed!\r\n");
        return;
    }
    /* 初始化帧缓存*/
    if (aw_fb_init(p_fb) != AW_OK) {
        aw_kprintf("Screen fb Initialization failed!\r\n");
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
    __g_lcd_info.p_mem = (uint8_t *)fix_info.vram_addr;
    /* 获取帧缓存宽和高*/
    __g_lcd_info.xsize = fix_var_info.res.x_res;
    __g_lcd_info.ysize = fix_var_info.res.y_res;
    memset(__g_lcd_info.p_mem, 0x00, __g_lcd_info.xsize * __g_lcd_info.ysize * 2);
    /* 设置摄像头显示位置*/
    __g_lcd_info.rect.x0 = (fix_var_info.res.x_res - VIDEO_WIDTH) / 2;
    __g_lcd_info.rect.y0 = (fix_var_info.res.y_res - VIDEO_HEIGHT) / 2;
    __g_lcd_info.rect.x1 = __g_lcd_info.rect.x0 + VIDEO_WIDTH - 1;
    __g_lcd_info.rect.y1 = __g_lcd_info.rect.y0 + VIDEO_HEIGHT - 1;

    /* 注册UVC设备监控*/
    uvc_monitor_handle = usbh_dev_monitor_register(USBDEV_IGONRE_VID, USBDEV_IGONRE_PID,
                                                   USBH_DEV_UVC,
                                                   USBDEV_INJECT | USBDEV_EJECT | USBDEV_EXIST,
                                                   uvc_monitor, NULL);
    if(uvc_monitor_handle == NULL){
        aw_kprintf("uvc monitor register failed!\r\n");
        return;
    }
    /* 注册一个命令子集 */
    AW_SHELL_REGISTER_CMDSET(&__g_uvc_cmdset, __g_uvc_cmdset_cmds);

    AW_TASK_DECL_STATIC(uvc_task, 1024 * 10 );

    /* 初始化任务*/
    AW_TASK_INIT(uvc_task,                /* 任务实体 */
               "uvc_task",                /* 任务名字 */
               12,                        /* 任务优先级 */
               1024 * 10 ,                /* 任务堆栈大小 */
               __uvc_task,                /* 任务入口函数 */
               (void *)NULL);             /* 任务入口参数 */

    /* 启动任务uvc_task */
    AW_TASK_STARTUP(uvc_task);
}
