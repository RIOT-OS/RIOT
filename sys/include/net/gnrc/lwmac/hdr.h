/*
 * Copyright (C) 2015 Daniel Krebs
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lwmac Simplest possible MAC layer
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       Header definition LWMAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 */

#ifndef GNRC_LWMAC_HDR_H_
#define GNRC_LWMAC_HDR_H_

#include <stdint.h>
#include <stdbool.h>
#include <net/gnrc/lwmac/lwmac.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

typedef struct {
    uint8_t  addr[LWMAC_MAX_L2_ADDR_LEN];// __attribute__ ((aligned (LWMAC_MAX_L2_ADDR_LEN)));
    uint8_t  len;
} l2_addr_t;
#define LWMAC_L2_ADDR_INIT      { {0}, 0 }

/******************************************************************************/

typedef enum {
    FRAMETYPE_WR = 1,
    FRAMETYPE_WA,
    FRAMETYPE_DATA,
    FRAMETYPE_BROADCAST,
} lwmac_frame_type_t;

/******************************************************************************/

/**
 * @brief   lwMAC header
 */
typedef struct __attribute__((packed)) {
    lwmac_frame_type_t type; /**< type of frame */
} lwmac_hdr_t;

/**
 * @brief   lwMAC WR frame
 */
typedef struct __attribute__((packed)) {
    lwmac_hdr_t header;
    l2_addr_t dst_addr; /**< WR is broadcast, so destination address needed */
} lwmac_frame_wr_t;

/**
 * @brief   lwMAC WA frame
 */
typedef struct __attribute__((packed)) {
    lwmac_hdr_t header;
    l2_addr_t dst_addr; /**< WA is broadcast, so destination address needed */
    uint32_t current_phase;
} lwmac_frame_wa_t;

/**
 * @brief   lwMAC broadcast data frame
 */
typedef struct __attribute__((packed)) {
    lwmac_hdr_t header;
    uint8_t seq_nr;
} lwmac_frame_broadcast_t;

/**
 * @brief   lwMAC unicast data frame
 */
typedef struct __attribute__((packed)) {
    lwmac_hdr_t header;
} lwmac_frame_data_t;


void lwmac_print_hdr(lwmac_hdr_t* hdr);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_LWMAC_HDR_H_ */
/** @} */
