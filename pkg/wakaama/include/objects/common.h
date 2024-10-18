/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_wakaama
 * @defgroup    lwm2m_objects_common Common LwM2M Object functionalities
 * @brief       Common functionalities to interact with LwM2M objects
 * @{
 *
 * @file
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef OBJECTS_COMMON_H
#define OBJECTS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "liblwm2m.h"
#include "lwm2m_client.h"

/**
 * @brief Get the handle for an object type of the client by its ID.
 *
 * @note This is not a single instance of a given object, but the object type
 *       representation.
 *
 * @param[in] client_data       Pointer to the client data
 * @param[in] id                ID of the object to get
 *
 * @return Pointer to the object type
 */
static inline lwm2m_object_t *lwm2m_get_object_by_id(lwm2m_client_data_t *client_data, uint16_t id)
{
    return (lwm2m_object_t *)LWM2M_LIST_FIND(client_data->lwm2m_ctx->objectList, id);
}

/**
 * @brief Get the value of a string-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (out != NULL)`
 *
 * @param[in]       client_data       Pointer to the LwM2M client data.
 * @param[in]       uri               Initialized URI structure specifying the resource to get.
 * @param[out]      out               Buffer to place the resource in. Must not be NULL.
 * @param[in, out]  out_len           Available space in @p out, returns the amount of read bytes.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_string(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, char *out,
                     size_t *out_len);

/**
 * @brief Get the value of a string-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_string with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (out != NULL)`
 *
 * @param[in]       client_data       Pointer to the LwM2M client data.
 * @param[in]       path              Array containing the path to the resource to get.
 * @param[in]       path_len          Length of @p path.
 * @param[out]      out               Buffer to place the resource in. Must not be NULL.
 * @param[in, out]  out_len           Available space in @p out, returns the amount of read bytes.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_string_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             char *out, size_t *out_len);

/**
 * @brief Get the value of an opaque-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (out != NULL)`
 *
 * @param[in]       client_data       Pointer to the LwM2M client data.
 * @param[in]       uri               Initialized URI structure specifying the resource to get.
 * @param[out]      out               Buffer to place the resource in. Must not be NULL.
 * @param[in, out]  out_len           Available space in @p out, returns the amount of read bytes.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_opaque(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, uint8_t *out,
                     size_t *out_len);

/**
 * @brief Get the value of an opaque-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_opaque with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (out != NULL)`
 *
 * @param[in]       client_data       Pointer to the LwM2M client data.
 * @param[in]       path              Array containing the path to the resource to get.
 * @param[in]       path_len          Length of @p path.
 * @param[out]      out               Buffer to place the resource in. Must not be NULL.
 * @param[in, out]  out_len           Available space in @p out, returns the amount of read bytes.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_opaque_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             uint8_t *out, size_t *out_len);

/**
 * @brief Get the value of an integer-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to get.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_int(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, int64_t *out);

/**
 * @brief Get the value of an integer-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_int with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_int_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                          int64_t *out);

/**
 * @brief Get the value of an float-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to get.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_float(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, double *out);

/**
 * @brief Get the value of an float-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_float with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_float_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                            double *out);

/**
 * @brief Get the value of an boolean-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to get.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_bool(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, bool *out);

/**
 * @brief Get the value of an float-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_bool with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[out] out               Pointer where to place the value.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_bool_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                           bool *out);

/**
 * @brief Get the value of an objectlink-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (object_id_out != NULL) &&
 *       (instance_id_out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to get.
 * @param[out] object_id_out     Pointer where to place the object ID.
 * @param[out] instance_id_out   Pointer where to place the instance ID.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the resource cannot be read or has an unexpected type
 */
int lwm2m_get_objlink(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                      uint16_t *object_id_out, uint16_t *instance_id_out);

/**
 * @brief Get the value of an objectlink-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_get_objlink with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (object_id_out != NULL) &&
 *       (instance_id_out != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[out] object_id_out     Pointer where to place the object ID.
 * @param[out] instance_id_out   Pointer where to place the instance ID.
 *
 * @retval 0 on success
 * @retval -ENOMEM when there is not enough space in buffer or can not allocate a data structure
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL if the path is malformed, the resource cannot be read or has an unexpected type
 */
int lwm2m_get_objlink_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                              uint16_t *object_id_out, uint16_t *instance_id_out);

/**
 * @brief Set the value of a string-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (val != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  val               Buffer with the value to set. Must not be NULL.
 * @param[in]  val_len           Length of @p val.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_string(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, char *val,
                     size_t val_len);

/**
 * @brief Set the value of a string-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_string with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (val != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  val               Buffer with the value to set. Must not be NULL.
 * @param[in]  val_len           Length of @p val.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_string_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             char *val, size_t val_len);

/**
 * @brief Set the value of an opaque-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL) && (val != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  val               Buffer with the value to set. Must not be NULL.
 * @param[in]  val_len           Length of @p val.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_opaque(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, uint8_t *val,
                     size_t val_len);

/**
 * @brief Set the value of an opaque-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_opaque with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL) && (val != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  val               Buffer with the value to set. Must not be NULL.
 * @param[in]  val_len           Length of @p val.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_opaque_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             uint8_t *val, size_t val_len);

/**
 * @brief Set the value of an integer-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_int(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, int64_t val);

/**
 * @brief Set the value of an integer-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_int with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_int_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                          int64_t val);

/**
 * @brief Set the value of an float-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_float(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, double val);

/**
 * @brief Set the value of an float-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_float with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_float_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                            double val);

/**
 * @brief Set the value of an boolean-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_bool(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, bool val);

/**
 * @brief Set the value of an float-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_bool with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  val               Value to set.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_bool_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                           bool val);

/**
 * @brief Set the value of an objectlink-type resource, specified by @p uri.
 *
 * @pre `(client_data != NULL) && (uri != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to set.
 * @param[in]  object_id_in      Object ID value to set.
 * @param[in]  instance_id_in    Instance ID value to set.
 *
 * @retval 0 on success
 * @retval -ENOMEM when data structure can not be allocated
 * @retval -ENOENT when the resource is not found
 * @retval -EINVAL when the value can not be set
 */
int lwm2m_set_objlink(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                      uint16_t object_id_in, uint16_t instance_id_in);

/**
 * @brief Set the value of an objectlink-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref lwm2m_set_objlink with a string representing the resource's
 * path.
 *
 * @pre `(client_data != NULL) && (path != NULL)`
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to set.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  object_id_in      Object ID value to set.
 * @param[in]  instance_id_in    Instance ID value to set.
 *
 * @retval 0 on success
 * @retval -EINVAL when the path is malformed, the value can not be set
 * @retval -ENOENT when the resource is not found
 * @retval -ENOMEM when data structure can not be allocated
 */
int lwm2m_set_objlink_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                              uint16_t object_id_in, uint16_t instance_id_in);

#ifdef DOXYGEN
/**
 * @name    URI representation
 * @brief   Note that these are defined in `liblwm2m.h`.
 * @{
 */

/**
 * @brief Flag to indicate in @ref lwm2m_uri_t::flag that the object ID is present.
 */
#define LWM2M_URI_FLAG_OBJECT_ID    (uint8_t)0x04

/**
 * @brief Flag to indicate in @ref lwm2m_uri_t::flag that the instance ID is present.
 */
#define LWM2M_URI_FLAG_INSTANCE_ID  (uint8_t)0x02

/**
 * @brief Flag to indicate in @ref lwm2m_uri_t::flag that the resource ID is present.
 */
#define LWM2M_URI_FLAG_RESOURCE_ID  (uint8_t)0x01

/**
 * @brief Representation of an URI in a LwM2M client (defined in `liblwm2m.h`).
 */
typedef struct {
    uint8_t     flag;       /**< indicates which segments are present */
    uint16_t    objectId;   /**< object ID */
    uint16_t    instanceId; /**< instance ID */
    uint16_t    resourceId; /**< resource ID */
} lwm2m_uri_t;

/** @} */
#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OBJECTS_COMMON_H */
