/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * Util 'cpu' dummy functions
 *
 * @ingroup cpu_util
 * @{
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <fcntl.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* cmdline args are 0 separated */
static unsigned _get_argc(const char *args, const char *end)
{
    unsigned count = 0;
    while (args != end) {
        if (*args++ == 0) {
            ++count;
        }
    }
    return count;
}

void pm_reboot(void)
{
    /* get command line from procfs */
    char cmdline[ARG_MAX];
    int fd = open("/proc/self/cmdline", O_RDONLY);
    int n_bytes = read(fd, cmdline, sizeof(cmdline));
    close(fd);

    char *end = cmdline + n_bytes;
    char *argv[_get_argc(cmdline, cmdline + n_bytes)];

    /* fill the argv array */
    unsigned i = 0;
    for (char *arg = cmdline; arg < end; ) {
        argv[i++] = arg;
        arg += strlen(arg) + 1;
    }
    argv[i] = 0;

    execve("/proc/self/exe", argv, NULL);
}

void pm_off(void)
{
    exit(0);
}

void irq_restore(unsigned state)
{
    (void) state;
}

unsigned irq_disable(void)
{
    return 0;
}

/** @} */
