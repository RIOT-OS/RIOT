/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdlib.h>
#include <stdio.h>

#include "olsr.h"
#include "list.h"
#include "debug.h"
#include "util.h"
#include "routing.h"
#include "constants.h"

/* sorted list, only for faster access
 * Keeps yet unroutable nodes, so we don't have to traverse the entire list
 */
struct free_node {
	struct free_node* next;
	struct olsr_node* node;
	uint8_t hops; // for sorting only
};

static struct free_node* _pending_head = 0;
static bool _update_pending = false;

void add_free_node(struct olsr_node* node) {
	struct free_node* n = simple_list_find_cmp(_pending_head, node, (int (*)(void *, void *)) olsr_node_cmp);
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

bool remove_free_node(struct olsr_node* node) {
	struct free_node* n = simple_list_find_cmp(_pending_head, node, (int (*)(void *, void *)) olsr_node_cmp);
	if (n == NULL)
		return false;
	return simple_list_remove(&_pending_head, n);
}

void fill_routing_table(void) {
	struct free_node* head = _pending_head;

	if (_pending_head == NULL || !_update_pending)
		return;

	_update_pending = false;
	DEBUG("update routing table");

	struct free_node* fn;
	bool noop = false;	/* when in an iteration there was nothing removed from free nodes */
	while (head && !noop) {
		noop = true;	/* if no nodes could be removed in an iteration, abort */
		struct free_node *prev;
		char skipped;
		simple_list_for_each_safe(head, fn, prev, skipped) {
			/* chose shortest route from the set of availiable routes */
			uint8_t min_hops = 255;
			struct olsr_node* node = NULL;
			struct alt_route* route;
			simple_list_for_each(fn->node->other_routes, route) {

				/* the node is actually a neighbor of ours */
				if (netaddr_cmp(route->last_addr, get_local_addr()) == 0) {
					/* don't use pending nodes */
					if (fn->node->pending)
						continue;

					min_hops = 1;
					break;
				}

				/* see if we can find a better route */
				struct olsr_node* _tmp = get_node(route->last_addr);
				if (_tmp != NULL && _tmp->addr != NULL &&
					_tmp->distance + 1 <= min_hops &&
					_tmp->next_addr != NULL) {

					if (_tmp->next_addr == NULL)
						continue;
					/* ignore pending nodes */
					if (_tmp->distance == 1 && _tmp->pending)
						continue;

					/* try to minimize MPR count */
					if (min_hops == 2) {
						/* use the neighbor with the most 2-hop neighbors */
						if (h1_deriv(node)->mpr_neigh > h1_deriv(_tmp)->mpr_neigh + 1)
							continue;
					}

					node = _tmp;
					min_hops = _tmp->distance + 1;
				}
			}

			/* We found a valid route */
			if (min_hops == 1) {
				DEBUG("%s (%s) is a 1-hop neighbor",
					netaddr_to_str_s(&nbuf[0], fn->node->addr), fn->node->name);
				noop = false;
				fn->node->next_addr = netaddr_use(fn->node->addr);
				fn->node->distance = 1;
				fn->node->lost = 0;

				pop_other_route(fn->node, get_local_addr());
				simple_list_for_each_remove(&head, fn, prev);

			} else if (node != NULL) {
				DEBUG("%s (%s) -> %s (%s) -> […] -> %s",
					netaddr_to_str_s(&nbuf[0], fn->node->addr), fn->node->name,
					netaddr_to_str_s(&nbuf[1], node->addr), node->name,
					netaddr_to_str_s(&nbuf[2], node->next_addr));
				DEBUG("%d = %d", fn->node->distance, node->distance + 1);

				noop = false;

				/* update MPR information */
				if (node->distance == 1) {
					h1_deriv(node)->mpr_neigh++;
				}

				fn->node->distance = node->distance + 1;
				fn->node->next_addr = netaddr_use(node->next_addr);

				pop_other_route(fn->node, node->addr);
				simple_list_for_each_remove(&head, fn, prev);
			} else
				DEBUG("don't yet know how to route %s", netaddr_to_str_s(&nbuf[0], fn->node->addr));
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
