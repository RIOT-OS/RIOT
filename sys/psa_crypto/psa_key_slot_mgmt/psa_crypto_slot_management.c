/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto sys_psa_crypto_slot_mgmt
 * @{
 *
 * @file
 * @brief       PSA Crypto Key Slot Management implementation
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "clist.h"
#include "psa_crypto_slot_management.h"

#define ENABLE_DEBUG    0
#include "debug.h"

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

/**
 * @brief   Array containing the protected key slots
 */
static psa_prot_key_slot_t protected_key_slots[PSA_PROTECTED_KEY_COUNT];

/**
 * @brief   List pointing to empty protected key slots
 */
static clist_node_t protected_list_empty;
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

/**
 * @brief   Array containing the asymmetric key slots
 */
static psa_key_pair_slot_t key_pair_slots[PSA_ASYMMETRIC_KEYPAIR_COUNT];

/**
 * @brief   List pointing to empty asymmetric key slots
 */
static clist_node_t key_pair_list_empty;
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_SINGLE_KEY_COUNT
/**
 * @brief   Array containing the single key slots
 */
static psa_key_slot_t single_key_slots[PSA_SINGLE_KEY_COUNT];

/**
 * @brief   List pointing to empty single key slots
 */
static clist_node_t single_key_list_empty;
#endif

/**
 * @brief   Global list of used key slots
 */
static clist_node_t key_slot_list;

/**
 * @brief   Counter for volatile key IDs.
 */
static psa_key_id_t key_id_count = PSA_KEY_ID_VOLATILE_MIN;

/**
 * @brief   Get the correct empty slot list, depending on the key type
 *
 * @param   attr
 * @return  clist_node_t*   Pointer to the list the key is supposed to be stored in,
 *                          according to its attributes
 */
static clist_node_t * psa_get_empty_key_slot_list(const psa_key_attributes_t *attr)
{
    if (!psa_key_lifetime_is_external(attr->lifetime)) {
#if PSA_ASYMMETRIC_KEYPAIR_COUNT
        if (PSA_KEY_TYPE_IS_KEY_PAIR(attr->type)) {
            return &key_pair_list_empty;
        }
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */
#if PSA_SINGLE_KEY_COUNT
        return &single_key_list_empty;
#endif /* PSA_SINGLE_KEY_COUNT */
    }
#if PSA_PROTECTED_KEY_COUNT
    return &protected_list_empty;
#else
    return NULL;
#endif /* PSA_PROTECTED_KEY_COUNT */
}

void psa_init_key_slots(void)
{
#if PSA_PROTECTED_KEY_COUNT
    memset(protected_key_slots, 0, sizeof(protected_key_slots));

    for (size_t i = 0; i < PSA_PROTECTED_KEY_COUNT; i++) {
        clist_rpush(&protected_list_empty, &protected_key_slots[i].node);
    }

    DEBUG("Protected Slot Count: %d, Size: %d\n", PSA_PROTECTED_KEY_COUNT,
          sizeof(psa_prot_key_slot_t));
    DEBUG("Protected Slot Array Size: %d\n", sizeof(protected_key_slots));
    DEBUG("Protected Slot Empty List Size: %d\n", clist_count(&protected_list_empty));
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
    memset(key_pair_slots, 0, sizeof(key_pair_slots));

    for (size_t i = 0; i < PSA_ASYMMETRIC_KEYPAIR_COUNT; i++) {
        clist_rpush(&key_pair_list_empty, &key_pair_slots[i].node);
    }

    DEBUG("Asymmetric Slot Count: %d, Size: %d\n", PSA_ASYMMETRIC_KEYPAIR_COUNT,
          sizeof(psa_key_pair_slot_t));
    DEBUG("Asymmetric Slot Array Size: %d\n", sizeof(key_pair_slots));
    DEBUG("Asymmetric Slot Empty List Size: %d\n", clist_count(&key_pair_list_empty));
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_SINGLE_KEY_COUNT
    memset(single_key_slots, 0, sizeof(single_key_slots));

    for (size_t i = 0; i < PSA_SINGLE_KEY_COUNT; i++) {
        clist_rpush(&single_key_list_empty, &single_key_slots[i].node);
    }

    DEBUG("Single Key Slot Count: %d, Size: %d\n", PSA_SINGLE_KEY_COUNT, sizeof(psa_key_slot_t));
    DEBUG("Single Key Slot Array Size: %d\n", sizeof(single_key_slots));
    DEBUG("Single Key Slot Empty List Size: %d\n", clist_count(&single_key_list_empty));
#endif /* PSA_SINGLE_KEY_COUNT */
}

/**
 * @brief   Wipe key slot with correct key slot size
 *
 * @param   slot    Key sloit to be wiped
 */
static void psa_wipe_real_slot_type(psa_key_slot_t *slot)
{
    psa_key_attributes_t attr = slot->attr;

    if (!psa_key_lifetime_is_external(attr.lifetime)) {
        if (!PSA_KEY_TYPE_IS_KEY_PAIR(attr.type)) {
            memset(slot, 0, sizeof(psa_key_slot_t));
        }
#if PSA_ASYMMETRIC_KEYPAIR_COUNT
        else {

            memset((psa_key_pair_slot_t *)slot, 0, sizeof(psa_key_pair_slot_t));
        }
#endif
    }
#if PSA_PROTECTED_KEY_COUNT
    else {
        memset((psa_prot_key_slot_t *)slot, 0, sizeof(psa_prot_key_slot_t));
    }
#endif
}

psa_status_t psa_wipe_key_slot(psa_key_slot_t *slot)
{
    /* Get list the slot is stored in */
    clist_node_t *empty_list = psa_get_empty_key_slot_list(&slot->attr);

    if (empty_list == NULL) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Get node to remove from key slot list */
    clist_node_t *n = clist_remove(&key_slot_list, &slot->node);

    if (n == NULL) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    psa_key_slot_t *tmp = container_of(n, psa_key_slot_t, node);

    /* Wipe slot associated with node */
    psa_wipe_real_slot_type(tmp);

    /* Append node to empty list for later reuse */
    clist_rpush(empty_list, n);
    return PSA_SUCCESS;
}

void psa_wipe_all_key_slots(void)
{
    /* Move all list items to empty lists */
    while (!clist_is_empty(&key_slot_list)) {
        clist_node_t *to_remove = clist_rpop(&key_slot_list);
        psa_key_slot_t *slot = container_of(to_remove, psa_key_slot_t, node);
        clist_node_t *empty_list = psa_get_empty_key_slot_list(&slot->attr);

        psa_wipe_real_slot_type(slot);
        clist_rpush(empty_list, to_remove);
    }
}

/**
 * @brief   Check whether a given key ID equals the ID of the given node.
 *
 *          This is the break condition of the @ref clist_foreach function used
 *          in @ref psa_get_and_lock_key_slot_in_memory.
 *          clist_foreach iterates over all key slots and calls this function
 *          to compare each slot's key ID to the given input key ID.
 *          If they are equal, this function returns True and breaks the foreach
 *          loop.
 *
 *          For more information see core/lib/include/clist.h.
 *
 * @param   n   Pointer to clist node referencing a key slot
 * @param   arg Pointer to required key ID
 * @return  int
 *          1 if input ID equals node ID
 *          0 otherwise
 */
static int node_id_equals_key_id(clist_node_t *n, void *arg)
{
    psa_key_slot_t *slot = container_of(n, psa_key_slot_t, node);
    psa_key_id_t id = *((psa_key_id_t *)arg);

    if (slot->attr.id == id) {
        return 1;
    }

    return 0;
}

/**
 * @brief   Find the key slot containing the key with a specified ID
 *
 * @param   id      ID of the required key
 * @param   p_slot  Pointer to the slot that will contain the required key
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_INVALID_HANDLE
 *          @ref PSA_ERROR_CORRUPTION_DETECTED
 *          @ref PSA_ERROR_NOT_SUPPORTED
 */
static psa_status_t psa_get_and_lock_key_slot_in_memory(psa_key_id_t id, psa_key_slot_t **p_slot)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if (psa_key_id_is_volatile(id)) {
        clist_node_t *slot_node = clist_foreach(&key_slot_list, node_id_equals_key_id, &id);
        if (slot_node == NULL) {
            return PSA_ERROR_INVALID_HANDLE;
        }

        psa_key_slot_t *slot = container_of(slot_node, psa_key_slot_t, node);
        status = psa_lock_key_slot(slot);
        if (status == PSA_SUCCESS) {
            *p_slot = slot;
        }
        return status;
    }

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_get_and_lock_key_slot(psa_key_id_t id, psa_key_slot_t **p_slot)
{
    /* TODO validate ID */

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    *p_slot = NULL;

    status = psa_get_and_lock_key_slot_in_memory(id, p_slot);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        return status;
    }

    /* TODO: get persistent key from storage and load into slot */

    return status;
}

/**
 * @brief   Allocate a free slot for a new key creation
 *
 * @param   p_slot  Pointer that will contain the free key slot.
 * @param   attr    Attributes of type @ref psa_key_attrubutes_t for the key to be created
 *
 * @return  @ref PSA_SUCCESS
 *          @ref PSA_ERROR_DOES_NOT_EXIST   No key slots for this type of key exist
 *          @ref PSA_ERROR_INSUFFICIENT_STORAGE
 */
static psa_status_t psa_allocate_key_slot_in_list(psa_key_slot_t **p_slot,
                                                  const psa_key_attributes_t *attr)
{
    clist_node_t *empty_list = psa_get_empty_key_slot_list(attr);

    if (empty_list == NULL) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Check if any empty elements of this key slot type are left */
    if (clist_is_empty(empty_list)) {
        DEBUG("Key Slot MGMT: No PSA Key Slot available\n");
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }

    /* TODO: If no slots left: Look for slot in list with persistent key
       (key will be stored in persistent memory and slot can be reused) */

    /* Remove key slote node from empty list and append to actual list */
    clist_node_t *new_slot = clist_rpop(empty_list);

    clist_rpush(&key_slot_list, new_slot);

    psa_key_slot_t *slot = container_of(new_slot, psa_key_slot_t, node);

    *p_slot = slot;
    return PSA_SUCCESS;
}

psa_status_t psa_allocate_empty_key_slot(   psa_key_id_t *id,
                                            const psa_key_attributes_t *attr,
                                            psa_key_slot_t **p_slot)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *new_slot = NULL;

    /* Change later, when we also have persistent keys */
    if (key_id_count == PSA_KEY_ID_VOLATILE_MAX) {
        DEBUG("Key Slot MGMT: Maximum key ID reached\n");
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }

    status = psa_allocate_key_slot_in_list(&new_slot, attr);
    if (status != PSA_SUCCESS) {
        *p_slot = NULL;
        *id = 0;
        return status;
    }

    if (new_slot != NULL) {
        status = psa_lock_key_slot(new_slot);
        if (status != PSA_SUCCESS) {
            *p_slot = NULL;
            *id = 0;
            return status;
        }
        *id = key_id_count++;
        *p_slot = new_slot;

        return PSA_SUCCESS;
    }

    status = PSA_ERROR_INSUFFICIENT_MEMORY;
    *p_slot = NULL;
    *id = 0;
    return status;
}

psa_status_t psa_lock_key_slot(psa_key_slot_t *slot)
{
    if (slot->lock_count >= SIZE_MAX) {
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

    slot->lock_count++;

    return PSA_SUCCESS;
}

psa_status_t psa_unlock_key_slot(psa_key_slot_t *slot)
{
    if (slot == NULL) {
        return PSA_SUCCESS;
    }

    if (slot->lock_count > 0) {
        slot->lock_count--;
        return PSA_SUCCESS;
    }

    return PSA_ERROR_CORRUPTION_DETECTED;
}

psa_status_t psa_validate_key_location(psa_key_lifetime_t lifetime, psa_se_drv_data_t **p_drv)
{
    if (psa_key_lifetime_is_external(lifetime)) {
#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
        psa_se_drv_data_t *driver = psa_get_se_driver_data(lifetime);
        if (driver != NULL) {
            if (p_drv != NULL) {
                *p_drv = driver;
            }
            return PSA_SUCCESS;
        }
#else
        (void)p_drv;
#endif /* MODULE_PSA_SECURE_ELEMENT */
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    else {
        (void)p_drv;
        return PSA_SUCCESS;
    }
}

psa_status_t psa_validate_key_persistence(psa_key_lifetime_t lifetime)
{
    if (PSA_KEY_LIFETIME_IS_VOLATILE(lifetime)) {
        return PSA_SUCCESS;
    }
    /* TODO: Implement persistent key storage */
    return PSA_ERROR_NOT_SUPPORTED;
}

int psa_is_valid_key_id(psa_key_id_t id, int vendor)
{
    if ((PSA_KEY_ID_USER_MIN <= id) &&
        (id <= PSA_KEY_ID_USER_MAX)) {
        return 1;
    }

    if (vendor
        && (PSA_KEY_ID_VENDOR_MIN <= id)
        && (id <= PSA_KEY_ID_VENDOR_MAX)) {
        return 1;
    }

    return 0;
}

size_t psa_get_key_data_from_key_slot(const psa_key_slot_t *slot, uint8_t **key_data,
                                    size_t **key_bytes)
{
    psa_key_attributes_t attr = slot->attr;
    size_t key_data_size = 0;

    *key_data = NULL;
    *key_bytes = NULL;


    if (!psa_key_lifetime_is_external(attr.lifetime)) {
        if (!PSA_KEY_TYPE_IS_KEY_PAIR(attr.type)) {
#if PSA_SINGLE_KEY_COUNT
            *key_data = (uint8_t *)slot->key.data;
            *key_bytes = (size_t *)&slot->key.data_len;
            key_data_size = sizeof(slot->key.data);
#endif /* PSA_SINGLE_KEY_COUNT */
        }
        else {
#if PSA_ASYMMETRIC_KEYPAIR_COUNT
            *key_data = ((psa_key_pair_slot_t *)slot)->key.privkey_data;
            *key_bytes = &((psa_key_pair_slot_t *)slot)->key.privkey_data_len;
            key_data_size = sizeof(((psa_key_pair_slot_t *)slot)->key.privkey_data);
#endif
        }
    }
    return key_data_size;
}

#if IS_USED(MODULE_PSA_SECURE_ELEMENT) && PSA_PROTECTED_KEY_COUNT
psa_key_slot_number_t * psa_key_slot_get_slot_number(const psa_key_slot_t *slot)
{
    return &(((psa_prot_key_slot_t *)slot)->key.slot_number);
}
#endif

void psa_get_public_key_data_from_key_slot(const psa_key_slot_t *slot, uint8_t **pubkey_data,
                                           size_t **pubkey_data_len)
{
    psa_key_attributes_t attr = slot->attr;

    /* If key type is not asymmetric, no public key exists */
    if (!PSA_KEY_TYPE_IS_ASYMMETRIC(attr.type)) {
        *pubkey_data = NULL;
        *pubkey_data_len = NULL;
        return;
    }

    if (!psa_key_lifetime_is_external(attr.lifetime)) {
        if (!PSA_KEY_TYPE_IS_KEY_PAIR(attr.type)) {
#if PSA_SINGLE_KEY_COUNT
            *pubkey_data = ((psa_key_slot_t *)slot)->key.data;
            *pubkey_data_len = &((psa_key_slot_t *)slot)->key.data_len;
            return;
#endif /* PSA_SINGLE_KEY_COUNT */
        }
        else {
#if PSA_ASYMMETRIC_KEYPAIR_COUNT
            *pubkey_data = ((psa_key_pair_slot_t *)slot)->key.pubkey_data;
            *pubkey_data_len = &((psa_key_pair_slot_t *)slot)->key.pubkey_data_len;
            return;
#endif
        }
    }
#if PSA_PROTECTED_KEY_COUNT && IS_USED(MODULE_PSA_ASYMMETRIC)
    *pubkey_data = ((psa_prot_key_slot_t *)slot)->key.pubkey_data;
    *pubkey_data_len = &((psa_prot_key_slot_t *)slot)->key.pubkey_data_len;
#endif
}
