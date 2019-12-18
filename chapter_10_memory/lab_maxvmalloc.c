/* **************** LFD5435:5.4 s_10/lab_maxvmalloc.c **************** */
/*
 * The code herein is: Copyright the Linux Foundation, 2019
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    https://training.linuxfoundation.org
 *     email:  training@linuxfoundation.org
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Testing Maximum Memory Allocation (vmalloc)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

//static long mem = 100000000L;
static int mem = 64;
module_param(mem, int, 0444);

#define MB (1024*1024)

static int __init my_init(void)
{
	static char *vm_buff;
	long size;

	/* try vmalloc */

	for (size = 4 * MB; size <= mem * MB; size *= 2) {
		pr_info(" pages=%6ld, size=%8ld ", size / PAGE_SIZE, size / MB);
		vm_buff = vmalloc(size);
		if (!vm_buff) {
			pr_err("... vmalloc failed\n");
			break;
		}
		pr_info("... vmalloc OK\n");
		vfree(vm_buff);
	}

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_10/lab_maxvmalloc.c");
MODULE_LICENSE("GPL v2");
