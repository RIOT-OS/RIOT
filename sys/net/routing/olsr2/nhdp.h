#ifndef NHDP_H_
#define NHDP_H_

#include "common/avl.h"
#include "common/netaddr.h"

#include "node.h"

struct olsr_node* add_neighbor(struct netaddr* addr, uint8_t vtime, char* name);

void print_neighbors(void);

#endif /* NHDP_H_ */
