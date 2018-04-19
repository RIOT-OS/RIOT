/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_simple CoRE RD Simple Client
 * @ingroup     net
 * @brief       CoAP-based CoRE Resource Directory client supporting the simple
 *              registration only
 * @{
 *
 * @file
 * @brief       Interface for a simple CoRE RD registration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_RDCLI_SIMPLE_H
#define NET_RDCLI_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Error codes used by the rdcli_simple implementation
 */
enum {
    RDCLI_SIMPLE_OK     =  0,   /**< all good */
    RDCLI_SIMPLE_NOADDR = -1,   /**< on address conversion errors */
    RDCLI_SIMPLE_ERROR  = -2,   /**< on other errors */
};

/**
 * @brief   Initiate the node registration by sending an empty CoAP POST message
 *          to the RD server's /.well-known/core resource
 *
 * @return  RDCLI_SIMPLE_OK on success
 * @return  RDCLI_SIMPLE_NOADDR if conversion of RD address fails
 * @return  RDCLI_SIMPLE_ERROR if something goes wrong preparing or sending the
 *          initial request
 */
int rdcli_simple_register(void);

/**
 * @brief   Spawn a new thread that registers the node and updates the
 *          registration with all responding RDs using the simple registration
 *          process
 */
void rdcli_simple_run(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_SIMPLE_H */
/** @} */
