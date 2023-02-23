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

#include <stdio.h>
#include "kernel_defines.h"
#include "net/gcoap.h"
#include "net/gcoap/fileserver.h"
#include "shell.h"
#include "vfs_default.h"

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/vfs",
      COAP_GET |
#if IS_USED(MODULE_GCOAP_FILESERVER_PUT)
      COAP_PUT |
#endif
#if IS_USED(MODULE_GCOAP_FILESERVER_DELETE)
      COAP_DELETE |
#endif
      COAP_MATCH_SUBTREE,
      gcoap_fileserver_handler, VFS_DEFAULT_DATA },
};

static gcoap_listener_t _listener = {
    .resources = _resources,
    .resources_len = ARRAY_SIZE(_resources),
};

static void _event_cb(gcoap_fileserver_event_t event, gcoap_fileserver_event_ctx_t *ctx)
{
    switch (event) {
    case GCOAP_FILESERVER_GET_FILE_START:
        printf("gcoap fileserver: Download started: %s\n", ctx->path);
        break;
    case GCOAP_FILESERVER_GET_FILE_END:
        printf("gcoap fileserver: Download finished: %s\n", ctx->path);
        break;
    case GCOAP_FILESERVER_PUT_FILE_START:
        printf("gcoap fileserver: Upload started: %s\n", ctx->path);
        break;
    case GCOAP_FILESERVER_PUT_FILE_END:
        printf("gcoap fileserver: Upload finished: %s\n", ctx->path);
        break;
    case GCOAP_FILESERVER_DELETE_FILE:
        printf("gcoap fileserver: Delete %s\n", ctx->path);
        break;
    }
}

int main(void)
{
    gcoap_register_listener(&_listener);

    if (IS_USED(MODULE_GCOAP_FILESERVER_CALLBACK)) {
        gcoap_fileserver_set_event_cb(_event_cb, NULL);
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
