/*
 * The main file of the pellet burner control program
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "serial.h"

static char *device = "/dev/ttyUSB0";

int main(int argc, char *argv[])
{
    char chr;
    int c, fd;
    int errflg;
    extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, "d:")) != -1)
    {
        switch (c) {
            case 'd':
                printf ("Device: '%s', length %d\n", optarg, strlen(optarg));
                device = (char *)calloc (1, strlen(optarg)+1);
                strncpy (device, optarg, strlen(optarg));
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                errflg++;
        }
    }
    printf ("Using serial device %s\n", device);
    fd = serial_create (device);
    if (fd < 0)
        exit (1);

    while (1) {
        if (read(fd, &chr, 1) == 1) {
            printf("Value: 0x%02x", chr);
            if ((chr & 0xa2) == 0xa2) printf(" Sync");
            printf("\n");
        }
    }
}
