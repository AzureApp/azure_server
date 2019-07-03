cmake_minimum_required(VERSION 3.4.1)
project(libazure)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE libazure_HDR_FILES "libazure/*.h")
file(GLOB_RECURSE libazure_SRC_FILES "libazure/*.cc")

file(GLOB_RECURSE azure_daemon_HDR_FILES "daemon/*.h")
file(GLOB_RECURSE azure_daemon_SRC_FILES "daemon/*.cc")

if(WIN32)
set(PROTOC_EXECUTABLE "C:\\Protobuf\\protoc.exe" CACHE STRING "Path to protoc executable")
set(GRPC_CPP_PLUGIN "C:\\Protobuf\\grpc_cpp_plugin.exe" CACHE STRING "Path to grpc_cpp_plugin executable")
else()
set(PROTOC_EXECUTABLE "/usr/bin/protoc" CACHE STRING "Path to protoc executable")
set(GRPC_CPP_PLUGIN "/usr/bin/grpc_cpp_plugin" CACHE STRING "Path to grpc_cpp_plugin executable")
endif()

if(WIN32)
add_definitions(-D_WIN32_WINNT=0x0601)
endif()

if(NOT EXISTS ${PROTOC_EXECUTABLE})
message(FATAL "protoc compiler not found at ${PROTOC_EXECUTABLE}")
endif()

if(NOT EXISTS ${GRPC_CPP_PLUGIN})
message(FATAL "grpc cpp plugin not found at ${GRPC_CPP_PLUGIN}")
endif()

# Use lite runtime
set(gRPC_USE_PROTO_LITE ON CACHE BOOL "Use lite protobuf")

if(gRPC_USE_PROTO_LITE)
add_definitions(-DGRPC_USE_PROTO_LITE)
endif()

set(GRPC_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc" CACHE BOOL "Directory of grpc source code")

set(GRPC_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/grpc)
file(MAKE_DIRECTORY ${GRPC_BUILD_DIR})

add_subdirectory(${GRPC_SRC_DIR} ${GRPC_BUILD_DIR})

set(PROTO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/proto/src)

# output directory for generated proto source files
set(GRPC_PROTO_GENS_DIR ${CMAKE_BINARY_DIR}/gens)
file(MAKE_DIRECTORY ${GRPC_PROTO_GENS_DIR})
include_directories(${GRPC_PROTO_GENS_DIR})

# generate source files from proto definitions
function(protobuf_grpc_generate_cpp SRC_FILES HDR_FILES INCLUDE_ROOT)
  if(NOT ARGN)
    message(SEND_ERROR "Error: protobuf_grpc_generate_cpp() called without any proto files")
    return()
  endif()

  set(${SRC_FILES})
  set(${HDR_FILES})
  set(PROTOBUF_INCLUDE_PATH -I ${INCLUDE_ROOT})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    file(RELATIVE_PATH REL_FIL ${INCLUDE_ROOT} ${ABS_FIL})
    get_filename_component(REL_DIR ${REL_FIL} DIRECTORY)
    set(RELFIL_WE "${REL_DIR}/${FIL_WE}")

    message(WARNING "ARGN: ${ARGN}\n FIL: ${FIL}\n ABS_FIL: ${ABS_FIL}\n REL_FIL: ${REL_FIL}\n PROTOBUF_INCLUDE_PATH: ${PROTOBUF_INCLUDE_PATH}")

    list(APPEND ${SRC_FILES} "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.cc")
    list(APPEND ${HDR_FILES} "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.h")
    list(APPEND ${SRC_FILES} "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.cc")
    list(APPEND ${HDR_FILES} "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.h")

    add_custom_command(
      OUTPUT "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.cc"
             "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.grpc.pb.h"
             "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.cc"
             "${GRPC_PROTO_GENS_DIR}/${RELFIL_WE}.pb.h"
      COMMAND ${PROTOC_EXECUTABLE}
      ARGS --grpc_out=${GRPC_PROTO_GENS_DIR}
        --cpp_out=${GRPC_PROTO_GENS_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        ${PROTOBUF_INCLUDE_PATH}
        ${REL_FIL}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${PROTOC_EXECUTABLE} ${GRPC_CPP_PLUGIN} ${ABS_FIL} )
  endforeach()

  set_source_files_properties(${${SRC_FILES}} ${${HDR_FILES}} PROPERTIES GENERATED TRUE)
  set(${SRC_FILES} ${${SRC_FILES}} PARENT_SCOPE)
  set(${HDR_FILES} ${${HDR_FILES}} PARENT_SCOPE)
endfunction()

message(WARNING "PROTO_SRC_DIR ${PROTO_SRC_DIR}")

file(GLOB_RECURSE PROTO_FILES "${PROTO_SRC_DIR}/*.proto")
message(WARNING "PROTO_FILES ${PROTO_FILES}")

protobuf_grpc_generate_cpp(
    AZURE_PROTO_SRCS AZURE_PROTO_HDRS ${PROTO_SRC_DIR} ${PROTO_FILES})

add_library(azure_proto_lib
  STATIC ${AZURE_PROTO_HDRS} ${AZURE_PROTO_SRCS})

target_link_libraries(azure_proto_lib
  grpc++_unsecure
  libprotobuf-lite)

add_library(libazure STATIC ${libazure_HDR_FILES} ${libazure_SRC_FILES})
target_include_directories(libazure PUBLIC "." ${GRPC_PROTO_GENS_DIR})
target_link_libraries(libazure azure_proto_lib)

add_executable(azured ${azure_daemon_HDR_FILES} ${azure_daemon_SRC_FILES})
target_link_libraries(azured libazure)