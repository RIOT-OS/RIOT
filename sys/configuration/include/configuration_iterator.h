/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_configuration
 * @{
 *
 * @file
 * @brief       Configuration iterator types
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef CONFIGURATION_ITERATOR_H
#define CONFIGURATION_ITERATOR_H

#include <stdbool.h>
#include <stdint.h>

#include "configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Iterator item type to retrieve when iterating over all nodes in the configuration tree
 */
typedef struct conf_iterator_item {
    const conf_handler_t *node; /**< Configuration node to retrieve */
} conf_iterator_item_t;

/**
 * @brief   Iterator type to iterate over every node in the configuration tree non-recursively
 */
typedef struct configuration_iterator {
    const conf_handler_t *root;                              /**< Root node to start from */
    unsigned short sp;                                       /**< Stack pointer */
    conf_iterator_item_t stack[CONFIGURATION_DEPTH_MAX + 1]; /**< Stack of handlers */
} conf_iterator_t;

/**
 * @brief   Iterator item type to retrieve when iterating over all configuration path items
 *          in the configuration tree, including arrays
 */
typedef struct conf_path_iterator_item {
    const conf_handler_t *node; /**< Configuration node to retrieve */
    uint32_t index;             /**< Index when the node is an array */
} conf_path_iterator_item_t;

/**
 * @brief   Iterator type to iterate over all configuration SIDs non-recursively
 */
typedef struct configuration_path_iterator {
    const conf_handler_t *root;                                   /**< Root node to start from */
    unsigned short sp;                                            /**< Stack pointer */
    conf_path_iterator_item_t stack[CONFIGURATION_DEPTH_MAX + 1]; /**< Stack of handlers */
} conf_path_iterator_t;

/**
 * @brief   Type to restore a key after an iterator has modified it
 */
typedef struct conf_iterator_restore {
    conf_sid_t sid; /**< Initial SID */
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    int key_len;    /**< Initial key length */
#endif
} conf_iterator_restore_t;

#ifdef __cplusplus
}
#endif
#endif /* CONFIGURATION_ITERATOR_H */
/* @} */
