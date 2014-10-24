/**
 * Shell commands for the clock-sync evaluation module
 *
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_clocksync_eval.c
 * @brief   Provides interactive access to the clock-sync evaluation module.
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef MODULE_NET_IF
#include "net_if.h"
#endif
#include "transceiver.h"
#include "msg.h"

#include "clocksync/clocksync_eval.h"

static void _print_help(void);

void _clocksync_eval_handler(int argc, char **argv)
{
    if (argc == 5) {
        if (!strcmp(argv[1], "beacon")) {
            if (!strcmp(argv[2], "interval")) {
                int32_t upper = atoi(argv[3]);
                int32_t lower = atoi(argv[4]);
                clocksync_eval_set_beacon_interval((uint32_t) upper,
                                                   (uint32_t) lower);
                return;
            }
        }
    }

    if (argc == 4) {
        if (!strcmp(argv[1], "heartbeat")) {
            if (!strcmp(argv[2], "interval")) {
                int32_t interval = atoi(argv[3]);
                clocksync_eval_set_heartbeat_interval((uint32_t) interval);
                return;
            }
        }
    }

    if (argc == 3) {
        if (!strcmp(argv[1], "heartbeat")) {
            {
                if (!strcmp(argv[2], "on")) {
                    clocksync_eval_resume_heartbeat();
                    return;
                }
                else if (!strcmp(argv[2], "off")) {
                    clocksync_eval_pause_heartbeat();
                    return;
                }
            }
        }
        else if (!strcmp(argv[1], "beacon")) {
            if (!strcmp(argv[2], "on")) {
                clocksync_eval_resume_sending();
                return;
            }
            else if (!strcmp(argv[2], "off")) {
                clocksync_eval_pause_sending();
                return;
            }
        }
    }

    _print_help();
}

static void _print_help(void)
{
    printf(
        "Usage: clocksynce  [heartbeat [on|off]|[interval <ms>]] | [beacon [on|off]|[interval <upper_in_ms> <lower_in_ms>]]  \n");
}
