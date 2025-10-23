/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_string_path RIOT Registry String Path
 * @ingroup     sys
 * @brief       RIOT Registry String Path module
 * @{
 *
 * This module provides functions to convert between @ref registry_node_t and
 * a string path representation of it.
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
#include <string.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "clist.h"
#include "registry.h"
#include "registry/util.h"
#include "registry/error.h"
#include "registry/find.h"

#include "registry/string_path.h"

#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)

typedef struct {
    const char **path;
    size_t path_len;
    size_t position;
} _registry_string_comparator_context_t;

static int _internal_registry_to_group_string_path(
    const registry_group_t *current_group,
    const registry_group_t *group,
    char *path)
{
    int size = snprintf(NULL, 0, "/%s", current_group->name);

    if (path != NULL) {
        snprintf(path, size + 1, "/%s", current_group->name);
    }

    if (group == current_group) {
        return size;
    }

    for (size_t i = 0; i < current_group->groups_len; i++) {
        int res = _internal_registry_to_group_string_path(current_group->groups[i], group,
                                                          path != NULL ? path + size : NULL);
        if (res >= 0) {
            return size + res;
        }
    }

    return -EINVAL;
}

static int _internal_registry_to_parameter_string_path(
    const registry_group_t *current_group,
    const registry_parameter_t *parameter,
    char *path)
{
    int size = snprintf(NULL, 0, "/%s", current_group->name);

    if (path != NULL) {
        snprintf(path, size + 1, "/%s", current_group->name);
    }

    /* check if the parameter is a child of this group */
    for (size_t i = 0; i < current_group->parameters_len; i++) {
        if (current_group->parameters[i] == parameter) {
            int sub_size = snprintf(NULL, 0, "/%s", current_group->parameters[i]->name);

            if (path != NULL) {
                snprintf(path + size, sub_size + 1, "/%s", current_group->parameters[i]->name);
            }

            return size + sub_size;
        }
    }

    /* check if the parameter is the child of a subgroup */
    for (size_t i = 0; i < current_group->groups_len; i++) {
        int res = _internal_registry_to_parameter_string_path(current_group->groups[i], parameter,
                                                              path != NULL ? path + size : NULL);
        if (res >= 0) {
            return size + res;
        }
    }

    return -EINVAL;
}

int registry_node_to_string_path(
    const registry_node_t *node,
    char *path)
{
    assert(node != NULL);

    int size = 0;

    switch (node->type) {
    case REGISTRY_NODE_NAMESPACE:
        size = snprintf(NULL, 0, "/%s", node->value.namespace->name);

        if (path != NULL) {
            return snprintf(path, size + 1, "/%s", node->value.namespace->name);
        }
        break;

    case REGISTRY_NODE_SCHEMA:
        size = snprintf(NULL, 0, "/%s/%s", node->value.schema->namespace->name,
                        node->value.schema->name);

        if (path != NULL) {
            return snprintf(path, size + 1, "/%s/%s", node->value.schema->namespace->name,
                            node->value.schema->name);
        }
        break;

    case REGISTRY_NODE_INSTANCE:
        size = snprintf(NULL, 0, "/%s/%s/%s", node->value.instance->schema->namespace->name,
                        node->value.instance->schema->name, node->value.instance->name);

        if (path != NULL) {
            return snprintf(path, size + 1, "/%s/%s/%s",
                            node->value.instance->schema->namespace->name,
                            node->value.instance->schema->name, node->value.instance->name);
        }
        break;

    case REGISTRY_NODE_GROUP:
        size = snprintf(NULL, 0, "/%s/%s/%s", node->value.group.instance->schema->namespace->name,
                        node->value.group.instance->schema->name, node->value.group.instance->name);

        if (path != NULL) {
            snprintf(path, size + 1, "/%s/%s/%s",
                     node->value.group.instance->schema->namespace->name,
                     node->value.group.instance->schema->name, node->value.group.instance->name);
        }

        for (size_t i = 0; i < node->value.group.instance->schema->groups_len; i++) {
            int res =
                _internal_registry_to_group_string_path(
                    node->value.group.instance->schema->groups[i], node->value.group.group,
                    path != NULL ? path + size : NULL);
            if (res >= 0) {
                return size += res;
            }
        }

        return -EINVAL;
        break;

    case REGISTRY_NODE_PARAMETER:
        size = snprintf(NULL, 0, "/%s/%s/%s",
                        node->value.parameter.instance->schema->namespace->name,
                        node->value.parameter.instance->schema->name,
                        node->value.parameter.instance->name);

        if (path != NULL) {
            snprintf(path, size + 1, "/%s/%s/%s",
                     node->value.parameter.instance->schema->namespace->name,
                     node->value.parameter.instance->schema->name,
                     node->value.parameter.instance->name);
        }

        /* check if the parameter is a child of this schema */
        for (size_t i = 0; i < node->value.parameter.instance->schema->parameters_len; i++) {
            if (node->value.parameter.instance->schema->parameters[i] ==
                node->value.parameter.parameter) {
                int sub_size = snprintf(NULL, 0, "/%s",
                                        node->value.parameter.instance->schema->parameters[i]->name);

                if (path != NULL) {
                    snprintf(path + size, sub_size + 1, "/%s",
                             node->value.parameter.instance->schema->parameters[i]->name);
                }

                return size + sub_size;
            }
        }

        /* check if the parameter is the child of a group */
        for (size_t i = 0; i < node->value.parameter.instance->schema->groups_len; i++) {
            int res =
                _internal_registry_to_parameter_string_path(
                    node->value.parameter.instance->schema->groups[i],
                    node->value.parameter.parameter,
                    path !=
                            NULL ?
                        path + size :
                        NULL);
            if (res >= 0) {
                return size += res;
            }
        }

        return -EINVAL;
        break;
    }

    return size;
}

static registry_find_result_type _compare_node_by_string(
    const registry_node_t *node,
    const void *context)
{
    _registry_string_comparator_context_t *data = (void *)context;

    const char *name = NULL;

    switch (node->type) {
    case REGISTRY_NODE_NAMESPACE:
        name = node->value.namespace->name;
        break;

    case REGISTRY_NODE_SCHEMA:
        name = node->value.schema->name;
        break;

    case REGISTRY_NODE_INSTANCE:
        name = node->value.instance->name;
        break;

    case REGISTRY_NODE_GROUP:
        name = node->value.group.group->name;
        break;

    case REGISTRY_NODE_PARAMETER:
        name = node->value.parameter.parameter->name;
        break;
    }

    if (strncmp(data->path[data->position], name, strlen(name)) == 0) {
        if (data->path_len == data->position + 1) {
            return REGISTRY_FIND_EXACT_MATCH;
        }

        data->position++;
        return REGISTRY_FIND_PARTIAL_MATCH;
    }

    return REGISTRY_FIND_NO_MATCH;
}

registry_error_t registry_node_from_string_path(
    const char **path,
    const size_t path_len,
    registry_node_t *node)
{
    _registry_string_comparator_context_t context = {
        .path = path,
        .path_len = path_len,
        .position = 0,
    };

    return registry_find(_compare_node_by_string, &context, node);
}

#endif
