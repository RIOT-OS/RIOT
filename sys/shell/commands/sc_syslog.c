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

#include "kernel_defines.h"
#include "syslog.h"

static void _syslog_usage(void)
{
    puts("syslog list");
    puts("syslog hostname <name>");
    puts("syslog setmask <mask>");
    puts("syslog logd <level> <string>");
    if (IS_USED(MODULE_SYSLOG_BACKEND_FILE)) {
        puts("syslog file suspend");
        puts("syslog file resume");
        if (IS_ACTIVE(CONFIG_SYSLOG_BACKEND_FILE_HAS_SUSPEND)) {
            puts("syslog file stop");
            puts("syslog file start");
        }
    }
    if (IS_USED(MODULE_SYSLOG_BACKEND_STDIO)) {
        puts("syslog stdio enable");
        puts("syslog stdio disable");
    }
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

#if MODULE_SYSLOG_BACKEND_FILE
static int _file_handler(int argc, char **argv)
{
    if (argc < 2) {
        _syslog_usage();
        return 1;
    }
    if (strcmp(argv[1], "suspend") == 0) {
        syslog_backend_file_suspend_rotation();
    }
    else if (strcmp(argv[1], "resume") == 0) {
        syslog_backend_file_resume_rotation();
    }
    else if (IS_ACTIVE(CONFIG_SYSLOG_BACKEND_FILE_HAS_SUSPEND) &&
             strcmp(argv[1], "start") == 0) {
        syslog_backend_file_start();
    }
    else if (IS_ACTIVE(CONFIG_SYSLOG_BACKEND_FILE_HAS_SUSPEND) &&
             strcmp(argv[1], "stop") == 0) {
        syslog_backend_file_stop();
    }
    else {
        printf("syslog file: unsupported sub-command: %s\n", argv[1]);
        _syslog_usage();
        return 1;
    }

    return 0;
}
#endif

#if MODULE_SYSLOG_BACKEND_STDIO
static int _stdio_handler(int argc, char **argv)
{
    if (argc < 2) {
        _syslog_usage();
        return 1;
    }

    if (strcmp(argv[1], "enable") == 0) {
        syslog_backend_stdio_print(1);
    }
    else if (strcmp(argv[1], "disable") == 0) {
        syslog_backend_stdio_print(0);
    }
    else {
        printf("syslog stdio: unsupported sub-command: %s\n", argv[1]);
        _syslog_usage();
        return 1;
    }

    return 0;
}
#endif

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
#if MODULE_SYSLOG_BACKEND_FILE
    else if (strcmp(argv[1], "file") == 0) {
        return _file_handler(argc - 1, argv + 1);
    }
#endif
#if MODULE_SYSLOG_BACKEND_STDIO
    else if (strcmp(argv[1], "stdio") == 0) {
        return _stdio_handler(argc - 1, argv + 1);
    }
#endif
    else {
        printf("syslog: unsupported sub-command: %s\n", argv[1]);
        _syslog_usage();
        return 1;
    }
}
