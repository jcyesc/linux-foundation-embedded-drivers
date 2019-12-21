/* **************** LFD5435:5.4 s_32/lab_usb.c **************** */
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
 * Installing a USB device
 *
 * We are going to write a simple USB device driver.
 *
 * The driver should register itself with the USB sub-system upon
 * loading and unregister upon unloading.
 *
 * The probe() and disconnect() functions should issue printout
 * whenever the device is added or removed from the system.
 *
 * By proper use of the usb_device_id table, you can
 * configure your driver either to sense any device plugged, or only a specific one.
 * You can obtain the vendor and device ID's by noting the output when the
 * USB sub-system senses device connection.
 *
 * You will have to make sure your kernel has the proper USB support
 * compiled in, and that no driver for the device is already loaded,
 * as it may interfere with your driver claiming the device.
 *
 * Probably what you will want to do is first load your module and
 * then plug in the device and see if it claimed.  You may have to
 * make sure no currently loaded modules (such as usb_storage claim
 * your device.  Then you should install your module and see if it can
 * be automatically found and loaded when the device is plugged in.  *
 * Hint: You'll probably want to do a make modules_install to get *
 * automatic loading to work properly.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/version.h>

static struct device *my_dev;

struct my_usb_info {
	int connect_count;
};

static int
my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct my_usb_info *usb_info;
	struct usb_device *dev = interface_to_usbdev(intf);
	static int my_counter;

	my_dev = &dev->dev;

	dev_info(my_dev, "\nMY_USB:%s\n", __func__);
	dev_info(my_dev, "MY_USB:devnum=%d, speed=%d\n", dev->devnum,
		 (int)dev->speed);
	dev_info(my_dev,
		 "MY_USB:idVendor=0x%hX, idProduct=0x%hX, bcdDevice=0x%hX\n",
		 dev->descriptor.idVendor, dev->descriptor.idProduct,
		 dev->descriptor.bcdDevice);
	dev_info(my_dev, "MY_USB:class=0x%hX, subclass=0x%hX\n",
		 dev->descriptor.bDeviceClass, dev->descriptor.bDeviceSubClass);
	dev_info(my_dev, "MY_USB:protocol=0x%hX, packetsize=%hu\n",
		 dev->descriptor.bDeviceProtocol,
		 dev->descriptor.bMaxPacketSize0);
	dev_info(my_dev,
		 "MY_USB:manufacturer=0x%hX, product=0x%hX, serial=%hu\n",
		 dev->descriptor.iManufacturer, dev->descriptor.iProduct,
		 dev->descriptor.iSerialNumber);

	usb_info = kmalloc(sizeof(struct my_usb_info), GFP_KERNEL);
	usb_info->connect_count = my_counter++;
	usb_set_intfdata(intf, usb_info);
	dev_info(my_dev, "MY_USB:connect_count=%d\n\n",
		 usb_info->connect_count);

	return 0;
}

static void my_usb_disconnect(struct usb_interface *intf)
{
	struct my_usb_info *usb_info;

	usb_info = usb_get_intfdata(intf);
	dev_info(my_dev, "\nMY_USB:%s\n", __func__);
	kfree(usb_info);
}

static struct usb_device_id my_usb_table[] = {
	{USB_DEVICE(0x0545, 0x808a)},	/* Veo USB camera */
	{USB_DEVICE(0x046D, 0x870)},	/* Labtec WebCam */
	{USB_DEVICE(0x204, 0x6025)},	/* Via USB PEN  */
	{USB_DEVICE(0x0ace, 0x1215)},	/* Zonet Wireless */
	{USB_DEVICE(0x058f, 0x6387)},	/* Penguin(ALCOR) USB PEN  */
	{USB_DEVICE(0x0a12, 0x0001)},	/* Cambridge Silicon Radio,
					 * Ltd Bluetooth Dongle (HCI mode)
					 */
	{}			/* Null terminator (required) */
};

MODULE_DEVICE_TABLE(usb, my_usb_table);

static struct usb_driver my_usb_driver = {
	.name = "usb-hotplug",
	.probe = my_usb_probe,
	.disconnect = my_usb_disconnect,
	.id_table = my_usb_table,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0)
static int __init my_init_module(void)
{
	pr_info("Hello USB\n");
	return usb_register(&my_usb_driver);
}

static void my_cleanup_module(void)
{
	pr_info("Goodbye USB\n");
	usb_deregister(&my_usb_driver);
}

module_init(my_init_module);
module_exit(my_cleanup_module);
#else
module_usb_driver(my_usb_driver);
#endif

MODULE_AUTHOR("Terry Griffin");
MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_32/lab_usb.c");
MODULE_LICENSE("GPL v2");

