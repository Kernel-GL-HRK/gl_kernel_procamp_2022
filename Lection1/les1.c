//Hello world kernel module:
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

static int a,b;

module_param(a, int, S_IRUSR);
module_param(b, int, S_IRUSR);

int init_module(void)
{
	printk(KERN_INFO "Hi world!\n");

	printk(KERN_INFO "a is %d\n", a);
    printk(KERN_INFO "b is %d\n", b);
	printk(KERN_INFO "a + b is %d\n", a + b);
	printk(KERN_INFO "a - b is %d\n", a - b);
	printk(KERN_INFO "a * b is %d\n", a * b);
	return 0;
}

void cleanup_module(void) {
	printk(KERN_INFO "Bye world!\n");
}

MODULE_LICENSE("GPL");

