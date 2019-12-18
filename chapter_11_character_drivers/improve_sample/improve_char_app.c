/* **************** LFD5435:5.4 s_11/lab_char_test.c **************** */
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
 *  Basic read/write program
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int length, fd1, fd2, rc;
	char *nodename = "/dev/improve_sample_char_driver";
	char message[] = " *** TESTING CHAR/DRIVER ***\n";

	length = sizeof(message);

	if (argc > 1)
		nodename = argv[1];

	fd1 = open(nodename, O_RDWR);
	printf("Opened file descriptor first time  = %d\n", fd1);
	fd2 = open(nodename, O_RDWR);
	printf("Opened file descriptor second time  = %d\n", fd2);

	rc = write(fd1, message, length);
	printf("Return code from write = %d on %d, message=%s\n", rc, fd1,
	       message);

	memset(message, 0, length);

	rc = read(fd2, message, length);
	printf("Return code from read  = %d on %d, message=%s\n", rc, fd2,
	       message);

	close(fd1);
	exit(EXIT_SUCCESS);
}

