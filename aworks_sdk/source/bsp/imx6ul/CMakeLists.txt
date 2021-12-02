enable_language(ASM)
file(GLOB c_srcs "*.c" )
file(GLOB asm_srcs "*.S")
file(GLOB headers "*.h" )

add_library(aw_bsp STATIC
    ${c_srcs}
	${asm_srcs}
    ${headers}
    )
    
target_include_directories(aw_bsp PUBLIC 
                           . 
						   ${AWORKS_SDKROOT}/include/bsp
                           ${AWORKS_ROOT}/apollo/cpu/freescale/imx6ul/include)
target_link_libraries(aw_bsp PUBLIC aworks )


install(TARGETS aw_bsp 
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )
 
install(FILES ${headers} DESTINATION ${AWORKS_SDKROOT}/include/bsp )
aw_export(aw_bsp)

