/*
 * Multiple Kernel Timers
 *
 * Make the period in the first lab long enough so you can issue
 * multiple writes before the timer function run. (Hint: you may want
 * to save your data before running this lab.)
 *
 * How many times does the function get run?
 *
 * Fix the solution so multiple timers work properly.
 @*/

#include <linux/module.h>
#include <linux/timer.h>
#include <linux/delay.h>

/* either of these (but not both) will work */
/* #include "lab_char.h"  */
#include "lab_miscdev.h"

/* you probably don't need the ntimers variable, and it is a
   forward reference because we haven't done atomic variables
   but it is here to avoid unloading while there are still
   timers to unload.  It is also used sloppily on the exit :)
*/
static atomic_t ntimers;

static struct kt_data {
	struct timer_list timer;
	unsigned long value;
} *data;

static void kt_fun(struct timer_list *tl)
{
	struct kt_data *tdata = from_timer(tdata, tl, timer);
	dev_info(my_dev, "I am in my_timer_fun, jiffies = %ld\n", jiffies);
	dev_info(my_dev, " I think my current task pid is %d\n",
		 (int)current->pid);
	dev_info(my_dev, " my data is: %d\n", (int)tdata->value);
	kfree(tdata);
	//	kfree(tl);
	atomic_dec(&ntimers);
	dev_info(my_dev, "ntimers deced to %d\n", atomic_read(&ntimers));
}


static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	struct timer_list *tl;
	static int len = 100;

	dev_info(my_dev, " Entering the WRITE function\n");
	dev_info(my_dev, " my current task pid is %d\n", (int)current->pid);

	atomic_inc(&ntimers);

	dev_info(my_dev, "ntimers upped to %d\n", atomic_read(&ntimers));

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	data->value = len;

	tl= &data->timer;
	tl->expires = jiffies + 4 * HZ;	/* four second delay */

	timer_setup(tl,kt_fun, 0);

	add_timer(tl);

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

static int __init my_init(void)
{
	atomic_set(&ntimers, 0);
	return my_generic_init();
}

static void __exit my_exit(void)
{
	/* wait for all timers to finish ; pretty crummy */
	dev_info(my_dev, "ntimers in remove routine to %d\n",
		 atomic_read(&ntimers));
	while (atomic_read(&ntimers)) {
		dev_info(my_dev, "sleeping, ntimers still %d\n",
			 atomic_read(&ntimers));
		msleep(1000);	/* wait a second, ugly */
	}
	my_generic_exit();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
