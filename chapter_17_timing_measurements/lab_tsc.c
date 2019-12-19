/* **************** LFD5435:5.4 s_17/lab_tsc.c **************** */
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
 * Using the Time Stamp Counter
 *
 *  Construct a program that uses the TSC to measure elapsed time.
 *
 * Bracket a task (which could be as simple as sleep()) with the
 * rdtsc() macro, and get the number of processor cycles that have
 * elapsed.
 *
 * You'll have to calibrate by reading the CPU
 * MHZ value from /proc/cpuinfo.
 *
 * Try using gettimeofday() for the same
 * purpose.  How do the two values compare?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/* This instructions are architecture specific, in this case x86 */
#define rdtsc(low, high) \
	__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))
#define rdtscl(low) \
	__asm__ __volatile__("rdtsc" : "=a" (low) : : "edx")
#define rdtscll(val) \
	__asm__ __volatile__("rdtsc" : "=A" (val))

#define BUFLEN 4096

double getmhz(void)
{
	char *buf;
	int fd, j;
	double speed;

	buf = malloc(BUFLEN);

	fd = open("/proc/cpuinfo", O_RDONLY);
	if (fd < 0)
		DEATH("Couldnt open /proc/cpuinfo");

	if (read(fd, buf, BUFLEN) <= 0)
		DEATH("failed to read /proc/cpuinfo");

	close(fd);

	for (j = 0; j < BUFLEN; j++, buf++) {
		if (!strncmp(buf, "cpu MHz", 7)) {
			while (*buf != ':')
				buf++;
			buf++;
			speed = atof(buf);
			return speed;
		}
	}
	return 0.0;
}

#define NLOOP 10
int main(int argc, char *argv[])
{
	int j, mhz;
	unsigned long long start, end, diff;
	double speed, seconds, seconds_tv, sav1 = 0.0, sav2 = 0.0;
	struct timeval tv1, tv2;

	/* find out our MHZ rate */

	speed = getmhz();
	printf("CPU speed = %f MHZ\n", speed);

	for (j = 0; j < NLOOP; j++) {
		gettimeofday(&tv1, NULL);

		rdtscll(start);
		sleep(1);
		rdtscll(end);

		gettimeofday(&tv2, NULL);

		diff = end - start;
		mhz = diff / 1000000;

		seconds = 1e-6 * diff / speed;
		seconds_tv =
		    (tv2.tv_sec - tv1.tv_sec) + 1e-6 * (tv2.tv_usec -
							tv1.tv_usec);
		printf("TSC start:end:elapsed= %15lld:%15lld:%15lld\n"
		       "        SECS(TSC)=%15f, SECS(TV)=%15f,   diff=%15f, mhz=%d\n\n",
		       start, end, diff, seconds, seconds_tv,
		       seconds - seconds_tv, mhz);
		sav1 += seconds;
		sav2 += seconds_tv;
	}
	printf("\nAverages: SECS(TSC)= %15f,  SECS(TV)= %15f,  diff=%15f\n",
	       sav1 / NLOOP, sav2 / NLOOP, (sav1 - sav2) / NLOOP);
	exit(EXIT_SUCCESS);
}



