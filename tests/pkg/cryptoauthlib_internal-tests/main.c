/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "atca_test.h"

const char atecc508[] = "ecc508";
const char atecc608[] = "ecc608";
const char unit[] = "basic";

int main(void)
{
    atca_test_config_set_ifacecfg((ATCAIfaceCfg *) &atca_params[0]);

    /* Set device to ATECC508A */
    if (ATCA_DEVTYPE == ATECC608A) {
        atca_run_cmd((char *) atecc608, sizeof(atecc608));
    }
    else if (ATCA_DEVTYPE == ATECC508A) {
        atca_run_cmd((char *) atecc508, sizeof(atecc508));
    }
    else {
        printf("This device is currently not supported.");
        return 0;
    }

    atca_run_cmd((char *) unit, sizeof(unit));

    return 0;
}
