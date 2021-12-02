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
 * \brief frame buffer����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ�ܣ�
 *      - AW_COM_CONSOLE��
 *      - ��Ӧƽ̨�Ĵ��ںꣻ
 *      - AW_COM_SHELL_SERIAL
 *      - ʹ�õ���ʾ���ĺꡢ��������ĺꣻ
 *
 *   2. ��shell�����նˣ����ò�����Ϊ115200-8-N-1.
 *   3. ��LCD�����ļ����������Ĭ�ϲ�������__HW800480_BUS_BPP 16��__HW800480_WORD_LBPP 16��
 *      ���ʾ16λ����������16���������ߵĿ�ȵ�LCD��ʾ��__HW800480_BUS_BPP 24��
 *      __HW800480_WORD_LBPP 24�����ʾ24λ����������24���������ߵĿ�ȵ�LCD��ʾ
 *   4. ���LCD�������Ǿ�̬����,����Ҫע��LCD�öεĴ�С�����ܵ���һ֡���ݴ�С��
 *      ����������RGB565(16bpp),800*480�ֱ��ʣ�һ֡���ݴ�СΪ800*480*2=768000
 *      ����������RGB888(24bpp),800*480�ֱ��ʣ�һ֡���ݴ�СΪ800*480*3=1152000
 *
 * - ʵ������
 *   1. �Ƚ�����ĻУ׼��
 *   2. ��Ļ�м����������ʾ�������������7�����ƣ�
 *   3. �����Ļ�м���ʾһ����ɫ�ĺ��ߣ��ұ���ʾһ����ɫ�����ߣ�
 *   4. ��Ļ���Ͻ���ʾ��AWorks��;
 *   5. ������ĻĬ�Ͻ�����Ļ�ϻ���,ͨ����MULTI_TS����ѡ���ǵ��㻮�߻��ǲ��Զ�㴥����
 *   6. ��Ļ���½ǳ���(x,y)��������ֵ
 *   7. �û�ͨ��shell�ն�,��������:
 *      ����"LCD_DCLK 48000000"���������LCDʱ��Ƶ��
 *      ����"LCD_BPP 16 16"��"LCD_BPP 16 24"�����"LCD_BPP 24 16"�����"LCD_BPP 24 24"���
 *          ������LCDλ������ȣ�һ�����صĴ�С���� �������ߵĿ�ȣ�֧��4��ģʽ��
 *      ����"LCD_HSYNC 1 800 1000 46"���������LCDˮƽͬ���ź���Ϣ
 *      ����"LCD_VSYNC 1 480 22 23"���������LCD��ֱͬ���ź���Ϣ
 *      ����"LCD_RES   800 480"�����������Ļ�ֱ��ʣ�ʹ�ø������޸ķֱ��ʣ����LCD�������Ǿ�̬���Σ����Ի�
 *          �����ڴ��洦�������ʹ�ã�ȷ���ָ�LCD��̬���εĴ�С�㹻��
 *          ����ֱ��ʸı䣬��HSYNC��VSYNC����ҲҪ����Ӧ�ı䣬�����"LCD_RES 480 272"������������
 *          "LCD_HSYNC 1 480 4 15"��"LCD_VSYNC 1 272 114 13"�����ڶ����������Ӧ��
 *      ����"LCD_RESET"����ḴλLCD
 *   8. �û����úò���֮�������"LCD_RESET"���λLCD���ۿ����κ����ʾЧ��
 *
 * \par Դ����
 * \snippet demo_lcd.c src_lcd
 *
 * \internal
 * \par Modification history
 * - 1.00 18-07-16  sdq, first implementation.
 * - 2.00 21-04-13  swh, second implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lcd ��ʾ������
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

/* �Ƿ���Զ�㴥���� ��ע�͵� ���ǣ��򿪶���*/
//#define MULTI_TS

/* �������豸������֧�ֵĵ��� */
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
#define __TS_NAME    DE_TS_NAME           /*����������*/

#define __FB_NAME    DE_FB                /*��ʾ������*/

/* ���� */
#define __CONTINUOUS      1
/* ������ */
#define __DISCONTINUOUS   0

aw_local void     * __g_p_fb;
aw_fb_fix_info_t  g_fix_screen_info;
aw_fb_var_info_t  g_var_screen_info;

aw_local uint8_t __g_color[][3] = {{0xff, 0x00, 0x00},   //��ɫ
                                   {0x00, 0xff, 0x00},   //��ɫ
                                   {0x00, 0x00, 0xff},   //��ɫ
                                   {0xff, 0xff, 0x00},   //��ɫ
                                   {0xff, 0xff, 0xff}};  //��ɫ

/*****************************ʱ����λ�̬����***********************************/
/**
 * \brief shell����LCDʱ��Ƶ���������
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

    /* ����ʱ��Ƶ�� */
    g_var_screen_info.dclk_f.typ_dclk_f = atoi(argv[0]); /** \brief ����ʱ��Hz */
    aw_fb_config(__g_p_fb, AW_FB_CONF_DCLK_VINFO, &g_var_screen_info.dclk_f);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell����LCDλ��������������
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

    /* ����bpp */
    g_var_screen_info.bpp.word_lbpp     = atoi(argv[0]);
    g_var_screen_info.bpp.bus_bpp       = atoi(argv[1]);
    aw_fb_config(__g_p_fb, AW_FB_CONF_BPP_VINFO, &g_var_screen_info.bpp);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell����LCDˮƽͬ����Ϣ�������
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

    /* ������ͬ����Ϣ */
    g_var_screen_info.hsync.h_pulse_width = atoi(argv[0]); /** \brief �������� */
    g_var_screen_info.hsync.h_active      = atoi(argv[1]); /** \brief һ�е����ظ��� */
    g_var_screen_info.hsync.h_fporch      = atoi(argv[2]); /** \brief ˮƽǰ�� */
    g_var_screen_info.hsync.h_bporch      = atoi(argv[3]); /** \brief ˮƽ���� */
    aw_fb_config(__g_p_fb, AW_FB_CONF_HSYNC_VINFO, &g_var_screen_info.hsync);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell����LCD��ֱͬ����Ϣ�������
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

    /* ���ó�ͬ����Ϣ */
    g_var_screen_info.vsync.v_pulse_width = atoi(argv[0]); /** \brief ��ֱ������ */
    g_var_screen_info.vsync.v_active      = atoi(argv[1]); /** \brief һ֡��������� */
    g_var_screen_info.vsync.v_fporch      = atoi(argv[2]); /** \brief ��ֱǰ�� */
    g_var_screen_info.vsync.v_bporch      = atoi(argv[3]); /** \brief ��ֱ���� */
    aw_fb_config(__g_p_fb, AW_FB_CONF_VSYNC_VINFO, &g_var_screen_info.vsync);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell����LCD�ֱ����������
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

        /* ������Ļ�ֱ��� */
    g_var_screen_info.res.x_res         = atoi(argv[0]);
    g_var_screen_info.res.y_res         = atoi(argv[1]);
    aw_fb_config(__g_p_fb, AW_FB_CONF_RES_VINFO, &g_var_screen_info.res);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&g_var_screen_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&g_fix_screen_info);

    return AW_OK;
}

/**
 * \brief shell����LCD�������
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

    /* ����֡�����豸 */
    aw_fb_reset(__g_p_fb);

    draw_lcd(__g_p_fb);

    return AW_OK;
}

/**
 * \brief shell lcd���������
 */
aw_local const aw_shell_cmd_item_t __g_my_lcd_cmds[] = {
    /* ʱ��Ƶ�� */
    {__shell_lcd_dclk,   "LCD_DCLK",    "<lcd clock frequency>"},
    /* λ�������  */
    {__shell_lcd_bpp,    "LCD_BPP",     "<lcd bit pixel depth>"},
    /* ˮƽͬ�� */
    {__shell_lcd_hsync,  "LCD_HSYNC",   "<lcd horizontal synchronization>"},
    /* ��ֱͬ�� */
    {__shell_lcd_vsync,  "LCD_VSYNC",   "<lcd vertical synchronization>"},
    /* �ֱ��� */
    {__shell_lcd_res,    "LCD_RES",     "<lcd resolution>"},
    /* ����LCD */
    {__shell_lcd_reset,  "LCD_RESET",   "<lcd reset>"},
};

/**********************************������ʼ��************************************/
/**
 * \brief У׼����
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

    /* ��ʼ����㴥������ */
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

    /* ��㴥�� */
    for (i = 0; i < 5; i++) {
        /* ������ͼ��*/
        if (i != 0) {
        	/* ����ϸ���ͼ��*/
            clear_cursor(p_fb, p_cal->log[i - 1].x, p_cal->log[i - 1].y, 10);
        }
        draw_cursor(p_fb, p_cal->log[i].x, p_cal->log[i].y, 10);

        while (1) {
            /* �ȴ���ȡ�������λ�ô������� */
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
 * \brief ������ʼ�����
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

    /* ��ȡ�����豸 */
    sys_ts = aw_ts_serv_id_get(p_ts_name, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        return NULL;
    }

    /*֡�����ַ*/
    p_buf = (uint8_t *)fix_info.vram_addr;
    /* ���ñ���Ϊ��ɫ*/
    memset(p_buf, 0, var_info.buffer.buffer_size * var_info.buffer.buffer_num);

    /* �ж��Ƿ��豸֧�ִ���У׼ */
    if (aw_ts_calc_flag_get(sys_ts)) {
        /* ����ϵͳ�������� */
        if (aw_ts_calc_data_read(sys_ts) != AW_OK) {
            /* û����Ч���ݣ�����У׼ */
            do {
                if (__ts_calibrate(p_buf, sys_ts, &cal, p_fb) < 0) {
                    //todo
                }
            } while (aw_ts_calibrate(sys_ts, &cal) != AW_OK);
            /* У׼�ɹ������津������ */
            aw_ts_calc_data_write(sys_ts);
        }
    } else {
        /* ��������Ҫ����XYת�� */
        if (strcmp(p_ts_name, "ft5x06") == 0) {
          /* ����������4.3��������Ҫ����XYת�� */
          aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
        }
    }

    return sys_ts;
}

/**
 * \brief ��ȡ���㴥��ֵ������
 */
aw_local void __get_ts_val_draw_line(void *p_fb, aw_ts_id sys_ts)
{
    static uint8_t state = 0;
    static uint32_t x0 = 0;
    static uint32_t y0 = 0;
    struct aw_ts_state      ts;
    /*frame buffer �豸��Ϣ����*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_info);

    /* ������ȡ����״̬ */
    if (aw_ts_exec(sys_ts, &ts, 1) > 0) {
        if (ts.x < var_info.res.x_res && ts.y < var_info.res.y_res) {

            /* ���û�����ɫ */
            set_pen_color(0x1f, 0x3f, 0x1f, var_info.bpp.word_lbpp);

            if(state == __DISCONTINUOUS) {
                /* ����һ�δ����ĵ� */
                draw_point ((void *)fix_info.vram_addr,
                            ts.x,
                            ts.y,
                            var_info.res.x_res,
                            var_info.bpp.word_lbpp);
                state = __CONTINUOUS;
            } else {
                draw_line(p_fb, x0, y0, ts.x, ts.y, 2);
            }

            x0 = ts.x; //�����ϴδ����ĺ���ֵ
            y0 = ts.y; //�����ϴδ���������ֵ

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
 * \brief ��ȡ��㴥��ֵ
 */
aw_local void __get_ts_multi_val_draw(void *p_fb, char * p_ts_name)
{
    struct aw_ts_state      ts[5];
    int i;
    aw_ts_id                sys_ts;
    /*frame buffer �豸��Ϣ����*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(p_fb,AW_FB_CMD_GET_FINFO,&fix_info);
    int multi_ts_cnt;
    int pressed_num;
    static int flag = AW_FALSE;

    /* ��ȡ�����豸 */
    sys_ts = aw_ts_serv_id_get(p_ts_name, 0, 0);

    if (sys_ts == NULL) {
        aw_kprintf("get touch server failed\r\n");
        return ;
    }

    __GET_MULTI_TS_CNT(p_ts_name, &multi_ts_cnt);

    /* ������ȡ����״̬ */
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
 * \brief shell�������
 * \param[in] p_arg : �������
 * \return ��
 */
aw_local void __lcd_task (void *p_arg)
{
    static aw_shell_cmd_list_t cmd_list;
    aw_ts_id                sys_ts;

    /* ��frame buffer */
    __g_p_fb = aw_fb_open(__FB_NAME, 0);
    if (__g_p_fb == NULL) {
        aw_kprintf("FB open failed.\r\n");
        return;
    }

    /*frame buffer ��ʼ��*/
    aw_fb_init(__g_p_fb);

    /*frame buffer �豸��Ϣ����*/
    aw_fb_fix_info_t  fix_info;
    aw_fb_var_info_t  var_info;
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_VINFO,&var_info);
    aw_fb_ioctl(__g_p_fb,AW_FB_CMD_GET_FINFO,&fix_info);

    /*frame buffer�豸��Ϣ����*/
    aw_fb_start(__g_p_fb);

    memset((void *)fix_info.vram_addr,
           0,
           var_info.buffer.buffer_size * var_info.buffer.buffer_num);

    /* �򿪱��Ⲣ������� */
    aw_fb_backlight(__g_p_fb, 100);

    /* ����У׼ */
    sys_ts = __touch_handel(__TS_NAME, __g_p_fb);
    if (sys_ts == NULL) {
        while(1);
    }

    draw_lcd(__g_p_fb);

    /* ����Զ���shell����  */
    AW_SHELL_REGISTER_CMDS(&cmd_list, __g_my_lcd_cmds);

    aw_shell_system("aworks");  /* ִ��aworks����  */

    /* �������� */
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
