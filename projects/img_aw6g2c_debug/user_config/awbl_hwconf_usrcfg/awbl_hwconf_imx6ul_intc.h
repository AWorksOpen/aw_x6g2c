/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  �жϿ�����������Ϣ
*******************************************************************************/
#include "driver/intctlr/awbl_imx6ul_intc.h"

/* �豸��Ϣ */
aw_local aw_const awbl_imx6ul_intc_devinfo_t  __g_imx6ul_int_param = {
    {
        0,      /* ֧�ֵ���ʼ�жϺ� */
        159     /* ֧�ֵĽ����жϺ� */
    },
    0x00A00000  /* �Ĵ�������ַ */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx6ul_intc_dev_t __g_imx6ul_int;

#define AWBL_HWCONF_IMX6UL_INTC     \
    {                               \
        AWBL_IMX6UL_INTC_NAME,      \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_imx6ul_int.super),    \
        &(__g_imx6ul_int_param)     \
    },

/* end of file */

