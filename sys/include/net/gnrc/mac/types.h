/*
 * Copyright (C) 2015 Daniel Krebs
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_mac  A common MAC type for providing key MAC parameters and helper functions
 * @ingroup     net
 * @brief       A common MAC type for providing key MAC parameters and helper functions.
 * @{
 *
 * @file
 * @brief       Internal types used by the GNRC_MAC entities
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef GNRC_MAC_TYPES_H_
#define GNRC_MAC_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <kernel_types.h>
#include <xtimer.h>
#include <net/netdev2.h>
#include <net/gnrc/netdev2.h>
#include <net/gnrc.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief definition for device transmission feedback types
 */
typedef enum {
    TX_FEEDBACK_UNDEF = 0,    /* Transmission just start, no Tx feedback yet */
    TX_FEEDBACK_SUCCESS,      /* Transmission succeeded */
    TX_FEEDBACK_NOACK,        /* No ACK for the transmitted packet */
    TX_FEEDBACK_BUSY          /* found medium busy when doing transmission */
} gnrc_mac_tx_feedback_t;

#ifdef __cplusplus
}
#endif

#endif /* GNRC_MAC_TYPES_H_ */
/** @} */
