/* **************** LFD5435:5.4 s_19/lab_ioctl_test.c **************** */
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
 *  simple user-mode program to use the lab_ioctl interface
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>		/* this needed for ioctl(2) syscall */
#include <string.h>		/* needed for strerror(3) */
#include <errno.h>		/* needed for errno variable */
#include <stdint.h>		/* for uint32_t and friends */

#include "basic_ioctl.h"		/* my_ioctl interface definitions */

char *dev = "/dev/basic_ioctl";	/* default */

struct my_ioctl_data my_data;

int main(int argc, char **argv)
{
	int fd;

	if (argc > 1)
		dev = argv[1];


	printf("Opening %s device\n", dev);
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "can't open %s:  %s\n", dev, strerror(errno));
		exit(1);
	}

	printf("MY_CMD_CONFIG value is %lu\n", MY_CMD_CONFIG);
	/* set the device configuration */
	my_data.csr.control_reg_value = MY_CTRL_VAMPIRE;
	sprintf(my_data.message, "I vant your blood");
	if (ioctl(fd, MY_CMD_CONFIG, &my_data) < 0) {
		fprintf(stderr, "CONFIG ioctl failed:  %s\n", strerror(errno));
		exit(1);
	}
	printf("after CONFIG, status register is 0x%x.  Bitwise AND with "
	       "control:  0x%x\n",
	       my_data.csr.status_reg_value,
	       my_data.csr.status_reg_value & MY_CTRL_VAMPIRE);
	fflush(stdout);

	if (ioctl(fd, MY_CMD_START) < 0) {
		fprintf(stderr, "START ioctl failed:  %s\n", strerror(errno));
		exit(1);
	}
	printf("successfully started the device\n");
	fflush(stdout);

	if (ioctl(fd, MY_CMD_STOP) < 0) {
		fprintf(stderr, "STOP ioctl failed:  %s\n", strerror(errno));
		exit(1);
	}
	printf("successfully stopped the device\n");
	fflush(stdout);

	close(fd);
	printf("sizeof(uint32_t) is %ld\n", sizeof(uint32_t));
	return 0;		/* success */
}
