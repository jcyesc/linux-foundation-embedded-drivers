/* **************** LFD5435:5.4 s_26/lab_dma.c **************** */
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
 * DMA Memory Allocation
 *
 * Extend your character or miscellaneous driver module to allocate
 * and map a suitable DMA buffer, and obtain the bus address handle.
 *
 * Do this in three ways:
 *
 *    Using dma_alloc_coherent().
 *    Using dma_map_single()
 *    Using a DMA Pool.
 *
 * Compare the resulting kernel and bus addresses; how do they differ?
 * Compare with the value of PAGE_OFFSET.
 *
 * In each case copy a string into the buffer and make sure it can be
 * read back properly.
 *
 * In the case of dma_map_single(), you may want to compare the use of
 * different direction arguments.
 *
 * To execute this driver run:
 *
 * make
 * sudo rmmod basic_dma
 * sudo insmod basic_dma.ko
 * dmesg
 * sudo rmmod basic_dma
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include "generic_misc_dev.h"

static char *kbuf;
static dma_addr_t handle;
static size_t size = (10 * PAGE_SIZE), pool_size = 1024, pool_align = 8;
static struct dma_pool *mypool;

/* int direction = PCI_DMA_TODEVICE ; */
/*/ int direction = PCI_DMA_FROMDEVICE ; */
static int direction = PCI_DMA_BIDIRECTIONAL;
/* int direction = PCI_DMA_NONE; */

static int setup_dma_mask(struct device *dev)
{
	if (dev->dma_mask == NULL) {
		/*
		 * This happens with misc character devices
		 */
		dev_warn(dev, "previous dma_mask pointer was NULL\n");
		dev->dma_mask = kmalloc(sizeof(u64), GFP_KERNEL);
		if (dev->dma_mask == NULL)
			return -1;
	}
	if (dma_set_mask(dev, DMA_BIT_MASK(32)) != 0) {
		pr_info("FAILED:  dma_set_mask()\n");
		return -1;
	}
	dma_set_coherent_mask(dev, DMA_BIT_MASK(32));
	return 0;
}

static void output(char *kbuf, dma_addr_t handle, size_t size, char *string)
{
	unsigned long diff;

	diff = (unsigned long)kbuf - handle;
	dev_info(my_dev, "kbuf=%12p, handle=%12p, size = %d\n", kbuf,
		 (unsigned long *)handle, (int)size);
	dev_info(my_dev,
		 "(kbuf-handle)= %12p, %12lu, PAGE_OFFSET=%12lu, compare=%lu\n",
		 (void *)diff, diff, PAGE_OFFSET, diff - PAGE_OFFSET);
	strcpy(kbuf, string);
	dev_info(my_dev, "string written was, %s\n", kbuf);
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
};

static int __init my_init(void)
{
	int rc;

	rc = my_generic_init();
	if (rc < 0)
		return rc;
	dev_info(my_dev, "Loading DMA allocation test module\n");

	rc = setup_dma_mask(my_dev);
	if (rc < 0) {
		dev_info(my_dev, "dma setup mask failed\n");
		return rc;
	}

	/* dma_alloc_coherent method */

	dev_info(my_dev, "\nTesting dma_alloc_coherent()..........\n\n");
	kbuf = dma_alloc_coherent(my_dev, size, &handle, GFP_KERNEL);
	output(kbuf, handle, size, "This is the dma_alloc_coherent() string");
	dma_free_coherent(my_dev, size, kbuf, handle);

	/* dma_map/unmap_single */

	dev_info(my_dev, "\nTesting dma_map_single()................\n\n");
	kbuf = kmalloc(size, GFP_KERNEL);
	handle = dma_map_single(my_dev, kbuf, size, direction);
	output(kbuf, handle, size,
	       "This is the dma_map_single() string");
	dma_unmap_single(my_dev, handle, size, direction);
	kfree(kbuf);

	/* dma_pool method */

	dev_info(my_dev, "\nTesting dma_pool_alloc()..........\n\n");
	mypool =
	    dma_pool_create("mypool", my_dev, pool_size, pool_align, 0);
	kbuf = dma_pool_alloc(mypool, GFP_KERNEL, &handle);
	if (kbuf == NULL) {
		dev_err(my_dev, "dma_pool_alloc() returned NULL\n");
	} else {
		output(kbuf, handle, size,
		       "This is the dma_pool_alloc() string");
	}
	dma_pool_free(mypool, kbuf, handle);
	dma_pool_destroy(mypool);

	return rc;
}

module_init(my_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_AUTHOR("Bill Kerr");
MODULE_AUTHOR("John Bonesio");
MODULE_DESCRIPTION("LFD5435:5.4 s_26/lab_dma.c");
MODULE_LICENSE("GPL v2");

