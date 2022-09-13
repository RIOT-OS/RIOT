/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @brief       Example application for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"

extern void key_operations(void);
extern void random_number_generation(void);

int main(void)
{
    psa_crypto_init();

    key_operations();
    random_number_generation();

    puts("All Done");
    return 0;
}
