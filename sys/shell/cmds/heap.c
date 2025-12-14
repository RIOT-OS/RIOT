/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
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
 * @brief       Print the state of the heap
 *
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @}
 */

#include "cpu_conf.h"
#include "shell.h"

#if defined(MODULE_NEWLIB_SYSCALLS_DEFAULT) || defined (HAVE_HEAP_STATS)
extern void heap_stats(void);
#else
#include <stdio.h>
#endif

static int _heap_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

#if defined(MODULE_NEWLIB_SYSCALLS_DEFAULT) || defined (HAVE_HEAP_STATS)
    heap_stats();
    return 0;
#else
    printf("heap statistics are not supported for %s cpu\n", buildinfo_cpu_name);
    return 1;
#endif
}

SHELL_COMMAND(heap, "Prints heap statistics.", _heap_handler);
