#include "psa/crypto.h"
#include "hashes/psa/riot_hashes.h"

psa_status_t psa_mac_compute_hmac_sha256(   const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *mac,
                                            size_t mac_size,
                                            size_t *mac_length)
{
    hmac_sha256(key_buffer, key_buffer_size, input, input_length, mac);
    *mac_length = 32;

    (void)mac_size;
    (void)attributes;
    return PSA_SUCCESS;
}
