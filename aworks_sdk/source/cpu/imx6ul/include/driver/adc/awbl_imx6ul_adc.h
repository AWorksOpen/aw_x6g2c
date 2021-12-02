/******************************************************************************
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
 * \brief imx6ul adc driver head file
 *
 * \internal
 * \par Modification History
 * - 1.01 16-05-30  sky, modified.
 * - 1.00 16-03-21  lnk, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_ADC_H
#define __AWBL_IMX6UL_ADC_H

#include "apollo.h"
#include "aw_gpio.h"
#include "awbl_adc.h"
#include "aw_spinlock.h"
#include "aw_isr_defer.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief imx6ul ADC ������ */
#define IMX6UL_ADC_NAME         "imx6ul_adc"

/** \brief imx6ul ADC ͨ������ */
#define IMX6UL_ADC1_MIN_CH       0          /* 0 -> GPIO1_0 ... 9 -> GPIO1_9 */
#define IMX6UL_ADC1_MAX_CH       15
#define IMX6UL_ADC2_MIN_CH       16         /* 16+0 -> GPIO1_0 ... 16+9 -> GPIO1_9 */
#define IMX6UL_ADC2_MAX_CH       31

/**
 * \brief imx6ul adc �豸��Ϣ
 */
typedef struct awbl_imx6ul_adc_devinfo {
    struct awbl_adc_servinfo adc_servinfo;     /**< \brief ADC ���������Ϣ */
    uint32_t                 regbase;          /**< \brief ADC �Ĵ�������ַ  */
    uint8_t                  inum;             /**< \brief ADC �жϺ� */
    uint8_t                  bits;             /**< \brief ADC λ�� */
    uint32_t                 refmv;            /**< \brief ADC �ο���ѹ mV */
    const aw_clk_id_t       *dev_clks;         /**< \brief the clocks which the timer need */
    int                      dev_clk_count;    /**< \brief count of device clocks */
    void (*pfn_plfm_init) (void);              /**< \brief ƽ̨��ʼ������ */
} awbl_imx6ul_adc_devinfo_t;

/**
 * \brief imx6ul adc �豸ʵ��
 */
typedef struct awbl_imx6ul_adc_dev {

    struct awbl_dev         super;             /**< \brief �̳��� AWBus �豸 */
    struct awbl_adc_service adc_serv;          /**< \brief ADC ���� */

    aw_adc_channel_t        phy_chan;          /**< \brief �豸��ǰת����ͨ�� */

    aw_adc_buf_desc_t      *p_bufdesc;         /**< \brief ָ��������������� */
    uint32_t                bufdesc_num;       /**< \brief ���������� */
    uint32_t                cur_bufdesc_id;    /**< \brief ���ڴ���Ļ�����ID */

    uint32_t                samples;           /**< \brief ����������ɵĴ��� */
    uint32_t                remain_count;      /**< \brief ʣ�������ת������ */
    aw_bool_t                  is_stopped;        /**< \brief ��־�Ƿ�ǿ��ֹͣ */

    pfn_adc_complete_t      pfn_seq_complete;  /**< \brief ������ɻص����� */
    void                   *p_arg;             /**< \brief �ص��������� */

} awbl_imx6ul_adc_dev_t;

/**
 * \brief imx6ul ADC ����ע��
 */
void awbl_imx6ul_adc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX6UL_ADC_H */

/* end of file */
