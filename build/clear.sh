#!/bin/sh

cmake ../src/

CURR_DIR=`pwd`

make clean 
rm Makefile Leaf CMakeFiles  cmake_install.cmake CMakeCache.txt -rf
