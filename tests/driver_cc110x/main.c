/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the CC110x driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

#define MAIN_QUEUE_SIZE     (8)

static int sc_dump(int argc, char **argv);
int sc_cc110x(int argc, char **argv);
int sc_cc110x_sleep(int argc, char **argv);
int sc_cc110x_wakeup(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "dump", "Enable/disable dumping of frames", sc_dump },
    { "cc110x", "Print the low level state of an CC110x device", sc_cc110x },
    { "sleep", "Set CC110x onto sleep mode", sc_cc110x_sleep },
    { "wakeup", "Wake-up CC110x device", sc_cc110x_wakeup },
    { NULL, NULL, NULL }
};

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static gnrc_netreg_entry_t dump;

static int sc_dump(int argc, char **argv)
{
    static int is_enabled = 0;
    if (argc == 1) {
        if (is_enabled) {
            puts("Currently dumping packets");
        }
        else {
            puts("Currently NOT dumping packets");
        }
        return 0;
    }
    else if (argc == 2) {
        int new_state = 0;
        if (!strcmp("y", argv[1])) {
            new_state = 1;
        }
        else if (!strcmp("n", argv[1])) {
            new_state = 0;
        }
        else {
            printf("Usage: %s [y/n]\n", argv[0]);
            return 0;
        }
        if (new_state == is_enabled) {
            // Nothing to do;
            return 0;
        }

        if (new_state) {
            if (gnrc_netreg_register(GNRC_NETTYPE_SIXLOWPAN, &dump)) {
                puts("Failed to register packet dumping");
            }
        }
        else {
            gnrc_netreg_unregister(GNRC_NETTYPE_SIXLOWPAN, &dump);
        }

        is_enabled = new_state;
        return 0;
    }

    printf("Usage: %s [y/n]\n", argv[0]);
    return 0;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    gnrc_netreg_entry_init_pid(&dump, GNRC_NETREG_DEMUX_CTX_ALL,
                               gnrc_pktdump_pid);

    puts("cc110x driver test application\n"
         "==============================\n"
         "\n"
         "Use the shell and two boards equipped with an CC1100/CC1101\n"
         "transceiver to test the driver. Common testing tasks:\n"
         "\n"
         "- Using \"ifconfig\":\n"
         "    - Check the information stated for plausibility/correctness\n"
         "    - Try to get/set parameters like TX power, channel, address, ...\n"
         "    - BEWARE: With short communication distances (<=1m) for boards\n"
         "      with high gain antennas a high TX power may result in packet\n"
         "      loss: The incoming signal can only be demodulated when the\n"
         "      input signal is at most +10 dBm on the CC1101.\n"
         "    - Check the statistics for correctness/plausibility (after\n"
         "      sending frames using \"txtsnd\" or \"ping\")\n"
         "- Using \"ping\":\n"
         "    - Does the other device respond to the ping?\n"
         "    - Does the measured RSSI increase when the nodes are closer\n"
         "      together?\n"
         "    - Try to increase the size of the pings, so that the TX/RX FIFO\n"
         "      needs to be filled/drain more than once per frame. The TX/RX\n"
         "      FIFO can hold 64 bytes\n"
         "    - Try to increase the size of the pings in order to trigger L2\n"
         "      fragmentation. The driver supports frames of up to 255 bytes\n"
         "- Using \"txtsnd\":\n"
         "    - Turn on packet dumping using the command \"dump y\" on node A\n"
         "    - Send both unicast and broadcast frame from node B to A\n"
         "- Using \"cc110x\":\n"
         "    - This tool will print low level details for all CC110x devices\n"
         "      attached\n"
         "    - This will be mostly useful for debugging, not for testing\n"
         "- Using \"sleep\":\n"
         "    - Puts the given (or all, if no iface is given) transceiver into\n"
         "      deep sleep mode\n"
         "- Using \"awake\":\n"
         "    - Wakes up the given (or all, if no iface is given) transceiver(s)"
         "      \n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
