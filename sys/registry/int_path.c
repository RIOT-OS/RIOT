/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_int_path RIOT Registry Int Path
 * @ingroup     sys
 * @brief       RIOT Registry integer path module
 * @{
 *
 * This module provides functions to convert between @ref registry_node_t and
 * @ref registry_int_path_t.
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "clist.h"
#include "registry.h"
#include "registry/util.h"
#include "registry/error.h"
#include "registry/find.h"

#include "registry/int_path.h"

registry_error_t registry_node_to_int_path(
    const registry_node_t *node,
    registry_int_path_t *path)
{
    assert(node != NULL);
    assert(path != NULL);

    switch (node->type) {
    case REGISTRY_NODE_NAMESPACE:
        path->type = REGISTRY_INT_PATH_TYPE_NAMESPACE;
        path->value.namespace_path.namespace_id = node->value.namespace->id;
        break;

    case REGISTRY_NODE_SCHEMA:
        path->type = REGISTRY_INT_PATH_TYPE_SCHEMA;
        path->value.schema_path.namespace_id = node->value.schema->namespace->id;
        path->value.schema_path.schema_id = node->value.schema->id;
        break;

    case REGISTRY_NODE_INSTANCE:
        path->type = REGISTRY_INT_PATH_TYPE_INSTANCE;
        path->value.instance_path.namespace_id = node->value.instance->schema->namespace->id;
        path->value.instance_path.schema_id = node->value.instance->schema->id;
        path->value.instance_path.instance_id = node->value.instance->id;
        break;

    case REGISTRY_NODE_GROUP:
        path->type = REGISTRY_INT_PATH_TYPE_GROUP;
        path->value.group_path.namespace_id = node->value.group.group->schema->namespace->id;
        path->value.group_path.schema_id = node->value.group.group->schema->id;
        path->value.group_path.instance_id = node->value.group.instance->id;
        path->value.group_path.group_id = node->value.group.group->id;
        break;

    case REGISTRY_NODE_PARAMETER:
        path->type = REGISTRY_INT_PATH_TYPE_PARAMETER;
        path->value.parameter_path.namespace_id =
            node->value.parameter.parameter->schema->namespace->id;
        path->value.parameter_path.schema_id = node->value.parameter.parameter->schema->id;
        path->value.parameter_path.instance_id = node->value.parameter.instance->id;
        path->value.parameter_path.parameter_id = node->value.parameter.parameter->id;
        break;
    }

    return REGISTRY_ERROR_NONE;
}

static registry_find_result_type _compare_node_by_id(
    const registry_node_t *node,
    const void *context)
{
    const registry_int_path_t *path = context;

    bool id_matches = false;
    bool path_type_matches = false;

    switch (node->type) {
    case REGISTRY_NODE_NAMESPACE:
        id_matches = node->value.namespace->id == path->value.namespace_path.namespace_id;
        path_type_matches = path->type == REGISTRY_INT_PATH_TYPE_NAMESPACE;
        break;

    case REGISTRY_NODE_SCHEMA:
        id_matches = node->value.schema->id == path->value.schema_path.schema_id;
        path_type_matches = path->type == REGISTRY_INT_PATH_TYPE_SCHEMA;
        break;

    case REGISTRY_NODE_INSTANCE:
        id_matches = node->value.instance->id == path->value.instance_path.instance_id;
        path_type_matches = path->type == REGISTRY_INT_PATH_TYPE_INSTANCE;
        break;

    case REGISTRY_NODE_GROUP:
        id_matches = node->value.group.group->id == path->value.group_path.group_id;
        path_type_matches = path->type == REGISTRY_INT_PATH_TYPE_GROUP ||
                            path->type == REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER;
        break;

    case REGISTRY_NODE_PARAMETER:
        id_matches = node->value.parameter.parameter->id == path->value.parameter_path.parameter_id;
        path_type_matches = path->type == REGISTRY_INT_PATH_TYPE_PARAMETER ||
                            path->type == REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER;
        break;
    }

    if (id_matches) {
        if (path_type_matches) {
            return REGISTRY_FIND_EXACT_MATCH;
        }

        return REGISTRY_FIND_PARTIAL_MATCH;
    }

    return REGISTRY_FIND_NO_MATCH;
}

registry_error_t registry_node_from_int_path(
    const registry_int_path_t *path,
    registry_node_t *node)
{
    assert(path != NULL);
    assert(node != NULL);

    return registry_find(_compare_node_by_id, path, node);
}
