/* **************** LFD5435:5.4 s_22/lab_wait_event.c **************** */
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
 *  Using Wait Queues (wait_event(), 1 sleeper wakes up solution)
 *
 *  Generalize the previous character driver to use wait queues,
 *
 *  Have the read() function go to sleep until woken by a write()
 *  function.  (You could also try reversing read and write.)
 *
 *  You may want to open up two windows and read in one window and
 *  then write in the other window.
 *
 *  Try putting more than one process to sleep, i.e., run your test
 *  read program more than once simultaneously before running the
 *  write program to awaken them.  If you keep track of the pid's you
 *  should be able to detect in what order processes are woken.
 *
 *  There are several solutions given:
 *
 *      Using wait_event_interruptible().  You may want to use atomic
 *      functions for any global variables used in the logical
 *      condition.
 *
 *      Using wait_for_completion().
 *
 *      Using mutexes.
 *
 *      Using semaphores.
 *
 *      Using exclusive waiting on the many readers solution..  How
 *      many processes wake up?
 *
 *  If you test with cat, echo, or dd, you may see different results
 *  than if you use the supplied simple read/write programs.  Why?
 */

#include <linux/module.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

static DECLARE_WAIT_QUEUE_HEAD(wq);

static atomic_t data_ready;

static ssize_t
mycdrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	dev_info(my_dev, "process %i (%s) going to sleep\n", current->pid,
		 current->comm);
	wait_event_interruptible(wq, (atomic_read(&data_ready)));
	atomic_set(&data_ready, 0);
	dev_info(my_dev, "process %i (%s) awakening\n", current->pid,
		 current->comm);
	return mycdrv_generic_read(file, buf, lbuf, ppos);
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	int nbytes = mycdrv_generic_write(file, buf, lbuf, ppos);

	dev_info(my_dev, "process %i (%s) awakening the readers...\n",
		 current->pid, current->comm);
	atomic_set(&data_ready, 1);
	wake_up_interruptible(&wq);
	return nbytes;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

static int __init my_init(void)
{
	atomic_set(&data_ready, 0);
	return my_generic_init();
}

module_init(my_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_22/lab_wait_event.c");
MODULE_LICENSE("GPL v2");
