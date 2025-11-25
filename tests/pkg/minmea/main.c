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
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "fmt.h"
#include "minmea.h"

static const char *_gga = "$GNGGA,142142.227,5227.022,N,01317.919,E,1,12,1.0,0.0,M,0.0,M,,*70";
static const char *_gll = "$GNGLL,5229.0178,N,01326.7605,E,114350.000,A,A*45";
static const char *_gsa = "$GNGSA,A,3,01,02,03,04,05,06,07,08,09,10,11,12,1.0,1.0,1.0*2E";
static const char *_rmc = "$GPRMC,142142.227,A,5227.022,N,01317.919,E,,,201023,000.0,W*78";

int _parse_nmea_msg(const char* nmea_msg)
{
    /* get message type */
    int msg_type = minmea_sentence_id(nmea_msg, false);
    switch (msg_type) {
    case MINMEA_SENTENCE_GGA: {
        struct minmea_sentence_gga frame;
        if (minmea_parse_gga(&frame, nmea_msg) && frame.fix_quality) {
            puts("check_nmea: GGA recorded");

            /* print values */
            print_str("\tlat: ");
            print_float(minmea_tocoord(&frame.latitude), 6);
            printf("\n");
            print_str("\tlon: ");
            print_float(minmea_tocoord(&frame.longitude), 6);
            printf("\n");
            printf("\talt: %"PRId32"\n", minmea_rescale(&frame.altitude, 1));
            printf("\tn_sats: %d\n", frame.satellites_tracked);
        }
        else {
            puts("check_nmea: invalid GGA record");
        }
    } break;
    case MINMEA_SENTENCE_GLL: {
        struct minmea_sentence_gll frame;
        if (minmea_parse_gll(&frame, nmea_msg)) {
            puts("check_nmea: GLL recorded");

            /* print values */
            print_str("\tlat: ");
            print_float(minmea_tocoord(&frame.latitude), 6);
            printf("\n");
            print_str("\tlon: ");
            print_float(minmea_tocoord(&frame.longitude), 6);
            printf("\n");
        }
        else {
            puts("check_nmea: invalid GLL record");
        }
    } break;
    case MINMEA_SENTENCE_GSA: {
        struct minmea_sentence_gsa frame;
        if (minmea_parse_gsa(&frame, nmea_msg)) {
            puts("check_nmea: GSA recorded");

            /* print values */
            printf("\tmode: %c\n", frame.mode); /* (A)automatic and (M)manual mode */
            /* identify fix-type */
            switch (frame.fix_type) {
            case MINMEA_GPGSA_FIX_NONE:
                printf("\tfix_type: %s\n", "NONE");
                break;
            case MINMEA_GPGSA_FIX_2D:
                printf("\tfix_type: %s\n", "2D");
                break;
            case MINMEA_GPGSA_FIX_3D:
                printf("\tfix_type: %s\n", "3D");
                break;
            default:
                printf("\tfix_type: %s\n", "UNKNOWN");
            }
            /* always a record of 12 satellites */
            for (int i = 0; i < 12; i++) {
                printf("\t%d. sat: %d\n", i, frame.sats[i]);
            }
        }
        else {
            puts("check_nmea: invalid GSA record");
        }
    } break;
    case MINMEA_SENTENCE_RMC: {
        struct minmea_sentence_rmc frame;
        if (minmea_parse_rmc(&frame, nmea_msg) && frame.valid) {
            puts("check_nmea: RMC recorded");

            /* print values */
            print_str("\tlat: ");
            print_float(minmea_tocoord(&frame.latitude), 6);
            printf("\n");
            print_str("\tlon: ");
            print_float(minmea_tocoord(&frame.longitude), 6);
            printf("\n");
            /* NMEA RMC is providing UTC time */
            printf("\tdate: %d.%d.%d\n", frame.date.day, frame.date.month, frame.date.year);
            printf("\ttime: %dh %dm %ds, %dms\n", frame.time.hours, frame.time.minutes,
                                            frame.time.seconds, frame.time.microseconds/1000);
        }
        else {
            puts("check_nmea: invalid RMC record");
        }
    } break;
    default:
        printf("check_nmea: unknown message type %d\n", msg_type);
        return -1;
    }

    return 0;
}

int main(void)
{
    puts("START");

    /* check GGA */
    if (_parse_nmea_msg(_gga) != 0) {
        puts("Error parsing GGA");
    }
    /* check GLL */
    if (_parse_nmea_msg(_gll) != 0) {
        puts("Error parsing GLL");
    }
    /* check GSA */
    if (_parse_nmea_msg(_gsa) != 0) {
        puts("Error parsing GSA");
    }
    /* check RMC */
    if (_parse_nmea_msg(_rmc) != 0) {
        puts("Error parsing RMC");
    }

    puts("SUCCESS");

    return 0;
}
