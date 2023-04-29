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
 * @brief       Provides an interface for runtime configuration of modules
 *              which must keep persistent configuration parameters
 *
 *              TODO: Write some documentation
 * @{
 *
 * @file
 * @brief       Runtime configuration API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa-com>
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "list.h"
#include "mutex.h"
#include "auto_init_utils.h"
#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_AUTO_INIT_CONFIGURATION) || defined(DOXYGEN)
/**
 * @brief   __attribute__((weak)) function to be overwritten by the application
 *          to set the backend of the configuration root.
 *
 *          This function sets the riotconf backend by default
 */
void auto_init_configuration_root_backend_init(void);
/**
 * @brief   Add a configuration module to the auto-initialization array
 *
 * @param   function    Function to be called on initialization @ref auto_init_fn_t
 * @param   priority    Priority level @ref auto_init_prio_t
 */
#define AUTO_INIT_CONFIGURATION(function, priority)                                                 \
    _AUTO_INIT(auto_init_configuration_xfa, function, priority)
#else
#define AUTO_INIT_CONFIGURATION(...)
#endif

#if !defined(CONFIG_CONFIGURATION_DEPTH_MAX) || defined(DOXYGEN)
/**
 * @brief   A path to a configuration item must not have more than this number of segments
 */
#define CONFIG_CONFIGURATION_DEPTH_MAX              8
#endif
#if !defined(CONFIG_CONFIGURATION_ENCODING_BUF_SIZE) || defined(DOXYGEN)
/**
 * @brief   The size of the encoding buffer for the default export handler
 */
#define CONFIG_CONFIGURATION_ENCODING_BUF_SIZE      512
#endif
#if !defined(CONFIG_CONFIGURATION_DECODING_BUF_SIZE) || defined(DOXYGEN)
/**
 * @brief   The size of the decoding buffer for the default import handler
 */
#define CONFIG_CONFIGURATION_DECODING_BUF_SIZE      512
#endif

/**
 * @brief   @ref CONFIG_CONFIGURATION_DEPTH_MAX
 */
#define CONFIGURATION_DEPTH_MAX             CONFIG_CONFIGURATION_DEPTH_MAX
/**
 * @brief   @ref CONFIG_CONFIGURATION_ENCODING_BUF_SIZE
 */
#define CONFIGURATION_ENCODING_BUF_SIZE     CONFIG_CONFIGURATION_ENCODING_BUF_SIZE
/**
 * @brief   @ref CONFIG_CONFIGURATION_DECODING_BUF_SIZE
 */
#define CONFIGURATION_DECODING_BUF_SIZE     CONFIG_CONFIGURATION_DECODING_BUF_SIZE

#if IS_USED(MODULE_CONFIGURATION_STRINGS) || defined(DOXYGEN)
/**
 * @brief   When configuration_strings is used as a module,
 *          a member to store a configuration path segment is added to a node
 * @internal
 */
#define _CONF_SUBTREE               const char *subtree;
/**
 * @brief   When module configuration_strings is used as a module,
 *          a member to store the configuration path is added to a key buffer
 * @internal
 */
#define _CONF_KEY_BUF(len)          char buf[len];
/**
 * @brief   When configuration_strings is used as a module,
 *          a member to store the length of the configuration key buffer is added
 * @internal
 */
#define _CONF_KEY_BUF_LEN           unsigned char buf_len;
/**
 * @brief   Key buffer initializer
 * @internal
 */
#define _CONF_KEY_INITIALIZER(_sid, _buf_len)                                                       \
{                                                                                                   \
    .sid = _sid,                                                                                    \
    .buf_len = _buf_len,                                                                            \
    .buf = "",                                                                                      \
}
/**
 * @brief   Internal initialization macro for a configuration handler node
 * @internal
 */
#define _CONF_HANDLER_NODE_ID_INITIALIZER(_sid_lower, _sid_upper, _subtree)                         \
{                                                                                                   \
    .subtree = _subtree,                                                                            \
    .sid_lower = _sid_lower,                                                                        \
    .sid_upper = _sid_upper,                                                                        \
}
/**
 * @brief   Internal initialization macro for a configuration handler
 * @internal
 */
#define _CONF_HANDLER_ID_INITIALIZER(_sid, _subtree)                                                \
{                                                                                                   \
    .subtree = _subtree,                                                                            \
    .sid_lower = _sid,                                                                              \
}
/**
 * @brief   Internal initialization macro for a configuration handler array
 * @internal
 */
#define _CONF_HANDLER_ARRAY_ID_INITIALIZER(_sid_lower, _sid_upper, _sid_stride, _subtree)           \
{                                                                                                   \
    .subtree = _subtree,                                                                            \
    .sid_lower = _sid_lower,                                                                        \
    .sid_upper = _sid_upper,                                                                        \
    .sid_stride = _sid_stride,                                                                      \
}
/**
 * @brief   Key buffer initializer with a static maximum key length
 *
 * @param   name        Name of the key buffer variable
 * @param   sid         SID of the configuration item
 * @param   len         Buffer length to store a key
 */
#define CONF_KEY(name, sid, len)                                                                    \
    CONF_KEY_T(len) name = _CONF_KEY_INITIALIZER(sid, len)
/**
 * @brief   Macro to instantiate a configuration handler node identification
 *
 * @param   name        Name of the configuration handler node identifier variable
 * @param   sid_lower   Lower SID of the configuration handler node
 * @param   sid_upper   Upper SID of the configuration handler node (inclusive)
 * @param   subtree     Subtree string
 */
#define CONF_HANDLER_NODE_ID(name, sid_lower, sid_upper, subtree)                                   \
    const conf_handler_node_id_t name =                                                             \
    _CONF_HANDLER_NODE_ID_INITIALIZER(sid_lower, sid_upper, subtree)
/**
 * @brief   Macro to instantiate a configuration handler identification
 *
 * @param   name        Name of the configuration handler variable
 * @param   sid         SID of the configuration handler
 * @param   subtree     Subtree string
 */
#define CONF_HANDLER_ID(name, sid, subtree)                                                         \
    const conf_handler_id_t name = _CONF_HANDLER_ID_INITIALIZER(sid, subtree)
/**
 * @brief   Macro to instantiate a configuration handler array identification
 *
 * @param   name        Name of the configuration array handler identifier variable
 * @param   sid_lower   Lower SID of the configuration handler array
 * @param   sid_upper   Upper SID of the configuration handler array (inclusive)
 * @param   sid_stride  SID Stride from one to the next array entry
 * @param   subtree     Subtree string
 */
#define CONF_HANDLER_ARRAY_ID(name, sid_lower, sid_upper, sid_stride, subtree)                      \
    const conf_handler_array_id_t name =                                                            \
    _CONF_HANDLER_ARRAY_ID_INITIALIZER(sid_lower, sid_upper, sid_stride, subtree)
#else
#define _CONF_SUBTREE
#define _CONF_KEY_BUF(len)
#define _CONF_KEY_BUF_LEN
#define _CONF_KEY_INITIALIZER(_sid, ...)                                                            \
{                                                                                                   \
    .sid = _sid,                                                                                    \
}
#define _CONF_HANDLER_NODE_ID_INITIALIZER(_sid_lower, _sid_upper)                                   \
{                                                                                                   \
    .sid_lower = _sid_lower,                                                                        \
    .sid_upper = _sid_upper,                                                                        \
}
#define _CONF_HANDLER_ID_INITIALIZER(_sid)                                                          \
{                                                                                                   \
    .sid_lower = _sid,                                                                              \
}
#define _CONF_HANDLER_ARRAY_ID_INITIALIZER(_sid_lower, _sid_upper, _sid_stride)                     \
{                                                                                                   \
    .sid_lower = _sid_lower,                                                                        \
    .sid_upper = _sid_upper,                                                                        \
    .sid_stride = _sid_stride,                                                                      \
}
#define CONF_KEY(name ,sid, ...)                                                                    \
    CONF_KEY_T(0) name = _CONF_KEY_INITIALIZER(sid)
#define CONF_HANDLER_NODE_ID(name, sid_lower, sid_upper, ...)                                       \
    const conf_handler_node_id_t name =                                                             \
    _CONF_HANDLER_NODE_ID_INITIALIZER(sid_lower, sid_upper)
#define CONF_HANDLER_ID(name, sid, ...)                                                             \
    const conf_handler_id_t name = _CONF_HANDLER_ID_INITIALIZER(sid)
#define CONF_HANDLER_ARRAY_ID(name, sid_lower, sid_upper, sid_stride, ...)                          \
    const conf_handler_array_id_t name =                                                            \
    _CONF_HANDLER_ARRAY_ID_INITIALIZER(sid_lower, sid_upper, sid_stride)
#endif

#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS) || defined(DOXYGEN)
/**
 * @brief   Static initializer for an intermediate handler node
 * @internal
 */
#define _CONF_HANDLER_INITIALIZER(_id, _data_ops, _be_ops, _be_data_ops, _flags, _item_size, _loc)  \
{                                                                                                   \
    .node_id = _id,                                                                                 \
    .ops_dat = _data_ops,                                                                           \
    .ops_be = _be_ops,                                                                              \
    .ops_be_dat = _be_data_ops,                                                                     \
    .mutex = MUTEX_INIT,                                                                            \
    .conf_flags = _flags,                                                                           \
    .data = _loc,                                                                                   \
    .size = _item_size,                                                                             \
}
#else
#define _CONF_HANDLER_INITIALIZER(_id, _data_ops, _be_ops, _be_data_ops, _flags, _item_size, _loc)  \
{                                                                                                   \
    .node_id = _id,                                                                                 \
    .ops_dat = _data_ops,                                                                           \
    .mutex = MUTEX_INIT,                                                                            \
    .conf_flags = _flags,                                                                           \
    .data = _loc,                                                                                   \
    .size = _item_size,                                                                             \
}
#endif
/**
 * @brief   Macro to instantiate a configuration handler node for a compound configuration item
 *
 * @param   name                Name of the configuration handler variable
 * @param   id                  Pointer to configuration handler ID @ref CONF_HANDLER_NODE_ID
 * @param   data_ops            Pointer to handler data operations (verify, apply)
 * @param   be_ops              Pointer to handler backend operations (import, export, delete)
 * @param   be_data_ops         Pointer to handler backend data operations (encode, decode)
 * @param   item_size           Size of the configuration item
 * @param   loc                 Location of the configuration data
 */
#define CONF_HANDLER(name, id, data_ops, be_ops, be_data_ops, item_size, loc)                       \
    conf_handler_t name =                                                                           \
    _CONF_HANDLER_INITIALIZER((const conf_handler_node_id_t *)id, data_ops, be_ops, be_data_ops,    \
                              { 0 }, item_size, loc)

/**
 * @brief   Macro to instantiate a configuration handler for a primitive type
 *
 * @param   name                Name of the configuration handler variable
 * @param   id                  Pointer to configuration handler ID @ref CONF_HANDLER_ID
 * @param   data_ops            Pointer to handler data operations (verify, apply)
 * @param   be_ops              Pointer to handler backend operations (import, export, delete)
 * @param   be_data_ops         Pointer to handler backend data operations (encode, decode)
 * @param   flags               Configuration flags @ref conf_handler_flags_t
 * @param   item_size           Size of the configuration item
 * @param   loc                 Location of the configuration data
 */
#define CONF_PRIMITIVE_HANDLER(name, id, data_ops, be_ops, be_data_ops, flags, item_size, loc)      \
    conf_handler_t name =                                                                           \
    _CONF_HANDLER_INITIALIZER((const conf_handler_node_id_t *)id, data_ops, be_ops, be_data_ops,    \
                              _CONF_FLAGS(flags), item_size, loc)
/**
 * @brief   Static initializer for a configuration handler node
 * @internal
 */
#define _CONF_ARRAY_HANDLER_INITIALIZER(_id, _data_ops, _be_ops, _be_data_ops, _item_size, _loc, _numof)    \
{                                                                                                           \
    .handler =  _CONF_HANDLER_INITIALIZER((const conf_handler_node_id_t *)_id, _data_ops, _be_ops,          \
                                          _be_data_ops, CONF_FLAGS_ARRAY(), _item_size, _loc),              \
    .array_size = _numof,                                                                                   \
}
/**
 * @brief   Macro to instantiate a configuration array handler
 *
 * @param   name                Name of the configuration array handler variable
 * @param   id                  Pointer to configuration array handler ID @ref CONF_HANDLER_ARRAY_ID
 * @param   data_ops            Pointer to handler data operations
 * @param   be_ops              Pointer to handler backend operations
 * @param   be_data_ops         Pointer to handler backend data operations
 * @param   item_size           Size of a single item in the array
 * @param   loc                 Location of the configuration data
 * @param   numof               Number of items in the array
 */
#define CONF_ARRAY_HANDLER(name, id, data_ops, be_ops, be_data_ops, item_size, loc, numof)          \
    conf_array_handler_t name =                                                                     \
    _CONF_ARRAY_HANDLER_INITIALIZER(id, data_ops, be_ops, be_data_ops, item_size, loc, numof)

/**
 * @brief   Macro to instantiate a configuration backend for a handler
 *
 * @param   name                Name of the configuration backend variable
 * @param   ops                 Pointer to backend operations
 * @param   fmt                 Encoding format
 */
#define CONF_BACKEND(name, ops, fmt)                                                                \
    conf_backend_t name = _CONF_BACKEND_INITIALIZER(ops, fmt)

/**
 * @brief   Configuration backend initializer
 * @internal
 */
#define _CONF_BACKEND_INITIALIZER(_ops, _fmt)                                                       \
{                                                                                                   \
    .ops = _ops,                                                                                    \
    .fmt = _fmt,                                                                                    \
}

/**
 * @brief   Unique identifier for a configuration item
 */
typedef uint64_t conf_sid_t;

/**
 * @brief   Configuration key base type
 */
#define _CONF_KEY_BASE_T                                \
struct {                                                \
    conf_sid_t sid;                                     \
    conf_sid_t sid_normal;                              \
    unsigned offset;                                    \
}

/**
 * @brief   Key buffer type with a static maximum key length
 *
 * @param   len             Buffer length to store a key
 */
#define CONF_KEY_T(len)                                 \
struct {                                                \
    _CONF_KEY_BASE_T;                                   \
    _CONF_KEY_BUF_LEN                                   \
    _CONF_KEY_BUF(len)                                  \
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
 * @brief   Configuration encoding format
 */
typedef enum {
    CONF_FMT_RAW,       /**< Unformatted */
    CONF_FMT_CBOR,      /**< CBOR */
} conf_fmt_t;

/**
 * @brief   Forward declaration of a configuration handler
 */
struct conf_handler;

/**
 * @brief   Forward declaration for a persistent configuration storage backend
 */
struct conf_backend;

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
 * @ref configuration_set() only stores the configuration value to the internal location, and
 * @ref configuration_apply() applies the value to the subject.
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
 * Another use case of this function could be to disguise confidential data which are not
 * supposed to be exported to a certain backend.
 *
 * @param[in]           handler     Reference to the handler
 * @param[in]           key         Configuration key which belongs to the configuration handler
 * @param[in]           sid_start   Start SID of the handler which is to be exported
 * @param[in]           fmt         Encoding format
 * @param[out]          enc_data    pointer to encoded data to be exported
 * @param[out]          enc_size    in: size of data to be encoded; out: size of encoded data
 *
 * @return  0 on success
 * @return  -ENOBUFS if the encoding failed due to a too small buffer and the buffer must be flushed
 * @return  Any other negative value is an unexpected error and aborts the encoding
 */
typedef int (*conf_data_encode_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key, const conf_sid_t *sid_start,
                                         conf_fmt_t fmt, void **enc_data, size_t *enc_size);
/**
 * @brief   Handler prototype to decode the internal representation of a configuration item
 *          for example from CBOR to a struct representation.
 *
 * This is called by the default import handler after importing.
 *
 * @param[in]           handler     Reference to the handler, NULL if next value must be skipped
 * @param[in]           key         Configuration key which belongs to the configuration handler,
                                    NULL if the next SID should be decoded
 * @param[in]           sid_start   Start SID of the handler which is to be imported
 * @param[in]           fmt         Encoding format
 * @param[in, out]      dec_data    Decoding buffer to read from
 * @param[in, out]      dec_size    Size left in buffer
 *
 * @return  0 on success
 * @return  -ENOBUFS if the decoding failed due to a too small buffer and the buffer must be reset
 * @return  Any other negative value is an unexpected error and aborts the decoding
 */
typedef int (*conf_data_decode_handler) (const struct conf_handler *handler,
                                         conf_key_buf_t *key, conf_sid_t *sid_start,
                                         conf_fmt_t fmt, const void **dec_data, size_t *dec_size);

/**
 * @brief   Configuration handler data operations
 */
typedef struct conf_handler_data_ops {
    conf_data_verify_handler verify;        /**< Verify the currently set configuration */
    conf_data_apply_handler apply;          /**< Apply the currently set configuration */
} conf_handler_data_ops_t;

/**
 * @brief   Configuration handler backend data operations
 */
typedef struct conf_handler_backend_data_ops {
    conf_data_encode_handler encode;        /**< Encode the currently set configuration */
    conf_data_decode_handler decode;        /**< Decode a given configuration */
} conf_handler_backend_data_ops_t;

/**
 * @brief   Configuration handler backend operations
 */
typedef struct conf_handler_backend_ops {
    conf_data_import_handler import;        /**< Import a configuration value
                                                 from persistent storage */
    conf_data_export_handler export;        /**< Export a configuration value
                                                 to persistent storage */
    conf_data_delete_handler delete;        /**< Delete a configuration value
                                                 from persistent storage */
} conf_handler_backend_ops_t;

/**
 * @brief   Configuration handler base type
 */
#define _CONF_HANDLER_BASE_T                                \
struct {                                                    \
    _CONF_SUBTREE                                           \
    conf_sid_t sid_lower;                                   \
}

/**
 * @brief   Configuration handler array identifier
 */
#define CONF_HANDLER_ARRAY_ID_T                             \
struct {                                                    \
    _CONF_HANDLER_BASE_T;                                   \
    conf_sid_t sid_upper;                                   \
    uint32_t sid_stride;                                    \
}

/**
 * @brief   A subrange of identifiers for a configuration array which handles a variable number of
 *          (compound) items
 */
typedef CONF_HANDLER_ARRAY_ID_T conf_handler_array_id_t;

/**
 * @brief   Configuration handler node identification
 */
#define CONF_HANDLER_NODE_ID_T                              \
struct {                                                    \
    _CONF_HANDLER_BASE_T;                                   \
    conf_sid_t sid_upper;                                   \
}

/**
 * @brief   A subrange of identifiers for a configuration node which handles a compound item
 */
typedef CONF_HANDLER_NODE_ID_T conf_handler_node_id_t;

/**
 * @brief   Configuration handler identification
 */
#define CONF_HANDLER_ID_T                                   \
struct {                                                    \
    _CONF_HANDLER_BASE_T;                                   \
}

/**
 * @brief   An identifier for a configuration handler which has no subitems
 */
typedef CONF_HANDLER_ID_T conf_handler_id_t;

/**
 * @brief   Configuration of handler behavior
 */
typedef struct {
    uint32_t handles_array      :1;     /**< True if the handler handles an array of items */
    uint32_t handles_primitive  :1;     /**< True if the handler handles a primitive item */
    uint32_t primitive_type     :6;     /**< Type of the primitive item */
} conf_handler_flags_t;

enum {
    CONF_PRIM_TYPE_UINT = 0,            /**< Unsigned integer */
    CONF_PRIM_TYPE_INT,                 /**< Signed integer */
    CONF_PRIM_TYPE_BSTR,                /**< Byte string */
    CONF_PRIM_TYPE_TSTR,                /**< Text string */
    /* extend on demand */
};

/**
 * @brief   Configuration flags initializer
 * @internal
 */
#define _CONF_FLAGS(_flags)         _flags()
/**
 * @brief   Configuration array handler flags initializer
 * @internal use CONF_FLAGS_ARRAY
 */
#define CONF_FLAGS_ARRAY()          { .handles_array = 1 }
/**
 * @brief   Configuration unsigned integer handler flags initializer
 * @internal use CONF_FLAGS_PRIMITIVE_UINT
 */
#define CONF_FLAGS_PRIMITIVE_UINT() { .handles_primitive = 1, .primitive_type = CONF_PRIM_TYPE_UINT }
/**
 * @brief   Configuration signed integer handler flags initializer
 * @internal use CONF_FLAGS_PRIMITIVE_INT
 */
#define CONF_FLAGS_PRIMITIVE_INT()   { .handles_primitive = 1, .primitive_type = CONF_PRIM_TYPE_INT }
/**
 * @brief   Configuration byte string handler flags initializer
 * @internal use CONF_FLAGS_PRIMITIVE_BSTR
 */
#define CONF_FLAGS_PRIMITIVE_BSTR()  { .handles_primitive = 1, .primitive_type = CONF_PRIM_TYPE_BSTR }
/**
 * @brief   Configuration text string handler flags initializer
 * @internal use CONF_FLAGS_PRIMITIVE_TSTR
 */
#define CONF_FLAGS_PRIMITIVE_TSTR()  { .handles_primitive = 1, .primitive_type = CONF_PRIM_TYPE_TSTR }

/**
 * @brief   A node in the configuration tree
 */
typedef struct conf_handler {
    list_node_t node;                                   /**< Every node is in a list, managed by its parent */
    struct conf_handler *subnodes;                      /**< Every node has a list of subnodes */
    union {
        const conf_handler_array_id_t *array_id;        /**< Pointer to handler array identification */
        const conf_handler_node_id_t *node_id;          /**< Pointer to handler node identification */
        const conf_handler_id_t *handler_id;            /**< Pointer to handler identification */
    };
    const conf_handler_data_ops_t *ops_dat;             /**< Handler data operations */
#if IS_USED(MODULE_CONFIGURATION_CUSTOM_OPERATIONS) || defined(DOXYGEN)
    const conf_handler_backend_ops_t *ops_be;           /**< Handler backend operations */
    const conf_handler_backend_data_ops_t *ops_be_dat;  /**< Handler backend data operations */
#endif
    const struct conf_backend *src_backend;             /**< Backend to load configuration data from */
#if IS_USED(MODULE_CONFIGURATION_DESTINATION_BACKEND) || defined(DOXYGEN)
    const struct conf_backend *dst_backend;             /**< Backend to store configuration data to */
#endif
    void *data;                                         /**< Pointer to the configuration item data location (may be NULL) */
    uint32_t size;                                      /**< Configuration item size in bytes */
    unsigned level;                                     /**< Level in the configuration tree (root = 0) */
    conf_handler_flags_t conf_flags;                    /**< Configuration of handler behavior */
    mutex_t mutex;                                      /**< Lock for unique access to the configuration item */
} conf_handler_t;

/**
 * @brief   A node with handler operations, which handle an array of configuration objects
 */
typedef struct conf_array_handler {
    conf_handler_t handler;             /**< Configuration handler */
    uint32_t array_size;                /**< Number of items in the array */
} conf_array_handler_t;

/**
 * @brief   Get the array index of a normalized SID
 * @internal
 *
 * @param[in]           array       Reference to the array handler
 * @param[in]           sid         SID of the configuration item
 *
 * @return  Array index
 */
static inline uint32_t _sid_array_index(const conf_array_handler_t *array, conf_sid_t sid)
{
    assert(sid > array->handler.array_id->sid_lower);
    return (sid - array->handler.array_id->sid_lower - 1) / array->handler.array_id->sid_stride;
}

/**
 * @brief   Get the array index remainder of a normalized SID
 * @internal
 *
 * @param[in]           array       Reference to the array handler
 * @param[in]           sid         SID of the configuration item
 *
 * @return  Array index remainder
 */
static inline uint64_t _sid_array_remainder(const conf_array_handler_t *array, conf_sid_t sid)
{
    assert(sid > array->handler.array_id->sid_lower);
    return (sid - array->handler.array_id->sid_lower - 1) % array->handler.array_id->sid_stride;
}

/**
 * @brief   Flags to be interpreted by the backend handlers
 */
typedef enum {
    CONF_BACKEND_FLAG_START   = 1u << 0,    /**< Storage backend is not opened yet */
    CONF_BACKEND_FLAG_FINISH  = 1u << 1,    /**< Storage backend must be closed */
    CONF_BACKEND_FLAG_MORE    = 1u << 2,    /**< More data is available */
} conf_backend_flags_t;

/**
 * @brief   Handler prototype to load configuration data from a persistent storage backend
 *
 * This is called by the configuration handler on import.
 *
 * @param[in]           be          Reference to the backend
 * @param[in]           key         Configuration key which belongs to the configuration item
 * @param[out]          val         Import location of the configuration value
 * @param[in, out]      size        Maximum size of the value to be imported as input,
 *                                  and remaining size as output.
 *                                  If offset is 0 it returns the full size of the configuration value
 * @param[in]           offset      Offset in the value to be imported
 *                                  (may not be supported by all backends)
 * @param[in, out]      flg         Backend flags
 *
 * @return  0 on success
 */
typedef int (*conf_backend_load_handler) (const struct conf_backend *be,
                                          conf_key_buf_t *key, void *val, size_t *size,
                                          size_t offset, conf_backend_flags_t *flg);

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
 * @param[in]           offset      Offset in the value to be exported
 *                                  (may not be supported by all backends)
 * @param[in]           flg         Backend flags
 *
 * @return  0 on success
 */
typedef int (*conf_backend_store_handler) (const struct conf_backend *be,
                                           conf_key_buf_t *key, const void *val, size_t *size,
                                           size_t offset, conf_backend_flags_t *flg);

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
    conf_fmt_t fmt;                             /**< Encoding format */
} conf_backend_t;

/**
 * @brief   Get or set the source backend of a configuration handler
 *
 * @param[in]           handler     Reference to the handler
 *
 * @return  Reference to the source backend
 */
static inline
const struct conf_backend **configuration_get_src_backend(const conf_handler_t *handler)
{
    return (const struct conf_backend **)&handler->src_backend;
}

/**
 * @brief   Get or set the destination backend of a configuration handler
 *
 * @param[in]           handler     Reference to the handler
 *
 * @return  Reference to the destination backend
 */
static inline
const struct conf_backend **configuration_get_dst_backend(const conf_handler_t *handler)
{
#if IS_USED(MODULE_CONFIGURATION_DESTINATION_BACKEND)
    return (const struct conf_backend **)&handler->dst_backend;
#endif
    return (const struct conf_backend **)&handler->src_backend;
}

/**
 * @brief   Get access to the key string buffer or NULL
 *          if the module configuration_strings is not used
 *
 * @param[in]           key             Configuration key buffer
 *
 * @return  Pointer to the key string buffer or NULL
 */
static inline char *configuration_key_buf(conf_key_t *key)
{
#if IS_USED(MODULE_CONFIGURATION_STRINGS)
    return ((conf_key_buf_t *)key)->buf;
#else
    (void)key;
    return "";
#endif
}

/**
 * @brief   Get the root node of the configuration tree
 *
 * @return  Configuration root node
 */
conf_handler_t *configuration_get_root(void);

/**
 * @brief   Append a configuration node to the configuration tree
 *
 * @param[in]           parent          Parent configuration node
 * @param[in]           node            New configuration node which is
 *                                      either an intermediate or a leaf node
 */
void configuration_register(conf_handler_t *parent, conf_handler_t *node);

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
 *
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
 * @brief   Default handler backend operations
 */
extern const conf_handler_backend_ops_t configuration_default_backend_ops;
/**
 * @brief   Default handler backend data operations
 */
extern const conf_handler_backend_data_ops_t configuration_default_backend_data_ops;

#ifdef __cplusplus
}
#endif
#endif /* CONFIGURATION_H */
/** @} */
