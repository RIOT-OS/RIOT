/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 * @file
 * @brief       FIDO2 CTAP test application that creates an authenticator
 *              which uses CTAPHID as underlying communication protocol
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "xtimer.h"

#include "fido2/ctap.h"
#include "fido2/ctap/transport/ctap_transport.h"

int main(void)
{
    /* sleep in order to see early DEBUG outputs */
    xtimer_sleep(3);
    fido2_ctap_transport_init();
}
