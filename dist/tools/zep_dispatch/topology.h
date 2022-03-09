/*
 * Copyright (C) 2021 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "list.h"
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Struct describing a graph of nodes and their connections
 */
typedef struct {
    list_node_t nodes;  /**< list of nodes */
    list_node_t edges;  /**< list of connections between nodes. Unused if topology is flat */
    struct sockaddr_in6 sniffer_addr;   /**< address of sniffer node. Unused if topology is flat */
    bool has_sniffer;   /**< true if a sniffer node is connected. Unused if topology is flat */
    bool flat;          /**< flat topology, all nodes are connected to each other */
} topology_t;

/**
 * @brief   Parse a file with topology information
 *
 * @param[in]  file Filename to open & parse
 *                  May be "-" to use stdin
 * @param[out] out  Topology from file
 *
 * @return 0 on success, error otherwise
 */
int topology_parse(const char *file, topology_t *out);

/**
 * @brief   Print topology as Graphviz diagram
 *
 * @param[in] file_out  Filename to write to
 *                      May be "-" to use stdout
 * @param[in] t         The topology to render
 *
 * @return 0 on success, error otherwise
 */
int topology_print(const char *file_out, const topology_t *t);

/**
 * @brief   Populate a spot in the topology with a connected node
 *
 * @param[in, out]  t       topology to use
 * @param[in]       mac     ZEP l2 address of the new node
 * @param[in]       mac_len ZEP l2 address length
 * @param[in]       addr    real address of the virtual node
 *
 * @return true if the node could be added to the topology
 */
bool topology_add(topology_t *t, const uint8_t *mac, uint8_t mac_len,
                  struct sockaddr_in6 *addr);

/**
 * @brief   Add a sniffer to the topology
 *          A sniffer node will receive every packet but won't be able to
 *          send packets on it's own.
 *
 * @param[in, out]  t       topology to configure
 * @param[in]       addr    real address of the sniffer
 */
void topology_set_sniffer(topology_t *t, struct sockaddr_in6 *addr);

/**
 * @brief   Send a buffer to all nodes connected to a source node
 *
 * @param[in] t             topology to use
 * @param[in] sock          socket to use for sending
 * @param[in] src_addr      source node address
 * @param[in] buffer        ZEP frame to send
 * @param[in] len           ZEP frame length
 */
void topology_send(const topology_t *t, int sock,
                   const struct sockaddr_in6 *src_addr,
                   void *buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* TOPOLOGY_H */
