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
 * \brief imx6ul SDMA ����Դ�ļ�
 *
 *    ע��: ͨ��0 �Ѿ����ڲ�ռ��, SDMA��ʵ���첽ģʽ��δʵ��ѭ��ģʽ
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  lnk, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include <string.h>

#include "aw_assert.h"
#include "driver/dma/awbl_imx6ul_sdma_service.h"

static const struct awbl_imx6ul_sdma_service     *__g_p_sdma_serv = NULL;

/******************************************************************************
 implementation
 ******************************************************************************/

/**
 * \brief allocate imx6ul sdma service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __imx6ul_sdma_serv_alloc_helper (
        struct awbl_dev *p_dev,
        void *p_arg)
{
    if( NULL != __g_p_sdma_serv ) {
        return AW_OK;
    }

    if(strcmp(AWBL_IMX6UL_SDMA_NAME,p_dev->p_driver->p_drvname) == 0) {
        awbl_method_handler_t   pfn_imx6ul_sdma_get_serv = NULL;
        pfn_imx6ul_sdma_get_serv = awbl_dev_method_get(p_dev,
                AWBL_METHOD_CALL(awbl_imx6ul_sdma_serv_func_get));
        if(NULL != pfn_imx6ul_sdma_get_serv) {
            pfn_imx6ul_sdma_get_serv(p_dev,&__g_p_sdma_serv);
        }
    }
    return AW_OK;
}


/**
 * \brief allocate adc services from instance tree
 */
static int __imx6ul_sdma_serv_alloc (void)
{
    /* todo: count the number of service */

    awbl_dev_iterate(__imx6ul_sdma_serv_alloc_helper,
            NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

void * imx6ul_sdma_alloc_chan(struct imx_dma_data *data)
{
    void               *p = NULL;

    if(NULL != __g_p_sdma_serv) {
        p = __g_p_sdma_serv->p_serv_func->pfn_alloc_chan(
                __g_p_sdma_serv->p_dev,data);
    }

    return p;
}

int imx6ul_sdma_control(void *chan, enum dma_ctrl_cmd cmd,unsigned long arg)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_control(
                chan,cmd,arg);
    }

    return ret;
}

int imx6ul_sdma_prep_memcpy(void *chan,
        dma_addr_t dma_dst,
        dma_addr_t dma_src,
        size_t len,
        unsigned long flags)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_prep_memcpy(
                chan,
                dma_dst,
                dma_src,
                len,
                flags
                );
    }

    return ret;
}

int imx6ul_sdma_prep_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_preq_sg(
                chan,
                dst_sg,
                dst_count,
                src_sg,
                src_count,
                direction);
    }

    return ret;
}

int imx6ul_sdma_prep_memcpy_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        unsigned long flags)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_preq_memcpy_sg(
                chan,
                dst_sg,
                dst_count,
                src_sg,
                src_count,
                flags
                );
    }

    return ret;
}

int imx6ul_sdma_prep_slave_sg(void *chan,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_preq_slave_sg(
                chan,
                src_sg,
                src_count,
                direction
                );
    }

    return ret;
}

int imx6ul_sdma_prep_dma_cyclic(void *chan,
        dma_addr_t dma_addr, size_t buf_len,
        size_t period_len, enum dma_transfer_direction direction)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_preq_dma_cyclic(
                chan,
                dma_addr,
                buf_len,
                period_len,
                direction
                );
    }

    return ret;
}

int imx6ul_sdma_chan_start_sync(void *chan,uint32_t timeout)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_chan_start_sync(
                chan,timeout );
    }
    return ret;
}

int imx6ul_sdma_chan_start_async(void *chan,
        pfn_dma_callback_t callback,void *param)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_chan_start_async(
                chan,callback,param );
    }
    return ret;
}

int imx6ul_sdma_get_transfer_count(void *chan)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_get_transfer_count(
                chan );
    }
    return ret;
}

int imx6ul_sdma_chan_stop(void *chan)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_chan_stop(chan);
    }
    return ret;
}

int imx6ul_sdma_free_chan(void *chan)
{
    int         ret = -AW_ENODEV;

    if(NULL != __g_p_sdma_serv) {
        ret = __g_p_sdma_serv->p_serv_func->pfn_chan_free(chan);
    }
    return ret;
}


/**
 * \brief adc initial
 */
void awbl_imx6ul_sdma_init (void)
{
    /* allocate adc services from instance tree */
    __imx6ul_sdma_serv_alloc();
}

/** \brief AWBus method, used to get a AWBus imx6ul SDMA service
 *
 *    a driver can implement the method 'awbl_imx6ul_sdma_serv_func_get'
 * to provide a AWBus ADC service. The method hander should return
 * a pointer of a valid AWbus ADC service instance form the argument 'p_arg'.
 *
 * \par ����
 * \code
 * aw_err_t awbl_method_handler_adc_service_get (struct awbl_dev *p_dev, void *p_arg)
 * {
 *      struct awbl_adc_service *p_serv;
 *
 *      // get adc service instance memory
 *      p_serv = __get_adc_serv(p_dev);
 *
 *      // initialize the adc service instance
 *      p_serv->p_servinfo  = __get_adc_serv(p_dev);
 *      p_serv->p_servfuncs = &__g_adc_servfuncs;
 *      p_serv->p_inst      = (void *)p_dev;
 *
 *      // send back the service instance
 *      *(struct awbl_adc_service **)p_arg = p_serv;
 *      return AW_OK
 * }
 * \endcode
 */
AWBL_METHOD_DEF(awbl_imx6ul_sdma_serv_func_get, "awbl_imx6ul_sdma_serv_func_get");
