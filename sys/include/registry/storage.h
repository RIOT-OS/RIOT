/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/**
 * @brief Prototype of a callback function for the load action of a storage interface.
 *
 * @param[in] node A location within the registry configuration tree. (In this case it must be a configuration parameter)
 * @param[in] buf The saved value of the configuration parameter.
 * @param[in] buf_len The size of @p buf.
 *
 * @return 0 on success, non-zero on failure.
 */
typedef registry_error_t (*load_cb_t)(const registry_node_t *node, const void *buf,
                                      const size_t buf_len);

/**
 * @brief Data structure of a storage interface see @p _registry_storage_t.
 */
typedef struct _registry_storage_t registry_storage_t;

/**
 * @brief The Storage instance references the @p registry_storage_t
 * and holds configuration data such as mount points.
 */
typedef struct {
    registry_storage_t *storage; /**< interface of the storage. */
    void *data;                  /**< Struct containing all config data for the storage (specific to storage implementation). */
} registry_storage_instance_t;

/**
 * @brief Storage interface. All storages should, at least, implement the load and save actions.
 */
struct _registry_storage_t {
    /**
     * @brief Loads all saved parameters and calls the @p load_cb callback function.
     *
     * @param[in] storage_instance Storage instance to load the configurations from.
     * @param[in] load_cb Callback function to call for every saved parameter.
     *
     * @return 0 on success, non-zero on failure.
     */
    registry_error_t (*load)(
        const registry_storage_instance_t *storage,
        const load_cb_t load_cb);

    /**
     * @brief If implemented, it is used for any preparation the storage may
     * need before starting a saving process.
     * NULL if not implemented
     *
     * @param[in] storage_instance Storage instance to save the configurations to.
     *
     * @return 0 on success, non-zero on failure.
     */
    registry_error_t (*save_start)(const registry_storage_instance_t *storage);

    /**
     * @brief Saves a parameter into storage.
     *
     * @param[in] storage_instance Storage instance to save the configurations to.
     * @param[in] node A location within the registry configuration tree.
     * @param[in] value Configuration parameter value.
     *
     * @return 0 on success, non-zero on failure.
     */
    registry_error_t (*save)(
        const registry_storage_instance_t *storage,
        const registry_node_t *node,
        const registry_value_t *value);

    /**
     * @brief If implemented, it is used for any tear-down the storage may need
     * after a saving process.
     * NULL if not implemented
     *
     * @param[in] storage_instance Storage instance to save the configurations to.
     *
     * @return 0 on success, non-zero on failure.
     */
    registry_error_t (*save_end)(const registry_storage_instance_t *storage);
};

/**
 * @brief Load all configuration parameters from the storages that are registered
 * using @p REGISTRY_ADD_STORAGE_SOURCE.
 *
 * @param[in] storage_instance Storage instance to load the configurations from.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_storage_load(
    const registry_storage_instance_t *storage_instance);

/**
 * @brief Save all configuration parameters that are within
 * the scope of the to the @p node. to the storage device, that was registered
 * using @p REGISTRY_SET_STORAGE_DESTINATION.
 *
 * @param[in] storage_instance Storage instance to save the configurations to.
 * @param[in] node A location within the registry configuration tree.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_storage_save(
    const registry_storage_instance_t *storage_instance,
    const registry_node_t *node);

/**
 * @brief Set storage instances to expose them to configuration managers such as
 * the RIOT CLI.
 *
 * @param[in] storage_instances An array of pointers to storage instances.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_storage_set_instances(
    const registry_storage_instance_t **storage_instances);

/**
 * @brief Get exposed storage instances to use them in a configuration manager
 * such as the RIOT CLI.
 *
 * @param[out] storage_instances An array of pointers to storage instances.
 *
 * @return 0 on success, non-zero on failure.
 */
registry_error_t registry_storage_get_instances(
    const registry_storage_instance_t ***storage_instances);

/* heap */
#if IS_USED(MODULE_REGISTRY_STORAGE_HEAP) || IS_ACTIVE(DOXYGEN)
extern registry_storage_t registry_storage_heap;
#endif

/* vfs */
#if IS_USED(MODULE_REGISTRY_STORAGE_VFS) || IS_ACTIVE(DOXYGEN)
extern registry_storage_t registry_storage_vfs;
#endif

#ifdef __cplusplus
}
#endif
/** @} */
