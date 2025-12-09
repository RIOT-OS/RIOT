/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_storage_heap RIOT Registry Storage: Heap
 * @ingroup     sys
 * @brief       RIOT Registry Storage Heap, only uses the heap for testing.
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"

#include "registry/storage.h"

#ifndef REGISTRY_STORAGE_HEAP_CAPACITY
#  define REGISTRY_STORAGE_HEAP_CAPACITY 100
#endif

static registry_error_t load(
    const registry_storage_instance_t *storage,
    const load_cb_t load_cb);

static registry_error_t save(
    const registry_storage_instance_t *storage,
    const registry_node_t *node,
    const registry_value_t *value);

typedef struct {
    registry_node_t node;
    void *buf;
    size_t buf_len;
} heap_storage_t;

/* This is the "storage device" containing all the data */
static heap_storage_t heap_storage[REGISTRY_STORAGE_HEAP_CAPACITY];
static size_t heap_storage_len = 0;

/* Storage interface descriptor to be registered in the RIOT Registry */
registry_storage_t registry_storage_heap = {
    .load = load,
    .save = save,
};

static registry_error_t load(
    const registry_storage_instance_t *storage,
    const load_cb_t load_cb)
{
    (void)storage;

    for (size_t i = 0; i < heap_storage_len; i++) {
        load_cb(&heap_storage[i].node, heap_storage[i].buf, heap_storage[i].buf_len);
    }

    return REGISTRY_ERROR_NONE;
}

static registry_error_t save(
    const registry_storage_instance_t *storage,
    const registry_node_t *node,
    const registry_value_t *value)
{
    assert(node->type == REGISTRY_NODE_PARAMETER);
    assert(node->value.parameter.parameter != NULL);
    assert(node->value.parameter.instance != NULL);

    (void)storage;

    /* Search value in storage */
    for (size_t i = 0; i < heap_storage_len; i++) {
        if (heap_storage[i].node.value.parameter.instance == node->value.parameter.instance &&
            heap_storage[i].node.value.parameter.parameter == node->value.parameter.parameter) {
            memcpy(heap_storage[i].buf, value->buf, value->buf_len);
            return REGISTRY_ERROR_NONE;
        }
    }

    /* Value not found in storage => Append it at the end */
    heap_storage[heap_storage_len] = (heap_storage_t){
        .node = *node,
        .buf = malloc(value->buf_len),
        .buf_len = value->buf_len,
    };
    memcpy(heap_storage[heap_storage_len].buf, value->buf, value->buf_len);

    heap_storage_len++;
    return REGISTRY_ERROR_NONE;
}
