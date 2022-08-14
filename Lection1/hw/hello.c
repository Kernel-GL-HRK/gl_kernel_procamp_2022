// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>



static int a;
static int b;



module_param(a, int, 0660);
module_param(b, int, 0660);
MODULE_PARM_DESC(a, "The first important variable.");
MODULE_PARM_DESC(b, "The second important variable.");



int init_module(void)
{
	pr_info("Hello! Init.\n");

	pr_info("a: %d.\n", a);
	pr_info("b: %d.\n", b);
	pr_info("a + b = %d.\n", a + b);
	pr_info("a - b = %d.\n", a - b);
	pr_info("a * b = %d.\n", a * b);

	return 0;
}



void cleanup_module(void)
{
	pr_info("Hello! Cleanup...\n");
}



MODULE_LICENSE("GPL");
