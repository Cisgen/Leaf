#!/bin/sh

curuser=`whoami`

if [ $curuser = "root" ];then
 echo "root Operation is not allowed!!"
 exit -1
fi

../bin/zone_svr  --id=5.15.61.1 --business-id=10  --conf-file ../cfg/zone_svr.xml --pid-file=./zone_svr_5_15.pid stop 

