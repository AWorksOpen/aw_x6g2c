/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief frame buffer例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能：
 *      - AW_COM_CONSOLE；
 *      - 对应平台的串口宏；
 *      - AW_COM_SHELL_SERIAL
 *      - 使用的显示屏的宏、触摸外设的宏；
 *
 *   2. 打开shell调试终端，设置波特率为115200-8-N-1.
 *   3. 在LCD配置文件中配置相关默认参数，如__HW800480_BUS_BPP 16，__HW800480_WORD_LBPP 16，
 *      则表示16位像素数据在16线数据总线的宽度的LCD显示。__HW800480_BUS_BPP 24，
 *      __HW800480_WORD_LBPP 24，则表示24位像素数据在24线数据总线的宽度的LCD显示
 *   4. 如果LCD缓冲区是静态区段,则需要注意LCD该段的大小，不能低于一帧数据大小，
 *      如像素数据RGB565(16bpp),800*480分辨率，一帧数据大小为800*480*2=768000
 *      如像素数据RGB888(24bpp),800*480分辨率，一帧数据大小为800*480*3=1152000
 *
 * - 实验现象：
 *   1. 先进行屏幕校准；
 *   2. 屏幕中间从上至下显示红绿蓝黄紫青白7条条纹；
 *   3. 左边屏幕中间显示一条绿色的横线，右边显示一条红色的竖线；
 *   4. 屏幕左上角显示“AWorks”;
 *   5. 触摸屏幕默认将在屏幕上画线,通过“MULTI_TS”宏选择是单点划线还是测试多点触摸。
 *   6. 屏幕左下角出现(x,y)触摸坐标值
 *   7. 用户通过shell终端,键入命令:
 *      键入"LCD_DCLK 48000000"命令，会配置LCD时钟频率
 *      键入"LCD_BPP 16 16"或"LCD_BPP 16 24"命令或"LCD_BPP 24 16"命令或"LCD_BPP 24 24"命令，
 *          会配置LCD位像素深度（一个像素的大小）和 数据总线的宽度（支持4种模式）
 *      键入"LCD_HSYNC 1 800 1000 46"命令，会配置LCD水平同步信号信息
 *      键入"LCD_VSYNC 1 480 22 23"命令，会配置LCD垂直同步信号信息
 *      键入"LCD_RES   800 480"命令，会配置屏幕分辨率，使用该命令修改分辨率，如果LCD缓冲区是静态区段，可以会
 *          导致内存益处，请谨慎使用，确保分给LCD静态区段的大小足够。
 *          如果分辨率改变，则，HSYNC与VSYNC参数也要做相应改变，如键入"LCD_RES 480 272"，则需键入键入
 *          "LCD_HSYNC 1 480 4 15"，"LCD_VSYNC 1 272 114 13"，即第二个参数相对应。
 *      键入"LCD_RESET"命令，会复位LCD
 *   8. 用户配置好参数之后需键入"LCD_RESET"命令复位LCD，观看调参后的显示效果
 *
 * \par 源代码
 * \snippet demo_lcd.c src_lcd
 *
 * \internal
 * \par Modification history
 * - 1.00 18-07-16  sdq, first implementation.
 * - 2.00 21-04-13  swh, second implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lcd 显示屏例程
 * \copydoc demo_lcd.c
 */

/** [src_lcd] */
#include "aworks.h"
#include "aw_ts.h"
#include "aw_fb.h"
#include <string.h>
#include <stdio.h>
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

#include "lcd_draw.h"

/* 是否测试多点触摸， 否：注释掉 ，是：打开定义*/
//#define MULTI_TS

/* 各触摸设备最大可以支持的点数 */
#define __GT911_MULTI_TS_CNT      5
#define __FT5X06_MULTI_TS_CNT     5
#define __BU21029MUV_MULTI_TS_CNT 2

#define __GET_MULTI_TS_CNT(name, num) \
do{ \
    if (strcmp(name, "ft5x06") == 0) { \
        *num = __FT5X06_MULTI_TS_CNT; \
    } else if (strcmp(name, "gt911") == 0) { \
        *num = __GT911_MULTI_TS_CNT; \
    } else if (strcmp(name, "bu21029muv") == 0) { \
        *num = __BU21029MUV_MULTI_TS_CNT; \
    } else { \
        *num = 1; \
    }\
} while(0)

/******************************************************************************/
#define __TS_NAME    DE_TS_NAME           /*触摸屏名字*/

#define __FB_NAME    DE_FB                /*显示屏名称*/

/* 连续 */
#define __CONTINUOUS      1
/* 不连续 */
#define __DISCONTINUOUS   0

aw_local void     * __g_p_fb;
aw_fb_fix_info_t  g_fix_screen_info;
aw_fb_var_info_t  g_var_screen_info;

aw_local uint8_t __g_color[][3] = {{0xff, 0x00, 0x00},   //红色
                                   {0x00, 0xff, 0x00},   //绿色
                                   {0x00, 0x00, 0xff},   //蓝色
                                   {0xff, 0xff, 0x00},   //黄色
                                   {0xff, 0xff, 0xff}};  //白色

/*****************************时序与位深动态配置***********************************/
/**
 * \brief shell配置LCD时钟频率命令入口
 */
aw_local int __shell_lcd_dclk (int                   argc,
                               char                 *argv[],
                               struct aw_shell_user *p_shell_user)
{
    if (!(argc == 1) || atoi(argv[0]) == 0) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                       "argv[0]:lcd clock frequency\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_DCLK 48000000\r\n");
        return -AW_E2BIG;
    }

    /* 配置时钟频率 */
    g_var_screen_info.dclk_f.typ_dclk_f = atoi(argv[0]); /** \brief 典型时钟Hz */
    aw_fb_config(__g_p_fb, AW_FB_CONF_DCLK_VINFO, &g_var_screen_info.dclk_f);

    /*frame buffer 设备信息货物*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD位像素深度命令入口
 */
aw_local int __shell_lcd_bpp (int                   argc,
                              char                 *argv[],
                              struct aw_shell_user *p_shell_user)
{
    if (!(argc == 2)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd word length bpp \r\n"
                                      "argv[1]:lcd bus bpp\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_BPP 16 16\r\n");
        aw_shell_printf(p_shell_user, "            LCD_BPP 16 24\r\n");
        aw_shell_printf(p_shell_user, "            LCD_BPP 24 16\r\n");
        aw_shell_printf(p_shell_user, "            LCD_BPP 24 24\r\n");
        return -AW_E2BIG;
    }

    /* 配置bpp */
    g_var_screen_info.bpp.word_lbpp     = atoi(argv[0]);
    g_var_screen_info.bpp.bus_bpp       = atoi(argv[1]);
    aw_fb_config(__g_p_fb, AW_FB_CONF_BPP_VINFO, &g_var_screen_info.bpp);

    /*frame buffer 设备信息货物*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD水平同步信息命令入口
 */
aw_local int __shell_lcd_hsync (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (!(argc == 4)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd horizontal sync pulse width \r\n"
                                      "argv[1]:lcd horizontal active\r\n"
                                      "argv[2]:lcd horizontal front porch \r\n"
                                      "argv[3]:lcd horizontal back porch\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_HSYNC 1 800 1000 46\r\n");
        return -AW_E2BIG;
    }

    /* 配置行同步信息 */
    g_var_screen_info.hsync.h_pulse_width = atoi(argv[0]); /** \brief 行脉冲宽度 */
    g_var_screen_info.hsync.h_active      = atoi(argv[1]); /** \brief 一行的像素个数 */
    g_var_screen_info.hsync.h_fporch      = atoi(argv[2]); /** \brief 水平前沿 */
    g_var_screen_info.hsync.h_bporch      = atoi(argv[3]); /** \brief 水平后沿 */
    aw_fb_config(__g_p_fb, AW_FB_CONF_HSYNC_VINFO, &g_var_screen_info.hsync);

    /*frame buffer 设备信息货物*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD垂直同步信息命令入口
 */
aw_local int __shell_lcd_vsync (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (!(argc == 4)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd vertical sync pulse width \r\n"
                                      "argv[1]:lcd vertical active\r\n"
                                      "argv[2]:lcd vertical front porch \r\n"
                                      "argv[3]:lcd vertical back porch\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_VSYNC 1 480 22 23\r\n");
        return -AW_E2BIG;
    }

    /* 配置场同步信息 */
    g_var_screen_info.vsync.v_pulse_width = atoi(argv[0]); /** \brief 垂直脉冲宽度 */
    g_var_screen_info.vsync.v_active      = atoi(argv[1]); /** \brief 一帧画面的行数 */
    g_var_screen_info.vsync.v_fporch      = atoi(argv[2]); /** \brief 垂直前沿 */
    g_var_screen_info.vsync.v_bporch      = atoi(argv[3]); /** \brief 垂直后沿 */
    aw_fb_config(__g_p_fb, AW_FB_CONF_VSYNC_VINFO, &g_var_screen_info.vsync);

    /*frame buffer 设备信息货物*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell配置LCD分辨率命令入口
 */
aw_local int __shell_lcd_res    (int                   argc,
                                 char                 *argv[],
                                 struct aw_shell_user *p_shell_user)
{
    if (!(argc == 2)) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "param:\r\n"
                                      "argv[0]:lcd resolution x\r\n"
                                      "argv[1]:lcd resolution y\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_RES 800 480\r\n");
        return -AW_E2BIG;
    }

        /* 配置屏幕分辨率 */
    g_var_screen_info.res.x_res         = atoi(argv[0]);
    g_var_screen_info.res.y_res         = atoi(argv[1]);
    aw_fb_config(__g_p_fb, AW_FB_CONF_RES_VINFO, &g_var_screen_info.res);

    /*frame buffer 设备信息货物*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell重启LCD命令入口
 */
aw_local int __shell_lcd_reset (int                   argc,
                                char                 *argv[],
                                struct aw_shell_user *p_shell_user)
{
    if (argc > 0) {
        aw_shell_printf(p_shell_user, "Unknow Arg\r\n");
        aw_shell_printf(p_shell_user, "For Example:LCD_RESET\r\n");
        return -AW_E2BIG;
    }

    /* 重启帧缓冲设备 */
    aw_fb_reset(__g_p_fb);

    draw_lcd(__g_p_fb);

    return AW_OK;
}

/**
 * \brief shell lcd控制命令表
 */
aw_local const aw_shell_cmd_item_t __g_my_lcd_cmds[] = {
    /* 时钟频率 */
    {__shell_lcd_dclk,   "LCD_DCLK",    "<lcd clock frequency>"},
    /* 位像素深度  */
    {__shell_lcd_bpp,    "LCD_BPP",     "<lcd bit pixel depth>"},
    /* 水平同步 */
    {__shell_lcd_hsync,  "LCD_HSYNC",   "<lcd horizontal synchronization>"},
    /* 垂直同步 */
    {__shell_lcd_vsync,  "LCD_VSYNC",   "<lcd vertical synchronization>"},
    /* 分辨率 */
    {__shell_lcd_res,    "LCD_RES",     "<lcd resolution>"},
    /* 重启LCD */
    {__shell_lcd_reset,  "LCD_RESET",   "<lcd reset>"},
};

/**********************************触摸初始化************************************/
/**
 * \brief 校准函数
 */
aw_local int __ts_calibrate (uint8_t                 *screen_buf,
                             aw_ts_id                 id,
                             aw_ts_lib_calibration_t *p_cal,
                             void*                    p_fb)
{
    int                 i;
    struct aw_ts_state  sta;

    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    /* 初始化五点触摸数据 */
    p_cal->log[0].x = 60 - 1;
    p_cal->log[0].y = 60 - 1;
    p_cal->log[1].x = var_info.res.x_res - 60 - 1;
    p_cal->log[1].y = 60 - 1;
    p_cal->log[2].x = var_info.res.x_res - 60 - 1;
    p_cal->log[2].y = var_info.res.y_res - 60 - 1;
    p_cal->log[3].x = 60 - 1;
    p_cal->log[3].y = var_info.res.y_res - 60 - 1;
    p_cal->log[4].x = var_info.res.x_res / 2 - 1;
    p_cal->log[4].y = var_info.res.y_res / 2 - 1;

    p_cal->cal_res_x = var_info.res.x_res;
    p_cal->cal_res_y = var_info.res.y_res;

    /* 五点触摸 */
    for (i = 0; i < 5; i++) {
        /* 画出点图案*/
        if (i != 0) {
        	/* 清除上个点图案*/
            clear_cursor(p_fb, p_cal->log[i - 1].x, p_cal->log[i - 1].y, 10);
        }
        draw_cursor(p_fb, p_cal->log[i].x, p_cal->log[i].y, 10);

        while (1) {
            /* 等待获取光标所在位置触摸数据 */
            if ((aw_ts_get_phys(id, &sta, 1) > 0) &&
                (sta.pressed == AW_TRUE)) {
                p_cal->phy[i].x = sta.x;
                p_cal->phy[i].y = sta.y;

                aw_kprintf("\n x=%d, y=%d \r\n", (uint32_t)sta.x, (uint32_t)sta.y);
                while(1) {
                    aw_mdelay(500);
                    if ((aw_ts_get_phys(id, &sta, 1) == AW_OK) &&
                        (sta.pressed == AW_FALSE)) {
                        break;
                    }
                }
                break;
            }
            aw_mdelay(10);
        }

    }
    return 0;
}

/**
 * \brief 触摸初始化入口
 */
aw_local aw_ts_id __touch_handel (char * p_ts_name, void * p_fb)
{
    aw_ts_id                sys_ts;
    aw_ts_lib_calibration_t cal;
    uint8_t                *p_buf = NULL;

    aw_fb_fix_info_t fix_info;
    aw_fb_var_info_t var_info;
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_FINFO, &fix_info);
    aw_fb_ioctl(p_fb, AW_FB_CMD_GET_VINFO, &var_info);

    /* 获取触摸设备 */
    sys_ts = aw_ts_serv_id_get(p_ts_name, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        return NULL;
    }

    /*帧缓存地址*/
    p_buf = (uint8_t *)fix_info.vram_addr;
    /* 设置背景为黑色*/
    memset(p_buf, 0, var_info.buffer.buffer_size * var_info.buffer.buffer_num);

    /* 判断是否设备支持触摸校准 */
    if (aw_ts_calc_flag_get(sys_ts)) {
        /* 加载系统触摸数据 */
        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {
            /* 没有有效数据，进行校准 */
            do {
                if (__ts_calibrate(p_buf, sys_ts, &cal, p_fb) < 0) {
                    //todo
                }
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);
            /* 校准成功，保存触摸数据 */
            aw_ts_calc_data_write(sys_ts);
        }
    } else {
        /* 电容屏需要进行XY转换 */
        if (strcmp(p_ts_name, "ft5x06") == 0) {
          /* 评估板配套4.3电容屏需要进行XY转换 */
          aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
        }
    }

    return sys_ts;
}

/**
 * \brief 获取单点触摸值并划线
 */
aw_local void __get_ts_val_draw_line(void *p_fb, aw_ts_id sys_ts)
{
    static uint8_t state = 0;
    static uint32_t x0 = 0;
    static uint32_t y0 = 0;
    struct aw_ts_state      ts;
    /*frame buffer 设备信息货物*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_info);

    /* 持续获取触摸状态 */
    if (aw_ts_exec(sys_ts, &ts, 1) > 0) {
        if (ts.x < var_info.res.x_res && ts.y < var_info.res.y_res) {

            /* 设置画笔颜色 */
            set_pen_color(0x1f, 0x3f, 0x1f, var_info.bpp.word_lbpp);

            if(state == __DISCONTINUOUS) {
                /* 画第一次触摸的点 */
                draw_point ((void *)fix_info.vram_addr,
                            ts.x,
                            ts.y,
                            var_info.res.x_res,
                            var_info.bpp.word_lbpp);
                state = __CONTINUOUS;
            } else {
                draw_line(p_fb, x0, y0, ts.x, ts.y, 2);
            }

            x0 = ts.x; //保存上次触摸的横向值
            y0 = ts.y; //保存上次触摸的纵向值

            draw_xy(p_fb,
                    0,
                    var_info.res.y_res-20,
                    ts.x,
                    ts.y,
                    __g_color[AW_NELEMENTS(__g_color)-1]);
        }
    } else {
        state = __DISCONTINUOUS;
    }
}

/**
 * \brief 获取多点触摸值
 */
aw_local void __get_ts_multi_val_draw(void *p_fb, char * p_ts_name)
{
    struct aw_ts_state      ts[5];
    int i;
    aw_ts_id                sys_ts;
    /*frame buffer 设备信息货物*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_info);
    int multi_ts_cnt;
    int pressed_num;
    static int flag = AW_FALSE;

    /* 获取触摸设备 */
    sys_ts = aw_ts_serv_id_get(p_ts_name, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        return ;
    }

    __GET_MULTI_TS_CNT(p_ts_name, &multi_ts_cnt);

    /* 持续获取触摸状态 */
    pressed_num = aw_ts_exec(sys_ts, ts, multi_ts_cnt);
    if (pressed_num > 0) {

        for(i = 0; i < multi_ts_cnt; i++) {
            if (ts[i].pressed == 1) {

                set_pen_color(__g_color[i][0],
                              __g_color[i][1],
                              __g_color[i][2],
                              var_info.bpp.word_lbpp);
                draw_circle(p_fb, ts[i].x, ts[i].y, 60);
                draw_xy(p_fb,
                        0,
                        var_info.res.y_res-(20*(i+1)),
                        ts[i].x,
                        ts[i].y,
                        __g_color[i]);
            }
        }
    }

    if (pressed_num != 0 && flag == AW_FALSE) {
        memset((void *)fix_info.vram_addr,
               0,
               var_info.buffer.buffer_size * var_info.buffer.buffer_num);
        flag = AW_TRUE;
    } else if (pressed_num == 0 && flag == AW_TRUE) {
        draw_lcd(__g_p_fb);
        flag = AW_FALSE;
    }
}

/**
 * \brief shell任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
aw_local void __lcd_task (void *p_arg)
{
    static aw_shell_cmd_list_t cmd_list;
    aw_ts_id                sys_ts;

    /* 打开frame buffer */
    __g_p_fb = aw_fb_open(__FB_NAME, 0);
    if (__g_p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return;
    }

    /*frame buffer 初始化*/
    aw_fb_init(__g_p_fb);

    /*frame buffer 设备信息货物*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&fix_info);

    /*frame buffer设备信息开启*/
    aw_fb_start(__g_p_fb);

    memset((void *)fix_info.vram_addr,
           0,
           var_info.buffer.buffer_size * var_info.buffer.buffer_num);

    /* 打开背光并运行面板 */
    aw_fb_backlight(__g_p_fb, 100);

    /* 触屏校准 */
    sys_ts = __touch_handel(__TS_NAME, __g_p_fb);
    if (sys_ts == NULL) {
        while(1);
    }

    draw_lcd(__g_p_fb);

    /* 添加自定义shell命令  */
    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_my_lcd_cmds);

    aw_shell_system("aworks");  /* 执行aworks命令  */

    /* 触摸画线 */
    while (1) {

#ifdef MULTI_TS
        __get_ts_multi_val_draw(__g_p_fb, __TS_NAME);
#else
        __get_ts_val_draw_line(__g_p_fb, sys_ts);
#endif

        aw_mdelay(5);
    }
}

/******************************************************************************/
void demo_lcd_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Lcd demo",
                         12,
                         4096,
                         __lcd_task,
                         NULL);
    if (tsk != NULL) {
        aw_kprintf("lcd shell task create ok.\r\n");
        if (aw_task_startup(tsk) == AW_OK) {
            aw_kprintf("lcd shell task startup ok.\r\n");
        }else{
            aw_kprintf("lcd shell task startup fail.\r\n");
        }
    }else{
        aw_kprintf("lcd shell task create fail.\r\n");
    }

    return;
}

/** [src_lcd] */

/* end of file */
