/*
 * Kernel Timers from a Character Driver
 *
 * Write a driver that puts launches a low resolution kernel timer
 * whenever a write to the device takes place.
 *
 * Pass some data to the driver and have it print out.
 *
 * Have it print out the current->pid field when the timer function
 * is queued up, and then again when the function is executed.
 *
 @*/

#include <linux/module.h>
#include <linux/timer.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

static struct timer_list my_timer;
static int timer_initd;

static void my_timer_function(unsigned long ptr)
{
	dev_info(my_dev, "I am in my_timer_fun, jiffies = %ld\n", jiffies);
	dev_info(my_dev, " I think my current task pid is %d\n",
		 (int)current->pid);
	dev_info(my_dev, " my data is: %d\n", (int)ptr);
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	static int len = 100;
	dev_info(my_dev, " Entering the WRITE function\n");
	dev_info(my_dev, " my current task pid is %d\n", (int)current->pid);
	my_timer.expires = jiffies + HZ;	/* one second delay */
	setup_timer(&my_timer, my_timer_function, len);
	add_timer(&my_timer);
	timer_initd = 1;
	dev_info(my_dev, "Adding timer at jiffies = %ld\n", jiffies);
	len += 100;
	return mycdrv_generic_write(file, buf, lbuf, ppos);
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

static void __exit my_exit(void)
{
	/*
	   delete any running timers. The timer_inid variable avoids
	   trying to delete an unitialized timer, which hangs rmmod.
	   One could instead do del_timer(), but while hiding the
	   symptoms would still be an error
	*/

	if (timer_initd)
		dev_info(my_dev, "Deleted time,r rc = %d\n",
			 del_timer_sync(&my_timer));
	else
		dev_info(my_dev, "Timer not used\n");
	my_generic_exit();
}

module_init(my_generic_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_AUTHOR("John Bonesio");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
