
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
#include <asm/msr.h>		/* needed for Time Stamp Counter functions */
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

static struct timer_list timer_a, timer_b;

static struct my_data {
	unsigned long period;
	unsigned long start_time;	/* jiffies value when we first started the timer */
} *data_a, *data_b;

static void ktfun_a(unsigned long var)
{
	struct my_data *dat = (struct my_data *)var;

	pr_info("A: period = %ld  elapsed = %ld\n",
		dat->period, jiffies - dat->start_time);
	dat->start_time = jiffies;
	mod_timer(&timer_a, dat->period + jiffies);
}

static void ktfun_b(unsigned long var)
{
	struct my_data *dat = (struct my_data *)var;

	pr_info("   B: period = %ld  elapsed = %ld\n", dat->period,
		jiffies - dat->start_time);
	dat->start_time = jiffies;
	mod_timer(&timer_b, dat->period + jiffies);
}

static int __init my_init(void)
{
	data_a = kmalloc(sizeof(*data_a), GFP_KERNEL);
	data_b = kmalloc(sizeof(*data_b), GFP_KERNEL);

	data_a->period = 1 * HZ;	/* short period, 1 second  */
	data_b->period = 10 * HZ;	/* longer period, 10 seconds */

	data_a->start_time = jiffies;
	data_b->start_time = jiffies;

	timer_a.expires = jiffies + data_a->period;
	timer_b.expires = jiffies + data_b->period;

	setup_timer(&timer_a, ktfun_a, (unsigned long)data_a);
	setup_timer(&timer_b, ktfun_b, (unsigned long)data_b);

	add_timer(&timer_a);
	add_timer(&timer_b);

	return 0;
}

static void __exit my_exit(void)
{
	/* delete any running timers */
	pr_info("Deleted timer A: rc = %d\n", del_timer_sync(&timer_a));
	pr_info("Deleted timer B: rc = %d\n", del_timer_sync(&timer_b));
	kfree(data_a);
	kfree(data_b);
	pr_info("Module successfully unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("MODULE_DESCRIPTION_NAME");
MODULE_LICENSE("GPL v2");
