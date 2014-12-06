/**
 * Generic radio driver interface
 *
 * Copyright (C) 2008-2009  Freie Universitaet Berlin (FUB).
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#ifndef RADIO_H_
#define RADIO_H_

/**
 * @defgroup    net_datalink    Data link layer
 * @ingroup     net
 *
 * @brief   Defines interface of data link layers for use with micro mesh stack.
 *
 * @{
 */

/**
 * @file
 * @brief
 *
 * @author      baar
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "radio/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define L1_PROTOCOL_CATCH_ALL             (0xff)    /**< Catch all protocol ID */

/**
 * @brief Link layer protocols (sic!) for proprietary cc110x protocol stack
 */
enum layer_1_protocols {
    LAYER_1_PROTOCOL_LL_ACK     = 1,    /**< Link-Level Acknowledgement (LL-ACK) */
    LAYER_1_PROTOCOL_MM         = 2,    /**< Micro Mesh network packet (MM) */
};

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* RADIO_H_ */
