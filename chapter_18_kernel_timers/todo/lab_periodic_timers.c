/*
 * Multiple Periodic Kernel Timers
 *
 * Write a module that launches two low-resolution periodic kernel
 * i.e., they should re-install themselves.
 *
 * One periodic sequence should be for a second or less and the other
 * should have a significantly longer period.
 *
 * Each time the timer functions execute, print out the total elapsed
 * time since the module was loaded (in jiffies).
 *
 @*/
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

//static struct timer_list timer_A, timer_B;

static struct kt_data {
	struct timer_list timer;
	unsigned long period;
	unsigned long start_time;	/* jiffies value when we first started the timer */
	char *IDENT;
} *data_A, *data_B;

static void ktfun(struct timer_list *tl)
{
	struct kt_data *tdata = from_timer(tdata, tl, timer);
				 
	pr_info("%s: period = %ld  elapsed = %ld\n",
		tdata->IDENT, tdata->period, jiffies - tdata->start_time);
	tdata->start_time = jiffies;
	mod_timer(tl, tdata->period + jiffies);
}

static int __init my_init(void)
{

	struct timer_list *tl_A, *tl_B;

	data_A = kmalloc(sizeof(*data_A), GFP_KERNEL);
	data_B = kmalloc(sizeof(*data_B), GFP_KERNEL);

	data_A->period = 1 * HZ;	/* short period, 1 second  */
	data_B->period = 10 * HZ;	/* longer period, 10 seconds */

	data_A->start_time = jiffies;
	data_B->start_time = jiffies;

	data_A->IDENT="A ";
	data_B->IDENT="   B ";

	tl_A = &data_A->timer;
	tl_B = &data_B->timer;

	tl_A->expires = jiffies + data_A->period;
	tl_B->expires = jiffies + data_B->period;

	timer_setup(tl_A, ktfun, 0);
	timer_setup(tl_B, ktfun, 0);

	add_timer(tl_A);
	add_timer(tl_B);

	return 0;
}

static void __exit my_exit(void)
{
	/* delete any running timers */
	pr_info("Deleted timer A: rc = %d\n", del_timer_sync(&data_A->timer));
	pr_info("Deleted timer B: rc = %d\n", del_timer_sync(&data_B->timer));
	kfree(data_A);
	kfree(data_B);
	pr_info("Module successfully unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
