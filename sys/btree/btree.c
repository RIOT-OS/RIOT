/*
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_btree2
 * @{
 * @file
 * @brief   Function implementations for btree structures
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "btree.h"

static void _path_helper(btree_node_t *node, size_t depth, void *ctx)
{
    (void)node;
    size_t *max_depth = ctx;
    if (*max_depth < depth) {
        *max_depth = depth;
    }
}

static size_t _max_path(btree_node_t *node)
{
    size_t len = 0;
    btree_t start = {.start = node};
    btree_traverse(&start, _path_helper, &len);
    return len;
}

/**
 * @brief find the node with a matching key
 *
 * @param   node    parent node
 */
static btree_node_t *_find_key(btree_node_t *cur_node, btree_node_t **parent, uint32_t key)
{
    while (cur_node) {
        if (cur_node->key > key) {
            *parent = cur_node;
            cur_node = cur_node->left;
        }
        else if (cur_node->key < key) {
            *parent = cur_node;
            cur_node = cur_node->right;
        }
        else {
            break;
        }
    }
    return cur_node;
}

static size_t _find_distance(btree_node_t *parent, btree_node_t *child)
{
    size_t depth = 0;
    while (child && (parent != child)) {
        depth++;
        child = child->parent;
    }
    return depth;
}

static btree_node_t *_find_min(btree_node_t *node)
{
    btree_node_t *min = NULL;
    _find_key(node, &min, 0);
    return min;
}

static btree_node_t *_find_max(btree_node_t *node)
{
    btree_node_t *max = NULL;
    _find_key(node, &max, UINT32_MAX);
    return max;
}

static void _replace_ref(btree_node_t *parent, btree_node_t *child, btree_node_t *new)
{
    if (parent->left == child) {
        parent->left = new;
    }
    else {
        parent->right = new;
    }
}

static void _update_parent_ref(btree_node_t *node, btree_node_t *parent)
{
    if (node) {
        node->parent = parent;
    }
}

static void _start_rotate(btree_t *btree, btree_node_t *root, btree_node_t *pivot)
{
    if (root->parent) {
        _replace_ref(root->parent, root, pivot);
    }
    else {
        btree->start = pivot;
    }
    _update_parent_ref(pivot, root->parent);
    root->parent = pivot;
}

static void _rotate_left(btree_t *btree, btree_node_t *root, btree_node_t *pivot)
{
    _start_rotate(btree, root, pivot);
    _update_parent_ref(pivot->left, root);
    root->right = pivot->left;
    pivot->left = root;
}

static void _rotate_right(btree_t *btree, btree_node_t *root, btree_node_t *pivot)
{
    _start_rotate(btree, root, pivot);
    _update_parent_ref(pivot->right, root);
    root->left = pivot->right;
    pivot->right = root;
}

static void _balance(btree_t *btree, btree_node_t *node)
{
    int child_balance = 0;
    while (node) {
        /* Determine max length of the other branch */
        /* TODO: reuse the result from the previous loop */
        int balance = _max_path(node->right) - _max_path(node->left);
        if (balance > 1) {
            /* right/left case */
            if (child_balance < 0) {
                /* Tranform to right/right case */
                _rotate_right(btree, node->right,
                              node->right->left);
            }
            /* right/right case */
            _rotate_left(btree, node, node->right);
            node = node->right;
            child_balance = 0;
        }
        else if (balance < -1) {
            /* left/right case */
            if (child_balance > 0) {
                /* Tranform to left/left case */
                _rotate_left(btree, node->left, node->left->right);
            }
            /* left/left case */
            _rotate_right(btree, node, node->left);
            node = node->left;
            child_balance = 0;
        }
        else {
            child_balance = balance;
            node = node->parent;
        }
    }
}

btree_node_t *btree_find_key(btree_t *btree, uint32_t key)
{
    btree_node_t *_tmp;
    return _find_key(btree->start, &_tmp, key);
}

int btree_insert(btree_t *btree, btree_node_t *new, uint32_t key)
{
    new->left = NULL;
    new->right = NULL;
    new->key = key;

    if (_find_key(btree->start, &new->parent, key) != NULL) {
        return BTREE_ERROR_NODE_EXISTS;
    }

    btree_node_t *p = new->parent;

    if (!p) {
        btree->start = new;
        return BTREE_OK;
    }

    if (p->key > key) {
        p->left = new;
    }
    else {
        p->right = new;
    }

    /* Only balance if the rank changes */
    if (!(p->left && p->right)) {
        _balance(btree, p);
    }

    return BTREE_OK;
}

btree_node_t *btree_remove(btree_t *btree, uint32_t key)
{
    btree_node_t *balance_start;
    btree_node_t *d = _find_key(btree->start, &balance_start, key);

    if (d == NULL) {
        /* Key not found */
        return NULL;
    }

    btree_node_t *left = _find_min(d->left);
    btree_node_t *right = _find_max(d->right);

    size_t left_len = _find_distance(d, left) + _max_path(left);
    size_t right_len = _find_distance(d, right) + _max_path(right);

    btree_node_t *replacement = left_len > right_len ? left : right;

    if (d->parent) {
        _replace_ref(d->parent, d, replacement);
    }
    else {
        btree->start = replacement;
    }

    if (replacement) {
        btree_node_t *p_replacement = replacement->parent;
        replacement->parent = d->parent;

        /* The replacement node can, by definition, only have one child node */
        btree_node_t *dangling = replacement->left ? replacement->left : replacement->right;

        /* Overwrite the node acting as the deleted nodes replacement with it's
         * child node */
        _replace_ref(p_replacement, replacement, dangling);
        /* Overwrite the replacements children with the deleted nodes children */
        replacement->left = d->left;
        replacement->right = d->right;
        _update_parent_ref(replacement->right, replacement);
        _update_parent_ref(replacement->left, replacement);

        balance_start = p_replacement;
    }

    _balance(btree, balance_start);

    return d;
}

void btree_traverse(btree_t *btree, btree_cb_t cb, void *ctx)
{
    btree_node_t *node = btree->start;
    if (!node) {
        return;
    }

    btree_node_t *parent = node->parent;
    btree_node_t *prev_node = parent;

    size_t depth = 0;

    /* Look Ma, no recursion! */
    while (node != parent) {
        btree_node_t *next = NULL;

        do {
            if (prev_node == node->parent) {
                depth++;
                next = node->left;
                if (next) {
                    break;
                }
            }
            if ((prev_node == node->parent) || (prev_node == node->left)) {
                cb(node, depth, ctx);
                next = node->right;
                if (next) {
                    break;
                }
            }
            depth--;
            next = node->parent;
        } while (0);

        prev_node = node;
        node = next;
    }
}

size_t btree_max_depth(btree_t *btree)
{
    if (btree->start) {
        return _max_path(btree->start);
    }
    return 0;
}

static void _dump(btree_node_t *node, size_t level, char *prefix)
{
    if (node) {
       _dump(node->right, level + 2, "┌─r");
       printf("%*s%s: %"PRIu32"\n", level, "", prefix, node->key);
       _dump(node->left, level + 2, "└─l");
    }
}

void btree_dump(btree_t *btree)
{
   _dump(btree->start, 0, "O");
}
