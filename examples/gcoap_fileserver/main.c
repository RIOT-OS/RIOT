/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the GCoAP file server
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "net/gcoap.h"
#include "net/gcoap/fileserver.h"
#include "shell.h"
#include "vfs_default.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const gcoap_fileserver_entry_t _vfs_entry = {
    .root = VFS_DEFAULT_DATA,
    .resource = "/vfs",
};

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/vfs", COAP_GET | COAP_MATCH_SUBTREE, gcoap_fileserver_handler, (void *)&_vfs_entry },
};

static gcoap_listener_t _listener = {
    .resources = _resources,
    .resources_len = ARRAY_SIZE(_resources),
};

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_register_listener(&_listener);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
