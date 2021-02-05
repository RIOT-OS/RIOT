/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_neterr Error reporting
 * @ingroup     net_gnrc
 * @brief       Allows for asynchronous error reporting in the network stack.
 * @{
 *
 * @file
 * @brief   Error reporting definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETERR_H
#define NET_GNRC_NETERR_H

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Error code to signalise success (no error occurred) to an gnrc_neterr subscriber.
 */
#define GNRC_NETERR_SUCCESS         (0)

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETERR_H */
/** @} */
