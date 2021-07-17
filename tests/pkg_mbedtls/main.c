/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the correct loading and linking of the mbedtls package and
                execute built-in selftests.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"

#include "entropy_mbedtls_riot.h"

int main(void)
{
    puts("mbedtls test\n");

    /* Execute built-in tests */
    mbedtls_sha256_self_test(1);
    mbedtls_entropy_self_test(1);

    /* Execute convenience functions */
    unsigned char out[4];
    entropy_mbedtls_riot_init();
    entropy_mbedtls_riot_retrieve(out, sizeof(out));
    entropy_mbedtls_riot_uninit();

    entropy_mbedtls_riot_get(out, sizeof(out));

    return 0;
}
