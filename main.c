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

#include "tpl.h"
#include "lc1.h"

static char *device = "/dev/ttyUSB0";
static lambda_ipc_t lambda_ipc =  {
        .sync_word = 0xfade
};

int main(int argc, char *argv[])
{
    int c;
    int errflg;
    extern char *optarg;
    extern int optind, optopt;
    int childstat;
    // Comm pipe for the lc1 process
    int lc1pipe[2];


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

    // Create comms pipe
    pipe(lc1pipe);

    if (!create_lc1_listener (device, lc1pipe)) {
        printf ("Could not create a LC-1 listener so I need to terminate !\n");
        close (lc1pipe[0]);
        close (lc1pipe[2]);
        exit (1);
    }

    close (lc1pipe[1]);
    while (1) {
        tpl_node *tn;

        tn = tpl_map("S(vvj)", &lambda_ipc);
        tpl_load(tn, TPL_FD, lc1pipe[0]);
        tpl_unpack(tn, 0);
        tpl_free(tn);

        switch (lambda_ipc.func) {
            case 0:
                printf ("Lambda value: %d.%d\n", lambda_ipc.lambda / 100, lambda_ipc.lambda - ((lambda_ipc.lambda / 100) * 100));
                break;

            case 1:
                printf ("Lambda value: %d.%d\%\n", lambda_ipc.lambda / 100, lambda_ipc.lambda - ((lambda_ipc.lambda / 100) * 100));
                break;

            default:
                printf ("Error: Synch Word: 0x%04x, Func: %d\n", lambda_ipc.sync_word, lambda_ipc.func);
                break;
        }
    }
    wait (&childstat);
    printf ("Parent process terminating !\n");

    return 0;
}
