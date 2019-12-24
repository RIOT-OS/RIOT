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

#include "fmt.h"
#include "minmea.h"

static const char *_gll = "$GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45";

int main(void)
{
    print_str("START\n");
    struct minmea_sentence_gll frame;

    int res = minmea_parse_gll(&frame, _gll);
    if (!res) {
        print_str("FAILURE: error parsing GPS sentence\n");
    }
    else {
        print_str("parsed coordinates: lat=");
        print_float(minmea_tocoord(&frame.latitude), 6);
        print_str(" lon=");
        print_float(minmea_tocoord(&frame.longitude), 6);
        print_str("\nSUCCESS\n");
    }

    return 0;
}
