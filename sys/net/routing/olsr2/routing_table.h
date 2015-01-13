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

#ifndef ROUTING_H_
#define ROUTING_H_

#include "node.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * add a node to the list of pending nodes
 */
void add_free_node(struct olsr_node *node);

/*
 * remove a node from the list of pending nodes
 * returns true if node was found and removed
 */
bool remove_free_node(struct olsr_node *node);

/*
 * try to find a route for pending nodes
 */
void fill_routing_table(void);

#ifdef __cplusplus
}
#endif

#endif /* ROUTING_H_ */
