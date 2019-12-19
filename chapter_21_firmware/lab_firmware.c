/* **************** LFD5435:5.4 s_21/lab_firmware.c **************** */
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
 * Loading Firmware (create device solution)

 * Write a module that loads some firmware from the filesystem. It
 * should print out the contents.
 *
 * In order to do this you'll need to place a firmware file under
 * /lib/firmware. You can use just a text file for the purpose of this
 * demonstration.

 * Since this is a pseudo device, you will have to declare and
 * initialize a device structure. Minimally you must set the void
 * (*release)(struct device *dev) field in this structure, and call
 *
 *    int dev_set_name (struct device *dev, const char *fmt, ...);
 *
 * to set the device name, which can be read out with:
 *
 *    const char *dev_name(const struct device *dev);
 *
 * (You may want to see what happens if you neglect setting one of
 * these quantities.)
 *
 * Make sure you call
 *
 *    device_register  (struct device *dev);
 *    device_unregister(struct device *dev);
 *
 * before requesting and after releasing the firmware.
 *
 * As an alternative (and easier solution) just adapt your previous
 * miscellaneous or character driver to do this.
 *
 * To execute the driver run:
 *
 * sudo vim /lib/firmware/my_fwfile
 * sudo insmod lab_firmware.ko
 * ls
 * dmesg
 * more /lib/firmware/my_fwfile
 * dmesg
 * sudo rmmod lab_firmware
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/firmware.h>

#define FWFILE "my_fwfile"
/*
 * The firmware struct can be found in:
 *
 * https://docs.huihoo.com/doxygen/linux/kernel/3.7/structfirmware.html
 */
static const struct firmware *fw;

static void my_release(struct device *dev)
{
	dev_info(dev, "releasing firmware device\n");
}

static struct device dev = {
	.release = my_release
};

static int __init my_init(void)
{
	dev_set_name(&dev, "my0");

	if (device_register(&dev) < 0) {
		pr_info("Failing to register firmware device\n");
		return -1;
	}
	dev_info(&dev, "firmware_example: my device inserted\n");

	if (request_firmware(&fw, FWFILE, &dev)) {
		pr_err("requesting firmware failed\n");
		device_unregister(&dev);
		return -1;
	}

	dev_info(&dev, "firmware size = {%d} contents={%s}\n", (int) fw->size, fw->data);
	return 0;
}

static void __exit my_exit(void)
{
	release_firmware(fw);
	dev_info(&dev, "release firwmare and unregistered device\n");
	device_unregister(&dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_FIRMWARE(FWFILE);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LFD5435:5.4 s_21/lab_firmware.c");
MODULE_AUTHOR("Jerry Cooperstein");

