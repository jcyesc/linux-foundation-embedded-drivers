
/*
 * Defines a sample char driver that does the following:
 *
 * - Initializes the device driver
 * - Register the char device with register_chrdev_region()
 * - The /dev/sample_char_driver node is created till we run mknod.
 *
 * 	sudo insmod  sample_char_driver.ko
 * 	ls -la /dev/sample_char_driver
 * 	sudo mknod -m 666 /dev/sample_char_driver c 500 20
 * 	ls -la /dev/sample_char_driver
 * 	sudo rmmod sample_char_driver
 * 	sudo rm /dev/sample_char_driver
 *
 * Because we are not calling class_create() and device_create(), we
 * need to execute the command mknod.
 *
 */

#include <linux/module.h>	/* for modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev utilities */

#define MYDEV_NAME "sample_char_driver"

static char *ramdisk;
#define ramdisk_size ((size_t) (128*PAGE_SIZE))

static dev_t first;
static unsigned int count = 1;
static int my_major = 500;
static int my_minor = 20;
static struct cdev *my_cdev;

static int mycdrv_open(struct inode *inode, struct file *file)
{
	pr_info("OPENING device: %s:\n\n", MYDEV_NAME);
	return 0;
}

static int mycdrv_release(struct inode *inode, struct file *file)
{
	pr_info("CLOSING device: %s:\n\n", MYDEV_NAME);
	return 0;
}

static ssize_t
mycdrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;

	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_to_user(buf, ramdisk + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("\n READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	int nbytes;

	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("trying to write past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user(ramdisk + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("\n WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_open,
	.release = mycdrv_release,
};

static int __init my_init(void)
{
	pr_info("Sample char driver init()\n");
	ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
	first = MKDEV(my_major, my_minor);
	register_chrdev_region(first, count, MYDEV_NAME);
	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &mycdrv_fops);
	cdev_add(my_cdev, first, count);
	pr_info("\nSample char driver init() succeeded in registering character device %s\n", MYDEV_NAME);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Sample char driver exit()\n");
	cdev_del(my_cdev);
	unregister_chrdev_region(first, count);
	pr_info("\ndevice unregistered\n");
	kfree(ramdisk);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_LICENSE("GPL v2");


