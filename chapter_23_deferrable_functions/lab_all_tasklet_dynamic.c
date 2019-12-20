/* **************** LFD5435:5.4 s_23/lab_all_tasklet_dynamic.c **************** */
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
 * Sharing All Interrupts, Bottom Halves, Producer/Consumer Problem
 * (tasklet solution)
 *
 * Find solutions for the producer/consumer problem for the previous
 * lab.
 */

#include <linux/module.h>
#include "lab_all_interrupt.h"

struct my_dat {
	int irq;
	struct tasklet_struct tsk;
};

static void t_fun(unsigned long t_arg)
{
	struct my_dat *data = (struct my_dat *)t_arg;

	atomic_inc(&bhs[data->irq]);
	kfree(data);
}

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	struct tasklet_struct *t;
	struct my_dat *data;

	data = kmalloc(sizeof(struct my_dat), GFP_ATOMIC);
	data->irq = irq;
	t = &data->tsk;
	atomic_inc(&interrupts[irq]);
	tasklet_init(t, t_fun, (unsigned long)data);
	tasklet_schedule(t);
	mdelay(delay);
	/* we return IRQ_NONE because we are just observing */
	return IRQ_NONE;
}

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_23/lab_all_tasklet_dynamic.c");
MODULE_LICENSE("GPL v2");
