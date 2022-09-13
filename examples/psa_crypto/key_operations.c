/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @brief   Example functions for key import and random number generation.
 *
 * @author  Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

/**
 * @brief   Size of an AES 128 key in bytes
 */
#define AES_128_KEY_SIZE    (16)

/**
 * @brief   16 byte key to demonstrate key import.
 */
static const uint8_t KEY_128[] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

/**
 * @brief   Example for key import and destruction
 */
void key_operations(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    psa_key_id_t key_id = 0;
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT;

    psa_set_key_algorithm(&attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, 128);
    psa_set_key_type(&attr, PSA_KEY_TYPE_AES);

    status = psa_import_key(&attr, KEY_128, AES_128_KEY_SIZE, &key_id);
    if (status != PSA_SUCCESS) {
        printf("Key Import failed: %d\n", (int)status);
        return;
    }

    printf("PSA Key import done, Key ID: 0x%02x\n", (unsigned int)key_id);

    status = psa_destroy_key(key_id);
    if (status != PSA_SUCCESS) {
        printf("Key Destruction failed: %d\n", (int)status);
        return;
    }

    puts("PSA Key destruction done.");
}

/**
 * @brief   Example for random number generation
 */
void random_number_generation(void)
{
    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    uint8_t output_buffer[AES_128_KEY_SIZE] = { 0 };
    status = psa_generate_random(output_buffer, sizeof(output_buffer));
    if (status != PSA_SUCCESS) {
        printf("Random Number Generation failed: %d\n", (int)status);
        return;
    }

    puts("Random number generation done:\n");
    for (size_t i = 0; i < sizeof(output_buffer); i++) {
        printf("0x%02x ", output_buffer[i]);
    }
    puts("\n");
}
