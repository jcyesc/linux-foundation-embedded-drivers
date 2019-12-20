/* **************** LFD5435:5.4 s_22/lab_killable.c **************** */
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
 * Killable Sleep
 *
 * Modify the wait_event_interruptible() lab to use
 * wait_event_killable().  After a reading process goes to sleep, send
 * it a non-fatal signal, such as
 *
 *   $ kill -SIGCONT <pid>
 *
 * followed by a kill signal, such as SIGKILL.
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
	wait_event_killable(wq, (atomic_read(&data_ready)));
	dev_info(my_dev, "process %i (%s) awakening\n", current->pid,
		 current->comm);
	atomic_set(&data_ready, 0);
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
MODULE_DESCRIPTION("LFD5435:5.4 s_22/lab_killable.c");
MODULE_LICENSE("GPL v2");
