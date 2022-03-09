/*
 * Copyright (C) 2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF9160DK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

/* Only include content of this file if the nrf_modem pkg is used */
#include "board.h"
#include "nrf_modem_at.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const char *const ar_commands_default[] = {
    "AT\%XSYSTEMMODE=0,0,1,0",      /* Enable GNSS service */
    "AT\%XCOEX0=1,1,1565,1586",     /* Set Coexistence parameters */
    "AT+CPSMS=0",                   /* Disable power saving */
    "AT+CFUN=31",                   /* Enable only GNSS */
};

int _setup_modem(void)
{
    int ret = 0;

    for (unsigned int i = 0; i < ARRAY_SIZE(ar_commands_default); i++) {
        if (ar_commands_default[i][0] == '\0') {
            continue;
        }

        ret = nrf_modem_at_printf("%s", ar_commands_default[i]);
        if (ret != 0) {
            DEBUG("AT command:%u failed with error%d\n", i, ret);
            return -1;
        }
    }

    return 0;
}

int nrf_modem_setup(void)
{
    /* Setup modem with default parameters */
    if (_setup_modem() != 0) {
        DEBUG_PUTS("setup board params for modem failed");
        return -1;
    } else {
        DEBUG_PUTS ("board: setup modem done");
    }
    return 0;
}
