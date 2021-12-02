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
 * \brief ������������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_XXX_SAI1
 *      - AW_DEV_XXX_MQS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ������/������������ӵ�HPL/HPR����
 *
 * - ʵ������
 *   1. ����shell���"play_wav"��������Ƶ��
 *   2. ����shell���"play_wav [path]"������·���е���Ƶ�ļ���
 *
 * \par Դ����
 * \snippet demo_sai.c src_sai
 *
 * \internal
 * \par Modification history
 * - 1.00 19-05-23  may, first implementation.
 * \endinternal
 */
/**
 * \addtogroup demo_if_sai ��������
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

/* wav��Ƶ�ļ�����*/
extern const unsigned char g_sound_wav_data[];
/* wav��Ƶ�ļ����ݳ���*/
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

/* WAV shell����ص�����*/
/* argc�������������*/
/* argv���������*/
/* sh��shell�û���ӡ����*/
aw_local int play_wave_entry (int argc, char *argv[], struct aw_shell_user *sh)
{
    int ret;

    if (argc == 1 && !strcmp(argv[0], "--help")) {
        aw_shell_printf(sh, sai_help_info);
        return AW_OK;
    }

    /* �д����������ò����ļ�����*/
    if (1 == argc) {
        ret = aw_imx6ul_sound_wav_file_play_sync(argv[0]);
    } else {  /* û�в����������ͨ���ź���*/
        ret = aw_imx6ul_sound_wav_play_sync(g_sound_wav_data,
                  g_sound_wav_data_len);
    }

    if (0 != ret) {
        aw_shell_printf(sh, "play failed\n");
    }

    return 0;
}

/* shell����*/
/* play_wave_entry���ص�����*/
aw_local const struct aw_shell_cmd __g_play_wave_test_cmds[] = {
    {play_wave_entry,  "play_wav",  "Play the WAV file"},
};


/* SAI���Գ������*/
void demo_sai_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;
    /* ע��shell����*/
    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_play_wave_test_cmds);
}
/** [src_sai] */

/* end of file */
