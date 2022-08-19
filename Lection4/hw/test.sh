#!/bin/sh

dmesg -c
lsmod
insmod hello.ko

cat /sys/kernel/hello/relative
sleep .1
cat /sys/kernel/hello/relative
sleep .2
cat /sys/kernel/hello/relative
sleep .5
cat /sys/kernel/hello/relative
sleep 1
cat /sys/kernel/hello/relative
sleep 2
cat /sys/kernel/hello/relative
sleep 5
cat /sys/kernel/hello/relative

sleep .1
cat /sys/kernel/hello/absolute
sleep .2
cat /sys/kernel/hello/absolute
sleep .5
cat /sys/kernel/hello/absolute
sleep 1
cat /sys/kernel/hello/absolute
sleep 2
cat /sys/kernel/hello/absolute
sleep 5
cat /sys/kernel/hello/absolute
cat /sys/kernel/hello/absolute

rmmod hello
lsmod
dmesg -c
