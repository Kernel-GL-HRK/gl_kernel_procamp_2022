#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#define DEBUG

struct list_node {
	char *data;
	struct list_head list;
};

static struct list_head list;

static ssize_t list_show(struct kobject *object, struct kobj_attribute *attribute, char *buffer)
{
	struct list_node *list_head = NULL;
	struct list_head *it = NULL;
	ssize_t count = 0;

	list_for_each(it, &list) {
		list_head = list_entry(it, struct list_node, list);
		sprintf(buffer + count, "%s", list_head->data);
		count += strlen(list_head->data);
	}

	return count;
}

static ssize_t list_store(struct kobject *object, struct kobj_attribute *attribute, const char *buffer, size_t count)
{
	struct list_node *list_head = kmalloc(sizeof(struct list_node), GFP_KERNEL);

	list_head->data = kmalloc(count + 1/*for null terminator*/, GFP_KERNEL);
	memcpy(list_head->data, buffer, count);
	list_head->data[count] = '\0';

	list_add_tail(&list_head->list, &list);

	return count;
}

static struct kobj_attribute list_attribute =
	__ATTR(list, 0664, list_show, list_store);

static struct kobject *hello_kobject;



int hello_init(void)
{
	int result = 0;

	INIT_LIST_HEAD(&list);

	hello_kobject = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_kobject)
		return -ENOMEM;

	result = sysfs_create_file(hello_kobject, &list_attribute.attr);
	if (result)
		kobject_put(hello_kobject);

	pr_info("Module inited\n");
    pr_debug("Hello! I`m pr_debug");

	return result;
}

void hello_exit(void)
{
	struct list_node *list_head = NULL;
	struct list_head *it = NULL;
	struct list_head *next = NULL;

	list_for_each_safe(it, next, &list) {
		list_head = list_entry(it, struct list_node, list);
		list_del(it);
		kfree(list_head->data);
		kfree(list_head);
	}
	list_del(&list);

	kobject_del(hello_kobject);

	pr_info("Module removed, list deleted\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytro Maladyka");