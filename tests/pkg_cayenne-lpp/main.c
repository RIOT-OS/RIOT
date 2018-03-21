/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       Cayenne Low Power Payload example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "cayenne_lpp.h"

static cayenne_lpp_t lpp;

static void _print_buffer(cayenne_lpp_t *lpp)
{
    for (uint8_t i = 0; i < lpp->cursor; ++i) {
        printf("%02X", lpp->buffer[i]);
    }
    puts("");
}

int main(void)
{
    puts("Cayenne LPP test application");

    /* generate payload like the one given as example at
     * https://mydevices.com/cayenne/docs_stage/lora/#lora-cayenne-low-power-payload
     */
    /* Device with 2 temperature sensors */
    cayenne_lpp_add_temperature(&lpp, 3, 27.2);
    cayenne_lpp_add_temperature(&lpp, 5, 25.5);
    _print_buffer(&lpp);

    /* Device with temperature and acceleration sensors */
    cayenne_lpp_reset(&lpp);
    cayenne_lpp_add_temperature(&lpp, 1, -4.1);
    cayenne_lpp_add_accelerometer(&lpp, 6, 1.234, -1.234, 0);
    _print_buffer(&lpp);

    /* Device with GPS */
    cayenne_lpp_reset(&lpp);
    cayenne_lpp_add_gps(&lpp, 1, 42.3519, -87.9094, 10);
    _print_buffer(&lpp);

    return 0;
}
