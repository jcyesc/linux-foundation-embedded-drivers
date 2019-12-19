/* **************** LFD5435:5.4 s_18/hrexamp.c **************** */
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
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

static struct kt_data {
	struct hrtimer timer;
	ktime_t period;
} *data;

static enum hrtimer_restart ktfun(struct hrtimer *var)
{
	struct my_data *data = container_of(var, struct kt_data, timer);
	ktime_t now = var->base->get_time();

	pr_info("timer running at jiffies=%ld\n", jiffies);
	hrtimer_forward(var, now, data->period);
	return HRTIMER_RESTART;
}

static int __init my_init(void)
{
	data = kmalloc(sizeof(*data), GFP_KERNEL);
	data->period = ktime_set(1, 0);	/* short period, 1 second  */
	hrtimer_init(&data->timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	data->timer.function = ktfun;
	hrtimer_start(&data->timer, data->period, HRTIMER_MODE_REL);

	return 0;
}

static void __exit my_exit(void)
{
	hrtimer_cancel(&data->timer);
	kfree(data);
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL v2");
