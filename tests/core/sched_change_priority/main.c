/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for sched_change_priority / nice
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "architecture.h"
#include "sched.h"
#include "shell.h"
#include "thread.h"

static int sc_hint(int argc, char **argv);

static WORD_ALIGNED char t2_stack[THREAD_STACKSIZE_TINY];
static WORD_ALIGNED char t3_stack[THREAD_STACKSIZE_DEFAULT];
static kernel_pid_t t3_pid;

static const shell_command_t cmds[] = {
    { "hint", "Display the correct invocation of nice for this teset", sc_hint },
    { NULL, NULL, NULL }
};

/*
 * Note: An extra shell command just for displaying this hint is very much of
 *       an overkill for a human being, especially since the ps command already
 *       provides all the details. However, for automatic testing this makes it
 *       easy to extract the pid of t3 and the numeric value of
 *       THREAD_PRIORITY_MAIN - 1, resulting in more robust automatic testing.
 *       A shell command also has the convenient side effect of synchronizing
 *       with the shell.
 */
static int sc_hint(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Run \"nice %" PRIkernel_pid " %u\"\n",
           t3_pid, (unsigned)THREAD_PRIORITY_MAIN - 1);

    return EXIT_SUCCESS;
}

static void *t2_func(void *unused)
{
    (void)unused;

    while (1) {
        /* blocking t3 from running with busy loop while t3 has lower prio than me */
    }

    return NULL;
}

static void *t3_func(void *unused)
{
    (void)unused;

    while (1) {
        uint8_t prio = THREAD_PRIORITY_MAIN + 2;
        printf("[t3] Setting my priority to THREAD_PRIORITY_MAIN + 2 = %u\n",
               (unsigned)prio);
        sched_change_priority(thread_get_active(), prio);
        puts("[t3] Running again.");
    }

    return NULL;
}

int main(void)
{
    thread_create(t2_stack, sizeof(t2_stack), THREAD_PRIORITY_MAIN + 1,
                  0, t2_func, NULL, "t2");

    t3_pid = thread_create(t3_stack, sizeof(t3_stack), THREAD_PRIORITY_MAIN - 1,
                           0, t3_func, NULL, "t3");

    puts("[main] Use shell command \"nice\" to increase prio of t3.\n"
         "[main] If it works, it will run again. The \"hint\" cmd can be useful.");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmds, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
