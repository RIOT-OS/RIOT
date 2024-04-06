/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_storage RIOT Registry Storage
 * @ingroup     sys
 * @brief       RIOT Registry Storage module allowing to store configuration parameters to non-volatile storage
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
#include "registry/util.h"
#include "registry/error.h"

XFA_INIT_CONST(registry_storage_instance_t *, _registry_storage_instances_src_xfa);

/* registry_load */
static int _registry_load_cb(const registry_node_t *node, const void *buf, const size_t buf_len)
{
    assert(node->type == REGISTRY_NODE_PARAMETER);
    assert(node->location.parameter != NULL);
    assert(node->instance != NULL);

    return registry_set(node, buf, buf_len);
}

int registry_load(void)
{
    for (size_t i = 0;
         i < XFA_LEN(registry_storage_instance_t *, _registry_storage_instances_src_xfa); i++) {
        registry_storage_instance_t *src = _registry_storage_instances_src_xfa[i];

        int res = src->storage->load(src, _registry_load_cb);

        if (res != 0) {
            return res;
        }
    }

    return 0;
}

/* registry_save */
static int _registry_save_export_cb(const registry_node_t *node, const void *context)
{
    (void)context;

    /* the registry also exports just the namespace or just a schema, but the storage is only interested in configuration parameter values */
    if (node->type != REGISTRY_NODE_PARAMETER) {
        return 0;
    }

    /* check if a destination storage is registered */
    if (!_registry_storage_instance_dst) {
        return -REGISTRY_ERROR_NO_DST_STORAGE;
    }

    /* get value of configuration parameter */
    registry_value_t value;
    registry_get(node, &value);

    /* save parameter value via the save function of the registered destination storage */
    return _registry_storage_instance_dst->storage->save(_registry_storage_instance_dst,
                                                         node, &value);
}

int registry_save(const registry_node_t *node)
{
    int res;

    if (!_registry_storage_instance_dst) {
        return -REGISTRY_ERROR_NO_DST_STORAGE;
    }

    if (_registry_storage_instance_dst->storage->save_start) {
        _registry_storage_instance_dst->storage->save_start(_registry_storage_instance_dst);
    }

    res = registry_export(node, _registry_save_export_cb, 0, NULL);

    if (_registry_storage_instance_dst->storage->save_end) {
        _registry_storage_instance_dst->storage->save_end(_registry_storage_instance_dst);
    }

    return res;
}
