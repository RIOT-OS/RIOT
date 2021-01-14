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
#include <stdlib.h>

#include "syslog.h"

static void _syslog_usage(void)
{
    puts("syslog list");
    puts("syslog hostname <name>");
    puts("syslog setmask <mask>");
    puts("syslog logd <level> <string>");
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

static int _hostname_handler(int argc, char **argv)
{
    static char hostname[32] = "";

    if (argc < 2) {
        _syslog_usage();
        return 1;
    }

    strncpy(hostname, argv[1], sizeof(hostname) - 1);

    syslog_set_hostname(hostname);

    return 0;
}

static int _logd_handler(int argc, char **argv)
{
    if (argc != 3) {
        _syslog_usage();
        return 1;
    }
    syslog(syslog_default_entry, atoi(argv[1]), "%s", argv[2]);

    return 0;
}

static int _setmask_handler(int argc, char **argv)
{
    if (argc != 2) {
        _syslog_usage();
        return 1;
    }
    setlogmask(syslog_default_entry, atoi(argv[1]));

    return 0;
}

int _syslog_handler(int argc, char **argv)
{
    if (argc < 2) {
        _syslog_usage();
        return 1;
    }
    if (strcmp(argv[1], "list") == 0) {
        return _list_handler(argc - 1, argv + 1);
    }
    else if (strcmp(argv[1], "hostname") == 0) {
        return _hostname_handler(argc - 1, argv + 1);
    }
    else if (strcmp(argv[1], "logd") == 0) {
        return _logd_handler(argc - 1, argv + 1);
    }
    else if (strcmp(argv[1], "setmask") == 0) {
        return _setmask_handler(argc - 1, argv + 1);
    }
    else {
        printf("syslog: unsupported sub-command: %s\n", argv[1]);
        _syslog_usage();
        return 1;
    }
}
