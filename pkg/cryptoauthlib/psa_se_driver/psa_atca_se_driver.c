/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Glue code translating between PSA Crypto and the Microchip Cryptoauth Library APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "atca_params.h"
#include "psa/crypto.h"
#include "psa_crypto_se_driver.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief   AES block size supported by ATCA devices
 */
#define AES_128_BLOCK_SIZE      (16)

/**
 * @brief   AES key size supported by ATCA devices
 */
#define AES_128_KEY_SIZE        (16)

/**
 * @brief   Size of an ellipic curve public key supported by ATCA devices
 */
#define ECC_P256_PUB_KEY_SIZE   (64)

/**
 * @brief   Maximum IV length supported by ATCA devices
 */
#define ATCA_MAX_IV_LEN         (16)

/**
 * @brief   Check whether a specified algorithm is supported by this driver
 *
 * @param   alg Algorithm of type @ref psa_algorithm_t
 *
 * @return  int
 *          1 if @c alg is supported
 *          0 otherwise
 */
#define ALG_IS_SUPPORTED(alg)   \
    ((alg == PSA_ALG_ECB_NO_PADDING) || \
     (alg == PSA_ALG_CBC_NO_PADDING) || \
     (alg == PSA_ALG_ECDSA(PSA_ALG_SHA_256)) || \
     (alg == PSA_ALG_HMAC(PSA_ALG_SHA_256)))

/**
 * @brief   Check whether a specified key size is supported by this driver
 *
 * @param   size    Size of the specified key
 * @param   type    Type of the specified key
 *
 * @return  int
 *          1 if @c size is supported
 *          0 otherwise
 */
#define KEY_SIZE_IS_SUPPORTED(size, type) \
    ((type == PSA_KEY_TYPE_AES && size == AES_128_KEY_SIZE) || \
     (PSA_KEY_TYPE_IS_ECC(type) && size == (ECC_P256_PUB_KEY_SIZE + 1)) || \
     (type == PSA_KEY_TYPE_HMAC && size == 32))

/**
 * @brief   Convert ATCA status values to PSA errors
 *
 * @param   error
 *
 * @return  @ref psa_status_t
 */
static psa_status_t atca_to_psa_error(ATCA_STATUS error)
{
    switch (error) {
    case ATCA_NOT_LOCKED:
    case ATCA_EXECUTION_ERROR:
    case ATCA_FUNC_FAIL:
        return PSA_ERROR_BAD_STATE;
    case ATCA_WAKE_FAILED:
    case ATCA_RX_FAIL:
    case ATCA_RX_NO_RESPONSE:
    case ATCA_TX_TIMEOUT:
    case ATCA_RX_TIMEOUT:
    case ATCA_TOO_MANY_COMM_RETRIES:
    case ATCA_COMM_FAIL:
    case ATCA_TIMEOUT:
    case ATCA_TX_FAIL:
        return PSA_ERROR_COMMUNICATION_FAILURE;
    case ATCA_RX_CRC_ERROR:
    case ATCA_STATUS_CRC:
        return PSA_ERROR_DATA_CORRUPT;
    case ATCA_SMALL_BUFFER:
        return PSA_ERROR_BUFFER_TOO_SMALL;
    case ATCA_BAD_OPCODE:
    case ATCA_BAD_PARAM:
    case ATCA_INVALID_SIZE:
    case ATCA_INVALID_ID:
        return PSA_ERROR_INVALID_ARGUMENT;
    case ATCA_UNIMPLEMENTED:
        return PSA_ERROR_NOT_SUPPORTED;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}

/* Secure Element Cipher Functions */
/**
 * @brief   Set up a driver specific AES CBC mode operation
 *
 * @param   ctx
 * @param   dev
 * @param   key_slot
 */
static void atca_cbc_setup(atca_aes_cbc_ctx_t *ctx,
                           ATCADevice dev,
                           psa_key_slot_number_t key_slot)
{
    ctx->device = dev;
    ctx->key_id = key_slot;
    ctx->key_block = 0;
}

psa_status_t atca_cipher_setup(psa_drv_se_context_t *drv_context,
                               void *op_context,
                               psa_key_slot_number_t key_slot,
                               psa_algorithm_t algorithm,
                               psa_encrypt_or_decrypt_t direction)
{
    ATCADevice dev = (ATCADevice)drv_context->transient_data;
    psa_se_cipher_context_t *ctx = (psa_se_cipher_context_t *)op_context;

    /* Only device type ATECC608 supports AES operations */
    if (dev->mIface.mIfaceCFG->devtype != ATECC608) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* This implementation is for demonstration and currently only supports AES ECB encryption */
    if (!ALG_IS_SUPPORTED(algorithm)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    switch (algorithm) {
    case PSA_ALG_CBC_NO_PADDING:
        atca_cbc_setup(&ctx->drv_ctx.atca_aes_cbc, dev, key_slot);
        break;
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
    ctx->direction = direction;

    return PSA_SUCCESS;
}

psa_status_t atca_cipher_set_iv(void *op_context,
                                const uint8_t *p_iv,
                                size_t iv_length)
{

    atca_aes_cbc_ctx_t *ctx = &((psa_se_cipher_context_t *)op_context)->drv_ctx.atca_aes_cbc;

    if (iv_length != ATCA_MAX_IV_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(ctx->ciphertext, p_iv, iv_length);

    return PSA_SUCCESS;
}

psa_status_t atca_cipher_update(void *op_context,
                                const uint8_t *p_input,
                                size_t input_size,
                                uint8_t *p_output,
                                size_t output_size,
                                size_t *p_output_length)
{
    psa_se_cipher_context_t *ctx = (psa_se_cipher_context_t *)op_context;
    ATCA_STATUS status = ATCA_EXECUTION_ERROR;
    size_t input_offset = 0;
    size_t output_offset = 0;

    for (size_t data_block = 0; data_block < (input_size / AES_128_BLOCK_SIZE); data_block++) {
        output_offset += data_block * AES_128_BLOCK_SIZE;
        input_offset += data_block * AES_128_BLOCK_SIZE;
        if (ctx->direction == PSA_CRYPTO_DRIVER_ENCRYPT) {
            status = atcab_aes_cbc_encrypt_block(&ctx->drv_ctx.atca_aes_cbc, p_input + input_offset,
                                                 p_output + output_offset);
        }
        else {
            status = atcab_aes_cbc_decrypt_block(&ctx->drv_ctx.atca_aes_cbc, p_input + input_offset,
                                                 p_output + output_offset);
        }
    }

    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }

    *p_output_length += input_size;

    (void)output_size;
    return PSA_SUCCESS;
}

psa_status_t atca_cipher_finish(void *op_context,
                                uint8_t *p_output,
                                size_t output_size,
                                size_t *p_output_length)
{
    (void)op_context;
    (void)p_output;
    (void)output_size;
    (void)p_output_length;
    return PSA_SUCCESS;
}

psa_status_t atca_cipher_ecb(psa_drv_se_context_t *drv_context,
                             psa_key_slot_number_t key_slot,
                             psa_algorithm_t algorithm,
                             psa_encrypt_or_decrypt_t direction,
                             const uint8_t *p_input,
                             size_t input_size,
                             uint8_t *p_output,
                             size_t output_size)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;
    size_t offset;

    if (dev->mIface.mIfaceCFG->devtype != ATECC608) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (algorithm != PSA_ALG_ECB_NO_PADDING || direction != PSA_CRYPTO_DRIVER_ENCRYPT) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (input_size % AES_128_BLOCK_SIZE != 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    offset = 0;
    do {
        status = calib_aes_encrypt(dev, key_slot, 0, p_input + offset, p_output + offset);
        if (status != ATCA_SUCCESS) {
            DEBUG("ATCA Error: %d\n", status);
            return atca_to_psa_error(status);
        }

        offset += AES_128_BLOCK_SIZE;
    } while (offset < input_size);

    (void)output_size;
    return PSA_SUCCESS;
}

/* Secure Element Key Management Functions */

psa_status_t atca_allocate(psa_drv_se_context_t *drv_context,
                           void *persistent_data,
                           const psa_key_attributes_t *attributes,
                           psa_key_creation_method_t method,
                           psa_key_slot_number_t *key_slot)
{
    if (!ALG_IS_SUPPORTED(attributes->policy.alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* TODO: This function needs to be implemented correctly:
                - Read device config zone during registration, store info in persistent data
                - Read persistent data to find a free, correctly configured key slot
                - Return slot number and mark key slot as used
     */

    if (attributes->type == PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
        /* At the time of the implementation we are using an SE in which key slot 1
           is configured for ECC private keys, so we return key slot nr. 1 */
        *key_slot = (psa_key_slot_number_t)1;
    }
    else if (attributes->type == PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)) {
        /* Slots 9-14 on device are configured to hold public keys */
        *key_slot = (psa_key_slot_number_t)9;
    }
    else {
        /* Returns the device's TEMPKEY-Register ID for AES and HMAC key import.  */
        *key_slot = (psa_key_slot_number_t)ATCA_TEMPKEY_KEYID;
    }

    (void)drv_context;
    (void)persistent_data;
    (void)method;

    return PSA_SUCCESS;
}

psa_status_t atca_import(psa_drv_se_context_t *drv_context,
                         psa_key_slot_number_t key_slot,
                         const psa_key_attributes_t *attributes,
                         const uint8_t *data,
                         size_t data_length,
                         size_t *bits)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    if (!ALG_IS_SUPPORTED(attributes->policy.alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (!KEY_SIZE_IS_SUPPORTED(data_length, attributes->type)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (key_slot == ATCA_TEMPKEY_KEYID) {
        uint8_t buf_in[32] = { 0 };
        /* This implementation only uses the device's TEMPKEY Register for key import,
        which only accepts input sizes of 32 or 64 Bytes, so we copy a smaller key into
        a 32 Byte buffer that is padded with zeros */
        memcpy(buf_in, data, data_length);

        status = calib_nonce_load(dev, NONCE_MODE_TARGET_TEMPKEY, buf_in, sizeof(buf_in));
        if (status != ATCA_SUCCESS) {
            DEBUG("ATCA Error: %d\n", status);
            return atca_to_psa_error(status);
        }
        *bits = PSA_BYTES_TO_BITS(data_length);

        return PSA_SUCCESS;
    }
    else if (PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(attributes->type)) {

        status = calib_write_pubkey(dev, key_slot, data + 1);
        if (status != ATCA_SUCCESS) {
            DEBUG("ATCA Error: %d\n", status);
            return atca_to_psa_error(status);
        }
        *bits = PSA_BYTES_TO_BITS(data_length);

        return PSA_SUCCESS;
    }

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t atca_generate_key(psa_drv_se_context_t *drv_context,
                               psa_key_slot_number_t key_slot,
                               const psa_key_attributes_t *attributes,
                               uint8_t *pubkey, size_t pubkey_size, size_t *pubkey_length)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    if (!PSA_KEY_TYPE_IS_ECC(attributes->type)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (pubkey_size > PSA_EXPORT_PUBLIC_KEY_MAX_SIZE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (pubkey != NULL) {
        /* The driver already exports the public key, in the correct format
        (uncompressed binary). We can just write the key into the pubkey buffer.
        First byte is reserved for format encoding (set below) */
        status = calib_genkey(dev, key_slot, &pubkey[1]);
    }
    else {
        status = calib_genkey(dev, key_slot, NULL);
    }

    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }
    /* Set first byte of pubkey buffer to 0x04 to encode public key uncompressed binary format */
    pubkey[0] = 0x04;
    *pubkey_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);
    DEBUG("ATCA Pubkey Length: %d\n", *pubkey_length);
    return PSA_SUCCESS;
}

psa_status_t atca_export_public_key(psa_drv_se_context_t *drv_context,
                                    psa_key_slot_number_t key_slot,
                                    uint8_t *p_data,
                                    size_t data_size,
                                    size_t *p_data_length)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    if (data_size < ECC_P256_PUB_KEY_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    status = calib_get_pubkey(dev, key_slot, &p_data[1]);
    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }

    p_data[0] = 0x04;
    *p_data_length = ECC_P256_PUB_KEY_SIZE + 1;

    return PSA_SUCCESS;
}

psa_status_t atca_sign(psa_drv_se_context_t *drv_context,
                       psa_key_slot_number_t key_slot,
                       psa_algorithm_t alg,
                       const uint8_t *p_hash,
                       size_t hash_length,
                       uint8_t *p_signature,
                       size_t signature_size,
                       size_t *p_signature_length)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if ((signature_size != ECC_P256_PUB_KEY_SIZE) || (hash_length != 32)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = calib_sign(dev, key_slot, p_hash, p_signature);
    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }

    *p_signature_length = signature_size;
    return PSA_SUCCESS;
}

psa_status_t atca_verify(psa_drv_se_context_t *drv_context,
                         psa_key_slot_number_t key_slot,
                         psa_algorithm_t alg,
                         const uint8_t *p_hash,
                         size_t hash_length,
                         const uint8_t *p_signature,
                         size_t signature_length)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    bool is_verified;

    /* We only support the operation on public key, if they're stored on a device. */
    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if ((signature_length != ECC_P256_PUB_KEY_SIZE) || (hash_length != 32)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = calib_verify_stored(dev, p_hash, p_signature, key_slot, &is_verified);
    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }

    return is_verified ? PSA_SUCCESS : PSA_ERROR_INVALID_SIGNATURE;
}

psa_status_t atca_generate_mac(psa_drv_se_context_t *drv_context,
                               const uint8_t *p_input,
                               size_t input_length,
                               psa_key_slot_number_t key_slot,
                               psa_algorithm_t alg,
                               uint8_t *p_mac,
                               size_t mac_size,
                               size_t *p_mac_length)
{
    ATCA_STATUS status;
    ATCADevice dev = (ATCADevice)drv_context->transient_data;

    DEBUG("ATCA_SE_DRIVER\n");
    if (!PSA_ALG_IS_HMAC(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (mac_size < 32) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    status = calib_sha_hmac(dev, p_input, input_length, key_slot, p_mac, SHA_MODE_TARGET_OUT_ONLY);
    if (status != ATCA_SUCCESS) {
        DEBUG("ATCA Error: %d\n", status);
        return atca_to_psa_error(status);
    }
    *p_mac_length = 32;

    return PSA_SUCCESS;
}

static psa_drv_se_mac_t atca_mac = {
    .context_size = 0,
    .p_setup = NULL,
    .p_update = NULL,
    .p_finish = NULL,
    .p_finish_verify = NULL,
    .p_abort = NULL,
    .p_mac = atca_generate_mac,
    .p_mac_verify = NULL,
};

static psa_drv_se_cipher_t atca_cipher = {
    .context_size = 0,
    .p_setup = atca_cipher_setup,
    .p_set_iv = atca_cipher_set_iv,
    .p_update = atca_cipher_update,
    .p_finish = atca_cipher_finish,
    .p_abort = NULL,
    .p_ecb = atca_cipher_ecb
};

static psa_drv_se_key_management_t atca_key_management = {
    .p_allocate = atca_allocate,
    .p_validate_slot_number = NULL,
    .p_import = atca_import,
    .p_generate = atca_generate_key,
    .p_destroy = NULL,
    .p_export = NULL,
    .p_export_public = atca_export_public_key
};

static psa_drv_se_asymmetric_t atca_asymmetric = {
    .p_sign = atca_sign,
    .p_verify = atca_verify,
    .p_encrypt = NULL,
    .p_decrypt = NULL
};

psa_drv_se_t atca_methods = {
    .hal_version = PSA_DRV_SE_HAL_VERSION,
    .persistent_data_size = 0,
    .p_init = NULL,
    .key_management = &atca_key_management,
    .mac = &atca_mac,
    .cipher = &atca_cipher,
    .aead = NULL,
    .asymmetric = &atca_asymmetric,
    .derivation = NULL
};
