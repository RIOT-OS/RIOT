/*
 * Copyright (C) 2025 Ahmad Mahmod <ahmad.nawras.mahmod@gmail.com -- mahmod@unistra.fr> *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_rpl_sr_table RPL source routing table
 * @ingroup     net_gnrc_rpl
 * @brief       Implementation of RPL source routing table and operations
 *
 * @see <a href="https://tools.ietf.org/html/rfc6550">RFC 6550</a>
 *
 * @{
 *
 * @file
 * @brief       Definitions for RPL source routing table and operations
 *
 * @author      Ahmad Mahmod <ahmad.nawras.mahmod@gmail.com -- mahmod@unistra.fr>
 */

#include <stdint.h>
#include <stddef.h>
#include "net/gnrc/ipv6.h"

/**
 * @def GNRC_SR_MAX_ROUTE_SIZE
 * @brief Maximum number of hops per route in the source routing table.
 */
#ifndef GNRC_SR_MAX_ROUTE_SIZE
#   define GNRC_SR_MAX_ROUTE_SIZE (16)
#endif

/**
 * @def GNRC_SR_FIB_TABLE_HEADER_ELEMENTS
 * @brief Number of header entries in the SR FIB table.
 */
#ifndef GNRC_SR_FIB_TABLE_HEADER_ELEMENTS
#   define GNRC_SR_FIB_TABLE_HEADER_ELEMENTS (2)
#endif

/**
 * @def GNRC_SR_FIB_TABLE_SIZE
 * @brief Total size of the SR FIB table.
 */
#ifndef GNRC_SR_FIB_TABLE_SIZE
#   define GNRC_SR_FIB_TABLE_SIZE (32)
#endif

/**
 * @def GNRC_SR_FIB_TABLE_HEADER_SIZE
 * @brief Size of the header section in the SR FIB table.
 */
#ifndef GNRC_SR_FIB_TABLE_HEADER_SIZE
#   define GNRC_SR_FIB_TABLE_HEADER_SIZE (32)
#endif

/**
 * @def GNRC_SR_FIB_TABLE_ENTRY_POOL_SIZE
 * @brief Number of entries in the FIB entry pool.
 */
#ifndef GNRC_SR_FIB_TABLE_ENTRY_POOL_SIZE
#   define GNRC_SR_FIB_TABLE_ENTRY_POOL_SIZE (32)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the source routing table with a specified root node.
 *
 * @param[in] root IPv6 address of the root node.
 * @param[in] iface Interface identifier (currently unused).
 */
void gnrc_sr_initialize_table(ipv6_addr_t *root, kernel_pid_t iface);

/**
 * @brief Prints the current contents of the source routing table (FIB).
 */
void gnrc_sr_table_print(void);

/**
 * @brief Prints the full source route to a given node from the root.
 *
 * @param[in] node IPv6 address of the destination node.
 */
void gnrc_sr_print_table_route(ipv6_addr_t *node);

/**
 * @brief Adds a new source routing entry to the routing table.
 *
 * @param[in] child       IPv6 address of the destination (child) node.
 * @param[in] prefix      Prefix length (typically 128 for full address).
 * @param[in] parent      IPv6 address of the parent node.
 * @param[in] sr_iface_id Interface ID associated with this route.
 * @param[in] sr_flags    Flags indicating the type of route (e.g., FIB_FLAG_RPL_ROUTE).
 * @param[in] lifetime    Lifetime of the route in seconds.
 *
 * @return 0 on success, -1 on failure.
 */
int gnrc_sr_add_new_dst(ipv6_addr_t *child, size_t prefix, ipv6_addr_t *parent,
                        kernel_pid_t sr_iface_id, uint32_t sr_flags, uint32_t lifetime);

/**
 * @brief Deletes an existing source routing entry from the table.
 *
 * @param[in] dst_node IPv6 address of the destination node.
 * @param[in] dst_size Size of the destination address (should be 16).
 *
 * @return 0 on success, -1 on failure.
 */
int gnrc_sr_delete_route(ipv6_addr_t *dst_node, size_t dst_size);

/**
 * @brief Retrieves the full source route from a node to the root.
 *
 * The route is constructed by walking up the parent hierarchy in the FIB table.
 *
 * @param[in]  dst_node     IPv6 address of the target node.
 * @param[out] route_buffer Output buffer to hold the route (from node to root).
 * @param[out] route_length Pointer to variable to store number of hops.
 *
 * @return 0 on success, -1 if route could not be reconstructed.
 */
int gnrc_sr_get_full_route(ipv6_addr_t *dst_node, ipv6_addr_t *route_buffer, size_t *route_length);

#ifdef __cplusplus
}
#endif

/** @} */
