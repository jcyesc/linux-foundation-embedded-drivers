/* **************** LFD5435:5.4 s_23/lab_write.c **************** */
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
 * Basic write program USAGE: foo [filename(def=/dev/mycdrv)]
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
	int fd, rc;
	char *buffer = "TESTING CHAR DRIVER WITH A WRITE", *filename =
	    "/dev/mycdrv";

	if (argc > 1)
		filename = argv[1];

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	rc = write(fd, buffer, strlen(buffer));
	printf("process %d wrote %d bytes to %s\n", getpid(), rc, filename);
	close(fd);
	exit(EXIT_SUCCESS);
}
