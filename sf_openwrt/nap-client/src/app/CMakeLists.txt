cmake_minimum_required(VERSION 3.5.1)

project(SigfoxNAP C CXX)

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-stack-protector")
else()
  add_definitions(-D_WIN32_WINNT=0x600)
endif()

add_definitions(-DMTK_BUILDROOT_COMPAT)
add_definitions(-DCONFIG_RALINK_MT7620)
add_definitions(-D_GNU_SOURCE)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

# This branch assumes that gRPC and all its dependencies are already installed
# on this system, so they can be located by find_package().

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

foreach(_proto 
    google-protobuf configuration
    msg-frame msg-location msg-heartbeat msg-configuration msg-service
    cmd-configuration cmd-downlink cmd-upgrade cmd-service
    )

  # Proto file
  get_filename_component(hw_proto "protos/${_proto}.proto" ABSOLUTE)
  get_filename_component(hw_proto_path "${hw_proto}" PATH)

  # Generated sources
  set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.pb.cc")
  set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.pb.h")
  set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.grpc.pb.cc")
  set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto}.grpc.pb.h")
  list(APPEND proto_list ${hw_proto_srcs} ${hw_grpc_srcs})
  add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")
endforeach()

# Include generated *.pb.h files
include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}" 
  ../grpc-1.34/third_party/boringssl-with-bazel/src/include 
  "${LINUX_DRIVER_INCLUDE_DIR}"
  "${LINUX_USER_INCLUDE_DIR}"
  "${LINUX_NL_TINY_INCLUDE_DIR}"
)

# Target nap-client
add_executable(nap-client
  ${proto_list}
  main.cc
  log.cc
  event.cc
  timer.cc
  cli.cc
  ipc-fifo.cc
  main-client.cc
  config-manager.cc
  config-handler.cc
  auth-handler.cc
  auth-json.cc
  grpc-handler.cc
  rms-handler.cc
  rms-json.cc
  fwup-handler.cc
  curl-handler.cc
  json-manager.cc
  cert-manager.cc
  at-handler.cc
  sigfox-handler.cc
  sigfox-json.cc
  watchdog-handler.cc
  ep-manager.cc
  monitor.cc
  utils.cc
  json/cJSON.c
  switch/switch_netlink.c
)
target_link_libraries(nap-client
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  curl
  fwmgr
  nvram
  nl-tiny
  m
)

# Target nap-server
#add_executable(nap-server
#  ${proto_list}
#  log.cc
#  event.cc
#  config-manager.cc
#  timer.cc
#  test-server/server-main.cc
#  test-server/server-proc.cc
#  test-server/server-handler.cc
#  test-server/server-cli.cc
#)
#target_link_libraries(nap-server
#  ${_REFLECTION}
#  ${_GRPC_GRPCPP}
#  ${_PROTOBUF_LIBPROTOBUF}
#)
#target_include_directories(nap-server PUBLIC ${CMAKE_SOURCE_DIR})

