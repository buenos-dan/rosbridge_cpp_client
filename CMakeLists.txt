cmake_minimum_required(VERSION 3.21)
set(CMAKE_TOOLCHAIN_FILE /home/buenos/buenos/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(mininav-ros-bridge
    VERSION   0.0.1
    LANGUAGES CXX)

find_package(protobuf CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(ixwebsocket CONFIG REQUIRED)
find_package(gflags CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
# 编译 proto/ros_msg.proto
set(proto_source ${CMAKE_CURRENT_SOURCE_DIR}/proto/ros_msg.proto)
set(proto_outdir ${CMAKE_CURRENT_BINARY_DIR}/proto)
set(compiled_proto ${proto_outdir}/ros_msg.pb.cc)
if (${proto_source} IS_NEWER_THAN ${compiled_proto})
      if (NOT EXISTS ${proto_outdir})
            file(MAKE_DIRECTORY ${proto_outdir})
      endif ()
      execute_process(
            COMMAND
                  ${Protobuf_PROTOC_EXECUTABLE}
                  "--proto_path=${CMAKE_CURRENT_LIST_DIR}/proto"
                  "--cpp_out=${proto_outdir}"
                  ${proto_source}
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            RESULT_VARIABLE protoc_return_value)
      if (NOT ${protoc_return_value} STREQUAL "0")
            message(FATAL_ERROR "Failed compile: ${proto}. See details above.")
      endif ()
endif ()

add_library(ros-bridge
    src/rosbridge_helper.cc
    src/rosbridge_man.cc
    src/rosbridge_ws.cc
    src/thread_tool.cc
    ${proto_outdir}/ros_msg.pb.cc)

target_include_directories(ros-bridge
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${proto_outdir})

target_link_libraries(ros-bridge
    PUBLIC
        ixwebsocket::ixwebsocket
        gflags::gflags
        glog::glog
        protobuf::libprotobuf
)


# test
add_executable(test_send_string test/test_send_string.cc)
target_link_libraries(test_send_string
      ros-bridge
)