/**
 * internal nativenet tap network layer interface
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */
#ifndef _TAP_H
#define _TAP_H

#include <net/ethernet.h>

#include "board.h"
#include "radio/types.h"

/**
 * create and/or open tap device "name"
 *
 * if "name" is an empty string, the kernel chooses a name
 * if "name" is an existing device, that device is used
 * otherwise a device named "name" is created
 *
 * On OSX a name has to be provided.
 */
int tap_init(char *name);

extern int _native_tap_fd;
extern unsigned char _native_tap_mac[ETHER_ADDR_LEN];

struct nativenet_header {
    radio_packet_length_t length;
    radio_address_t dst;
    radio_address_t src;
} __attribute__((packed));
#define TAP_MAX_DATA ((ETHERMTU) - 6) /* XXX: this is suboptimal */

struct nativenet_packet {
    struct nativenet_header nn_header;
    uint8_t data[ETHERMTU - sizeof(struct nativenet_header)];
} __attribute__((packed));

union eth_frame {
    struct {
        struct ether_header header;
        struct nativenet_packet payload;
    } field;
    unsigned char buffer[ETHER_MAX_LEN];
} __attribute__((packed));

#endif /* _TAP_H */
