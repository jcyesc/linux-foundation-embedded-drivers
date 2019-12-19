/* **************** LFD5435:5.4 s_19/lab_ioctl.h **************** */
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
 *  This header is used by both the kernel module and all user-mode
 *  application programs.
 *
 *  Each class of program (user/kernel) must include the proper ioctl.h
 *  header file that defines _IOC_DIR, _IOWR, etc.
 */

#define MY_IOCTL_TYPE  '9'


// For user applications, uncomment this.
// #include <stdint.h>

struct my_ioctl_data {
	union {
		uint32_t control_reg_value;	/* passed into kernel */
		uint32_t status_reg_value;	/* passed from kernel */
	} csr;
	char message[32];	/* passed into kernel */
};

/* ioctl "cmd" values */

#define MY_CMD_CONFIG _IOWR(MY_IOCTL_TYPE, 1, struct my_ioctl_data)
#define MY_CMD_START  _IO(MY_IOCTL_TYPE, 2)
#define MY_CMD_STOP   _IO(MY_IOCTL_TYPE, 3)

/* here are the legal values for control_reg_value */
#define MY_CTRL_VAMPIRE   0x394828	/* vampire mode */
#define MY_CTRL_UMPIRE    0x438949	/* umpire mode */
#define MY_CTRL_PERSPIRE  0x383054	/* perspire mode */
