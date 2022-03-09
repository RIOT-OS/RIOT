/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Calls some of the unit tests from the CryptoAuth Library
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "cryptoauthlib_test.h"

int main(void)
{
    /* Set device to ATECC508A */
    if (ATCA_DEVTYPE == ATECC608A) {
        atca_run_cmd("608");
    }
    else if (ATCA_DEVTYPE == ATECC508A) {
        atca_run_cmd("508");
    }
    else {
        printf("This device is currently not supported.");
        return 0;
    }

    atca_run_cmd("unit");

    return 0;
}
