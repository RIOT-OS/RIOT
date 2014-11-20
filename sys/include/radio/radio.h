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
 * @version     $Revision: 1961 $
 *
 * @note        $Id$
 */

#include <stdint.h>
#include <stdbool.h>
#include "radio/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define L1_PROTOCOL_CATCH_ALL             (0xff)    ///< Catch all protocol ID

enum layer_1_protocols {
    LAYER_1_PROTOCOL_LL_ACK     = 1,    ///< Link-Level Acknowledgement (LL-ACK)
    LAYER_1_PROTOCOL_MM         = 2,    ///< Micro Mesh network packet (MM)
};

/**
 * Radio/MAC API.
 */
typedef struct {
    const char  *name;
    const radio_address_t broadcast_address;
    const uint8_t output_power_max;
    /**
     * @return  the average transmission duration of one packet
     *          in milliseconds, e.g. till ACK received.
     */
    int (*get_avg_transmission_duration)(void);
    radio_address_t (*get_address)(void);
    bool (*set_address)(radio_address_t address);
    bool (*set_output_power)(uint8_t pa_idx);
    bool (*set_packet_monitor)(packet_monitor_t monitor);
    /**
     * @return  -1 if an error occurs (e.g. handler table full) else >= 0.
     */
    int (*set_packet_handler)(protocol_t protocol, packet_handler_t handler);
    /**
     * @return  A negative value if operation failed; else the number of transmitted bytes.
     */
    int (*send)(radio_address_t address, protocol_t protocol, int priority, char *payload, int payload_len);
    void (*print_stats)(void);
    void (*print_config)(void);
} radio_t;

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* RADIO_H_ */
