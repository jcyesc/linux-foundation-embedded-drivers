/* **************** LFD5435:5.4 s_13/lab_usermap_align-not-needed.c **************** */
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
 * Mapping User Pages
 *
 * Use the character device driver, adapt it to use get_user_pages()
 * for the read() and write() entry points.
 *
 * An alternative soulution using get_user_pages_fast() is
 * also given.
 *
 * To properly exercise this you will need to use a page-aligned utility
 * such as dd, or write page-aligned reading and writing programs.
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/pagemap.h>

/* either of these (but not both) will work */
/* #include "lab_char.h" */
#include "lab_miscdev.h"

unsigned int pg_offset(const void *addr)
{
	unsigned long mask = PAGE_SIZE - 1;

	return (unsigned int)((unsigned long)addr & mask);
}

unsigned long pfn_of(const void *addr)
{
	return (unsigned long)addr >> PAGE_SHIFT;
}

//#define min(x, y) ((x) < (y) ? (x) : (y))

static ssize_t
mycdrv_rw(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos,
	  int rw)
{
	int j, rc, npages;
	struct page **pages;
	char *uaddr, *ktmp;
	size_t nb, cur_len;

	/* get the total number of pages of memory needed */
	npages = (pfn_of(buf + lbuf) - pfn_of(buf)) + 1;

	/* allocate space for array of page structures */

	dev_info(my_dev, "npages = %d for %ld bytes\n", npages, (long)lbuf);
	pages = kmalloc_array(npages, sizeof(pages), GFP_KERNEL);
	if (!pages) {
		dev_err(my_dev, "allocating pages failed\n");
		return -ENOMEM;
	}

	/* get the page structures, protect with a lock */

	down_read(&current->mm->mmap_sem);
	rc = get_user_pages((unsigned long)buf, npages, FOLL_WRITE, pages, NULL);
	up_read(&current->mm->mmap_sem);
	dev_info(my_dev, " I received %d pages from the user\n", rc);

	ktmp = ramdisk;
	cur_len = lbuf;
	for (j = 0; j < npages; j++) {

		/* remap the page address; could also use page_address(page) */

		uaddr = kmap(pages[j]);
		nb = PAGE_SIZE;

		if (j == 0) {
			nb -= pg_offset(buf);
			uaddr += pg_offset(buf);
		}
		nb = min(nb, cur_len);

		/* do the actual i/o operation ; for now just to a kernel buffer */

		/* should put the switch outside the loop; for now live with it */

		switch (rw) {
		case 0:	/* read */
			memcpy(uaddr, ktmp, nb);
			/* dev_info(my_dev," kernel sees on a read:\n%s\n", uaddr); */
			break;
		case 1:	/* write */
			memcpy(ktmp, uaddr, nb);
			/* dev_info(my_dev," kernel sees on a write:uaddr:\n%s\n", uaddr); */
			break;
		default:	/* should never hit here */
			dev_err(my_dev, "Error in rw routine, rw=%d\n", rw);
			break;
		}

		cur_len -= nb;
		ktmp += nb;

		/* release the page cache -- no surprises to apps */

		set_page_dirty_lock(pages[j]);
		put_page(pages[j]);	/* make sure it is released from cache */

		/* release the remap; don't need to do if use page_address() */
		kunmap(pages[j]);
	}

	kfree(pages);

	dev_info(my_dev, "Leaving the READ  function, nbytes=%ld\n",
		 (long)lbuf);
	return lbuf;
}

static ssize_t
mycdrv_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	int rw = 0;

	return mycdrv_rw(file, buf, lbuf, ppos, rw);
}

static ssize_t
mycdrv_write(struct file *file, const char __user *buf, size_t lbuf,
	     loff_t *ppos)
{
	int rw = 1;

	return mycdrv_rw(file, (char __user *)buf, lbuf, ppos, rw);
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_AUTHOR("John Bonesio");
MODULE_DESCRIPTION("LFD5435:5.4 s_13/lab_usermap_align-not-needed.c");
MODULE_LICENSE("GPL v2");