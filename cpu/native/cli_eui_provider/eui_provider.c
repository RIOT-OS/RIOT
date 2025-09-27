/*
 * SPDX-FileCopyrightText: 2020 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup cpu_native
 * @brief   Native CPU EUI provider
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "net/l2util.h"
#include "native_cli_eui_provider.h"
#include "list.h"

#include "native_internal.h"

/* list of user supplied EUI-64s */
typedef struct {
    list_node_t node;
    eui64_t addr;
} native_eui64_list_t;

static list_node_t head;

/* parse EUI-64 from command line */
void native_cli_add_eui64(const char *s)
{
    _native_syscall_enter();
    native_eui64_list_t *e = real_malloc(sizeof(native_eui64_list_t));
    _native_syscall_leave();

    size_t res = l2util_addr_from_str(s, e->addr.uint8);
    assert(res <= sizeof(eui64_t));

    /* if the provided address exceeds eui64_t, l2util_addr_from_str()
     * *will* corrupt memory. */
    if (res > sizeof(eui64_t)) {
        exit(-1);
    }

    list_add(&head, &e->node);
}

/* callback for EUI provider */
int native_cli_get_eui64(uint8_t index, eui64_t *addr)
{
    uint8_t cnt = 0;
    for (list_node_t *e = head.next; e != NULL; e = e->next) {
        if (cnt++ == index) {
            *addr = container_of(e, native_eui64_list_t, node)->addr;
            return 0;
        }
    }

    return -1;
}
