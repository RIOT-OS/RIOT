/*
 * SPDX-FileCopyrightText: 2022 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdio.h>
#include "kernel_defines.h"
#include "net/gcoap.h"
#include "net/nanocoap/fileserver.h"
#include "shell.h"
#include "vfs_default.h"

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/vfs",
      COAP_GET |
#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
      COAP_PUT |
#endif
#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
      COAP_DELETE |
#endif
      COAP_MATCH_SUBTREE,
      nanocoap_fileserver_handler, VFS_DEFAULT_DATA },
};

static gcoap_listener_t _listener = {
    .resources = _resources,
    .resources_len = ARRAY_SIZE(_resources),
};

static void _event_cb(nanocoap_fileserver_event_t event, nanocoap_fileserver_event_ctx_t *ctx)
{
    switch (event) {
    case NANOCOAP_FILESERVER_GET_FILE_START:
        printf("gcoap fileserver: Download started: %s\n", ctx->path);
        break;
    case NANOCOAP_FILESERVER_GET_FILE_END:
        printf("gcoap fileserver: Download finished: %s\n", ctx->path);
        break;
    case NANOCOAP_FILESERVER_PUT_FILE_START:
        printf("gcoap fileserver: Upload started: %s\n", ctx->path);
        break;
    case NANOCOAP_FILESERVER_PUT_FILE_END:
        printf("gcoap fileserver: Upload finished: %s\n", ctx->path);
        break;
    case NANOCOAP_FILESERVER_DELETE_FILE:
        printf("gcoap fileserver: Delete %s\n", ctx->path);
        break;
    }
}

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_register_listener(&_listener);

    if (IS_USED(MODULE_NANOCOAP_FILESERVER_CALLBACK)) {
        nanocoap_fileserver_set_event_cb(_event_cb, NULL);
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
