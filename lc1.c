/*
 * lc1.c
 *
 *  Created on: 28 dec 2013
 *      Author: pontus
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "serial.h"

/*
 * Creates a LC-1 listener.
 */
pid_t create_lc1_listener (char *device)
{
    pid_t lc1_PID;
    int fd;
    char chr;

    errno = 0;
    lc1_PID = fork();
    if (lc1_PID >= 0) {
        if (lc1_PID == 0) {
            /* This is the child process */
            printf ("Using serial device %s\n", device);
            fd = serial_create (device);
            if (fd < 0)
                _exit (1);

            while (1) {
                if (read(fd, &chr, 1) == 1) {
                    printf("Value: 0x%02x", chr);
                    if ((chr & 0xa2) == 0xa2) printf(" Sync");
                    printf("\n");
                    if (chr == 0x01) break;
                }
            }
            close(fd);
            fflush(stdout);
            _exit(0);
        } else {
            /* Parent process returns with the PID of the new child process */
            return lc1_PID;
        }
    }
    /* We'll use errno to inform the caller that the creation failed */
    errno = -1;
    return 0;
}
