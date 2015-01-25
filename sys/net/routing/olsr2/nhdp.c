/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#include <stdlib.h>

#include "nhdp.h"
#include "util.h"
#include "debug.h"
#include "node.h"
#include "routing_table.h"
#include "constants.h"

#include "common/avl.h"

#ifdef ENABLE_DEBUG
static struct netaddr_str nbuf[1];
#endif

static struct olsr_node *_node_replace(struct olsr_node *old_n)
{
    struct olsr_node *new_n = calloc(1, sizeof(struct nhdp_node));

    if (new_n == NULL) {
        return old_n;
    }

    /* remove things that held a pointer to this */
    avl_remove(get_olsr_head(), &old_n->node);
    bool _free_node = remove_free_node(old_n);

    memcpy(new_n, old_n, sizeof(struct olsr_node));
    memset(&new_n->node, 0, sizeof(new_n->node));

    new_n->type = NODE_TYPE_NHDP;
    new_n->node.key = new_n->addr;
    avl_insert(get_olsr_head(), &new_n->node);

    free(old_n);

    if (_free_node) {
        add_free_node(new_n);
    }

    new_n->pending = 1;
    h1_deriv(new_n)->link_quality = OLSR2_HYST_SCALING;

    return new_n;
}

struct olsr_node *olsr2_add_neighbor(struct netaddr *addr, metric_t metric, uint8_t vtime, char *name)
{
    struct olsr_node *n = get_node(addr);

    if (n == NULL) {
        DEBUG("\tadding new neighbor: %s", netaddr_to_str_s(&nbuf[0], addr));
        n = calloc(1, sizeof(struct nhdp_node));

        if (n == NULL) {
            return NULL;
        }

        n->addr = netaddr_dup(addr);

        if (n->addr == NULL) {
            free(n);
            return NULL;
        }

        n->type = NODE_TYPE_NHDP;
        n->distance = 1;
        n->link_metric = metric;
        h1_deriv(n)->link_quality = OLSR2_HYST_SCALING;
        n->pending = 1;

#ifdef ENABLE_NAME

        if (name != NULL) {
            n->name = strdup(name);
        }

#endif

        n->node.key = n->addr;
        avl_insert(get_olsr_head(), &n->node);
    }
    else if (n->type != NODE_TYPE_NHDP) {
        DEBUG("\tconverting olsr node %s to nhdp node",
              netaddr_to_str_s(&nbuf[0], n->addr));
        n = _node_replace(n);
    }

    /* add_other_route would otherwise not update expires */
    if (n->next_addr == NULL) {
        n->expires = time_now() + vtime;
    }

    add_other_route(n, get_local_addr(), 1, metric, vtime);

    return n;
}
