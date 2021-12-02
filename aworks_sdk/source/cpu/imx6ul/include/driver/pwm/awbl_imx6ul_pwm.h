/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Freescale i.MX6ULϵ��PWM��������
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-07  hgo, first implemetation
 * \endinternal
 */
#ifndef __AWBL_IMX6UL_PWM_H
#define __AWBL_IMX6UL_PWM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "awbl_pwm.h"

#define AWBL_IMX6UL_PWM_NAME            "imx6ul_pwm"
/*******************************************************************************
 ��������
*******************************************************************************/
/** \brief PWM��STOPģʽ�¿�����  */
#define AWBL_IMX6UL_PWM_CFG_STOPEN  (0x1u << 7)
/** \brief PWM��Dozeģʽ�¿�����  */
#define AWBL_IMX6UL_PWM_CFG_DOZEN   (0x1u << 6)
/** \brief PWM��Waitģʽ�¿�����  */
#define AWBL_IMX6UL_PWM_CFG_WAITEN  (0x1u << 5)
/** \brief PWM��Debugģʽ�¿�����  */
#define AWBL_IMX6UL_PWM_CFG_DBGEN   (0x1u << 4)
/*******************************************************************************
 PWMʱ������
*******************************************************************************/
/**
 * \brief PWMʱ��ѡ��Ϊipg_clk
 * \note  ����ֹͣģʽ��PWM��Ҫ����ʱ��ʱ�Ӳ������ó� #AWBL_IMX6UL_PWM_CFG_CLKSRC_IPG_CLK
 *        ��Ϊֹͣģʽ�£�ipg_clk�ѹرա�
 */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_IPG_CLK        (0x1u << 2)
/** \brief PWMʱ��ѡ��Ϊipg_clk_highfreq  */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_PERCLK_CLK     (0x2u << 2)
/** \brief PWMʱ��ѡ��Ϊipg_clk_32k  */
#define AWBL_IMX6UL_PWM_CFG_CLKSRC_CKIL_SYNC_CLK  (0x3u << 2)
/*******************************************************************************
 PWM�����������
*******************************************************************************/
/** \brief Ĭ��PWM��ƽΪ�� , ����ƥ�䵽ռ�ձȼ���ʱ��Ϊ�͵�ƽ��ƥ�䵽����ʱ��Ϊ�ߵ�ƽ */
#define AWBL_IMX6UL_PWM_CFG_LEVEL_HIGH     (0x0u << 0)
/** \brief Ĭ��PWM��ƽΪ�� ,�� ��ƥ�䵽ռ�ձȼ���ʱ��Ϊ�ߵ�ƽ��ƥ�䵽����ʱ��Ϊ�͵�ƽ */
#define AWBL_IMX6UL_PWM_CFG_LEVEL_LOW      (0x1u << 0)

/** \brief PWM����Ĭ������ */
#define AWBL_IMX6UL_PWM_CFG_DEFAULT  (AWBL_IMX6UL_PWM_CFG_CLKSRC_PERCLK_CLK |\
                                      AWBL_IMX6UL_PWM_CFG_LEVEL_HIGH)

#define __IMX6UL_PWM_NUM     8  /**< \brief PWM���� */
/**
 * \brief imx6ul PWM �豸����
 */
typedef struct awbl_imx6ul_pwm_devcfg {
    uint8_t  pid;                     /**< \brief PWM ID */
    uint32_t gpio;                    /**< \brief PWM��Ӧ������ */
    uint32_t func;                    /**< \brief PWM���Ź��� */
    uint32_t dfunc;                   /**< \brief ����PWM���Ĭ�����Ź��� */
    /**
     * \brief PWM��������
     * \note ����ʹ�� #AWBL_IMX6UL_PWM_CFG_*������һ�������Ļ�ֵ������
     *       һ��Ҫ����PWM��ʱ��Դ�������ҽ���AWBL_IMX6UL_PWM_CFG_CLKSRC_*��ֵ
     */
    uint8_t  cfgs;
}awbl_imx6ul_pwm_devcfg_t;

/**
 * \brief imx6ul PWM �豸��Ϣ
 */
typedef struct awbl_imx6ul_pwm_devinfo {
    struct awbl_pwm_servinfo pwm_servinfo;      /**< \brief PWM���������Ϣ */
    struct awbl_imx6ul_pwm_devcfg *pwm_devcfg;  /**< \brief pwm������Ϣ */
    uint8_t  pnum;                              /**< \brief ʹ�õ���pwmͨ����*/
}awbl_imx6ul_pwm_devinfo_t;

/**
 * \brief imx6ul PWM �豸ʵ��
 */
typedef struct awbl_imx6ul_pwm_dev {
    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief PWM ����*/
    struct awbl_pwm_service pwm_serv;
    
    /** \brief ������Դʱ��*/
    aw_clk_id_t counter_src_clk_id[__IMX6UL_PWM_NUM];

}awbl_imx6ul_pwm_dev_t;

/**
 * \brief imx6ul3 pwm driver register
 */
void awbl_imx6ul3_pwm_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWBL_IMX6UL_PWM_H */
