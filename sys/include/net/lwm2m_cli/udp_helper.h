/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_lwm2m_cli
 * @{
 *
 * @file
 * @brief       UDP helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LWM2M_CLI_UDP_HELPER_H
#define LWM2M_CLI_UDP_HELPER_H

#include <stddef.h>
#include <stdint.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Send the given data via UDP to the given address
 *
 * @param[in] addr  destination address
 * @param[in] port  destination port
 * @param[in] data  data to send
 * @param[in] len   number of bytes to send
 *
 * @return          number of bytes send
 * @return          0 on error
 */
size_t udp_helper_send(ipv6_addr_t *addr, uint16_t port,
                       uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLI_UDP_HELPER_H */
/** @} */
