/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_btree Binary tree
 * @ingroup     sys
 * @brief       Self-balancing sorted binary tree
 *
 * This module provides a minimal self-balancing sorted binary tree
 * implementation. It can be used to implement key-value stores and other
 * key-based structures.
 *
 * The binary search tree provided here is optimized for fast key lookups.
 * Insertions and deletions are slow and should be avoided when possible. Both
 * insertions and deletions trigger the relative slow balancing algorithm to
 * keep the tree into an relative optimal shape for search operations.
 *
 * The btree_node_t structs stores the required internal references and a key.
 * It is up to the implementor to inherit from the struct and add the required
 * data that must be associated with the key.
 *
 * example:
 *
 * ```
 * typedef struct {
 *     btree_node_t node;
 *     uint32_t value;
 * } keyval_entry_t;
 * ```
 *
 * Inserting a keyval entry can be done with:
 *
 * ```
 * static keyval_entry_t entry = { .value = 5 };
 * if (btree_insert(btree, &entry.node, 12) < 0) {
 *     LOG_ERROR("Key already exists in tree\n");
 * }
 * ```
 * Note that the btree does not copy the inserted object. The object must be
 * kept in memory while it is used in the tree.
 *
 * Retrieving the pair is then as simple as:
 *
 * ```
 * keyval_entry_t *entry = (keyval_entry_t*)btree_find_key(btree, 12);
 * if (entry) {
 *     _handle_entry(entry);
 * }
 * ```
 *
 * Removing the entry from the btree can done with:
 *
 * ```
 * keyval_entry_t *entry = (keyval_entry_t*)btree_remove(btree, 12);
 * if (entry) {
 *     _deallocate_entry(entry);
 * }
 * ```
 *
 * The removal function returns the removed entry or NULL when no entry was
 * removed. The returned entry can safely be reinserted or can be deallocated by
 * an external memory allocator.
 *
 * Allocating entry nodes is the responsibility of the application. If all nodes
 * are equal in allocated size the @ref sys_memarray module can be used for dynamic
 * allocation of entry nodes.
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */
#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    BTREE_OK = 0,                   /**< No error */

    /**
     * @brief Node could not be inserted because the key already exists in the
     *        tree
     */
    BTREE_ERROR_NODE_EXISTS = -1,
};

/**
 * @brief Forward declaration for the btree node struct
 */
typedef struct btree_node btree_node_t;

/**
 * @brief btree node
 */
struct btree_node {
    struct btree_node *left;    /**< Lower value reference  */
    struct btree_node *right;   /**< Higher value reference */
    struct btree_node *parent;  /**< Parent node reference  */
    uint32_t key;               /**< Key of this node       */
};

/**
 * @brief btree reference
 */
typedef struct {
    btree_node_t *start; /**< Root node reference */
} btree_t;

/**
 * @brief retrieve the key of a node
 *
 * @param       Node    The node to retrieve the key from
 *
 * @returns             The key of the node
 */
static inline uint32_t btree_node_key(btree_node_t *node) {
    return node->key;
}

/**
 * @brief Callback function for tree traversal
 *
 * @param   node    Current node
 * @param   depth   Depth of the current node
 * @param   ctx     Pointer to the supplied traversal context
 */
typedef void (*btree_cb_t)(btree_node_t *node, size_t depth, void *ctx);

/**
 * @brief Traverse the full tree, visiting every node exactly once in ascending
 *        order.
 *
 * @warning     Modifying the btree during the traverse is not supported and
 *              consistency of the tree can't be guaranteed.
 *
 * @param   btree   Tree to traverse
 * @param   cb      Callback to call for every node
 * @param   ctx     Context to pass to every callback
 */
void btree_traverse(btree_t *btree, btree_cb_t cb, void *ctx);

/**
 * @brief Find a node with a specified key
 *
 * @param   btree   Tree to search
 * @param   key     Key to find
 *
 * @returns         The node with the requested key
 * @returns         NULL if no node was found
 */
btree_node_t *btree_find_key(btree_t *btree, uint32_t key);

/**
 * @brief Insert a new node into the tree
 *
 * @param   btree   Tree to insert into
 * @param   New     Preallocated node to insert
 * @param   key     Key to insert
 */
int btree_insert(btree_t *btree, btree_node_t *new, uint32_t key);

/**
 * @brief Remove a node from the tree
 *
 * @param   btree   Tree to remove from
 * @param   key     Key to remove from the tree
 *
 * @returns         A reference to the deleted node
 * @returns         NULL if no node to delete was found
 */
btree_node_t *btree_remove(btree_t *btree, uint32_t key);

/**
 * @brief Auxiliary function to retrieve the depth of the tree
 *
 * @param   btree   tree to find the max depth for
 *
 * @returns         Depth of the tree
 */
size_t btree_max_depth(btree_t *btree);

/**
 * @brief Auxiliary function to dump a print of the tree to stdio
 *
 * @param   btree   tree to print
 */
void btree_dump(btree_t *btree);

#ifdef __cplusplus
}
#endif
#endif /* UUID_H */
/** @} */
