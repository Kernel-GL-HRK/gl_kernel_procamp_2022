// SPDX-License-Identifier: GPL-2.0

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/path.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/types.h>



struct list_entry {
	char *payload;
	struct list_head list;
};

static struct list_head list;



struct sysfs_ops_stat {
	unsigned int reads;
	unsigned int writes;
};

static struct sysfs_ops_stat stats = {0, 0};



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

	stats.reads++;

	return count;
}

static ssize_t on_sysfs_store(struct kobject *object, struct kobj_attribute *attribute, const char *buffer, size_t count)
{
	struct list_entry *entry = kmalloc(sizeof(struct list_entry), GFP_KERNEL);

	entry->payload = kmalloc(count + 1/*for null terminator*/, GFP_KERNEL);
	memcpy(entry->payload, buffer, count);
	entry->payload[count] = '\0';

	list_add_tail(&entry->list, &list);

	stats.writes++;

	return count;
}

static struct kobj_attribute list_attribute =
	__ATTR(list, 0664, on_sysfs_show, on_sysfs_store);



static struct kobject *hello_kobject;



#define PROC_DIR_NAME			"hello_iface"
#define PROC_FILE_READS_NAME	"read_num"
#define PROC_FILE_WRITES_NAME	"write_num"
#define PROC_FILE_OWNER_NAME	"module_owner"
#define PROC_MESSAGE_CAPACITY	16

// Helper structure to keep together all information about message transfer related
// to the specific file.
//
// There are the following conventions:
//  - `was_send` is true means, that data transfer was completed during the previous
//    call;
//  - `strlen(message)` is zero means that we are in the first call of a new session
//    and the new message hasn't been created yet;
//  - `create` each file in procfs must have its own function for message creation.
//
// At different transfer stages the structure members will have the following values.
//
// Initial ------> Construct -----> Transfering ----> Complete -----> Initial
// -------         ---------        -----------       --------       ---------
// was_send=false  was_send=false   was_send=false    was_send=true  was_send=false
// len(message)=0  len(message)!=0  len(message)!=0   memssage=?     len(message)=0
// pos=0           pos=0            pos<len(message)  pos=?          pos=0
struct proc_message_transfer {
	char message[PROC_MESSAGE_CAPACITY];
	bool was_send;
	loff_t pos;
	void (*create)(char *buffer, size_t capacity);
};

// Constants to map procfs files with their transfer contexts.
enum proc_message_transfer_id {
	PROC_READS_ID	= 0,
	PROC_WRITES_ID	= 1,
	PROC_OWNER_ID	= 2,
};

void proc_message_create_reads(char *buffer, size_t capacity)
{
	snprintf(buffer, capacity, "%u\n", stats.reads);
}

void proc_message_create_writes(char *buffer, size_t capacity)
{
	snprintf(buffer, capacity, "%u\n", stats.writes);
}

void proc_message_create_owner(char *buffer, size_t capacity)
{
	snprintf(buffer, capacity, "module owner\n");
}

static struct proc_message_transfer proc_transfers[] = {
	{ {}, false, 0, proc_message_create_reads},
	{ {}, false, 0, proc_message_create_writes},
	{ {}, false, 0, proc_message_create_owner}
};

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file_reads;
static struct proc_dir_entry *proc_file_writes;
static struct proc_dir_entry *proc_file_owner;

static ssize_t on_proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
	int message_size;
	int remains;
	struct proc_message_transfer *transfer = NULL;

	if (!strcmp(file->f_path.dentry->d_name.name, PROC_FILE_READS_NAME))
		transfer = &proc_transfers[PROC_READS_ID];
	else if (!strcmp(file->f_path.dentry->d_name.name, PROC_FILE_WRITES_NAME))
		transfer = &proc_transfers[PROC_WRITES_ID];
	else if (!strcmp(file->f_path.dentry->d_name.name, PROC_FILE_OWNER_NAME))
		transfer = &proc_transfers[PROC_OWNER_ID];
	else
		transfer = NULL;

	if (!transfer) {
		pr_warn("Hello module. Unexpected file entry!\n");
		return 0;
	}

	if (transfer->was_send)
		goto stop_loop;

	if (!strlen(transfer->message))
		transfer->create(transfer->message, PROC_MESSAGE_CAPACITY);

	transfer->pos += *pos;
	if (transfer->pos < 0)
		transfer->pos = 0;

	message_size = strlen(transfer->message) - transfer->pos;
	if (message_size <= 0)
		goto stop_loop;

	message_size = min_t(size_t, count, message_size);
	remains = copy_to_user(buffer, &transfer->message[transfer->pos], message_size);
	transfer->pos += message_size - remains;
	transfer->was_send = transfer->pos >= strlen(transfer->message)	// Normal conditions.
						|| message_size == remains;					// Error conditions.

	return message_size - remains;

stop_loop:
	transfer->was_send = false;
	transfer->pos = 0;
	transfer->message[0] = '\0';

	return 0;
}

static struct proc_ops on_proc_ops = {
	.proc_read = on_proc_read,
};



int hello_init(void)
{
	INIT_LIST_HEAD(&list);

	hello_kobject = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_kobject)
		goto error_list;

	if (sysfs_create_file(hello_kobject, &list_attribute.attr))
		goto error_kobject;

	proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
	if (!proc_dir)
		goto error_sysfs;

	proc_file_reads = proc_create(PROC_FILE_READS_NAME, 0444, proc_dir, &on_proc_ops);
	if (!proc_file_reads)
		goto error_proc_dir;

	proc_file_writes = proc_create(PROC_FILE_WRITES_NAME, 0444, proc_dir, &on_proc_ops);
	if (!proc_file_writes)
		goto error_proc_file;

	proc_file_owner = proc_create(PROC_FILE_OWNER_NAME, 0444, proc_dir, &on_proc_ops);
	if (!proc_file_owner)
		goto error_proc_files;

	pr_info("Hello module is inited.\n");

	return 0;

error_proc_files:
	remove_proc_entry(PROC_FILE_WRITES_NAME, proc_dir);
error_proc_file:
	remove_proc_entry(PROC_FILE_READS_NAME, proc_dir);
error_proc_dir:
	remove_proc_entry(PROC_DIR_NAME, NULL);
error_sysfs:
	kobject_put(hello_kobject);
error_kobject:
	kobject_del(hello_kobject);
error_list:
	list_del(&list);

	return -ENOMEM;
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

	proc_remove(proc_dir);

	kobject_del(hello_kobject);

	pr_info("Hello module is exited.\n");
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Sergii Piatakov <sergii.piatakov@globallogic.com>");
MODULE_DESCRIPTION("Simple module for educational purposes.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
