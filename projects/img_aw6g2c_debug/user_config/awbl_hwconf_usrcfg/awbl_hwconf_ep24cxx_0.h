/*******************************************************************************
  EEPROM CAT24C04 ������Ϣ
*******************************************************************************/
#ifndef AWBL_HWCONFG_EP24CXX_0_H
#define AWBL_HWCONFG_EP24CXX_0_H

#include "driver/nvram/awbl_ep24cxx.h"
#ifdef AW_DEV_EXTEND_EP24CXX_0

/* �洢�������б�1�� */
aw_local aw_const struct awbl_nvram_segment __g_ep24cxx_seglst[] = {

    /* �洢������, �洢�ε�Ԫ��, ��ʼ��ַ, ���� */
    {"ep24c02", 0, 0, 256},             /* ����EEPROM�洢�ռ� */
    {"data", 0, 0, 16},                 /* �洢�� "data"_0 */
    {"data", 1, 16, 16},                /* �洢�� "data"_1 */
    {"data2", 0, 32, 16}                /* �洢�� "data2"_0 */
};

/* CAT24C256 �豸��Ϣ */
aw_local aw_const struct awbl_ep24cxx_devinfo __g_ep24cxx_devinfo = {
    0x50,                               /* I2C�ӻ���ַ ��2�� */
    AWBL_EP24CXX_EP24C02,               /* оƬ�ͺ� ��3�� */
    &__g_ep24cxx_seglst[0],             /* ����ʧ�Դ洢�������б� */
    AW_NELEMENTS(__g_ep24cxx_seglst)    /* ����ʧ�Դ洢�������б���Ŀ�� */
};

/* CAT24C04 0 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_ep24cxx_dev __g_ep24cxx_dev;

/*******************************************************************************
  Ӳ���豸�б�
*******************************************************************************/

#define AWBL_HWCONF_EP24CXX_0  \
    {                          \
        "ep24cxx",             \
        0,                     \
        AWBL_BUSID_I2C,        \
        IMX6UL_I2C1_BUSID,     \
        (struct awbl_dev *)&__g_ep24cxx_dev, \
        &__g_ep24cxx_devinfo   \
    },
#else
#define AWBL_HWCONF_EP24CXX_0

#endif
#endif
