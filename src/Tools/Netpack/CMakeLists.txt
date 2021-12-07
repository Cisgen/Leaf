#设置工程名为Netpack
project(Netpack)

#设置cmake最低版本需求
cmake_minimum_required(VERSION 2.6)

#包含CMake源路径下的CMakeComm.txt文件
include(${CMAKE_SOURCE_DIR}/CMakeComm.txt)

#设置输出路径
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/../Tools/Netpack/bin")

#获取所有源文件
add_src_subdirs(source_files "./")

#生成应用程序
add_executable(Netpack ${source_files})

set(CMAKE_EXE_LINKER_FLAGS "${lib_link_flag}")
