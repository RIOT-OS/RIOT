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

#ifndef NHDP_H_
#define NHDP_H_

#include "common/avl.h"
#include "common/netaddr.h"

#include "node.h"

#ifdef __cplusplus
extern "C" {
#endif

struct olsr_node *olsr2_add_neighbor(struct netaddr *addr, metric_t metric, uint8_t vtime, char *name);

#ifdef __cplusplus
}
#endif

#endif /* NHDP_H_ */
