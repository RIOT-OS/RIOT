/*
 * SPDX-FileCopyrightText: 2023 ML!PA Consulting Gmbh
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_l2scanlist   Scan List - List of wireless access points
 * @ingroup     net
 * @brief       Internal list data structure of scanned access points
 *              @ref NETOPT_SCAN
 * @{
 *
 * @file
 * @brief       L2 Scan list API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include <stdlib.h>

#include "list.h"
#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type of a Link Layer scan list
 */
typedef struct l2scan_list {
    list_node_t head;          /**< List head, where head->next is the first element */
    /* items */
} l2scan_list_t;

/**
 * @brief   Empty the list to start a new scan
 *
 * @param[in, out]  list            Pointer to list
 * @param[in, out]  nodes           Pointer to nodes array
 * @param[in]       nodes_numof     Number of nodes in the array
 * @param[in]       node_size       Size of one node element in the array
 */
void l2scan_list_empty(l2scan_list_t *list,
                       list_node_t *nodes, unsigned nodes_numof,
                       size_t node_size);

/**
 * @brief   Insert a new scan result into the list
 *
 * @param[in, out]  list            Pointer to list
 * @param[in, out]  nodes           Pointer to nodes array
 * @param[in]       nodes_numof     Number of nodes in the array
 * @param[in]       node_size       Size of one node element in the array
 * @param[in]       result          New result to insert
 */
void l2scan_list_insert(l2scan_list_t *list,
                        list_node_t *nodes, unsigned nodes_numof,
                        size_t node_size,
                        const netopt_scan_result_t *result);

/**
 * @brief   Copy the content of a L2 scan list to an array to get rid of the list overhead
 *
 * @param[in]       list            Pointer to list
 * @param[out]      nodes_array     Buffer of nodes to store the result
 * @param[in]       nodes_numof     Maximum number of nodes that can be copied
 * @param[in]       node_size       Size of one node element in the array
 *
 * @return  Number of copied nodes
 */
unsigned l2scan_list_to_array(const l2scan_list_t *list,
                              void *nodes_array, unsigned nodes_numof,
                              size_t node_size);

#ifdef __cplusplus
}
#endif

/** @} */
