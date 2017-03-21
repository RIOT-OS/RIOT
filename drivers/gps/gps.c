/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>

#include "minmea.h"
#include "gps.h"

#ifndef GPS_USE_FMT
#define GPS_USE_FMT 1
#endif

#if GPS_USE_FMT
#include "fmt.h"
#else
#include <stdio.h>
#endif

int gps_parse_gll(const uint8_t *buf, char *lat, char *lon)
{
    struct minmea_sentence_gll frame;

    if (minmea_parse_gll(&frame, (const char*)buf)) {
        if (frame.status != 'A') {
            return -1;
        }
        if (lat) {
            if (GPS_USE_FMT) {
                size_t res = fmt_float(lat, minmea_tocoord(&frame.latitude), 5);
                lat[res] = '\0';
            }
            else {
                sprintf(lat, "%f", minmea_tocoord(&frame.latitude));
            }
        }
        if (lon) {
            if (GPS_USE_FMT) {
                size_t res = fmt_float(lon, minmea_tocoord(&frame.longitude), 5);
                lon[res] = '\0';
            }
            else {
                sprintf(lon, "%f", minmea_tocoord(&frame.longitude));
            }
        }
        return 0;
    }

    return -1;
}
