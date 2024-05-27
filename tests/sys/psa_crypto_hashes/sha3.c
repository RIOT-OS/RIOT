#include "psa/crypto.h"

psa_status_t sha_3_glue_test(void) {
    psa_hashes_sha3_ctx_t handle_glue;
    keccak_state_t handle_original;
    static unsigned char hash_glue_256[SHA3_256_DIGEST_LENGTH];
    static unsigned char hash_original_256[SHA3_256_DIGEST_LENGTH];

    static const uint8_t input[] = { 0x20, 0x24, 0x84, 0x51, 0x25, 0x26 };
    size_t input_len = sizeof(input);

    sha3_256_init(&handle_original);
    sha3_update(&handle_original, input, input_len);
    sha3_256_final(&handle_original, hash_original_256);

    psa_status_t status = psa_hashes_sha3_256_setup(&handle_glue);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    status = psa_hashes_sha3_update(&handle_glue, input, input_len);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_256_finish(&handle_glue, hash_glue_256);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    // check if glue hash and expected hash are the same
    if( memcmp(hash_glue_256, hash_original_256, SHA3_256_DIGEST_LENGTH) != 0 )
        return PSA_ERROR_DATA_INVALID;

    // reset handles
    memset(&handle_original, 0, sizeof(handle_original));    
    memset(&handle_glue, 0, sizeof(handle_glue));
    static unsigned char hash_glue_384[SHA3_384_DIGEST_LENGTH];
    static unsigned char hash_original_384[SHA3_384_DIGEST_LENGTH];
    
    sha3_384_init(&handle_original);
    sha3_update(&handle_original, input, input_len);
    sha3_384_final(&handle_original, hash_original_384);

    status = psa_hashes_sha3_384_setup(&handle_glue);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_update(&handle_glue, input, input_len);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_384_finish(&handle_glue, hash_glue_384);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    // check if glue hash and expected hash are the same
    if( memcmp(hash_glue_384, hash_original_384, SHA3_384_DIGEST_LENGTH) != 0 )
        return PSA_ERROR_DATA_INVALID;
    
    // reset handles
    memset(&handle_original, 0, sizeof(handle_original));    
    memset(&handle_glue, 0, sizeof(handle_glue));
    static unsigned char hash_glue_512[SHA3_512_DIGEST_LENGTH];
    static unsigned char hash_original_512[SHA3_512_DIGEST_LENGTH];

    sha3_512_init(&handle_original);
    sha3_update(&handle_original, input, input_len);
    sha3_512_final(&handle_original, hash_original_512);

    status = psa_hashes_sha3_512_setup(&handle_glue);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;
    
    status = psa_hashes_sha3_update(&handle_glue, input, input_len);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    status = psa_hashes_sha3_512_finish(&handle_glue, hash_glue_512);
    if(status != PSA_SUCCESS)
        return PSA_ERROR_BAD_STATE;

    // check if glue hash and expected hash are the same
    if( memcmp(hash_glue_512, hash_original_512, SHA3_512_DIGEST_LENGTH) != 0 )
        return PSA_ERROR_DATA_INVALID;

    return PSA_SUCCESS;

}
