/* **************** LFD5435:5.4 s_10/lab_wastemem.c **************** */
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
/* simple program to defragment memory, J. Cooperstein 2/04
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

#define MB (1024*1024)
#define BS 16			/* will allocate BS*MB at each step */
#define CHUNK (MB*BS)

int main(int argc, char **argv)
{
	struct sysinfo si;
	int j, m;
	char *c;

	/* get total memory on the system */
	sysinfo(&si);
	m = si.totalram / MB;
	printf("Total System Memory in MB = %d MB\n", m);
	m = (9 * m) / 10;	/* drop 10 percent */
	printf("Using somewhat less: %d MB\n", m);

	if (argc == 2) {
		m = atoi(argv[1]);
		printf("Choosing instead mem = %d MB\n", m);
	}

	for (j = 0; j <= m; j += BS) {
		/* yes we know this is a memory leak, no free,
		 * that's the idea!
		 */
		c = malloc(CHUNK);
		/* just fill the block with j over and over */
		memset(c, j, CHUNK);
		printf("%8d", j);
		fflush(stdout);
	}
	printf("\nAll memory allocated, pausing 5 seconds\n");
	sleep(5);
	printf("Quitting and releasing memory\n");
	exit(EXIT_SUCCESS);
}
