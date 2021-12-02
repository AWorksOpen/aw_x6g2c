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
 * \brief the definition of consts and struct of pmu in imx6ul
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-18  randy, first implementation
 * \endinternal
 */

#ifndef __IMX6UL_PMU_REGS_H
#define __IMX6UL_PMU_REGS_H

#include "apollo.h"
#include "aw_bitops.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/*!
 * @brief consts of PMU_REG_3P0
 */

/** \brief enable the LDO */
#define PMU_REG_3P0_ENABLE          AW_BIT(0)
/** \brief enable the LDO brownout circuitry */
#define PMU_REG_3P0_ENABLE_BO       AW_BIT(1)
/** \brief enable the LDO current-limit circuitry*/
#define PMU_REG_3P0_ENABLE_LIMIT    AW_BIT(2)
/** \brief enable the LDO */
#define PMU_REG_3P0_BO_OFFSET(x)    AW_SBF(x,4)
/** \brief input voltage source for LDO_3P0 from USB_OTG1_VBUS */
#define PMU_REG_3P0_VBUS_SEL_OTG1    (0<<7)
/** \brief input voltage source for LDO_3P0 from USB_OTG2_VBUS */
#define PMU_REG_3P0_VBUS_SEL_OTG2    AW_BIT(7)
/** \brief set the output voltage,x is the count of 0.025V above 2.625V */
#define PMU_REG_3P0_OUTPUT_TRIG(x)   AW_SBF(x,8)
/** \brief a brownout is detected on the regulator output */
#define PMU_REG_3P0_BO_VDD3P0       AW_BIT(16)
/** \brief the regulator output is ok */
#define PMU_REG_3P0_OK_VDD3P0       AW_BIT(17)

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX6UL_IOMUXC_REGS_H */

/* end of file */
