/*
 * Copyright (C) 2022 Freie Universität Berlin
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
 * @author      Nils Ollrogge <nils.ollrogge@mailbox.tu-dresden.de>
 * @}
 */
#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "ztimer.h"

#include "fido2/ctap/transport/ctap_transport.h"

int main(void)
{
    /* sleep in order to see early DEBUG outputs */
    ztimer_sleep(ZTIMER_MSEC, 3000);
    fido2_ctap_transport_init();
}
