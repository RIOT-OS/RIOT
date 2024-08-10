#include "psa/crypto.h"
#include "tee_random.h"

psa_status_t psa_custom_generate_random(uint8_t *output, size_t output_size)
{
    if (!output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* TODO: Should point to a CSPRNG API in the future */
    return tee_generate_random(output, output_size);
}
