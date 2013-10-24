/**
 * internal nativenet ZEP network layer interface
 *
 * Copyright (C) 2013 Ludwig Ortmann
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#ifndef _ZEP_H
#define _ZEP_H

#include <inttypes.h>
#include <net/ethernet.h>

#include "radio/types.h"

/**
 * create and/or open zep device
 */
int zep_init();
int send_buf(radio_packet_t *packet);

#include <stdbool.h>
#include <time.h>

#define ZEP_DEFAULT_PORT   "17754"

/*  ZEP Preamble Code */
#define ZEP_PREAMBLE        "EX"

/*  ZEP Header lengths. */
#define ZEP_V1_HEADER_LEN   16
#define ZEP_V2_HEADER_LEN   32
#define ZEP_V2_ACK_LEN      8

#define ZEP_V2_TYPE_DATA    1
#define ZEP_V2_TYPE_ACK     2

#define ZEP_LENGTH_MASK     0x7F

/*
 *      ZEP v1 Header will have the following format:
 *      |Preamble|Version|Channel ID|Device ID|CRC/LQI Mode|LQI Val|Reserved|Length|
 *      |2 bytes |1 byte |  1 byte  | 2 bytes |   1 byte   |1 byte |7 bytes |1 byte|
 */

struct zep_header {
    char        preamble[2];
    uint8_t     version;
    uint8_t     channel;
    uint16_t    device;
    uint8_t     lqi_mode;
    uint8_t     lqi;
    uint8_t     reserved[7];
    uint8_t     length;
}  __attribute__((packed));

union zep_frame {
    struct {
        struct zep_header header;
        unsigned char payload[ZEP_LENGTH_MASK];
    } field;
    unsigned char buffer[ZEP_LENGTH_MASK + sizeof(struct zep_header)];
} __attribute__((packed));

extern int _native_zep_fd;

#endif /* _ZEP_H */
