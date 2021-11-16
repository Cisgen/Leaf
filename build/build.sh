#!/bin/sh

CURR_DIR=`pwd`

cmake ../src/ -DCMAKE_BUILD_TYPE=Debug
make -j4

