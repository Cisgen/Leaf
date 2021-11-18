project(leaf_svr)

cmake_minimum_required(VERSION 2.6)

include(${CMAKE_SOURCE_DIR}/CMakeComm.txt)

set(EXECUTABLE_OUTPUT_PATH "${home_dir}/runenv/Leaf/bin")

#获取源文件
add_src_subdirs(source_files  "./Frame" "./Macro" "./Network" "./Log" "./Logic")

#添加配置文件描述
#set(source_files "${res_dir}/src/zone_svr_conf_desc.c" ${source_files})
message(STATUS "source_files:${source_files}")

add_executable(leaf_svr ${source_files}) 

set(CMAKE_EXE_LINKER_FLAGS "${lib_link_flag}")
target_link_libraries(leaf_svr ${lib_link}) 
