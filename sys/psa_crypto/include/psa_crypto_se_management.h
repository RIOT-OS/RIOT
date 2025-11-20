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
 * @defgroup    psa_crypto_se_mgmt  PSA Crypto SE Management
 * @{
 *
 * @file        psa_crypto_se_management.h
 * @brief       PSA Secure Element management function declarations
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "psa_crypto_se_driver.h"

/**
 * @brief   Maximum number of available secure elements.
 */
#ifndef CONFIG_PSA_MAX_SE_COUNT
#define CONFIG_PSA_MAX_SE_COUNT     (1)
#endif /* CONFIG_PSA_MAX_SE_COUNT */

/**
 * @brief Maximum supported number of secure elements
 */
#define PSA_MAX_SE_COUNT    (CONFIG_PSA_MAX_SE_COUNT)

/**
 * @brief   Internal secure element driver context.
 *
 * @details This is the same structure as @ref psa_drv_se_context_t, with the difference that it is
 *          also writeable for the implementation.
 *
 *          This structure is not to be used by applications, only by the PSA Crypto implementation.
 */
typedef struct {
    void * persistent_data;  /**< Driver specific persistent data */
    size_t persistent_data_size;    /**< Size of persistent data in bytes */
    uintptr_t transient_data;       /**< Driver specific transient data */
} psa_drv_se_internal_context_t;

/**
 * @brief   Structure containing secure element driver data and contexts.
 */
struct psa_se_drv_data_s {
    psa_key_location_t location;    /**< Location value assigned to driver */
    const psa_drv_se_t *methods;    /**< Methods implemented by driver */
    union {
        psa_drv_se_internal_context_t internal; /**< Internally writable SE driver context */
        psa_drv_se_context_t context;           /**< SE driver context, read only */
    } ctx;                                        /**< SE driver context */
};

/**
 * @brief   Encodes the secure element driver data
 */
typedef struct psa_se_drv_data_s psa_se_drv_data_t;

/**
 * @brief   Register a secure element driver with the SE management.
 *
 * @details This function is called by the @c auto_init module during boot.
 *
 * @param   location                Location the driver should be registered with,
 *                                  of type @ref psa_key_location_t
 * @param   methods                 Structure of available driver entry points of the driver
 * @param   psa_se_configuration    Pointer to a secure element configuration structure
 * @param   drv_transient_data      Transient driver data to be used by the driver
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_INVALID_ARGUMENT The location value is invalid
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 * @return  @ref PSA_ERROR_INSUFFICIENT_MEMORY
 * @return  @ref PSA_ERROR_ALREADY_EXISTS *
 */
psa_status_t psa_register_secure_element(psa_key_location_t location,
                                         const psa_drv_se_t *methods,
                                         void *psa_se_configuration,
                                         const void *drv_transient_data);

/**
 * @brief   Get the driver data of a specified driver
 *
 * @param   lifetime    Lifetime value of type @ref psa_key_lifetime_t of the key to be used
 *
 * @return  @ref psa_se_drv_data_t*     Pointer to the driver data
 * @return  @c NULL if no driver exists with this location
 */
psa_se_drv_data_t *psa_get_se_driver_data(psa_key_lifetime_t lifetime);

/**
 * @brief   Get the driver entry points and context of a specified driver
 *
 * @param   lifetime        Lifetime value of type @ref psa_key_lifetime_t of the key to be used
 * @param   p_methods       Pointer that will reference the driver methods
 * @param   p_drv_context   Pointer that will reference the driver context
 *
 * @return  1 if a driver was found
 * @return  0 if no driver exists with this location
 */
int psa_get_se_driver(  psa_key_lifetime_t lifetime,
                        const psa_drv_se_t **p_methods,
                        psa_drv_se_context_t **p_drv_context);

/**
 * @brief   Get the driver entry points of a specified driver
 *
 * @param   driver  Driver data of type @ref psa_se_drv_data_t containing the entry points
 *
 * @return  const psa_drv_se_t*
 */
const psa_drv_se_t *psa_get_se_driver_methods(const psa_se_drv_data_t *driver);

/**
 * @brief   Get the driver context of a specified driver
 *
 * @param   driver  Driver data of type @ref psa_se_drv_data_t containing the context
 *
 * @return  @ref psa_drv_se_context_t*
 */
psa_drv_se_context_t *psa_get_se_drv_context(psa_se_drv_data_t *driver);

/**
 * @brief   Find an empty key slot on a secure element appropriate to the key attributes
 *
 * @param   attributes  @ref psa_key_attributes_t containing the attributes of the key to be created
 * @param   method      The method used to create the key (see @ref psa_key_creation_method_t)
 * @param   driver      Pointer to the driver for the SE the key should be created on
 * @param   slot_number Pointer that will contain the slot number of the free SE slot
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_CORRUPTION_DETECTED
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_find_free_se_slot( const psa_key_attributes_t *attributes,
                                    psa_key_creation_method_t method,
                                    psa_se_drv_data_t *driver,
                                    psa_key_slot_number_t *slot_number);

/**
 * @brief   Destroy the key on a secure element
 *
 * @note    Some secure elements may not support this operation.
 *
 * @param   driver      Driver of the SE containing the key to be destroyed
 * @param   slot_number Slot number of the key that is to be destroyed
 *
 * @return  @ref PSA_SUCCESS
 * @return  @ref PSA_ERROR_NOT_PERMITTED
 */
psa_status_t psa_destroy_se_key(psa_se_drv_data_t *driver,
                                psa_key_slot_number_t slot_number);

/**
 * @brief   Load SE data from persistent memory
 *
 * @note    This operation is not yet supported by this implementation
 *
 * @param   driver  Pointer to the driver data the loaded data should be stored in
 *
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_load_se_persistent_data(const psa_se_drv_data_t *driver);

/**
 * @brief   Save SE data to persistent memory
 *
 * @note    This operation is not yet supported by this implementation
 *
 * @param   driver  Pointer to the driver data containing the data to be saved
 *
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_save_se_persistent_data(const psa_se_drv_data_t *driver);

/**
 * @brief   Destroy SE data in persistent memory
 *
 * @note    This operation is not yet supported by this implementation
 *
 * @param   location  Location of the data that should be destroyed
 *
 * @return  @ref PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t psa_destroy_se_persistent_data(psa_key_location_t location);

#ifdef __cplusplus
}
#endif

/**@}*/
