#!/bin/sh

dmesg -c
lsmod
insmod hello.ko

cat /proc/hello_iface/read_num
cat /proc/hello_iface/write_num
cat /proc/hello_iface/module_owner

echo 'Hello' > /sys/kernel/hello/list
echo 'from' > /sys/kernel/hello/list
echo 'me' > /sys/kernel/hello/list
echo '!' > /sys/kernel/hello/list

cat /sys/kernel/hello/list

cat /proc/hello_iface/read_num
cat /proc/hello_iface/write_num
cat /proc/hello_iface/module_owner

rmmod hello
lsmod
dmesg -c
