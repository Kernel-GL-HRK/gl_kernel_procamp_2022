// SPDX-License-Identifier: GPL-2.0+
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int f_param = 1;
static int s_param = 1;

module_param(f_param, int, 0660);
module_param(s_param, int, 0660);


static int __init task_1_init(void)
{
	pr_info("Hello, World! This is simple kernel module\n\n");
	pr_info("First param: %d\n", f_param);
	pr_info("Second param: %d\n", s_param);
	pr_info("Addition result: %d\n", f_param + s_param);
	pr_info("Substraction result: %d\n", f_param - s_param);
	pr_info("Multiplication result: %d\n", f_param * s_param);
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
