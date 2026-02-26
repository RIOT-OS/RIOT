/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Registry shell commands
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "registry.h"
#include "registry/util.h"
#include "registry/int_path.h"
#include "registry/storage.h"
#include "shell.h"

/**
 * @brief Separator character to define hierarchy in configurations names.
 */
#define REGISTRY_CLI_PATH_SEPARATOR '/'

static int _parse_string_path(const char *string_path,
                              registry_int_path_t *registry_path)
{
    char *ptr = (char *)string_path;

    if (string_path == NULL || *ptr == '\0') {
        return -EINVAL;
    }

    /* get namespace_id */
    registry_namespace_id_t namespace_id = strtol(ptr, &ptr, 10);

    if (*ptr == '\0') {
        registry_path->type = REGISTRY_INT_PATH_TYPE_NAMESPACE;
        registry_path->value.namespace_path = (registry_namespace_int_path_t){
            .namespace_id = namespace_id,
        };
        return 0;
    }

    /* get schema_id */
    ptr++; /* skip the '/' character */
    registry_schema_id_t schema_id = strtol(ptr, &ptr, 10);

    if (*ptr == '\0') {
        registry_path->type = REGISTRY_INT_PATH_TYPE_SCHEMA;
        registry_path->value.schema_path = (registry_schema_int_path_t){
            .namespace_id = namespace_id,
            .schema_id = schema_id,
        };
        return 0;
    }

    /* get instance_id */
    ptr++; /* skip the '/' character */
    registry_instance_id_t instance_id = strtol(ptr, &ptr, 10);

    if (*ptr == '\0') {
        registry_path->type = REGISTRY_INT_PATH_TYPE_INSTANCE;
        registry_path->value.instance_path = (registry_instance_int_path_t){
            .namespace_id = namespace_id,
            .schema_id = schema_id,
            .instance_id = instance_id,
        };
        return 0;
    }

    /* get group_or_parameter_id */
    ptr++; /* skip the '/' character */
    registry_group_or_parameter_id_t group_or_parameter_id = strtol(ptr, &ptr, 10);

    if (*ptr == '\0') {
        registry_path->type = REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER;
        registry_path->value.group_or_parameter_path = (registry_group_or_parameter_int_path_t){
            .namespace_id = namespace_id,
            .schema_id = schema_id,
            .instance_id = instance_id,
            .group_or_parameter_id = group_or_parameter_id,
        };
        return 0;
    }

    return 0;
}

static registry_error_t _export_cb(const registry_node_t *node, const void *context)
{
    (void)context;

    /* calculate the indentation based on the the exported data type */
    /* fallthrough switch is intentional */
    /* the more nesting we have, the more indentation we need. => highest nesting level first */
    int indentation = 0;

    switch (node->type) {
    case REGISTRY_NODE_PARAMETER:
    case REGISTRY_NODE_GROUP:
        indentation++;
        /* fallthrough */

    case REGISTRY_NODE_INSTANCE:
        indentation++;
        /* fallthrough */

    case REGISTRY_NODE_SCHEMA:
        indentation++;
        /* fallthrough */

    case REGISTRY_NODE_NAMESPACE:
        indentation++;
    }

    printf("%*c\b", ((indentation - 1) * 2) + 1, ' ');

    /* print the path element, that is currently being exported */
    switch (node->type) {
    case REGISTRY_NODE_NAMESPACE:
        printf("%" PRIu8 " %s\n", node->value.namespace->id, node->value.namespace->name);
        break;

    case REGISTRY_NODE_SCHEMA:
        printf("%" PRIu32 " %s\n", node->value.schema->id, node->value.schema->name);
        break;

    case REGISTRY_NODE_INSTANCE:
        printf("%" PRIu16 " %s\n", node->value.instance->id, node->value.instance->name);
        break;

    case REGISTRY_NODE_GROUP:
        printf("%" PRIu16 " %s (group)\n", node->value.group.group->id, node->value.group.group->name);
        break;

    case REGISTRY_NODE_PARAMETER:
        printf("%" PRIu8 " %s\n", node->value.parameter.parameter->id, node->value.parameter.parameter->name);
        break;
    }

    return 0;
}

static int _registry(int argc, char **argv)
{
    registry_int_path_t path;
    registry_node_t node;
    registry_value_t value;

    if (argc == 1) {
        /* show help for main commands */
        goto help_error;
    }

    if (strcmp(argv[1], "get") == 0) {
        if (_parse_string_path(argv[2], &path) < 0) {
            printf("usage: %s %s <path>\n", argv[0], argv[1]);
            return 1;
        }

        if (path.type != REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER) {
            return -EINVAL;
        }

        /* get instance and parameter of the path */
        int res = registry_node_from_int_path(&path, &node);

        if (node.type != REGISTRY_NODE_PARAMETER) {
            return -EINVAL;
        }

        if (res == 0) {
            res = registry_get(&node, &value);

            if (res == 0) {
                /* get the length of the value as a string */
                size_t str_len = registry_util_convert_value_to_str(&value, NULL, 0);

                /* convert the value to a string */
                char str[str_len + 1];
                registry_util_convert_value_to_str(&value, str, str_len + 1);

                /* print the string */
                printf("%s\n", str);
                return 0;
            }
        }

        printf("error: %d\n", res);
        return 1;
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (_parse_string_path(argv[2], &path) < 0) {
            printf("usage: %s %s <path> <value>\n", argv[0], argv[1]);
            return 1;
        }

        if (path.type != REGISTRY_INT_PATH_TYPE_GROUP_OR_PARAMETER) {
            return -EINVAL;
        }

        /* get instance and parameter of the path */
        int res = registry_node_from_int_path(&path, &node);

        if (node.type != REGISTRY_NODE_PARAMETER) {
            return -EINVAL;
        }

        if (res == 0) {
            /* get value from the registry, to know its correct type and size */
            res = registry_get(&node, &value);

            if (res == 0) {
                /* convert the string into the correct value type */
                uint8_t new_value_buf[value.buf_len];
                registry_util_convert_str_to_value(argv[3], new_value_buf, value.buf_len,
                                                   value.type);

                /* let the registry_value_t object point to the buffer of the new value */
                value.buf = new_value_buf;

                /* set the new value in the registry */
                registry_set(&node, value.buf, value.buf_len);
                return 0;
            }
        }

        printf("error: %d\n", res);
        return 1;
    }
    else if (strcmp(argv[1], "apply") == 0) {
        int res = 0;

        /* we need a count of 3 arguments to access index 2, otherwise no path was specified */
        if (argc >= 3) {
            if (_parse_string_path(argv[2], &path) < 0) {
                printf("usage: %s %s <path>\n", argv[0], argv[1]);
                return 1;
            }

            res = registry_node_from_int_path(&path, &node);

            if (res == 0) {
                res = registry_apply(&node);
            }
        }
        else {
            registry_apply(NULL);
        }

        if (res != 0) {
            printf("error: %d\n", res);
            return 1;
        }

        return 0;
    }
    else if (strcmp(argv[1], "export") == 0) {
        int res = 0;
        bool has_path = false;
        int tree_traversal_depth = 0;

        /* we need a count of 3 arguments to access index 2, otherwise no path was specified */
        if (argc >= 3) {
            /* first check if -d is provided, in which case there is definitely no path */
            if (strcmp(argv[2], "-d") == 0) {
                tree_traversal_depth = atoi(argv[3]);
            }
            /* convert the given path to an int path if it exists */
            else if (_parse_string_path(argv[2], &path) == 0) {
                res = registry_node_from_int_path(&path, &node);
                has_path = true;
                if (argc > 4 && strcmp(argv[3], "-d") == 0) {
                    tree_traversal_depth = atoi(argv[4]);
                }
            }
            else {
                printf("usage: %s %s <path> [-d <tree traversal depth>]\n", argv[0], argv[1]);
                return 1;
            }

            if (res == 0) {
                if (!has_path) {
                    res = registry_export(NULL, _export_cb, tree_traversal_depth, NULL);
                }
                else {
                    if (res == 0) {
                        res = registry_export(&node, _export_cb, tree_traversal_depth, NULL);
                    }
                }
            }
        }
        else {
            res = registry_export(NULL, _export_cb, 0, NULL);
        }

        if (res != 0) {
            printf("error: %d\n", res);
            return 1;
        }

        return 0;
    }
#if IS_USED(MODULE_REGISTRY_STORAGE) || IS_ACTIVE(DOXYGEN)
    else if (strcmp(argv[1], "load") == 0) {
        // TODO implement storage selector
        const registry_storage_instance_t **storage_instances;
        registry_storage_get_instances(&storage_instances);

        if (argc > 2) {
            printf("usage: %s %s\n", argv[0], argv[1]);
            return 1;
        }

        registry_storage_load(storage_instances[0]);

        return 0;
    }
    else if (strcmp(argv[1], "save") == 0) {
        int res = 0;
        // TODO implement storage selector
        const registry_storage_instance_t **storage_instances;
        registry_storage_get_instances(&storage_instances);

        if (argc > 2) {
            if (_parse_string_path(argv[2], &path) < 0) {
                printf("usage: %s %s [path]\n", argv[0], argv[1]);
                return 1;
            }

            res = registry_node_from_int_path(&path, &node);
            if (res == 0) {
                res = registry_storage_save(storage_instances[0], &node);
            }
        }
        else {
            // save everything
            res = registry_storage_save(storage_instances[0], NULL);
        }

        if (res != 0) {
            printf("error: %d\n", res);
            return 1;
        }

        return 0;
    }
#endif

help_error:
    printf("usage: %s {get|set|apply|export|load|save}\n", argv[0]);

    return 1;
}

SHELL_COMMAND(registry, "Registry cli", _registry);
