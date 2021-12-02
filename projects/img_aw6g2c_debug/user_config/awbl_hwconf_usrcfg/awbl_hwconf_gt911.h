/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief ����������������������Ϣ
 *
 * \internal
 * \par modification history:
 * - 1.01 16-08-18  anu, first implemetation
 * \endinternal
 */

#ifndef __AWBL_HWCONF_GT911_H
#define __AWBL_HWCONF_GT911_H

#ifdef AW_DEV_GT911

#include "aw_gpio.h"

#include "imx6ul_gpio_pin.h"
#include "driver/input/touchscreen/awbl_gt911_ts.h"
#include "aw_ts.h"

/* configure infomation */
aw_local struct awbl_gt911_ts_devinfo __g_gt911_ts_devinfo={
        GT911_ADDRESS1,                   /** \brief GT911�豸��ַ */
        GPIO89,                          /** \brief ����GT911�豸�ĸ�λ���� */
        GPIO90,                          /** \brief ����GT911�豸���ж����� */
        800,                              /** \brief �û������x��������� */
        480,                              /** \brief �û������y��������� */
        {
            "gt911",                      /**< \brief ������������ʾ������       */
            0,                            /**< \brief ������������ʾ������ID */
            0,                            /**< \brief ������������ʾ����ƷID */
            5,                            /**< \brief ֧����ഥ����ĸ���.    */
            3,                            /**< \brief ��������, ��СֵΪ1  */
            1,                            /**< \brief �˲�����, ���ܴ��ڲ�������, ��СֵΪ1 */
            10,                           /**< \brief ����ʱ��      */
            40,                           /**< \brief �������޷�ֵ,��������,���ݹ�����ͬ����ʾ�������������. */
            5,                            /**< \brief У׼���޷�ֵ,������֤У׼ϵ��,���ݹ�����ͬ����ʾ�������������. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief ���У׼�㷨   */
        },
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_gt911_ts_dev __g_gt911_ts;

#define AWBL_HWCONF_GT911              \
    {                                   \
        AWBL_GT911_TOUCHSCREEN_NAME,   \
        0,                              \
        AWBL_BUSID_I2C,                 \
        IMX6UL_GPIO_I2C0_BUSID,         \
        &(__g_gt911_ts.super),         \
        &(__g_gt911_ts_devinfo)        \
    },
#else

#define AWBL_HWCONF_GT911

#endif /* AW_DEV_FT5X06 */

#endif /* __AWBL_HWCONF_FT5X06_H */

