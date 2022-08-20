// SPDX-License-Identifier: GPL-2.0

#include <linux/device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>


#define DO_TEST_FOR_KMALLOC
#define LIMIT_SIZE_POWER 24
#define MAX_TRY 10

#ifndef DO_TEST_FOR_PAGES
static void do_test(void)
{
	// Flags and counters.
	int i, j, k;
	bool is_failed = false;

	// Timestamps and durations.
	struct timespec64 begin, end, elapsed;
	struct timespec64 duration_allocation_min, duration_free_min;

	size_t size;
	char *buffer = NULL;
	char *buffers[MAX_TRY];


	for (i = 0; i < LIMIT_SIZE_POWER && !is_failed; ++i) {
		duration_allocation_min.tv_sec = 1000;
		duration_free_min.tv_sec = 1000;
		size = ((size_t)1) << i;
		for (j = 0; j < MAX_TRY; ++j) {
			ktime_get_raw_ts64(&begin);
			#ifdef DO_TEST_FOR_VMALLOC
			buffer = vmalloc(size);
			#endif//DO_TEST_FOR_VMALLOC
			#ifdef DO_TEST_FOR_KMALLOC
			buffer = kmalloc(size, GFP_KERNEL);
			#endif//DO_TEST_FOR_KMALLOC
			#ifdef DO_TEST_FOR_KZALLOC
			buffer = kzalloc(size, GFP_KERNEL);
			#endif//DO_TEST_FOR_KZALLOC
			ktime_get_raw_ts64(&end);
			elapsed = timespec64_sub(end, begin);

			if (timespec64_compare(&duration_allocation_min, &elapsed) > 0)
				duration_allocation_min = elapsed;

			if (!buffer) {
				is_failed = true;
				break;
			}

			buffer[size - 1] = '\0';
			buffers[j] = buffer;
		}

		for (k = 0; k < j; ++k) {
			ktime_get_raw_ts64(&begin);
			#ifdef DO_TEST_FOR_VMALLOC
			vfree(buffers[k]);
			#endif//DO_TEST_FOR_VMALLOC
			#ifdef DO_TEST_FOR_KMALLOC
			kfree(buffers[k]);
			#endif//DO_TEST_FOR_KMALLOC
			#ifdef DO_TEST_FOR_KZALLOC
			kfree(buffers[k]);
			#endif//DO_TEST_FOR_KZALLOC
			ktime_get_raw_ts64(&end);
			elapsed = timespec64_sub(end, begin);

			if (timespec64_compare(&duration_free_min, &elapsed) > 0)
				duration_free_min = elapsed;
		}

		if (is_failed) {
			pr_info("Allocation failed for size: 2^%d=%lu.\n", i, size);
		} else {
			pr_info("%lu, %lld, %lld\n", size, duration_allocation_min.tv_sec * 1000 * 1000 * 1000
										  + duration_allocation_min.tv_nsec
										  , duration_free_min.tv_sec * 1000 * 1000 * 1000
										  + duration_free_min.tv_nsec);
		}
	}
}
#endif//DO_TEST_FOR_PAGES



#ifdef DO_TEST_FOR_PAGES
static void do_test_pages(void)
{
	// Flags and counters.
	int i, j, k;
	bool is_failed = false;

	// Timestamps and durations.
	struct timespec64 begin, end, elapsed;
	struct timespec64 duration_allocation_min, duration_free_min;

	unsigned long page = 0;
	unsigned long pages[MAX_TRY];


	for (i = 0; i < LIMIT_SIZE_POWER && !is_failed; ++i) {
		duration_allocation_min.tv_sec = 1000;
		duration_free_min.tv_sec = 1000;
		for (j = 0; j < MAX_TRY; ++j) {
			ktime_get_raw_ts64(&begin);
			page = __get_free_pages(GFP_KERNEL, i);
			ktime_get_raw_ts64(&end);
			elapsed = timespec64_sub(end, begin);

			if (timespec64_compare(&duration_allocation_min, &elapsed) > 0)
				duration_allocation_min = elapsed;

			if (!page) {
				is_failed = true;
				break;
			}

			((char *)page)[(PAGE_SIZE << i) - 1] = '\0';
			pages[j] = page;
		}

		for (k = 0; k < j; ++k) {
			ktime_get_raw_ts64(&begin);
			free_pages(pages[k], i);
			ktime_get_raw_ts64(&end);
			elapsed = timespec64_sub(end, begin);

			if (timespec64_compare(&duration_free_min, &elapsed) > 0)
				duration_free_min = elapsed;
		}

		if (is_failed) {
			pr_info("Allocation failed for order: %d.\n", i);
		} else {
			pr_info("%lu, %lld, %lld\n", (PAGE_SIZE << i)
										  , duration_allocation_min.tv_sec * 1000 * 1000 * 1000
										  + duration_allocation_min.tv_nsec
										  , duration_free_min.tv_sec * 1000 * 1000 * 1000
										  + duration_free_min.tv_nsec);
		}
	}
}
#endif//DO_TEST_FOR_PAGES



int hello_init(void)
{
	pr_info("Hello! Init.\n");

	#ifdef DO_TEST_FOR_PAGES
	do_test_pages();
	#else
	do_test();
	#endif//DO_TEST_FOR_PAGES

	return 0;
}

void hello_exit(void)
{
	pr_info("Hello! Cleanup.\n");
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_AUTHOR("Sergii Piatakov <sergii.piatakov@globallogic.com>");
MODULE_DESCRIPTION("Simple module for educational purposes.");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
