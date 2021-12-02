/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * awbl_imx6ul_can.h
 * \internal
 * \par modification history:
 * - 1.00 16-06-24  sky, first implementation
 * \endinternal
 */
#include "driver/thermistor/awbl_temp.h"
#include "aw_sem.h"

#ifndef __AWBL_IMX6UL_TEMPMON_H
#define __AWBL_IMX6UL_TEMPMON_H

#define AWBL_IMX6UL_TEMPMON_NAME "imx6ul_tempmon"
#define __TEMPMON_TASK_STACK_SIZE   4096

/*******************************************************************************
    types
*******************************************************************************/

/** \brief AWBus tempmon callback funcs */
typedef void (*aw_pfunc_tempmon_callback_t) (void *, uint8_t);

/** \brief imx6ul tempmon initialization structure for plb based AWBus devices */
typedef struct awbl_imx6ul_tempmon_devinfo {

    uint32_t    regbase;
    uint8_t     inum;
    int32_t     low_alarm_temp;     /* low limit (1000 times -30000 ~ 115000) */
    int32_t     high_alarm_temp;    /* high limit (1000 times -30000 ~ 115000) */
    int32_t     panic_temp;         /* reset limit (1000 times -30000 ~ 115000) */
    uint32_t    task_prio;

    void(*pfn_plfm_init)(void);

} awbl_imx6ul_tempmon_dev_info_t;

/** \brief imx6ul tempmon device structure for awbus lite */
typedef struct awbl_imx6ul_tempmon_dev {

    /** \brief device */
    struct awbl_dev  dev;

    /** \brief  temp sensor serv */
    struct awbl_temp_service  temp_serv;

    /** \brief callback func */
    aw_pfunc_tempmon_callback_t pfn_tempmon_callback;

    /** \brief callback p_arg */
    void *p_arg;

    /** \brief tempmon callback func exist */
    int func_exist;

    /** \brief device mutex */
    AW_MUTEX_DECL(mutex);

    /** \brief task define */
    AW_TASK_DECL(tempmon_task_decl, __TEMPMON_TASK_STACK_SIZE);

    /** \brief task signal */
    AW_SEMB_DECL(tempmon_sem_sync);

} awbl_imx6ul_tempmon_dev_t;

/*
 * \name tempmon interruput state
 * \anchor grp_tempmon_int_state
 * @{
 */

#define AWBL_IMX6UL_TEMPMON_STATE_TEMP_LOW          0x01
#define AWBL_IMX6UL_TEMPMON_STATE_TEMP_HIGH         0x02
#define AWBL_IMX6UL_TEMPMON_STATE_TEMP_PANIC        0x04

/** @} */

/**
 * \brief tempmon limit flags
 * \anchor grp_tempmon_limit_flags
 * @{
 */

#define AWBL_IMX6UL_TEMPMON_LIMIT_TEMP_LOW         0x01
#define AWBL_IMX6UL_TEMPMON_LIMIT_TEMP_HIGH        0x02
#define AWBL_IMX6UL_TEMPMON_LIMIT_TEMP_PANIC       0x04

/** @} */

/**
 * \brief AWBus imx6ul tempmon set limit temp
 *
 * \param[in] flags      : point to limit flags #AWBL_IMX6UL_TEMPMON_LIMIT_TEMP_LOW
 *                         see in grp_tempmon_limit_flags
 * \param[in] limit_temp : limit temp (1000 times of ture temp)
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid param
 * \retval -AW_EPERM  : operation is not allowed(isr has been reg once)
 * 
 * \note panic temp is reset limit(SRC WARM default is enable), when the temp is
 *       increase above panic temp, well mcu will reset. 
 */
aw_err_t awbl_imx6ul_tempmon_limit_temp_set (uint8_t flags, int limit_temp);

/**
 * \brief AWBus imx6ul tempmon get limit temp
 *
 * \param[in] flags        : point to limit flags #AWBL_IMX6UL_TEMPMON_LIMIT_TEMP_LOW
 *                           see in grp_tempmon_limit_flags
 * \param[in] p_limit_temp : point to limit temp buf (1000 times of ture temp)
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid param
 * \retval -AW_EPERM  : operation is not allowed(isr has been reg once)
 */
aw_err_t awbl_imx6ul_tempmon_limit_temp_get (uint8_t flags, int *p_limit_temp);

/**
 * \brief AWBus imx6ul tempmon int enable
 * \param[in] void
 * \retval AW_OK  : success
 * \retval -ENXIO : inum doesn't exist
 */
aw_err_t awbl_imx6ul_tempmon_int_enable (void);

/**
 * \brief AWBus imx6ul tempmon int disable
 * \param[in] void
 * \retval AW_OK  : success
 * \retval -ENXIO : inum doesn't exist
 */
aw_err_t awbl_imx6ul_tempmon_int_disable (void);

/**
 * \brief AWBus imx6ul tempmon int connect
 *
 * \param[in] pfn_callback : point to callback func. arg 1 is user's arg,
 *                           arg 2 is temp int state, see in grp_tempmon_int_state
 * \param[in] p_arg        : point to arg
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid param
 * \retval -AW_EPERM  : pfunc has already existed
 */
aw_err_t awbl_imx6ul_tempmon_int_connect (aw_pfunc_tempmon_callback_t pfn_callback, void *p_arg);

/**
 * \brief AWBus imx6ul tempmon int disconnect
 *
 * \param[in] void
 *
 * \retval AW_OK      : success
 * \retval -AW_EINVAL : invalid param
 */
aw_err_t awbl_imx6ul_tempmon_int_disconnect (void);

/**
 * \brief imx6ul tempmon driver register
 */
void awbl_imx6ul_tempmon_drv_register (void);

#endif /* __AWBL_IMX6UL_TEMPMON_H */
