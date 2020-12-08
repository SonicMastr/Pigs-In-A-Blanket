cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

project(GLES20_Cube_System)
include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

set(DOLCE_APP_NAME "OpenGL ES 2.0 Cube System")
set(DOLCE_TITLEID  "GLES00003")
set(DOLCE_VERSION  "01.00")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA -Wno-main -Wall -fno-lto -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(DOLCE_MKSFOEX_FLAGS "${DOLCE_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

add_executable(${PROJECT_NAME}
  main.c
)

target_link_libraries(${PROJECT_NAME}
  pib
  SceLibKernel_stub
  SceThreadmgr_stub
  SceModulemgr_stub
  SceSysmodule_stub
  SceIofilemgr_stub
  SceGxm_stub
  SceDisplay_stub
  SceLibc_stub
  SceLibm_stub
)

set(DOLCE_ELF_CREATE_FLAGS "${DOLCE_ELF_CREATE_FLAGS} -h 2097152")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin
  COMMAND dolce-make-bootparam app_memsize 0x8000 attribute 0x02 ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin # 0x8000 = 32MB(32768kb)memsize
)

dolce_create_self(eboot.bin ${PROJECT_NAME} UNSAFE BOOT_PARAM ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin)

dolce_create_vpk(${PROJECT_NAME}.vpk ${DOLCE_TITLEID} eboot.bin
  VERSION ${DOLCE_VERSION}
  NAME ${DOLCE_APP_NAME}
)
