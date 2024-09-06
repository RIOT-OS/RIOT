#include "psa/crypto.h"

#include "CYS/unprotected.h"

psa_status_t psa_hashes_sha256_setup(psa_hashes_sha256_ctx_t *ctx)
{
    return CYS_sha256_init(ctx);
}

psa_status_t psa_hashes_sha256_update(psa_hashes_sha256_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    return CYS_sha256_update(ctx, input, input_length);
}

psa_status_t psa_hashes_sha256_finish(psa_hashes_sha256_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    if(hash_size < CYS_SHA256_DIGEST_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    psa_status_t status;
    status = CYS_sha256_finalize(ctx, hash);
    if(status != PSA_SUCCESS) {
        return status;
    }

    *hash_length = CYS_SHA256_DIGEST_SIZE;
    return PSA_SUCCESS;
}
