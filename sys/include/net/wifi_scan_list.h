/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    net_wifi_scan_list List of scanned WiFis access points
 * @ingroup     net
 * @brief       Wrapper around l2scan list for WiFi scan results
 * @{
 *
 * @file
 * @brief       Thin wrapper around l2scan list to support WiFi scan results
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef NET_WIFI_SCAN_LIST_H
#define NET_WIFI_SCAN_LIST_H

#include <stddef.h>

#include "l2scan_list.h"
#include "net/netdev/wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Empty the WiFi scan list to start a new scan
 *
 * @param[in, out]  list            Pointer to WiFi scan result list
 * @param[in, out]  nodes           Pointer to WiFi scan result nodes array
 * @param[in]       nodes_numof     Number of nodes in the array
 */
static inline void wifi_scan_list_empty(wifi_scan_list_t *list,
                                        wifi_scan_list_node_t *nodes,
                                        unsigned nodes_numof)
{
    l2scan_list_empty(list, &nodes->node, nodes_numof,
                      sizeof(wifi_scan_list_node_t));
}

/**
 * @brief   Insert a new WiFi scan result into the list
 *
 * @param[in, out]  list            Pointer to WiFi scan result list
 * @param[in, out]  nodes           Pointer to WiFi scan result nodes array
 * @param[in]       nodes_numof     Number of nodes in the array
 * @param[in]       result          New WiFi scan result to insert
 */
static inline void wifi_scan_list_insert(wifi_scan_list_t *list,
                                         wifi_scan_list_node_t *nodes,
                                         unsigned nodes_numof,
                                         const wifi_scan_result_t *result)
{
    l2scan_list_insert(list, &nodes->node, nodes_numof,
                       sizeof(wifi_scan_list_node_t), &result->base);
}

/**
 * @brief   Copy the content of a WiFi scan list to an array to get rid of the list overhead
 *
 * @param[in]       list            Pointer to list
 * @param[in]       array           Buffer of nodes to store the result
 * @param[in]       numof           Maximum number of nodes that can be copied
 *
 * @return  Number of copied nodes
 */
static inline unsigned wifi_scan_list_to_array(const wifi_scan_list_t *list,
                                               wifi_scan_result_t *array,
                                               unsigned numof)
{
    return l2scan_list_to_array(list, array, numof,
                                sizeof(wifi_scan_list_node_t));
}

#ifdef __cplusplus
}
#endif

#endif /* NET_WIFI_SCAN_LIST_H */
/** @} */
