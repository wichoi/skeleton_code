cmake_minimum_required(VERSION 3.5.1)
project(atcmd C CXX)
find_package(Threads REQUIRED)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Target atcmd
add_executable(atcmd
  main.cc
  log.cc
  event.cc
  timer.cc
  utils.cc
  cli.cc
  config-manager.cc
  config-handler.cc
  main-client.cc
  at-handler.cc
  ipc-console.cc
  ipc-web.cc
  modem-handler.cc
  gps-handler.cc
  json-manager.cc
  json/cJSON.c
)

target_link_libraries(atcmd
  pthread
)
