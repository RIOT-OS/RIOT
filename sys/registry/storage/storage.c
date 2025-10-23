/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_storage RIOT Registry Storage
 * @ingroup     sys
 * @brief       RIOT Registry Storage module to store configurations to non-volatile storage
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "clist.h"

#include "registry/storage.h"
#include "registry/error.h"

static const registry_storage_instance_t **_storage_instances;

/* registry_load */
static registry_error_t _registry_load_cb(
    const registry_node_t *node,
    const void *buf,
    const size_t buf_len)
{
    assert(node->type == REGISTRY_NODE_PARAMETER);
    assert(node->value.parameter.parameter != NULL);
    assert(node->value.parameter.instance != NULL);

    return registry_set(node, buf, buf_len);
}

registry_error_t registry_storage_load(
    const registry_storage_instance_t *storage_instance)
{
    return storage_instance->storage->load(storage_instance, _registry_load_cb);
}

/* registry_save */
static registry_error_t _registry_save_export_cb(
    const registry_node_t *node,
    const void *context)
{
    const registry_storage_instance_t *storage_instance = context;

    /* the registry also exports just the namespace or just a schema, */
    /* but the storage is only interested in configuration parameter values */
    if (node->type != REGISTRY_NODE_PARAMETER) {
        return REGISTRY_ERROR_NONE;
    }

    /* get value of configuration parameter */
    registry_value_t value;
    registry_get(node, &value);

    /* save parameter value via the save function of the provided storage instance */
    return storage_instance->storage->save(storage_instance, node, &value);
}

registry_error_t registry_storage_save(
    const registry_storage_instance_t *storage_instance,
    const registry_node_t *node)
{
    assert(storage_instance != NULL);

    if (storage_instance->storage->save_start) {
        storage_instance->storage->save_start(storage_instance);
    }

    registry_error_t res = registry_export(node, _registry_save_export_cb, REGISTRY_EXPORT_ALL,
                                           storage_instance);

    if (storage_instance->storage->save_end) {
        storage_instance->storage->save_end(storage_instance);
    }

    return res;
}

registry_error_t registry_storage_set_instances(
    const registry_storage_instance_t **storage_instances)
{
    assert(storage_instances != NULL);

    _storage_instances = storage_instances;

    return REGISTRY_ERROR_NONE;
}

registry_error_t registry_storage_get_instances(
    const registry_storage_instance_t ***storage_instances)
{
    assert(storage_instances != NULL);

    *storage_instances = _storage_instances;

    return REGISTRY_ERROR_NONE;
}
