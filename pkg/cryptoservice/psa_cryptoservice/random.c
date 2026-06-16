/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "psa/crypto.h"
#include "CYS/unprotected.h"

psa_status_t psa_custom_generate_random(uint8_t *output, size_t output_size)
{
    if (!output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return CYS_random_generate(output, output_size);
}
