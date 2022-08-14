// SPDX-License-Identifier: GPL-2.0

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>



struct list_entry {
	char *payload;
	struct list_head list;
};

static struct list_head list;



static ssize_t on_sysfs_show(struct kobject *object, struct kobj_attribute *attribute, char *buffer)
{
	struct list_entry *entry = NULL;
	struct list_head *it = NULL;
	ssize_t count = 0;

	list_for_each(it, &list) {
		entry = list_entry(it, struct list_entry, list);
		sprintf(buffer + count, "%s", entry->payload);
		count += strlen(entry->payload);
	}

	return count;
}

static ssize_t on_sysfs_store(struct kobject *object, struct kobj_attribute *attribute, const char *buffer, size_t count)
{
	struct list_entry *entry = kmalloc(sizeof(struct list_entry), GFP_KERNEL);

	entry->payload = kmalloc(count + 1/*for null terminator*/, GFP_KERNEL);
	memcpy(entry->payload, buffer, count);
	entry->payload[count] = '\0';

	list_add_tail(&entry->list, &list);

	return count;
}

static struct kobj_attribute list_attribute =
	__ATTR(list, 0664, on_sysfs_show, on_sysfs_store);



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

	pr_info("Hello! Init.\n");

	return result;
}

void hello_exit(void)
{
	struct list_entry *entry = NULL;
	struct list_head *it = NULL;
	struct list_head *next = NULL;

	list_for_each_safe(it, next, &list) {
		entry = list_entry(it, struct list_entry, list);
		list_del(it);
		kfree(entry->payload);
		kfree(entry);
	}
	list_del(&list);

	kobject_del(hello_kobject);

	pr_info("Hello! Cleanup.\n");
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Sergii Piatakov <sergii.piatakov@globallogic.com>");
MODULE_DESCRIPTION("Simple module for educational purposes.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
