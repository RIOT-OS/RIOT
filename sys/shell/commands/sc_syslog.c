/*
 * Copyright (C) 2021 Continental
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
 * @brief       Shell commands for the syslog module
 *
 * @author      Vincent Dupont <vincent.dupont@continental-its.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "syslog.h"

static void _syslog_usage(char **argv)
{
    printf("%s list\n", argv[0]);
    printf("%s getmask\n", argv[0]);
}

static int _list_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    const syslog_entry_t *log = NULL;
    do {
        log = syslog_get_next_log(log);
        if (log) {
            const char *ident = syslog_getident(log);
            int mask = syslog_getmask(log);
            printf("%s: %d\n", ident ? ident : "unknown", mask);
        }
    } while (log);

    return 0;
}

int _syslog_handler(int argc, char **argv)
{
    if (argc < 2) {
        _syslog_usage(argv);
        return 1;
    }
    if (strcmp(argv[1], "list") == 0) {
        return _list_handler(argc - 1, argv + 1);
    }
    else {
        printf("%s: unsupported sub-command: %s\n", argv[0], argv[1]);
        _syslog_usage(argv);
        return 1;
    }
}