/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_custom_random PSA Crypto Custom Random
 * @{
 *
 * @file        psa_crypto_custom_random.h
 * @brief       Function declarations for the PSA Crypto custom random number generator.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CUSTOM_RANDOM_H
#define PSA_CUSTOM_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief   Generate random bytes using a custom (backend-specific) RNG.
 *
 * @param[out]  output       Buffer to write the generated random bytes to
 * @param       output_size  Number of random bytes to generate
 *
 * @return  @ref psa_status_t
 */
psa_status_t psa_custom_generate_random(uint8_t *output, size_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CUSTOM_RANDOM_H */
/** @} */
