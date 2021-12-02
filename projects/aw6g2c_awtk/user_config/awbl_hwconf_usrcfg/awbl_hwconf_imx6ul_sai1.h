/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      Apollo.support@zlg.cn
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX6UL_SAI1_H
#define  __AWBL_HWCONF_IMX6UL_SAI1_H

#ifdef AW_DEV_IMX6UL_SAI1

/*******************************************************************************
 SAI1������������Ϣ
*******************************************************************************/
#include "apollo.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/sai/awbl_imx6ul_sai.h"
#include "aw_gpio.h"
#include "imx6ul_regs_base.h"

#define __IMX6UL_SAI1_PAD_CTRL             IMX6UL_PAD_CTL(              \
                                           PUS_1_47K_Ohm_Pull_Up      | \
                                           PKE_1_Pull_Keeper_Enabled  | \
                                           PUE_1_Pull                 | \
                                           Open_Drain_Disabled        | \
                                           SPEED_3_max_200MHz         | \
                                           DSE_1_R0                   | \
                                           SRE_0_Fast_Slew_Rate)
aw_local void __imx6ul_sai1_plat_init (void)
{
    aw_clk_enable(IMX6UL_CLK_SAI1_IPG);
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_imx6ul_sai_devinfo __g_imx6ul_sai1_devinfo = {
     IMX6UL_SAI1_BASE_ADDR,          /* �Ĵ�����ַ */
     INUM_SAI1,                      /* �жϺ�          */
     IMX6UL_CLK_SAI1,                  /* ʱ��id    */
     AW_BL_IMX6UL_SAI_MCLK_SEL_1,    /* ʹ��MQSʱ����ֵ����Ϊ 1 */
     __imx6ul_sai1_plat_init,        /* ƽ̨��س�ʼ�� */
     SDMA_EVENTS_SAI1_TX,
     0,                              /* ��������ʹ��DMA�������ʹ��,����ʹӲ���ҵ� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx6ul_sai_dev __g_imx6ul_sai1_dev;

#define AWBL_HWCONF_IMX6UL_SAI1                     \
    {                                               \
        AWBL_IMX6UL_SAI_NAME,                       \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx6ul_sai1_dev,    \
        &__g_imx6ul_sai1_devinfo                    \
    },


#else

#define AWBL_HWCONF_IMX6UL_SAI1

#endif  /* AW_DEV_IMX6UL_SAI1 */

#endif /* __AWBL_HWCONF_IMX6UL_SAI1_H */

/* end of file */
