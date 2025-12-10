/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Shell Scripting Test Application
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdlib.h>

#include "shell.h"
#include "vfs_default.h"
#include "vfs_util.h"
#include "ztimer.h"

#ifndef SCRIPT_FILE
#define SCRIPT_FILE VFS_DEFAULT_DATA "/script.sh"
#endif

static int cmd_msleep(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }

    ztimer_sleep(ZTIMER_MSEC, atoi(argv[1]));
    return 0;
}

static int cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    puts("");

    return 0;
}

static int cmd_add(int argc, char **argv)
{
    int sum = 0;

    for (int i = 1; i < argc; ++i) {
        sum += atoi(argv[i]);
    }

    printf("%d\n", sum);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "msleep", "sleep for a number of ms", cmd_msleep },
    { "echo", "echo parameters to console", cmd_echo },
    { "add", "add up a list of numbers", cmd_add },
    { NULL, NULL, NULL }
};

static int _create_script(void)
{
    const char file[] = {
        "# this is a comment\n"
        "msleep 500\n"
        "echo Hello RIOT!\n"
        "\n"
        "add 10 23 9\n"
        "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
    };

    return vfs_file_from_buffer(SCRIPT_FILE, file, sizeof(file) - 1);
}

int main(void)
{
    _create_script();

    unsigned line;
    int res = shell_parse_file(shell_commands, SCRIPT_FILE, &line);
    if (res) {
        fprintf(stderr, "%s:%u: error %d\n", SCRIPT_FILE, line, res);
    }
    return res;
}
