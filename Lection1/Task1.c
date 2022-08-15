// SPDX-License-Identifier: GPL-2.0+
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init task_1_init(void)
{
	pr_info("Hello, World!\n");
	return 0;
}

static void __exit task_1_exit(void)
{
	pr_info("Goodbye, World!\n");
}

module_init(task_1_init);
module_exit(task_1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vladyslav Andrishko");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");
