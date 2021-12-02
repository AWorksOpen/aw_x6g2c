
enable_language(ASM)
set(imx6ul_source
  source/aw_sdk_version.c
  source/imx_clk_busy.c
  source/imx_clk_enet_tx_clk_ref.c
  source/imx_clk_gate2.c
  source/imx_clk_pfd.c
  source/imx_clk_pllv3.c
  source/imx6ul_clock.c
  source/aw_sdk_version.c
  source/imx6ul_sys_timer.c
  source/imx6ul_power.c
  source/imx6ul_cpu_idle.S
  source/imx6ul_iomuxc_regs.c
  source/driver/adc/awbl_imx6ul_adc.c
  source/driver/can/awbl_imx6ul_can.c
  source/driver/caam/awbl_imx6ul_caam.c
  source/driver/caam/aw_imx6ul_caam.c
  source/driver/caam/caam_ctrl.c
  source/driver/caam/caam_func.c
  source/driver/caam/caam_jr.c
  source/driver/caam/caam_sm_store.c
  source/driver/display/awbl_mxs_emwin_fb.c
  source/driver/display/awbl_mxs_hw480272f.c
  source/driver/display/awbl_mxs_lcdif.c
  source/driver/display/awbl_mxs_method.c
  source/driver/gpio/awbl_imx6ul_gpio.c
  source/driver/input/touchscreen/awbl_imx6ul_ts.c
  source/driver/intctlr/awbl_imx6ul_intc.c
  source/driver/intctlr/gic.c
  source/driver/intctlr/interrupt.c
  source/driver/nvram/awbl_imx6ul_ocotp.c
  source/driver/serial/awbl_imx6ul_uart.c
  source/driver/timer/awbl_imx6ul_timer.c
  source/driver/timer/awbl_imx6ul_gptimer.c
  source/driver/net/awbl_imx6ul_enet.c
  source/driver/sdio/awbl_imx6ul_sdio.c
  source/driver/i2c/awbl_imx6ul_i2c.c
  source/driver/pwm/awbl_imx6ul_pwm.c
  source/driver/spi/awbl_imx6ul_ecspi.c
  source/driver/spi/awbl_imx6ul_qspi.c
  source/driver/usb/awbl_imx6ul_usbh.c
  source/driver/usb/awbl_imx6ul_usbd.c
  source/driver/mqs/awbl_imx6ul_mqs.c
  source/driver/sai/awbl_imx6ul_sai.c
  source/imx6ul_sound_wav.c
  source/stream.c
  source/driver/sim/awbl_sim.c
  source/driver/sim/awbl_imx6ul_sim.c
  source/driver/tempmon/awbl_imx6ul_tempmon.c
  source/driver/wdt/awbl_imx6ul_wdt.c
  source/driver/busctlr/awbl_imx6ul_gpmi.c
  source/driver/dma/awbl_imx6ul_dma.c
  source/driver/dma/aw_imx6ul_sdma_fw.c
  source/driver/dma/awbl_imx6ul_sdma.c
  source/driver/dma/awbl_imx6ul_sdma_service.c
  source/driver/gprs/awbl_gprs_me909s.c
  source/driver/zigbee/aw_zb_module.c
  source/driver/zigbee/awbl_zb_module.c
)


include_directories(${AWORKS_ROOT}/apollo/components/gui/emwin5xx_adapter/include)
include_directories(${AWORKS_ROOT}/apollo/bsp/imx6ul)

add_library(aw_cpu STATIC ${imx6ul_source} )

target_include_directories( aw_cpu PUBLIC
  include
  include_notinsdk
  ${AWORKS_ROOT}/apollo/clock/include
  )
  
install(DIRECTORY include/ DESTINATION ${AWORKS_SDKROOT}/include/cpu)

target_link_libraries( aw_cpu PUBLIC aworks aw_common aw_lwip_1.4.x )

option(AWORKS_CRP_EN OFF "crp enable")
if(AWORKS_CRP_EN)
    target_compile_definitions(aw_cpu PRIVATE AWORKS_CRP_EN=1 )
endif()


aw_export(aw_cpu)

install(TARGETS aw_cpu
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )
  
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/source/libstackoverflowcheck/awbl_stack_overflow.c )
  add_library( awbl_stackoverflowcheck STATIC
    source/libstackoverflowcheck/awbl_stack_overflow.c 
    source/libstackoverflowcheck/stc_md5c.c 
    )
  target_link_libraries(awbl_stackoverflowcheck PRIVATE aworks aw_cpu)
  target_include_directories(awbl_stackoverflowcheck PRIVATE
    source
    include_notinsdk
    )

  # 根据FPU类型配置imx6ul参数
  if(AWORKS_CORE_FLOAT_ABI STREQUAL "soft")
    set(libawbl_stackoverflowcheck_instll_dir ${AWORKS_ROOT}/apollo/lib/cortex_a9_rtk_gcc_soft/ )
  else()
    if(AWORKS_CORE_FPU_TYPE STREQUAL "default")
      set(libawbl_stackoverflowcheck_instll_dir ${AWORKS_ROOT}/apollo/lib/cortex_a9_rtk_gcc_${AWORKS_CORE_FLOAT_ABI}/ )
    else()
      set(libawbl_stackoverflowcheck_instll_dir ${AWORKS_ROOT}/apollo/lib/cortex_a9_rtk_gcc_${AWORKS_CORE_FLOAT_ABI}_${AWORKS_CORE_FPU_TYPE}/ )
    endif()
  endif()

  install(TARGETS awbl_stackoverflowcheck
    LIBRARY DESTINATION ${libawbl_stackoverflowcheck_instll_dir}
    ARCHIVE DESTINATION ${libawbl_stackoverflowcheck_instll_dir}
    RUNTIME DESTINATION bin
  )
endif()