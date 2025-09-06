/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     tests
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @}
 */

#include <stdbool.h>
#include <stdint.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes networking for tests
 */
void _net_init(void);

/**
 * @brief   Injects a received UDP packet into the stack
 *
 * @param[in] src       The source address of the UDP packet
 * @param[in] dst       The destination address of the UDP packet
 * @param[in] src_port  The source port of the UDP packet
 * @param[in] dst_port  The destination port of the UDP packet
 * @param[in] data      The payload of the UDP packet
 * @param[in] data_len  The payload length of the UDP packet
 * @param[in] netif     The interface the packet came over, currently ignored
 *
 * @return  true, if packet was successfully injected
 * @return  false, if an error occurred during injection
 */
bool _inject_packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                    uint16_t src_port, uint16_t dst_port,
                    void *data, size_t data_len, uint16_t netif);

/**
 * @brief   Checks networking state (e.g. packet buffer state)
 *
 * @return  true, if networking component is still in valid state
 * @return  false, if networking component is in an invalid state
 */
bool _check_net(void);

/**
 * @brief   Checks if a UDP packet was sent by the networking component
 *
 * @param[in] src               Expected source address of the UDP packet
 * @param[in] dst               Expected destination address of the UDP packet
 * @param[in] src_port          Expected source port of the UDP packet
 * @param[in] dst_port          Expected destination port of the UDP packet
 * @param[in] data              Expected payload of the UDP packet
 * @param[in] data_len          Expected payload length of the UDP packet
 * @param[in] netif             Expected interface the packet is supposed to
 *                              be send over
 * @param[in] random_src_port   Do not check source port, it might be random
 *
 * @return  true, if all parameters match as expected
 * @return  false, if not.
 */
bool _check_packet(const ipv6_addr_t *src, const ipv6_addr_t *dst,
                   uint16_t src_port, uint16_t dst_port,
                   void *data, size_t data_len, uint16_t netif,
                   bool random_src_port);

#ifdef __cplusplus
}
#endif
