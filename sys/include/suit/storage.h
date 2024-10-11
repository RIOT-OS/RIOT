/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_storage SUIT secure firmware OTA upgrade storage
 *                               infrastructure
 * @ingroup     sys_suit
 * @brief       SUIT firmware storage backends
 *
 * @{
 *
 * @brief       Storage backend functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * The interface defined here specifies a generic API for SUIT OTA storage
 * backends.
 *
 * The driver allows for creating multiple backends, each possibly servicing
 * multiple locations. An example of this is a VFS storage backend. This backend
 * could service multiple file locations on a filesystem.
 *
 * A SUIT component ID is formatted as an array of bytestrings. To make it easy
 * to match and use a string, the location is supplied as string, each component
 * separated by a separator provided in the driver. If no separator (`\0`) is
 * set, the components are concatenated without separator. The @ref
 * suit_storage_driver_t::set_active_location must be called before starting
 * operations on the backend.
 *
 * A write sequence by the caller must start with @ref
 * suit_storage_driver_t::start. The total length of the image is supplied to
 * allow the backend to check if the payload fits in the available space.  The
 * payload data can be supplied piecewise with multiple calls to @ref
 * suit_storage_driver_t::write. The caller is free to specify the offset, but
 * the backend may enforce strict monotonicity on the offset and may enforce the
 * gapless writes. After all bytes are supplied, the @ref
 * suit_storage_driver_t::finish function must be called to signal the end of
 * the write stage.
 *
 * Only when the @ref suit_storage_driver_t::install is called, the payload must
 * be marked as valid. The mechanism for this can be backend specific. However
 * in the case of a firmware image, it must not be bootable before this function
 * is called. Similarly, a file payload must not be available at its provided
 * path until after this function is called. The reason behind this is that the
 * payload often must first be stored on the device before the image_match is
 * called by the manifest.
 *
 * The other option is that the @ref suit_storage_driver_t::erase is called. In
 * this case the not-yet-installed payload must be erased again as its contents
 * might not be what is expected by the digest in the manifest. The payload must
 * then be removed to prevent the possibility of storing malicious code on the
 * device.
 *
 * A form of read access must be implemented to provide a way to read back the
 * data and check the digest of the payload. @ref suit_storage_driver_t::read
 * must be implemented, providing piecewise reading of the data. @ref
 * suit_storage_driver_t::read_ptr is optional to implement, it can provide
 * direct read access on memory-mapped storage.
 *
 * As the storage backend provides a mechanism to store persistent data,
 * functions are added to set and retrieve the manifest sequence number. While
 * not strictly required to implement, a firmware without a mechanism to
 * retrieve and store sequence numbers will always fail to update.
 *
 * The @ref suit_storage_driver_t::match_offset function allows the manifest
 * handler to check the component-offset condition against a storage backend.
 *
 * The usual call sequence by a manifest handler is:
 *
 * 1.  @ref suit_storage_driver_t::init as on-boot initialization.
 * 2.  @ref suit_storage_driver_t::get_seq_no to determine if the manifest is
 *     not replayed.
 * 3.  @ref suit_storage_driver_t::has_location to determine if the backend
 *     handles the payload in the manifest.
 * 4.  @ref suit_storage_driver_t::set_active_location to set the active
 *     location for the payload.
 * 5.  @ref suit_storage_driver_t::start to start a payload write sequence.
 * 6.  At least one @ref suit_storage_driver_t::write calls to write the payload
 *     data.
 * 7.  @ref suit_storage_driver_t::finish to mark the end of the payload write.
 * 8.  @ref suit_storage_driver_t::read or @ref suit_storage_driver_t::read_ptr
 *     to read back the written payload. This to verify the digest of the
 *     payload with what is provided in the manifest.
 * 9.  @ref suit_storage_driver_t::install if the digest matches with what is
 *     expected and the payload can be installed or marked as valid, or:
 * 10. @ref suit_storage_driver_t::erase if the digest does not match with what
 *     is expected and must be erased.
 * 11. ref suit_storage_driver_t::set_seq_no to update the sequence number
 *     stored in the backend.
 *
 * @warning This API is by design not thread safe
 */

#ifndef SUIT_STORAGE_H
#define SUIT_STORAGE_H

#include "suit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration for storage struct
 */
typedef struct suit_storage suit_storage_t;

/**
 * @brief SUIT storage backend driver struct
 */
typedef struct suit_storage_driver {

    /**
     * @brief One-time initialization function. Called at boot.
     *
     * @param[in]   storage     Storage context
     */
    int (*init)(suit_storage_t *storage);

    /**
     * @brief Start a new payload write sequence
     *
     * @param[in]   storage     Storage context
     * @param[in]   manifest    The suit manifest context
     * @param[in]   len         Total size of the payload in bytes
     *
     * @returns     @ref SUIT_OK on successfully starting the write
     * @returns     @ref suit_error_t on error
     */
    int (*start)(suit_storage_t *storage, const suit_manifest_t *manifest,
                 size_t len);

    /**
     * @brief   Write a new chunk of the payload to the storage backend
     *
     * @param[in]   storage     Storage context
     * @param[in]   manifest    The suit manifest context
     * @param[in]   buf         Buffer to read the payload chunk from
     * @param[in]   offset      Offset to write at
     * @param[in]   len         Length of the payload chunk
     *
     * @returns     @ref SUIT_OK on successfully writing the chunk
     * @returns     @ref suit_error_t on error
     */
    int (*write)(suit_storage_t *storage, const suit_manifest_t *manifest, const
                 uint8_t *buf, size_t offset, size_t len);

    /**
     * @brief Signal that the payload write stage done to the storage backend
     *
     * @param[in]   storage     Storage context
     * @param[in]   manifest    The suit manifest context
     *
     * @returns     @ref SUIT_OK on successfully finalizing the write
     * @returns     @ref suit_error_t on error
     */
    int (*finish)(suit_storage_t *storage, const suit_manifest_t *manifest);

    /**
     * @brief Read a chunk of previously written data back.
     *
     * @param[in]   storage     Storage context
     * @param[out]  buf         Buffer to write the read data in
     * @param[in]   offset      Offset to read from
     * @param[in]   len         Number of bytes to read
     *
     * @returns     @ref SUIT_OK on successfully reading the chunk
     * @returns     @ref suit_error_t on error
     */
    int (*read)(suit_storage_t *storage, uint8_t *buf, size_t offset,
                size_t len);

    /**
     * @brief retrieve a direct read pointer for this storage backend
     *
     * @note Optional to implement
     *
     * @param[in]   storage     Storage context
     * @param[out]  buf         Pointer to the location data
     * @param[out]  len         Full length of the location data
     *
     * @returns     @ref SUIT_OK on successfully providing the region
     * @returns     @ref suit_error_t on error
     */
    int (*read_ptr)(suit_storage_t *storage,
                    const uint8_t **buf, size_t *len);

    /**
     * @brief Install the payload or mark the payload as valid
     *
     * @param[in]   storage     Storage context
     * @param[in]   manifest    The suit manifest context
     *
     * @returns     @ref SUIT_OK on successfully installing the payload
     * @returns     @ref suit_error_t on error
     */
    int (*install)(suit_storage_t *storage, const suit_manifest_t *manifest);

    /**
     * @brief Erase the previously loaded payload
     *
     * @param[in]   storage     Storage context
     *
     * @returns     @ref SUIT_OK on successfully erasing the data
     * @returns     @ref suit_error_t on error
     */
    int (*erase)(suit_storage_t *storage);

    /**
     * @brief Check if this storage backend services a location
     *
     * @param[in] storage     Storage context
     * @param[in] location    Location to check
     *
     * @returns             True if this storage driver must be used for the
     *                      supplied location
     */
    bool (*has_location)(const suit_storage_t *storage, const char *location);

    /**
     * @brief Checks if the supplied offset is true or false for the current
     *        location
     *
     * @note  Optional to implement, should not be implemented if the backend
     *        doesn't support the image_offset
     *
     * @param[in]   storage     Storage context
     * @param[in]   offset      Offset to check
     *
     * @returns     True if the location matches the offset,
     * @returns     False otherwise
     */
    bool (*match_offset)(const suit_storage_t *storage, size_t offset);

    /**
     * @brief Set the active location of the storage handler
     *
     * A storage backend can handle multiple locations, e.g. a VFS backend
     * targeting multiple files on a filesystem, setting the location selects
     * the target location for writes or reads.
     *
     * @note Must be idempotent
     *
     * @param[in]   storage     Storage backend context
     * @param[in]   location    The location supplied as string with components
     *                          separated by the
     *                          @ref suit_storage_driver_t::separator
     *
     * @returns     SUIT_OK on success
     * @returns     SUIT_ERR_STORAGE_UNAVAILABLE if the location is not
     *                      available.
     */
    int (*set_active_location)(suit_storage_t *storage, const char *location);

    /**
     * @brief Retrieve the sequence number from the storage backend
     *
     * @note The sequence number must be global to the storage context, it must
     *       not depend on the location
     *
     * @param[in]   storage     Storage context
     * @param[out]  seq_no      Retrieved sequence number
     *
     * @returns     SUIT_OK on success
     * @returns     @ref suit_error_t if the sequence number can't be retrieved
     */
    int (*get_seq_no)(const suit_storage_t *storage, uint32_t *seq_no);

    /**
     * @brief Set a new sequence number in the storage backend.
     *
     * @param[in]   storage     Storage context
     * @param[in]   seq_no      Sequence number to store
     *
     * @returns     SUIT_OK on success
     * @returns     @ref suit_error_t if the sequence number can't be stored.
     */
    int (*set_seq_no)(suit_storage_t *storage, uint32_t seq_no);

    /**
     * @brief Component ID separator used by this storage driver.
     */
    char separator;
} suit_storage_driver_t;

/**
 * @brief Generic storage backend state.
 */
struct suit_storage {
    const suit_storage_driver_t *driver; /**< Storage driver functions */
};

/**
 * @brief retrieve a storage backend based on the location ID string
 *
 * @param[in]   id  ID string to match
 *
 * @returns     The first storage driver that handles this ID
 * @returns     NULL if none of the storage drivers is able to handle this ID.
 */
suit_storage_t *suit_storage_find_by_id(const char *id);

/**
 * @brief retrieve a storage backend based on the suit component
 *
 * @param[in]   manifest    SUIT manifest context
 * @param[in]   component  Component to find a storage backend for
 *
 * @returns     The first storage driver that handles this component
 * @returns     NULL if none of the storage drivers is able to handle this
 *              component.
 */
suit_storage_t *suit_storage_find_by_component(const suit_manifest_t *manifest,
                                               const suit_component_t *component);

/**
 * @brief initialize all storage backends
 */
void suit_storage_init_all(void);

/**
 * @brief Get the highest sequence number among available backends
 *
 * @param[out]  seq_no  Retrieved sequence number
 *
 * @returns     suit_ok if at least one sequence number is retrieved
 * @returns     @ref suit_error_t on error
 */
int suit_storage_get_highest_seq_no(uint32_t *seq_no);

/**
 * @brief Set the new sequence number on all available backends
 *
 * @param[in]   seq_no  Sequence number to store
 *
 * @returns     @ref SUIT_OK on successfully storing the sequence number on at
 *              least one backend
 * @returns     @ref suit_error_t on error
 */
int suit_storage_set_seq_no_all(uint32_t seq_no);

/**
 * @name Storage driver helper functions
 *
 * For easy access to the @ref suit_storage_driver_t functions.
 * @{
 */

/**
 * @brief get the separator for a storage backend
 *
 * @param[in]   storage     Storage context
 *
 * @returns     The separator char
 */
static inline char suit_storage_get_separator(const suit_storage_t *storage)
{
    return storage->driver->separator;
}

/**
 * @brief Check if the storage backend implements the @ref
 * suit_storage_driver_t::read_ptr function
 *
 * @param[in]   storage     Storage context
 *
 * @returns     True if the function is implemented,
 * @returns     False otherwise
 */
static inline bool suit_storage_has_readptr(const suit_storage_t *storage)
{
    return (storage->driver->read_ptr);
}

/**
 * @brief Check if the storage backend implements the @ref
 * suit_storage_driver_t::match_offset function
 *
 * @param[in]   storage     Storage context
 *
 * @returns     True if the function is implemented,
 * @returns     False otherwise
 */
static inline bool suit_storage_has_offset(const suit_storage_t *storage)
{
    return (storage->driver->match_offset);
}

/**
 * @brief One-time initialization function. Called at boot.
 *
 * @param[in]   storage     Storage context
 */
static inline int suit_storage_init(suit_storage_t *storage)
{
    return storage->driver->init(storage);
}

/**
 * @brief Start a new payload write sequence
 *
 * @param[in]   storage     Storage context
 * @param[in]   manifest    The suit manifest context
 * @param[in]   len         Total size of the payload in bytes
 *
 * @returns     @ref SUIT_OK on successfully starting the write
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_start(suit_storage_t *storage,
                                     const suit_manifest_t *manifest,
                                     size_t len)
{
    return storage->driver->start(storage, manifest, len);
}

/**
 * @brief   Write a new chunk of the payload to the storage backend
 *
 * @param[in]   storage     Storage context
 * @param[in]   manifest    The suit manifest context
 * @param[in]   buf         Buffer to read the payload chunk from
 * @param[in]   offset      Offset to write at
 * @param[in]   len         Length of the payload chunk
 *
 * @returns     @ref SUIT_OK on successfully writing the chunk
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_write(suit_storage_t *storage,
                                     const suit_manifest_t *manifest,
                                     const uint8_t *buf, size_t offset,
                                     size_t len)
{
    return storage->driver->write(storage, manifest, buf, offset, len);
}

/**
 * @brief Signal that the payload write stage done to the storage backend
 *
 * @param[in]   storage     Storage context
 * @param[in]   manifest    The suit manifest context
 *
 * @returns     @ref SUIT_OK on successfully finalizing the write
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_finish(suit_storage_t *storage,
                                      const suit_manifest_t *manifest)
{
    return storage->driver->finish(storage, manifest);
}

/**
 * @brief Read a chunk of previously written data back.
 *
 * @param[in]   storage     Storage context
 * @param[out]  buf         Buffer to write the read data in
 * @param[in]   offset      Offset to read from
 * @param[in]   len         Number of bytes to read
 *
 * @returns     @ref SUIT_OK on successfully reading the chunk
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_read(suit_storage_t *storage, uint8_t *buf,
                                    size_t offset, size_t len)
{
    return storage->driver->read(storage, buf, offset, len);
}

/**
 * @brief retrieve a direct read pointer for this storage backend
 *
 * @note Optional to implement
 *
 * @param[in]   storage     Storage context
 * @param[out]  buf         Pointer to the location data
 * @param[out]  len         Full length of the location data
 *
 * @returns     @ref SUIT_OK on successfully providing the region
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_read_ptr(suit_storage_t *storage, const uint8_t
                                        **buf, size_t *len)
{
    return storage->driver->read_ptr(storage, buf, len);
}

/**
 * @brief Install the payload or mark the payload as valid
 *
 * @param[in]   storage     Storage context
 * @param[in]   manifest    The suit manifest context
 *
 * @returns     @ref SUIT_OK on successfully installing the payload
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_install(suit_storage_t *storage,
                                       const suit_manifest_t *manifest)
{
    return storage->driver->install(storage, manifest);
}

/**
 * @brief Erase the previously loaded payload
 *
 * @param[in]   storage     Storage context
 *
 * @returns     @ref SUIT_OK on successfully erasing the data
 * @returns     @ref suit_error_t on error
 */
static inline int suit_storage_erase(suit_storage_t *storage)
{
    return storage->driver->erase(storage);
}

/**
 * @brief Check if this storage backend services a location
 *
 * @param[in] storage     Storage context
 * @param[in] location    Location to check
 *
 * @returns             True if this storage driver must be used for the
 *                      supplied location
 */
static inline bool suit_storage_has_location(suit_storage_t *storage,
                                             const char *location)
{
    return storage->driver->has_location(storage, location);
}

/**
 * @brief Checks if the supplied offset is true or false for the current
 *        location
 *
 * @note  Optional to implement, should not be implemented if the backend
 *        doesn't support the image_offset
 *
 * @param[in]   storage     Storage context
 * @param[in]   offset      Offset to check
 *
 * @returns     True if the location matches the offset,
 * @returns     False otherwise
 */
static inline int suit_storage_match_offset(const suit_storage_t *storage,
                                            size_t offset)
{
    return storage->driver->match_offset(storage, offset);
}

/**
 * @brief Set the active location of the storage handler
 *
 * A storage backend can handle multiple locations, e.g. a VFS backend
 * targeting multiple files on a filesystem, setting the location selects
 * the target location for writes or reads.
 *
 * @note Must be idempotent
 *
 * @param[in]   storage     Storage backend context
 * @param[in]   location    The location supplied as string with components
 *                          separated by the
 *                          @ref suit_storage_driver_t::separator
 *
 * @returns     SUIT_OK on success
 * @returns     SUIT_ERR_STORAGE_UNAVAILABLE if the location is not
 *                      available.
 */
static inline int suit_storage_set_active_location(suit_storage_t *storage,
                                                   const char *location)
{
    return storage->driver->set_active_location(storage, location);
}

/**
 * @brief Retrieve the sequence number from the storage backend
 *
 * @note The sequence number must be global to the storage context, it must
 *       not depend on the location
 *
 * @param[in]   storage     Storage context
 * @param[out]  seq_no      Retrieved sequence number
 *
 * @returns     SUIT_OK on success
 * @returns     @ref suit_error_t if the sequence number can't be retrieved
 */
static inline int suit_storage_get_seq_no(const suit_storage_t *storage,
                                          uint32_t *seq_no)
{
    return storage->driver->get_seq_no(storage, seq_no);
}

/**
 * @brief Set a new sequence number in the storage backend.
 *
 * @param[in]   storage     Storage context
 * @param[in]   seq_no      Sequence number to store
 *
 * @returns     SUIT_OK on success
 * @returns     @ref suit_error_t if the sequence number can't be stored.
 */
static inline int suit_storage_set_seq_no(suit_storage_t *storage,
                                          uint32_t seq_no)
{
    return storage->driver->set_seq_no(storage, seq_no);
}
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SUIT_STORAGE_H */
