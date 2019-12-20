/* **************** LFD5435:5.4 s_27/lab_network.c **************** */
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
 * Building a Basic Network Driver Stub
 *
 * Write a basic network device driver.
 *
 * It should register itself upon loading, and unregister upon
 * removal.
 *
 * Supply minimal open() and stop() methods.
 *
 * You should be able to exercise it with:
 *
 *   $  sudo insmod lab_network.ko
 *   $  sudo ifconfig mynet0 up 192.168.3.197
 *   $  sudo ifconfig mynet0
 *   $  sudo ifconfig mynet0 down
 *   $  ping -I mynet0 localhost
 *   $  sudo rmmod basic_network_driver
 *
 *   or
 *
 *   $ sudo insmod lab_network.ko
 *   $ sudo ip link set mynet0 up
 *   $ sudo ip addr add 192.168.3.197 dev eth0
 *   $ ip link show
 *
 * Make sure your chosen address is not being used by anything else.
 *
 * Warning: Depending on kernel version, your stub driver may crash if
 * you try to bring it up or ping it.  If you put in a trivial
 * transmit function, such as
 *
 * static int stub_start_xmit (struct sk_buff *skb, struct net_device *dev)
 * {
 *   dev_kfree_skb (skb);
 *   return 0;
 * }
 * this should avoid the problems.
 *
 */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/version.h>

static struct net_device *dev;

static int net_open(struct net_device *dev)
{
	netdev_info(dev, "net_open() Hit: %s(%s)\n", __func__, dev->name);

	/* start up the transmission queue */

	netif_start_queue(dev);
	return 0;
}

static int net_close(struct net_device *dev)
{
	netdev_info(dev, "net_close() Hit: %s(%s)\n", __func__, dev->name);

	/* shutdown the transmission queue */

	netif_stop_queue(dev);
	return 0;
}

/* Note this method is only needed on some; without it
 *  module will fail upon removal or use. At any rate there
 *  is a memory leak whenever you try to send a packet
 *  through in any case
 */

static int stub_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	netdev_info(dev, "stub_start_xmit() is executing\n");
	dev_kfree_skb(skb);
	return 0;
}

static const struct net_device_ops ndo = {
	.ndo_open = net_open,
	.ndo_stop = net_close,
	.ndo_start_xmit = stub_start_xmit,
};

static void my_setup(struct net_device *dev)
{
	int j;

	netdev_info(dev, "%s(%s)\n", __func__, dev->name);

	/* Fill in the MAC address with a phoney */
	for (j = 0; j < ETH_ALEN; ++j)
		dev->dev_addr[j] = (char) j;

	ether_setup(dev);
	dev->netdev_ops = &ndo;
}

static int __init my_init(void)
{
	pr_info("Loading stub network module:....");
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)
	dev = alloc_netdev(0, "mynet%d", my_setup);
#else
	dev = alloc_netdev(0, "mynet%d", NET_NAME_ENUM, my_setup);
#endif
	if (register_netdev(dev)) {
		pr_err(" Failed to register\n");
		free_netdev(dev);
		return -1;
	}
	netdev_info(dev, "Succeeded in loading %s!\n\n", dev_name(&dev->dev));
	return 0;
}

static void __exit my_exit(void)
{
	netdev_info(dev, "Unloading stub network module\n\n");
	unregister_netdev(dev);
	free_netdev(dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_AUTHOR("Tatsuo Kawasaki");
MODULE_DESCRIPTION("LFD5435:5.4");
MODULE_LICENSE("GPL v2");


