Home task 3

Implement kernel mode.
les3.ko


Add messages from module load using pr_info().
pr_info("Module inited\n");

Run module.
# insmod les3.ko
les3: loading out-of-tree module taints kernel.
Module inited


Check messages using dmesg.
dmesg
les3: loading out-of-tree module taints kernel.
Module inited

Examine /sys/module//sections/
# cd /sys/module/les3/sections/
# ls -la
total 0
drwxr-xr-x    2 root     root             0 Sep  1 12:34 .
drwxr-xr-x    5 root     root             0 Sep  1 12:34 ..
-r--------    1 root     root            19 Sep  1 12:38 .bss
-r--------    1 root     root            19 Sep  1 12:38 .data
-r--------    1 root     root            19 Sep  1 12:38 .gnu.linkonce.this_module
-r--------    1 root     root            19 Sep  1 12:38 .note.Linux
-r--------    1 root     root            19 Sep  1 12:38 .note.gnu.build-id
-r--------    1 root     root            19 Sep  1 12:38 .rodata.str1.1
-r--------    1 root     root            19 Sep  1 12:38 .rodata.str1.8
-r--------    1 root     root            19 Sep  1 12:38 .strtab
-r--------    1 root     root            19 Sep  1 12:38 .symtab
-r--------    1 root     root            19 Sep  1 12:38 .text
-r--------    1 root     root            19 Sep  1 12:38 .text.unlikely


Check /sys/kernel/debug/dynamic_debug
# cd /sys/kernel/
boot_params/     hello/           mm/              slab/
cgroup/          irq/             reboot/          software_nodes/


Compile kernel with option CONFIG_DYNAMIC_DEBUG. Rebuild kernel.

Add call pr_debug() to your module.

Check messages for with and without macro #define DEBUG

Try options p, f, m for debug messages.

Add macro BUG_ON() instead of printing.

Load module.

Find place of code where BUG_ON called.

Add log files and output from dmesg in text format to repository.