#include <linux/module.h>
#include <linux/kernel.h>

void cleanup_module(void)
{
    printk(KERN_INFO "Good luck!\n");
}

int init_module(void)
{
    printk(KERN_INFO "Hello from module!\n");
    return 0;
}

MODULE_LICENSE("Proprietary");