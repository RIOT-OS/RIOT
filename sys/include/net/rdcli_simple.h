/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_simple Simple CoRE Resource Directory Registration
 * @ingroup     net_rdcli
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
 * @brief   Initiate the node registration by sending an empty CoAP POST message
 *          to the RD server's /.well-known/core resource
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
