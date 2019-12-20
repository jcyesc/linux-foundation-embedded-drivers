/* **************** LFD5435:5.4 s_23/lab_uio_api_test.c **************** */
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
 * The UIO API (test program)
 *
 */
#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>

int main(void)
{
	int fd;
	unsigned long nint;

	fd = open("/dev/uio0", O_RDONLY);
	if (fd < 0) {
		perror("Failed to open /dev/uio0\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Started uio test driver.\n");
	while (read(fd, &nint, sizeof(nint)) >= 0)
		fprintf(stderr, "Interrupts: %ld\n", nint);
	exit(EXIT_SUCCESS);
}
