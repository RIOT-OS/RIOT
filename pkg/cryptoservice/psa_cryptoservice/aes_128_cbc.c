#include "psa/crypto.h"

#include "CYS/unprotected.h"

psa_status_t psa_cipher_cbc_aes_128_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    (void)key_buffer_size;

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_cipher_operation_t operation = psa_cipher_operation_init();
    size_t iv_length = 0;
    size_t required_output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                        PSA_ALG_CBC_NO_PADDING, input_length);

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    operation.iv_required = 1;
    operation.default_iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);
    *output_length = 0;

    status = psa_cipher_generate_iv(&operation, output, operation.default_iv_length, &iv_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = CYS_aes_128_cbc_encrypt(key_buffer, output, input, input_length, output + iv_length);
    *output_length = (status == PSA_SUCCESS) ? input_length + iv_length : 0;

    return status;
}

psa_status_t psa_cipher_cbc_aes_128_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    (void)key_buffer_size;

    size_t required_output_buf_size = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                        PSA_ALG_CBC_NO_PADDING, input_length);
    size_t iv_length = PSA_CIPHER_IV_LENGTH(attributes->type, alg);

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status = CYS_aes_128_cbc_decrypt(key_buffer, input, input + iv_length, input_length - iv_length, output);
    *output_length = (status == PSA_SUCCESS) ? input_length - iv_length : 0;

    return status;
}
