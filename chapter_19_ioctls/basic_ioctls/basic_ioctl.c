/* **************** LFD5435:5.4 s_19/lab_ioctl.c **************** */
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
 * Basic ioctl driver
 *
 * Adapt your earlier misc or character driver to include a basic
 * ioctl() entry point. It should use use a number of command values
 * shared with a user space program through a common header.
 *
 * Try passing a basic data structure back and forth and reading and
 * setting the value of some phony device control and status
 * registers.
 *
 * To install the module run:
 *
 * sudo insmod basic_ioctl.ko
 * dmesg
 * gcc -o a.out basic_ioctl_app.c  basic_ioctl.h
 * ls
 * sudo chmod 666 /dev/basic_ioctl
 * ./a.out
 * dmesg
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>

#include "basic_ioctl.h"		/* interface shared with user programs */
#include "generic_misc_dev.h"

/*
 *  ioctl() --- verify the user program's "cmd" and, when configuring the
 *   pretend device, the control register value the user says he wants to
 *   use.
 */
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long data)
{
	struct my_ioctl_data my_data;
	long csr;		/* copy of my_data csr value */
	dev_info(my_dev, "my_ioctl() received cmd = %du and %lu\n", cmd, data);

	switch (cmd) {
	case MY_CMD_CONFIG:
		dev_info(my_dev, "CONFIG command\n");
		if (copy_from_user(&my_data, (void __user *)data,
				   sizeof(struct my_ioctl_data))) {
			dev_err(my_dev, "fault on copy_from_user\n");
			return -EFAULT;
		}
		/* verify control register value, and pretend to set device */
		csr = my_data.csr.control_reg_value;
		if (csr != MY_CTRL_VAMPIRE &&
		    csr != MY_CTRL_UMPIRE && csr != MY_CTRL_PERSPIRE) {
			dev_err(my_dev, "invalid csr %lx\n", csr);
			return -EINVAL;
		}
		/* pretend to put csr value into device here .... */
		dev_info(my_dev, "ioctl message %s\n", my_data.message);

		/* mark message as empty, and put back a "status" reg val */
		my_data.message[0] = 0;

		/* for simple demo, just invert all the bits */
		my_data.csr.status_reg_value = ~csr;

		if (copy_to_user((void __user *)data, &my_data,
				 sizeof(struct my_ioctl_data))) {
			dev_err(my_dev, "fault on copy_to_user\n");
			return -EFAULT;
		}
		dev_info(my_dev, "device configured\n");
		return 0;	/* success */

	case MY_CMD_START:
		dev_info(my_dev, "device started\n");
		return 0;

	case MY_CMD_STOP:
		dev_info(my_dev, "device stopped\n");
		return 0;

	default:
		dev_info(my_dev, "invalid cmd %x\n", cmd);
		return -EINVAL;
	}
}

static const struct file_operations basic_ioctl_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_generic_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
	.unlocked_ioctl = my_ioctl,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Bill Kerr");
MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_19/lab_ioctl.c");
MODULE_LICENSE("GPL v2");
