/*
 * Copyright (c) 2024 ML!PA Consulting GmbH
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
 * @brief       nanoCoAP fs test app
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"

#include "net/nanocoap/fs.h"
#include "string_utils.h"
#include "shell.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static int _cmd_mount(int argc, char **argv)
{
    static char url[64];
    static char mp[64];
    int res;

    if (argc != 3) {
        printf("usage: %s <url> <path>\n", argv[0]);
        return 1;
    }

    if ((res = strscpy(mp, argv[2], sizeof(mp))) < 0) {
        return res;
    }
    if ((res = strscpy(url, argv[1], sizeof(url))) < 0) {
        return res;
    }

    if (url[res] != '/') {
        url[res] = '/';
        if (res == sizeof(url)) {
            return -ENOBUFS;
        }
        url[++res] = 0;
    }

    static nanocoap_fs_t nanocoap_fs_desc = {
        .url = url,
    };

    static vfs_mount_t _mount = {
        .fs = &nanocoap_fs_file_system,
        .mount_point = mp,
        .private_data = &nanocoap_fs_desc,
    };

    res = vfs_mount(&_mount);

    return res;
}

static const shell_command_t shell_commands[] = {
    { "mount", "Mount a remote fs", _cmd_mount },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
