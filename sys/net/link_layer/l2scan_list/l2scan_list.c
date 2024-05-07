/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_l2scan_list
 * @{
 *
 * @file
 * @brief       List to store the result of a network scan
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "list.h"
#include "net/netopt.h"
#include "net/l2scan_list.h"

#define SCAN_LIST_NODE_AT(array, pos, size) ((void *)(((uint8_t *)(array)) + ((pos) * (size))))

/**
 * @brief  Basic type of a scan result in a list
 */
typedef struct scan_list_node {
    list_node_t node;               /* Basic node */
    netopt_scan_result_t result;    /* Basic scan result */
} scan_list_node_t;

static scan_list_node_t *_scan_list_get_insert(l2scan_list_t *list,
                                               list_node_t *array, unsigned array_numof,
                                               size_t item_size)
{
    scan_list_node_t *lowest = (scan_list_node_t *)array;
    for (unsigned i = 0; i < array_numof; i++) {
        /* look for free slot or lowest element */
        scan_list_node_t *result = SCAN_LIST_NODE_AT(array, i, item_size);
        if (!result->result.strength) {
            return result; /* free slot */
        }
        if (result->result.strength < lowest->result.strength) {
            lowest = result; /* override lowest element */
        }
    }
    list_remove(&list->head, &lowest->node);
    return lowest;
}

void l2scan_list_empty(l2scan_list_t *list,
                       list_node_t *nodes, unsigned nodes_numof,
                       size_t node_size)
{
    list->head.next = NULL;
    memset(nodes, 0, nodes_numof * node_size);
}

void l2scan_list_insert(l2scan_list_t *list,
                        list_node_t *nodes, unsigned nodes_numof,
                        size_t node_size,
                        const netopt_scan_result_t *result)
{
    scan_list_node_t *insert = _scan_list_get_insert(list, nodes,
                                                     nodes_numof, node_size);
    *insert = (scan_list_node_t) { .node = { .next = NULL }, };
    memcpy(&insert->result, result, node_size - sizeof(list_node_t));
    const scan_list_node_t *next;
    list_node_t *before = &list->head;
    while ((next = (scan_list_node_t *)before->next) &&
           next->result.strength > result->strength) {
        before = before->next;
    }
    list_add(before, &insert->node);
}

unsigned l2scan_list_to_array(const l2scan_list_t *list,
                              void *nodes_array, unsigned nodes_numof,
                              size_t node_size)
{
    list_node_t *node = list->head.next;
    uint8_t *buf = nodes_array;
    size_t size = node_size - sizeof(*node);
    unsigned i;
    for (i = 0; i < nodes_numof && node; i++, buf += size, node = node->next) {
        memcpy(buf, &node[1], size);
    }
    return i;
}
