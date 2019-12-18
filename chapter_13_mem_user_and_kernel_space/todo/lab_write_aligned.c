/* **************** LFD5435:5.4 s_13/lab_write_aligned.c **************** */
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
 * Basic page-aligned write program USAGE: foo [filename(def=/dev/mycdrv)]
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>

#define STRING "TESTING DRIVER WITH A WRITE"

int main(int argc, char *argv[])
{
	int fd, rc = 0, size, pagesize;
	void *buffer;
	char *filename = "/dev/mycdrv";

	size = strlen(STRING) + 1;	/* leave room for \0 */
	pagesize = getpagesize();
	rc = posix_memalign(&buffer, pagesize, size);
	/*    buffer = (char *)memalign (pagesize, size); */
	printf("rc=%d, buffer=%p, mod pagesize = %ld\n", rc, buffer,
	       (unsigned long)buffer % pagesize);
	strcpy(buffer, STRING);

	if (argc > 1)
		filename = argv[1];

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	printf("opened file: %s,  with file descriptor = %d\n", filename, fd);
	rc = write(fd, buffer, strlen(buffer) + 1);
	printf("wrote %d bytes which were:\n%s\n", rc, (char *)buffer);
	close(fd);
	exit(EXIT_SUCCESS);
}
