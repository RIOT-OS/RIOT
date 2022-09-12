/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto psa_crypto_se_mgmt
 * @{
 *
 * @brief       PSA Crypto Secure Element Management implementation
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_crypto_se_management.h"
#include "psa_crypto_se_driver.h"

/**
 * @brief   Array containing the registered driver instances.
 */
static psa_se_drv_data_t driver_table[PSA_MAX_SE_COUNT];

psa_status_t psa_register_secure_element(psa_key_location_t location,
                                         const psa_drv_se_t *methods,
                                         void *drv_data)
{
    size_t i;
    psa_se_drv_data_t *temp;

    if (methods->hal_version != PSA_DRV_SE_HAL_VERSION) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (location == PSA_KEY_LOCATION_LOCAL_STORAGE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (location > PSA_KEY_LOCATION_SE_MAX) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (methods->persistent_data_size > PSA_MAX_PERSISTENT_DATA_SIZE) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Find next free slot in driver table */
    for (i = 0; i < PSA_MAX_SE_COUNT; i++) {
        if (driver_table[i].location == location) {
            return PSA_ERROR_ALREADY_EXISTS;
        }

        if (driver_table[i].location == 0) {
            break;
        }
    }

    if (i == PSA_MAX_SE_COUNT) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    temp = &driver_table[i];
    temp->location = location;
    temp->methods = methods;
    temp->u.internal.transient_data = (uintptr_t)drv_data;
    temp->u.internal.persistent_data_size = methods->persistent_data_size;

    /* TODO: Load Persistent data if persistent_data_size != 0 */

    return PSA_SUCCESS;
}

psa_se_drv_data_t *psa_get_se_driver_data(psa_key_lifetime_t lifetime)
{
    psa_se_drv_data_t *drv = NULL;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(lifetime);

    if (location == 0) {
        return NULL;
    }

    for (size_t i = 0; i < PSA_MAX_SE_COUNT; i++) {
        if (driver_table[i].location == location) {
            drv = &driver_table[i];
        }
    }

    (void)lifetime;
    return drv;
}

int psa_get_se_driver(psa_key_lifetime_t lifetime,
                      const psa_drv_se_t **p_methods,
                      psa_drv_se_context_t **p_drv_context)
{
    psa_se_drv_data_t *driver = psa_get_se_driver_data(lifetime);

    if (p_methods != NULL) {
        *p_methods = (driver ? driver->methods : NULL);
    }
    if (p_drv_context != NULL) {
        *p_drv_context = (driver ? &driver->u.context : NULL);
    }
    return (driver != NULL);
}

const psa_drv_se_t *psa_get_se_driver_methods(const psa_se_drv_data_t *driver)
{
    return driver->methods;
}

psa_drv_se_context_t *psa_get_se_drv_context(psa_se_drv_data_t *driver)
{
    return &driver->u.context;
}

psa_status_t psa_find_free_se_slot(const psa_key_attributes_t *attributes,
                                   psa_key_creation_method_t method,
                                   psa_se_drv_data_t *driver,
                                   psa_key_slot_number_t *slot_number)
{
    psa_status_t status;
    psa_key_location_t key_location =
        PSA_KEY_LIFETIME_GET_LOCATION(psa_get_key_lifetime(attributes));

    if (driver->location != key_location) {
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

    if (driver->methods->key_management == NULL ||
        driver->methods->key_management->p_allocate == NULL) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = driver->methods->key_management->p_allocate(&driver->u.context,
                                                         driver->u.internal.persistent_data,
                                                         attributes, method, slot_number);

    return status;
}

psa_status_t psa_destroy_se_key(psa_se_drv_data_t *driver,
                                psa_key_slot_number_t slot_number)
{
    if (driver->methods->key_management == NULL ||
        driver->methods->key_management->p_destroy == NULL) {
        return PSA_ERROR_NOT_PERMITTED;
    }
    return driver->methods->key_management->p_destroy(&driver->u.context,
                                                      driver->u.internal.persistent_data,
                                                      slot_number);

    /* TODO: Store Persistent Data */
}

psa_status_t psa_load_se_persistent_data(const psa_se_drv_data_t *driver)
{
    (void)driver;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_save_se_persistent_data(const psa_se_drv_data_t *driver)
{
    (void)driver;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_destroy_se_persistent_data(psa_key_location_t location)
{
    (void)location;
    return PSA_ERROR_NOT_SUPPORTED;
}
