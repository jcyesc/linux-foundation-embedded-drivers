/* **************** LFD5435:5.4 s_19/lab_ioctl_data.c **************** */
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
 * Using ioctl's to pass data
 *
 * Write a simple module that uses the ioctl directional information
 * to pass a data buffer of fixed size back and forth between the
 * driver and the user-space program.
 *
 * The size and direction(s) of the data transfer should be encoded in
 * the command number.
 *
 * You'll need to write a user-space application to test this.
 */

#include <linux/module.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

struct my_data {
	int i;
	long x;
	char s[256];
};

static struct my_data my_data = {
	.i = -100,
	.x = 100,
	.s = "original string",
};

#define MYIOC_TYPE 'k'

static long
mycdrv_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int size, rc, direction;
	void __user *ioargp = (void __user *)arg;

	if (_IOC_TYPE(cmd) != MYIOC_TYPE) {
		dev_err(my_dev, " got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}

	direction = _IOC_DIR(cmd);
	size = _IOC_SIZE(cmd);

	switch (direction) {

	case _IOC_WRITE:
		dev_info(my_dev,
			 " reading = %d bytes from user-space and writing to device\n",
			 size);
		rc = copy_from_user(&my_data, ioargp, size);
		dev_info(my_dev,
			 "    my_data.i = %d\n    (int)my_data.x = %ld\n    my_data.s = %s\n",
			 my_data.i, my_data.x, my_data.s);
		return rc;

	case _IOC_READ:
		dev_info(my_dev,
			 " reading device and writing = %d bytes to user-space\n",
			 size);
		dev_info(my_dev,
			 "    my_data.i = %d\n    (int)my_data.x = %ld\n    my_data.s = %s\n",
			 my_data.i, my_data.x, my_data.s);
		rc = copy_to_user(ioargp, &my_data, size);
		return rc;

	default:
		dev_info(my_dev, " got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_19/lab_ioctl_data.c");
MODULE_LICENSE("GPL v2");
