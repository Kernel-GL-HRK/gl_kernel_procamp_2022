# Home task 3

Implement kernel mode.

Add messages from module load using pr_info().

Run module.

Check messages using dmesg.

Examine /sys/module/<modulename>/sections/

Check /sys/kernel/debug/dynamic_debug

Compile kernel with option CONFIG_DYNAMIC_DEBUG.
Rebuild kernel.

Add call pr_debug() to your module.

Check messages for with and without macro
#define DEBUG

Try options p, f, m for debug messages.

Add macro BUG_ON() instead of printing.

Load module.

Find place of code where BUG_ON called.

Add log files and output from dmesg in text format to repository.
