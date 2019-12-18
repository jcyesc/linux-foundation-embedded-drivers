/* **************** LFD5435:5.4 s_11/lab_priv.c **************** */
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
 * Private Data for Each Open</SUBTOPIC>
 *
 * Modify the improve_char_driver so that each opening of the device
 * allocates its own data area, which is freed upon release. Thus data
 * will not be persistent across multiple opens.
 *
 * In order to install the module run:
 *
 * 	sudo insmod memory_allocation.ko
 * 	ls -la /dev/memory_allocation_driver
 * 	sudo mknod -m 666 /dev/memory_allocation_driver c 500 23
 * 	ls -la /dev/memory_allocation_driver
 *
 * To remove the driver and the device node
 *
 * 	sudo rmmod memory_allocation_driver
 * 	sudo rm /dev/memory_allocation_driver
 *
 * To run the client execute:
 *
 * 	gcc -o memory_app memory_allocation_app.c
 * 	./memory_app
 *
 * Because we are not calling class_create() and device_create(), we
 * need to execute the command mknod.
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define MYDEV_NAME "memory_allocation_driver"

static dev_t first;
static unsigned int count = 1;
static int my_major = 500;
static int my_minor = 23;
static struct cdev *my_cdev;
static size_t ramdisk_size = (16 * PAGE_SIZE);

static int mycdrv_open(struct inode *inode, struct file *file)
{
	static int counter;

	char *ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);

	file->private_data = ramdisk;
	pr_info("attempting to open device: %s:\n", MYDEV_NAME);
	pr_info("MAJOR number = %d, MINOR number = %d\n",
		imajor(inode), iminor(inode));
	counter++;
	pr_info("successfully open  device: %s:\n\n", MYDEV_NAME);
	pr_info("I have been opened  %d times since being loaded\n", counter);
	pr_info("ref=%d\n", (int)module_refcount(THIS_MODULE));

	return 0;
}

static int mycdrv_release(struct inode *inode, struct file *file)
{
	char *ramdisk = file->private_data;
	pr_info("CLOSING device: %s:\n\n", MYDEV_NAME);
	kfree(ramdisk);
	return 0;
}

static ssize_t
mycdrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	char *ramdisk = file->private_data;

	int nbytes = lbuf - copy_to_user(buf, ramdisk + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	char *ramdisk = file->private_data;

	int nbytes = lbuf - copy_from_user(ramdisk + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
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
	first = MKDEV(my_major, my_minor);
	if (register_chrdev_region(first, count, MYDEV_NAME) < 0) {
		pr_err("failed to register character device region\n");
		return -1;
	}
	my_cdev = cdev_alloc();
	if (!my_cdev) {
		pr_err("cdev_alloc() failed\n");
		unregister_chrdev_region(first, count);
		return -1;
	}
	cdev_init(my_cdev, &mycdrv_fops);
	if (cdev_add(my_cdev, first, count) < 0) {
		pr_err("cdev_add() failed\n");
		cdev_del(my_cdev);
		unregister_chrdev_region(first, count);
		return -1;
	}
	pr_info("\nSucceeded in registering character device %s\n", MYDEV_NAME);
	return 0;
}

static void __exit my_exit(void)
{
	if (my_cdev)
		cdev_del(my_cdev);
	unregister_chrdev_region(first, count);
	pr_info("\nDevice %s unregistered\n", MYDEV_NAME);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_11/lab_priv.c");
MODULE_LICENSE("GPL v2");


