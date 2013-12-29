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
#include "lc1.h"
#include "tpl.h"

static float target_lambda = 2.2;
static lambda_ipc_t lambda_ipc =  {
        .sync_word = 0xfade
};

static uint8_t read_chr(int fd)
{
    uint8_t c;

    read(fd, &c, 1);

    return c;
}

static uint16_t get_frame_sync (int fd)
{
    uint8_t x, y;

    do {
        // Wait for synch character 1
        while (((x = read_chr(fd)) & ISP2_PROTOCOL_SYNC1) != ISP2_PROTOCOL_SYNC1);
        y = read_chr(fd);
    } while (!(y & ISP2_PROTOCOL_SYNC2));
    return (x << 8) | y;
}

/*
 * Read two characters from the serial stream and pack them
 * as a 16 bit word.
 */
static uint16_t get_data_word (int fd)
{
    return (read_chr(fd) << 8) | read_chr(fd);
}

/*
 * Process the current LC-1 frame
 */
static int process_frame(int fd)
{
    return 0;
}

/*
 * Creates a LC-1 listener.
 */
pid_t create_lc1_listener (char *device, int *pipe)
{
    pid_t lc1_PID;
    int fd;
    uint16_t synch_word, status_word, raw_lambda;
    int pack_len;
    int func2, afr;
    int int_lambda;
    float lambda;
    int state = ISP2_STATE_NO_STATE;
    int burner_state = BURNER_STATE_UNKNOWN;

    errno = 0;
    lc1_PID = fork();
    if (lc1_PID >= 0) {
        if (lc1_PID == 0) {
            close (pipe[0]);
            /* This is the child process */
            printf ("\nUsing serial device %s\n", device);
            fd = serial_create (device);
            if (fd < 0)
                _exit (1);

            while (1) {
                synch_word = get_frame_sync(fd);
                /* Skip command replies */
                if (synch_word & ISP2_COMMAND_DATA) {
                    /* Get status word and unpack fields */
                    status_word = get_data_word(fd);
                    /* Get raw lambda LC-1 value */
                    raw_lambda = get_data_word(fd);

                    /* Unpack packet length */
                    pack_len = (synch_word & ISP2_PACKET_LENGTH_MASK) |
                            ((synch_word & ISP2_PACKET_LENGTH_BIT7_MASK) >>
                                    ISP2_PACKET_LENGTH_BIT7_SHIFT_STEPS);

                    /* Unpack function bits */
                    func2 = (status_word & ISP2_FUNC2_MASK) >> ISP2_FUNC2_SHIFT_STEPS;

                    /* Unpack AFR multiplier */
                    afr = (status_word & ISP2_AFR_MUL_MASK) |
                            ((status_word & ISP2_AFR_MUL_BIT7_MASK) >> ISP2_AFR_MUL_BIT7_SHIFT_STEPS);

                    /* And finally unpack the lambda word */
                    int_lambda = (raw_lambda & ISP2_LAMBDA_MASK) |
                            ((raw_lambda & ISP2_LAMBDA_TOP_MASK) >> ISP2_LAMBDA_TOP_SHIFT_STEPS);

                    switch (func2) {
                        case 0:
                            lambda = ((float)int_lambda + 500) * ((float)afr / 10) / 10000;
                            lambda_ipc.func = state = ISP2_STATE_LAMBDA;
                            break;

                        case 1:
                            lambda = (float)int_lambda / 10;
                            lambda_ipc.func = state = ISP2_STATE_PERCENT;
                            break;

                        default:
                            lambda_ipc.func = state = ISP2_STATE_NO_STATE;
                            lambda_ipc.lambda = (uint16_t)int_lambda;
                            break;
                    }
                }

                if (state != ISP2_STATE_NO_STATE) {
                    tpl_node *tn;

                    lambda_ipc.func = func2;
                    // Send lambda as int16 with two digits precision
                    lambda_ipc.lambda = (int16_t)(lambda *100);
                    tn = tpl_map("S(vvj)", &lambda_ipc);
                    tpl_pack(tn, 0);
                    tpl_dump(tn, TPL_FD, pipe[1]);
                    tpl_free(tn);
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
