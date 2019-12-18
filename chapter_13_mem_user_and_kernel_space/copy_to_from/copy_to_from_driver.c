
/*
 * Copy to From driver
 *
 * This driver shows how to read and write memory from the kernel space to the user space
 * and vice.
 *
 * To execute this module run:
 *
 *	sudo insmod copy_to_from_driver.ko
 *	ls /dev/copy_to_from
 * 	dmesg
 * 	echo 'Lets dance' > /dev/copy_to_from
 * 	chmod /dev/copy_to_from
 * 	sudo chmod 777 /dev/copy_to_from
 * 	echo 'Lets dance' > /dev/copy_to_from
 * 	more /dev/copy_to_from
 *	sudo rmmod copy_to_from_driver
 */

#include <linux/module.h>

/* either of these (but not both) will work */
#include "misc_dev.h"

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_generic_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_13/lab_copytofrom.c");
MODULE_LICENSE("GPL v2");
