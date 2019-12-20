/* **************** LFD5435:5.4 s_22/lab_read.c **************** */
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
 * Basic read program USAGE: foo [filename(def=/dev/mycdrv)] [nbytes(def=4096)]
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd, rc, nbytes = 32;
	char *buffer, *filename = "/dev/mycdrv";

	if (argc > 1)
		filename = argv[1];
	if (argc > 2)
		nbytes = atoi(argv[2]);
	buffer = malloc(nbytes);

	fd = open(filename, O_RDONLY);
	printf("opened file: %s,  with file descriptor = %d\n", filename, fd);
	rc = read(fd, buffer, nbytes);
	printf("read %d bytes which were:\n%s\n", rc, buffer);
	close(fd);
	exit(EXIT_SUCCESS);
}
