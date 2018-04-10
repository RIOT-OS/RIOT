/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli CoRE RD Endpoint Library
 * @ingroup     net
 * @brief       Library for using RIOT as CoRE Resource Directory endpoint
 *
 * This module implements a CoRE Resource Directory endpoint library, that
 * allows RIOT nodes to register themselves with resource directories.
 * It implements the standard endpoint functionality as defined in
 * draft-ietf-core-resource-directory-15.
 * @see https://tools.ietf.org/html/draft-ietf-core-resource-directory-15
 *
 * @note        As the name of this library (`rdcli`) can be misleading in
 *              context of the RD draft (endpoint vs client), this library
 *              will most likely undergo a name change in the near future...
 *
 * # Design Decisions
 * - all operations provided by this module are fully synchronous, meaning that
 *   the functions will block until an operation is successful or will time out
 * - the implementation limits the client to be registered with a single RD at
 *   any point in time
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_RDCLI_H
#define NET_RDCLI_H

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Return values and error codes used by this module
 */
enum {
    RDCLI_OK        =  0,   /**< everything went as expected */
    RDCLI_TIMEOUT   = -1,   /**< no response from the network */
    RDCLI_ERR       = -2,   /**< internal error or invalid reply */
    RDCLI_NORD      = -3,   /**< not connected to an RD */
    RDCLI_OVERFLOW  = -4,   /**< internal buffers can not handle input */
};

/**
 * @brief   Discover the registration interface resource of a RD
 *
 * @param[in] remote    remote endpoint of the target RD
 * @param[out] regif    the registration interface is written to this buffer
 * @param[in] maxlen    size of @p regif
 *
 * @return  RDCLI_OK on success
 * @return  RDCLI_TIMEOUT if the discovery request times out
 * @return  RDCLI_NORD if addressed endpoint is not a RD
 * @return  RDCLI_ERR on any other internal error
 */
int rdcli_discover_regif(const sock_udp_ep_t *remote,
                         char *regif, size_t maxlen);

/**
 * @brief   Initiate the node registration by sending an empty push
 *
 * - if registration fails (e.g. timeout), we are not associated with any RD
 *   anymore (even if we have been before we called rdcli_register)
 *
 * @note    In case a multicast address is given, the @p regif parameter MUST be
 *          NULL. The first RD responding to the request will be chosen and all
 *          replies from other RD servers are ignored.
 *
 * @param[in] remote    remote endpoint of the target RD
 * @param[in] regif     registration interface resource of the RD, it will be
 *                      discovered automatically when set to NULL
 *
 * @return  RDCLI_OK on success
 * @return  RDCLI_TIMEOUT on registration timeout
 * @return  RDCLI_NORD if addressed endpoint is not a RD
 * @return  RDCLI_OVERFLOW if @p regif does not fit into internal buffer
 * @return  RDCLI_ERR on any other internal error
 */
int rdcli_register(const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Update our current entry at the RD
 *
 * @return  RDCLI_OK on success
 * @return  RDCLI_TIMEOUT if the update request times out
 * @return  RDCLI_ERR on any other internal error
 */
int rdcli_update(void);

/**
 * @brief   Unregister from a given RD server
 *
 * @return  RDCLI_OK on success
 * @return  RDCLI_TIMEOUT if the remove request times out
 * @return  RDCLI_ERR on any other internal error
 */
int rdcli_remove(void);

/**
 * @brief   Dump the current RD connection status to STDIO (for debugging)
 */
void rdcli_dump_status(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_H */
/** @} */
