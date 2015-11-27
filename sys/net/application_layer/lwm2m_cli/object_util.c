/*
 * Copyright (C) 2015 Alexandru Razvan Caciulescu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>

#include "net/lwm2m_cli/object_util.h"
#include "net/lwm2m_cli/objects.h"


lwm2m_object_t *lwm2m_cli_obj_find(lwm2m_context_t *ctx, uint16_t id)
{
    for (int i = 0; i < ctx->numObject; i++) {
        if (ctx->objectList[i]->objID == id) {
            return ctx->objectList[i];
        }
    }
    return NULL;
}

void lwm2m_cli_obj_init(lwm2m_object_t **obj_store, size_t len)
{
    (void)len;
    int server_id = 1;  /* MUST not be 0 */

    /* now lets initialize a minimal object store */
    obj_store[0] = get_security_object(server_id, false);
    if (obj_store[0] == NULL) {
        puts("failed to create empty security object");
        return;
    }

    obj_store[1] = get_server_object(server_id, "U", 36000, false);
    if (obj_store[1] == NULL) {
        puts("Failed to create server object");
        return;
    }

    obj_store[2] = get_object_device();
    if (obj_store[2] == NULL) {
        puts("Failed to create Device object");
        return;
    }
}
