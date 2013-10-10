/*
 * network_nodes.h - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        network_nodes.h
 * @brief       Table of the known network nodes.
 *              This module manages a list of all the network nodes known by
 *              this sensor node in the network. The nodes it knows could be
 *              added because as part of a route request, because these nodes
 *              are neighbours or when the node is specifically excluded from
 *              the list.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      D. Sleator <sleator@cs.cmu.edu> (SPLAY TREE)
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 12:23:33
 */



#ifndef __NETWORK_NODES_H
#define __NETWORK_NODES_H


/*---------------------------------------------------------------------------*/
//                                      Structures
/*---------------------------------------------------------------------------*/

/**
 * @brief network_node struct that contains known information about other nodes
 *        in the network
 */
typedef struct network_node {
    // The node address and network
    uint16_t node_id;
    // Dynamically allocate this, to avoid routes
    uint8_t pairwise_key[SECURE_ROUTING_KEYSIZE];
    /* number of local keep-alive periods that this node did not receive any
     * local keep-alive packet from the node with the ID above
     **/
    uint8_t missed_keep_alives;
    /* the number of packets with wrong MAC received from the node with the
     * above ID since the last local keep-alive
     **/
    uint8_t wrong_MACs;
    // Says if this node is a neighbour.
    uint8_t is_neighbour;
    // The gateway used to reach this node
    uint8_t gateway;
    // The sequence number
    sequence_number_t seq_nmbr;
    // How many hops away is this node
    uint8_t hop_distance;
    // Stores the full route to this node
    uint8_t full_route[10];
    // Pointer to the next item in this list
    struct network_node_t *left;
    // Pointer to the next item in this list
    struct network_node_t *right;
} network_node_t;

// This structure provides info about the network. Used to decide which node
// can safely be removed and replaced with another
typedef struct network_stats {
    network_node_t *oldest_neighbour;
    network_node_t *oldest_route;
    int neighbour_count;
    int route_count;
    int oldest_neighbour_depth;
    int oldest_route_depth;
}  network_stats_t;


/**
 * @brief  Initialises the network nodes
 */
void network_nodes_init(void);

/**
 * @brief   Prints out all nodes that are neighbours
 *
 * @param   only_neighbours   limits the print out to just neighbours
 * @param   start_node        The node at which search should start
 *
 */
void network_nodes_print_nodes(network_node_t *start_node,
                               int only_neighbours);

/**
 * @brief   Sets the sequence number of a network node
 *          Given a node id and a sequence number, it will create an entry in
 *          the nodes table mapping the node id with the sequence number. If
 *          it does not exist, will be created
 *
 * @param   node_id    The id of the node for whom the key should be set
 * @param   seq_nmbr   The new sequence ID
 *
 * @return Pointer to the node modified, or node newly created
 */
network_node_t *network_nodes_set_sequence_nmbr(uint16_t node_id,
                                                sequence_number_t seq_nmbr);


/**
 * @brief  Deletes all nodes from memory
 */
void network_nodes_delete_all(network_node_t *start_node);

/**
 * @brief   Sets the pairwise_key for a neighbouring node
 *
 * Given a node id and a pairwise key, either updates the node pairwise
 * key or it creates a new node and adds it the list
 *
 * @param   node_id         The id of the node for whom the key should be set
 * @param   pairwise_key    The key to be set for the node. Length is globally
 *                          defined
 *
 * @return Pointer to the node modified, or node newly created
 */
network_node_t *network_nodes_set_neighbour(uint8_t node_id,
        uint8_t *pairwise_key);


/**
 * @brief   Add an entry to the routing table
 *
 *
 * @param       dst             Destination network address
 * @param       gateway         Next hop network address to destination
 * @param       hop_count       Number of hops till destination
 * @param       list            Full list of routes
 * @param       list_len        Length of list
 * @param       count_backwards Indicates that the list is reversed
 *                              (rrep vs rreq)
 *
 * @return  true if route table entry was successfully added;
 *          false otherwise (e.g. no free space in route table).
 */
int network_nodes_add_route(uint8_t dst, uint8_t gateway, uint8_t hop_count,
                            uint16_t *list, uint8_t list_len);

/**
 * Returns the number of neighbours that this node has stored
 *
 * @param The node where the search should start
 *
 * @return Number of neighbouring nodes
 */
int network_nodes_count_neighbours(network_node_t *start_node);

/**
 * Returns the first neighbour that is found. There is no guarantee that it
 * will return the same neighbour each time. A neighbouring node shares a
 * pairwise key with this node. It's possible that the neighbour entry may be
 * stale and neighbouring node may not actually be working
 *
 * @param The node where the search should start
 *
 * @return Pointer to a neighbouring node or 0 if no known neighbour
 */
network_node_t *network_nodes_get_any_neighbour(network_node_t *start_node);


/**
 * Given a node id, returns a pointer to the node structure. Will search
 * through the list of known nodes to find this node.
 *
 * @param node_id the address of the node to be searched for
 *
 * @return Pointer to the node, or 0 of node not found
 */
network_node_t *network_nodes_find(uint8_t node_id);


/**
 * Walks through the list of neighbours and increments the keep-alive value of
 * the nodes by 1
 *
 * @param The node where the search should start
 *
 * @return Number of neighbours incremented
 */
int network_nodes_increment_neighbours_keepalive(network_node_t *start_node);


/**
 * Walks through the list of neighbours and removes any neighbour whose
 * keep-alive has exceeded the threshhold required
 *
 * @return Number of neighbours pruned
 */
int network_nodes_drop_dead_neighbours(network_node_t *start_node);

/**
 * Sets the specified node to not be a neighbor anymore. Clears the pairwise
 * key and resets all values
 *
 * @param node The pointer to the neighbour to be deneighborred
 *
 */
void network_nodes_deneighbour(network_node_t *node);

/**
 * @brief  Remove all route table entries with a given gateway.
 *         Note: Won't remove static routes.
 *
 * @param     addr        The gateway network address
 * @param     start_node  Where the search should start
 *
 * @return  The number of deleted routes.
 */
int network_nodes_set_node_unreachable(network_node_t *start_node,
                                       uint16_t addr);

/**
 * @brief  Returns the number of nodes with mac errors greater than
 *         the specified limit
 *
 * @param       start_node         The node where the search should start
 * @param       greater_than_limit The limit
 *
 * @return      The number of nodes found
 */
int network_nodes_count_mac_errors(network_node_t *start_node,
                                   uint8_t greater_than_limit);



/**
 * @brief  Returns the node if it exists or adds a new entry if it does not
 *
 * @param       start_node   Where the search should start
 * @param       node_id      The address of the node
 * @param       only_find    Indicates if the node should be added if not found
 *
 * @return      Pointer to the found or added node
 */
network_node_t *network_nodes_find_or_add(network_node_t *start_node,
                                          uint16_t node_id,
                                          int only_find);

/**
 * Given the id of a node, will say if this node is a neighbour of us
 *
 * @param     node_id The address of the node
 *
 * @return 1 if neighbour, or 0 otherwise
 */
int network_nodes_is_neighbour(uint16_t node_id);

/**
 * Should be called when the global key is updated. Various
 * properties that are dependent on the global key will be
 * reset
 *
 * @param   new_encryption_key   The new global key
 *
 */
void network_nodes_set_globalkey(network_node_t *start_node,
                                 uint8_t *new_encryption_key);

/**
 * Used to call the send function for all neighbours. Will call the specifed
 * function for each neighbour, after changing the send function to the ID of
 * the specified node
 *
 * @param       start_node    The node to start the search at
 * @param       send_packet   The callback function
 *
 */
int network_nodes_packets_to_neighbors(network_node_t *start_node,
                                       secure_packet_t *packet,
                                       secure_packet_t *list,
                                       int list_size,
                                       int cur_pos);

/**
 * @brief  Return the root of the network nodes tree
 *
 * @return  Pointer to the top node
 */
network_node_t *network_nodes_get_root(void);

/**
 * @brief  Prints out the entire node tree in binary tree format
 *
 * @param root The start of the tree
 */
void network_nodes_print_all_nodes(network_node_t *root);

/**
 * @brief  Deletes a node from the tree and frees the memory
 *
 * @param root    The start of the tree to search
 * @param node_id The id of the node to search for
 *
 * @return The root of the resulting tree
 */
network_node_t *network_nodes_delete_node(network_node_t *root,
                                          uint16_t node_id);

/**
 * @brief  Removes oldest route
 *
 * @param start_node             The start of the tree to search
 * @param going_to_add_neighbour If we are going to insert neighbour.
 *
 */
void network_nodes_pop_oldest_node(network_node_t *start_node,
                                   int going_to_add_neighbour);

#endif
