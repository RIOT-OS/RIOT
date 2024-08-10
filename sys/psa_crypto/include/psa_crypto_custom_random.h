/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @defgroup       <name> <description>
 * @{
 *
 * @file        psa_custom_random.h
 * @brief
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

psa_status_t psa_custom_generate_random(uint8_t *output, size_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CUSTOM_RANDOM_H */
/** @} */
