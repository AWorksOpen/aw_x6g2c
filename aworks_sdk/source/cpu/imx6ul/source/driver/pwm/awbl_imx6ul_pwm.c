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
 * \brief Freescale i.MX6UL系列PWM外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-07  hgo, first implemetation
 * \endinternal
 */
/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "aw_clk.h"
#include "driver/pwm/awbl_imx6ul_pwm.h"
#include "aw_system.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"

#include "awbl_plb.h"

/*******************************************************************************
  registers offset
*******************************************************************************/
/**
 * \brief IMX6UL PWM硬件寄存器
 */
typedef struct {
    uint32_t pwmcr;             /* PWM控制寄存器 */
    uint32_t pwmsr;             /* PWM状态寄存器 */
    uint32_t pwmir;             /* PWM中断控制寄存器 */
    uint32_t pwmsar;            /* PWM采样寄存器 */
    uint32_t pwmpr;             /* PWM周期寄存器 */
    uint32_t pwmcnt;            /* PWM计数器 */
}awbl_imx6ul_pwm_hw_t;
/*******************************************************************************
 registers bit definition Macros
*******************************************************************************/
/* CR */
#define __IMXPWM_CR_EN_MASK                 (0x1u << 0)
#define __IMXPWM_CR_SWR_MASK                (0x1u << 3)
#define __IMXPWM_CR_CLKSRC_MASK             (0x3u << 16)
#define __IMXPWM_CR_CLKSRC_NONE             (0x0u << 16)
#define __IMXPWM_CR_CLKSRC_IPG_CLK          (0x1u << 16)
#define __IMXPWM_CR_CLKSRC_IPG_CLK_HFREQ    (0x2u << 16)
#define __IMXPWM_CR_CLKSRC_IPG_CLK_32K      (0x3u << 16)
#define __IMXPWM_CR_PRESCALE_MASK           (0xFFFu << 4)
#define __IMXPWM_CR_PRESCALE_SET(prescale) (((prescale) & 0xFFF) << 4)
#define __IMXPWM_CR_POUTC_MASK              (0x3u << 18)
#define __IMXPWM_CR_POUTC_HIGH              (0x0u << 18)
#define __IMXPWM_CR_POUTC_LOW               (0x1u << 18)
#define __IMXPWM_CR_STOPEN                  (0x1u << 25)
#define __IMXPWM_CR_DOZEN                   (0x1u << 24)
#define __IMXPWM_CR_WAITEN                  (0x1u << 23)
#define __IMXPWM_CR_DBGEN                   (0x1u << 22)

/* SR */
#define __IMXPWM_SR_FE_MASK               (0x1u << 3)
#define __IMXPWM_SR_ROV_MASK              (0x1u << 4)
#define __IMXPWM_SR_CMP_MASK              (0x1u << 5)
#define __IMXPWM_SR_FWE_MASK              (0x1u << 6)

/* SAR */
#define __IMXPWM_SAR_SAMPLE_MASK          (0xFFFFu << 0)

/* PR */
#define __IMXPWM_PR_PERIOD_MASK           (0xFFFFu << 0)

/*******************************************************************************
  macro operate
*******************************************************************************/
#define __PWM_DEV_TO_PARAM(p_dev) \
    (awbl_imx6ul_pwm_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_THIS(p_dev) \
    (awbl_imx6ul_pwm_dev_t *)(p_dev)

/** \brief PWMn的基地值 */
const aw_local uint32_t __g_pwm_regbase[__IMX6UL_PWM_NUM] = {
        0x02080000, /* PWM1 */
        0x02084000, /* PWM2 */
        0x02088000, /* PWM3 */
        0x0208C000, /* PWM4 */
        0x020F0000, /* PWM5 */
        0x020F4000, /* PWM6 */
        0x020F8000, /* PWM7 */
        0x020FC000, /* PWM8 */
};

/* get imx6ul pwm hardware */
#define __IMXPWM_HW_DECL(p_hw_pwm, pid, regbasebuf)    \
        awbl_imx6ul_pwm_hw_t *p_hw_pwm =            \
            ((awbl_imx6ul_pwm_hw_t *)(regbasebuf[pid]))

/** \brief
 *  当对一个channel进行编程时，对于寄存器写来说，有个写顺序依赖。
 *  HW_PWM_ACTIVEn寄存器必须先写，随后是HW_PWM_PERIODn。
 *  如果顺序反了，写进HW_PWM_ACTIVEn的参数将不会在硬件中生效。
 */
aw_local aw_err_t __imx6ul_pwm_config(void          *p_cookie,
                                      int            pid,
                                      unsigned long  duty_ns,
                                      unsigned long  period_ns)
{
    __IMXPWM_HW_DECL(p_hw_pwm, 0, __g_pwm_regbase);
    uint64_t c = 0;
    unsigned long src_clk_freq;
    uint32_t cvalue = 0;
    uint16_t best_prescale = 0;       /* 最佳分频值 */
    uint16_t best_period_count = 0;   /* 最佳的周期计数值 */
    uint16_t best_duty_count = 0;     /* 最佳的占空比计数值*/
    awbl_imx6ul_pwm_dev_t      *p_this = __PWM_DEV_TO_THIS(p_cookie);


    if ((period_ns < 0) || (period_ns == 0) || (duty_ns < 0 ) || (duty_ns > period_ns)) {
        AW_INFOF(("awbl_imx6ul_pwm: duty_ns or period_ns is invalid.\n"));
        return -AW_EINVAL;
    }

    if (period_ns < 50) {
        AW_INFOF(("awbl_imx6ul_pwm: period_ns must be greater then 50ns.\n"));
        return -AW_EINVAL;
    }

//    if (duty_ns < 15 || (period_ns - duty_ns) < 15) {
//        AW_INFOF(("awbl_imx6ul_pwm: duty_ns and (period_ns - duty_ns) "
//                  "must be greater then 15ns\n"));
//        return -AW_EINVAL;
//    }

    if ((pid < 0) || (pid > __IMX6UL_PWM_NUM)) {
       AW_INFOF(("awbl_imx6ul_pwm: cannot find installed PWM for the pid number.\n"));
       return -AW_EINVAL;
    }

    src_clk_freq = aw_clk_rate_get(p_this->counter_src_clk_id[pid]);

    /* period is match? */
    /* note : the data is easy to overflow */
    if (period_ns > (((uint64_t)1000000000UL * 268431360UL / src_clk_freq) )) {
        AW_INFOF(("awbl_imx6ul_pwm: period_ns is invalid, as it is too larger.\n"));
        return -AW_EINVAL;
    }
    if (period_ns < (1000000000UL / src_clk_freq)) {
        AW_INFOF(("awbl_imx6ul_pwm: period_ns is invalid, as it is too smaller.\n"));
        return -AW_EINVAL;
    }
    if (duty_ns < (1000000000UL / src_clk_freq)) {
       AW_INFOF(("awbl_imx6ul_pwm: duty_ns is invalid, as it is too smaller.\n"));
       return -AW_EINVAL;
    }

    /*
     * period_ns =((prescale + 1) / src_clk_freq) * period * 1000000000;
    */
    c = (uint64_t)src_clk_freq * period_ns;
    c = c / 1000000000UL;

    best_prescale = c / 0xFFFFUL;
    if (c > 2 ) {
        best_period_count = c / (best_prescale + 1) - 2;
    } else {
        best_period_count = 0;
    }
    /*
    * duty_ns =((prescale + 1) / src_clk_freq) * duty * 1000000000;
    */
    c = (uint64_t)src_clk_freq * duty_ns;
    c = c / 1000000000UL;
    best_duty_count = (c / (best_prescale + 1));

    p_hw_pwm = (awbl_imx6ul_pwm_hw_t *)__g_pwm_regbase[pid];

    cvalue = readl(&p_hw_pwm->pwmcr);
    if (((cvalue & __IMXPWM_CR_PRESCALE_MASK) ^
          (__IMXPWM_CR_PRESCALE_SET(best_prescale)))) {
        /* set prescale */
        cvalue &= ~__IMXPWM_CR_PRESCALE_MASK;
        cvalue |= __IMXPWM_CR_PRESCALE_SET(best_prescale);
        writel(cvalue, &p_hw_pwm->pwmcr);
    }

    if (((readl(&p_hw_pwm->pwmpr)  & __IMXPWM_PR_PERIOD_MASK) == best_period_count) &&
        ((readl(&p_hw_pwm->pwmsar) & __IMXPWM_SAR_SAMPLE_MASK) == best_duty_count)) {
        /* all period and duty config data is the same as old */
        return AW_OK;
    }

    /* set sar sample value */
    writel(best_duty_count, &p_hw_pwm->pwmsar);
    /* set period value */
    writel(best_period_count, &p_hw_pwm->pwmpr);

    /* clear status register */
    writel(__IMXPWM_SR_CMP_MASK |
           __IMXPWM_SR_FE_MASK  |
           __IMXPWM_SR_FWE_MASK |
           __IMXPWM_SR_ROV_MASK , &p_hw_pwm->pwmsr);

    return AW_OK;
}


 /** \brief
  *
  */
 aw_local aw_err_t __imx6ul_pwm_enable(void *p_cookie, int pid)
 {
     awbl_imx6ul_pwm_devinfo_t *p_devinfo = __PWM_DEV_TO_PARAM(p_cookie);
     awbl_imx6ul_pwm_devcfg_t  *p_devcfg  = p_devinfo->pwm_devcfg;
     __IMXPWM_HW_DECL(p_hw_pwm, 0, __g_pwm_regbase);
     uint8_t n = 0;
     uint32_t cvalue = 0;

     if ((pid < 0) || (pid > __IMX6UL_PWM_NUM)) {
         return AW_ERROR;
     }
     /* 查找与pid匹配的引脚, 配置为默认引脚模式 */
     for (n = 0; n < p_devinfo->pnum; n++)  {
         if (pid == p_devcfg[n].pid) {
             aw_gpio_pin_cfg(p_devcfg[n].gpio, p_devcfg[n].func);
             break;
         }
     }
     if (n >= p_devinfo->pnum) {
         return AW_ERROR;
     }
     p_hw_pwm = (awbl_imx6ul_pwm_hw_t *)__g_pwm_regbase[pid];

     cvalue           = readl(&p_hw_pwm->pwmcr);
     cvalue          |= __IMXPWM_CR_EN_MASK ;

     writel(cvalue, &p_hw_pwm->pwmcr);

     return AW_OK;
 }

 /** \brief
  *
  */
 aw_local aw_err_t __imx6ul_pwm_disable(void *p_cookie, int pid)
 {
     awbl_imx6ul_pwm_devinfo_t *p_devinfo = __PWM_DEV_TO_PARAM(p_cookie);
     awbl_imx6ul_pwm_devcfg_t  *p_devcfg  = p_devinfo->pwm_devcfg;
     __IMXPWM_HW_DECL(p_hw_pwm, 0, __g_pwm_regbase);
     uint8_t n = 0;
     uint32_t cvalue = 0;

     if ((pid < 0) || (pid > __IMX6UL_PWM_NUM)) {
         return AW_ERROR;
     }
     /* 查找与pid匹配的引脚, 配置为默认引脚模式 */
     for (n = 0; n < p_devinfo->pnum; n++) {
         if (pid == p_devcfg[n].pid) {
             aw_gpio_pin_cfg(p_devcfg[n].gpio, p_devcfg[n].dfunc);
             break;
         }
     }
     if (n >= p_devinfo->pnum) {
         return AW_ERROR;
     }
     p_hw_pwm = (awbl_imx6ul_pwm_hw_t *)__g_pwm_regbase[pid];

     cvalue           = readl(&p_hw_pwm->pwmcr);
     cvalue          &= ~__IMXPWM_CR_EN_MASK;     /* 禁能模块 */

     writel(cvalue, &p_hw_pwm->pwmcr);

     return AW_OK;
 }

 /** \brief first level initialization routine */
 aw_local void __pwm_inst_init1 (awbl_dev_t *p_dev)
 {
    __IMXPWM_HW_DECL(p_hw_pwm, 0, __g_pwm_regbase);
    awbl_imx6ul_pwm_dev_t      *p_this = __PWM_DEV_TO_THIS(p_dev);
    awbl_imx6ul_pwm_devinfo_t  *p_devinfo = __PWM_DEV_TO_PARAM(p_dev);
    awbl_imx6ul_pwm_devcfg_t   *p_devcfg  = p_devinfo->pwm_devcfg;
    uint32_t                    value = 0, i = 0, pid = 0;
    aw_tick_t                   start_ticks, end_ticks;
    volatile uint32_t           timeout_ms;
    aw_clk_id_t                 parent;

    /* 软件复位所有PWM */
    for (i = 0; i < p_devinfo->pnum; i++) {
        pid = p_devcfg[i].pid;

        switch ((p_devcfg[i].cfgs & 0x0C)) {
        case AWBL_IMX6UL_PWM_CFG_CLKSRC_IPG_CLK:
            parent = IMX6UL_CLK_IPG;
            break;
        case AWBL_IMX6UL_PWM_CFG_CLKSRC_PERCLK_CLK:
            parent = IMX6UL_CLK_PERCLK;
            break;
        case AWBL_IMX6UL_PWM_CFG_CLKSRC_CKIL_SYNC_CLK:
            parent = IMX6UL_CLK_CKIL;
            break;
        default:
            parent = IMX6UL_CLK_CKIL;
            break;
        }
        p_this->counter_src_clk_id[pid] = parent;

        /* 打开时钟 */
        aw_clk_enable(IMX6UL_CLK_PWM1 + pid);
        p_hw_pwm = (awbl_imx6ul_pwm_hw_t *)__g_pwm_regbase[pid];
        /* 初始化时软件复位PWM */
        value           = readl(&p_hw_pwm->pwmcr);
        value          |= __IMXPWM_CR_SWR_MASK;
        writel(value, &p_hw_pwm->pwmcr);
        /* 等待复位完成 */
        start_ticks  = aw_sys_tick_get();
        do {
         end_ticks  = aw_sys_tick_get();
         timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);
        } while( (readl(&p_hw_pwm->pwmcr) & __IMXPWM_CR_SWR_MASK) &&
              (timeout_ms < 10));

        /*
        * Output pin is cleared at rollover and set at comparison
        * if not true, the defualt setting is:
        *   Output pin is set at rollover and cleared at comparison
        */
        value  = readl(&p_hw_pwm->pwmcr);
        value &= ~(__IMXPWM_CR_CLKSRC_MASK |
                __IMXPWM_CR_POUTC_MASK  |
                __IMXPWM_CR_STOPEN      |
                __IMXPWM_CR_DOZEN       |
                __IMXPWM_CR_WAITEN      |
                __IMXPWM_CR_DBGEN) ;

        value |=  ((p_devcfg[i].cfgs & 0xF0) << (22-4)) | /* 功耗配置 */
                  ((p_devcfg[i].cfgs & 0x0C) << (16-2)) | /* 时钟源配置 */
                  ((p_devcfg[i].cfgs & 0x03) << (18-0)) ; /* PWMO 电平配置 */

        writel(value, &p_hw_pwm->pwmcr);
    }


 }

 /** \brief second level initialization routine */
 aw_local void __pwm_inst_init2 (awbl_dev_t *p_dev)
 {

 }

 /** \brief third level initialization routine */
 aw_local void __pwm_inst_connect(awbl_dev_t *p_dev)
 {
 }

/*******************************************************************************
    pwm driver functions
*******************************************************************************/
aw_local aw_const struct awbl_pwm_servopts __g_imx6ul_pwm_servopts = {
        __imx6ul_pwm_config,      /* pfunc_pwm_config */
        __imx6ul_pwm_enable,      /* pfunc_pwm_enable */
        __imx6ul_pwm_disable,     /* pfunc_pwm_disable */
};

aw_local aw_const struct awbl_drvfuncs __g_pwm_drvfuncs = {
        __pwm_inst_init1,
        __pwm_inst_init2,
        __pwm_inst_connect
};

/** \brief method "awbl_pwm_func_get" handler */
aw_local aw_err_t __imx6ul_pwmserv_get (struct awbl_dev *p_dev, void *p_arg)
{
     awbl_imx6ul_pwm_dev_t  *p_this  =  __PWM_DEV_TO_THIS(p_dev);
     awbl_imx6ul_pwm_devinfo_t *p_devinfo = __PWM_DEV_TO_PARAM(p_dev);

     struct awbl_pwm_service *p_serv = NULL;

     /* 获取PWM服务实例 */
    p_serv = &p_this->pwm_serv;

    /* 初始化PWM服务实例 */
    p_serv->p_next = NULL;
    p_serv->p_servinfo = &p_devinfo->pwm_servinfo;
    p_serv->p_servfuncs = &__g_imx6ul_pwm_servopts;
    p_serv->p_cookie = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_pwm_service **)p_arg = p_serv;

     return AW_OK;
}

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_pwmserv_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx6ul_pwm_methods[] = {
    AWBL_METHOD(awbl_pwmserv_get, __imx6ul_pwmserv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_pwm_drv_registration = {
    {
            AWBL_VER_1,                  /* awb_ver */
            AWBL_BUSID_PLB,              /* bus_id */
            AWBL_IMX6UL_PWM_NAME,         /* p_drvname */
            &__g_pwm_drvfuncs,           /* p_busfuncs */
            &__g_imx6ul_pwm_methods[0],   /* p_methods */
            NULL                         /* pfunc_drv_probe */
    }
};

void awbl_imx6ul_pwm_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_pwm_drv_registration);
}


