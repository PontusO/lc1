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
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <glib.h>

#include "lc1.h"

static char *device = "/dev/ttyUSB0";

int main(int argc, char *argv[])
{
    int c;
    int errflg;
    extern char *optarg;
    extern int optind, optopt;
    int childstat;

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

    if (!create_lc1_listener (device)) {
        printf ("Could not create a LC-1 listener so I need to terminate !\n");
        exit (1);
    }

    wait (&childstat);
    printf ("Parent process terminating !\n");

    return 0;
}
