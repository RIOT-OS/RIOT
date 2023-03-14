/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NET_L2SCAN_LIST_H
#define NET_L2SCAN_LIST_H

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
    list_node_t *head;          /**< Pointer to the list head node */
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

#ifdef __cplusplus
}
#endif

#endif /* NET_L2SCAN_LIST_H */
/** @} */
