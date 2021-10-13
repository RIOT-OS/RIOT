/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_cord_epsim CoRE RD Simple Registration Endpoint
 * @ingroup     net_cord
 * @brief       CoRE Resource Directory endpoint using the simple registration
 *              procedure
 *
 * This module is designed to provide nodes with the possibility to register
 * with resource directories without having to allocate a lot of resources. All
 * the user has to do, is to call the cord_epsim_register() function in periodic
 * intervals, depending on the value of the `CONFIG_CORD_LT` variable.
 *
 * @{
 *
 * @file
 * @brief       Interface for the CoRE RD simple registration endpoint
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_CORD_EPSIM_H
#define NET_CORD_EPSIM_H

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Error codes used by the cord_epsim implementation
 */
enum {
    CORD_EPSIM_OK     =  0,     /**< all good */
    CORD_EPSIM_ERROR  = -1,     /**< on failing to send POST request */
    CORD_EPSIM_BUSY   = -2,     /**< endpoint registration is in progress */
};

/**
 * @brief   Initiate the node registration by sending an empty CoAP POST message
 *          to the RD server's /.well-known/core resource
 *
 * @pre     remote != NULL
 *
 * @param[in] remote    address and port of the target resource directory
 *
 * @return  CORD_EPSIM_OK on success
 * @return  CORD_EPSIM_BUSY if registration is already in progress
 * @return  CORD_EPSIM_ERROR if something goes wrong preparing or sending the
 *          registration request
 */
int cord_epsim_register(const sock_udp_ep_t *remote);

/**
 * @brief   Get the status of the latest registration procedure
 *
 * @return  CORD_EPSIM_OK if last registration was triggered successfully
 * @return  CORD_EPSIM_BUSY if a registration is currently in progress
 * @return  CORD_EPSIM_ERROR if latest registration attempt failed
 */
int cord_epsim_state(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_EPSIM_H */
/** @} */
