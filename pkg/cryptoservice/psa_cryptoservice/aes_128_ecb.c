#include "psa/crypto.h"

#include "CYS/unprotected.h"

psa_status_t psa_cipher_ecb_aes_128_encrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    (void)attributes;
    (void)key_buffer_size;
    (void)alg;

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    size_t required_output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                        PSA_ALG_ECB_NO_PADDING, input_length);

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    status = CYS_aes_128_ecb_encrypt(key_buffer, input, input_length, output);
    *output_length = (status == PSA_SUCCESS) ? input_length : 0;

    return status;
}

psa_status_t psa_cipher_ecb_aes_128_decrypt(const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            psa_algorithm_t alg,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *output,
                                            size_t output_size,
                                            size_t *output_length)
{
    (void)attributes;
    (void)key_buffer_size;
    (void)alg;

    size_t required_output_buf_size = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES,
                                        PSA_ALG_ECB_NO_PADDING, input_length);

    if (output_size < required_output_buf_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status = CYS_aes_128_ecb_decrypt(key_buffer, input, input_length, output);
    *output_length = (status == PSA_SUCCESS) ? input_length : 0;

    return status;
}
