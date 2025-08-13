/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_slot_mgmt    PSA Crypto Key Slot Management
 * @{
 *
 * @file        psa_crypto_slot_management.h
 * @brief       PSA key slot management function declarations
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "clist.h"
#include "psa/crypto.h"
#include "psa_crypto_se_management.h"

/**
 * @brief   Number of allocated slots for keys in protected memory or secure elements.
 */
#define PSA_PROTECTED_KEY_COUNT         (CONFIG_PSA_PROTECTED_KEY_COUNT)

/**
 * @brief   Number of allocated slots for asymmetric key pairs.
 */
#define PSA_ASYMMETRIC_KEYPAIR_COUNT    (CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT)

/**
 * @brief   Number of allocated slots for single keys in local memory.
 */
#define PSA_SINGLE_KEY_COUNT            (CONFIG_PSA_SINGLE_KEY_COUNT)

/**
 * @brief   Complete number of available key slots
 */
#define PSA_KEY_SLOT_COUNT              (PSA_PROTECTED_KEY_COUNT + \
                                         PSA_ASYMMETRIC_KEYPAIR_COUNT + \
                                         PSA_SINGLE_KEY_COUNT)

/**
 * @brief   Minimum key id for volatile keys.
 *
 * @details This is used to assign volatile identifiers to created keys.
 */
#define PSA_KEY_ID_VOLATILE_MIN (PSA_KEY_ID_VENDOR_MIN)

/**
 * @brief   Maximum key id for volatile keys.
 *
 * @details This is the maximum volatile identifiers that can be assigned to created keys.
 */
#define PSA_KEY_ID_VOLATILE_MAX (PSA_KEY_ID_VENDOR_MAX)

/**
 * @brief   Structure of a virtual key slot in local memory.
 *
 * @details A slot contains key attributes, a lock count and the @c key_data structure.
 *          @c key_data consists of the size of the stored key in bytes and a @c uint8_t data array
 *          large enough to store the largest key used in the current build. This type of key slot
 *          contains symmetric keys, asymmetric public keys or unstructured data.
 */
typedef struct {
    clist_node_t node;                          /**< List node to link slot in global list */
    size_t lock_count;                          /**< Number of entities accessing the slot */
    psa_key_attributes_t attr;                  /**< Attributes associated with the stored key */
    /** Structure containing key data */
#if PSA_SINGLE_KEY_COUNT
    struct key_data {
        uint8_t data[PSA_MAX_KEY_DATA_SIZE];    /**< Key data buffer */
        size_t data_len;                        /**< Size of actual key data in bytes */
    } key;                                      /**< Key data structure */
#endif /* PSA_SINGLE_KEY_COUNT */
} psa_key_slot_t;

#if PSA_PROTECTED_KEY_COUNT
/**
 * @brief   Structure for a protected key slot.
 *
 *          These slots hold Slot Numbers for keys in protected storage and, if the key type is an
 *          asymmetric key pair, the public key.
 */
typedef struct {
    clist_node_t node;
    size_t lock_count;
    psa_key_attributes_t attr;
    struct prot_key_data {
        psa_key_slot_number_t slot_number;
#if IS_USED(MODULE_PSA_ASYMMETRIC)
        uint8_t pubkey_data[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
        size_t pubkey_data_len;
#endif
    } key;
} psa_prot_key_slot_t;
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
/**
 * @brief   Structure for asymmetric key pairs.
 *
 *          Contains asymmetric private and public key pairs.
 *
 */
typedef struct {
    clist_node_t node;
    size_t lock_count;
    psa_key_attributes_t attr;
    struct key_pair_data {
        /**  Contains asymmetric private key*/
        uint8_t privkey_data[PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE)];
        /** Contains actual size of asymmetric private key */
        size_t privkey_data_len;
        /** Contains asymmetric public key */
        uint8_t pubkey_data[PSA_EXPORT_PUBLIC_KEY_MAX_SIZE];
        /*!< Contains actual size of asymmetric private key */
        size_t pubkey_data_len;
    } key;
} psa_key_pair_slot_t;
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

/**
 * @brief   Initializes the allocated key slots and prepares the internal key slot lists.
 */
void psa_init_key_slots(void);

/**
 * @brief   Check whether a key identifier is a volatile key identifier.
 *
 * @param   key_id  Key identifier to test.
 *
 * @return  1       The key identifier is a volatile key identifier.
 * @return  0       The key identifier is not a volatile key identifier.
 */
static inline int psa_key_id_is_volatile(psa_key_id_t key_id)
{
    return ((key_id >= PSA_KEY_ID_VOLATILE_MIN) &&
            (key_id <= PSA_KEY_ID_VOLATILE_MAX));
}

/**
 * @brief   Check whether a key slot is locked
 *
 * @param   slot    Pointer to the slot to be checked
 *
 * @return  1 if slot is locked, otherwise 0
 */
static inline int psa_is_key_slot_locked(psa_key_slot_t *slot)
{
    return (slot->lock_count > 0);
}

/**
 * @brief   Get slot number in protected memory
 *
 * @param   slot    Pointer to the slot containing the protected slot number
 * @return  @ref psa_key_slot_number_t  Key slot number stored in the input slot
 */
psa_key_slot_number_t * psa_key_slot_get_slot_number(const psa_key_slot_t *slot);

/**
 * @brief   Check whether a key is stored on an external device
 *
 * @param   lifetime    Lifetime value of the key that's supposed to be checked
 *
 * @return  int
 * @return  1 if key is stored on external device, otherwise 0
 */
static inline int psa_key_lifetime_is_external(psa_key_lifetime_t lifetime)
{
    return (PSA_KEY_LIFETIME_GET_LOCATION(lifetime) != PSA_KEY_LOCATION_LOCAL_STORAGE);
}

/**
 * @brief   Wipe volatile key slot and its contents. Wiped key slots can be reused.
 *
 * @param   slot    Pointer to the key slot to be wiped
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_DOES_NOT_EXIST
 */
psa_status_t psa_wipe_key_slot(psa_key_slot_t *slot);

/**
 * @brief   Wipe all existing volatile key slots.
 */
void psa_wipe_all_key_slots(void);

/**
 * @brief   Find a key slot in local memory and lock it.
 *
 * @param   id      ID of the key to be used
 * @param   slot    Pointer to the slot the key is stored in
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_DOES_NOT_EXIST
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_get_and_lock_key_slot(psa_key_id_t id, psa_key_slot_t **slot);

/**
 * @brief   Store a key slot in persistent storage
 *
 * @param   slot    Pointer to slot to store in persistent storage
 * @return  psa_status_t
 */
psa_status_t psa_persist_key_slot_in_storage(psa_key_slot_t *slot);

/**
 * @brief   Find a currently empty key slot that is appropriate for the key.
 *
 * @param   id      Key ID of the newly generated or imported key
 * @param   attr    Attributes of the key that is supposed to be stored in the slot
 * @param   p_slot  Pointer to the empty slot in memory
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INSUFFICIENT_STORAGE
 */
psa_status_t psa_allocate_empty_key_slot(   psa_key_id_t *id,
                                            const psa_key_attributes_t *attr,
                                            psa_key_slot_t **p_slot);

/**
 * @brief   Increase lock count
 *
 * @param   slot    Slot to be locked
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_lock_key_slot(psa_key_slot_t *slot);

/**
 * @brief   Decrease lock count
 *
 * @param   slot    Slot to be unlocked
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 */
psa_status_t psa_unlock_key_slot(psa_key_slot_t *slot);

/**
 * @brief   Check if key location exists
 *
 * @param   lifetime    Lifetime value of the key to be validated
 * @param   driver      Pointer to driver assigned to the existing key location, if it exists
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT
 */
psa_status_t psa_validate_key_location( psa_key_lifetime_t lifetime,
                                        psa_se_drv_data_t **driver);

/**
 * @brief   Validate key persistence. Currently only volatile keys are supported.
 *
 * @param   lifetime    Lifetime of key to be validated
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_validate_key_persistence(psa_key_lifetime_t lifetime);

/**
 * @brief   Check if provided key ID is either a valid user ID or vendor ID
 *
 * @param   id      ID of key to be validated
 * @param   vendor  If ID is supposed to be user or vendor ID
 *
 * @return  1 if valid
 * @return  0 if invalid
 */
int psa_is_valid_key_id(psa_key_id_t id, int vendor);

/**
 * @brief   Get key data and key size from key slot
 *
 * @param   slot        Slot the desired key is stored in
 * @param   key_data    Pointer to key data
 * @param   key_bytes   Pointer to key data size in bytes
 *
 * @return  Size of @p key_data in bytes.
 */
size_t psa_get_key_data_from_key_slot(const psa_key_slot_t *slot,
                                    uint8_t **key_data,
                                    size_t **key_bytes);

/**
 * @brief   Get public key data and size from key slot
 *
 * @param   slot            Slot the desired key is stored in
 * @param   pubkey_data     Pointer to key data
 * @param   pubkey_data_len    Pointer to key data size in bytes
 */
void psa_get_public_key_data_from_key_slot( const psa_key_slot_t *slot,
                                            uint8_t **pubkey_data,
                                            size_t **pubkey_data_len);

#ifdef __cplusplus
}
#endif

/**@}*/
