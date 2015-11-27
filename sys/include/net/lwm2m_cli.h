/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_lwm2m_cli LWM2M Client Adapter
 * @ingroup     net
 * @brief       RIOT specific LWM2M client adaption
 *
 * @{
 * @file
 * @brief       Interface to RIOT's LWM2M client adaption layer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandru Razvan Caciulescu <alex.darredevil@gmail.com>
 */

#ifndef LWM2M_CLI_H
#define LWM2M_CLI_H

#include <stdint.h>

#include "liblwm2m.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the node as LWM2M client
 *
 * @param[in] name      human readable name of the client
 * @param[in] addr      address of the server
 * @param[in] port      server port to connect to
 *
 * @return      0 on success
 * @return      -1 on error
 */
int lwm2m_cli_init(char *name, ipv6_addr_t *addr, uint16_t port);

/**
 * @brief   Change a resource in the client's object store
 *
 * @param[in] uri       URI of the resource
 * @param[in] val       value to set
 * @param[in] len       length of the value
 *
 * @return      0 on success
 * @return      -1 on error
 */
int lwm2m_cli_res_change(lwm2m_uri_t *uri, uint8_t *val, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLI_H */
/** @} */
