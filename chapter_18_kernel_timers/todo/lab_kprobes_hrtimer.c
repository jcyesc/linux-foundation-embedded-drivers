/* **************** LFD5435:5.4 s_18/lab_kprobes_hrtimer.c **************** */
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
 * Using kprobes to get statistics.
 *
 * Using kprobes, find out how often kernel timers are deleted before
 * they are run.
 *
 * Examination of the kernel source discloses that timers are started
 * with either add_timer() or mod_timer()
 *
 * You can see how often timers are deleted by monitoring del_timer()
 * and del_timer_sync(); however, on single processor systems,
 * del_timer_sync() is not defined.
 *
 * Timers are frequent so you will probably not want to print out
 * every time they are queued up or deleted, but say every 100 times
 * plus final statistics.
 *
 * Is it possible that timer deletion can be more frequent than timer
 * queuing?
 *
 * Extra Credit: Do the same thing for high precision timers,
 * examining hrtimer_start(), hrtimer_forward() and hrtimer_cancel().
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>

static struct kprobe kp_hrtimer_forward, kp_hrtimer_cancel, kp_hrtimer_start;
static int count_hrtimer_forward = 0, count_hrtimer_cancel = 0,
	count_hrtimer_start = 0;

static int do_count(int *count, char *name)
{
	(*count)++;
	/*
	 * hrtimers much more frequent than timewheel timers!
	 * if ((*count) % 100 == 1) {
	 */
	if ((*count) % 1000 == 1) {
		pr_info("count_%s=%8d    current->comm=%s, current->pid=%d\n",
			name, *count, current->comm, current->pid);
	}
	return 0;
}

static int h_hrtimer_start(struct kprobe *p, struct pt_regs *regs)
{
	return do_count(&count_hrtimer_start, "hrtimer_start");
}

static int h_hrtimer_forward(struct kprobe *p, struct pt_regs *regs)
{
	return do_count(&count_hrtimer_forward, "hrtimer_forward");
}

static int h_hrtimer_cancel(struct kprobe *p, struct pt_regs *regs)
{
	return do_count(&count_hrtimer_cancel, "hrtimer_cancel");
}

static int __init setup_probe(struct kprobe *kp, const char *name,
			      int (*h_pre)(struct kprobe *kp,
					   struct pt_regs *regs))
{
	kp->pre_handler = h_pre;
	kp->symbol_name = (char *)name;
	pr_info("handler for %s at loaded\n", name);

	if (register_kprobe(kp)) {
		pr_info("Failed to register kprobe, quitting\n");
		return -1;
	}
	return 0;
}

static int __init my_init(void)
{
	/* set the handler functions */

	/*  4.3 kernel makes this an inline, hrtimer_start_range_ns() is called
	 * also when resetting the timer expiration so this is ok
	 */
	if (setup_probe
	    (&kp_hrtimer_start, "hrtimer_start_range_ns", h_hrtimer_start))
		return -1;

	if (setup_probe
	    (&kp_hrtimer_forward, "hrtimer_forward", h_hrtimer_forward)) {
		unregister_kprobe(&kp_hrtimer_start);
		return -1;
	}
	if (setup_probe(&kp_hrtimer_cancel, "hrtimer_cancel", h_hrtimer_cancel)) {
		unregister_kprobe(&kp_hrtimer_start);
		unregister_kprobe(&kp_hrtimer_forward);
		return -1;
	}

	pr_info("Hello: module loaded at 0x%p\n", my_init);

	return 0;
}

static void __exit my_exit(void)
{
	unregister_kprobe(&kp_hrtimer_forward);
	unregister_kprobe(&kp_hrtimer_start);
	unregister_kprobe(&kp_hrtimer_cancel);

	pr_info("\n\n FINAL STATISTICS:\n\n");
	pr_info("count_hrtimer_start = %d\n", count_hrtimer_start);
	pr_info("count_hrtimer_forward = %d\n", count_hrtimer_forward);
	pr_info("Totals add+mod = %d\n",
		count_hrtimer_start + count_hrtimer_forward);
	pr_info("count_hrtimer_cancel = %d\n", count_hrtimer_cancel);
	pr_info("Bye: module unloaded from 0x%p\n", my_exit);
	pr_info("Totals dels= %d\n", count_hrtimer_cancel);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_18/lab_kprobes_hrtimer.c");
MODULE_LICENSE("GPL v2");
