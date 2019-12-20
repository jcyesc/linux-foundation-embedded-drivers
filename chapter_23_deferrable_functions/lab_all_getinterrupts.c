/* **************** LFD5435:5.4 s_23/lab_all_getinterrupts.c **************** */
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
/* Sharing All Interrupts, Bottom Halves (get results application)
 *
 * Extend the solution to share all possible interrupts, and evaluate
 * the consumer/producer problem.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

#define MAXIRQS 256
#define NB (MAXIRQS * sizeof(int))

int main(int argc, char *argv[])
{
	int fd, j;
	char *nodename = "/dev/mycdrv";
	int *interrupts = malloc(NB);
	int *bhs = malloc(NB);

	if (argc > 1)
		nodename = argv[1];

	fd = open(nodename, O_RDONLY);
	if (fd < 0)
		DEATH("opening device node");
	if (read(fd, interrupts, NB) != NB)
		DEATH("reading interrupts");
	if (read(fd, bhs, NB) != NB)
		DEATH("reading bhs");

	for (j = 0; j < MAXIRQS; j++)
		if (interrupts[j] > 0)
			printf(" %4d %10d%10d\n", j, interrupts[j], bhs[j]);
	exit(EXIT_SUCCESS);
}
