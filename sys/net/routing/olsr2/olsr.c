/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef ENABLE_NAME
#include <string.h>
#endif

#include "common/netaddr.h"
#include "rfc5444/rfc5444.h"

#include "olsr.h"
#include "util.h"
#include "olsr_debug.h"
#include "routing.h"
#include "constants.h"
#include "list.h"

static struct olsr_node* _new_olsr_node(struct netaddr* addr,
	uint8_t distance, metric_t metric, uint8_t vtime, char* name) {

	struct olsr_node* n = calloc(1, sizeof(struct olsr_node));

	if (n == NULL)
		return NULL;

	n->addr = netaddr_dup(addr);

	if (n->addr == NULL) {
		free(n);
		return NULL;
	}

	n->node.key = n->addr;
	n->type = NODE_TYPE_OLSR;
	n->distance = distance;
	n->link_metric = metric;
	n->expires = time_now() + vtime;
#ifdef ENABLE_NAME
	if (name)
		n->name = strdup(name);
#endif

	avl_insert(get_olsr_head(), &n->node);
	return n;
}

static void _get_new_flood_mpr(struct netaddr* old_flood_mpr) {
	TRACE_FUN("%s", netaddr_to_str_s(&nbuf[0], old_flood_mpr));
	struct olsr_node *node;
	avl_for_each_element(get_olsr_head(), node, node) {
		if (node->distance != 2)
			continue;
		if (node->flood_mpr != NULL && netaddr_cmp(old_flood_mpr, node->flood_mpr) != 0)
			continue;

		DEBUG("chosing new flood MPR for %s (%s)", netaddr_to_str_s(&nbuf[0], node->addr), node->name);

		struct nhdp_node *mpr_b, *mpr_a = h1_deriv(get_node(node->last_addr));
		struct alt_route *route;
		simple_list_for_each(node->other_routes, route) {
			mpr_b = h1_deriv(get_node(route->last_addr));
			if (mpr_b == NULL || h1_super(mpr_b)->pending)
				continue;
			if (mpr_a == NULL || h1_super(mpr_a)->pending || mpr_a->flood_neighbors < mpr_b->flood_neighbors)
				mpr_a = mpr_b;
		}

		if (mpr_a != NULL) {
			mpr_a->mpr_neigh_flood++;

			netaddr_switch(&node->flood_mpr, h1_super(mpr_a)->addr);
			DEBUG("[%s] setting flood MPR to %s", __FUNCTION__, netaddr_to_str_s(&nbuf[0], node->flood_mpr));
		} else
			node->flood_mpr = netaddr_free(node->flood_mpr);

		DEBUG("\tnew flood MPR: %s", netaddr_to_str_s(&nbuf[0], node->flood_mpr));
	}
}

/*
 * find a new route for nodes that use last_addr as their default route
 * if lost_node_addr is not null, all reference to it will be removed (aka lost node)
 */
static void _update_children(struct netaddr* last_addr, struct netaddr* lost_node_addr) {
	TRACE_FUN("%s, %s", netaddr_to_str_s(&nbuf[0], last_addr),
		netaddr_to_str_s(&nbuf[1], lost_node_addr));

	struct olsr_node *node;
	avl_for_each_element(get_olsr_head(), node, node) {

		if (lost_node_addr != NULL) {
			remove_other_route(node, lost_node_addr);
			if (node->flood_mpr != NULL && netaddr_cmp(lost_node_addr, node->flood_mpr) == 0)
				node->flood_mpr = netaddr_free(node->flood_mpr);
		}

		if (node->last_addr != NULL && netaddr_cmp(node->last_addr, last_addr) == 0) {

			if (lost_node_addr != NULL)
				remove_default_node(node);
			else
				push_default_route(node);

			add_free_node(node);

			_update_children(node->addr, lost_node_addr);
		}
	}
}

static void _olsr_node_expired(struct olsr_node* node) {
	TRACE_FUN();

	remove_default_node(node);
	_update_children(node->addr, NULL);

	add_free_node(node);

	// 1-hop neighbors will become normal olsr_nodes here, should we care? (possible waste of memory)
}

static void _remove_olsr_node(struct olsr_node* node) {
	TRACE_FUN();

	avl_remove(get_olsr_head(), &node->node);
	remove_free_node(node);

	/* remove other routes from node that is about to be deleted */
	char skipped;
	struct alt_route *route, *prev;
	simple_list_for_each_safe(node->other_routes, route, prev, skipped) {
		netaddr_free(route->last_addr);
		simple_list_for_each_remove(&node->other_routes, route, prev);
	}

	netaddr_free(node->flood_mpr);

	remove_default_node(node);
	_update_children(node->addr, node->addr);

#ifdef ENABLE_NAME
	if (node->name)
		free(node->name);
#endif
	netaddr_free(node->addr);
	free(node);
}

static bool _route_expired(struct olsr_node* node, struct netaddr* last_addr) {
	if (node->last_addr != NULL && netaddr_cmp(node->last_addr, last_addr) == 0)
		return time_now() > node->expires;

	if (node->other_routes == NULL)
		return true;

	struct alt_route* route = simple_list_find_memcmp(node->other_routes, last_addr);

	if (route == NULL)
		return true;

	return time_now() > route->expires;
}

static void _update_link_quality(struct nhdp_node* node) {
	TRACE_FUN("%s", netaddr_to_str_s(&nbuf[0], h1_super(node)->addr));
	if (_route_expired(h1_super(node), get_local_addr()))
		node->link_quality = node->link_quality * (1 - OLSR2_HYST_SCALING);
	else
		node->link_quality = node->link_quality * (1 - OLSR2_HYST_SCALING) + OLSR2_HYST_SCALING;

	if (!h1_super(node)->pending && node->link_quality < OLSR2_HYST_LOW) {
		h1_super(node)->pending = 1;
		h1_super(node)->lost = LOST_ITER_MAX;

		if (node->mpr_neigh_flood > 0)
			_get_new_flood_mpr(h1_super(node)->addr);

		node->mpr_neigh_flood = 0;
		node->mpr_neigh_route = 0;

		add_free_node(h1_super(node));
		push_default_route(h1_super(node));
		_update_children(h1_super(node)->addr, NULL);
	}

	if (h1_super(node)->pending && node->link_quality > OLSR2_HYST_HIGH) {
		h1_super(node)->pending = 0;
		h1_super(node)->lost = 0;

		/* node may just have become a 1-hop node */
		push_default_route(h1_super(node));
		add_free_node(h1_super(node));
	}
}

bool remove_expired(struct olsr_node* node) {
	time_t _now = time_now();

	if (node->type == NODE_TYPE_NHDP)
		_update_link_quality(h1_deriv(node));

	char skipped;
	struct alt_route *route, *prev;
	simple_list_for_each_safe(node->other_routes, route, prev, skipped) {
		if (_now - route->expires < OLSR2_HOLD_TIME_SECONDS)
			continue;

		DEBUG("alternative route to %s (%s) via %s expired, removing it",
			node->name, netaddr_to_str_s(&nbuf[0], node->addr),
			netaddr_to_str_s(&nbuf[1], route->last_addr));
		simple_list_for_each_remove(&node->other_routes, route, prev);
	}

	if (_now - node->expires > OLSR2_HOLD_TIME_SECONDS) {

		DEBUG("%s (%s) expired",
			node->name, netaddr_to_str_s(&nbuf[0], node->addr));

		if (node->other_routes == NULL) {
			_remove_olsr_node(node);
			return true;
		} else
			_olsr_node_expired(node);
	}

	return false;
}

void route_expired(struct olsr_node* node, struct netaddr* last_addr) {
	DEBUG("%s (%s) over %s expired",
			node->name, netaddr_to_str_s(&nbuf[0], node->addr),
			netaddr_to_str_s(&nbuf[1], last_addr));

	if (node->last_addr != NULL && netaddr_cmp(node->last_addr, last_addr) == 0)
		_olsr_node_expired(node);
	else
		remove_other_route(node, last_addr);

	if (node->last_addr == NULL && node->other_routes == NULL)
		_remove_olsr_node(node);
}

void add_olsr_node(struct netaddr* addr, struct netaddr* last_addr, uint8_t vtime, uint8_t distance, metric_t metric, char* name) {
	struct olsr_node* n = get_node(addr);

	if (n == NULL)
		n = _new_olsr_node(addr, distance, metric, vtime, name);

	if (n == NULL) {
		puts("ERROR: add_olsr_node failed - out of memory");
		return;
	}

	/* we have added a new node */
	if (n->last_addr == NULL) {
#ifdef ENABLE_NAME
		if (n->name == NULL && name != NULL)
			n->name = strdup(name);
#endif
		add_other_route(n, last_addr, distance, metric, vtime);
		add_free_node(n);

		return;
	}

	struct olsr_node* new_lh = get_node(last_addr);

	/* minimize MPR count */
	if (new_lh->type == NODE_TYPE_NHDP) {

		/* see if a better flooding MPR is availiable */
		if (n->flood_mpr != NULL && netaddr_cmp(n->flood_mpr, last_addr) != 0) {
			struct nhdp_node* old_flood_mpr = h1_deriv(get_node(n->flood_mpr));

			if (old_flood_mpr != NULL && h1_deriv(new_lh)->flood_neighbors > old_flood_mpr->flood_neighbors) {
				DEBUG("switching flooding MPR (%s -> %s)", h1_super(old_flood_mpr)->name, new_lh->name);
				old_flood_mpr->mpr_neigh_flood--;
				h1_deriv(new_lh)->mpr_neigh_flood++;
				netaddr_switch(&n->flood_mpr, last_addr);
			}
		}

		/* see if a better routing MPR is availiable */
		if (new_lh->path_metric + metric == n->path_metric && netaddr_cmp(last_addr, n->last_addr) != 0) {
			struct nhdp_node* cur_mpr = h1_deriv(get_node(n->next_addr));

			/* see if the new route is better, that means uses a neighbor that is alreay
			   used for reaching (more) 2-hop neighbors. */
			if (cur_mpr != NULL && (new_lh != NULL &&
				h1_deriv(new_lh)->mpr_neigh_route + 1 > cur_mpr->mpr_neigh_route)) {
				DEBUG("switching routing MPR (%s -> %s)", h1_super(cur_mpr)->name, new_lh->name);
				_update_children(n->addr, NULL);
				push_default_route(n);
				add_free_node(n);
			}
		}
	}

	/* worse or same route */
	if (new_lh->path_metric + metric >= n->path_metric || netaddr_cmp(last_addr, n->last_addr) == 0) {
		add_other_route(n, last_addr, distance, metric, vtime);
		return;
	}

	DEBUG("better route found (old: %d (%d) hops over %s new: %d (%d) hops over %s)",
		n->distance, n->path_metric, netaddr_to_str_s(&nbuf[0], n->last_addr),
		distance, new_lh->path_metric + metric, netaddr_to_str_s(&nbuf[1], last_addr));

	n->distance = distance; // only to keep free_nodes sorted
	_update_children(n->addr, NULL);
	push_default_route(n);
	add_other_route(n, last_addr, distance, metric, vtime);
	add_free_node(n);
}

bool is_known_msg(struct netaddr* addr, uint16_t seq_no, uint8_t vtime) {
	struct olsr_node* node = get_node(addr);
	if (!node) {
		node = _new_olsr_node(addr, 255, RFC5444_METRIC_INFINITE, vtime, NULL);
		node->seq_no = seq_no;
		return false;
	}

	uint16_t tmp = node->seq_no;
	/*	S1 > S2 AND S1 - S2 < MAXVALUE/2 OR
		S2 > S1 AND S2 - S1 > MAXVALUE/2	*/
	if (((seq_no > tmp) && (seq_no - tmp < (1 << 15))) ||
		((seq_no < tmp) && (tmp - seq_no > (1 << 15))) ) {
		node->seq_no = seq_no;
		return false;
	}

	return true;
}

#ifdef ENABLE_NAME
void print_routing_graph(void) {
	puts("\n----BEGIN ROUTING GRAPH----\n");
	puts("subgraph routing {");
	puts("\tedge [ color = red ]");
	struct olsr_node* node, *tmp;
	avl_for_each_element(get_olsr_head(), node, node) {
		if (node->addr != NULL && node->last_addr != NULL) {
			tmp = get_node(node->last_addr);
			printf("\t%s -> %s\n", tmp ? tmp->name : local_name, node->name);
		}
	}
	puts("}");

	puts("subgraph mpr_f {");
	puts("\tedge [ color = green ]");
	puts("// BEGIN FLOODING MPR");
	avl_for_each_element(get_olsr_head(), node, node) {
		if (node->type == NODE_TYPE_NHDP && h1_deriv(node)->mpr_slctr_flood) {
			printf("\t%s -> %s\n", node->name, local_name);
		}
	}
	puts("// END FLOODING MPR");
	puts("}");

	puts("subgraph mpr_r {");
	puts("\tedge [ color = blue ]");
	puts("// BEGIN ROUTING MPR");
	avl_for_each_element(get_olsr_head(), node, node) {
		if (node->distance == 1 && h1_deriv(node)->mpr_slctr_route) {
			printf("\t%s -> %s\n", node->name, local_name);
		}
	}
	puts("// END ROUTING MPR");
	puts("}");

	puts("\n----END ROUTING GRAPH----\n");

}
#else
void print_routing_graph(void) {}
#endif

void print_topology_set(void) {
#ifndef ENABLE_DEBUG_OLSR
	struct netaddr_str nbuf[3];
#endif

	struct alt_route* route;
	struct olsr_node* node;

	puts("");
	puts("---[ Topology Set ]--");
#ifdef ENABLE_NAME
	printf(" [ %s | %s ]\n", netaddr_to_str_s(&nbuf[0], get_local_addr()), local_name);
#else
	printf(" [%s]\n", netaddr_to_str_s(&nbuf[0], get_local_addr()));
#endif

	avl_for_each_element(get_olsr_head(), node, node) {

		printf("%s ", netaddr_to_str_s(&nbuf[0], node->addr));
#ifdef ENABLE_NAME
		printf("(%s)", node->name);
#endif
		printf("\t=> %s; %d hops, metric: %d, next: %s (%d), %lds ",
			netaddr_to_str_s(&nbuf[1], node->last_addr),
			node->distance,
			node->path_metric,
			netaddr_to_str_s(&nbuf[2], node->next_addr),
			node->link_metric,
			node->expires - time_now());
		if (node->type == NODE_TYPE_NHDP) {
			printf("%s %.2f ",
			node->pending ? "pending" : "",
			h1_deriv(node)->link_quality);
			printf("[%d/%d|%d] [%s%s]",
			h1_deriv(node)->mpr_neigh_flood,
			h1_deriv(node)->flood_neighbors,
			h1_deriv(node)->mpr_neigh_route,
			h1_deriv(node)->mpr_slctr_flood ? "F" : " ",
			h1_deriv(node)->mpr_slctr_route ? "R" : " "
			);
		}
		if (node->flood_mpr != NULL)
			printf(" flood: %s", netaddr_to_str_s(&nbuf[0], node->flood_mpr));
		puts("");

		simple_list_for_each (node->other_routes, route) {
			printf("\t\t\t=> %s (%d); %ld s\n",
				netaddr_to_str_s(&nbuf[0], route->last_addr),
				route->link_metric,
				route->expires - time_now());
		}
	}
	puts("---------------------");
}
