/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_hdlc
 * @ingroup     net
 * @brief       This is a generic low-level hdlc parser, according to ISO13239
 * @{
 *
 * @file
 * @brief
 *
 * @author      Max van Kessel
 */

#ifndef NET_HDLC_H
#define NET_HDLC_H

#include <stdint.h>

#include "hdlc/hdr.h"
#include "net/netdev.h"
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default HDLC buffer size
 */
#ifndef HDLC_BUFSIZE
#define HDLC_BUFSIZE (128)
#endif

/**
 * @brief Default timeout for retransmission of sequence flag
 */
#ifndef HDLC_MAX_IDLE_TIME_MS
#define HDLC_MAX_IDLE_TIME_MS   (100 * US_PER_MS)
#endif

/**
 * @brief Sequence flag value
 */
#define HDLC_FLAG_SEQUENCE      (0x7E)

/**
* @brief Control escape value
*/
#define HDLC_CONTROL_ESCAPE     (0x7D)

/**
* @brief HDLC Sixth complement
*/
#define HDLC_SIX_CMPL           (0x20)

/**
* @brief All stations address value
*/
#define HDLC_ALL_STATION_ADDR   (0xFF)

/**
* @brief HDLC states
*/
enum {
    HDLC_IDLE = 0,/**< HDLC_IDLE */
    HDLC_START,   /**< HDLC_START */
    HDLC_ADDRESS, /**< HDLC_ADDRESS */
    HDLC_CONTROL, /**< HDLC_CONTROL */
    HDLC_DATA,    /**< HDLC_DATA */
};

/**
 * @brief   Device descriptor for HDLC
 *
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                /**< parent class */
    tsrb_t inbuf;                   /**< RX buffer */
    uint8_t rxmem[HDLC_BUFSIZE];    /**< memory used by RX buffer */
    uint8_t txmem[HDLC_BUFSIZE];    /**< memory used by TX buffer */

    uint16_t fcs;                   /**< checksum variable */
    uint8_t esc;                    /**< escaping next character */
    uint8_t state;                  /**< parser state */
} hdlc_t;

/**
 * @brief Setup HDLC layer
 *
 * @param[in] dev   HDLC device
 */
void hdlc_setup(hdlc_t *dev);

/**
 * @brief Prints HDLC header
 *
 * @param[in] hdr header to print
 */
void hdlc_hdr_print(hdlc_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* NET_HDLC_H */
/**
 * @}
 */
