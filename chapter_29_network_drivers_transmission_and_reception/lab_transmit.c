/* **************** LFD5435:5.4 s_29/lab_transmit.c **************** */
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
 * Building a Transmitting Network Driver
 *
 * Extend your stub network device driver to include a transmission
 * function, which means supplying a method for
 * ndo_start_xmit().
 *
 * While you are at it, you may want to add other entry points to see
 * how you may exercise them.
 *
 * Once again, you should be able to exercise it with:
 *
 *   insmod lab_network.ko
 *   ifconfig mynet0 up 192.168.3.197
 *   ping -I mynet0 localhost
 *       or
 *   ping -bI mynet0 192.168.3
 *
 * Make sure your chosen address is not being used by anything else.
 *
 */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/version.h>

static struct net_device *dev;
static struct net_device_stats *stats;

static void printline(unsigned char *data, int n)
{
	char line[256], entry[16];
	int j;

	strcpy(line, "");
	for (j = 0; j < n; j++) {
		sprintf(entry, " %2x", data[j]);
		strcat(line, entry);
	}
	netdev_info(dev, "%s\n", line);
}

static int my_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int i;

	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);

	netif_trans_update(dev);
	netdev_info(dev, "Sending packet :\n");

	/* print out 16 bytes per line */

	for (i = 0; i < skb->len; i += 16)
		printline(&skb->data[i],
			  (skb->len - i) < 16 ? skb->len - i : 16);

	netdev_info(dev, "\n");

	dev_kfree_skb(skb);
	stats->tx_bytes += skb->len;
	++stats->tx_packets;

	return 0;
}

static int my_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	netdev_info(dev, "%s(%s) 0x%08X\n", __func__, dev->name, cmd);
	return -1;
}

static struct net_device_stats *my_get_stats(struct net_device *dev)
{
	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);
	return stats;
}

/*
 * This is where ifconfig comes down and tells us who we are, etc.
 * We can just ignore this.
 */
static int my_config(struct net_device *dev, struct ifmap *map)
{
	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);
	if (dev->flags & IFF_UP)
		return -EBUSY;
	return 0;
}

static int my_change_mtu(struct net_device *dev, int new_mtu)
{
	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);
	return -1;
}

static int my_open(struct net_device *dev)
{
	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);

	/* start up the transmission queue */

	netif_start_queue(dev);
	return 0;
}

static int my_close(struct net_device *dev)
{
	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);

	/* shutdown the transmission queue */

	netif_stop_queue(dev);
	return 0;
}

static const struct net_device_ops ndo = {
	.ndo_open = my_open,
	.ndo_stop = my_close,
	.ndo_start_xmit = my_start_xmit,
	.ndo_do_ioctl = my_do_ioctl,
	.ndo_get_stats = my_get_stats,
	.ndo_set_config = my_config,
	.ndo_change_mtu = my_change_mtu,
};

static void my_setup(struct net_device *dev)
{
	int j;

	netdev_info(dev, "Hit: %s(%s)\n", __func__, dev->name);

	/* Fill in the MAC address with a phoney */

	for (j = 0; j < ETH_ALEN; ++j)
		dev->dev_addr[j] = (char)j;

	ether_setup(dev);

	dev->netdev_ops = &ndo;
	dev->flags |= IFF_NOARP;
	stats = &dev->stats;

	/*
	 * Just for laughs, let's claim that we've seen 50 collisions.
	 */
	stats->collisions = 50;
}

static int __init my_init(void)
{
	pr_info("Loading transmitting network module:....");
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)
	dev = alloc_netdev(0, "mynet%d", my_setup);
#else
	dev = alloc_netdev(0, "mynet%d", NET_NAME_ENUM, my_setup);
#endif
	if (register_netdev(dev)) {
		pr_info(" Failed to register\n");
		free_netdev(dev);
		return -1;
	}
	netdev_info(dev, "Succeeded in loading %s!\n\n", dev_name(&dev->dev));
	return 0;
}

static void __exit my_exit(void)
{
	netdev_info(dev, "Unloading transmitting network module\n\n");
	unregister_netdev(dev);
	free_netdev(dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LFD5435:5.4 s_29/lab_transmit.c");
MODULE_LICENSE("GPL v2");


