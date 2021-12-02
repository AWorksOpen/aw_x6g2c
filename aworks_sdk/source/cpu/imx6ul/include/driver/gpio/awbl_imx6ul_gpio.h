/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX6UL GPIO ����
 *
 * iMX6UL GPIO Ϊ iMX6UL ϵ��MCU��Ƭ��GPIO����������160��GPIO���ſ��ã�������
 * ����:
 *  - ���ſ������������Ϊ����/���
 *  - ÿ���������ſɱ������ⲿ�ж���������
 *  - ÿ���жϿ�����Ϊ�½��ء������ػ�ߵ͵�ƽ�����ж�
 *  - ����GPIO ����Ĭ��Ϊ����
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "imx6ul_gpio"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_imx6ul_gpio_devinfo
 *
 * \internal
 * \par modification history:
 * - 1.00 2015-10-24, jac, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX6UL_GPIO_H
#define __AWBL_IMX6UL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio IMX6UL GPIO����
 * \copydetails awbl_imx6ul_gpio.h
 * @{
 */

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_regs IMX6UL GPIO�Ĵ�������
 * @{
 */

#pragma pack(push)
#pragma pack(1)

/** \brief imx6ul GPIOÿbank�Ĵ��� */
typedef struct __tag_imx6ul_gpio_bank_regs
{
    uint32_t    data;           /**< \brief GPIO data register */
    uint32_t    dir;            /**< \brief GPIO direction register */
    uint32_t    psr;            /**< \brief GPIO pad status register */
    uint32_t    icr1;           /**< \brief GPIO interrupt configuration register1 */
    uint32_t    icr2;           /**< \brief GPIO interrupt configuration register2 */
    uint32_t    imr;            /**< \brief GPIO interrupt mask register */
    uint32_t    isr;            /**< \brief GPIO interrupt status register */
    uint32_t    edge_sel;       /**< \brief GPIO edge select register */
}imx6ul_gpio_bank_regs_t;

/** \brief imx6ul GPIO�Ĵ��� */
typedef struct __tag_imx6ul_gpio_regs
{
    imx6ul_gpio_bank_regs_t     bank1;          /**< \brief GPIO bank1 */
    uint32_t                    reserved1[4088];
    imx6ul_gpio_bank_regs_t     bank2;          /**< \brief GPIO bank2 */
    uint32_t                    reserved2[4088];
    imx6ul_gpio_bank_regs_t     bank3;          /**< \brief GPIO bank3 */
    uint32_t                    reserved3[4088];
    imx6ul_gpio_bank_regs_t     bank4;          /**< \brief GPIO bank4 */
    uint32_t                    reserved4[4088];
    imx6ul_gpio_bank_regs_t     bank5;          /**< \brief GPIO bank5 */
    uint32_t                    reserved5[4088];
}imx6ul_gpio_regs_t;

/** \brief ��GPIO�Ĵ�����ַ��ȡĳ���˿ڶ�Ӧ�Ĵ����Ļ�ַ */
#define GET_GPIO_BANK_REGS(p_gpio,x)  (imx6ul_gpio_bank_regs_t *)((uint32_t *)(p_gpio)+4096*(x) )

#pragma pack(pop)

/** @} grp_awbl_drv_imx6ul_gpio_regs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_micro_defs IMX6UL GPIO�����궨��
 * @{
 */

/** \brief imx6ul GPIO ������ */
#define AWBL_IMX6UL_GPIO_NAME   "imx6ul_gpio"

/** \brief imx6ul GPIO �Ķ˿�(bank)�� */
#define  IMX6UL_GPIO_PORTS   5
/** \brief imx6ul GPIO ÿ�˿ں���GPIO���� */
#define  IMX6UL_PORT_PINS    32

/** @} grp_awbl_drv_imx6ul_gpio_micro_defs*/

/**
 * \defgroup grp_awbl_drv_imx6ul_gpio_structs IMX6UL GPIO������صĽṹ
 * @{
 */

/** \brief imx6ul GPIOƽ̨��ʼ������ָ�� */
typedef void (*pfn_gpio_plat_init_t)(void);

/** \brief imx6ul GPIO �豸��Ϣ */
struct awbl_imx6ul_gpio_info
{
    pfn_gpio_plat_init_t         plat_init; /**< \brief ƽ̨��صĳ�ʼ������ */
    uint32_t                     mem_base;  /**< \brief GPIO �Ĵ�������ַ */
    uint32_t                     iomuxc_base;/**< \brief IOMUXC �Ĵ�������ַ */
    struct awbl_gpio_servinfo    gpio_info; /**< \brief GPIO ����������Ϣ */
    struct awbl_intctlr_servinfo int_info;  /**< \brief IntCtlr ����������Ϣ */
    uint8_t  *p_pin_cfg;   /**< \brief GPIO ��������״̬ */
};

/******************************************************************************/
/** \brief GPIO�����жϴ�����Ϣ�ṹ */
struct awbl_imx6ul_gpio_trigger_info
{
    aw_pfuncvoid_t  pfunc_callback;     /**< \brief �жϻص����� */
    void           *p_arg;              /**< \brief ���ݸ��ص������Ĳ��� */
};

/******************************************************************************/

/******************************************************************************/
/** \brief imx28x GPIO �豸ʵ�� */
struct awbl_imx6ul_gpio_dev
{
    struct awbl_dev             awdev;          /**< \brief �̳��� AWBus �豸 */
    struct awbl_gpio_service    gpio_serv;      /**< \brief GPIO ���� */
    struct awbl_intctlr_service intctlr_serv;   /**< \brief IntCtlr ���� */
    imx6ul_gpio_regs_t          *reg;
    uint32_t                     iomuxc_base;   /**< \brief IOMUXC �Ĵ�������ַ */
    /** \brief GPIO ����ISR��Ϣ */
    struct awbl_imx6ul_gpio_trigger_info trig_info[IMX6UL_GPIO_PORTS][IMX6UL_PORT_PINS];
    /** \brief GPIO ���Ŵ���״̬ */
    uint8_t  trig_state[IMX6UL_GPIO_PORTS][IMX6UL_PORT_PINS];
};
/** \brief ��Awbus �豸��imx6ul gpio�豸 */
#define awbldev_to_imx6ul_gpio_dev(dev)     ((struct awbl_imx6ul_gpio_dev *)(dev))
/** \brief ��Awbus �豸��imx6ul gpio�豸��Ϣ */
#define imx6ul_gpio_get_param(dev)          ((struct awbl_imx6ul_gpio_info *)AWBL_DEVINFO_GET(dev))

/** @} grp_awbl_drv_imx6ul_gpio_structs*/

/**
 * \addtogroup grp_awbl_drv_imx6ul_gpio_awbus_interface GPIO����Awbus�ӿ�
 * @{
 */
/** \brief �û����ô˺���ע��GPIO���� */
void awbl_imx6ul_gpio_drv_register (void);

/** @} grp_awbl_drv_imx6ul_gpio_usr_register */
/** @} grp_awbl_drv_imx6ul_gpio*/

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */
