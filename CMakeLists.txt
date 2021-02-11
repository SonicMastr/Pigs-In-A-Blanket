cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

include("$ENV{DOLCESDK}/share/dolce.cmake" REQUIRED)

project(pib)

add_executable(${PROJECT_NAME}
  src/pib.c
  src/hooks.c
  src/patches.c
  src/sha1.c
  src/shacccgpatch.c
  src/sysmodepatch.c
)

link_directories(${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -Wno-incompatible-pointer-types -Wno-pointer-sign -O3 -nostartfiles -nostdlib -DVITA -D__VITA__ -DPIB_PRX")

set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS
  -nostartfiles
)

include_directories(
  include
)

target_link_libraries(${PROJECT_NAME}
  SceLibc_stub_weak
  SceGxm_stub_weak
  SceDisplay_stub_weak
  SceThreadmgr_stub_weak
  SceLibKernel_stub_weak
  libScePiglet_stub_weak
  taihen_stub_weak
  SceShaccCg_stub_weak
  SceAppMgr_stub_weak
  SceSharedFb_stub_weak
  SceGxmInternalForVsh_stub_weak
  SceGxmInternal_stub_weak
)

dolce_create_self(${PROJECT_NAME}.suprx ${PROJECT_NAME}
  CONFIG ${CMAKE_SOURCE_DIR}/pib.yml
  UNSAFE
)

dolce_create_stubs(${PROJECT_NAME}_stubs 
  ${PROJECT_NAME} 
  ${CMAKE_SOURCE_DIR}/pib.yml
  LIB pib_stub
)