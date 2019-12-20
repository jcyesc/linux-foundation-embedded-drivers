/* **************** LFD5435:5.4 s_23/lab_one_tasklet_dynamic.c **************** */
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
 * Producer/Consumer (dynamic tasklet solution)
 *
 * You may have noticed that
 * you lost some bottom halves. This will happen when more than one
 * interrupt arrives before bottom halves are accomplished. For
 * instance, the same tasklet can only be queued up twice.
 *
 * Write a bottom half that can "catch up"; i.e., consume more than
 * one event when it is called, cleaning up the pending queue.  Do
 * this for at least one of the previous solutions.
 */

#include <linux/module.h>
#include "lab_one_interrupt.h"

static void t_fun(unsigned long t_arg)
{
	struct my_dat *data = (struct my_dat *)t_arg;

	atomic_inc(&counter_bh);
	pr_info("In BH: counter_th = %d, counter_bh = %d, jiffies=%ld, %ld\n",
		atomic_read(&counter_th), atomic_read(&counter_bh),
		data->jiffies, jiffies);
	kfree(data);
}

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	struct tasklet_struct *t;
	struct my_dat *data;

	data = kmalloc(sizeof(struct my_dat), GFP_ATOMIC);
	t = &data->tsk;
	data->jiffies = jiffies;

	tasklet_init(t, t_fun, (unsigned long)data);

	atomic_inc(&counter_th);
	tasklet_schedule(t);
	mdelay(delay);		/* hoke up a delay to try to cause pileup */
	return IRQ_NONE;	/* we return IRQ_NONE because we are just observing */
}

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_23/lab_one_tasklet_dynamic.c");
MODULE_LICENSE("GPL v2");
