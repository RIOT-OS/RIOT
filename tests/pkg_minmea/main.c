/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       minmea GPS NMEA parser library package test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "minmea.h"

static const char *_gll = "$GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45";

int main(void)
{
    puts("START");
    struct minmea_sentence_gll frame;

    int res = minmea_parse_gll(&frame, _gll);
    if (!res) {
        puts("FAILURE: error parsing GPS sentence");
    }
    else {
        printf("parsed coordinates: lat=%f lon=%f\n",
                minmea_tocoord(&frame.latitude),
                minmea_tocoord(&frame.longitude));
        puts("SUCCESS");
    }

    return 0;
}
