
/*
 * Using get_user() and put_user().
 *
 * Adapt your character driver to use get_user() and put_user().
 *
 * In order to execute the driver run:
 *
 * sudo insmod put_and_get_driver.ko
 * dmesg
 * ls /dev/put_and_get
 * echo 1 > /dev/put_and_get
 * sudo chmod 666 /dev/put_and_get
 * echo 1 > /dev/put_and_get
 * more /dev/put_and_get
 * gcc -o app put_and_get_app.c
 * ./app
 * more /dev/put_and_get
 * sudo rmmod put_and_get_driver
 */

#include <linux/module.h>

#include "generic_char_driver.h"

static inline ssize_t
mycdrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes = 0, maxbytes, bytes_to_do;

	char *tmp = ramdisk + *ppos;

	maxbytes = ramdisk_size - *ppos;
	bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
	if (bytes_to_do == 0)
		dev_warn(my_dev, "Reached end of the device on a read");
	while ((nbytes < bytes_to_do) && !put_user(*tmp, (buf + nbytes))) {
		nbytes++;
		tmp++;
	}
	*ppos += nbytes;
	dev_info(my_dev, "\n Leaving the   READ function, nbytes=%d, pos=%d\n",
		 nbytes, (int)*ppos);
	return nbytes;
}

static inline ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	int nbytes = 0, maxbytes, bytes_to_do;
	char *tmp = ramdisk + *ppos;
	
	maxbytes = ramdisk_size - *ppos;
	bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
	if (bytes_to_do == 0)
		dev_warn(my_dev, "Reached end of the device on a write");
	while ((nbytes < bytes_to_do) && !get_user(*tmp, (buf + nbytes))) {
		nbytes++;
		tmp++;
	}
	*ppos += nbytes;
	dev_info(my_dev, "\n Leaving the   WRITE function, nbytes=%d, pos=%d\n",
		 nbytes, (int)*ppos);
	return nbytes;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_13/lab_putget.c");
MODULE_LICENSE("GPL v2");

