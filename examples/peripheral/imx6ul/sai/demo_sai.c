/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 声卡播放例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_XXX_SAI1
 *      - AW_DEV_XXX_MQS
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 将耳机/扬声器接入板子的HPL/HPR引脚
 *
 * - 实验现象：
 *   1. 输入shell命令："play_wav"，播放音频；
 *   2. 输入shell命令："play_wav [path]"，播放路径中的音频文件；
 *
 * \par 源代码
 * \snippet demo_sai.c src_sai
 *
 * \internal
 * \par Modification history
 * - 1.00 19-05-23  may, first implementation.
 * \endinternal
 */
/**
 * \addtogroup demo_if_sai 声卡播放
 * \copydoc demo_sai.c
 */

/** [src_sai] */
#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "imx6ul_sound_wav.h"
#include "aw_delay.h"
#include "aw_shell.h"
#include <string.h>

/* wav音频文件数据*/
extern const unsigned char g_sound_wav_data[];
/* wav音频文件数据长度*/
extern const unsigned int  g_sound_wav_data_len;

aw_local aw_const char sai_help_info[] = {
    "Command:play_wav [path]\n\n"
    "Description:Play the WAV file\n\n"
    "Examples:\n"
    "    1.Play the WAV file of the wav array data:\n"
    "      play_wav\n"
    "    2.Play the WAV file of the specified path:\n"
    "      play_wav [path]\n"
};

/* WAV shell命令回调函数*/
/* argc：命令参数个数*/
/* argv：命令参数*/
/* sh：shell用户打印数据*/
aw_local int play_wave_entry (int argc, char *argv[], struct aw_shell_user *sh)
{
    int ret;

    if (argc == 1 && !strcmp(argv[0], "--help")) {
        aw_shell_printf(sh, sai_help_info);
        return AW_OK;
    }

    /* 有带参数，调用播放文件函数*/
    if (1 == argc) {
        ret = aw_imx6ul_sound_wav_file_play_sync(argv[0]);
    } else {  /* 没有参数则调用普通播放函数*/
        ret = aw_imx6ul_sound_wav_play_sync(g_sound_wav_data,
                  g_sound_wav_data_len);
    }

    if (0 != ret) {
        aw_shell_printf(sh, "play failed\n");
    }

    return 0;
}

/* shell命令*/
/* play_wave_entry：回调函数*/
aw_local const struct aw_shell_cmd __g_play_wave_test_cmds[] = {
    {play_wave_entry,  "play_wav",  "Play the WAV file"},
};


/* SAI测试程序入口*/
void demo_sai_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;
    /* 注册shell命令*/
    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_play_wave_test_cmds);
}
/** [src_sai] */

/* end of file */
