#!/bin/sh

curuser=`whoami`

if [ $curuser = "root" ];then
 echo "root Operation is not allowed!!"
 exit -1
fi

../bin/leaf_svr leaf_svr -D start
