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
#include <stdio.h>

#include "olsr.h"
#include <slist.h>
#include "olsr_debug.h"
#include "util.h"
#include "routing.h"
#include "constants.h"
#include "rfc5444/rfc5444.h"

/* sorted list, only for faster access
 * Keeps yet unroutable nodes, so we don't have to traverse the entire list
 */
struct free_node {
    struct free_node *next;
    struct olsr_node *node;
    uint8_t hops; // for sorting only
};

static struct free_node *_pending_head = 0;
static bool _update_pending = false;

void add_free_node(struct olsr_node *node)
{
    struct free_node *n = simple_list_find_cmp(_pending_head, node, (int ( *)(void *, void *)) olsr_node_cmp);

    if (n == NULL) {
        uint8_t hops = node->distance;
        n = simple_list_add_before(&_pending_head, hops);
    }

    if (n == NULL) {
        printf("ERROR: out of memory in %s\n", __FUNCTION__);
        return;
    }

    n->node = node;

    node->next_addr = netaddr_free(node->next_addr);	/* empty next_addr marks route as pending */
    _update_pending = true;
}

bool remove_free_node(struct olsr_node *node)
{
    struct free_node *n = simple_list_find_cmp(_pending_head, node, (int ( *)(void *, void *)) olsr_node_cmp);

    if (n == NULL) {
        return false;
    }

    return simple_list_remove(&_pending_head, n);
}

void fill_routing_table(void)
{
    struct free_node *head = _pending_head;

    if (_pending_head == NULL || !_update_pending) {
        return;
    }

    _update_pending = false;
    DEBUG("update routing table");

    struct free_node *fn;
    bool noop = false;	/* when in an iteration there was nothing removed from free nodes */

    while (head && !noop) {
        noop = true;	/* if no nodes could be removed in an iteration, abort */
        struct free_node *prev;
        char skipped;
        simple_list_for_each_safe(head, fn, prev, skipped) {
            DEBUG("trying to find a route to %s", fn->node->name);
            /* chose shortest route from the set of availiable routes */
            metric_t min_mtrc = RFC5444_METRIC_INFINITE;
            struct olsr_node *node = NULL; /* chosen route */
            struct nhdp_node *flood_mpr = NULL;
            struct alt_route *route; /* current other_route */
            simple_list_for_each(fn->node->other_routes, route) {
                DEBUG("\tconsidering %s (%d)", netaddr_to_string(&nbuf[0], route->last_addr), route->link_metric);

                /* the node is actually a neighbor of ours */
                if (netaddr_cmp(route->last_addr, get_local_addr()) == 0) {
                    DEBUG("\t\t1-hop neighbor");

                    /* don't use pending nodes */
                    if (fn->node->pending) {
                        DEBUG("\t\tpending -> skipped");
                        continue;
                    }

                    if (route->link_metric > min_mtrc) {
                        continue;
                    }

                    min_mtrc = route->link_metric;
                    node = fn->node;
                    continue;
                }

                /* see if we can find a better route */
                struct olsr_node *_tmp = get_node(route->last_addr);

                if (_tmp == NULL || _tmp->addr == NULL || _tmp->next_addr == NULL) {
                    DEBUG("\t\tnot routable");
                    continue;
                }

                /* ignore pending nodes */
                if (_tmp->distance == 1 && _tmp->pending) {
                    DEBUG("\t\tpending -> skipped");
                    continue;
                }

                /* flooding MPR selection */
                if (_tmp->type == NODE_TYPE_NHDP &&
                    (flood_mpr == NULL || flood_mpr->flood_neighbors < h1_deriv(_tmp)->flood_neighbors)) {
                    flood_mpr = h1_deriv(_tmp);
                }

                if (_tmp->path_metric + route->link_metric > min_mtrc) {
                    DEBUG("\t\tdoesn't offer a better route, %d + %d > %d", _tmp->path_metric, route->link_metric, min_mtrc);
                    continue;
                }

                /* try to minimize MPR count */
                if (_tmp->type == NODE_TYPE_NHDP && min_mtrc == _tmp->path_metric + route->link_metric) {
                    DEBUG("\t\tequaly good route found, try to optimize MPR seleciton");
                    struct nhdp_node *old_mpr = h1_deriv(node);

                    /* a direct neighbor might be reached over an additional hop, the true MPR */
                    if (netaddr_cmp(_tmp->next_addr, _tmp->addr) != 0) {
                        old_mpr = h1_deriv(get_node(_tmp->next_addr));
                    }

                    /* use the neighbor with the most 2-hop neighbors */
                    if (old_mpr->mpr_neigh_route >= h1_deriv(_tmp)->mpr_neigh_route + 1) {
                        DEBUG("\t\told MPR (%d) is better (new: %d)", old_mpr->mpr_neigh_route, h1_deriv(_tmp)->mpr_neigh_route);
                        continue;
                    }
                }

                DEBUG("\t\t[possible candidate]");
                node = _tmp;
                min_mtrc = _tmp->path_metric + route->link_metric;
            } /* for each other_route */

            if (flood_mpr != NULL) {
                netaddr_switch(&fn->node->flood_mpr, h1_super(flood_mpr)->addr);
                DEBUG("[%s] setting flood MPR to %s", __FUNCTION__, netaddr_to_str_s(&nbuf[0], fn->node->flood_mpr));
                flood_mpr->mpr_neigh_flood++;
            }
            else {
                fn->node->flood_mpr = netaddr_free(fn->node->flood_mpr);
            }

            /* We found a valid route */
            if (node == fn->node) {
                DEBUG("\t%s (%s) is a 1-hop neighbor",
                      netaddr_to_str_s(&nbuf[0], fn->node->addr), fn->node->name);
                noop = false;
                fn->node->next_addr = netaddr_use(fn->node->addr);
                fn->node->path_metric = fn->node->link_metric;
                fn->node->distance = 1;
                fn->node->lost = 0;

                pop_other_route(fn->node, get_local_addr());
                simple_list_for_each_remove(&head, fn, prev);

            }
            else if (node != NULL) {
                DEBUG("\t%s (%s) -> %s (%s) -> […] -> %s",
                      netaddr_to_str_s(&nbuf[0], fn->node->addr), fn->node->name,
                      netaddr_to_str_s(&nbuf[1], node->addr), node->name,
                      netaddr_to_str_s(&nbuf[2], node->next_addr));
                DEBUG("%d = %d", fn->node->distance, node->distance + 1);

                noop = false;

                /* update routing MPR information */
                if (node->type == NODE_TYPE_NHDP || fn->node->flood_mpr != NULL) {
                    struct nhdp_node *mpr = h1_deriv(get_node(node->next_addr));
                    DEBUG("\tincrementing mpr_neigh_route for %s", h1_super(mpr)->name);
                    mpr->mpr_neigh_route++;
                }

                fn->node->distance = node->distance + 1;
                fn->node->path_metric = min_mtrc;
                fn->node->next_addr = netaddr_use(node->next_addr);

                pop_other_route(fn->node, node->addr);
                simple_list_for_each_remove(&head, fn, prev);
            }
            else {
                DEBUG("\tdon't yet know how to route %s", netaddr_to_str_s(&nbuf[0], fn->node->addr));
            }
        }
    }

    _pending_head = head;

#ifdef DEBUG

    while (head != NULL) {
        DEBUG("Could not find next hop for %s (%s), should be %s (%d hops)",
              netaddr_to_str_s(&nbuf[0], head->node->addr), head->node->name,
              netaddr_to_str_s(&nbuf[1], head->node->last_addr), head->node->distance);

        head = head->next;
    }

#endif
}
