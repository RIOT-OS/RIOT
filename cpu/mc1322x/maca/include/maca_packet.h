/*
 * maca_packet.h - defines a rxtx packet for the MACA driver
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* does not include 2 byte FCS checksum */
#ifndef MACA_MAX_PAYLOAD_SIZE
#define MACA_MAX_PAYLOAD_SIZE 125
#endif

#define PACKET_STATS 0

/**
 * @brief MC1322x internal MACA packet format */
struct packet {
    uint8_t length;                 /**< does not include FCS checksum */
    volatile struct packet *left;   /**< pointer to left neighbor in queue */
    volatile struct packet *right;  /**< pointer to right neighbor in queue */
    uint8_t offset;                 /**< offset into data for first byte of the
                                      *  packet payload
                                      *  On TX this should be 0
                                      *  On RX this should be 1 since the maca
                                      *  puts the length as the first byte
                                      */
    uint8_t lqi;                    /**< link quality indicator */
    uint8_t status;                 /**< packet status */
    uint32_t rx_time;               /**< receiving timestamp */
#if PACKET_STATS
    uint8_t seen;
    uint8_t post_tx;
    uint8_t get_free;
    uint8_t rxd;
#endif
    uint8_t data[MACA_MAX_PAYLOAD_SIZE+2+1]; /**< +2 for FCS; + 1 since maca
                                               *  returns the length as the
                                               *  first byte */
};
typedef struct packet maca_packet_t;

#ifdef __cplusplus
}
#endif

#endif
