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
 * \brief imx6ul adc driver source file
 *
 *  12bits 采样位宽, 禁能硬件平均，使用IPG CLK
 *
 * \internal
 * \par Modification History
 * - 1.01 16-05-30  sky, modified.
 * - 1.00 16-03-04  lnk, first implementation.
 * \endinternal
 */

#include "aw_common.h"
#include "aw_int.h"
#include "aw_adc.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_adc.h"
#include "aw_vdebug.h"
#include "aw_clk.h"
#include "driver/adc/awbl_imx6ul_adc.h"

#include "aw_gpio.h"
#include "imx6ul_gpio_pin.h"
#include "driver/gpio/awbl_imx6ul_gpio_private.h"

/*******************************************************************************
  local defines
*******************************************************************************/

/**
 * \brief imx6ul ADC 寄存器块定义
 */
typedef struct imx6ul_adc_regs {
    uint32_t hc0;               /**< \brief 硬件/软件触发控制寄存器0 */
    uint32_t hc1;               /**< \brief 硬件触发控制寄存器1 */
    uint32_t hs;                /**< \brief 硬件触发状态寄存器 */
    uint32_t r0;                /**< \brief 硬件触发数值结果寄存器0 */
    uint32_t r1;                /**< \brief 硬件触发数据结果寄存器1 */
    uint32_t cfg;               /**< \brief 配置寄存器 */
    uint32_t gc;                /**< \brief 通用控制寄存器 */
    uint32_t gs;                /**< \brief 通用状态寄存器 */
    uint32_t cv;                /**< \brief 比较数值寄存器 */
    uint32_t ofs;               /**< \brief 偏移数值寄存器 */
    uint32_t cal;               /**< \brief 校准数值寄存器 */
} imx6ul_adc_regs_t;

/** 
 * \brief ADC 寄存器位域定义
 * @{
 */

/** \brief 转换完成中断使能 */
#define __ADC_HC0_AIEN         (1u << 7)

/** \brief 输入通道选择掩码 */
#define __ADC_HC0_ADCH_MASK    (0x0000001F)

/** \brief 转换完成标志 */
#define __ADC_HS_COCO0         (1u << 0)

/** \brief 输入时钟源掩码 */
#define __ADC_CFG_ADICLK_MASK  (3u << 0)

/** \brief 输入时钟源位移 */
#define __ADC_CFG_ADICLK_SHIFT (0u)

/** \brief Low Power Config */
#define __ADC_CFG_LPC          (1u << 7)

/** \brief 12bits MODE */
#define __ADC_CFG_MODE_12_BIT  (2u << 2)

/** \brief 12bits MODE */
#define __ADC_CFG_MODE_10_BIT  (1u << 2)

/** \brief 12bits MODE */
#define __ADC_CFG_MODE_8_BIT   (0u << 2)

/** \brief 时钟分频掩码 */
#define __ADC_CFG_ADIV_MASK    (3u << 5)

/** \brief 时钟分频掩码 */
#define __ADC_CFG_ADIV_SHIFT   (5)

/** \brief DMA使能 */
#define __ADC_GC_DMAEN         (1u << 1)

/** \brief 硬件平均使能 */
#define __ADC_GC_AVGE          (1u << 5)

/** \brief ADC转换时间周期 */
#define __ADC_CT_RATE_8BIT      25
#define __ADC_CT_RATE_10BIT     29
#define __ADC_CT_RATE_12BIT     33

/** \brief ADC测试通道选择 */
#define __ADC_TEST_CHAN         0x19

/** \brief ADC温度通道选择 */
#define __ADC_TEMP_CHAN         0x10

/** @} */

/** \brief 从P_DEV获得ADC_DEV宏操作 */
#define __DEV_GET_ADC_DEV(p_dev) \
    (awbl_imx6ul_adc_dev_t *)(p_dev)

/** \brief 从P_DEV获得ADC_INFO宏操作 */
#define __DEV_GET_ADC_INFO(p_dev) \
    (awbl_imx6ul_adc_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx6ul_adc_inst_init2 (awbl_dev_t *p_dev);

aw_local aw_err_t __imx6ul_adc_serv_get (struct awbl_dev *p_dev, void *p_arg);

aw_local void     __imx6ul_adc_isr (void *p_cookie);

aw_local aw_err_t __imx6ul_adc_start (void                 *p_cookie,
                                      aw_adc_channel_t      channel,
                                      aw_adc_buf_desc_t    *p_desc,
                                      int                   desc_num,
                                      uint32_t              count,
                                      pfn_adc_complete_t    pfn_seq_complete,
                                      void                 *p_arg);
                                 
aw_local aw_err_t __imx6ul_adc_stop (void                  *p_cookie,
                                     aw_adc_channel_t       channel);

aw_local aw_err_t __imx6ul_adc_bits_get (void *p_cookie, aw_adc_channel_t channel);
aw_local aw_err_t __imx6ul_adc_vref_get (void *p_cookie, aw_adc_channel_t channel);
                                
aw_local aw_err_t __imx6ul_adc_rate_get (void               *p_cookie,
                                         aw_adc_channel_t    channel,
                                         uint32_t           *p_rate);

aw_local aw_err_t __imx6ul_adc_rate_set (void               *p_cookie,
                                         aw_adc_channel_t    channel,
                                         uint32_t            rate);
                               
/*******************************************************************************
  loccals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_imx6ul_adc_drvfuncs = {
    NULL,
    __imx6ul_adc_inst_init2,
    NULL
};

/** \brief adc service functions (must defined as aw_const) */
aw_local aw_const struct awbl_adc_servfuncs __g_imx6ul_adc_servfuncs = {
    __imx6ul_adc_start,
    __imx6ul_adc_stop,
    __imx6ul_adc_bits_get,
    __imx6ul_adc_vref_get,
    __imx6ul_adc_rate_get,
    __imx6ul_adc_rate_set
};

/** \brief driver methods (must defined as aw_const) */
aw_local aw_const struct awbl_dev_method __g_imx6ul_adc_dev_methods[] = {
    AWBL_METHOD(awbl_adcserv_get, __imx6ul_adc_serv_get),
    AWBL_METHOD_END
};
/******************************************************************************/

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_imx6ul_adc_drv_registration = {
    {
        AWBL_VER_1,                        /** \brief awb_ver */
        AWBL_BUSID_PLB,                    /** \brief bus_id */
        IMX6UL_ADC_NAME,                   /** \brief p_drvname */
        &__g_imx6ul_adc_drvfuncs,          /** \brief p_busfuncs */
        &__g_imx6ul_adc_dev_methods[0],    /** \brief p_methods */
        NULL                               /** \brief pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief ADC 第2阶段初始化
 */
aw_local void __imx6ul_adc_inst_init2 (awbl_dev_t *p_dev)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_dev);
    awbl_imx6ul_adc_dev_t     *p_this    = __DEV_GET_ADC_DEV(p_dev);
    imx6ul_adc_regs_t         *p_reg     = (imx6ul_adc_regs_t *)(p_devinfo->regbase);
    int                        i;

    /* 使能ADC时钟 */
    for (i = 0; i< p_devinfo->dev_clk_count;i++) {
        aw_clk_enable(p_devinfo->dev_clks[i]);
    }

    /* 平台相关初始化  */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    p_this->bufdesc_num      = 0;
    p_this->cur_bufdesc_id   = 0;
    p_this->phy_chan         = 0;
    p_this->is_stopped       = AW_TRUE;      /* 未启动，处于停止态*/
    p_this->pfn_seq_complete = NULL;
    p_this->p_arg            = NULL;
    p_this->p_bufdesc        = NULL;
    p_this->remain_count     = 0;
    p_this->samples          = 0;

    /* 清空CFG寄存器 */
    writel(0, &p_reg->cfg);

    /* 清空GC寄存器 */
    writel(0, &p_reg->gc);

    /* 打开低功耗模式 */
    AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_LPC);
    AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_LPC);

    /* 预设输入时钟分频为8分频 */
    AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_ADIV_MASK);

    /*
     * 设置采样位数
     */
    if (p_devinfo->bits <= 8) {
        AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_MODE_8_BIT);
    } else if (p_devinfo->bits <= 10) {
        AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_MODE_10_BIT);
    } else {
        AW_REG_BIT_SET_MASK32(&p_reg->cfg, __ADC_CFG_MODE_12_BIT);
    }

    /* 连接并使能中断 */
    aw_int_connect(p_devinfo->inum, __imx6ul_adc_isr, p_dev);
    aw_int_enable(p_devinfo->inum);
}

/**
 * \brief "awbl_adcserv_get" 方法实现
 */
aw_local aw_err_t __imx6ul_adc_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_imx6ul_adc_devinfo *p_devinfo = NULL;
    struct awbl_adc_service        *p_serv    = NULL;

    /* get adc service instance */
    p_serv = &((awbl_imx6ul_adc_dev_t *)p_dev)->adc_serv;

    /* get adc device information */
    p_devinfo = __DEV_GET_ADC_INFO(p_dev);

    /* initialize the adc service instance */
    p_serv->p_servinfo  = &p_devinfo->adc_servinfo;
    p_serv->p_servfuncs = &__g_imx6ul_adc_servfuncs;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_adc_service **)p_arg = p_serv;

    return AW_OK;
}

/** 
 * \brief ADC 启动通道转换
 */
aw_local void __imx6ul_adc_start_convert (uint32_t adc_regbase, uint8_t phy_ch)
{
    imx6ul_adc_regs_t *p_reg = (imx6ul_adc_regs_t *)(adc_regbase);

    /* 修改测量通道并使能转换完成中断 */
    writel((phy_ch & __ADC_HC0_ADCH_MASK) | __ADC_HC0_AIEN, &p_reg->hc0);
}

/**
 * \brief ADC 转换结果数据填充
 */
aw_local void __imx6ul_adc_data_fill (struct awbl_imx6ul_adc_dev *p_this,
                                      uint16_t                    data)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_this);
    aw_adc_buf_desc_t         *p_desc    = &(p_this->p_bufdesc[p_this->cur_bufdesc_id]);

    aw_bool_t    is_continue = AW_FALSE;
    uint16_t *p_buf       = (uint16_t *)p_desc->p_buf;

    p_buf[p_this->samples++] = data;

    /* 当前缓冲区转换完毕 */
    if (p_this->samples == p_desc->length) {
        pfn_adc_complete_t pfn_tmp = p_desc->pfn_complete;

        if (pfn_tmp) {
            pfn_tmp(p_desc->p_arg, AW_OK);
        }

        /* 序列中最后一个缓冲区,序列正常结束 */
        if (p_this->cur_bufdesc_id == p_this->bufdesc_num - 1) {

            if (p_this->remain_count == 0) {     /* 持续转换 */
                is_continue = AW_TRUE;
            } else {
                if (p_this->remain_count == 1) { /* 最后一个的count=1,转换完毕 */
                    is_continue = AW_FALSE;
                } else {
                    p_this->remain_count--;
                    is_continue = AW_TRUE;
                }
            }

            /* 序列转换完，send msg OK*/
            if (p_this->pfn_seq_complete) {
                p_this->pfn_seq_complete(p_this->p_arg, AW_OK);
            }

        } else {
            is_continue = AW_TRUE;
        }

        /* 当前描述缓冲区传输完成，进行下一个描述缓冲区的传输 */
        p_this->samples = 0;
        p_this->cur_bufdesc_id++;
        if (p_this->cur_bufdesc_id == p_this->bufdesc_num) {
            p_this->cur_bufdesc_id = 0;
        }

    } else {
        is_continue = AW_TRUE;
    }

    /* 继续转换 */
    if (is_continue == AW_TRUE) {
        __imx6ul_adc_start_convert(p_devinfo->regbase, p_this->phy_chan);
    }
}

/**
 * \brief ADC 转换完成中断服务
 */
aw_local void __imx6ul_adc_isr (void *p_cookie)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);
    awbl_imx6ul_adc_dev_t     *p_this    = __DEV_GET_ADC_DEV(p_cookie);
    imx6ul_adc_regs_t         *p_reg     = (imx6ul_adc_regs_t *)(p_devinfo->regbase);

    uint32_t data = 0;

    /* 转换已被停止 */
    if (p_this->is_stopped == AW_TRUE) {
        return;
    }

    /* 参数无效 */
    if (p_this->bufdesc_num == 0 || p_this->p_bufdesc == NULL) {
        return;
    }

    /* 转换完成标志 */
    if (readl(&p_reg->hs) & __ADC_HS_COCO0) {

        /* 禁能转换完成中断 */
        AW_REG_BIT_CLR_MASK32(&p_reg->hc0, __ADC_HC0_AIEN);

        /* 读取采样值 */
        data = readl(&p_reg->r0);

        /* 填充数据 */
        __imx6ul_adc_data_fill(p_this, data);
    }
}

/**
 * \brief ADC 转换开始
 */
aw_local aw_err_t __imx6ul_adc_start (void                 *p_cookie,
                                      aw_adc_channel_t      channel,
                                      aw_adc_buf_desc_t    *p_bufdesc,
                                      int                   bufdesc_num,
                                      uint32_t              count,
                                      pfn_adc_complete_t    pfn_seq_complete,
                                      void                 *p_arg)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);
    awbl_imx6ul_adc_dev_t     *p_this    = __DEV_GET_ADC_DEV(p_cookie);

    /* 最大通道号为15 */
    if (channel > 15) {
        return -AW_ENXIO;
    }

    /* 参数有效性检测 */
    if (p_bufdesc == NULL || bufdesc_num == 0 || p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_this->phy_chan         = channel;
    p_this->samples          = 0;
    p_this->p_bufdesc        = p_bufdesc;
    p_this->bufdesc_num      = bufdesc_num;
    p_this->remain_count     = count;
    p_this->is_stopped       = AW_FALSE;
    p_this->cur_bufdesc_id   = 0;

    p_this->pfn_seq_complete = pfn_seq_complete;
    p_this->p_arg            = p_arg;

    /* 启动通道转换 */
    __imx6ul_adc_start_convert(p_devinfo->regbase, channel);

    return AW_OK;
}

/**
 * \brief ADC 转换停止
 */
aw_local aw_err_t __imx6ul_adc_stop (void               *p_cookie,
                                     aw_adc_channel_t    channel)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);
    awbl_imx6ul_adc_dev_t     *p_this    = __DEV_GET_ADC_DEV(p_cookie);

    aw_adc_channel_t  phy_ch = channel - p_devinfo->adc_servinfo.ch_min;

    /* 最大通道号为15 */
    if (phy_ch > 15) {
        return -AW_ENXIO;
    }

    /* 当前转换的通道不为需要停止的通道 */
    if (p_this->phy_chan != phy_ch) {
        return -AW_EPERM;
    }

    /* 标识转换被停止 */
    p_this->is_stopped = AW_TRUE;

    return AW_OK;
}

/**
 * \brief 获取ADC 通道位宽 bits
 */
aw_local aw_err_t __imx6ul_adc_bits_get (void *p_cookie, aw_adc_channel_t channel)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);

    return p_devinfo->bits;
}

/**
 * \brief 获取ADC采样率
 */
aw_local aw_err_t __imx6ul_adc_rate_get (void               *p_cookie,
                                         aw_adc_channel_t    channel,
                                         uint32_t           *p_rate)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);
    imx6ul_adc_regs_t         *p_reg     = (imx6ul_adc_regs_t *)(p_devinfo->regbase);
    
    uint8_t div      = 0;
    uint8_t multiple = 0;
    
    /* ADIV */
    div = (uint8_t)((readl(&p_reg->cfg) & __ADC_CFG_ADIV_MASK) >> __ADC_CFG_ADIV_SHIFT);

    div = (1 << div);
    
    /* clk source use IPG /2 ? */
    if ((readl(&p_reg->cfg) & __ADC_CFG_ADICLK_MASK) == 0x01) {
        div = div * 2;
    }
    
    /* 查找采样倍率 */
    switch(__imx6ul_adc_bits_get(p_cookie, channel)) {

    case 8:
        multiple = __ADC_CT_RATE_8BIT;
        break;

    case 10:
        multiple = __ADC_CT_RATE_10BIT;
        break;

    case 12:
        multiple = __ADC_CT_RATE_12BIT;
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    /* 此处需要注意ADC的时钟源 */
    *p_rate = (aw_clk_rate_get(IMX6UL_CLK_IPG) / div) / multiple;

    return AW_OK;
}

/**
 * \brief 设置ADC采样率
 * 
 *  |--SFCAdder1--|-------BCT------|------LSTAddr-------|--SFCAdder2--|
 *
 *    SFCAdder1 + SFCAdder2 = 4ADCK
 *    AverageNum = 1 （不使用硬件平均）
 *    BCT(8bit)  = 17ADCK ** BCT(10bit) = 21ADCK ** BCT(12bit) = 25ADCK
 *    LSTAddr    = 3 ADCK （选择短采样模式 ADLSMP = 0, ADSTS = 00）
 *
 *    conversion Timer = SFCAdder1+ SFCAdder2+ AverageNum * (BCT + LSTAddr)
 *    = 25(8bit) 29(10bit) 33(12bit)
 */
aw_local aw_err_t __imx6ul_adc_rate_set (void               *p_cookie,
                                         aw_adc_channel_t    channel,
                                         uint32_t            rate)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);
    imx6ul_adc_regs_t         *p_reg     = (imx6ul_adc_regs_t *)(p_devinfo->regbase);

    uint32_t last_rate = 0;
    uint32_t div       = 0;
    uint8_t  multiple  = 0;
    uint32_t clk_in    = 0;
    
    /* 查找采样倍率 */
    switch(__imx6ul_adc_bits_get(p_cookie, channel)) {

    case 8:
        multiple = __ADC_CT_RATE_8BIT;
        break;

    case 10:
        multiple = __ADC_CT_RATE_10BIT;
        break;

    case 12:
        multiple = __ADC_CT_RATE_12BIT;
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    /* 获取输入时钟 */
    clk_in =aw_clk_rate_get(IMX6UL_CLK_IPG);

    /* 最大采样率为1Msps */
    if (rate >= 1000000) {
        rate = 1000000;
    }

    __imx6ul_adc_rate_get(p_cookie, channel, &last_rate);

    if (last_rate == rate) {
        return AW_OK;
    } else {
          
        /* 可实现IPG时钟 1 2 4 8 16 分频*/
        div = clk_in / (rate * multiple);
        
        AW_REG_BIT_CLR_MASK32(&p_reg->cfg, __ADC_CFG_ADICLK_MASK);
        AW_REG_BIT_CLR_MASK32(&p_reg->cfg, __ADC_CFG_ADIV_MASK);

        if (div < 2) {
            /* div = 1 */
        } else if (div < 3) {
            /* div  = 2*/
            AW_REG_BIT_SET_MASK32(& p_reg->cfg, (0x01 << __ADC_CFG_ADIV_SHIFT));
        } else if (div < 6) {
            /* div = 4 */
            AW_REG_BIT_SET_MASK32(& p_reg->cfg, (0x02 << __ADC_CFG_ADIV_SHIFT));
        } else if (div < 12) {
            /* div = 8 */
            AW_REG_BIT_SET_MASK32(& p_reg->cfg, (0x03 << __ADC_CFG_ADIV_SHIFT));
        } else {
            /* div = 16 */
            AW_REG_BIT_SET_MASK32(& p_reg->cfg, (0x03 << __ADC_CFG_ADIV_SHIFT) );
            AW_REG_BIT_SET_MASK32(& p_reg->cfg, (0x01 << __ADC_CFG_ADICLK_SHIFT));
        }
    }

    return AW_OK;
}

/**
 * \brief 获取ADC 参考电压
 */
aw_local aw_err_t __imx6ul_adc_vref_get (void *p_cookie, aw_adc_channel_t channel)
{
    awbl_imx6ul_adc_devinfo_t *p_devinfo = __DEV_GET_ADC_INFO(p_cookie);

    return p_devinfo->refmv;
}
/******************************************************************************/
/**
 * \brief ADC 驱动注册
 */
void awbl_imx6ul_adc_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_imx6ul_adc_drv_registration);
}

