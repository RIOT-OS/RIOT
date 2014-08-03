#ifndef OLSR_H_
#define OLSR_H_

#include <stdbool.h>

#include "common/avl.h"
#include "common/netaddr.h"

#include "node.h"

void add_olsr_node(struct netaddr *addr, struct netaddr *last_addr, uint8_t vtime, uint8_t distance, metric_t metric, char *name);
bool is_known_msg(struct netaddr *src, uint16_t seq_no, uint8_t vtime);
bool remove_expired(struct olsr_node *node);
void route_expired(struct olsr_node *node, struct netaddr *last_addr);

void print_topology_set(void);
void print_routing_graph(void);

#endif /* OLSR_H_ */
