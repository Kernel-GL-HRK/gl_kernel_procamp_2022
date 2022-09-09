// SPDX-License-Identifier: GPL-2.0

#define DEBUG

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>



int hello_init(void)
{
	int result = 0;

	BUG_ON(1);

	return result;
}

void hello_exit(void)
{
	pr_info("Hello! Cleanup.\n");
	pr_debug("Hello! Finalized debug message.\n");

	
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Sergii Piatakov <sergii.piatakov@globallogic.com>");
MODULE_DESCRIPTION("Simple module for educational purposes.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
