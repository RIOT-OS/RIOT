#include "psa/crypto.h"
#include "psa_crypto_se_driver.h"
#include "psa_crypto_se_management.h"

#include "CYS/protected_key.h"

#include <stdbool.h>
#include <stdint.h>

#define SLOT_COUNT CONFIG_PSA_SEALED_KEY_COUNT

static CYS_PROT_p256_key_t slots[SLOT_COUNT];
static bool slots_used[SLOT_COUNT];

static psa_status_t palloc(psa_drv_se_context_t *drv_context, void *persistent_data,
        const psa_key_attributes_t *attributes, psa_key_creation_method_t method,
        psa_key_slot_number_t *key_slot) {

    (void)drv_context;
    (void)persistent_data;
    (void)attributes;
    (void)method;

    int slot_idx = -1;
    for(size_t i = 0; i < SLOT_COUNT; i++) {
        if(!slots_used[i]) {
            slot_idx = i;
            break;
        }
    }

    if(slot_idx < 0) {
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }

    slots_used[slot_idx] = true;
    *key_slot = slot_idx;
    return PSA_SUCCESS;
}

static psa_status_t destroy(psa_drv_se_context_t *drv_context,
    void *persistent_data, psa_key_slot_number_t key_slot) {

    (void)drv_context;
    (void)persistent_data;

    if(key_slot >= SLOT_COUNT) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    slots_used[key_slot] = false;
    return PSA_SUCCESS;
}

static psa_status_t generate(psa_drv_se_context_t *drv_context, psa_key_slot_number_t key_slot,
        const psa_key_attributes_t *attributes, uint8_t *pubkey, size_t pubkey_size,
        size_t *pubkey_length) {

    (void)drv_context;
    (void)attributes;
    (void)pubkey_size;

    if(key_slot >= SLOT_COUNT) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* TODO: pubkey_size is always zero */

    psa_status_t status;
    status = CYS_PROT_p256_generate(&slots[key_slot], pubkey);
    if(status != PSA_SUCCESS) {
        return status;
    }

    *pubkey_length = CYS_PROT_P256_PUB_SIZE;

    return PSA_SUCCESS;
}

static psa_status_t sign(psa_drv_se_context_t *drv_context, psa_key_slot_number_t key_slot,
        psa_algorithm_t alg, const uint8_t *p_hash, size_t hash_length, uint8_t *p_signature,
        size_t signature_size, size_t *p_signature_length) {

    (void)drv_context;
    (void)alg;
    (void)signature_size;

    if(signature_size < CYS_PROT_P256_SIG_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if(key_slot >= SLOT_COUNT) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    psa_status_t status;
    status = CYS_PROT_p256_sign(&slots[key_slot], p_hash, hash_length, p_signature);
    if(status != PSA_SUCCESS) {
        return status;
    }

    *p_signature_length = CYS_PROT_P256_SIG_SIZE;

    return PSA_SUCCESS;
}

static psa_drv_se_key_management_t drv_se_key_mgmt = {
    .p_allocate = &palloc,
    .p_generate = &generate,
    .p_destroy = &destroy
};

static psa_drv_se_asymmetric_t drv_se_asym = {
    .p_sign = &sign
};

static psa_drv_se_t drv_se = {
    .hal_version = PSA_DRV_SE_HAL_VERSION,
    .persistent_data_size = 0,
    .key_management = &drv_se_key_mgmt,
    .asymmetric = &drv_se_asym
};

void auto_init_cryptoservice(void) {
    psa_crypto_init();
    psa_register_secure_element(PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT, &drv_se, NULL, NULL);
}
