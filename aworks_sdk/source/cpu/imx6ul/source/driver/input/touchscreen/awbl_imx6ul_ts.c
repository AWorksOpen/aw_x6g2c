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
 * \brief TI imx6ul ����������������Դ�ļ�
 *            
 *    imx6ul ��TSC����������ʹ��ADCʵ�֣�4��ģʽ��16����ƽ����12bits
 *
 * \internal
 * \par modification history:
 * - 1.01 16-06-15  sky, modified.
 * - 1.00 16-03-22  lnk, first implemetation.
 * \endinternal
 */
 
#include "aworks.h" 
#include "aw_adc.h"
#include "aw_delay.h"
#include "aw_ts.h"
#include "aw_input.h"
#include "aw_vdebug.h"
#include "aw_ts_lib.h"
#include <string.h>
#include "aw_clk.h"
#include "driver/input/touchscreen/awbl_imx6ul_ts.h"

/*******************************************************************************
 local defines
*******************************************************************************/

/**
 * \brief  imx6ul TSC �Ĵ����鶨��
 */
typedef struct __imx6ul_tsc_regs {
    uint32_t basic_setting;
    uint32_t reserved0[3];
    uint32_t input_buffer_addr;
    uint32_t reserved1[3];
    uint32_t flow_ctrl;
    uint32_t reserved2[3];
    uint32_t measeure_value;
    uint32_t reserved3[3];
    uint32_t int_en;
    uint32_t reserved4[3];
    uint32_t int_sig_en;
    uint32_t reserved5[3];
    uint32_t int_status;
    uint32_t reserved6[3];
    uint32_t debug_mode;
    uint32_t reserved7[3];
    uint32_t debug_mode2;
} __imx6ul_tsc_regs_t;

/**
 * \brief  TSC �Ĵ���λ����
 * @{
 */

/** \brief �����λ */
#define __TSC_FLOW_CTRL_RST      (1ul << 0)

/** \brief TSC ����ģʽ, �����ֶ�����SW */
#define __TSC_DEBUG_MODE_EN_SHIF     28
#define __TSC_DEBUG_MODE_EN      (1ul << 28)

/** \brief XPUL ���� */
#define __TSC_DEBUG_XPUL_UP      (1ul << 1)

/** \brief XNUR ���� */
#define __TSC_DEBUG_XNUR_DOWN    (1ul << 3)

/** \brief YPLL ���� */
#define __TSC_DEBUG_YPLL_UP      (1ul << 7)

/** \brief YNLR ���� */
#define __TSC_DEBUG_YNLR_DOWN    (1ul << 9)

/**
 * @}
 */

#define  __DEV_TO_IMX6UL_TS_DEV(p_ts_dev, p_dev) \
    awbl_imx6ul_ts_dev_t  *p_ts_dev = \
    (awbl_imx6ul_ts_dev_t *)(AW_CONTAINER_OF(p_dev, struct awbl_imx6ul_ts_dev, adev))

#define  __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv) \
    awbl_imx6ul_ts_dev_t  *p_ts_dev = \
    (awbl_imx6ul_ts_dev_t *)(AW_CONTAINER_OF(p_serv, struct awbl_imx6ul_ts_dev, ts_serv))

/** \brief Macro that covert struct awbl_dev to awbl_imx6ul_ts_devinfo */
#define __DEV_GET_TS_INFO(p_devinfo, p_dev) \
    awbl_imx6ul_ts_devinfo_t *p_devinfo = (awbl_imx6ul_ts_devinfo_t *)AWBL_DEVINFO_GET(p_dev)
/*******************************************************************************
forward define
*******************************************************************************/
aw_local void __imx6ul_ts_inst_init1 (struct awbl_dev *p_dev);
aw_local void __imx6ul_ts_inst_init2 (struct awbl_dev *p_dev);

aw_local void __imx6ul_ts_active_x(struct awbl_ts_service *p_serv);
aw_local void __imx6ul_ts_active_y(struct awbl_ts_service *p_serv);

aw_local void __imx6ul_ts_measure_x(struct awbl_ts_service *p_serv,
                                    void               *p_sample[],
                                    int                     max_slots,
                                    int                     count);
                                    
aw_local void __imx6ul_ts_measure_y(struct awbl_ts_service *p_serv,
                                    void               *p_sample[],
                                    int                     max_slots,
                                    int                     count);
aw_local void __imx6ul_ts_lock(struct awbl_ts_service *p_serv, aw_bool_t lock);

aw_local int __imx6ul_ts_xy_get (struct awbl_ts_service *p_serv,
                                  void               *p_x_sample[],
                                  void               *p_y_sample[],
                                  int                     max_slots,
                                  int                     count);
aw_local int __imx6ul_ts_is_touch (struct awbl_ts_service *p_serv);
/** 
 * \brief  driver functions (must defined as aw_const) 
 */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_imx6ul_ts = {
    __imx6ul_ts_inst_init1,      /* pfunc_dev_init1 */
    __imx6ul_ts_inst_init2,      /* pfunc_dev_init2 */
    NULL                         /* pfunc_dev_connect */
};

/**
 * \brief  awbl_drvinfo for driver registration (must defined as aw_const) 
 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_imx6ul_ts = {

    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    AWBL_IMX6UL_TOUCHSCREEN_NAME,   /* p_drvname */
    &__g_awbl_drvfuncs_imx6ul_ts,   /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};
/*******************************************************************************
implementation
*******************************************************************************/

/** 
 * \brief  TS ��1�׶γ�ʼ��
 */
aw_local void __imx6ul_ts_inst_init1 (struct awbl_dev *p_dev)
{

    __DEV_GET_TS_INFO(p_devinfo, p_dev);

    /* platform initialize */
    if (p_devinfo->pfunc_plfm_init) {
        p_devinfo->pfunc_plfm_init();
    }
}

/** 
 * \brief  TS ��2�׶γ�ʼ��
 */
aw_local void __imx6ul_ts_inst_init2 (struct awbl_dev *p_dev)
{

        __DEV_TO_IMX6UL_TS_DEV(p_ts_dev, p_dev);
        __DEV_GET_TS_INFO(p_devinfo, p_dev);
        __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);
        uint32_t reg = 0;

        /* ��ʼ��һ��touch screen���� */
        awbl_ts_service_init(&p_ts_dev->ts_serv, &p_devinfo->ts_serv_info);

        p_ts_dev->ts_serv.drv_fun.pfn_lock_set      =  __imx6ul_ts_lock;
        p_ts_dev->ts_serv.drv_fun.pfn_active_x   =  __imx6ul_ts_active_x;
        p_ts_dev->ts_serv.drv_fun.pfn_active_y  =  __imx6ul_ts_active_y;
        p_ts_dev->ts_serv.drv_fun.pfn_measure_x =  __imx6ul_ts_measure_x;
        p_ts_dev->ts_serv.drv_fun.pfn_measure_y =  __imx6ul_ts_measure_y;
        p_ts_dev->ts_serv.drv_fun.pfn_is_touch  = __imx6ul_ts_is_touch;
        p_ts_dev->ts_serv.drv_fun.pfn_xy_get    = __imx6ul_ts_xy_get;

        /* �õ����ǲ���ֵ�� ��������ֵ����ҪУ׼   */
        p_ts_dev->ts_serv.ts_cal_att   = 1;

        /* ע��TS�豸 */
        awbl_ts_service_register(&p_ts_dev->ts_serv);

        /* ��λ TSC */
        reg = readl(&p_reg->flow_ctrl);
        reg |= __TSC_FLOW_CTRL_RST;
        writel(reg, &p_reg->flow_ctrl);

        while (readl(&p_reg->flow_ctrl) & __TSC_FLOW_CTRL_RST);

        /* DEBUGģʽʹ�� */
//        AW_REG_BIT_SET32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);
        /* ���ó��ʱ�� */
        writel(0xFF, &p_reg->input_buffer_addr);
        /* �����ж�ʹ�� */
        writel((0x1 << 4), &p_reg->int_en);
        /* �����ź�ʹ�� */
        writel((0x1 << 4), &p_reg->int_sig_en);
        /* ��ʼ���� */
        writel((0x1 << 12), &p_reg->flow_ctrl);
        /* ��������ж�״̬��־λ */
        writel((0x1 << 4), &p_reg->int_status);
}

/** 
 * \brief  ׼��X���ѹֵ����
 *         (1) ��ͨX�Ĳ�����ѹ, ׼����X����.
 *         (2) �ж�X�ĵ�ѹ,���ö�X�Ĳ���.
 */
aw_local void __imx6ul_ts_active_x(struct awbl_ts_service *p_serv)
{
    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);

    /* XPUL up, XNUR down*/
    writel(__TSC_DEBUG_XPUL_UP | __TSC_DEBUG_XNUR_DOWN, &p_reg->debug_mode2);
}


/** 
 * \brief  ׼��Y���ѹֵ����
 *         (1) ��ͨY�Ĳ�����ѹ, ׼����Y����.
 *         (2) �ж�Y�ĵ�ѹ,���ö�Y�Ĳ���.
 */
aw_local void __imx6ul_ts_active_y(struct awbl_ts_service *p_serv)
{
    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);

    /* YPLL up, YNLR down */
    writel(__TSC_DEBUG_YPLL_UP | __TSC_DEBUG_YNLR_DOWN, &p_reg->debug_mode2);
}

/**
 * \brief  ����X��ĵ�ѹֵ
 */
aw_local void __imx6ul_ts_measure_x(struct awbl_ts_service *p_serv,
                                    void                   *p_sample[],
                                    int                     max_slots,
                                    int                     count)
{

    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);
    
    aw_task_id_t            task_id;
    unsigned int            priority;

    /* Debugģʽʹ�� */
    AW_REG_BIT_SET32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);

    task_id = aw_task_id_self();
    priority = aw_task_priority_get(task_id);

    aw_task_priority_set(task_id,AW_TASK_SYS_PRIORITY(5) );
    __imx6ul_ts_active_y(p_serv);    /* Y�ṩ�� */
    aw_mdelay(2);                   /* �ȴ������ȶ� */
    aw_adc_rate_set(p_devinfo->x_minus_ch, 500000);
    aw_adc_sync_read(p_devinfo->x_minus_ch, p_sample[0], count, AW_FALSE);
    aw_task_priority_set(task_id,priority);


    /* DEBUGģʽ���� */
    AW_REG_BIT_CLR32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);
}

/**
 * \brief  ����Y��ĵ�ѹֵ
 */
aw_local void __imx6ul_ts_measure_y(struct awbl_ts_service *p_serv,
                                    void               *p_sample[],
                                    int                     max_slots,
                                    int                     count)
{

    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);
    aw_task_id_t            task_id;
    unsigned int            priority;
    
    /* Debugģʽʹ�� */
    AW_REG_BIT_SET32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);

    task_id = aw_task_id_self();
    priority = aw_task_priority_get(task_id);

    aw_task_priority_set(task_id,AW_TASK_SYS_PRIORITY(5) );
    __imx6ul_ts_active_x(p_serv);    /* X�ṩ�� */
    aw_mdelay(2);                   /* �ȴ������ȶ� */
    aw_adc_rate_set(p_devinfo->y_minus_ch, 500000);
    aw_adc_sync_read(p_devinfo->y_minus_ch, p_sample[0], count, AW_FALSE);
    aw_task_priority_set(task_id,priority);
    /* DEBUGģʽ���� */
    AW_REG_BIT_CLR32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);
}

/**
 * \brief  TS ����
 */
aw_local void __imx6ul_ts_lock(struct awbl_ts_service *p_serv, aw_bool_t lock)
{

    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t *p_reg = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);

    if (AW_TRUE == lock) {
        //AW_INFOF(("lock screen. \n"));

        /* DEBUGģʽ���� */
        AW_REG_BIT_CLR32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);
    } else {
        //AW_INFOF(("unlock screen. \n"));
        
        /* DEBUGģʽʹ�� */
        AW_REG_BIT_SET32(&p_reg->debug_mode, __TSC_DEBUG_MODE_EN_SHIF);
    }
}

/**
 * \brief �ж��Ƿ���Ļ���� 0: δ����  1: ����
 */
static int  __get_touch_status(struct awbl_ts_service *p_serv)
{
	__TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    __DEV_GET_TS_INFO(p_devinfo, &p_ts_dev->adev);
    __imx6ul_tsc_regs_t  *p_hw_tsc = (__imx6ul_tsc_regs_t *)(p_devinfo->regbase);
    int is_pressed = 0;
    /* ����ж�״̬��־λ */
    writel((0x1 << 4), &p_hw_tsc->int_status);
    /* ��λTSC */
    writel(0x1, &p_hw_tsc->flow_ctrl);
    /* ��ʼ���� */
    writel((0x1 << 12), &p_hw_tsc->flow_ctrl);
    /* �ȴ������� */
    aw_mdelay(1);
    /* ���Detect Signalλ��λ����˵����������Ļ */
    if (p_hw_tsc->int_status & (1 << 4)) {
        is_pressed = 1;
    }
    return is_pressed;
}

/**
 * \brief ��ֵ�˲�
 */
static int __ts_filter_threshold(uint16_t *sample, int num, int threshold)
{
    int i = 0, j = 0, min;
    uint16_t tmp = 0;
    for (i = 0; i < num - 1; i++) {
        min = i;
        for (j = i + 1; j < num; j++) {
            if (sample[i] > sample[j]) {
                min = j;
            }
        }
        /* ��С�������� */
        tmp = sample[i];
        sample[i] = sample[min];
        sample[min] = tmp;
    }

    if (num == 1) {
        return AW_OK;
    }

    /* �м�����������ֵ�Ƿ�ӽ�,������Ϊ�Ƕ���  */
    if(abs((sample[num / 2] - sample[num / 2 + 1])) > threshold) {
        return -AW_ENOTSUP;
    } else {
        return AW_OK;
    }
}

/* ��ȡX Y �����꣬���ҷ��ذ��µĵ��� */
aw_local int __imx6ul_ts_xy_get (struct awbl_ts_service *p_serv,
                                  void               *p_x_sample[],
                                  void               *p_y_sample[],
                                  int                     max_slots,
                                  int                     count)
{
    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    int i,ret;
    static uint8_t is_debounce = 0;

    aw_bool_t is_pressed = AW_TRUE;
    uint16_t **p_adc_xsample  = (uint16_t **)p_x_sample;
    uint16_t **p_adc_ysample  = (uint16_t **)p_y_sample;

    /* ���жϴ������Ƿ��� */
    if (__get_touch_status(p_serv)) {
        if (is_debounce == 0) {
            /* ��һ�θհ���ʱ��ʱ���� */
            aw_mdelay(p_serv->p_service_info->debounce_ms);
            is_debounce = 1;
        }
        /* ���ǰ��� */
        if (__get_touch_status(p_serv)) {
            /* ���´����������� */
        	__imx6ul_ts_measure_x(p_serv, p_x_sample, max_slots, count);
            /* �˲����м����βɵ������ݻ��ǹ���ģ�������δ��� */
            ret = __ts_filter_threshold(p_adc_xsample[0], count, p_serv->p_service_info->sample_threshold);
            if (ret != AW_OK) {
                aw_mdelay(p_serv->p_service_info->debounce_ms);
                is_pressed = AW_FALSE;
                is_debounce = 0;
                return 0;
            }
            __imx6ul_ts_measure_y(p_serv, p_y_sample, max_slots, count);
            ret = __ts_filter_threshold(p_adc_ysample[0], count, p_serv->p_service_info->sample_threshold);
            if (ret != AW_OK) {
                aw_mdelay(p_serv->p_service_info->debounce_ms);
                is_pressed = AW_FALSE;
                is_debounce = 0;
                return 0;
            }

            /* ������ɻ��ǰ��� */
            if (__get_touch_status(p_serv)) {
                is_pressed = AW_TRUE;
            } else {
                aw_mdelay(p_serv->p_service_info->debounce_ms);
                is_pressed = AW_FALSE;
                is_debounce = 0;
                return 0;
            }

        } else {
            aw_mdelay(p_serv->p_service_info->debounce_ms);
            is_pressed = AW_FALSE;
            is_debounce = 0;
        }
    } else {  /* �ɿ� */
        /* ��һ�θհ���ʱ��ʱ���� */
        if (is_debounce == 1) {
           aw_mdelay(p_serv->p_service_info->debounce_ms);
           is_debounce = 0;
        }
        is_pressed = AW_FALSE;
    }

    for (i = 0; i < count; i++) {
        if (p_adc_xsample[0][i] <= 200 || p_adc_xsample[0][i] >=3800 ||
                p_adc_ysample[0][i] >= 3800 || p_adc_ysample[0][i] <= 200){
            is_pressed = AW_FALSE;
        }
    }



    p_ts_dev->is_pressed = is_pressed;

    return is_pressed ? 1 : 0;
}


/* ��ȡ�������Ƿ��� */
aw_local int __imx6ul_ts_is_touch (struct awbl_ts_service *p_serv)
{
    __TS_SERV_TO_IMX6UL_TS_DEV(p_ts_dev, p_serv);
    return p_ts_dev->is_pressed;
}
/******************************************************************************/

/**
 * \brief  imx6ul TS ����ע��
 */
void awbl_imx6ul_touch_screen_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_imx6ul_ts);
}
 
/* end of file */
