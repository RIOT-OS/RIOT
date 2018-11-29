/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry RIOT Registry
 * @ingroup     sys
 * @brief       RIOT Registry module for handling runtime configurations
 *
 * ## About
 *
 * The RIOT Registry is a module for interacting with persistent key-value
 * configurations. It forms part of the Runtime Configuration System for RIOT
 * nodes.
 *
 * ## Architecture
 *
 * The Registry interacts with other RIOT modules via
 * @ref registry_handler_t "Registry Handlers", and with non-volatile devices
 * via @ref sys_registry_store "Storage Facilities". This way the functionality
 * of the RIOT Registry is independent of the functionality of the module and
 * storage devices.
 *
 * ![RIOT Registry architecture](riot-registry-architecture.svg)
 *
 * ### Registry Handlers
 *
 * @ref registry_handler_t "Registry Handlers" (RH) represent Configuration
 * Groups in the RIOT Registry. A RIOT module is required to implement and
 * register a RH in order to expose its configurations in the Registry.
 *
 * A RH is defined by a @ref registry_handler_t::name "name" and a series of
 * handlers for interacting with the configuration parametes of the
 * configuration group. The handlers are:
 *
 * - @ref registry_handler_t::hndlr_get "get"
 * - @ref registry_handler_t::hndlr_set "set"
 * - @ref registry_handler_t::hndlr_commit "commit"
 * - @ref registry_handler_t::hndlr_export "export"
 *
 * It is responsibility of the module to implement these handlers and perform
 * all necessary checks before applying values to the configuration parameters.
 *
 * ### Storage Facilities
 *
 * @ref sys_registry_store "Storage Facilities" (SF) implement the
 * @ref registry_store_itf_t "storage interface" to allow the RIOT Registry to
 * load, search and store configuration parameters.
 *
 * ## RIOT Registry usage flow
 * - 1 The RIOT registry is initialized with @ref registry_init().
 * - 2 Modules declare and register RHs for configuration groups by calling
 *     @ref registry_register().
 * - 3 SFs are registered as sources and/or destinations of configurations by
 *     calling registry_<storage-name>_src() and registry_<storage-name>_dst().
 *
 * @{
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#ifndef REGISTRY_REGISTRY_H
#define REGISTRY_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "clist.h"

/**
 * @brief Separator character to define hierarchy in configurations names.
 */
#define REGISTRY_NAME_SEPARATOR    '/'

/**
 * @brief Maximum amount of levels of hierarchy in configurations names.
 */
#define REGISTRY_MAX_DIR_DEPTH     8

/**
 * @brief Maximum amount of characters per level in configurations names.
 */
#define REGISTRY_MAX_DIR_NAME_LEN  64

/**
 * @brief Maximum length of a value when converted to string
 */
#define REGISTRY_MAX_VAL_LEN       64

/**
 * @brief Maximum length of a configuration name.
 * @{
 */
#define REGISTRY_MAX_NAME_LEN      ((REGISTRY_MAX_DIR_NAME_LEN * \
                                    REGISTRY_MAX_DIR_DEPTH) + \
                                    (REGISTRY_MAX_DIR_DEPTH - 1))
/** @} */

/**
 * @brief Data types of the registry
 *
 * @note Float and int64 types must be enabled by defining
 * `CONFIG_REGISTRY_USE_FLOAT` and `CONFIG_REGISTRY_USE_INT64`. Use with caution
 * as they bloat the code size.
 */
typedef enum {
    REGISTRY_TYPE_NONE = 0,  /**< No type specified */
    REGISTRY_TYPE_INT8,      /**< 8-bits integer */
    REGISTRY_TYPE_INT16,     /**< 16-bits integer */
    REGISTRY_TYPE_INT32,     /**< 32-bits integer */
    REGISTRY_TYPE_STRING,    /**< String */
    REGISTRY_TYPE_BYTES,     /**< Binary data */
    REGISTRY_TYPE_BOOL,      /**< Boolean */

#if defined(CONFIG_REGISTRY_USE_INT64) || defined(DOXYGEN)
    REGISTRY_TYPE_INT64,     /**< 64-bits integer */
#endif /* CONFIG_REGISTRY_USE_INT64 */

#if defined(CONFIG_REGISTRY_USE_FLOAT) || defined(DOXYGEN)
    REGISTRY_TYPE_FLOAT,     /**< Float */
#endif /* CONFIG_REGISTRY_USE_FLOAT */
} registry_type_t;


/**
 * @brief Prototype of a callback function for the load action of a store
 * interface
 */
typedef void (*load_cb_t)(char *name, char *val, void *cb_arg);

/**
 * @brief Descriptor used to check duplications in store facilities
 */
typedef struct {
    char *name; /**< name of the parameter being checked */
    char *val;  /**< value of the parameter being checked */
    int is_dup; /**< flag indicating if the parameter is duplicated or not */
} registry_dup_check_arg_t;

/**
 * @brief Store facility descriptor
 */
typedef struct {
    clist_node_t node;                    /**< linked list node */
    const struct registry_store_itf *itf; /**< interface for the facility */
} registry_store_t;

/**
 * @brief Storage facility interface.
 * All store facilities should, at least, implement the load and save actions.
 */
typedef struct registry_store_itf {
    /**
     * @brief Loads all stored parameres and calls the @p cb callback function.
     *
     * @param[in] store Storage facility descriptor
     * @param[in] cb Callback function to call for every stored parameter
     * @param[in] cb_arg Argument passed to @p cb function
     * @return 0 on success, non-zero on failure
     */
    int (*load)(registry_store_t *store, load_cb_t cb, void *cb_arg);

    /**
     * @brief If implemented, it is used for any preparation the storage may
     * need before starting a saving process.
     *
     * @param[in] store Storage facility descriptor
     * @return 0 on success, non-zero on failure
     */
    int (*save_start)(registry_store_t *store);

    /**
     * @brief Saves a parameter into storage.
     *
     * @param[in] store Storage facility descriptor
     * @param[in] name String representing the parameter (key)
     * @param[in] value String representing the value of the parameter
     * @return 0 on success, non-zero on failure
     */
    int (*save)(registry_store_t *store, const char *name, const char *value);

    /**
     * @brief If implemented, it is used for any tear-down the storage may need
     * after a saving process.
     *
     * @param[in] store Storage facility descriptor
     * @return 0 on success, non-zero on failure
     */
    int (*save_end)(registry_store_t *store);
} registry_store_itf_t;

/**
 * @brief Handler for configuration groups. Each configuration group should
 * register a handler using the @ref registry_register() function.
 * A handler provides the pointer to get, set, commit and export configuration
 * parameters.
 */
typedef struct {
    clist_node_t node; /**< Linked list node */
    char *name; /**< String representing the configuration group */

    /**
     * @brief Handler to get the current value of a configuration parameter
     * from the configuration group. The handler should return the current value
     * of the configuration parameter as a string copied to val.
     *
     * @param[in] argc Number of elements in @p argv
     * @param[in] argv Parsed string representing the configuration parameter.
     * @param[out] val Buffer to return the current value
     * @param[in] val_len_max Size of @p val
     * @param[in] context Context of the handler
     * @return Pointer to the buffer containing the current value, NULL on
     * failure
     *
     * @note The strings passed in @p argv do not contain the string of the name
     * of the configuration group. E.g. If a parameter name is 'group/foo/var'
     * and the name of the group is 'group', argv will contain 'foo' and 'var'.
     */
    char *(*hndlr_get)(int argc, char **argv, char *val, int val_len_max,
                       void *context);

    /**
     * @brief Handler to set a the value of a configuration parameter.
     * The value will be passed as a string in @p val.
     *
     * @param[in] argc Number of elements in @p argv
     * @param[in] argv Parsed string representing the configuration parameter.
     * @param[out] val Buffer containing the string of the new value
     * @param[in] context Context of the handler
     * @return 0 on success, non-zero on failure
     *
     * @note The strings passed in @p argv do not contain the string of the name
     * of the configuration group. E.g. If a parameter name is 'group/foo/var'
     * and the name of the group is 'group', argv will contain 'foo' and 'var'.
     */
    int (*hndlr_set)(int argc, char **argv, char *val, void *context);

    /**
     * @brief Handler to apply (commit) the configuration parameters of the
     * group, called once all configurations are loaded from storage.
     * This is useful when a special logic is needed to apply the parameters
     * (e.g. when dependencies exist). This handler should also be called after
     * setting the value of a configuration parameter.
     *
     * @param[in] context Context of the handler
     * @return 0 on success, non-zero on failure
     */
    int (*hndlr_commit)(void *context);

    /**
     * @brief Handler to export the configuration parameters of the group.
     * The handler must call the @p export_func function with the name and the
     * string representing the value of the requested configuration parameter
     * defined by @p argv.
     *
     * If @p argc is 0 then the handler should call the @p export_func for every
     * configuration parameter of the group.
     *
     * @param[in] export_func Export function that should be called by the
     * handler.
     * @param[in] argc Number of elements in @p argv
     * @param[in] argv Parsed string representing the configuration parameter.
     * @param[in] context Context of the handler
     * @return 0 on success, non-zero on failure (if @p export_func fails,
     *         i.e. returns non-zero, this error code should be returned by the
     *         handler as it is considered a failure).
     *
     * @note The strings passed in @p argv do not contain the string of the name
     * of the configuration group. E.g. If a parameter name is 'group/foo/var'
     * and the name of the group is 'group', argv will contain 'foo' and 'var'.
     */
    int (*hndlr_export)(int (*export_func)(const char *name, char *val),
                        int argc, char **argv, void *context);

    void *context; /**< Optional context used by the handlers */
} registry_handler_t;

/**
 * @brief List of registered handlers
 */
extern clist_node_t registry_handlers;

/**
 * @brief Initializes the RIOT Registry and the store modules.
 */
void registry_init(void);

/**
 * @brief Initializes the store module.
 */
void registry_store_init(void);

/**
 * @brief Registers a new group of handlers for a configuration group.
 *
 * @param[in] handler Pointer to the handlers structure.
 */
void registry_register(registry_handler_t *handler);

/**
 * @brief Registers a new storage as a source of configurations. Multiple
 *        storages can be configured as sources at the same time. Configurations
 *        will be loaded from all of them. This is commonly called by the
 *        storage facilities who implement their own registry_<storage-name>_src
 *        function.
 *
 * @param[in] src Pointer to the storage to register as source.
 */
void registry_src_register(registry_store_t *src);

/**
 * @brief Registers a new storage as a destination for saving configurations.
 *        Only one storage can be registered as destination at a time. If a
 *        previous storage had been registered before it will be replaced by the
 *        new one. This is commonly called by the storage facilities who
 *        implement their own registry_<storage-name>_dst function.
 *
 * @param[in] dst Pointer to the storage to register
 */
void registry_dst_register(registry_store_t *dst);

/**
 * @brief Sets the value of a parameter that belongs to a configuration group.
 *
 * @param[in] name String of the name of the parameter to be set
 * @param[in] val_str New value for the parameter
 * @return -EINVAL if handlers could not be found, otherwise returns the
 *             value of the set handler function.
 */
int registry_set_value(char *name, char *val_str);

/**
 * @brief Gets the current value of a parameter that belongs to a configuration
 *        group, identified by @p name.
 *
 * @param[in] name String of the name of the parameter to get the value of
 * @param[out] buf Pointer to a buffer to store the current value
 * @param[in] buf_len Length of the buffer to store the current value
 * @return Pointer to the beginning of the buffer
 */
char *registry_get_value(char *name, char *buf, int buf_len);

/**
 * @brief If a @p name is passed it calls the commit handler for that
 *        configuration group. If no @p name is passed the commit handler is
 *        called for every registered configuration group.
 *
 * @param[in] name Name of the configuration group to commit the changes (can
 * be NULL).
 * @return 0 on success, -EINVAL if the group has not implemented the commit
 * function.
 */
int registry_commit(char *name);

/**
 * @brief Convenience function to parse a configuration parameter value from
 * a string. The type of the parameter must be known and must not be `bytes`.
 * To parse the string to bytes @ref registry_bytes_from_str() function must be
 * used.
 *
 * @param[in] val_str Pointer of the string containing the value
 * @param[in] type Type of the parameter to be parsed
 * @param[out] vp Pointer to store the parsed value
 * @param[in] maxlen Maximum length of the output buffer when the type of the
 * parameter is string.
 * @return 0 on success, non-zero on failure
 */
int registry_value_from_str(char *val_str, registry_type_t type, void *vp,
                            int maxlen);

/**
 * @brief Convenience function to parse a configuration parameter value of
 * `bytes` type from a string.
 *
 * @param[in] val_str Pointer of the string containing the value
 * @param[out] vp Pointer to store the parsed value
 * @param len Length of the output buffer
 * @return 0 on success, non-zero on failure
 */
int registry_bytes_from_str(char *val_str, void *vp, int *len);

/**
 * @brief Convenience function to transform a configuration parameter value into
 * a string, when the parameter is not of `bytes` type, in this case
 * @ref registry_str_from_bytes() should be used. This is used for example to
 * implement the `get` or `export` handlers.
 *
 * @param[in] type Type of the parameter to be converted
 * @param[in] vp Pointer to the value to be converted
 * @param[out] buf Buffer to store the output string
 * @param[in] buf_len Length of @p buf
 * @return Pointer to the output string
 */
char *registry_str_from_value(registry_type_t type, void *vp, char *buf,
                              int buf_len);

/**
 * @brief Convenience function to transform a configuration parameter value of
 * `bytes` type into a string. This is used for example to implement the `get`
 * or `export` handlers.
 *
 * @param[in] vp Pointer to the value to be converted
 * @param[in] vp_len Length of @p vp
 * @param[out] buf Buffer to store the output string
 * @param[in] buf_len Length of @p buf
 * @return Pointer to the output string
 */
char *registry_str_from_bytes(void *vp, int vp_len, char *buf, int buf_len);

/**
 * @brief Load all configuration parameters from the registered storage
 * facilities.
 *
 * @note This should be called after the storage facilities were registered.
 *
 * @return 0 on success, non-zero on failure
 */
int registry_load(void);

/**
 * @brief Save all configuration parameters of every configuration group to the
 * registered storage facility.
 *
 * @return 0 on success, non-zero on failure
 */
int registry_save(void);

/**
 * @brief Save an specific configuration paramter to the registered storage
 * facility, with the provided value (@p val).
 *
 * @param[in] name String representing the configuration parameter
 * @param[in] val String representing the value of the configuration parameter
 * @return 0 on success, non-zero on failure
 */
int registry_save_one(const char *name, char *val);

/**
 * @brief Export an specific or all configuration parameters using the
 * @p export_func function. If name is NULL then @p export_func is called for
 * every configuration parameter on each configuration group.
 *
 * @param[in] export_func Exporting function call with the name and current
 * value of an specific or all configuration parameters
 * @param[in] name String representing the configuration parameter. Can be NULL.
 * @return 0 on success, non-zero on failure
 */
int registry_export(int (*export_func)(const char *name, char *val),
                    char *name);

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_REGISTRY_H */
