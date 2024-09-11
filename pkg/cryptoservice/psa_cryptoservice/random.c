#include "psa/crypto.h"
#include "CYS/unprotected.h"

psa_status_t psa_custom_generate_random(uint8_t *output, size_t output_size, size_t *output_length)
{
    if (!output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return CYS_random_generate(output, output_size, output_length);
}
