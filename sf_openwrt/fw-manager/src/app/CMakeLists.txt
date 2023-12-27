cmake_minimum_required(VERSION 3.5.1)
project(fw-manager C CXX)
find_package(OpenSSL REQUIRED)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(BASE_SOURCES
  fw_dbg.c
  fw_merger.c
  fw_splitter.c
  fw_crc32.c
  fw_crypto.c
)

add_library(fwmgr
  STATIC
  ${BASE_SOURCES}
  fw_api.c
)

add_executable(fw-manager
  ${BASE_SOURCES}
  fw_main.c
)

target_link_libraries(fw-manager
  crypto
  ssl
)
