/*
 * Copyright (C) 2019 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fuzzing FUZZING utilities
 * @ingroup     sys
 *
 * @brief       Various utilities for fuzzing network applications.
 *
 * @{
 * @file
 *
 * @author      Sören Tempel <tempel@uni-bremen.de>
 */

#ifndef FUZZING_H
#define FUZZING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "net/gnrc/pkt.h"

/**
 * @brief Initialize dummy network interface with given address.
 *
 * @param addr IPv6 address to use for interface, can be NULL.
 * @param pfx_len The prefix length of @p addr, ignored if @p addr is NULL.
 *
 * @return 0 on success, non-zero otherwise.
 */
int fuzzing_init(ipv6_addr_t *addr, unsigned pfx_len);

/**
 * @brief Read a network packet from the given file descriptor.
 *
 * @param fd File descriptor to read packet from.
 * @param pkt Allocated packet structure to write packet to,
 *            will be resized accordingly.
 *
 * @return 0 on success, non-zero otherwise.
 */
int fuzzing_read_packet(int fd, gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FUZZING_H */
