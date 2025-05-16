/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        key/lifetime.h
 * @brief       Key ilfetime definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Encoding of key lifetimes.
 *
 * @details The lifetime of a key indicates where it is stored and which application and system
 *          actions will create and destroy it.
 *
 *          Lifetime values have the following structure:
 *          - Bits[7:0]: Persistence level
 *            This value indicates what device management actions can cause it to be destroyed.
 *            In particular, it indicates whether the key is volatile or persistent.
 *            See @ref psa_key_persistence_t for more information.
 *            @ref PSA_KEY_LIFETIME_GET_PERSISTENCE(@p lifetime) returns the persistence level for
 *            a key lifetime value.
 *          - Bits[31:8]: Location indicator
 *            This value indicates where the key material is stored (or at least where it is
 *            accessible in cleartext) and where operations on the key are performed. See
 *            @ref psa_key_location_t for more information.
 *            @ref PSA_KEY_LIFETIME_GET_LOCATION(@p lifetime) returns the location indicator for a
 *            key lifetime value.
 *
 *          Volatile keys are automatically destroyed when the application instance terminates or
 *          on a power reset of the device. Persistent keys are preserved until the application
 *          explicitly destroys them or until an implementation-specific device management event
 *          occurs, for example, a factor reset.
 *
 *          Persistent keys have a key identifier of type @ref psa_key_id_t. This identifier
 *          remains valid throughout the lifetime of the key, even if the application instance
 *          that created the key terminates.
 *
 *          This specification defines two basic lifetime values:
 *          - Keys with the lifetime @ref PSA_KEY_LIFETIME_VOLATILE are volatile. All
 *            implementations should support this lifetime.
 *          - Keys with the lifetime @ref PSA_KEY_LIFETIME_PERSISTENT are persistent. All
 *            implementations that have access to persistent storage with appropriate security
 *            guarantees should support this lifetime.
 */
typedef uint32_t psa_key_lifetime_t;

/**
 * @brief   Encoding of key persistence levels.
 *
 * @details What distinguishes different persistence levels is which device management events can
 *          cause keys to be destroyed. For example, power reset, transfer of device ownership,
 *          or a factory reset are device management events that can affect keys at different
 *          persistence levels. The specific management events which affect persistent keys at
 *          different levels is outside the scope of the PSA Cryptography specification.
 *
 * @note    Key persistence levels are 8-bit values. Key management interfaces operate on lifetimes
 *          (type @ref psa_key_lifetime_t), and encode the persistence value as the lower 8 bits of
 *          a 32-bit value.
 *
 *          Values for persistence levels defined by this specification are shown below:
 *          - @c 0 =    @ref PSA_KEY_PERSISTENCE_VOLATILE : Volatile key.
 *                      A volatile key is automatically destroyed by the implementation when the
 *                      application instance terminates. In particular, a volatile key is
 *                      automatically destroyed on a power reset of the device.
 *          - @c 1 =    @ref PSA_KEY_PERSISTENCE_DEFAULT : Persistent key with a default lifetime.
 *                      Implementations should support this value if they support persistent keys
 *                      at all. Applications should use this value if they have no specific needs
 *                      that are only met by implementation-specific features.
 *          - @c 2–127: Persistent key with a PSA-specified lifetime. The PSA Cryptography
 *                      specification does not define the meaning of these values, but other PSA
 *                      specifications may do so.
 *          - @c 128–254:   Persistent key with a vendor-specified lifetime. No PSA specification
 *                          will define the meaning of these values, so implementations may choose
 *                          the meaning freely. As a guideline, higher persistence levels should
 *                          cause a key to survive more management events than lower levels.
 *          - @c 255 =  @ref PSA_KEY_PERSISTENCE_READ_ONLY : Read-only or write-once key.
 *                      A key with this persistence level cannot be destroyed. Implementations that
 *                      support such keys may either allow their creation through the PSA
 *                      Cryptography API, preferably only to applications with the appropriate
 *                      privilege, or only expose keys created through implementation-specific
 *                      means such as a factory ROM engraving process.
 *                      @note   Keys that are read-only due to policy restrictions rather than
 *                              due to physical limitations should not have this persistence level.
 */
typedef uint8_t psa_key_persistence_t;

/**
 * @brief   Encoding of key location indicators.
 *
 * @details If an implementation of this API can make calls to external cryptoprocessors such as
 *          secure elements, the location of a key indicates which secure element performs the
 *          operations on the key. If the key material is not stored persistently inside the secure
 *          element, it must be stored in a wrapped form such that only the secure element can
 *          access the key material in cleartext.
 *
 * @note    Key location indicators are 24-bit values. Key management interfaces operate on
 *          lifetimes (type @ref psa_key_lifetime_t), and encode the location as the upper 24 bits
 *          of a 32-bit value.
 *
 *          Values for location indicators defined by this specification are shown below:
 *          - @c 0:  Primary local storage.
 *                   All implementations should support this value. The primary local storage is
 *                   typically the same storage area that contains the key metadata.
 *          - @c 1:  Primary secure element.
 *                   @note  As of now, this value is not supported by this implementation.
 *                          Use the vendor-defined location values.
 *
 *                   Implementations should support this value if there is a secure element
 *                   attached to the operating environment. As a guideline, secure elements may
 *                   provide higher resistance against side channel and physical attacks than the
 *                   primary local storage, but may have restrictions on supported key types,
 *                   sizes, policies and operations and may have different performance
 *                   characteristics.
 *          - @c 2–0x7fffff:    Other locations defined by a PSA specification.
 *                              The PSA Cryptography API does not currently assign any meaning to
 *                              these locations, but future versions of this specification or other
 *                              PSA specifications may do so.
 *          - @c 0x800000–0xffffff: Vendor-defined locations. No PSA specification will assign a
 *                                  meaning to locations in this range.
 */
typedef uint32_t psa_key_location_t;

/**
 * @brief   The default lifetime for volatile keys.
 *
 * @details A volatile key only exists as long as its identifier is not destroyed. The key material
 *          is guaranteed to be erased on a power reset.
 *
 *          A key with this lifetime is typically stored in the RAM area of the PSA Crypto
 *          subsystem. However this is an implementation choice. If an implementation stores data
 *          about the key in a non-volatile memory, it must release all the resources associated
 *          with the key and erase the key material if the calling application terminates.
 */
#define PSA_KEY_LIFETIME_VOLATILE ((psa_key_lifetime_t)0x00000000)

/**
 * @brief   The default lifetime for persistent keys.
 *
 * @details A persistent key remains in storage until it is explicitly destroyed or until the
 *          corresponding storage area is wiped. This specification does not define any mechanism
 *          to wipe a storage area. Implementations are permitted to provide their own mechanism,
 *          for example, to perform a factory reset, to prepare for device refurbishment, or to
 *          uninstall an application.
 *
 *          This lifetime value is the default storage area for the calling application.
 *          Implementations can offer other storage areas designated by other lifetime values as
 *          implementation-specific extensions.
 */
#define PSA_KEY_LIFETIME_PERSISTENT ((psa_key_lifetime_t)0x00000001)

/**
 * @brief   The persistence level of volatile keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_VOLATILE ((psa_key_persistence_t)0x00)

/**
 * @brief   The default persistence level for persistent keys.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_DEFAULT ((psa_key_persistence_t)0x01)

/**
 * @brief   A persistence level indicating that a key is never destroyed.
 *
 * @details See @ref psa_key_persistence_t for more information.
 */
#define PSA_KEY_PERSISTENCE_READ_ONLY ((psa_key_persistence_t)0xff)

/**
 * @brief   The local storage area for persistent keys.
 *
 * @details This storage area is available on all systems that can store persistent keys without
 *          delegating the storage to a third-party cryptoprocessor.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_LOCAL_STORAGE          ((psa_key_location_t)0x000000)

/**
 * @brief   The default secure element storage area for persistent keys.
 *
 * @details This storage location is available on systems that have one or more secure elements
 *          that are able to store keys.
 *
 *          Vendor-defined locations must be provided by the system for storing keys in additional
 *          secure elements.
 *
 *          See @ref psa_key_location_t for more information.
 */
#define PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT ((psa_key_location_t)0x000001)

/**
 * @brief   Mark vendor defined key locations
 */
#define PSA_KEY_LOCATION_VENDOR_FLAG            ((psa_key_location_t)0x800000)

/**
 * @brief   Minimum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MIN (PSA_KEY_LOCATION_VENDOR_FLAG)

/**
 * @brief   Maximum location value for secure elements
 */
#define PSA_KEY_LOCATION_SE_MAX ((psa_key_location_t)0x8000ff)

/**
 * @brief   Extract the persistence level from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) \
    ((psa_key_persistence_t)((lifetime) & 0x000000ff))

/**
 * @brief   Extract the location indicator from a key lifetime.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 */
#define PSA_KEY_LIFETIME_GET_LOCATION(lifetime) \
    ((psa_key_location_t)((lifetime) >> 8))

/**
 * @brief   Whether a key lifetime indicates that the key is volatile.
 *
 *          A volatile key is automatically destroyed by the implementation when the application
 *          instance terminates. In particular, a volatile key is automatically destroyed on a
 *          power reset of the device.
 *
 *          A key that is not volatile is persistent. Persistent keys are preserved until the
 *          application explicitly destroys them or until an implementation-specific device
 *          management event occurs, for example, a factory reset.
 *
 * @param   lifetime The lifetime value to query: a value of type @ref psa_key_lifetime_t.
 *
 * @return  1 if the key is volatile, otherwise 0.
 */
#define PSA_KEY_LIFETIME_IS_VOLATILE(lifetime) \
    (PSA_KEY_LIFETIME_GET_PERSISTENCE(lifetime) == PSA_KEY_PERSISTENCE_VOLATILE)

/**
 * @brief   Construct a lifetime from a persistence level and a location.
 *
 * @param   persistence The persistence level: a value of type @ref psa_key_persistence_t.
 * @param   location    The location indicator: a value of type @ref psa_key_location_t.
 *
 * @return  The constructed lifetime value.
 */
#define PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location) \
    ((location) << 8 | (persistence))

#ifdef __cplusplus
}
#endif

/** @} */
