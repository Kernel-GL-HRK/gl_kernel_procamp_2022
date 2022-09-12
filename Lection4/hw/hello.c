// SPDX-License-Identifier: GPL-2.0

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/sysfs.h>

#include "helpers.h"



static struct kobject *hello_kobject;

// Timestamp storages.
static struct timespec64 past_relative = {
	.tv_sec = 0,
	.tv_nsec = 0
};
static struct timespec64 past_absolute = {
	.tv_sec = 0,
	.tv_nsec = 0
};

// A helper structure for forming pretty fraction output.
static struct prefix_t prefix;



static ssize_t on_relative_sysfs_show(struct kobject *object, struct kobj_attribute *attribute, char *buffer)
{
	ssize_t count = 0;
	struct timespec64 now, elapsed;
	long usec = 0;

	ktime_get_raw_ts64(&now);
	elapsed = timespec64_sub(now, past_relative);
	usec = elapsed.tv_nsec / 1000;
	count = sprintf(buffer, "%lld.%s%ld\n", elapsed.tv_sec, update_prefix(&prefix, usec), normalize_fraction(usec));
	past_relative = now;

	return count;
}

static ssize_t on_absolute_sysfs_show(struct kobject *object, struct kobj_attribute *attribute, char *buffer)
{
	const long usec = past_absolute.tv_nsec / 1000;
	const ssize_t count = sprintf(buffer, "%lld.%s%ld\n", past_absolute.tv_sec, update_prefix(&prefix, usec), normalize_fraction(usec));

	ktime_get_raw_ts64(&past_absolute);

	return count;
}

static struct kobj_attribute relative_attribute =
	__ATTR(relative, 0664, on_relative_sysfs_show, NULL);

static struct kobj_attribute absolute_attribute =
	__ATTR(absolute, 0664, on_absolute_sysfs_show, NULL);



int hello_init(void)
{
	int result = 0;

	hello_kobject = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_kobject)
		return -ENOMEM;

	result = sysfs_create_file(hello_kobject, &relative_attribute.attr);
	if (result)
		goto error;
	result = sysfs_create_file(hello_kobject, &absolute_attribute.attr);
	if (result)
		goto error;

	result = init_prefix(&prefix, 6/*micro*/, vmalloc);
	if (result)
		goto error;

	pr_info("Hello! Init.\n");

	return 0;

error:
	kobject_put(hello_kobject);
	return result;
}

void hello_exit(void)
{
	kobject_del(hello_kobject);

	release_prefix(&prefix, vfree);

	pr_info("Hello! Cleanup.\n");
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Sergii Piatakov <sergii.piatakov@globallogic.com>");
MODULE_DESCRIPTION("Simple module for educational purposes.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
