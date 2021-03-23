cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE "$ENV{SCE_PSP2_SDK_DIR}/host_tools/build/cmake/psp2-snc-toolchain.cmake")

project(GLES20_Cube_GLFW LANGUAGES C)

include(VitaDevelopmentSuite)
set(CMAKE_C_STANDARD 99)

add_compile_options(
  -Xdiag=2 -Xquit=0
)

include_directories(
  ${VDSUITE_USER_INCLUDE_DIRECTORIES}
  ${VDSUITE_STANDARD_INCLUDE_DIRECTORIES}
  include
)

link_directories(
  ${VDSUITE_LIBRARY_DIRECTORIES}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(${PROJECT_NAME}
  main.c
)

target_link_libraries(${PROJECT_NAME}
  glfw3
  SceLibMonoBridge_stub
  pib
  libScePiglet_stub
  SceShaccCg_stub
  SceCtrl_stub
  SceRtcUser_stub
  SceTouch_stub
  SceLibKernel_stub
  SceThreadmgr_stub
  SceModulemgr_stub
  SceSysmodule_stub
  SceIofilemgr_stub
  SceGxm_stub
  SceSharedFb_stub
  SceGxmInternalForVsh_stub
  SceDisplay_stub
  taihen_stub
)

VDSuiteCreateSfo(${PROJECT_NAME}.sfo param.yml)

VDSuitePackage(${PROJECT_NAME}.vpk
  FILES
    $<TARGET_FILE:${PROJECT_NAME}> eboot.bin
    ${PROJECT_NAME}.sfo sce_sys/param.sfo
)
