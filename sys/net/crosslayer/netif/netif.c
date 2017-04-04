/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "utlist.h"

#include "net/netif.h"

#define _NETIF_MIN_ID   (1)

netif_t *netifs = NULL;

int netif_add(netif_t *netif)
{
    assert((netif != NULL) && (netif->next != NULL));
    if (netifs != NULL) {
        netif_t *ptr;
        for (ptr = netifs; ptr->next != NULL; ptr = ptr->next) {
#if NETIF_NAMED
            if (strncmp(ptr->name, netif->name, NETIF_NAME_LEN) == 0) {
                return -EEXIST;
            }
#endif
        }
#if NETIF_NAMED
        /* last element was skipped in loop, so check it here */
        if (strncmp(ptr->name, netif->name, NETIF_NAME_LEN) == 0) {
            return -EEXIST;
        }
#endif
        /* already inserted, but is last (not recognized by assert above, but
         * not critical so we don't have to abort) */
        if (ptr == netif) {
            return 0;
        }
        ptr->next = netif;
    }
    else {
        netifs = netif;
    }
    return 0;
}

int netif_get_id(netif_t *netif)
{
    int id;
    netif_t *ptr;

    for (id = _NETIF_MIN_ID, ptr = netifs; ptr != NULL; id++, ptr = ptr->next) {
        if (ptr == netif) {
            return id;
        }
    }
    return -ENOENT;
}

netif_t *netif_by_id(int id)
{
    netif_t *ptr = NULL;

    if (id > 0) {
        /* We can use id as temporary variable to find the interface if we count
         * down, so */
        /* cppcheck-suppress uselessAssignmentArg */
        for (ptr = netifs; (ptr != NULL) && (id > _NETIF_MIN_ID); id--,
             ptr = ptr->next) {}
    }
    return ptr;
}

#if NETIF_NAMED || defined(DOXYGEN)
netif_t *netif_by_name(char *name)
{
    netif_t *ptr;

    assert(name != NULL);
    for (ptr = netifs; ptr != NULL; ptr = ptr->next) {
        if (strncmp(ptr->name, name, NETIF_NAME_LEN) == 0) {
            break;
        }
    }
    return ptr;
}

int netif_set_name(netif_t *netif, char *name)
{
    netif_t *ptr;
    int res = -ENOENT;

    assert((netif != NULL) && (name != NULL));
    for (ptr = netifs; ptr != NULL; ptr = ptr->next) {
        if (ptr == netif) {
            res = 0;
        }
        else if (strncmp(ptr->name, name, NETIF_NAME_LEN) == 0) {
            return -EEXIST;
        }
    }
    if (res == 0) {
        strncpy(netif->name, name, NETIF_NAME_LEN);
    }
    return res;
}

#endif

/** @} */
