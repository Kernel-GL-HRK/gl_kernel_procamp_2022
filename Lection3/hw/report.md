Report
------

Implement a simple module, that prints some debug output in the probe function:
```
	pr_info("Hello! Init.\n");
```


Load the module and check output using `dmesg`:
```
# dmesg -c

# lsmod 
Module                  Size  Used by    Tainted: G

# insmod hello.ko
# lsmod 
Module                  Size  Used by    Tainted: G
hello                  16384  0

# dmesg -c
Hello! Init.
```


Examine `/sys/module/<modulename>/sections/`:
```
# ls -la /sys/module/hello/sections/
total 0
drwxr-xr-x    2 root     root             0 Aug 16 20:17 .
drwxr-xr-x    5 root     root             0 Aug 16 20:17 ..
-r--------    1 root     root            19 Aug 16 20:17 .gnu.linkonce.this_module
-r--------    1 root     root            19 Aug 16 20:17 .note.Linux
-r--------    1 root     root            19 Aug 16 20:17 .note.gnu.build-id
-r--------    1 root     root            19 Aug 16 20:17 .rodata.str1.1
-r--------    1 root     root            19 Aug 16 20:17 .strtab
-r--------    1 root     root            19 Aug 16 20:17 .symtab
-r--------    1 root     root            19 Aug 16 20:17 .text.unlikely
```


Check `/sys/kernel/debug/dynamic_debug`:
```
# ls -l /sys/module/debug/dynamic_debug
ls: /sys/module/debug/dynamic_debug: No such file or directory
```


Rebuild Kernel with enabled `CONFIG_DYNAMIC_DEBUG` option:
```
$ make qemu_x86_64_defconfig

$ make linux-menuconfig
// Select the following options:
CONFIG_DYNAMIC_DEBUG=y
CONFIG_DEBUG_FS=y

$ make linux-update-defconfig

$ make
```


Added aditional `pr_debug` call in the `probe` function:
```
	pr_info("Hello! Init.\n");
	pr_debug("Hello! Debug message.\n");
```


Check the `dmesg` output with the `#define DEBUG`:
```
# insmod hello.ko 
Hello! Init.

# dmesg -c
Hello! Init.
Hello! Debug message.
```
Note: without the `DEBUG` definition the debug message isn't printed,
      but messages with the `info` severity are still printed.


Mount the `debugfs` and check the `dynamic_debug` directory again:
```
# mount -t debugfs none /sys/kernel/debug

# mount | grep debugfs
none on /sys/kernel/debug type debugfs (rw,relatime)

# ls -la /sys/kernel/debug/dynamic_debug/
total 0
drwxr-xr-x    2 root     root             0 Aug 18 19:16 .
drwx------   20 root     root             0 Aug 18 19:16 ..
-rw-r--r--    1 root     root             0 Aug 18 19:16 control

# cat /sys/kernel/debug/dynamic_debug/control | grep hello
/home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:15 [hello]hello_init =_ "Hello! Debug message.\012"
```
Note: instead of manual mounting it can be automated by adding corresponding line in the `fstab`.


Add additional output to the `exit` function:
```
#define DEBUG

	...

	pr_info("Hello! Cleanup.\n");
	pr_debug("Hello! Finalized debug message.\n");
```


And check the settings again:
```
# cat /sys/kernel/debug/dynamic_debug/control | grep hello
/home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:25 [hello]hello_exit =p "Hello! Finalized debug message.\012"
/home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:17 [hello]hello_init =p "Hello! Initial debug message.\012"
```


Add additional flags `m` and `f` to the finalization message, check settings and exit:
```
# echo "file hello.c line 25 +m" > /sys/kernel/debug/dynamic_debug/control
# echo "file hello.c line 25 +f" > /sys/kernel/debug/dynamic_debug/control

# cat /sys/kernel/debug/dynamic_debug/control | grep hello
/home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:25 [hello]hello_exit =pmf "Hello! Finalized debug message.\012"
/home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:17 [hello]hello_init =p "Hello! Initial debug message.\012"

# rmmod hello
Hello! Cleanup.

# dmesg -c
Hello! Init.
Hello! Initial debug message.
Hello! Cleanup.
hello:hello_exit: Hello! Finalized debug message.
```
Note: the `m` and `f` flags add the module and function names to the debug output.


Replace the debug message by the `BUG_ON` macro in the `probe` function, load and see result:
```
# insmod hello.ko 
------------[ cut here ]------------
kernel BUG at /home/spiatakov/work/cources/kernel/module/Lection3/hw/hello.c:16!
invalid opcode: 0000 [#1] SMP NOPTI
CPU: 0 PID: 122 Comm: insmod Tainted: G           O      5.15.18 #3
Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS rel-1.15.0-0-g2dd4b9b3f840-prebuilt.qemu.org 04/01/2014
RIP: 0010:init_module+0x0/0x2 [hello]
Code: Unable to access opcode bytes at RIP 0xffffffffc02dbfd6.
RSP: 0018:ffff9ec5001d7cf0 EFLAGS: 00000246
RAX: 0000000000000000 RBX: 0000000000000000 RCX: 00000000000008b2
RDX: 00000000000008b1 RSI: 0000000000000cc0 RDI: ffffffffc02dc000
RBP: ffff9ec5001d7d50 R08: ffff994541e182c0 R09: fffff6388007e488
R10: 0000000000000002 R11: 00000000000003ca R12: ffffffffc02dc000
R13: ffff994541e182c0 R14: ffff9ec5001d7e78 R15: ffffffffc02de040
FS:  00007f27843976a0(0000) GS:ffff994546600000(0000) knlGS:0000000000000000
CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
CR2: ffffffffc02dbfd6 CR3: 0000000001f04000 CR4: 00000000000006f0
Call Trace:
 <TASK>
 ? do_one_initcall+0x3c/0x160
 ? __vunmap+0x1e6/0x280
 ? kmem_cache_alloc+0x20/0x160
 do_init_module+0x58/0x250
 load_module+0x24cf/0x2790
 __do_sys_finit_module+0xb9/0x100
 __x64_sys_finit_module+0x15/0x20
 do_syscall_64+0x43/0x90
 entry_SYSCALL_64_after_hwframe+0x44/0xae
RIP: 0033:0x7f2784319d77
Code: 48 89 57 30 48 8b 04 24 48 89 47 38 e9 60 a2 02 00 48 89 f8 48 89 f7 48 89 d6 48 89 ca 4d 89 c2 4d 89 c8 4c 8b 4c 24 08 0f 05 <48> 3d 01 f0 ff ff 0f 83 39 fd ff ff c3 48 c7 c6 01 00 00 00 e9 a1
RSP: 002b:00007ffd91789068 EFLAGS: 00000246 ORIG_RAX: 0000000000000139
RAX: ffffffffffffffda RBX: 000055c17b1f4000 RCX: 00007f2784319d77
RDX: 0000000000000000 RSI: 000055c17b2a1010 RDI: 0000000000000003
RBP: 0000000000000003 R08: 0000000000000000 R09: 0000000000000000
R10: 000055c17b2a1010 R11: 0000000000000246 R12: 000055c17b2a1010
R13: 0000000000000000 R14: 00007ffd9178af4a R15: 0000000000000000
 </TASK>
Modules linked in: hello(O+)
---[ end trace b660410113a3babb ]---
RIP: 0010:init_module+0x0/0x2 [hello]
Code: Unable to access opcode bytes at RIP 0xffffffffc02dbfd6.
RSP: 0018:ffff9ec5001d7cf0 EFLAGS: 00000246
RAX: 0000000000000000 RBX: 0000000000000000 RCX: 00000000000008b2
RDX: 00000000000008b1 RSI: 0000000000000cc0 RDI: ffffffffc02dc000
RBP: ffff9ec5001d7d50 R08: ffff994541e182c0 R09: fffff6388007e488
R10: 0000000000000002 R11: 00000000000003ca R12: ffffffffc02dc000
R13: ffff994541e182c0 R14: ffff9ec5001d7e78 R15: ffffffffc02de040
FS:  00007f27843976a0(0000) GS:ffff994546600000(0000) knlGS:0000000000000000
CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
CR2: ffffffffc02dbfd6 CR3: 0000000001f04000 CR4: 00000000000006f0
Segmentation fault

```
Note: the issue is caused by a condition in the line 16 of the file: `hello.c`

Note: after unsuccessfull `insmod` the module can't be unloaded:
```
rmmod: can't unload module 'hello': Device or resource busy
```
