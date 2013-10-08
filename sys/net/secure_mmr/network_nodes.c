/*
 * network_nodes.c - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        network_nodes.c
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
 * @date        12.09.2013 19:53:25
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

#include <math.h>

/* Radio Includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

/* Project includes */
#include "secure_routing_globals.h"
#include "network_nodes.h"
#include "microeval.h"


/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
// The first item in the tree
static network_node_t *tree_root = 0;
// Change this to make it rediscover routes to neighbours
static int use_neighbours_as_routes = 1;
// Number of nodes in tree
static uint32_t tree_size = 0;
// Prevent splay tree changing from multiple threads
static mutex_t splay_mutex;

network_node_t *network_nodes_splay(network_node_t *node, uint16_t i);


/**
 * @brief  Initialises the network nodes
 */
void network_nodes_init()
{
    mutex_init(&splay_mutex);
}

/**
 * @brief  Return the root of the network nodes tree
 *
 * @return      Pointer to the top node
 */
network_node_t *network_nodes_get_root()
{
    return tree_root;
}

/**
 * @brief  Creates a new node structure on heap, inits it
 *
 * @return      Pointer to the new node
 */
network_node_t *new_node(uint16_t node_id)
{
    ulog_info("> Creating node #%d.Cur Root=%d", (uint8_t)node_id,
              tree_root ? tree_root->node_id : 0);

    // Implies we are to add, and node was not found
    network_node_t *result = (network_node_t *)malloc(sizeof(network_node_t));

    if (result == NULL) {
        ulog_error("Out of memory");
    }

    memset(result, 0, sizeof(network_node_t));
    result->node_id = node_id;
    result->is_neighbour = 0;
    result->gateway = 0;
    return result;
}

void network_nodes_get_network_stats(network_node_t *start_node,
                                     int current_depth,
                                     network_stats_t *stats)
{
    if (start_node == NULL) {
        return;
    }

    if (start_node->node_id != basestation_addr) { // not doing for BS
        if (start_node->is_neighbour) {
            stats->neighbour_count++;

            if (current_depth > stats->oldest_neighbour_depth) {
                stats->oldest_neighbour = start_node;
                stats->oldest_neighbour_depth = current_depth;
            }
        }

        if (!start_node->is_neighbour) {
            stats->route_count++;

            if (current_depth > stats->oldest_route_depth) {
                stats->oldest_route = start_node;
                stats->oldest_route_depth = current_depth;
            }
        }
    }

    current_depth++;
    // Right first so larger addressed get dropped before smaller
    network_nodes_get_network_stats((network_node_t *)start_node->right,
                                    current_depth, stats);
    network_nodes_get_network_stats((network_node_t *)start_node->left,
                                    current_depth, stats);
}

/**
 * @brief Will delete the oldest node which is in the wrong category
 */
void network_nodes_pop_oldest_node(network_node_t *start_node,
                                   int going_to_add_neighbour)
{
    // If tree is not full then don't delete anyone
    if (tree_size < MAX_NUMBER_NODES) {
        return;
    }

    // Tree is full, now we need to find the oldest neighbour node,
    // the oldest route node and the number of neighbour nodes and
    // the number of route nodes. Should be done in N time

    ulog("!> Popping node. Tree size=%d. Max Size=%d",
         tree_size, MAX_NUMBER_NODES);

    network_stats_t stats;
    memset(&stats, 0, sizeof(network_stats_t));
    network_nodes_get_network_stats(network_nodes_get_root(), 0, &stats);
    ulog("> Stats result. OldestNeighbour=%d OldestRoute=%d. \
         NeigbourCount=%d RouteCount=%d",
         stats.oldest_neighbour ? (uint8_t)stats.oldest_neighbour->node_id : 0,
         stats.oldest_route ? (uint8_t)stats.oldest_route->node_id : 0,
         (uint8_t)stats.neighbour_count,
         (uint8_t)stats.route_count);

    ulog("> Oldest Neighbour Depth=%d, Oldest Route Depth=%d",
         (uint8_t)stats.oldest_neighbour_depth,
         (uint8_t)stats.oldest_route_depth);

    // Now depending on stats result, we know which node to delete
    if (stats.route_count > ROUTE_NODES_ALLOCATION) {
        if (stats.oldest_route == 0) {
            ulog_error("ROUTE NODE IS NULL");
        }

        network_nodes_delete_node(network_nodes_get_root(),
                                  stats.oldest_route->node_id);
    }
    else if (stats.neighbour_count > NEIGHBOURS_NODES_ALLOCATION) {
        network_nodes_delete_node(network_nodes_get_root(),
                                  stats.oldest_neighbour->node_id);
    }
    else {
        // Both allocations are equally filled. In this case, we want to delete
        // the same type as the one we are about to delete

        if (going_to_add_neighbour) {
            network_nodes_delete_node(network_nodes_get_root(),
                                      stats.oldest_neighbour->node_id);
        }
        else {
            network_nodes_delete_node(network_nodes_get_root(),
                                      stats.oldest_route->node_id);
        }
    }
}

/**
 * @brief  Returns the node if it exists or adds a new entry if it does not.
 *         Based on Splay Tree adding implementation. "An implementation of
 *         top-down splaying"
 *         D. Sleator <sleator@cs.cmu.edu> March 1992
 */
network_node_t *network_nodes_find_or_add(network_node_t *root,
        uint16_t node_id,
        int only_find)
{
    if (node_id == 0) {
        ulog_error("Trying to find node with ID 0. Not allowed");
        return 0;
    }

    mutex_lock(&splay_mutex);

    network_node_t *found_node = 0;

    if (root != NULL) {
        root = network_nodes_splay(root, node_id);

        if (node_id < root->node_id) {
            if (only_find) {
                tree_root = root;
                mutex_unlock(&splay_mutex);
                return 0;
            }

            found_node = new_node(node_id);
            found_node->left = root->left;
            found_node->right = (struct network_node_t *)root;
            root->left = NULL;
            tree_size++;

            tree_root = found_node;
            mutex_unlock(&splay_mutex);
            return found_node;
        }
        else if (node_id > root->node_id) {
            if (only_find) {
                tree_root = root;
                mutex_unlock(&splay_mutex);
                return 0;
            }

            found_node = new_node(node_id);
            found_node->right = root->right;
            found_node->left = (struct network_node_t *)root;
            root->right = NULL;
            tree_size++;

            tree_root = found_node;

            mutex_unlock(&splay_mutex);
            return found_node;
        }
        else {
            tree_root = root;
            // already in tree

            mutex_unlock(&splay_mutex);
            return root;
        }
    }

    if (tree_root != NULL) {
        ulog_error("Consistency error in splay tree!!!");
    }

    if (only_find == 0) {
        tree_root = new_node(node_id);
        tree_size++;
    }

    mutex_unlock(&splay_mutex);
    return tree_root;
}



/**
 * @brief  Deletes all nodes from memory
 */
void network_nodes_delete_all(network_node_t *start_node)
{
    if (start_node == 0) {
        return;
    }

    network_nodes_delete_all((network_node_t *)start_node->left);
    network_nodes_delete_all((network_node_t *)start_node->right);

    ulog("> Deleting node %d", start_node->node_id);
    free(start_node);

    tree_root = 0;
}

/**
 * @brief  Sets the node as unreachable (deletes route, neighbour). See header
 *                      file for further documentation
 */
int network_nodes_set_node_unreachable(network_node_t *start_node,
                                               uint16_t addr)
{
    // same as deleting from routing table. But in our case just set
    // gateway to 0

    if (start_node == 0) {
        return 0;
    }

    int change_count = 0;

    if (start_node->node_id == addr) {
        ulog("> Deneighbouring node %d", start_node->node_id);

        start_node->is_neighbour = 0;
        memset(start_node->pairwise_key, 0, SECURE_ROUTING_KEYSIZE);
        start_node->missed_keep_alives = 0;
        start_node->wrong_MACs = 0;
        start_node->gateway = 0;
        start_node->hop_distance = 0;
        change_count++;
    }

    if (start_node->gateway == addr) {
        // The gateway to a node has been de-neighboured. Delete the gateway
        start_node->gateway = 0;
        ulog("> Node %d lost gateway %d", start_node->node_id,
             start_node->gateway);
        change_count++;
    }


    change_count = network_nodes_set_node_unreachable((network_node_t *)
                                                          start_node->left,
                                                          addr);
    change_count += network_nodes_set_node_unreachable((network_node_t *)
                                                           start_node->right,
                                                           addr);

    return change_count;
}

/**
 * @brief  Examine the list and write the route between this node and 'node'
 *         in the field
 */
void network_nodes_extract_route(network_node_t *node,
                                         uint16_t *list,
                                         uint8_t list_len)
{
    int this_node_pos = -1;
    int dst_node_pos = -1;

    for (int i = 0; i < list_len; i++) {
        if (list[i] == get_this_node_address()) {
            this_node_pos = i;
        }

        if (list[i] == node->node_id) {
            dst_node_pos = i;
        }

        // No need to break cos it's just 10 items
    }

    if (this_node_pos == -1 || dst_node_pos == -1) {
        ulog_info("ER: Could not find nodes");
        // Could not find both nodes
        return;
    }

    ulog_info("ER: this node pos = %d dst node pos = %d",
                   this_node_pos, dst_node_pos);

    int k = 0;

    if (this_node_pos > dst_node_pos) {
        for (int i = this_node_pos; i >= dst_node_pos; i--) {
            node->full_route[k] = list[i];
            k++;
        }
    }
    else {
        for (int i = this_node_pos; i <= dst_node_pos; i++) {
            node->full_route[k] = list[i];
            k++;
        }
    }
}

/**
 * @brief  Add this route as an entry with the specified gateway. See header
 *         file for detailed explanation.
 */
int network_nodes_add_route(uint8_t dst, uint8_t gateway,
                                    uint8_t hop_count,
                                    uint16_t *list,
                                    uint8_t list_len)
{
    network_nodes_pop_oldest_node(network_nodes_get_root(), 0);

    network_node_t *node = network_nodes_find_or_add(tree_root, dst, false);

    if (node != 0) {
        if (node->gateway == 0) {
            ulog("> Route to %d (gateway=%d) not in table. Adding. \
                 Hop Distance=%d", dst, gateway, hop_count);
            node->gateway = gateway;
            node->hop_distance = hop_count;

            network_nodes_extract_route(node, list, list_len);
        }
        else {
            if (node->gateway != gateway) {
                if (node->hop_distance == 0) {
                    ulog_error("0 hop route found.");
                }

                if (hop_count < node->hop_distance) {
                    ulog("> Shorter route found for %d. NewHops=%d, \
                          OldHops=%d. OldGateway=%d, NewGateway=%d",
                          node->node_id, hop_count, node->hop_distance,
                          node->gateway, gateway);
                          node->gateway = gateway;
                          node->hop_distance = hop_count;

                    network_nodes_extract_route(node, list, list_len);
                }
                else {
                    ulog("> New route for %d was longer!Not updating. \
                         NewHops=%d, OldHops=%d. OldGateway=%d, NewGateway=%d",
                         node->node_id,
                         hop_count,
                         node->hop_distance,
                         node->gateway,
                         gateway);
                }
            }
        }
    }
    else {
        ulog_error("Node could not be created");
    }

    return 0;
}

/**
 * @brief  Prints out a single node to console
 *
 */
void network_nodes_print(network_node_t *node)
{
    uint32_t seq_nmbr = node->seq_nmbr;
    printf("Node-ID:%d|Neighbour?%d| Gateway:%d|Distance(Hops):%d| \
                Missed-Keep-Alive:%d|SeqNmbr:%lu\n",
                node->node_id,
                node->is_neighbour,
                node->gateway,
                node->hop_distance,
                node->missed_keep_alives,
                seq_nmbr);
    printf("-- PairwiseKey=");

    for (int i = 0; i < SECURE_ROUTING_KEYSIZE; i++) {
        printf("%x", node->pairwise_key[i]);
    }

    printf("\n");

    if (node->hop_distance > 1) {
        printf("       -- Full Route=");

        for (int i = 0; i < 8; i++) {
            if (node->full_route[i + 1] == 0) {
                printf("%d.", node->full_route[i]);
                break;
            }
            else {
                printf("%d->", node->full_route[i]);
            }
        }

        printf("\n");
    }

    printf("\n");

}

/**
 * @brief   Prints out all nodes that are neighbours
 *
 */
void network_nodes_print_nodes(network_node_t *start_node, int only_neighbours)
{
    if (tree_root == 0) {
        ulog("No nodes in network node list");
    }

    if (start_node == 0) {
        return;
    }

    if (only_neighbours) {
        if (start_node->is_neighbour == 1) {
            network_nodes_print(start_node);
        }
    }
    else {
        network_nodes_print(start_node);
    }

    network_nodes_print_nodes((network_node_t *)start_node->left,
                                   only_neighbours);
    network_nodes_print_nodes((network_node_t *)start_node->right,
                                   only_neighbours);
}


/**
 * @brief  sets the sequence number of a node. See the header file for details.
 */
network_node_t *network_nodes_set_sequence_nmbr(uint16_t node_id,
                                                        sequence_number_t seq_nmbr)
{
    network_nodes_pop_oldest_node(network_nodes_get_root(), 0);

    network_node_t *node = network_nodes_find_or_add(tree_root,
                                                         node_id,
                                                         false);

    if (node != 0) {
        node->seq_nmbr = seq_nmbr;
        ulog("> Node %d sequence number is %lu", node->node_id, seq_nmbr);
    }

    return node;
}

/**
 * @brief  Set a node as being a neighbour by specifying the pairwise key.
 *         See header file for further details
 */
network_node_t *network_nodes_set_neighbour(uint8_t node_id,
                                                    uint8_t *pairwise_key)
{
    network_nodes_pop_oldest_node(network_nodes_get_root(), 1);

    network_node_t *node = network_nodes_find_or_add(tree_root,
                                                         node_id,
                                                         false);

    if (node == 0) {
        return 0;
    }

    if (node->is_neighbour == 0) {
        ulog_info("> EXPERIMENT(NewNeighbour, CurCount=%lu",
                   network_nodes_count_neighbours(network_nodes_get_root()));
    }

    node->is_neighbour = 1;

    if (use_neighbours_as_routes == 1) {
        node->gateway = node_id;
        node->hop_distance = 1;
    }
    else {
        node->gateway = 0;
    }

    memcpy(node->pairwise_key, pairwise_key, SECURE_ROUTING_KEYSIZE);

    return node;
}

/**
 * @brief  Increment the missed-keepalives of a node. See header file
 *                      for further details
 */
int network_nodes_increment_neighbours_keepalive(network_node_t *start_node)
{
    if (start_node == 0) {
        return 0;
    }

    int count = 0;

    if (start_node->is_neighbour == 1) {
        count = 1;
        start_node->missed_keep_alives += 1;
        ulog("> Node %d missed-keep-alive=%d", start_node->node_id,
                                                       start_node->missed_keep_alives);
    }

    count += network_nodes_increment_neighbours_keepalive((network_node_t *)
                                                               start_node->left);
    count += network_nodes_increment_neighbours_keepalive((network_node_t *)
                                                               start_node->right);

    return count;
}

/**
 * @brief  Removes nodes that exceed a particular limit. See header files for
 *         details
 */
int network_nodes_drop_dead_neighbours(network_node_t *start_node)
{
    if (start_node == 0) {
        return 0;
    }

    if (start_node->is_neighbour && start_node->missed_keep_alives >= 5) {
        network_nodes_deneighbour(start_node);
    }

    network_nodes_drop_dead_neighbours((network_node_t *)start_node->left);
    network_nodes_drop_dead_neighbours((network_node_t *)start_node->right);

    return 1;
}

/**
 * @brief  Given the id of a node, will say if this node is a neighbour of us
 */
int network_nodes_is_neighbour(uint16_t node_id)
{
    network_node_t *node = network_nodes_find_or_add(tree_root, node_id, true);

    if (node == 0) {
        return 0;
    }

    if (node->is_neighbour) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Removes a node as a neighbour. See header files for details
 */
void network_nodes_deneighbour(network_node_t *t)
{
    network_nodes_set_node_unreachable(t, t->node_id);
}

/**
 * @brief Called when the global key is updated. See header for extended
 *        description
 */
void network_nodes_set_globalkey(network_node_t *start_node,
                                         uint8_t *new_encryption_key)
{
    if (start_node == 0) {
        return;
    }

    // Reset wrong macs

    start_node->wrong_MACs  = 0;

    // Reset sequence numbers, because any old packets
    // can no longer be replayed due to changed global key
    start_node->seq_nmbr = 0;

    network_nodes_set_globalkey((network_node_t *) start_node->left,
                                     new_encryption_key);
    network_nodes_set_globalkey((network_node_t *) start_node->right,
                                     new_encryption_key);
}

/**
 * @brief Searches for a node in the list. See header files for details
 */
network_node_t *network_nodes_find(uint8_t node_id)
{
    return network_nodes_find_or_add(tree_root, node_id, true);
}


/**
 * @brief Counts the neighbours. See header file for details
 */
int network_nodes_count_neighbours(network_node_t *start_node)
{
    if (start_node == 0) {
        return 0;
    }

    int count = 0;

    if (start_node->is_neighbour == 1) {
        count++;
    }

    count += network_nodes_count_neighbours((network_node_t *)
                                                 start_node->left);
    count += network_nodes_count_neighbours((network_node_t *)
                                                 start_node->right);

    return count;
}

/**
 * @brief Will search for all neighbours, then copy to the specified array
 */
int network_nodes_packets_to_neighbors(network_node_t *start_node,
                                               secure_packet_t *packet,
                                               secure_packet_t *list,
                                               int list_size,
                                               int cur_pos)
{
    if (start_node == 0) {
        return cur_pos;
    }

    if (cur_pos == list_size) {
        return cur_pos;
    }

    if (start_node->is_neighbour == 1) {
        // Copy the packet because of in-place encryption
        secure_packet_t packet_copy;
        memcpy(&packet_copy, packet, sizeof(secure_packet_t));

        packet_copy.destination = start_node->node_id;

        if (packet_copy.destination != 0 &&
            packet_copy.destination != GLOBAL_BROADCAST &&
            // Recheck this to ensure no loop
            packet_copy.destination != LOCAL_BROADCAST) {
            ulog("Copying packet %d", packet_copy.destination);

            memcpy(&list[cur_pos], &packet_copy, sizeof(secure_packet_t));

            cur_pos++;
        }
    }

    cur_pos = network_nodes_packets_to_neighbors((network_node_t *)
                                                      start_node->left,
                                                      packet, list,
                                                      list_size,
                                                      cur_pos);
    return network_nodes_packets_to_neighbors((network_node_t *)
                                                   start_node->right,
                                                   packet,
                                                   list,
                                                   list_size,
                                                   cur_pos);
}

/**
 * @brief Searches for the first neighbour. See header files for details
 */
network_node_t *network_nodes_get_any_neighbour(network_node_t *start_node)
{
    if (start_node == 0) {
        return 0;
    }

    if (start_node->is_neighbour == 1) {
        return start_node;
    }

    network_node_t *node = network_nodes_get_any_neighbour((network_node_t *)
                                                                start_node->left);

    if (node != 0) {
        return node;
    }

    node = network_nodes_get_any_neighbour((network_node_t *)
                                                start_node->right);

    if (node != 0) {
        return node;
    }

    return 0;
}

/**
 * @brief       Returns the number of nodes with security errors greater
 *              than the specified limit. See header for more details
 */
int network_nodes_count_mac_errors(network_node_t *start_node,
                                           uint8_t greater_than_limit)
{
    if (start_node == 0) {
        return 0;
    }

    int count = 0;

    if (start_node->wrong_MACs > greater_than_limit) {
        count++;
    }

    count += network_nodes_count_mac_errors((network_node_t *)
                                                 start_node->left,
                                                 greater_than_limit);
    count += network_nodes_count_mac_errors((network_node_t *)
                                                 start_node->right,
                                                 greater_than_limit);

    return count;
}

/**
 * @brief   Splays the tree. From "An implementation of top-down splaying"
            D. Sleator <sleator@cs.cmu.edu> March 1992

   @param   node  The start node
   @param   i     The node_id
 */
network_node_t *network_nodes_splay(network_node_t *node, uint16_t i)
{
    network_node_t N, *l, *r, *y;

    if (node == 0) {
        return node;
    }

    N.left = N.right = NULL;
    l = r = &N;

    for (;;) {
        if (i < node->node_id) {
            //-------
            if (node->left == NULL) {
                break;
            }

            if (i < ((network_node_t *)(node->left))->node_id) {
                y = (network_node_t *)node->left;     /* rotate right */
                node->left = y->right;
                y->right = (struct network_node_t *)node;
                node = y;

                if (node->left == NULL) {
                    break;
                }
            }

            r->left = (struct network_node_t *)node;  /* link right */
            r = node;
            node = (network_node_t *)(node->left);
        }
        else if (i > node->node_id) {
            //-------
            if (node->right == NULL) {
                break;
            }

            if (i > ((network_node_t *)node->right)->node_id) {
                y = (network_node_t *)node->right;    /* rotate left */
                node->right = y->left;
                y->left = (struct network_node_t *)node;
                node = y;

                if (node->right == NULL) {
                    break;
                }
            }

            l->right = (struct network_node_t *)node; /* link left */
            l = node;
            node = (network_node_t *)(node->right);
        }
        else {
            break;
        }
    }

    l->right = node->left;    /* assemble */
    r->left = node->right;
    node->left = N.right;
    node->right = N.left;
    return node;
}

/**
 * @brief       Prints out the current list of nodes in tree form
 */
void network_nodes_print_all_nodes(network_node_t *root)
{
    if (root != NULL) {
        printf("Node:%d\n", root->node_id);

        printf("Left of %d: ", root->node_id);
        network_nodes_print_all_nodes((network_node_t *)root->left);

        printf("Right of %d: ", root->node_id);
        network_nodes_print_all_nodes((network_node_t *)root->right);
    }
    else {
        printf("Empty\n");
    }
}

/**
 * @brief       Deletes i from the tree if it's there. Return a pointer to the
 *              resulting tree.
 *              From "An implementation of top-down splaying"
 *              D. Sleator <sleator@cs.cmu.edu> March 1992
 **/
network_node_t *network_nodes_delete_node(network_node_t *root,
                                                  uint16_t node_id)
{
    network_node_t *x = 0;

    if (root == NULL) {
        return NULL;
    }

    mutex_lock(&splay_mutex);
    root = network_nodes_splay(root, node_id);

    if (node_id == root->node_id) {
        /* found it */
        if (root->left == NULL) {
            x = (network_node_t *)(root->right);
        }
        else {
            x = network_nodes_splay((network_node_t *)(root->left), node_id);
            x->right = root->right;
        }

        tree_size--;

        free(root);

        tree_root = x;
        mutex_unlock(&splay_mutex);
        return x;
    }

    tree_root = root;
    mutex_unlock(&splay_mutex);
    return root;                         /* It wasn't there */
}

