/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       A shell command to change the niceness (inverse priority) of a thread
 *
 * @note        Enable this by using the modules shell_commands and nice
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "sched.h"
#include "thread.h"

int _sc_nice(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <THREAD_ID> <PRIORITY>\n"
               "Note: Lower number means higher priority (niceness)\n",
               argv[0]);
        return EXIT_FAILURE;
    }

    /* Note: thread_get() does bounds checking and returns NULL on out of bounds PID */
    thread_t *thread = thread_get(atoi(argv[1]));
    if (!thread) {
        printf("No active thread found for ID \"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }

    uint8_t prio = atoi(argv[2]);
    if (prio >= SCHED_PRIO_LEVELS) {
        printf("Priority \"%s\" is invalid (try 0..%u)\n",
               argv[2], (unsigned)SCHED_PRIO_LEVELS - 1);
    }

    sched_change_priority(thread, prio);
    return EXIT_SUCCESS;
}
