/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test application for the GCoAP file server
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "fs/constfs.h"
#include "net/gcoap.h"
#include "net/nanocoap/fileserver.h"
#include "shell.h"
#include "vfs_default.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    {
        .path = "/const",
        .methods = COAP_GET | COAP_MATCH_SUBTREE,
        .handler = nanocoap_fileserver_handler,
        .context = "/const"
    },
    {
        .path = "/vfs",
        .methods = COAP_GET | COAP_PUT | COAP_MATCH_SUBTREE,
        .handler = nanocoap_fileserver_handler,
        .context = VFS_DEFAULT_DATA
    },
};

static gcoap_listener_t _listener = {
    .resources = _resources,
    .resources_len = ARRAY_SIZE(_resources),
};

static const char song[] =
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "\n"
    "Hoarders can get piles of money,\n"
    "That is true, hackers, that is true.\n"
    "But they cannot help their neighbors;\n"
    "That's not good, hackers, that's not good.\n"
    "\n"
    "When we have enough free software\n"
    "At our call, hackers, at our call,\n"
    "We'll kick out those dirty licenses\n"
    "Ever more, hackers, ever more.\n"
    "\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n"
    "Join us now and share the software;\n"
    "You'll be free, hackers, you'll be free.\n";

/* this defines two const files in the constfs */
static constfs_file_t constfs_files[] = {
    {
        .path = "/song.txt",
        .size = sizeof(song),
        .data = song,
    },
};

/* this is the constfs specific descriptor */
static constfs_t constfs_desc = {
    .nfiles = ARRAY_SIZE(constfs_files),
    .files = constfs_files,
};

/* constfs mount point, as for previous example, it needs a file system driver,
 * a mount point and private_data as a pointer to the constfs descriptor */
static vfs_mount_t const_mount = {
    .fs = &constfs_file_system,
    .mount_point = "/const",
    .private_data = &constfs_desc,
};

int main(void)
{
    vfs_mount(&const_mount);

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_register_listener(&_listener);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
