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

static struct timer_list timer;
static struct kt_data {
	unsigned long period;
	unsigned long start_time;	/* jiffies value when we first started the timer */
} data;

static void ktfun(unsigned long var)
{
	struct kt_data *tdata = (struct kt_data *)var;

	pr_info("ktimer: period = %ld  elapsed = %ld\n",
		tdata->period, jiffies - tdata->start_time);
	/* resubmit */
	mod_timer(&timer, tdata->period + jiffies);
}

static int __init my_init(void)
{

	data.period = 2 * HZ;	/* short period,   2 secs */
	timer.expires = jiffies + data.period;
	data.start_time = jiffies;
	setup_timer(&timer, ktfun,(unsigned long)&data);
	add_timer(&timer);

	return 0;
}

static void __exit my_exit(void)
{
	/* delete any running timers */
	pr_info("Deleted time,r rc = %d\n", del_timer_sync(&timer));
	pr_info("Module successfully unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
