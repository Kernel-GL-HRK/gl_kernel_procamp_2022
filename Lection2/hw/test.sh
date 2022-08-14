#!/bin/sh

dmesg -c
lsmod
insmod hello.ko

echo 'Hello' > /sys/kernel/hello/list
echo 'from' > /sys/kernel/hello/list
echo 'me' > /sys/kernel/hello/list
echo '!' > /sys/kernel/hello/list

cat /sys/kernel/hello/list

rmmod hello
lsmod
dmesg -c
