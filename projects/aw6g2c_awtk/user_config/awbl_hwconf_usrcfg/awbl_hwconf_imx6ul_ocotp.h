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
  IMX6UL ����OTP  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX6UL_OCOTP_H_
#define AWBL_HWCONF_IMX6UL_OCOTP_H_

#include "driver/nvram/awbl_imx6ul_ocotp.h"
#include "imx6ul_regs_base.h"

#ifdef AW_DEV_IMX6UL_OCOTP

/**
 * \brief imx283 �ڲ� OCOTP �洢�������б�
 */
aw_local aw_const struct awbl_nvram_segment __g_imx6ul_ocotp_seglst[] = {
    /* �洢������, ��Ԫ��, ��ʼ��ַ������ */
    {"otp_zy_data", 0,  0x180,  96},    /* ��˾�������� */
    {"otp_zy_mac",  1,  0x88,   0x06},  /* MAC1 */
    {"otp_zy_mac",  2,  0x8E,   0x06},  /* MAC2 */
    {"otp_cust",    0,  0x1C0,  0x20},  /* Customer storage space   */
    {"otp_lock",    0,  0x00,   0x04},
    {"srk_hash",    0,  0x60,   0x20},
    {"otp_osp",     0,  0x04,   0x08},  /* ���ֶ�ֻ��0x4C~0x53����, �洢��ΨһID�ţ�ֻ�� */
    {"otp_all",     0,  0x00,   0x200},

};

/* imx283 �ڲ� OCOTP �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_ocotp_devinfo __g_imx6ul_ocotp_devinfo = {
    &__g_imx6ul_ocotp_seglst[0],            /* ����ʧ�Դ洢�������б� */
    IMX6UL_OCOTP_BASE_ADDR,                 /* �Ĵ�����ַ */
    0,                                      /* ����ʱ��ID */
    AW_NELEMENTS(__g_imx6ul_ocotp_seglst)   /* ����ʧ�Դ洢�������б���Ŀ�� */
};

/* imx283 �ڲ� OCOTP �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_ocotp_dev __g_imx6ul_ocotp_dev;

#define AWBL_HWCONF_IMX6UL_OCOTP                    \
    {                                               \
        AWBL_IMX6UL_OCOTP_NAME,                     \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_ocotp_dev,   \
        &__g_imx6ul_ocotp_devinfo                   \
    },
#else
#define AWBL_HWCONF_IMX6UL_OCOTP
#endif  /* AW_DEV_IMX_OCOTP */

#endif /* AWBL_HWCONF_IMX6UL_OCOTP_H_ */
