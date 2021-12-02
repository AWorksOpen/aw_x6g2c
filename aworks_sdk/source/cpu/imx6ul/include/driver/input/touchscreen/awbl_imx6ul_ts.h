/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief TI imx6ul ����������������ͷ�ļ�
 *
 * \internal
 * \par modification history:
 * - 1.01 16-06-15  sky, modified.
 * - 1.00 16-03-22  lnk, first implemetation.
 * \endinternal
 */
 
#ifndef __AWBL_IMX6UL_TS_H
#define __AWBL_IMX6UL_TS_H

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "awbl_ts.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief imx6ul ������������ */
#define AWBL_IMX6UL_TOUCHSCREEN_NAME        "imx6ul_touchscreenctrl"

/** \brief imx6ul ���������豸��Ϣ  */
typedef struct awbl_imx6ul_ts_devinfo {
    uint32_t            regbase;        /**< \brief TSC�Ĵ�������ַ */
    uint8_t             x_plus_ch;      /**< \brief X����ADCͨ�� */
    uint8_t             x_minus_ch;     /**< \brief X����ADCͨ�� */
    uint8_t             y_plus_ch;      /**< \brief Y����ADCͨ�� */
    uint8_t             y_minus_ch;     /**< \brief Y����ADCͨ�� */

    awbl_ts_service_info_t ts_serv_info;
    void (*pfunc_plfm_init)(void);      /**< \brief ƽ̨��س�ʼ�� */

} awbl_imx6ul_ts_devinfo_t;

/** \brief imx6ul ���������豸 */
typedef struct awbl_imx6ul_ts_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev adev;

    /** \bried �̳���Touch SCreen�豸 */
    struct awbl_ts_service ts_serv;

    /** \brief �������Ƿ��� */
    aw_bool_t is_pressed;
}awbl_imx6ul_ts_dev_t;

/**
 * \brief imx6ul ��������ע�ắ��
 */
void awbl_imx6ul_touch_screen_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_TS_H */

/* end of file */
