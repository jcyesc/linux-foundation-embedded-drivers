/*
 * Periodic Kernel Timers
 *
 * Write a module that launches a low resolution periodic kernel timer
 * function; i.e., it should re-install itself.
 *
 @*/

#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/slab.h>

static struct kt_data {
	struct timer_list timer;
	unsigned long period;
	unsigned long start_time;	/* jiffies value when we first started the timer */
} *data;

static void ktfun(struct timer_list *tl)
{
	struct kt_data *tdata = from_timer(tdata, tl, timer);
	pr_info("ktimer: period = %ld  elapsed = %ld\n",
		tdata->period, jiffies - tdata->start_time);
	/* resubmit */
	mod_timer(tl, jiffies + tdata->period);
}

static int __init my_init(void)
{
	struct timer_list *tl;

	data = kmalloc(sizeof(*data), GFP_KERNEL);
	data->period = 2 * HZ;	/* short period,   2 secs */
	data->start_time = jiffies;

	tl = &data->timer;
	tl->expires = jiffies + data->period;
	timer_setup(tl, ktfun, 0);
	add_timer(tl);

	pr_info("Launching timer with period of %ld seconds\n\n",
		data->period);
	return 0;
}

static void __exit my_exit(void)
{
	/* delete any running timers */
	pr_info("Deleted time,r rc = %d\n", del_timer_sync(&data->timer));
	pr_info("Module successfully unloaded\n");
	kfree(data);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
