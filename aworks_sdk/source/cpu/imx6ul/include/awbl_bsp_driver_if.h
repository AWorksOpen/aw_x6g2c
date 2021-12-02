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
 * \brief AWBus driver register and initializing interfaces.
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-07  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BSP_DRIVER_IF_H
#define __AWBL_BSP_DRIVER_IF_H

#include "aw_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

aw_export void awbl_imx6ul_i2c_drv_register (void);
aw_export void awbl_imx6ul_enet_drv_register (void);
aw_export void awbl_imx6ul_timer_drv_register (void);
aw_export void awbl_imx6ul_gptimer_drv_register (void);
aw_export void awbl_imx6ul_adc_drv_register (void);
aw_export void awbl_imx6ul_flexcan_drv_register (void);
aw_export void awbl_mxs_hw480272f_drv_register (void);
aw_export void awbl_mxs_tft_panel_drv_register (void);
aw_export void awbl_mxs_emwin_fb_drv_register (void);
aw_export void awbl_imx6ul_touch_screen_drv_register (void);
aw_export void awbl_imx6ul_tempmon_drv_register (void);
aw_export void awbl_imx6ul_wdt_drv_register (void);
aw_export void awbl_imx6ul_sim_drv_register (void);
aw_export void awbl_imx6ul_pwm_drv_register (void);
aw_export void awbl_imx6ul_qspi_drv_register (void);
aw_export void awbl_imx6ul_gpmi_drv_register (void);
aw_export void awbl_imx6ul_dma_drv_register (void);
aw_export void awbl_imx6ul_gpio_drv_register (void);
aw_export void awbl_imx6ul_i2c_register (void);
aw_export void awbl_imx6ul_intc_drv_register (void);
aw_export void awbl_imx6ul_ocotp_register (void);
aw_export void awbl_imx6ul3_pwm_drv_register (void);
aw_export void awbl_imx6ul_uart_drv_register (void);
aw_export void awbl_imx6ul_ecspi_drv_register (void);
aw_export void awbl_imx6ul_usbd_drv_register (void);
aw_export void awbl_imx6ul_usbh_drv_register (void);
aw_export void awbl_imx6ul_sai_drv_register (void);
aw_export void awbl_imx6ul_mqs_drv_register (void);
aw_export void awbl_imx6ul_sdio_drv_register (void);
aw_export void awbl_zb_module_drv_register(void);
aw_export void awbl_ov7725_drv_register(void);
aw_export void awbl_imx6ul_csi_drv_register(void);
aw_export void awbl_imx6ul_eim_drv_register (void);
aw_export void awbl_imx6ul_timestamp_drv_register(void);

aw_export void awbl_sim_init (void);
aw_export void awbl_zigbee_init (void);
aw_export void aw_cplusplus_runtime_init (void);
aw_export void aw_camera_serve_init (void);

aw_export void awbl_gprs_init (void);
aw_export void sio_list_init(void);

void awbl_imx6ul_caam_drv_register (void);
void awbl_imx6ul_sdma_drv_register (void);
void awbl_imx6ul_sdma_init(void);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_DRIVER_IF_H */

/* end of file */
