#!/bin/bash
# _*_ coding:utf-8 _*_

#规定的版本号
RULE_VERSION_NUM="2014q1"

CC_VERSION_INFO=`arm-none-eabi-gcc -v 2>&1 | sed -n '2p'`

ret=$(echo $CC_VERSION_INFO | grep "${RULE_VERSION_NUM}")

if [ "$ret" != "" ]; then

#监测通过，正常退出
exit 0
fi



echo "Stop compiling!"
#结束编译
kill -9 $PPID

parentid=`ps -ef |grep -v PID | awk '{print $2}' | grep $PPID`
while [ "$parentid" != "" ]
do 
    echo "Stop compiling!"
    echo $parentid
    sleep 1
    parentid=`ps -ef |grep -v PID | awk '{print $2}' | grep $PPID`
done
echo "Stop compiling complete!"
find . -name "*.o"|xargs rm -f
rm *.hex *.elf 2>/dev/null
echo "GCC version does not match,please use ${RULE_VERSION_NUM} version."

exit -1
