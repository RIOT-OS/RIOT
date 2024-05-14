#include "psa/crypto.h"

psa_status_t sha_3_glue_test(void) {
    psa_status_t status = psa_hashes_sha3_256_setup(psa_hashes_sha3_ctx_t *ctx);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    status = psa_hashes_sha3_update(psa_hashes_sha3_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_256_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    psa_status_t status = psa_hashes_sha3_384_setup(psa_hashes_sha3_ctx_t *ctx);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    status = psa_hashes_sha3_update(psa_hashes_sha3_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_384_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    psa_status_t status = psa_hashes_sha3_512_setup(psa_hashes_sha3_ctx_t *ctx);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    status = psa_hashes_sha3_update(psa_hashes_sha3_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_512_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    return PSA_SUCCESS;

}
