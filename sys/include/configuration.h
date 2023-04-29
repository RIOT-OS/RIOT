/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_configuration Runtime configuration module
 * @ingroup     sys
 * @brief       Provides and interface for runtime configuration of modules
 *              which must keep persistent configuration parameters
 *
 * Configuration values are accessed in a key - value manner, where a key is
 * basically a path to the correct configuration handler subtree.
 * Each node in the configuration tree carries a segment of the path key.
 * The root node in a subtree of handlers handles the whole subtree, while
 * each node below handles an attribute of the configuration item.
 * The handler operations must be implemented
 * by a configuration subsystem. The set() and get() implementations are
 * mandatory for a handler.
 *
 * A configuration subsystem must have been initialized with a storage backend
 * where configuration data is stored and can be exported to.
 * Subhandlers can also be initialized with a different backend to store specific attributes
 * of one configuration object on another backend.
 * A backend must at least implement the load(), and store() functions.
 * An array is not stored as a whole but each item is exported with its own key because
 * the array size can change between applications.
 *
 * The configuration API is thread save if the configuration subtree has been locked.
 * You would usually lock the subtree, perform some set() or get() operations and check
 * for consistency using verify(). After that you would maybe export your configuration
 * to persistent storage and/or apply the configuration using apply().
 * Be sure to unlock the subtree when you are done.
 *
 * You don´t want to export or apply bad configuration values, so you should call
 * verify() before. You can specify that you want to reimport a configuration value
 * on failing verification. Besides that you can do a get() before you set() a value
 * and restore it when verification fails.
 *
 * A key consists of a static and a dynamic part. The static part identifies the handler
 * node. The dynamic part is often called "next" in the API and the handler is supposed
 * to handle it. The dynamic part is, what is left of the key path after the handler
 * has been found. An implementation can choose to not handle any dynamic part, by
 * having one dedicated node for every subitem in a configuration object, or
 * to handle subitems with the top level handler and the dynamic key part.
 *
 * @{
 *
 * @file
 * @brief       Runtime configuration API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa-com>
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stddef.h>

#include "list.h"
#include "mutex.h"
#include "auto_init_utils.h"
#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_AUTO_INIT_CONFIGURATION) || defined(DOXYGEN)
/**
 * @brief   Add a configuration module to the auto-initialization array
 *
 * @param   function    Function to be called on initialization @ref auto_init_fn_t
 * @param   priority    Priority level @ref auto_init_prio_t
 */
#define AUTO_INIT_CONFIGURATION(function, priority)     _AUTO_INIT(auto_init_configuration_xfa, function, priority)
#else
#define AUTO_INIT_CONFIGURATION(...)
#endif

#if !defined(CONFIG_CONFIGURATION_DEPTH_MAX) || defined(DOXYGEN)
/**
 * @brief   A path to a configuration item must not have more than this number
 *          of segments
 */
#define CONFIG_CONFIGURATION_DEPTH_MAX      8
#endif

/**
 * @brief   @ref CONFIG_CONFIGURATION_DEPTH_MAX
 */
#define CONFIGURATION_DEPTH_MAX             CONFIG_CONFIGURATION_DEPTH_MAX

/**
 * @brief   The configuration root path for this RIOT release version
 *
 * The prepended root does not count for @ref CONFIGURATION_DEPTH_MAX
 */
#define CONFIGURATION_RIOT_ROOT             "/RIOT/2307/c"

/**
 * @brief   Length of RIOT configuration root path
 */
#define CONFIGURATION_RIOT_ROOT_LEN         (ARRAY_SIZE((char[]){CONFIGURATION_RIOT_ROOT}) - 1)

/**
 * @brief   Key buffer type with a static maximum key length
 *
 * @param   len             Buffer length to store a key
 */
#define CONF_KEY_T(len)                                 \
struct {                                                \
    char *next;                                         \
    int offset;                                         \
    unsigned char buf_len;                              \
    char buf[len];                                      \
}

/**
 * @brief   Key buffer initializer with a static maximum key length
 *
 * @param   len             Buffer length to store a key
 * @param   cstring         Constant string value to initialize the key buffer with
 */
#define CONF_KEY_INITIALIZER(len, cstring)              \
{                                                       \
    "",                                                 \
    0,                                                  \
    len,                                                \
    cstring,                                            \
}

/**
 * @brief   Abstraction type of a configuration key buffer
 */
typedef CONF_KEY_T() conf_key_buf_t;

/**
 * @brief   Configuration key type
 */
typedef void conf_key_t;

/**
 * @brief   Forward declaration of a configuration handler
 */
struct conf_handler;

/**
 * @brief   Forward declaration for a persistent configuration storage backend
 */
struct conf_backend;

/**
 * @brief   Handler prototype to set a configuration value
 *
 * This is called by @ref configuration_set() for the handler.
 * When @p val and @p size are NULL the configuration handler should delete the
 * internal configuration value or mark it as deleted but not perform the
 * backend delete operation.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in]           val         Value to apply to the configuration item identified by the key
 * @param[in, out]      size        Size in bytes of the value to set as input,
 *                                  and remaining size as output
 *
 * @return  0 on success
 */
typedef int (*conf_data_set_handler) (const struct conf_handler *handler,
                                      conf_key_buf_t *key, const void *val,
                                      size_t *size);

/**
 * @brief   Handler prototype to get a configuration value
 *
 * This is called by @ref configuration_get() for the handler.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in]           val         Destination to write the configuration item to
 * @param[in, out]      size        Maximum size in bytes of the value to get as input,
 *                                  and remaining length as output
 *
 * @return  0 on success
 */
typedef int (*conf_data_get_handler) (const struct conf_handler *handler,
                                      conf_key_buf_t *key, void *val,
                                      size_t *size);

/**
 * @brief   Handler prototype to import a configuration value from persistent storage to
 *          the internal representation location of the configuration item
 *
 * This is called by @ref configuration_import() for the handler.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  0 on success
 */
typedef int (*conf_data_import_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Handler prototype to export a configuration value from the internal
 *          representation location of the configuration item to persistent storage
 *
 * This is called by @ref configuration_export() for the handler.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  0 on success
 */
typedef int (*conf_data_export_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Handler prototype to delete a configuration value from persistent storage
 *
 * This is called by @ref configuration_delete() for the handler.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  0 on success
 */
typedef int (*conf_data_delete_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Handler prototype to verify the internal representation of a configuration item
 *
 * This is called by @ref configuration_verify() for the handler.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  0 on success
 */
typedef int (*conf_data_verify_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Handler prototype to apply the internal representation of a configuration item to
 *          the configuration subject, e.g. a sensor
 *
 * This is called by @ref configuration_apply() for the handler.
 * @ref conf_handler_ops_t::set only stores the configuration value to the internal location, and
 * @ref conf_data_apply_handler::apply applies the value to the subject.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  0 on success
 */
typedef int (*conf_data_apply_handler) (const struct conf_handler *handler,
                                        conf_key_buf_t *key);


/**
 * @brief   Handler prototype to encode the internal representation of a configuration item
 *          from a struct representation to for example CBOR.
 *
 * This is called by the default export handler before exporting.
 * If needed, an internal buffer must be known to the implementation of this function.
 * Another use case of this function could be to disguise confidential data which are not
 * supposed to be exported to a certain backend.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in, out]      enc_data    in: pointer to data to be encoded; out: pointer to encoded data to be exported
 * @param[out]          enc_size    in: size of data to be encoded; out: size of encoded data
 *
 * @return  0 on success
 */
typedef int (*conf_data_encode_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key,
                                         const void **enc_data, size_t *enc_size);
/**
 * @brief   Handler prototype to decode the internal representation of a configuration item
 *          for example from CBOR to a struct representation.
 *
 * This is called by the default import handler after importing.
 * If needed, an internal buffer must be known to the implementation of this function.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in, out]      dec_data    When this is a pointer to NULL, this will return the decoding buffer
                                    and the size of it in @p dec_size
                                    in: pointer to data to be decoded; out: pointer to decoded data
 * @param[in, out]      dec_size    Size of decoding buffer or in: size of data to be decoded;
                                    out: size of decoded data
 *
 * @return  0 on success
 */
typedef int (*conf_data_decode_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key,
                                         void **dec_data, size_t *dec_size);

/**
 * @brief   Configuration handler operations
 */
typedef struct conf_handler_ops {
    conf_data_set_handler set;              /**< Set a configuration value */
    conf_data_get_handler get;              /**< Get a configuration value */
    conf_data_import_handler import;        /**< Import a configuration value
                                                 from persistent storage */
    conf_data_export_handler export;        /**< Export a configuration value
                                                 to persistent storage */
    conf_data_delete_handler delete;        /**< Delete a configuration value
                                                 from persistent storage */
} conf_handler_ops_t;

typedef struct conf_handler_data_ops {
    conf_data_verify_handler verify;        /**< Verify the currently set configuration */
    conf_data_apply_handler apply;          /**< Apply the currently set configuration */
    conf_data_encode_handler encode;        /**< Encode the currently set configuration */
    conf_data_decode_handler decode;        /**< Decode the currently set configuration */
} conf_handler_data_ops_t;

/**
 * @brief   An intermediate node in the configuration tree
 */
typedef struct conf_handler_node {
    list_node_t node;                       /**< Every node is in a list, managed by its parent */
    struct conf_handler_node *subnodes;     /**< Every node has a list of subnodes */
    const char *subtree;                    /**< A segment in a key to a configuration item */
    const conf_handler_ops_t *ops;          /**< Handler operations */
    const conf_handler_data_ops_t *ops_dat; /**< Handler data operations */
    unsigned level;                         /**< Level in the configuration tree (root = 0) */
} conf_handler_node_t;

/**
 * @brief   Static initializer for an intermediate handler node
 *
 * @param   path        Configuration path segment, e.g. "bar" in "foo/bar/bazz"
 */
#define CONF_HANDLER_NODE_INITIALIZER(path)             \
{                                                       \
    .subtree = path,                                    \
}

/**
 * @brief   Configuration of handler behavior
 */
typedef struct {
    uint32_t handles_array      :1;     /**< True if the handler handles an array of items */
    uint32_t export_as_a_whole  :1;     /**< If the handler handles an array, this specifies whether the array
                                             should be exported a whole or item by item with an index in the key */
} conf_handler_flags_t;

/**
 * @brief   A node with handler operations in the configuration tree
 */
typedef struct conf_handler {
    conf_handler_node_t node;               /**< Configuration tree node */
    mutex_t mutex;                          /**< Lock for unique access to the configuration item */
    const struct conf_backend *src_backend; /**< Backend to store the configuration item */
    const struct conf_backend *dst_backend; /**< Backend to store the configuration item TODO */
    void *data;                             /**< Pointer to the configuration item data location */
    uint32_t size;                          /**< Configuration item size in bytes */
    conf_handler_flags_t conf_flags;        /**< Configuration of handler behavior */
} conf_handler_t;

/**
 * @brief   A node with handler operations, which handle an array of configuration objects
 *          in the configuration tree
 */
typedef struct conf_array_handler {
    conf_handler_t handler;             /**< Configuration handler */
    uint32_t array_size;                /**< Number of items in the array */
} conf_array_handler_t;

/**
 * @brief   Static initializer for a configuration handler node
 *
 * @param   path        Configuration path segment
 * @param   operations  Pointer to handler operations
 * @param   item_size   Size of the configuration object
 * @param   location    Location of the configuration data
 */
#define CONF_HANDLER_INITIALIZER(path, operations, data_operations, item_size, location)    \
{                                                                                           \
    .node = {                                                                               \
        .subtree = path,                                                                    \
        .ops = operations,                                                                  \
        .ops_dat = data_operations,                                                         \
    },                                                                                      \
    .data = location,                                                                       \
    .size = item_size,                                                                      \
    .mutex = MUTEX_INIT,                                                                    \
}

/**
 * @brief   Static initializer for a configuration handler node
 *
 * @param   path        Configuration path segment
 * @param   operations  Pointer to handler operations
 * @param   size        Size of a single item in the array
 * @param   location    Location of the configuration data
 * @param   numof       Number of items in the array
 * @param   is_a_whole  If true, the array is exported as a whole,
 *                      otherwise each item is exported with an index in the kex
 */
#define CONF_ARRAY_HANDLER_INITIALIZER(path, operations, data_operations, item_size, location,          \
                                       numof, is_a_whole)                                               \
{                                                                                                       \
    .handler = {                                                                                        \
        .node = {                                                                                       \
            .subtree = path,                                                                            \
            .ops = operations,                                                                          \
            .ops_dat = data_operations,                                                                 \
        },                                                                                              \
        .data = location,                                                                               \
        .size = item_size,                                                                              \
        .mutex = MUTEX_INIT,                                                                            \
        .conf_flags = {                                                                                 \
            .handles_array = 1u,                                                                        \
            .export_as_a_whole = (uint32_t)!!(is_a_whole),                                              \
        },                                                                                              \
    },                                                                                                  \
    .array_size = numof,                                                                                \
}

/**
 * @brief   Handler prototype to load configuration data from a persistent storage backend
 *
 * This is called by the configuration handler on import.
 *
 * @param[in]           be          Reference to the backend
 * @param[in]           key         Configuration key which belongs to the configuration item
 * @param[out]          val         Import location of the configuration value
 * @param[in, out]      size        Maximum size of the value to be imported as input,
 *                                  and remaining size as output
 *
 * @return  0 on success
 */
typedef int (*conf_backend_load_handler) (const struct conf_backend *be,
                                          conf_key_buf_t *key, void *val, size_t *size);

/**
 * @brief   Handler prototype to store configuration data to a persistent storage backend
 *
 * This is called by the configuration handler on export.
 *
 * @param[in]           be          Reference to the backend
 * @param[in]           key         Configuration key which belongs to the configuration item
 * @param[in]           val         Export location of the configuration value
 * @param[in, out]      size        Size of the value to be exported as input,
 *                                  and 0 as output
 *
 * @return  0 on success
 */
typedef int (*conf_backend_store_handler) (const struct conf_backend *be,
                                           conf_key_buf_t *key, const void *val, size_t *size);

/**
 * @brief   Handler prototype to delete configuration data from a persistent storage backend
 *
 * This is called by the configuration handler on delete.
 *
 * @param[in]           be          Reference to the backend
 * @param[in]           key         Configuration key which belongs to the configuration item
 *
 * @return  0 on success
 */
typedef int (*conf_backend_delete_handler) (const struct conf_backend *be,
                                            conf_key_buf_t *key);

/**
 * @brief   Configuration storage backend operations
 */
typedef struct conf_backend_ops {
    conf_backend_load_handler be_load;          /**< Backend import function */
    conf_backend_store_handler be_store;        /**< Backend export function */
    conf_backend_delete_handler be_delete;      /**< Backend delete function */
} conf_backend_ops_t;

/**
 * @brief   Configuration backend
 */
typedef struct conf_backend {
    const conf_backend_ops_t *ops;              /**< Backend operations */
} conf_backend_t;

/**
 * @brief   Get the root node of the configuration tree
 *
 * @return  Configuration root node
 */
conf_handler_node_t *configuration_get_root(void);

/**
 * @brief   Append a configuration node to the configuration tree
 *
 * @param[in]           parent          Parent configuration node
 * @param[in]           node            New configuration node which is
 *                                      either an intermediate or a leaf node
 */
void configuration_register(conf_handler_node_t *parent, conf_handler_node_t *node);

/**
 * @brief   Import a configuration value by its key from the persistent storage backend
 *
 * @param[in,out]       key             Identifying configuration key
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 * @return Negative errno on handler error
 */
int configuration_import(conf_key_t *key);

/**
 * @brief   Export a configuration value by its key to the persistent storage backend
 *
 * This calls verify() internally and skips the keys for which verification fails.
 *
 * @param[in,out]       key             Identifying configuration key
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 * @return Negative errno on handler error
 */
int configuration_export(conf_key_t *key);

/**
 * @brief   Delete a configuration value by its key from the persistent storage backend
 *
 * @param[in,out]       key             Identifying configuration key
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 */
int configuration_delete(conf_key_t *key);

/**
 * @brief   Set the value of a configuration item identified by key
 *
 * @param[in,out]       key             Identifying configuration key
 * @param[in]           value           New configuration value
 * @param[in]           size            Size of the configuration value
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 * @return Negative errno on handler error
 */
int configuration_set(conf_key_t *key, const void *value, size_t *size);

/**
 * @brief   Get the value of a configuration item identified by key
 *
 * @param[in,out]       key             Identifying configuration key
 * @param[out]          value           Configuration value
 * @param[in, out]      size            Maximum size of the configuration value
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 * @return Negative errno on handler error
 */
int configuration_get(conf_key_t *key, void *value, size_t *size);

/**
 * @brief   Lock a subtree of the configuration tree for unique modification permission
 *
 * @warning Do not lock overlapping subtrees!
 *
 * @param[in,out]       key             Key to identify the subtree
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 */
int configuration_lock(conf_key_t *key);

/**
 * @brief   Unlock a subtree of the configuration tree after modification
 *
 * @param[in,out]       key             Key to identify the subtree
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 */
int configuration_unlock(conf_key_t *key);

/**
 * @brief   Verify the correctness of a configuration subtree
 *
 * @param[in,out]       key             Key to identify the subtree
 * @param[in]           try_reimport    If true, try to reimport the configuration
 *                                      value if verification fails when a bad value was set
 * @return  0 on success
 * @return -ENOENT no handler found by key
 * @return Negative errno on handler error
 */
int configuration_verify(conf_key_t *key, bool try_reimport);

/**
 * @brief   Apply the configuration subtree
 *
 * @param[in,out]       key             Key to identify the subtree
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 */
int configuration_apply(conf_key_t *key);

/**
 * @brief   Set the backend to store the configuration item
 *
 * @param[in,out]       key             Key to identify the subtree
 * @param[in]           src_backend     Backend to load the configuration item from
 *                                      and to store the configuration item to
 * @param[in]           dst_backend     Optional backend to store the configuration item to
 *
 * @return  0 on success
 * @return -ENOENT no handler found by key
 */
int configuration_set_backend(conf_key_t *key,
                              const conf_backend_t *src_backend,
                              const conf_backend_t *dst_backend);

/**
 * @brief   Prepend a root path to a configuration key to distinguish between
 *          stored configurations for different firmware versions
 *
 * This must be called by a RIOT handler implementation to prepend
 * @ref CONFIGURATION_RIOT_ROOT to the configuration key in
 * @ref conf_handler_ops_t::import,
 * @ref conf_handler_ops_t::export,
 * @ref conf_handler_ops_t::delete.
 *
 * @param[in,out]       key         Key to identify the subtree
 * @param[in]           root        Root path to prepend before @p key
 *
 * @return  Position of original key value in buffer or NULL on error
 */
char *configuration_key_prepend_root(conf_key_buf_t *key, const char *root);

/**
 * @brief   This is called to restore the original input key which must be done by a handler
 *
 * @param[in,out]       key         Key to identify the subtree
 * @param[in]           restore     Key string to restore
 * @param[in]           restore_len Length of the key to restore
 */
void configuration_key_restore(conf_key_buf_t *key_buf, const char *key, size_t key_len);

/**
 * @brief   Default set-handler to be used for a simple configuration item
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in]           val         Value to apply to the configuration item identified by the key
 * @param[in, out]      size        Size in bytes of the value to set as input,
 *                                  and remaining size as output
 *
 * @return  @ref conf_data_set_handler
 */
int configuration_set_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, const void *val,
                                      size_t *size);

/**
 * @brief   Default get-handler to be used for a simple configuration item
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in]           val         Destination to write the configuration item to
 * @param[in, out]      size        Maximum size in bytes of the value to get as input,
 *                                  and remaining length as output
 *
 * @return  @ref conf_data_get_handler
 */
int configuration_get_handler_default(const conf_handler_t *handler,
                                      conf_key_buf_t *key, void *val,
                                      size_t *size);

/**
 * @brief   Default import-handler to be used for a simple configuration item
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  @ref conf_data_import_handler
 */
int configuration_import_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Default export-handler to be used for a simple configuration item
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  @ref conf_data_export_handler
 */
int configuration_export_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Default delete-handler to be used for a simple configuration item
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 *
 * @return  @ref conf_data_delete_handler
 */
int configuration_delete_handler_default(const conf_handler_t *handler,
                                         conf_key_buf_t *key);

/**
 * @brief   Default handler operations
 */
extern const conf_handler_ops_t configuration_default_ops;

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_H */
/** @} */
